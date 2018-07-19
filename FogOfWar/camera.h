#pragma once

#include "pipeline.h"
#include "transform_component.h"

const float CAMERA_POS_DELTA = 1;
const float CAMERA_ZOOM_DELTA = 1;

class Camera
{
	public:
		Camera();	
		void setPos(glm::vec3 pos);
		void setZoom(float zoom);		
		void updateOrtho();
		
		void setOrtho(float x, float y, float width, float height, float nearIn, float farIn);

		void zoomOut();
		void zoomIn();
		glm::vec3 screenToWorldPoint(glm::vec2 screenPoint);
		glm::vec3 worldToScreen(glm::vec3 pos);


		TransformComponent transform;

		Pipeline& getPipeline();


	private:
		float m_cameraZoom;
		Pipeline m_pipeline;

};