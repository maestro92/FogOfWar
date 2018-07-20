#pragma once
#include "transform_component.h"
#include "render_component.h"
class Player
{
	public:
		const float STEP_SIZE = 0.3f;

		glm::vec2 simPos;
		glm::vec2 curDir;
		int vision;

		TransformComponent transform;
		RenderComponent render;

		void update();
		void setCurDir(glm::vec2 dir);
		void move();

		void renderCore(Pipeline& p, Renderer* r);
};