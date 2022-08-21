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
			// ������ʵ�����Ż��ռ䣬���������4���Ƚϣ����ǿ���ʡȥǰ�������Ƚϣ������޷����������Ϳ���ֻ���к��������Ƚ�
			if (x >= _width || y >= _height) {
				return;
			}
			_buffer[y * _width + x] = color;
		}

		void drawPoint(int x, int y, Rgba color, int ptSize);

		void drawLine(float2 pt1, float2 pt2, Rgba color1, Rgba color2);
	};
}
