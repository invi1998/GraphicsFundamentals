#include <windows.h>
#include <tchar.h>
#include "Raster.h"
#include "CELLMath.hpp"
//#include "CELLTimestamp.h"
//#include <FreeImage.h>

//#define _CRT_SECURE_NO_WARNINGS
// 在Windows下创建一个窗口

LRESULT CALLBACK windowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_SIZE:
		break;
	case WM_CLOSE:
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// $(Configuration)/temp/$(Platform)/$(ProjectName)\
//

// HINSTANCE hInstance,			当前实例的句柄（Windows里h开头的基本上都叫句柄，可以理解为指针）
// HINSTANCE hPrevInstance,	上一个实例的句柄
// LPSTR lpCmdLine,				命令行（我们给这个exe程序所传递的一些信息）
// int nShowCmd						显示命令（是否显示窗口）

void getResourcePath(HINSTANCE hInstance, char pPath[1024])
{
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], file[_MAX_FNAME], ext[_MAX_EXT];
	GetModuleFileName(hInstance, path_buffer, sizeof(path_buffer));
	//_splitpath_s(szPathName, szDriver, szPath, 0, 0);
	_splitpath_s(path_buffer, drive, dir, file, ext);
	sprintf_s(pPath, 1024, "%s%s", drive, dir);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// 1：注册一个窗口类
	::WNDCLASSEXA winClass;
	winClass.lpszClassName = "Raster";																			//指向类名称的指针，窗口类名称，告诉操作系统，我要注册这样一个窗口类，注册完之后，我们的操作系统中就保存了这样一个名称对应的窗口结构体的内容。
	winClass.cbSize = sizeof(::WNDCLASSEX);																//WNDCLASSEX 的大小。我们可以用sizeof（WNDCLASSEX）来获得准确的值
	winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;			//窗口风格
	winClass.lpfnWndProc = windowProc;																	//窗口的消息处理函数
	winClass.hInstance = hInstance;																				//当前应用程序实例句柄
	winClass.hIcon = NULL;																							//窗口的最小化图标为缺省图标
	winClass.hIconSm = NULL;																						//和窗口类关联的小图标。如果该值为NULL。则把hIcon中的图标转换成大小合适的小图标
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);												//窗口采用箭头光标
	winClass.hbrBackground = (HBRUSH)(WHITE_BRUSH);											//窗口背景色为白色
	winClass.lpszMenuName = NULL;																			//无窗口菜单
	winClass.cbClsExtra = 0;																							//窗口类无扩展
	winClass.cbWndExtra = 0;																						//窗口实例无扩展

	RegisterClassExA(&winClass);																					//注册窗口

	// 2： 创建窗口
	// 窗口创建完成之后会返回一个Windows句柄
	HWND hWnd = CreateWindowExA(
		NULL,
		"Raster",																											// 设置之前的创建基础类名，
		"Raster",																											// 窗口的名称
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,			// 这是一个说明窗口外观的通用标志
	   //WS_POPUPWINDOW,																						// 不要标题栏
		800,																													// 设置窗口右上角的位置 x
		50,																													// 设置窗口右上角的位置 y
		2000,																													// 设置窗口宽
		2000,																													// 设置窗口高
		0,																														// 如果有父窗口填父窗口的句柄，没有就取NULL
		0,																														// 指向附属窗口的句柄
		hInstance,																											// 这是应用程序的实例。这里使用WinMain()中第一个实参，hinstance句柄
		0																														// 设为NULL即可。用户自定义的变量（开发人员指定）
	);

	// 调用WindowsAPI就可以更新这个窗口，并显示这个窗口
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	// 得到窗口大小（得到当前客户区的窗口大小，客户区就是除了标题栏以外的区域），传入一个窗口句柄和一个RECT结构体指针，将窗口信息输出到这个指针结构体中
	RECT rt = { 0 };
	GetClientRect(hWnd, &rt);

	int width = rt.right - rt.left;
	int height = rt.bottom - rt.top;
	void* buffer = nullptr;

	// windows里绘制上下文（绘制）对象 就用这个DC，
	// 创建一个DC（当前窗口的DC）
	HDC hDC = GetDC(hWnd);
	// 创建一个内存DC（跟当前窗口兼容的DC）
	HDC hMem = ::CreateCompatibleDC(hDC);

	// 4通道（rgba）32位图
	//typedef int color;
	//color buffer[width][height];

	// Windows绘图基础
	BITMAPINFO bmpInfor;
	bmpInfor.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfor.bmiHeader.biWidth = width;
	bmpInfor.bmiHeader.biHeight = height;
	bmpInfor.bmiHeader.biPlanes = 1;
	bmpInfor.bmiHeader.biBitCount = 32;						// 一个像素占32个比特位
	bmpInfor.bmiHeader.biCompression = BI_RGB;
	bmpInfor.bmiHeader.biSizeImage = 0;
	bmpInfor.bmiHeader.biXPelsPerMeter = 0;
	bmpInfor.bmiHeader.biYPelsPerMeter = 0;
	bmpInfor.bmiHeader.biClrUsed = 0;
	bmpInfor.bmiHeader.biClrImportant = 0;

	// 创建位图
	// 这里这个函数会返回一个buffer，这个buffer就是输出，这个buffer就是指的是我们RGBA颜色值得缓冲区的内存地址
	HBITMAP hBmp = CreateDIBSection(hDC, &bmpInfor, DIB_RGB_COLORS, (void**)&buffer, 0, 0);
	// 把位图放在内存DC上（把内存DC和我们的位图关联到一起）
	// 这个DC（hMem）就相当于一个画板，这个位图（hBmp）就相当于一张白纸，如果你想画画，就需要将纸放在画板上，这个放的过程就是
	// 这里的 SelectObject
	SelectObject(hMem, hBmp);
	// windows里画图不能直接把图片画在窗口上，必须把图片放在画板上，跟DC关联到一起才能绘制

	// 得到编译之后的exe文件所在的文件路径
	char imagPathBuf[1024]{};
	getResourcePath(0, imagPathBuf);

	char szImage[1024];
	sprintf_s(szImage, 1024, "%s/image/1.jpg", imagPathBuf);
	CELL::Image* image_s = CELL::Image::loadFromFile(szImage);

	sprintf_s(szImage, 1024, "%s/image/2.jpg", imagPathBuf);
	CELL::Image* colorKey = CELL::Image::loadFromFile(szImage);

	sprintf_s(szImage, 1024, "%s/image/3.png", imagPathBuf);
	CELL::Image* alphaImage = CELL::Image::loadFromFile(szImage);

	struct Vertex
	{
		float x, y;
		float u, v;
		CELL::Rgba color;
	};

	// 创建一个我们的绘图对象
	CELL::Raster raster(width, height, buffer);

	// windows消息循环
	MSG msg = { 0 };
	while (true)
	{
		if (msg.message == WM_DESTROY || msg.message == WM_CLOSE || msg.message == WM_QUIT) {
			// 当收到 缩小，全屏，退出 都执行退出，关闭窗口
			break;
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		memset(buffer, 255, width * height * 4);

		unsigned char* rgba = (unsigned char*)buffer;		// 将buffer转为char 类型
		int pitch = width * 4;												// 计算一行的大小（字节数）
		memset(rgba + pitch * 10, 135, pitch);					// 然后到第十行的时候，把值全改位135
		// 因为我们每隔像素占4个通道（rgba），占4个字节，那么一行就占 4*width
		// 因为是二维数组表示，所以第n行的起始地址就是 buffer + （width*n)

		raster.clear();
		// 直接让 raster 使用我们创建好的buffer，就可以省去这里进行buffer拷贝的过程
		//memcpy(buffer, raster._buffer, raster.getLength() * sizeof(CELL::Rgba));

		// 绘制100个随机点
		/*for (int i = 0; i < 100; ++i) {
			raster.drawPoint(rand() % 256, rand() % 256, CELL::Rgba(255, 0, 0), 3);
		}*/

		// 绘制直线
		//raster.drawLine(CELL::float2(100, 300), CELL::float2(0, 0), CELL::Rgba(200, 0, 210), CELL::Rgba(10, 255, 64));

		// 绘制直线2
		/*CELL::float2 arPoint[] = {
			CELL::float2(11, 210),
			CELL::float2(101, 45),
			CELL::float2(1, 110),
			CELL::float2(187, 0),
			CELL::float2(62, 190),
		};

		raster.drawArrays(CELL::DM_LINE_STRIP, arPoint, sizeof(arPoint) / sizeof(CELL::float2));*/

		// 画一个圆
		/*x = r * cos() + center.x;
		y = r * sin() + center.y;*/

		//CELL::float2 center(100, 100);		// 圆心
		//float radius = 80;						// 半径
		//CELL::float2 arCircle[360];

		//for (int i = 0; i < 360; ++i)
		//{
		//	arCircle[i].x = radius * cos(DEG2RAD(i)) + center.x;
		//	arCircle[i].y = radius * sin(DEG2RAD(i)) + center.y;
		//}
		//raster.drawArrays(CELL::DM_LINE_LOOP, arCircle, sizeof(arCircle) / sizeof(CELL::float2));

		// 绘制贝塞尔曲线
		/*CELL::float2 arPointBzier[] = {
			CELL::float2(150, 50),
			CELL::float2(20, 160),
			CELL::float2(180, 100),
			CELL::float2(170, 210),
		};*/
		// 贝塞尔曲线公式
		//B(t) - Po(1 - t) + 3P1t(1 - t) + 3P2t(1 - t) + P3t。，t∈[0，1]
		/*CELL::float2 grev[2];
		for (float t = 0.0f; t < 1.0f; t += 0.01f)
		{
			float x = arPointBzier[0].x * pow(1 - t, 3) * pow(t, 0)
				+ 3 * arPointBzier[1].x * pow(1 - t, 2) * pow(t, 1)
				+ 3 * arPointBzier[2].x * pow(1 - t, 1) * pow(t, 2)
				+ arPointBzier[3].x * pow(1 - t, 0) * pow(t, 3);

			float y = arPointBzier[0].y * pow(1 - t, 3) * pow(t, 0)
				+ 3 * arPointBzier[1].y * pow(1 - t, 2) * pow(t, 1)
				+ 3 * arPointBzier[2].y * pow(1 - t, 1) * pow(t, 2)
				+ arPointBzier[3].y * pow(1 - t, 0) * pow(t, 3);

			if (t == 0)
			{
				grev[0] = CELL::float2(x, y);
			}
			else
			{
				grev[1] = CELL::float2(x, y);
				raster.drawArrays(CELL::DM_LINES, grev, 2);
				grev[0] = grev[1];
			}
		}*/

		// 绘制矩形
		//raster.drawFillRect(-145, -145, 1000, 587);

		// 绘制渐变矩形
		/*CELL::int2 points[] = {
			CELL::int2(10, 10),
			CELL::int2(800, 10),
			CELL::int2(800, 430),
			CELL::int2(10, 430)
		};

		CELL::Rgba colors[] = {
			CELL::Rgba(22, 45, 210),
			CELL::Rgba(122, 225, 20),
			CELL::Rgba(212, 15, 180),
			CELL::Rgba(252, 84, 44)
		};

		raster.drawRect(points, colors);*/

		// 绘制不规则三角形
		//CELL::int2 pt[3] = {
		//	CELL::int2(-100, 20),
		//	CELL::int2(450, 600),
		//	CELL::int2(700, 120),
		//};
		//CELL::Rgba colorTraggle[3] = {
		//	CELL::Rgba(240, 78, 121),
		//	CELL::Rgba(20, 178, 11),
		//	CELL::Rgba(45, 30, 111)
		//};

		//CELL::CELLTimestamp tms;

		//tms.update();
		///*for (int i = 0; i < 1000; ++i)
		//{
		//}*/
		//raster.drawTriggle(pt[0], pt[1], pt[2], colorTraggle[0], colorTraggle[1], colorTraggle[2]);

		//double mis = tms.getElapsedTimeInMicroSec();
		//// 优化前绘制一次					mis	3175.2772000122072	double
		//// 优化差值						mis	3212.1111000061037	double
		//// 优化减法和除法（改为一次加法）	mis	3002.5886000061037	double

		//char szBuf[128];
		//sprintf_s(szBuf, "%f", mis);

		//TextOut(hMem, width - 100, 10, szBuf, strlen(szBuf));

		// --------------------------
		// 绘制随机像素的图片
		//raster.drawImage(100, 100, 100, 100);

		// 绘制FreeImage加载进来的图片
		//raster.drawImage(0, 0, image_s);

		// 绘制图片2
		//raster.drawImage(0, 0, colorKey, 1500, 1500, 500, 500);

		// colorKey 剔除颜色
		//raster.drawImageWidthColorKey(0, 0, colorKey, CELL::Rgba(0, 0, 0));

		// alpha 测试
		//raster.drawImageAlphaTest(0, 0, alphaImage, 100);

		// alpha 混合
		//raster.drawImageAlphaBlend(0, 0, alphaImage, 0.8f);

		// alpha 透明
		//raster.drawImageAlpha(0, 0, alphaImage, 0.5f);

		// 最近点采样（图片缩放）
		// 1080 * 1512 ---> 540, 756
		//raster.drawImageScale(100, 100, 540, 756, image_s);
		// 1080 * 1512 ---> 1620, 2268
		//raster.drawImageScale(100, 0, 1620, 1512, image_s);

		// 通过UV坐标绘制三角形纹理
		/*CELL::Raster::Vertex vertex = {
			CELL::int2(0, 0),CELL::int2(1000, 2000),CELL::int2(2000, 0),
			CELL::float2(0.0f, 0.0f),CELL::float2(0.5f, 1.0f),CELL::float2(1.0f, 0.0f),
			CELL::Rgba(0, 163, 233),CELL::Rgba(255, 175, 202),CELL::Rgba(255, 242, 0)
		};
		raster.drawTriggle(vertex, image_s);*/

		//static float speet = 0.0f;

		Vertex vertexs[6] = {
			{
				0, 0, 0.0f, 0.0f, CELL::Rgba(231, 199, 10)
			},  {
				2000, 0, 2.0f, 0.0f, CELL::Rgba(21, 19, 45)
			}, {
				0, 2000, 0.0f, 2.0f, CELL::Rgba(121, 94, 110)
			},
			{
				0, 2000, 0.0f, 2.0f, CELL::Rgba(231, 199, 10)
			},  {
				2000, 2000, 2.0f, 2.0f, CELL::Rgba(21, 19, 45)
			}, {
				2000, 0, 2.0f, 0.0f, CELL::Rgba(121, 94, 110)
			},
		};

		/*for (int i = 0; i < 6; ++i)
		{
			vertexs[i].u += speet;
		}

		speet += 0.1f;*/

		// 平移矩阵的应用
		static float trans = 0.0f;
		CELL::matrix3 mat_t;
		mat_t.translate(2000, 2000);

		//trans += 1;

		// 缩放矩阵的应用
		static float scale = -1.0f;
		CELL::matrix3 mat_s;
		mat_s.scale(scale, scale);
		//scale += 0.1f;

		for (int i = 0; i < 6; ++i)
		{
			CELL::float3 pos(vertexs[i].x, vertexs[i].y, 1);
			pos = mat_s * pos;
			vertexs[i].x = pos.x;
			vertexs[i].y = pos.y;
		}

		for (int i = 0; i < 6; ++i)
		{
			CELL::float3 pos(vertexs[i].x, vertexs[i].y, 1);
			pos = mat_t * pos;
			vertexs[i].x = pos.x;
			vertexs[i].y = pos.y;
		}

		// 指定纹理包装类型
		image_s->setWrapType(1);

		// 指定图片
		raster.bindTexture(image_s);
		// 指定顶点
		raster.vertexPointer(2, CELL::DT_FLOAT, sizeof(Vertex), &vertexs[0].x);
		// 指定uv
		raster.textureCoordPointer(2, CELL::DT_FLOAT, sizeof(Vertex), &vertexs[0].u);
		// 指定颜色
		//raster.colorPointer(4, CELL::DT_BYTE, sizeof(Vertex), &vertexs[0].color);

		raster.drawArrays(CELL::DM_TRIANGLES, 0, 6);

		BitBlt(hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY);
	}

	//delete[] image_s;

	return 0;
}