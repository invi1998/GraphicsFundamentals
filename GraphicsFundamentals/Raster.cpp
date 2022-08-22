#include "Raster.h"

namespace CELL {
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

	void Raster::drawLine(int start_x, int end_x, Rgba color1, Rgba color2)
	{
		float length = tmax(end_x - start_x, 1);

		for (int x = start_x; x <= end_x; ++x)
		{
			Rgba color = colorLerp(color1, color2, (x - start_x) / length);
			setPiexl()
		}
	}
}