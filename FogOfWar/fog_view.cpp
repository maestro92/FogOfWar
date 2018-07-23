#include "fog_view.h"



void FogView::init(World* world, Map* map, FogManager* fogManager)
{
	m_fogManager = fogManager;

	glm::ivec2 minGc = glm::ivec2(0, 0);
	glm::ivec2 maxGc = glm::ivec2(map->getWidth() - 1, map->getHeight() - 1);

	glm::vec2 minSimPos = map->getCellMinCorner(minGc);
	glm::vec2 maxSimPos = map->getCellMaxCorner(maxGc);

	glm::vec3 minWorldPos = world->simPos2WorldPos(minSimPos);
	glm::vec3 maxWorldPos = world->simPos2WorldPos(maxSimPos);

	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	ModelManager::buildQuad3D(minWorldPos, maxWorldPos, COLOR_GREEN, vertices, indices);

	Mesh m(vertices, indices);

	FOWModel = new Model();
	FOWModel->addMesh(m);
	glm::vec3 pos = FOWGameObject.getPosition();

	FOWGameObject.setPosition(pos);
	FOWGameObject.setModel(FOWModel);

	m_textureWidth = map->getWidth();
	m_textureHeight = map->getHeight();

	cout << "texture width " << m_textureWidth << endl;
	cout << "texture height " << m_textureHeight << endl;

	m_fogTexture = utl::createNewTexture(m_textureWidth, m_textureHeight, GL_NEAREST, GL_CLAMP_TO_BORDER);
	clearTexture();

	initBlurPasses();
	initFadeUpdateStuff();

}



void FogView::initBlurPasses()
{
	blurPassFBO1 = utl::createFrameBufferObject(m_textureWidth, m_textureHeight, GL_LINEAR, GL_CLAMP_TO_BORDER);
	blurPassFBO2 = utl::createFrameBufferObject(m_textureWidth, m_textureHeight, GL_LINEAR, GL_CLAMP_TO_BORDER);
}


void FogView::initFadeUpdateStuff()
{
	m_fogFadeUpdateFBO = utl::createFrameBufferObject(m_textureWidth, m_textureHeight, GL_LINEAR, GL_CLAMP_TO_BORDER);
	m_fogFadeTexture = m_fogFadeUpdateFBO.colorTexture;

	m_fogFadeUpdatePipeline.loadIdentity();
	m_fogFadeUpdatePipeline.ortho(-1, 1, -1, 1, -1, 1);

	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	ModelManager::buildQuad2D(glm::vec2(-1,-1), glm::vec2(1,1), COLOR_GREEN, vertices, indices);

	Mesh m(vertices, indices);
	m_fadeUpdateQuad = new Model();
	m_fadeUpdateQuad->addMesh(m);
	o_updateQuadGameObject.setModel(m_fadeUpdateQuad);
}

/*


we update the RGB on our fogFadeTexture in the fade update
then in the rendering, we use the RGB as the alpha value we will render with

	we use the glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	fogFadeTexture reads from fogTexture, and do the alpha blending update. Assume our rate is 0.3

	Frame1	FogTexture	RGB (0,0,0)				FogFadeTexture	RGB(0,0,0,1)						
			Render (color, 0)
			
	Frame2	FogTexture	RGB(1,1,1)				FogFadeTexture	RGB(0,0,0,1)	

			src = (0.3, 0.3, 0.3, 0.3)			dst = (0,0,0,0)

			Final color = (0.3, 0.3, 0.3, 0.3) * 1 + (0, 0, 0, 1) * 0.7	 
						= (0.3, 0.3, 0.3, 1)
			Render (color, 0.3)

	Frame3	FogTexture	RGB(1,1,1)				FogFadeTexture	RGB(0.3, 0.3, 0.3, 1)	

			src = (0.3, 0.3, 0.3, 0.3)			dst = (0.3, 0.3, 0.3, 1)

			Final color = (0.3, 0.3, 0.3, 0.3) * 1 + (0.3, 0.3, 0.3, 1) * 0.7
						= (0.51, 0.51, 0.51, 1) 				  
			Render	(color, 0.51)

	Frame3	FogTexture	RGB(1,1,1)				FogFadeTexture	RGB(0.51, 0.51, 0.51, 1)

			src = (0.3, 0.3, 0.3, 0.3)			dst = (0.3, 0.3, 0.3, 1)

			Final color = (0.3, 0.3, 0.3, 0.3) * 1 + (0.51, 0.51, 0.51, 1) * 0.7
						= (0.657, 0.657, 0.657, 1)
			Render	(color, 0.657)


	Frame4	FogTexture	RGB(0,0,0)				FogFadeTexture	RGB(0.657, 0.657, 0.657, 1)	...
	
			src = (0.3, 0.3, 0.3, 0.3)			dst = (0.3, 0.3, 0.3, 1)

			Final color = (0.0, 0.0, 0.0, 0.3) * 1 + (0.657, 0.657, 0.657, 1) * 0.7
						= (0.4599, 0.4599, 0.4599, 1)
			Render	(color, 0.4599)

	Recall the formula is 
		Cf = (Cs * S) + (Cd * D)

	Cs is either 0 or 0.3 (or whatever your rate is)
		D is (1-0.3)

	we just need something either always increasing or decreasing, and GL_ONE, GL_ONE_MINUS_SRC satisfies it
*/


void FogView::fadeUpdate()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDepthMask(false);

	glm::vec2 texelSize = glm::vec2((float)1 / m_textureWidth, (float)1 / m_textureHeight);

	glViewport(0, 0, m_textureWidth, m_textureHeight);
	
	p_renderer = &global.rendererMgr->r_fogEdgeBlur;
	p_renderer->enableShader();	
	glBindFramebuffer(GL_FRAMEBUFFER, blurPassFBO1.FBO);

		p_renderer->setData(R_FOG_EDGE_BLUR::u_texture, 0, GL_TEXTURE_2D, m_fogTexture);
		p_renderer->setData(R_FOG_EDGE_BLUR::u_texelSize, texelSize);
		p_renderer->setData(R_FOG_EDGE_BLUR::u_offset, 0.6667f);
		o_updateQuadGameObject.renderCore(m_fogFadeUpdatePipeline, p_renderer);

	glBindFramebuffer(GL_FRAMEBUFFER, blurPassFBO2.FBO);

		p_renderer->setData(R_FOG_EDGE_BLUR::u_texture, 0, GL_TEXTURE_2D, blurPassFBO1.colorTexture);
		p_renderer->setData(R_FOG_EDGE_BLUR::u_texelSize, texelSize);
		p_renderer->setData(R_FOG_EDGE_BLUR::u_offset, 1.5f);
		o_updateQuadGameObject.renderCore(m_fogFadeUpdatePipeline, p_renderer);

	p_renderer->disableShader();
	


	glBindFramebuffer(GL_FRAMEBUFFER, m_fogFadeUpdateFBO.FBO);
	glEnable(GL_BLEND);	
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	p_renderer = &global.rendererMgr->r_fogFadeUpdate;
	p_renderer->enableShader();
		p_renderer->setData(R_FOG_FADE_UPDATE::u_texture, 0, GL_TEXTURE_2D, blurPassFBO2.colorTexture);
		// p_renderer->setData(R_FOG_FADE_UPDATE::u_texture, 0, GL_TEXTURE_2D, m_fogTexture);
		o_updateQuadGameObject.renderCore(m_fogFadeUpdatePipeline, p_renderer);
	p_renderer->disableShader();	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FogView::clearTexture()
{
	glBindTexture(GL_TEXTURE_2D, m_fogTexture);
	GLubyte* colorData = new GLubyte[4];
	colorData[0] = 255;
	colorData[1] = 0;
	colorData[2] = 0;
	colorData[3] = 0;
	for (int y = 0; y < m_textureHeight; y++)
	{
		for (int x = 0; x < m_textureWidth; x++)
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y,
				1, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, colorData);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}


// https://stackoverflow.com/questions/3887636/how-to-manipulate-texture-content-on-the-fly/10702468#10702468
// https://stackoverflow.com/questions/9863969/updating-a-texture-in-opengl-with-glteximage2d
// https://www.gamedev.net/forums/topic/577332-gltexsubimage2d-massive-performance-drop-off/
// glTexSubImage2D:
//	memroy from RAM is delivered to GPU ram and this information has to travel from one place to another. This takes time and
// is bandwidth limited.

void FogView::addDirtyCells(vector<FogCell> list)
{
	for (int i = 0; i < list.size(); i++)
	{
		dirtyFogCells.push_back(list[i]);
	}
}

void FogView::update()
{
	updateFOWTexture();
	fadeUpdate();
}




void FogView::updateFOWTexture()
{
	
	if (dirtyFogCells.size() > 0)
	{

		GLubyte* colorData = new GLubyte[4];
		colorData[0] = 255;
		colorData[1] = 255;
		colorData[2] = 255;
		colorData[3] = 255;
		
		unsigned int color = 0x000000FF;

		glBindTexture(GL_TEXTURE_2D, m_fogTexture);

		for (int i = 0; i < dirtyFogCells.size(); i++)
		{
//			glTexSubImage2D(GL_TEXTURE_2D, 0, dirtyFogCells[i].coord.x, dirtyFogCells[i].coord.y,
//				m_textureWidth, m_textureHeight, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, colorData);
	
			glTexSubImage2D(GL_TEXTURE_2D, 0, dirtyFogCells[i].coord.x, dirtyFogCells[i].coord.y,
				1, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, (GLubyte*)(&dirtyFogCells[i].data));

		}

//		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
	//		1, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, (GLubyte*)(&color));


		glBindTexture(GL_TEXTURE_2D, NULL);

		dirtyFogCells.clear();
	}
	
}


void FogView::render(Pipeline& p)
{
	p_renderer = &global.rendererMgr->r_fow;
	p_renderer->enableShader();
		p_renderer->setData(R_FOW::u_texture, 0, GL_TEXTURE_2D, m_fogFadeTexture);
		
		//		p_renderer->setData(R_FOW::u_texture, 0, GL_TEXTURE_2D, m_fogFadeUpdateFBO.colorTexture);

		FOWGameObject.renderCore(p, p_renderer);
	p_renderer->disableShader();

}