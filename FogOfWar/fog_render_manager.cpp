#include "fog_render_manager.h"



void FogRenderManager::init(World* world, Map* map, FogManager* fogManager)
{
	m_fogManager = fogManager;

	glm::ivec2 minGc = glm::ivec2(0, 0);
	glm::ivec2 maxGc = glm::ivec2(map->getWidth() - 1, map->getHeight() - 1);

	glm::vec2 minSimPos = map->getCellMinCorner(minGc);
	glm::vec2 maxSimPos = map->getCellMaxCorner(minGc);

	glm::vec3 minWorldPos = world->simPos2WorldPos(minSimPos);
	glm::vec3 maxWorldPos = world->simPos2WorldPos(maxSimPos);

	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	ModelManager::buildQuad3D(minWorldPos, maxWorldPos, COLOR_GRAY, vertices, indices);

	Mesh m(vertices, indices);

	FOWModel = new Model();
	FOWModel->addMesh(m);
	glm::vec3 pos = FOWGameObjectt.getPosition();
	pos.z -= 3;
	FOWGameObjectt.setPosition(pos);

	FOWGameObjectt.setModel(FOWModel);

}



void FogRenderManager::render(Pipeline& p)
{
/*
	p_renderer = &global.rendererMgr->r_fullVertexColor;
	p_renderer->enableShader();
	gridCellsGameObject.renderCore(p, p_renderer);
	p_renderer->disableShader();
*/
}