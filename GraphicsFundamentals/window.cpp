#include <windows.h>
#include <tchar.h>

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
		0,																														// ���ô������Ͻǵ�λ�� x
		0,																														// ���ô������Ͻǵ�λ�� y
		480,																													// ���ô��ڿ�
		420,																													// ���ô��ڸ�
		0,																														// ����и���������ڵľ����û�о�ȡNULL
		0,																														// ָ�������ڵľ��
		hInstance,																											// ����Ӧ�ó����ʵ��������ʹ��WinMain()�е�һ��ʵ�Σ�hinstance���
		0																														// ��ΪNULL���ɡ��û��Զ���ı�����������Աָ����
	);

	// ����WindowsAPI�Ϳ��Ը���������ڣ�����ʾ�������
	UpdateWindow(hWnd);
	ShowWindow(hWnd, SW_SHOW);

	// windows��Ϣѭ��
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