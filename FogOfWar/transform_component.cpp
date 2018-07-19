
#include "transform_component.h"

TransformComponent::TransformComponent()
{
	m_position = glm::vec3(0.0, 0.0, 0.0);
	m_velocity = glm::vec3(0.0, 0.0, 0.0);
	m_scale = glm::vec3(1.0, 1.0, 1.0);
	m_rotation = glm::mat4(1.0);
}






