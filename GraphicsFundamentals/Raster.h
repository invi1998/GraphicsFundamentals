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
		DM_LINE_STRIP = 3,
		DM_TRIANGLES = 4,
	};

	enum DATATYPE
	{
		DT_BYTE,
		DT_FLOAT,
		DT_DOUBLE
	};

	class Span
	{
	public:
		int _xStart;
		int _xEnd;
		int _y;

		Rgba _rolorStart;
		Rgba _colorEnd;

		float2 _uvStart;
		float2 _uvEnd;

	public:
		Span(int xStart, int xEnd, int y, Rgba colorS, Rgba colorE, float2 uvstart, float2 uvend);
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

		float2 _uv1;
		float2 _uv2;
	public:
		Edge(int x1, int x2, int y1, int y2, Rgba color1, Rgba color2, float2 uv1, float2 uv2);
		~Edge() = default;
	};

	// 数据元素的描述
	struct DataElementDes
	{
		int _size;
		DATATYPE _type;
		int _stride;
		const void* _data;
	};

	class Raster
	{
	private:
		int _width;
		int _height;
		Rgba _color;
		Rgba* _buffer;

		DataElementDes* _positionPointer;	// 顶点数据
		DataElementDes* _colorPointer;	// 颜色数据
		DataElementDes* _uvPointer;	// uv坐标数据
	public:
		Raster(int w, int h, void* buffer);
		~Raster();
		int getLength();
		void clear();

		struct Vertex
		{
			// 3个顶点坐标
			int2 p0;
			int2 p1;
			int2 p2;
			// 3组UV坐标
			float2 uv0;
			float2 uv1;
			float2 uv2;
			// 3个顶点颜色
			Rgba c0;
			Rgba c1;
			Rgba c2;
		};

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

		void drawSpan(const Span& span, Image* image);

		void drawEdge(const Edge& e1, const Edge& e2, Image* image);

		//void drawTriggle(int2 p0, int2 p1, int2 p2, Rgba c0, Rgba c1, Rgba c2);
		void drawTriggle(const Vertex& vertex, Image* image);

		void drawTriggle(Edge edges[], Image* image);

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
		 * \param image 图片
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
		 * \param image 绘制的图片
		 * \param alpha alpha通过值
		 */
		void drawImageAlphaTest(int star_x, int star_y, const Image* image, byte alpha);

		// alpha 混合 (带alpha透明的alpha混合）
		void drawImageAlphaBlend(int star_x, int star_y, const Image* image, float alpha);

		/**
		 * \brief alpha 透明
		 * \param star_x 起始点x
		 * \param star_y 起始点y
		 * \param image 绘制图片
		 * \param alpha alpha值，1 不透明，0 全透明
		 */
		void drawImageAlpha(int star_x, int star_y, const Image* image, float alpha);

		/**
		 * \brief 图像缩放
		 * \param distX 目标画板的X
		 * \param distY 目标画板的Y
		 * \param distW 目标画板的宽
		 * \param distH 目标画板的高
		 * \param image 绘制的图像
		 */
		void drawImageScale(int distX, int distY, int distW, int distH, const Image* image);

	public:
		// 模仿实现OpenGL的状态机，对现有接口进行改造

		/**
		 * \brief 指定顶点、颜色、uv 数据缓冲区的函数
		 * \param size 维度（二维还是三维，比如你这里有一个点，float类型，那么描述这个点是用2个float还是3个float）
		 * \param type 数据类型
		 * \param stride 偏移值（取完第一个元素值后，下一个元素值得位置在哪？就用这个偏移值进行计算）正常来说，这个值就等于 类型的大小*size
		 * \param pointer 数据指针
		 */
		void vertexPointer(int size, DATATYPE type, int stride, const void* pointer);
		void colorPointer(int size, DATATYPE type, int stride, const void* color);
		void textureCoordPointer(int size, DATATYPE type, int stride, const void* uv);

		/**
		 * \brief 绘制图像
		 * \param pri 绘制的模式（点，线， 面）
		 * \param start 从哪里开始？
		 * \param count 绘制数量
		 */
		void drawArrays(DRAWMODE pri, int start, int count);
	};
}
