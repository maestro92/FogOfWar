#ifndef MAP_VIEW_H_
#define MAP_VIEW_H_

#include "utility_sdl.h"
#include "map.h"
#include "world_object.h"
#include "model_manager.h"
#include "renderer.h"
#include "renderer_manager.h"
#include "world.h"

class MapView
{
	public:
		void init(World* world, Map* map);
		void initGridLines(World* world);
		void render(Pipeline& p);	

	private:

		Map* m_map;

		Renderer* p_renderer;
		Model* gridCellsModel;
		WorldObject gridCellsGameObject;

		Model* gridLineModels;			
		WorldObject gridLines;
};


#endif
