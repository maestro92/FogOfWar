#include "map_view.h"
#include "global.h"

void MapView::init(World* world, Map* map)
{
	/*
	blueGem = utl::loadSDLImage("Assets/blue gem.jpg");
	redGem = utl::loadSDLImage("Assets/red gem.jpg");
	orangeGem = utl::loadSDLImage("Assets/orange gem.jpg");
	purpleGem = utl::loadSDLImage("Assets/purple gem.jpg");
	greenGem = utl::loadSDLImage("Assets/green gem.jpg");
	*/

	m_map = map;

	int w = map->getWidth();
	int h = map->getHeight();
	
	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{		
			glm::vec2 min = map->getCellMinCorner(glm::ivec2(x, y));
			glm::vec2 max = map->getCellMaxCorner(glm::ivec2(x, y));

			glm::vec3 color;
			switch (m_map->getCell(x, y))
			{
				case Map::Cell::Empty:
					color = COLOR_WHITE;
					break;
				case Map::Cell::Wall:
					color = COLOR_GRAY;
					break;
				default:
					break;
			}

			glm::vec3 min3D = world->simPos2WorldPos(min);
			glm::vec3 max3D = world->simPos2WorldPos(max);

			ModelManager::buildQuad3D(min3D, max3D, color, vertices, indices);
		}
	}
	
	Mesh m(vertices, indices);

	gridCellsModel = new Model();
	gridCellsModel->addMesh(m);

	gridCellsGameObject.setModel(gridCellsModel);

	glm::vec3 pos = gridCellsGameObject.getPosition();
	pos.z -= 5;
	gridCellsGameObject.setPosition(pos);

	initGridLines(world);
}




void MapView::initGridLines(World* world)
{
	
	//	gridLines
	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;

	for (int x = 0; x <= m_map->getWidth(); x++)
	{
		glm::vec2 simPos0 = glm::vec2(x, 0);
		glm::vec2 simPos1 = glm::vec2(x, m_map->getHeight());

		glm::vec3 worldPos0 = world->simPos2WorldPos(simPos0);
		glm::vec3 worldPos1 = world->simPos2WorldPos(simPos1);

		ModelManager::buildLinesBetweenTwoPoints3D(worldPos0, worldPos1, 0.02, vertices, indices);
	}

	for (int y = 0; y <= m_map->getHeight(); y++)
	{
		glm::vec2 simPos0 = glm::vec2(0, y);
		glm::vec2 simPos1 = glm::vec2(m_map->getWidth(), y);

		glm::vec3 worldPos0 = world->simPos2WorldPos(simPos0);
		glm::vec3 worldPos1 = world->simPos2WorldPos(simPos1);

		ModelManager::buildLinesBetweenTwoPoints3D(worldPos0, worldPos1, 0.02, vertices, indices);
	}
	Mesh m(vertices, indices);

	gridLineModels = new Model();
	gridLineModels->addMesh(m);
	gridLines.setModel(gridLineModels);
	
}




void MapView::render(Pipeline& p)
{
	
	p_renderer = &global.rendererMgr->r_fullVertexColor;
	p_renderer->enableShader();
		gridCellsGameObject.renderCore(p, p_renderer);
	p_renderer->disableShader();
	
	p_renderer = &global.rendererMgr->r_fullColor;
	p_renderer->enableShader();
		p_renderer->setData(R_FULL_COLOR::u_color, COLOR_BLACK);
		gridLines.renderCore(p, p_renderer);
	p_renderer->disableShader();

	/*
	p_renderer = &global.rendererMgr->r_fullColor;

	p_renderer->enableShader();
	for (int y = 0; y < m_map->getHeight(); y++)
	{
		for (int x = 0; x < m_map->getWidth(); x++)
		{
			glm::vec3 color;
			switch (m_map->getCell(x,y))
			{
				case Map::Cell::Empty:
					color = COLOR_WHITE;
					break;
				case Map::Cell::Wall:
					color = COLOR_GRAY;
					break;
				default:
					break;
			}

			p_renderer->setData(R_FULL_COLOR::u_color, color);
			gridCells[y][x].renderGroup(p, p_renderer);
		}

	}

	p_renderer->setData(R_FULL_COLOR::u_color, COLOR_BLACK);
	gridLines.renderGroup(p, p_renderer);

	if (highlight.canRender())
	{
		renderHighlight(p);
	}

	p_renderer->disableShader();
	*/
}


/*
void MapView::createMeshForGridCellsHighlight(vector<glm::vec2> traversal)
{
	//	gridLines
	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	VertexData v;

	for (int i = 0; i < traversal.size(); i++)
	{
		glm::vec2 gridCoord = traversal[i];

		glm::vec2 pos0 = m_map->getCellMinCorner(gridCoord);
		glm::vec2 pos2 = m_map->getCellMaxCorner(gridCoord);
		
		glm::vec2 pos1 = glm::vec2(pos2.x, pos0.y);
		glm::vec2 pos3 = glm::vec2(pos0.x, pos2.y);


		ModelManager::buildLinesBetweenTwoPoints2D(pos0, pos1, 0.05, vertices, indices);
		ModelManager::buildLinesBetweenTwoPoints2D(pos1, pos2, 0.05, vertices, indices);
		ModelManager::buildLinesBetweenTwoPoints2D(pos2, pos3, 0.05, vertices, indices);
		ModelManager::buildLinesBetweenTwoPoints2D(pos3, pos0, 0.05, vertices, indices);
	}

	Mesh m(vertices, indices);

	highlightModel = new Model();
	highlightModel->addMesh(m);

	highlight.setModel(highlightModel);
}
*/