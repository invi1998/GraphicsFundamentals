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

		// ������ɫֵ
		Rgba _c1;
		Rgba _c2;

		float2 _uv1;
		float2 _uv2;
	public:
		Edge(int x1, int x2, int y1, int y2, Rgba color1, Rgba color2, float2 uv1, float2 uv2);
		~Edge() = default;
	};

	// ����Ԫ�ص�����
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

		DataElementDes* _positionPointer;	// ��������
		DataElementDes* _colorPointer;	// ��ɫ����
		DataElementDes* _uvPointer;	// uv��������
	public:
		Raster(int w, int h, void* buffer);
		~Raster();
		int getLength();
		void clear();

		struct Vertex
		{
			// 3����������
			int2 p0;
			int2 p1;
			int2 p2;
			// 3��UV����
			float2 uv0;
			float2 uv1;
			float2 uv2;
			// 3��������ɫ
			Rgba c0;
			Rgba c1;
			Rgba c2;
		};

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

		// ��ȡ��ǰ���ص�ı�����ɫ
		inline Rgba getPiexl(unsigned x, unsigned y) {
			return Rgba(_buffer[y * _width + x]);
		}

		/*
		 * ��դ����
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
		 * image �������
		 */
	public:
		void drawImage(int star_x, int star_y, const Image* image);

		/**
		 * \brief ����ͼƬplus�汾
		 * \param star_x ���ڻ��Ƶ���ʼλx
		 * \param star_y ���ڻ��Ƶ���ʼλy
		 * \param image ͼƬ
		 * \param x ͼƬ����ʼ����x
		 * \param y ͼƬ����ʼ����y
		 * \param w ͼƬ�Ļ��������
		 * \param h ͼƬ�Ļ��������
		 */
		void drawImage(int star_x, int star_y, const Image* image, int x, int y, int w, int h);

		void drawImageWidthColorKey(int star_x, int star_y, const Image* image, Rgba colorKey);

		/**
		 * \brief alpha����
		 * \param star_x ��ʼ��x
		 * \param star_y ��ʼ��y
		 * \param image ���Ƶ�ͼƬ
		 * \param alpha alphaͨ��ֵ
		 */
		void drawImageAlphaTest(int star_x, int star_y, const Image* image, byte alpha);

		// alpha ��� (��alpha͸����alpha��ϣ�
		void drawImageAlphaBlend(int star_x, int star_y, const Image* image, float alpha);

		/**
		 * \brief alpha ͸��
		 * \param star_x ��ʼ��x
		 * \param star_y ��ʼ��y
		 * \param image ����ͼƬ
		 * \param alpha alphaֵ��1 ��͸����0 ȫ͸��
		 */
		void drawImageAlpha(int star_x, int star_y, const Image* image, float alpha);

		/**
		 * \brief ͼ������
		 * \param distX Ŀ�껭���X
		 * \param distY Ŀ�껭���Y
		 * \param distW Ŀ�껭��Ŀ�
		 * \param distH Ŀ�껭��ĸ�
		 * \param image ���Ƶ�ͼ��
		 */
		void drawImageScale(int distX, int distY, int distW, int distH, const Image* image);

	public:
		// ģ��ʵ��OpenGL��״̬���������нӿڽ��и���

		/**
		 * \brief ָ�����㡢��ɫ��uv ���ݻ������ĺ���
		 * \param size ά�ȣ���ά������ά��������������һ���㣬float���ͣ���ô�������������2��float����3��float��
		 * \param type ��������
		 * \param stride ƫ��ֵ��ȡ���һ��Ԫ��ֵ����һ��Ԫ��ֵ��λ�����ģ��������ƫ��ֵ���м��㣩������˵�����ֵ�͵��� ���͵Ĵ�С*size
		 * \param pointer ����ָ��
		 */
		void vertexPointer(int size, DATATYPE type, int stride, const void* pointer);
		void colorPointer(int size, DATATYPE type, int stride, const void* color);
		void textureCoordPointer(int size, DATATYPE type, int stride, const void* uv);

		/**
		 * \brief ����ͼ��
		 * \param pri ���Ƶ�ģʽ���㣬�ߣ� �棩
		 * \param start �����￪ʼ��
		 * \param count ��������
		 */
		void drawArrays(DRAWMODE pri, int start, int count);
	};
}
