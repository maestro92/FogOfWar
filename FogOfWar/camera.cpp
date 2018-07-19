#include "camera.h"

Camera::Camera()
{
	m_cameraZoom = 10;
	m_pipeline.setMatrixMode(PROJECTION_MATRIX);
	m_pipeline.loadIdentity();
}

void Camera::setPos(glm::vec3 pos)
{
	transform.setPosition(pos);
}

void Camera::setZoom(float newZoom)
{
	m_cameraZoom = newZoom;
}


void Camera::setOrtho(float x, float y, float width, float height, float nearIn, float farIn)
{
	m_pipeline.setMatrixMode(PROJECTION_MATRIX);
	m_pipeline.loadIdentity();

	m_pipeline.ortho(x, y, width, height, nearIn, farIn);
}


Pipeline& Camera::getPipeline()
{
	return m_pipeline;
}

void Camera::updateOrtho()
{
	glm::vec3 pos = transform.getPosition();
	setOrtho(pos.x - m_cameraZoom,
		pos.x + m_cameraZoom,
		pos.y - m_cameraZoom,
		pos.y + m_cameraZoom, utl::Z_NEAR, utl::Z_FAR);
}

void Camera::zoomOut()
{
	m_cameraZoom -= CAMERA_ZOOM_DELTA;
	updateOrtho();
}

void Camera::zoomIn()
{
	m_cameraZoom += CAMERA_ZOOM_DELTA;
	updateOrtho();
}

glm::vec3 Camera::screenToWorldPoint(glm::vec2 screenPoint)
{
	glm::vec4 viewPort = glm::vec4(0, 0, utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);
	glm::vec3 temp = glm::vec3(screenPoint.x, screenPoint.y, 0);

	glm::vec3 worldPoint = glm::unProject(temp, (m_pipeline.getModelViewMatrix()), m_pipeline.getProjectionMatrix(), viewPort);
	return worldPoint;
}

glm::vec3 Camera::worldToScreen(glm::vec3 pos)
{
	glm::vec4 viewPort = glm::vec4(0, 0, utl::SCREEN_WIDTH, utl::SCREEN_HEIGHT);

	glm::vec3 screenPos = glm::project(pos, m_pipeline.getModelViewMatrix(), m_pipeline.getProjectionMatrix(), viewPort);
	return screenPos;
}