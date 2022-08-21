#pragma once

#include "CELLMath.hpp"

namespace CELL {
	class Raster
	{
	private:
		int _width;
		int _height;
	public:
		Rgba* _buffer;
	public:
		Raster(int w, int h, void* buffer);
		~Raster();
		int getLength();
		void clear();

	public:

		/*inline void setPiexl(int x, int y, Rgba color) {*/
		inline void setPiexl(unsigned x, unsigned y, Rgba color) {
			// 这里其实还有优化空间，这里进行了4个比较，但是可以省去前面两个比较，采用无符号整形数就可以只进行后面两个比较
			if (x >= _width || y >= _height) {
				return;
			}
			_buffer[y * _width + x] = color;
		}

		void drawPoint(int x, int y, Rgba color, int ptSize);

		void drawLine(float2 pt1, float2 pt2, Rgba color1, Rgba color2);
	};
}
