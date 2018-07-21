#include "fog_manager.h"


void FogManager::init(int w, int h)
{
	m_width = w;
	m_height = h;
	m_fogCells = new int*[h];
	for (int y = 0; y < h; y++)
	{
		m_fogCells[y] = new int[w];
	}
}

		
void FogManager::setSource(glm::ivec2 center, int radius, int state, vector<FogCell>& dirtyCells)
{
	int x0 = center.x;
	int y0 = center.y;

	int x = 0;
	int y = radius;
	int err = 1 - radius;

	while (y >= x)
	{
		fillLine(x0 - x, x0 + x, y0 + y, state, dirtyCells);
		fillLine(x0 - y, x0 + y, y0 + x, state, dirtyCells);
		fillLine(x0 - x, x0 + x, y0 - y, state, dirtyCells);
		fillLine(x0 - y, x0 + y, y0 - x, state, dirtyCells);

		if (err <= 0)
		{
			err = err + 2 * x + 3;
		}
		else
		{
			err = err + 2 * (x - y) + 5;
			y--;
		}
		x++;
	}
}


void FogManager::fillLine(int x0, int x1, int y, int state, vector<FogCell>& dirtyFogCells)
{


	for (int x = x0; x <= x1; x++)
	{
		if (isValidRange(glm::ivec2(x, y)))
		{
			m_fogCells[y][x] = state;

			dirtyFogCells.push_back(FogCell(glm::ivec2(x, y), state));
			
			if (state == HIDDEN)
			{
			//	cout << "	" << x << " " << y << "setting shit hidden" << endl;
				dirtyFogCells.push_back(FogCell(glm::ivec2(x, y), 0x000000FF));
			}
			else
			{
			//	cout << "	" << x << " " << y << "setting shit Visible" << endl;
			//	dirtyFogCells.push_back(FogCell(glm::ivec2(x, y), 0x00FF00FF));
				dirtyFogCells.push_back(FogCell(glm::ivec2(x, y), 0x00FF0000));
			}
			
		}
	}
}

bool FogManager::isValidRange(glm::ivec2 coord)
{
	return 0 <= coord.x && coord.x < m_width
		&& 0 <= coord.y && coord.y < m_height;
}


int FogManager::getWidth()
{
	return m_width;
}

int FogManager::getHeight()
{
	return m_height;
}

