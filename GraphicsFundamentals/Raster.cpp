#include "Raster.h"

namespace CELL {
	Raster::Raster(int w, int h, void* buffer) : _width(w), _height(h) {
		_buffer = (Rgba*)buffer;
	}

	Raster::~Raster() {}

	void Raster::drawPoint(int x, int y, Rgba color, int ptSize) {
		switch (ptSize)
		{
		case 1:
			//_buffer[x][y] = color;
			setPiexl(x, y, color);
			break;
		case 2:
			/*_buffer[x][y] = color;
			_buffer[x][y + 1] = color;
			_buffer[x + 1][y] = color;
			_buffer[x + 1][y + 1] = color;*/
			setPiexl(x, y, color);
			setPiexl(x + 1, y, color);
			setPiexl(x, y + 1, color);
			setPiexl(x + 1, y + 1, color);
			break;
		case 3:
			setPiexl(x + 1, y - 1, color);
			setPiexl(x - 1, y - 1, color);
			setPiexl(x, y - 1, color);
			setPiexl(x - 1, y, color);
			setPiexl(x, y, color);
			setPiexl(x + 1, y, color);
			setPiexl(x, y + 1, color);
			setPiexl(x + 1, y + 1, color);
			setPiexl(x - 1, y + 1, color);
			break;
		default:
			break;
		}
	}

	void Raster::clear() {
		memset(_buffer, 255, sizeof(Rgba) * getLength());
	}

	int Raster::getLength() {
		return _width * _height;
	}

	void Raster::drawLine(float2 pt1, float2 pt2, Rgba color) {
		// »ñÈ¡Ð±ÂÊ
		float xOffset = pt1.x - pt2.x;
		float yOffset = pt1.y - pt2.y;

		float scope = yOffset / xOffset;

		for (float x = pt1.x; x <= pt2.x; x += 1.0f) {
			float y = pt1.y + (x - pt1.x) * scope;
			setPiexl(x, y, color);
		}
	}
}