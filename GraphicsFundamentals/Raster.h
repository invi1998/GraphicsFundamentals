#pragma once

#include "CELLMath.hpp"

namespace CELL {
	class Raster
	{
	public:
		Rgba _buffer[256][256];
	public:
		Raster();
		~Raster();

		void drawPoint(int x, int y, Rgba color, int ptSize);

		void clear();

		void setPiexl(int x, int y, Rgba color);
	};
}
