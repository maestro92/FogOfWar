#pragma once

#include "renderer_constants.h"
#include "renderer.h"
#include "renderer_manager.h"
#include "world.h"
#include "fog_manager.h"
#include "model_manager.h"
#include "world_object.h"
#include "map.h"
#include "utility_gl.h"

class FogView
{
	public:
		void init(World* world, Map* map, FogManager* fogManager);
		void render(Pipeline& p);


		void updateFOWTexture();

	private:
		FogManager* m_fogManager;
		Renderer* p_renderer;

		GLuint m_fogTexture;
		Model* FOWModel;
		WorldObject FOWGameObject;
};