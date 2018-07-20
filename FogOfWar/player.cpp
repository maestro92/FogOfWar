#include "player.h"

void Player::renderCore(Pipeline& p, Renderer* r)
{
	render.render(&transform, p, r);
}

void Player::move()
{
	simPos += STEP_SIZE * curDir;
}

void Player::setCurDir(glm::vec2 dir)
{
	curDir = dir;
}

void Player::update()
{
	move();
}

