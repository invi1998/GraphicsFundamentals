#include <windows.h>
#include <tchar.h>
#include "Raster.h"
#include "CELLMath.hpp"
//#include "CELLTimestamp.h"
//#include <FreeImage.h>

//#define _CRT_SECURE_NO_WARNINGS
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

void getResourcePath(HINSTANCE hInstance, char pPath[1024])
{
	char path_buffer[_MAX_PATH], drive[_MAX_DRIVE], dir[_MAX_DIR], file[_MAX_FNAME], ext[_MAX_EXT];
	GetModuleFileName(hInstance, path_buffer, sizeof(path_buffer));
	//_splitpath_s(szPathName, szDriver, szPath, 0, 0);
	_splitpath_s(path_buffer, drive, dir, file, ext);
	sprintf_s(pPath, 1024, "%s%s", drive, dir);
}

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
		2000,																													// ���ô��ڿ�
		2000,																													// ���ô��ڸ�
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

	// �õ�����֮���exe�ļ����ڵ��ļ�·��
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

	// ����һ�����ǵĻ�ͼ����
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
		// ֱ���� raster ʹ�����Ǵ����õ�buffer���Ϳ���ʡȥ�������buffer�����Ĺ���
		//memcpy(buffer, raster._buffer, raster.getLength() * sizeof(CELL::Rgba));

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
		/*CELL::float2 arPointBzier[] = {
			CELL::float2(150, 50),
			CELL::float2(20, 160),
			CELL::float2(180, 100),
			CELL::float2(170, 210),
		};*/
		// ���������߹�ʽ
		//B(t) - Po(1 - t) + 3P1t(1 - t) + 3P2t(1 - t) + P3t����t��[0��1]
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

		// ���ƾ���
		//raster.drawFillRect(-145, -145, 1000, 587);

		// ���ƽ������
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

		// ���Ʋ�����������
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
		//// �Ż�ǰ����һ��					mis	3175.2772000122072	double
		//// �Ż���ֵ						mis	3212.1111000061037	double
		//// �Ż������ͳ�������Ϊһ�μӷ���	mis	3002.5886000061037	double

		//char szBuf[128];
		//sprintf_s(szBuf, "%f", mis);

		//TextOut(hMem, width - 100, 10, szBuf, strlen(szBuf));

		// --------------------------
		// ����������ص�ͼƬ
		//raster.drawImage(100, 100, 100, 100);

		// ����FreeImage���ؽ�����ͼƬ
		//raster.drawImage(0, 0, image_s);

		// ����ͼƬ2
		//raster.drawImage(0, 0, colorKey, 1500, 1500, 500, 500);

		// colorKey �޳���ɫ
		//raster.drawImageWidthColorKey(0, 0, colorKey, CELL::Rgba(0, 0, 0));

		// alpha ����
		//raster.drawImageAlphaTest(0, 0, alphaImage, 100);

		// alpha ���
		//raster.drawImageAlphaBlend(0, 0, alphaImage, 0.8f);

		// alpha ͸��
		//raster.drawImageAlpha(0, 0, alphaImage, 0.5f);

		// ����������ͼƬ���ţ�
		// 1080 * 1512 ---> 540, 756
		//raster.drawImageScale(100, 100, 540, 756, image_s);
		// 1080 * 1512 ---> 1620, 2268
		//raster.drawImageScale(100, 0, 1620, 1512, image_s);

		// ͨ��UV�����������������
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

		// ƽ�ƾ����Ӧ��
		static float trans = 0.0f;
		CELL::matrix3 mat_t;
		mat_t.translate(2000, 2000);

		//trans += 1;

		// ���ž����Ӧ��
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

		// ָ�������װ����
		image_s->setWrapType(1);

		// ָ��ͼƬ
		raster.bindTexture(image_s);
		// ָ������
		raster.vertexPointer(2, CELL::DT_FLOAT, sizeof(Vertex), &vertexs[0].x);
		// ָ��uv
		raster.textureCoordPointer(2, CELL::DT_FLOAT, sizeof(Vertex), &vertexs[0].u);
		// ָ����ɫ
		//raster.colorPointer(4, CELL::DT_BYTE, sizeof(Vertex), &vertexs[0].color);

		raster.drawArrays(CELL::DM_TRIANGLES, 0, 6);

		BitBlt(hDC, 0, 0, width, height, hMem, 0, 0, SRCCOPY);
	}

	//delete[] image_s;

	return 0;
}