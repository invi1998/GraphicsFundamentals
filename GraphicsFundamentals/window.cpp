#include <windows.h>
#include <tchar.h>

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
		0,																														// 设置窗口右上角的位置 x
		0,																														// 设置窗口右上角的位置 y
		480,																													// 设置窗口宽
		420,																													// 设置窗口高
		0,																														// 如果有父窗口填父窗口的句柄，没有就取NULL
		0,																														// 指向附属窗口的句柄
		hInstance,																											// 这是应用程序的实例。这里使用WinMain()中第一个实参，hinstance句柄
		0																														// 设为NULL即可。用户自定义的变量（开发人员指定）
	);

	// 调用WindowsAPI就可以更新这个窗口，并显示这个窗口
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	// windows消息循环
	MSG msg = { 0 };
	while (true)
	{
		if (msg.message == WM_DESTROY || msg.message == WM_CLOSE || msg.message == WM_QUIT) {
			break;
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return 0;
}