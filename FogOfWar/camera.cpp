#include "camera.h"

Camera::Camera()
{
	m_cameraZoom = 10;
	m_pipeline.setMatrixMode(PROJECTION_MATRIX);
	m_pipeline.loadIdentity();
}


void Camera::setPanningBounds(glm::vec3 worldMapMin, glm::vec3 worldMapMax)
{
	m_minBound = worldMapMin;
	m_maxBound = worldMapMax;

	updateBounds();
}

void Camera::updateBounds()
{
	m_curMinBound = m_minBound + m_cameraZoom;
	m_curMaxBound = m_maxBound - m_cameraZoom;
}




void Camera::setPos(glm::vec3 pos)
{
//	cout << "here " << m_cameraZoom << endl;
	pos.x = max(m_curMinBound.x, pos.x);
	pos.y = max(m_curMinBound.y, pos.y);

	pos.x = min(m_curMaxBound.x, pos.x);
	pos.y = min(m_curMaxBound.y, pos.y);


//	cout << "	pos " << pos.x << " " << pos.y << endl;
//	cout << "	m_curMinBound " << m_curMinBound.x << " " << m_curMinBound.y << endl;
//	cout << "	m_curMaxBound " << m_curMaxBound.x << " " << m_curMaxBound.y << endl;

	transform.setPosition(pos);

	updateOrtho();
}

void Camera::setZoom(float newZoom)
{
	m_cameraZoom = newZoom;
	updateBounds();
	updateOrtho();
}


void Camera::forceSetOrtho(float left, float right, float bottom, float top, float nearIn, float farIn)
{
	m_pipeline.setMatrixMode(PROJECTION_MATRIX);
	m_pipeline.loadIdentity();

	m_pipeline.ortho(left, right, bottom, top, nearIn, farIn);
}


Pipeline& Camera::getPipeline()
{
	return m_pipeline;
}

void Camera::updateOrtho()
{
	glm::vec3 pos = transform.getPosition();

	forceSetOrtho(pos.x - m_cameraZoom,
				pos.x + m_cameraZoom,
				pos.y - m_cameraZoom,
				pos.y + m_cameraZoom,
		 utl::Z_NEAR, utl::Z_FAR);
}

void Camera::zoomOut()
{
	m_cameraZoom -= CAMERA_ZOOM_DELTA;
	updateBounds();
	updateOrtho();
}

void Camera::zoomIn()
{
	m_cameraZoom += CAMERA_ZOOM_DELTA;
	updateBounds();
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