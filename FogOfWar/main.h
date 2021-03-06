#ifndef MAIN_H_
#define MAIN_H_

class WorldObject;

#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <unordered_set>
#include "SDL.h"
#include "SDL_image.h"
#include "fog_cell.h"
#include "define.h"
#include "utility.h"

#include "player.h"
#include "map.h"
#include "map_view.h"

#include "fog_view.h"
#include <queue>

#include <GL/glew.h>

#include "utility.h"
#include "shader.h"

#include "label.h"
#include "gui_manager.h"
#include <chrono>

#include "pipeline.h"
#include "camera.h"
#include <ft2build.h>

#include "renderer_manager.h"
#include "renderer.h"
#include "renderer_constants.h"
#include "world.h"

#include "quad_model.h"
#include "xyz_axis_model.h"
#include "world_object.h"
#include "raycaster.h"
#include "model_manager.h"

#define FRAME_VALUES 10
#include <list>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include "fog_manager.h"

#include <vector>

using namespace std;
/*

For style
use http://google-styleguide.googlecode.com/svn/trunk/cppguide.html#Function_Names

C++ style
http://geosoft.no/development/cppstyle.html

http://stackoverflow.com/questions/1228161/why-use-prefixes-on-member-variables-in-c-classes

I use:

m for members
c for constants/readonlys
p for pointer (and pp for pointer to pointer)
v for volatile
s for static
i for indexes and iterators
e for events
*/





const int TIME_PROFILER_BUFFER = 10;
const int FPS_PROFILER_BUFFER = 20;


struct TestCase
{
	TestCase()
	{
	};

	TestCase(vector<string> testMapIn, glm::vec2 startIn, glm::vec2 endIn)
	{
		testMap = testMapIn;
		start = startIn;
		end = endIn;
	}

	vector<string> testMap;
	glm::vec2 start;
	glm::vec2 end;
};


class FogOfWar
{
	public:
		Renderer*					p_renderer;
		
		/// GUI
		long long m_runningTime;
		Uint32 m_nextGameTick = 0;
		MouseState m_mouseState;

	//	Pipeline m_pipeline;

		float m_fps;
		float m_iterRefreshRate;
		float m_curIter;
		unsigned int m_frameCount;
		unsigned int m_frameTicks[FRAME_VALUES];
		unsigned int m_frameTicksIndex;
		unsigned int m_prevFrameTick;

		bool isRunning;


		// models
		Model*          p_model;


		bool containedFlag;

		WorldObject     o_worldAxis;
		Map map;
		MapView mapView;
	
		GUIManager m_gui;
		bool loadData;
		int frameNum;
		float m_zoom;
		float m_range;
	public:

		long long m_currentTimeMillis;

		int timeProfilerIndex;
		long long timeProfiler[TIME_PROFILER_BUFFER];

		int fpsProfilerIndex;
		int fpsProfiler[FPS_PROFILER_BUFFER];

		void initPlayer();
		void debugDrawing();

		~FogOfWar();

		/// init functions
		void init();

		void initGUI();

		WorldObject* constructLine(glm::vec2 p0, glm::vec2 p1, float width) const;
		void UpdatingCurrentRayNewEndPoint(glm::vec2 end);

		int endWithError(char* msg, int error = 0);
 

		void start();
		void update();

		int getAverageFPS();

		void clientFrame(long long dt);
		void render();
		void onMouseBtnUp();
		void onMouseBtnHold();
		void onMouseBtnDown();
		void onRightMouseBtnDown();

		WorldObject constructPoint(glm::vec2 p, float width) const;
		bool hasDrawnSource;
		bool hasDrawnEnd;

		bool shouldRenderCurrentRay();

		void GetTimeProfilerAverages();


		void renderGUI();
	

		vector<int> latencyOptions;		// round trip
		int latency;					// rount trip latency in milliseconds
		int curLatencyOption;


		long long getCurrentTimeMillis();
		void updateFogByMainPlayer(glm::ivec2 prevGc);

	private:
		long long startedTime;
		World world;
		Camera mainCamera;
		Player mainPlayer;
		FogManager fogManager;
		FogView fogView;
};

#endif