#pragma once

#include "define.h"
class FogManager
{
	const int VISIBLE = 1;
	const int HIDDEN = 0;

	public:

		void init(int w, int h);

		void setSource(glm::ivec2 center, int radius, int state);
		void fillLine(int x0, int x1, int y, int state);
		bool isValidRange(glm::ivec2 coord);

		int getWidth();
		int getHeight();
	private:
		int** m_fogCells;
		int m_width;
		int m_height;

};