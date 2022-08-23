#include "Raster.h"

namespace CELL {
	Image* Image::loadFromFile(const char* fileName)
	{
		// 1 获取图片格式
		FREE_IMAGE_FORMAT fifmt = FreeImage_GetFileType(fileName, 0);
		if (fifmt == FIF_UNKNOWN)
		{
			return nullptr;
		}

		// 2 加载图片
		FIBITMAP* dib = FreeImage_Load(fifmt, fileName, 0);

		FREE_IMAGE_COLOR_TYPE type = FreeImage_GetColorType(dib);

		// 获取数据指针
		FIBITMAP* temp = dib;
		dib = FreeImage_ConvertTo32Bits(dib);
		FreeImage_Unload(temp);

		BYTE* pixels = (BYTE*)FreeImage_GetBits(dib);
		int width = FreeImage_GetWidth(dib);
		int height = FreeImage_GetHeight(dib);

		Image* image = new Image(width, height, pixels);

		FreeImage_Unload(dib);
		return image;
	}

	Span::Span(int xStart, int xEnd, int y, Rgba colorS, Rgba colorE)
	{
		if (xStart < xEnd)
		{
			_xStart = xStart;
			_xEnd = xEnd;
			_y = y;

			_rolorStart = colorS;
			_colorEnd = colorE;
		}
		else
		{
			_xStart = xEnd;
			_xEnd = xStart;
			_y = y;

			_rolorStart = colorE;
			_colorEnd = colorS;
		}
	}

	// ------------------------------------------------
	Edge::Edge(int x1, int y1, int x2, int y2, Rgba color1, Rgba color2)
	{
		if (y1 < y2)
		{
			_x1 = x1;
			_y1 = y1;

			_x2 = x2;
			_y2 = y2;

			_c1 = color1;
			_c2 = color2;
		}
		else
		{
			_x1 = x2;
			_y1 = y2;

			_x2 = x1;
			_y2 = y1;

			_c1 = color2;
			_c2 = color1;
		}
	}

	//--------------------------------------------------

	Raster::Raster(int w, int h, void* buffer) : _width(w), _height(h), _color(90, 201, 87) {
		_buffer = (Rgba*)buffer;
	}

	Raster::~Raster() = default;

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

	void Raster::drawPoint(float2 pt, Rgba color)
	{
		setPiexl(pt.x, pt.y, color);
	}

	void Raster::clear() {
		memset(_buffer, 255, sizeof(Rgba) * getLength());
	}

	int Raster::getLength() {
		return _width * _height;
	}

	void Raster::drawLine(float2 pt1, float2 pt2, Rgba color1, Rgba color2) {
		// 获取斜率
		float xOffset = pt1.x - pt2.x;
		float yOffset = pt1.y - pt2.y;

		float xMin = tmin(pt1.x, pt2.x);
		float xMax = tmax(pt1.x, pt2.x);
		float yMin = tmin(pt1.y, pt2.y);
		float yMax = tmax(pt1.y, pt2.y);

		if (xOffset == 0.0f && yOffset == 0.0f)
		{
			setPiexl(xMin, yMin, color1);
		}
		else if (xOffset == 0.0f)
		{
			float length = yMax - yMin;

			for (float y = yMin; y <= yMax; y += 1.0f) {
				float scaler = (y - yMin) / length;
				Rgba color = colorLerp(color1, color2, scaler);
				setPiexl(xMin, y, color);
			}
		}
		else if (yOffset == 0.0f)
		{
			float length = xMax - xMin;
			for (float x = xMin; x <= xMax; x += 1.0f) {
				float scaler = (x - xMin) / length;
				Rgba color = colorLerp(color1, color2, scaler);
				setPiexl(x, yMax, color);
			}
		}
		else if (fabs(xOffset) > fabs(yOffset)) {
			float scope = yOffset / xOffset;
			float length = yMax - yMin;
			for (float x = xMin; x <= xMax; x += 1.0f) {
				float y = yMin + (x - xMin) * scope;
				float scaler = (y - yMin) / length;
				Rgba color = colorLerp(color1, color2, scaler);
				setPiexl(x, y, color);
			}
		}
		else if (fabs(xOffset) < fabs(yOffset)) {
			float scope = xOffset / yOffset;
			float length = xMax - xMin;
			for (float y = yMin; y <= yMax; y += 1.0f) {
				float x = xMin + (y - yMin) * scope;
				float scaler = (x - xMin) / length;
				Rgba color = colorLerp(color1, color2, scaler);
				setPiexl(x, y, color);
			}
		}
	}

	void Raster::drawArrays(DRAWMODE mode, const float2* points, int count)
	{
		switch (mode)
		{
		case DM_POINT:
		{
			for (int i = 0; i < count; ++i)
			{
				drawPoint(points[i], _color);
			}
		}
		break;
		case DM_LINES:
		{
			count = count / 2 * 2;		// 保证他是2的倍数
			for (int i = 0; i < count; i += 2)
			{
				drawLine(points[i], points[i + 1], _color, _color);
			}
		}

		break;
		case DM_LINE_LOOP:
		{
			drawLine(points[0], points[1], _color, _color);
			for (int i = 2; i < count; ++i)
			{
				drawLine(points[i - 1], points[i], _color, _color);
			}
			drawLine(points[0], points[count - 1], _color, _color);
		}
		break;
		case DM_LINE_STRIP:
		{
			drawLine(points[0], points[1], _color, _color);
			for (int i = 2; i < count; ++i)
			{
				drawLine(points[i - 1], points[i], _color, _color);
			}
		}
		break;
		default:
			break;
		}
	}

	void Raster::drawFillRect(int star_x, int star_y, int w, int h)
	{
		int left = tmax(star_x, 0);
		int top = tmax(star_y, 0);

		int right = tmin(star_x + w, _width);
		int bottom = tmin(star_y + h, _height);

		for (int x = left; x < right; ++x)
		{
			for (int y = top; y < bottom; ++y)
			{
				setPiexlEx(x, y, Rgba(139, 98, 210));
			}
		}
	}

	void Raster::drawRect(const int2* points, const Rgba* color)
	{
		/*
		 * 绘制如下一个渐变矩形
		 *
		 * p_0, c_0----------------------p_1, c_1
		 * |								|
		 * |								|
		 * |								|
		 * |								|
		 * p_3, c_3---------------------p_2, c_2
		 *
		 */

		int left = tmax(points[0].x, 0);
		int top = tmax(points[0].y, 0);

		int right = tmin(points[2].x, _width);
		int bottom = tmin(points[2].y, _height);

		float w = right - left;
		float h = bottom - top;

		for (int x = left; x < right; ++x)
		{
			// 计算横向的渐变颜色差值
			Rgba color0 = colorLerp(color[0], color[1], (x - left) / w);
			Rgba color1 = colorLerp(color[3], color[2], (x - left) / w);

			for (int y = top; y < bottom; ++y)
			{
				// 在横向渐变差值的基础上计算垂直方向的差值，得到这个点应该填充的颜色
				Rgba color2 = colorLerp(color0, color1, (y - top) / h);
				setPiexlEx(x, y, color2);
			}
		}
	}

	void Raster::drawSpan(const Span& span)
	{
		float length = span._xEnd - span._xStart;
		float scale = 0;
		float step = 1.0f / length;

		int starX = tmax(span._xStart, 0);
		int endX = tmin(span._xEnd, _width);

		scale += (starX - span._xStart) / length;

		for (int x = starX; x < endX; ++x)
		{
			//Rgba color = colorLerp(span._rolorStart, span._colorEnd, (x - span._xStart) / length);
			// 优化 把之前每次的减法和除法两个操作优化为做一个加法
			Rgba color = colorLerp(span._rolorStart, span._colorEnd, scale);
			scale += step;
			setPiexlEx(x, span._y, color);
		}
	}

	/*
	 *			(x1, y1)
	 *				/\
	 *			   /  \
	 *		E1	  /    \
	 *			 /      \
	 *			/        \		E2
	 * (x2,y2) /		  \
	 *		   .		   \
	 *		     .          \
	 *		E3		.		 \	(x3, y3)
	 *
	 */
	void Raster::drawEdge(const Edge& e1, const Edge& e2)
	{
		float yOffset1 = e1._y2 - e1._y1;
		if (yOffset1 == 0) return;

		float yOffset = e2._y2 - e2._y1;
		if (yOffset == 0) return;

		float xOffset = e2._x2 - e2._x1;
		float scale = 0.0f;
		float xStep = 1.0f / yOffset;

		float xOffset1 = e1._x2 - e1._x1;
		float scale1 = (e2._y1 - e1._y1) / yOffset1;
		float xStep1 = 1.0f / yOffset1;

		int starY = tmax(e2._y1, 0);
		int endY = tmin(e2._y2, _height);

		scale += (starY - e2._y1) / yOffset;

		int starY1 = tmax(e1._y1, 0);
		int endY1 = tmin(e1._y2, _height);

		scale1 += (starY1 - e1._y1) / yOffset1;

		for (int y = starY; y < endY; ++y)
		{
			int x1 = e1._x1 + scale1 * xOffset1;
			int x2 = e2._x1 + scale * xOffset;

			Rgba color1 = colorLerp(e1._c1, e1._c2, scale1);
			Rgba color2 = colorLerp(e2._c1, e2._c2, scale);

			Span span(x1, x2, y, color1, color2);

			drawSpan(span);

			scale += xStep;
			scale1 += xStep1;
		}
	}

	void Raster::drawTriggle(int2 p0, int2 p1, int2 p2, Rgba c0, Rgba c1, Rgba c2)
	{
		if (!isInRect(p0) && !isInRect(p1) && !isInRect(p2))
		{
			return;
		}
		Edge edges[3] = {
			Edge(p0.x, p0.y, p1.x, p1.y, c0, c1),
			Edge(p1.x, p1.y, p2.x, p2.y, c1, c2),
			Edge(p2.x, p2.y, p0.x, p0.y, c2, c0)
		};

		int iMax = 0;
		int length = edges[0]._y2 - edges[0]._y1;
		for (int i = 1; i < 3; ++i)
		{
			int len = edges[i]._y2 - edges[i]._y1;
			if (len > length)
			{
				length = len;
				iMax = i;
			}
		}
		int iShort1 = (iMax + 1) % 3;
		int iShort2 = (iMax + 2) % 3;
		drawEdge(edges[iMax], edges[iShort1]);
		drawEdge(edges[iMax], edges[iShort2]);
	}

	/*
	 * 纹理基础
	 */
	void Raster::drawImage(int star_x, int star_y, const Image* image)
	{
		int left = tmax(star_x, 0);
		int top = tmax(star_y, 0);

		int right = tmin(star_x + image->width(), _width);
		int bottom = tmin(star_y + image->height(), _height);

		for (int x = left; x < right; ++x)
		{
			for (int y = top; y < bottom; ++y)
			{
				Rgba color = image->piexlAt(x - left, y - top);
				setPiexlEx(x, y, color);
			}
		}
	}
}