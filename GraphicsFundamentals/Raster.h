#pragma once

#include "CELLMath.hpp"
#include "FreeImage.h"
#include "Image.hpp"

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

		// 获取当前像素点的背景颜色
		inline Rgba getPiexl(unsigned x, unsigned y) {
			return Rgba(_buffer[y * _width + x]);
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
		void drawImage(int star_x, int star_y, const Image* image);

		/**
		 * \brief 绘制图片plus版本
		 * \param star_x 窗口绘制的起始位x
		 * \param star_y 窗口绘制的起始位y
		 * \param image 图片的地址
		 * \param x 图片的起始坐标x
		 * \param y 图片的起始坐标y
		 * \param w 图片的绘制区域宽
		 * \param h 图片的绘制区域高
		 */
		void drawImage(int star_x, int star_y, const Image* image, int x, int y, int w, int h);

		void drawImageWidthColorKey(int star_x, int star_y, const Image* image, Rgba colorKey);

		/**
		 * \brief alpha测试
		 * \param star_x 起始点x
		 * \param star_y 起始点y
		 * \param image 绘制的图片路径
		 * \param alpha alpha通过值
		 */
		void drawImageAlphaTest(int star_x, int star_y, const Image* image, byte alpha);

		// alpha 混合 (带alpha透明的alpha混合）
		void drawImageAlphaBlend(int star_x, int star_y, const Image* image, float alpha);

		/**
		 * \brief alpha 透明
		 * \param star_x 起始点x
		 * \param star_y 起始点y
		 * \param image 绘制图片的地址
		 * \param alpha alpha值，1 不透明，0 全透明
		 */
		void drawImageAlpha(int star_x, int star_y, const Image* image, float alpha);
	};
}
