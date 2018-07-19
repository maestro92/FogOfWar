#include "player.h"

void Player::renderCore(Pipeline& p, Renderer* r)
{
	render.render(&transform, p, r);
}

void Player::move(glm::vec2 dir)
{
	simPos += STEP_SIZE * dir;
}

