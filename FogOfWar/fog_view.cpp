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

	m_fogTexture = utl::createNewTexture(m_textureWidth, m_textureHeight);
	clearTexture();
	// m_fogTexture = utl::loadTexture("Assets/Images/dots.png", true);
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
		p_renderer->setData(R_FOW::u_texture, 0, GL_TEXTURE_2D, m_fogTexture);
		FOWGameObject.renderCore(p, p_renderer);
	p_renderer->disableShader();

}