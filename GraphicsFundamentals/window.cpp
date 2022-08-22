#include <windows.h>
#include <tchar.h>
#include "Raster.h"
#include "CELLMath.hpp"

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
		1000,																													// 设置窗口宽
		500,																													// 设置窗口高
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

	// 画一个点
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
		CELL::float2 arPointBzier[] = {
			CELL::float2(150, 50),
			CELL::float2(20, 160),
			CELL::float2(180, 100),
			CELL::float2(170, 210),
		};
		// 贝塞尔曲线公式
		//B(t) - Po(1 - t) + 3P1t(1 - t) + 3P2t(1 - t) + P3t。，t∈[0，1]
		CELL::float2 grev[2];
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
		}

		// 绘制矩形
		raster.drawFillRect(45, 45, 100, 87);

		// 直接让 raster 使用我们创建好的buffer，就可以省去这里进行buffer拷贝的过程
		//memcpy(buffer, raster._buffer, raster.getLength() * sizeof(CELL::Rgba));
		BitBlt(hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY);
	}

	return 0;
}