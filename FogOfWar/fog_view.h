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
#include "pipeline.h"
#include "fog_cell.h"

class FogView
{


	public:
		void init(World* world, Map* map, FogManager* fogManager);
		void render(Pipeline& p);
		void update();
		void clearTexture();
		void addDirtyCells(vector<FogCell> list);
		void fadeUpdate();

	private:

		void initBlurPasses();
		void updateFOWTexture();

		FogManager* m_fogManager;
		Renderer* p_renderer;

		GLuint m_fogTexture;
		int m_textureWidth;
		int m_textureHeight;



		Model* FOWModel;
		WorldObject FOWGameObject;
	
		vector<FogCell> dirtyFogCells;


		FrameBufferObject blurPassFBO1;
		FrameBufferObject blurPassFBO2;



		GLuint m_fogFadeTexture;
		void initFadeUpdateStuff();
		Pipeline m_fogFadeUpdatePipeline;
		FrameBufferObject m_fogFadeUpdateFBO;
		Model* m_fadeUpdateQuad;
		WorldObject o_updateQuadGameObject;


};