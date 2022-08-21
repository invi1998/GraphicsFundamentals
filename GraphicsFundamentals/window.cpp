#include <windows.h>
#include <tchar.h>
#include "Raster.h"
#include "CELLMath.hpp"

// ��Windows�´���һ������

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

// HINSTANCE hInstance,			��ǰʵ���ľ����Windows��h��ͷ�Ļ����϶��о�����������Ϊָ�룩
// HINSTANCE hPrevInstance,	��һ��ʵ���ľ��
// LPSTR lpCmdLine,				�����У����Ǹ����exe���������ݵ�һЩ��Ϣ��
// int nShowCmd						��ʾ����Ƿ���ʾ���ڣ�

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	// 1��ע��һ��������
	::WNDCLASSEXA winClass;
	winClass.lpszClassName = "Raster";																			//ָ�������Ƶ�ָ�룬���������ƣ����߲���ϵͳ����Ҫע������һ�������࣬ע����֮�����ǵĲ���ϵͳ�оͱ���������һ�����ƶ�Ӧ�Ĵ��ڽṹ������ݡ�
	winClass.cbSize = sizeof(::WNDCLASSEX);																//WNDCLASSEX �Ĵ�С�����ǿ�����sizeof��WNDCLASSEX�������׼ȷ��ֵ
	winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;			//���ڷ��
	winClass.lpfnWndProc = windowProc;																	//���ڵ���Ϣ������
	winClass.hInstance = hInstance;																				//��ǰӦ�ó���ʵ�����
	winClass.hIcon = NULL;																							//���ڵ���С��ͼ��Ϊȱʡͼ��
	winClass.hIconSm = NULL;																						//�ʹ����������Сͼ�ꡣ�����ֵΪNULL�����hIcon�е�ͼ��ת���ɴ�С���ʵ�Сͼ��
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);												//���ڲ��ü�ͷ���
	winClass.hbrBackground = (HBRUSH)(WHITE_BRUSH);											//���ڱ���ɫΪ��ɫ
	winClass.lpszMenuName = NULL;																			//�޴��ڲ˵�
	winClass.cbClsExtra = 0;																							//����������չ
	winClass.cbWndExtra = 0;																						//����ʵ������չ

	RegisterClassExA(&winClass);																					//ע�ᴰ��

	// 2�� ��������
	// ���ڴ������֮��᷵��һ��Windows���
	HWND hWnd = CreateWindowExA(
		NULL,
		"Raster",																											// ����֮ǰ�Ĵ�������������
		"Raster",																											// ���ڵ�����
		// WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,			// ����һ��˵��������۵�ͨ�ñ�־
		WS_POPUPWINDOW,																						// ��Ҫ������
		800,																													// ���ô������Ͻǵ�λ�� x
		50,																													// ���ô������Ͻǵ�λ�� y
		256,																													// ���ô��ڿ�
		256,																													// ���ô��ڸ�
		0,																														// ����и���������ڵľ����û�о�ȡNULL
		0,																														// ָ�������ڵľ��
		hInstance,																											// ����Ӧ�ó����ʵ��������ʹ��WinMain()�е�һ��ʵ�Σ�hinstance���
		0																														// ��ΪNULL���ɡ��û��Զ���ı�����������Աָ����
	);

	// ����WindowsAPI�Ϳ��Ը���������ڣ�����ʾ�������
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	// �õ����ڴ�С���õ���ǰ�ͻ����Ĵ��ڴ�С���ͻ������ǳ��˱�������������򣩣�����һ�����ھ����һ��RECT�ṹ��ָ�룬��������Ϣ��������ָ��ṹ����
	RECT rt = { 0 };
	GetClientRect(hWnd, &rt);

	int width = rt.right - rt.left;
	int height = rt.bottom - rt.top;
	void* buffer = nullptr;

	// windows����������ģ����ƣ����� �������DC��
	// ����һ��DC����ǰ���ڵ�DC��
	HDC hDC = GetDC(hWnd);
	// ����һ���ڴ�DC������ǰ���ڼ��ݵ�DC��
	HDC hMem = ::CreateCompatibleDC(hDC);

	// 4ͨ����rgba��32λͼ
	//typedef int color;
	//color buffer[width][height];

	// Windows��ͼ����
	BITMAPINFO bmpInfor;
	bmpInfor.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfor.bmiHeader.biWidth = width;
	bmpInfor.bmiHeader.biHeight = height;
	bmpInfor.bmiHeader.biPlanes = 1;
	bmpInfor.bmiHeader.biBitCount = 32;						// һ������ռ32������λ
	bmpInfor.bmiHeader.biCompression = BI_RGB;
	bmpInfor.bmiHeader.biSizeImage = 0;
	bmpInfor.bmiHeader.biXPelsPerMeter = 0;
	bmpInfor.bmiHeader.biYPelsPerMeter = 0;
	bmpInfor.bmiHeader.biClrUsed = 0;
	bmpInfor.bmiHeader.biClrImportant = 0;

	// ����λͼ
	// ������������᷵��һ��buffer�����buffer������������buffer����ָ��������RGBA��ɫֵ�û��������ڴ��ַ
	HBITMAP hBmp = CreateDIBSection(hDC, &bmpInfor, DIB_RGB_COLORS, (void**)&buffer, 0, 0);
	// ��λͼ�����ڴ�DC�ϣ����ڴ�DC�����ǵ�λͼ������һ��
	// ���DC��hMem�����൱��һ�����壬���λͼ��hBmp�����൱��һ�Ű�ֽ��������뻭��������Ҫ��ֽ���ڻ����ϣ�����ŵĹ��̾���
	// ����� SelectObject
	SelectObject(hMem, hBmp);
	// windows�ﻭͼ����ֱ�Ӱ�ͼƬ���ڴ����ϣ������ͼƬ���ڻ����ϣ���DC������һ����ܻ���

	// ��һ����
	CELL::Raster raster(width, height, buffer);

	// windows��Ϣѭ��
	MSG msg = { 0 };
	while (true)
	{
		if (msg.message == WM_DESTROY || msg.message == WM_CLOSE || msg.message == WM_QUIT) {
			// ���յ� ��С��ȫ�����˳� ��ִ���˳����رմ���
			break;
		}

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		memset(buffer, 255, width * height * 4);

		unsigned char* rgba = (unsigned char*)buffer;		// ��bufferתΪchar ����
		int pitch = width * 4;												// ����һ�еĴ�С���ֽ�����
		memset(rgba + pitch * 10, 135, pitch);					// Ȼ�󵽵�ʮ�е�ʱ�򣬰�ֵȫ��λ135
		// ��Ϊ����ÿ������ռ4��ͨ����rgba����ռ4���ֽڣ���ôһ�о�ռ 4*width
		// ��Ϊ�Ƕ�ά�����ʾ�����Ե�n�е���ʼ��ַ���� buffer + ��width*n)

		raster.clear();

		// ����100�������
		/*for (int i = 0; i < 100; ++i) {
			raster.drawPoint(rand() % 256, rand() % 256, CELL::Rgba(255, 0, 0), 3);
		}*/

		// ����ֱ��
		raster.drawLine(CELL::float2(100, 300), CELL::float2(0, 0), CELL::Rgba(200, 0, 0));

		// ֱ���� raster ʹ�����Ǵ����õ�buffer���Ϳ���ʡȥ�������buffer�����Ĺ���
		//memcpy(buffer, raster._buffer, raster.getLength() * sizeof(CELL::Rgba));
		BitBlt(hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY);
	}

	return 0;
}