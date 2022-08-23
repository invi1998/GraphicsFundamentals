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

	class Span
	{
	public:
		int _xStart;
		int _xEnd;
		int _y;

		Rgba _rolorStart;
		Rgba _colorEnd;

	public:
		Span(int xStart, int xEnd, int y, Rgba colorS, Rgba colorE);
		~Span() = default;
	};

	class Edge
	{
	public:
		int _x1;
		int _y1;
		int _x2;
		int _y2;

		// 顶点颜色值
		Rgba _c1;
		Rgba _c2;

	public:
		Edge(int x1, int x2, int y1, int y2, Rgba color1, Rgba color2);
		~Edge() = default;
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
			// 这里其实还有优化空间，这里进行了4个比较，但是可以省去前面两个比较，采用无符号整形数就可以只进行后面两个比较
			if (x >= _width || y >= _height) {
				return;
			}
			_buffer[y * _width + x] = color;
		}

		// 不做安全检查的填充像素函数
		inline void setPiexlEx(unsigned x, unsigned y, Rgba color) {
			_buffer[y * _width + x] = color;
		}

		/*
		 * 光栅基础
		 */
	public:
		void drawPoint(int x, int y, Rgba color, int ptSize);

		void drawPoint(float2 pt, Rgba color);

		void drawLine(float2 pt1, float2 pt2, Rgba color1, Rgba color2);

		void drawArrays(DRAWMODE mode, const float2* points, int count);

		void drawFillRect(int star_x, int star_y, int w, int h);

		void drawRect(const int2* points, const Rgba* color);

		void drawSpan(const Span& span);

		void drawEdge(const Edge& e1, const Edge& e2);

		void drawTriggle(int2 p0, int2 p1, int2 p2, Rgba c0, Rgba c1, Rgba c2);

		inline bool isInRect(int2 pt)
		{
			if (pt.x >= 0 && pt.x <= _width && pt.y >= 0 && pt.y <= _height)
			{
				return true;
			}
			return false;
		}

		/*
		 * image 纹理基础
		 */
	public:
		void drawImage(int star_x, int star_y, int w, int h);
	};
}
