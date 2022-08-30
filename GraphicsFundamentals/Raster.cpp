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

		// 处理图片坐标和windows窗口坐标（y轴）倒置（不一致）的问题
		/*int pitch = width * 4;
		BYTE* row = new BYTE[width * 4];
		for (int j = 0; j < height / 2; j++)
		{
			memcpy_s(row, width * 4, pixels + j * pitch, pitch);
			memcpy_s(pixels + j * pitch, width * 4, pixels + (height - j - 1) * pitch, pitch);
			memcpy_s(pixels + (height - j - 1) * pitch, pitch, row, pitch);
		}

		delete[] row;*/

		Image* image = new Image(width, height, pixels);

		FreeImage_Unload(dib);
		return image;
	}

	Span::Span(int xStart, int xEnd, int y, Rgba colorS, Rgba colorE, float2 uvstart, float2 uvend)
	{
		if (xStart < xEnd)
		{
			_xStart = xStart;
			_xEnd = xEnd;
			_y = y;

			_rolorStart = colorS;
			_colorEnd = colorE;

			_uvStart = uvstart;
			_uvEnd = uvend;
		}
		else
		{
			_xStart = xEnd;
			_xEnd = xStart;
			_y = y;

			_rolorStart = colorE;
			_colorEnd = colorS;

			_uvStart = uvend;
			_uvEnd = uvstart;
		}
	}

	// ------------------------------------------------
	Edge::Edge(int x1, int y1, int x2, int y2, Rgba color1, Rgba color2, float2 uv1, float2 uv2)
	{
		if (y1 < y2)
		{
			_x1 = x1;
			_y1 = y1;

			_x2 = x2;
			_y2 = y2;

			_c1 = color1;
			_c2 = color2;

			_uv1 = uv1;
			_uv2 = uv2;
		}
		else
		{
			_x1 = x2;
			_y1 = y2;

			_x2 = x1;
			_y2 = y1;

			_c1 = color2;
			_c2 = color1;

			_uv1 = uv2;
			_uv2 = uv1;
		}
	}

	//--------------------------------------------------

	Raster::Raster(int w, int h, void* buffer) : _width(w), _height(h), _color(90, 201, 87) {
		_buffer = (Rgba*)buffer;

		memset(&_positionPointer, 0, sizeof(DataElementDes*));
		memset(&_colorPointer, 0, sizeof(DataElementDes*));
		memset(&_uvPointer, 0, sizeof(DataElementDes*));

		_positionPointer = new DataElementDes();
		_colorPointer = new DataElementDes();
		_uvPointer = new DataElementDes();
	}

	Raster::~Raster()
	{
		delete _positionPointer;
		delete _colorPointer;
		delete _uvPointer;
	};

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

	void Raster::drawSpan(const Span& span, Image* image)
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

			float2 uv = uvLerp(span._uvStart, span._uvEnd, scale);
			// 通过UV坐标提取图片像素坐标颜色值
			Rgba piexl = image->piexUV(uv.x, uv.y);

			Rgba dst = color + piexl;

			;
			setPiexlEx(x, span._y, dst);
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
	void Raster::drawEdge(const Edge& e1, const Edge& e2, Image* image)
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

			// 颜色差值计算
			Rgba color1 = colorLerp(e1._c1, e1._c2, scale1);
			Rgba color2 = colorLerp(e2._c1, e2._c2, scale);

			// UV坐标差值计算
			float2 uvstart = uvLerp(e1._uv1, e1._uv2, scale1);
			float2 uvend = uvLerp(e2._uv1, e2._uv2, scale);

			Span span(x1, x2, y, color1, color2, uvstart, uvend);

			drawSpan(span, image);

			scale += xStep;
			scale1 += xStep1;
		}
	}

	void Raster::drawTriggle(const Vertex& vertex, Image* image)
	{
		if (!isInRect(vertex.p0) && !isInRect(vertex.p1) && !isInRect(vertex.p2))
		{
			return;
		}
		Edge edges[3] = {
			Edge(vertex.p0.x, vertex.p0.y, vertex.p1.x, vertex.p1.y, vertex.c0, vertex.c1, vertex.uv0, vertex.uv1),
			Edge(vertex.p1.x, vertex.p1.y, vertex.p2.x, vertex.p2.y, vertex.c1, vertex.c2, vertex.uv1, vertex.uv2),
			Edge(vertex.p2.x, vertex.p2.y, vertex.p0.x, vertex.p0.y, vertex.c2, vertex.c0, vertex.uv2, vertex.uv0)
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
		drawEdge(edges[iMax], edges[iShort1], image);
		drawEdge(edges[iMax], edges[iShort2], image);
	}

	void Raster::drawTriggle(Edge edges[], Image* image)
	{
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
		drawEdge(edges[iMax], edges[iShort1], image);
		drawEdge(edges[iMax], edges[iShort2], image);
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
		int i = 0;
	}

	void Raster::drawImageWidthColorKey(int star_x, int star_y, const Image* image, Rgba colorKey)
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
				if (color != colorKey)
				{
					setPiexlEx(x, y, color);
				}
			}
		}
	}

	void Raster::drawImageAlphaTest(int star_x, int star_y, const Image* image, byte alpha)
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
				if (color._a > alpha)
				{
					setPiexlEx(x, y, color);
				}
			}
		}
	}

	void Raster::drawImageAlphaBlend(int star_x, int star_y, const Image* image, float alpha)
	{
		int left = tmax(star_x, 0);
		int top = tmax(star_y, 0);

		int right = tmin(star_x + image->width(), _width);
		int bottom = tmin(star_y + image->height(), _height);

		for (int x = left; x < right; ++x)
		{
			for (int y = top; y < bottom; ++y)
			{
				Rgba srcColor = image->piexlAt(x - left, y - top);
				Rgba dstColr = getPiexl(x, y);

				Rgba color = colorLerp(dstColr, srcColor, srcColor._a / 255.0f * alpha);
				setPiexlEx(x, y, color);
			}
		}
	}

	void Raster::drawImageAlpha(int star_x, int star_y, const Image* image, float alpha)
	{
		int left = tmax(star_x, 0);
		int top = tmax(star_y, 0);

		int right = tmin(star_x + image->width(), _width);
		int bottom = tmin(star_y + image->height(), _height);

		for (int x = left; x < right; ++x)
		{
			for (int y = top; y < bottom; ++y)
			{
				Rgba srcColor = image->piexlAt(x - left, y - top);
				Rgba dstColr = getPiexl(x, y);

				Rgba color = colorLerp(dstColr, srcColor, alpha);
				setPiexlEx(x, y, color);
			}
		}
	}

	void Raster::drawImage(int star_x, int star_y, const Image* image, int x1, int y1, int w, int h)
	{
		int left = tmax(star_x, 0);
		int top = tmax(star_y, 0);

		int right = tmin(star_x + w, _width);
		int bottom = tmin(star_y + h, _height);

		for (int x = left; x < right; ++x)
		{
			for (int y = top; y < bottom; ++y)
			{
				Rgba color = image->piexlAt(x - left + x1, y - top + y1);
				setPiexlEx(x, y, color);
			}
		}
	}

	void Raster::drawImageScale(int distX, int distY, int distW, int distH, const Image* image)
	{
		// 首先应该先得到我们图片宽高和我们要绘制画板的宽高的比例
		float xScale = static_cast<float>(image->width()) / static_cast<float>(distW);
		float yScale = static_cast<float>(image->height()) / static_cast<float>(distH);

		for (int x = distX; x < distX + distW; ++x)
		{
			for (int y = distY; y < distY + distH; ++y)
			{
				float xImage = (x - distX) * xScale;
				float yImage = (y - distY) * yScale;

				Rgba srcColor = image->piexlAt(xImage, yImage);
				setPiexlEx(x, y, srcColor);
			}
		}
	}

	// ----------------------------------------------------------
	void Raster::vertexPointer(int size, DATATYPE type, int stride, const void* pointer)
	{
		_positionPointer->_size = size;
		_positionPointer->_type = type;
		_positionPointer->_stride = stride;
		_positionPointer->_data = pointer;
	}

	void Raster::colorPointer(int size, DATATYPE type, int stride, const void* color)
	{
		_colorPointer->_size = size;
		_colorPointer->_type = type;
		_colorPointer->_stride = stride;
		_colorPointer->_data = color;
	}

	void Raster::textureCoordPointer(int size, DATATYPE type, int stride, const void* uv)
	{
		_uvPointer->_size = size;
		_uvPointer->_type = type;
		_uvPointer->_stride = stride;
		_uvPointer->_data = uv;
	}

	void Raster::drawArrays(DRAWMODE pri, int start, int count)
	{
		if (!_positionPointer->_data)
		{
			return;
		}
		char* pos = (char*)_positionPointer->_data;
		float* posData = (float*)pos;
		int2 p0(posData[0], posData[1]);
		posData += _positionPointer->_stride;
		int2 p1(posData[0], posData[1]);
		posData += _positionPointer->_stride;
		int2 p2(posData[0], posData[1]);

		char* color = (char*)_colorPointer->_data;
		Rgba* corData = (Rgba*)color;
		Rgba c0(corData[0]);
		corData += _colorPointer->_stride;
		Rgba c1(corData[0]);
		corData += _colorPointer->_stride;
		Rgba c2(corData[0]);

		char* tex = (char*)_uvPointer->_data;
		float* uvData = (float*)tex;
		float2 uv0(uvData[0], uvData[1]);
		uvData += _uvPointer->_stride;
		float2 uv1(uvData[0], uvData[1]);
		uvData += _uvPointer->_stride;
		float2 uv2(uvData[0], uvData[1]);

		Edge edges[] = {
				Edge(p0.x, p0.y, p1.x, p1.y, c0, c1, uv0, uv1),
				Edge(p1.x, p1.y, p2.x, p2.y, c1, c2, uv1, uv2),
				Edge(p2.x, p2.y, p0.x, p0.y, c2, c0, uv2, uv0)
		};

		drawTriggle(edges, nullptr);
	}
}