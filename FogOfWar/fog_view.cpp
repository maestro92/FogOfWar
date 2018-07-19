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

//	tex.m_id = utl::loadTexture(textureFiles[i], GL_LINEAR_MIPMAP_LINEAR, GL_REPEAT, true);
	m_fogTexture = utl::createNewTexture(map->getWidth(), map->getHeight());
}


// https://stackoverflow.com/questions/3887636/how-to-manipulate-texture-content-on-the-fly/10702468#10702468
// https://stackoverflow.com/questions/9863969/updating-a-texture-in-opengl-with-glteximage2d
// https://www.gamedev.net/forums/topic/577332-gltexsubimage2d-massive-performance-drop-off/
// glTexSubImage2D:
//	memroy from RAM is delivered to GPU ram and this information has to travel from one place to another. This takes time and
// is bandwidth limited.

void FogView::updateFOWTexture()
{

}


void FogView::render(Pipeline& p)
{

	p_renderer = &global.rendererMgr->r_fow;
	p_renderer->enableShader();
		p_renderer->setData(R_FOW::u_texture, 0, GL_TEXTURE_2D, m_fogTexture);
		FOWGameObject.renderCore(p, p_renderer);
	p_renderer->disableShader();

}