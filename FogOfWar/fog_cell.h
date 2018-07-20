#pragma once

#include "define.h"

struct FogCell
{
	glm::ivec2 coord;
	unsigned int data;

	FogCell()
	{}

	FogCell(glm::ivec2 coordIn, unsigned int dataIn)
	{
		coord = coordIn;
		data = dataIn;
	}
};