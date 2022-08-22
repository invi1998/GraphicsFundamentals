#pragma once

#include "CELLMath.hpp"

namespace CELL {
	enum DRAWMODE
	{
		DM_POINT = 0,
		DM_LINES = 1,
		DM_LINE_LOOP = 2,
		DM_LINE_STRIP = 3
	};

	class Raster
	{
	private:
		int _width;
		int _height;
		Rgba _color;
	public:
		Rgba* _buffer;
	public:
		Raster(int w, int h, void* buffer);
		~Raster();
		int getLength();
		void clear();

	private:

		/*inline void setPiexl(int x, int y, Rgba color) {*/
		inline void setPiexl(unsigned x, unsigned y, Rgba color) {
			// ������ʵ�����Ż��ռ䣬���������4���Ƚϣ����ǿ���ʡȥǰ�������Ƚϣ������޷����������Ϳ���ֻ���к��������Ƚ�
			if (x >= _width || y >= _height) {
				return;
			}
			_buffer[y * _width + x] = color;
		}

		// ������ȫ����������غ���
		inline void setPiexlEx(unsigned x, unsigned y, Rgba color) {
			_buffer[y * _width + x] = color;
		}

	public:
		void drawPoint(int x, int y, Rgba color, int ptSize);

		void drawPoint(float2 pt, Rgba color);

		void drawLine(float2 pt1, float2 pt2, Rgba color1, Rgba color2);

		void drawArrays(DRAWMODE mode, const float2* points, int count);

		void drawFillRect(int star_x, int star_y, int w, int h);

		void drawRect(const int2* points, const Rgba* color);
	};
}
