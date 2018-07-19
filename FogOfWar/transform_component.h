#pragma once

#include "utility.h"
#include "renderer.h"
#include "model_enum.h"
#include "model.h"
#include <string>
#include "global.h"
#include <vector>


using namespace std;

class TransformComponent
{
	public:
		TransformComponent();
		inline void setScale(float s);
		inline void setScale(glm::vec3 scale);
		inline void setScale(float x, float y, float z);

		inline void setPosition(glm::vec3 pos);
		inline void setPosition(float x, float y, float z);

		inline glm::vec3 getPosition();
		inline glm::vec3 getScale();
		inline glm::mat4 getRotation();

		glm::vec3 m_xAxis;
		glm::vec3 m_yAxis;
		glm::vec3 m_zAxis;
		inline void setRotation(glm::mat4 rot);
		inline void updateModelMatrix();

	private:
		glm::vec3 m_position;
		glm::vec3 m_velocity;
		glm::vec3 m_scale;
		glm::mat4 m_rotation;
		glm::mat4 m_modelMatrix;
};


inline void TransformComponent::setPosition(glm::vec3 pos)
{
	m_position = pos;
}

inline void TransformComponent::setPosition(float x, float y, float z)
{
	m_position = glm::vec3(x, y, z);
}

inline void TransformComponent::setScale(float s)
{
	m_scale = glm::vec3(s, s, s);
}

inline void TransformComponent::setScale(glm::vec3 scale)
{
	m_scale = scale;
}

inline void TransformComponent::setScale(float x, float y, float z)
{
	m_scale = glm::vec3(x, y, z);
}

inline void TransformComponent::setRotation(glm::mat4 rot)
{
	m_xAxis = glm::vec3(rot[0][0], rot[0][1], rot[0][2]);
	m_yAxis = glm::vec3(rot[1][0], rot[1][1], rot[1][2]);
	m_zAxis = glm::vec3(rot[2][0], rot[2][1], rot[2][2]);

	float temp[16] = {	rot[0][0], rot[0][1], rot[0][2], 0.0,
						rot[1][0], rot[1][1], rot[1][2], 0.0,
						rot[2][0], rot[2][1], rot[2][2], 0.0,
						0.0,       0.0,       0.0,       1.0 };
	m_rotation = glm::make_mat4(temp);
}


inline glm::vec3 TransformComponent::getPosition()
{
	return m_position;
}

inline glm::vec3 TransformComponent::getScale()
{
	return m_scale;
}

inline glm::mat4 TransformComponent::getRotation()
{
	return m_rotation;
}

inline void TransformComponent::updateModelMatrix()
{
	m_modelMatrix = glm::translate(m_position) * m_rotation * glm::scale(m_scale);
}

