#pragma once
#include "transform_component.h"
#include "render_component.h"
class Player
{
	public:
		const float STEP_SIZE = 0.3f;

		void init();

		glm::vec2 simPos;
		int vision;

		TransformComponent transform;
		RenderComponent render;

		void move(glm::vec2 dir);

		void renderCore(Pipeline& p, Renderer* r);
};