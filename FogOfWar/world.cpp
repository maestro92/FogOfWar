#include "world.h"


glm::vec3 World::simPos2WorldPos(glm::vec2 simPos)
{
	return glm::vec3(simPos.x, simPos.y, 0);
}