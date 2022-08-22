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
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,			// ����һ��˵��������۵�ͨ�ñ�־
	   //WS_POPUPWINDOW,																						// ��Ҫ������
		800,																													// ���ô������Ͻǵ�λ�� x
		50,																													// ���ô������Ͻǵ�λ�� y
		1000,																													// ���ô��ڿ�
		500,																													// ���ô��ڸ�
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
		//raster.drawLine(CELL::float2(100, 300), CELL::float2(0, 0), CELL::Rgba(200, 0, 210), CELL::Rgba(10, 255, 64));

		// ����ֱ��2
		/*CELL::float2 arPoint[] = {
			CELL::float2(11, 210),
			CELL::float2(101, 45),
			CELL::float2(1, 110),
			CELL::float2(187, 0),
			CELL::float2(62, 190),
		};

		raster.drawArrays(CELL::DM_LINE_STRIP, arPoint, sizeof(arPoint) / sizeof(CELL::float2));*/

		// ��һ��Բ
		/*x = r * cos() + center.x;
		y = r * sin() + center.y;*/

		//CELL::float2 center(100, 100);		// Բ��
		//float radius = 80;						// �뾶
		//CELL::float2 arCircle[360];

		//for (int i = 0; i < 360; ++i)
		//{
		//	arCircle[i].x = radius * cos(DEG2RAD(i)) + center.x;
		//	arCircle[i].y = radius * sin(DEG2RAD(i)) + center.y;
		//}
		//raster.drawArrays(CELL::DM_LINE_LOOP, arCircle, sizeof(arCircle) / sizeof(CELL::float2));

		// ���Ʊ���������
		CELL::float2 arPointBzier[] = {
			CELL::float2(150, 50),
			CELL::float2(20, 160),
			CELL::float2(180, 100),
			CELL::float2(170, 210),
		};
		// ���������߹�ʽ
		//B(t) - Po(1 - t) + 3P1t(1 - t) + 3P2t(1 - t) + P3t����t��[0��1]
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

		// ���ƾ���
		raster.drawFillRect(45, 45, 100, 87);

		// ֱ���� raster ʹ�����Ǵ����õ�buffer���Ϳ���ʡȥ�������buffer�����Ĺ���
		//memcpy(buffer, raster._buffer, raster.getLength() * sizeof(CELL::Rgba));
		BitBlt(hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY);
	}

	return 0;
}