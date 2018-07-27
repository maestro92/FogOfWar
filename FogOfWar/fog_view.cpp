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

	// we make it to the smallest power of 2 that is bigger than the fog map size
	m_textureWidth = m_fogManager->getWidth() - 1;
	m_textureWidth |= m_textureWidth >> 1;    // sizes up to 4 x 4
	m_textureWidth |= m_textureWidth >> 2;    // sizes up to 16 x 16
	m_textureWidth |= m_textureWidth >> 4;    // sizes up to 256 x 256
	m_textureWidth |= m_textureWidth >> 8;    // sizes up to 16384 x 16384
	m_textureWidth += 1;

	m_textureHeight = m_fogManager->getHeight() - 1;
	m_textureHeight |= m_textureHeight >> 1;
	m_textureHeight |= m_textureHeight >> 2;
	m_textureHeight |= m_textureHeight >> 4;
	m_textureHeight |= m_textureHeight >> 8;
	m_textureHeight += 1;


	// so we fit the fog texture inside our texture, so parts of it is unused
	m_fogTexture = utl::createNewTexture(m_textureWidth, m_textureHeight, GL_NEAREST, GL_CLAMP_TO_BORDER);

	m_noiseTexture = utl::loadTexture("Assets/Images/FogNoise.png", GL_LINEAR, GL_REPEAT, false);
	clearTexture();

	initBlurPasses();
	initFadeUpdateStuff();



	initFogMeshVertex2UVMatrix(world, map);
}



void FogView::initFogMeshVertex2UVMatrix(World* world, Map* map)
{

	glm::vec2 fogScale = glm::vec2(m_fogManager->getWidth() / (float)m_textureWidth, m_fogManager->getHeight() / (float)m_textureHeight);



	glm::ivec2 minGc = glm::ivec2(0, 0);
	glm::ivec2 maxGc = glm::ivec2(m_fogManager->getWidth() - 1, m_fogManager->getHeight() - 1);

	glm::vec2 minSimPos = map->getCellMinCorner(minGc);
	glm::vec2 maxSimPos = map->getCellMaxCorner(maxGc);

	glm::vec3 minWorldPos = world->simPos2WorldPos(minSimPos);
	glm::vec3 maxWorldPos = world->simPos2WorldPos(maxSimPos);

	glm::vec2 fogMeshOrigin = glm::vec2(minWorldPos.x, minWorldPos.y);
	glm::vec2 xAxis = glm::vec2(maxWorldPos.x, minWorldPos.x) - fogMeshOrigin;
	glm::vec2 yAxis = glm::vec2(minWorldPos.x, maxWorldPos.y) - fogMeshOrigin;
	
	/*
		to build the matrix, we put all the axis of the coordinate system by columns

			| xAxis.x		yAxis.x	|
			|						|
			| xAxis.y		yAxis.y	|
	*/

	float invDeterminant = 1 / (xAxis.x * yAxis.y - yAxis.x * xAxis.y);

	// in the shader, we will be converting from fogMeshVertexPos to UV coordinates
	// so we need to inverse of it
	// also we want to scale our x values with fogScale.x and fogScale.y respectively
	glm::vec4 fogMeshVertex2UV = invDeterminant * glm::vec4(fogScale.x * yAxis.y,		fogScale.x * -yAxis.x,
															fogScale.y * -xAxis.y,		fogScale.y *  xAxis.x);
	
	// https://thebookofshaders.com/13/	
	p_renderer = &global.rendererMgr->r_fow;
	p_renderer->setData(R_FOW::u_noiseTexSamplingLocationScale, 0.05f);
	p_renderer->setData(R_FOW::u_fogMeshVertexOrigin, fogMeshOrigin);
	p_renderer->setData(R_FOW::u_fogMeshVertex2UVMat, fogMeshVertex2UV);
	p_renderer->setData(R_FOW::u_noiseSpeed, 0.1f);
	p_renderer->setData(R_FOW::u_edgeShape, 0.01f);
	

}


/*
// all about fbm
// http://www.iquilezles.org/www/articles/warp/warp.htm
// https://stackoverflow.com/questions/16999520/in-need-of-fractional-brownian-noise-fbm-vs-perlin-noise-clarification
// https://code.google.com/archive/p/fractalterraingeneration/wikis/Fractional_Brownian_Motion.wiki  <<<<<<<<<<<<<< this link is very helpful!!
// which is a simple sum of perlin noise functions with increasing frequencies and decreasing amplitudes
// octave just means how many layers you are putting together
// 


	for (i = 0; i < octaves; ++i) 
	{ 
		total += noise((float)x * frequency, (float)y * frequency) * amplitude; 
		frequency *= lacunarity; 
		amplitude *= gain; 
	}


	lerp vs Smoothstep (Hermite Interpolation)

	Lerp is the simple method that interpolates along a straight line, according to the distance you specify.

	SmoothStep is similar, but first applies a curve function to the input distance value, so the movement starts out slow, speeds up in the middle, then slows down again. The velocity of the interpolation follows a kind of S curve. This function is often used in animation and blending scenarios where smooth changes are required, for instance
	http://www.fundza.com/rman_shaders/smoothstep/index.html has some examples of how people have used it in writing Renderman shaders for offline movie rendering effects.
	
	https://social.msdn.microsoft.com/Forums/en-US/db16c8ab-2b78-4771-a2af-bb92a60e8ef9/game-math-101-lerp-vs-smoothstep-?forum=xnagamestudioexpress
*/
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

void FogView::update(float elapsedTime)
{
	updateFOWTexture();
	fadeUpdate();

	p_renderer = &global.rendererMgr->r_fow;
	p_renderer->setData(R_FOW::u_time, elapsedTime);
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
			glTexSubImage2D(GL_TEXTURE_2D, 0, dirtyFogCells[i].coord.x, dirtyFogCells[i].coord.y,
				1, 1, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, (GLubyte*)(&dirtyFogCells[i].data));
		}

		glBindTexture(GL_TEXTURE_2D, NULL);
		dirtyFogCells.clear();
	}
	
}


void FogView::render(Pipeline& p)
{
	p_renderer = &global.rendererMgr->r_fow;
	p_renderer->enableShader();
		p_renderer->setData(R_FOW::u_fogFadeTexture, 0, GL_TEXTURE_2D, m_fogFadeTexture);
		p_renderer->setData(R_FOW::u_noiseTexture, 1, GL_TEXTURE_2D, m_noiseTexture);
//		p_renderer->setData(R_FOW::u_texture, 0, GL_TEXTURE_2D, m_fogFadeUpdateFBO.colorTexture);

		FOWGameObject.renderCore(p, p_renderer);
	p_renderer->disableShader();

}