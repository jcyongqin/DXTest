//DXTest.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "DXTest.h"


namespace DXTest {

	// 全局变量: 
	HINSTANCE hInst;                                // 当前实例
	WCHAR szTitle[100] = L"标题";                  // 标题栏文本
	WCHAR szWindowClass[100] = L"DXTestClass";            // 主窗口类名

	/**
	 * \brief 注册窗口类
	 * \param hInstance
	 * \return
	 */
	ATOM WndClassRegister(HINSTANCE hInstance, WNDCLASSEXW wcex = {}, WNDPROC WndProc = nullptr)
	{


		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DXTEST));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DXTEST);
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassExW(&wcex);
	}


	//   目的: 保存实例句柄并创建主窗口
	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
	{
		hInst = hInstance; // 将实例句柄存储在全局变量中

		HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

		if (!hWnd)
		{
			return FALSE;
		}

		ShowWindow(hWnd, nCmdShow);
		UpdateWindow(hWnd);

		return TRUE;
	}


	//  目的:    处理主窗口的消息。
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_COMMAND: 
		{
			int wmId = LOWORD(wParam);
			// 分析菜单选择: 
			switch (wmId)
			{

			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			SetTextColor(hdc, RGB(255, 0, 0));
			SetBkColor(hdc, RGB(0, 255, 0));
			TextOut(hdc, 200, 200, L"Hello World!", strlen("Hello World!"));
			EndPaint(hWnd, &ps);

		}
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}


}

using namespace DXTest;

extern CComModule _Module;

class CMyCustomWnd : public CWindowImpl<CMyCustomWnd>
{
public:
	// Optionally specify name of the new Windows class
	DECLARE_WND_CLASS(_T("MyName"))
	// If this macro is not specified in your
	// class, ATL will generate a class name

	BEGIN_MSG_MAP(CMyCustomWnd)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)

	END_MSG_MAP()

	LRESULT OnCreate(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// 创建 ID2D1Factory
		ID2D1Factory* pD2DFactory = NULL;
		HRESULT hr = D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
			&pD2DFactory
		);
		return 0;
	}

	LRESULT OnPaint(UINT /*nMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// Do some painting code
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(&ps);
		// TODO: 在此添加任意绘图代码...
		SetTextColor(hdc, RGB(255, 0, 0));
		SetBkColor(hdc, RGB(0, 255, 0));
		TextOut(hdc, 200, 200, L"Hello World!", strlen("Hello World!"));
		EndPaint(&ps);

		return 0;
	}

};



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	auto myWindow = CMyCustomWnd();

	myWindow.Create(NULL);
	myWindow.ShowWindow(nCmdShow);
	myWindow.UpdateWindow();

	// 窗口消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
	
}

/*
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {

	// 注册窗口类
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_HAND);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(BLACK_BRUSH));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"mywndclass";
	wcex.hIconSm = NULL;
	RegisterClassEx(&wcex);

	// 创建窗口
	HWND hWnd = CreateWindowEx(0, L"mywndclass", L"This is a win32 wnd", WS_OVERLAPPEDWINDOW,
		100, 100, 800, 600, NULL, NULL, hInstance, NULL);

	// 显示窗口
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);




	// 创建 ID2D1Factory
	ID2D1Factory* pD2DFactory = NULL;
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&pD2DFactory
	);

	// 创建呈现器目标
	// Obtain the size of the drawing area.
	RECT rc;
	GetClientRect(hwnd, &rc);

	// Create a Direct2D render target
	ID2D1HwndRenderTarget* pRT = NULL;
	hr = pD2DFactory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(
			hwnd,
			D2D1::SizeU(
				rc.right - rc.left,
				rc.bottom - rc.top)
		),
		&pRT
	);




	// 窗口消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

*/