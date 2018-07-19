#pragma once

#include "renderer.h"
#include "pipeline.h"
#include "transform_component.h"
#include "model.h"

class RenderComponent
{
	public:
		void render(TransformComponent* transform, Pipeline& p, Renderer* r);
		bool canRender();

		void setModel(Model* model);
		void clearModel();

	private:
		Model* m_model;
};