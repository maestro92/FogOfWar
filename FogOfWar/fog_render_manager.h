#pragma once

#include "renderer.h"
#include "renderer_manager.h"
#include "world.h"
#include "fog_manager.h"
#include "model_manager.h"
#include "world_object.h"
#include "map.h"

class FogRenderManager
{
	public:
		void init(World* world, Map* map, FogManager* fogManager);
		void render(Pipeline& p);

	private:
		FogManager* m_fogManager;
		Renderer* p_renderer;

		Model* FOWModel;
		WorldObject FOWGameObjectt;
};