#pragma once

namespace CELL
{
	class Image
	{
	protected:
		int _width;
		int _height;

		unsigned int* _pixel;
	public:
		Image(int w, int h, void* data)
		{
			if (w == 0 || h == 0 || data == nullptr)
			{
				_width = 0;
				_height = 0;
				_pixel = nullptr;
			}
			else
			{
				_width = w;
				_height = h;
				_pixel = new unsigned int[w * h];
				memcpy_s(_pixel, w * h * sizeof(unsigned int), data, w * h * sizeof(unsigned int));
			}
		}

		~Image()
		{
			delete[] _pixel;
		}

		int width() const
		{
			return _width;
		}

		int height() const
		{
			return _height;
		}

		unsigned int* pixel() const
		{
			return _pixel;
		}

	public:
		static Image* loadFromFile(const char* fileName);

		Rgba piexlAt(int x, int y) const
		{
			return Rgba(_pixel[y * _width + x]);
		}

		// 根据UV坐标查找图片对应的颜色值
		Rgba piexUV(float u, float v)
		{
			// 把UV坐标转为像素坐标
			float x = u * _width;
			float y = v * _height;

			return piexlAt((unsigned)(x) % _width, (unsigned)(y) % _height);
		}
	};
}