//DXTest.cpp: 定义应用程序的入口点。
//

#include "stdafx.h"
#include "main.h"

#include <map>
#include <vector>
#include <list>

// 忽略未引用变量的警告
#pragma warning(disable:4101)

using namespace DXTest;

namespace DXTest {



	WCHAR szTitle[100] = L"标题";                  // 标题栏文本
	WCHAR szWindowClass[100] = L"DXTestClass";            // 主窗口类名

	/**
	 * \brief 窗口类的接口
	 */
	class IWindow
	{
	public:

	protected:
		static WNDCLASS wndClass_;
	};

	/**
	 * \brief 应用程序接口,对 HINSTANCE 的封装
	 */
	class IApplication
	{
	public:
		IApplication() {};
		IApplication(LPWSTR) {};
		virtual ~IApplication() = default;

		virtual HINSTANCE getInstance() = 0;

		virtual IWindow createWindow() = 0;
	};

	/**
	 * \brief 应用程序接口的实现
	 */
	class Application :IApplication
	{
	public:
		Application()
		{
			hInstance_ = GetModuleHandleW(nullptr);
		}
		Application(LPWSTR lpCmdLine) :
			IApplication(lpCmdLine)
		{
			Application();
		}
		virtual ~Application()
		{
		}
		HINSTANCE getInstance() override
		{
			return hInstance_;
		}
		IWindow createWindow()
		{

		}
	protected:
		static HINSTANCE hInstance_;
	};

	

	


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

		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

		wcex.lpszClassName = szWindowClass;


		return RegisterClassExW(&wcex);
	}


	//   目的: 保存实例句柄并创建主窗口
	BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
	{
		//hInst = hInstance; // 将实例句柄存储在全局变量中

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

	int run()
	{
		// 窗口消息循环
		MSG msg;
		while (GetMessageW(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		return  static_cast<int>(msg.wParam);
	}

}






int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HMODULE module = GetModuleHandleW(NULL);
	if (module == hInstance)
	{
		MessageBoxW(NULL, L"==", NULL, NULL);
	}

	/*Application  app(hInstance);
	app.attack(hInstance, lpCmdLine)
		Window wnd = app.CreateWindow(WNDCLASS wndcls);
	wnd.show(nCmdShow);
*/


	return 0;//run();

}

