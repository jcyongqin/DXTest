#pragma once
#include <functional>

namespace BlankWindow
{
	class Window
	{
	private:
		HWND hWnd;
		HINSTANCE hInstance;
	public:
		Window() = default;
		Window(HINSTANCE Inst) :hWnd(nullptr), hInstance(Inst) {}

		static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
			Window* window = reinterpret_cast<Window*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));

			if (uMsg==WM_NCCREATE)
			{
				window= reinterpret_cast<Window*>(LPCREATESTRUCTW(lParam)->lpCreateParams);
				window->attach(hWnd);
			}

			if (window == nullptr)
			{
				return DefWindowProcW(hWnd, uMsg, wParam, lParam);
			}
			return window->dynamicWndProc(uMsg, wParam, lParam);
		}


		LRESULT dynamicWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			RECT rc;
			switch (uMsg)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_KEYDOWN:
				if (wParam == VK_ESCAPE)
				{
					DestroyWindow(hWnd);
				}
				break;
			case WM_CREATE:
				//SetWindowPos(hWnd, HWND_TOPMOST, 300, 300, 480, 360, FALSE);.
				rc.bottom = 123;
				//MoveWindow(hWnd, 300, 300, 480, 360, TRUE);
			case WM_PAINT:
				PAINTSTRUCT pc;

				BeginPaint(hWnd, &pc);
				GetWindowRect(hWnd, &rc);
				GetClientRect(hWnd, &rc);
				rc.top += 5;
				rc.left += 5;
				rc.bottom -= 5;
				rc.right -= 5;
				//FillRect(pc.hdc, &rc, HBRUSH(BLACK_BRUSH));
				EndPaint(hWnd, &pc);
				break;
			default:
				return DefWindowProcW(hWnd, uMsg, wParam, lParam);
			}
		}

		bool attach(HWND wnd)
		{
			SetWindowLongPtrW(wnd, GWLP_USERDATA, LONG_PTR(this));
			hWnd = wnd;
			return (GetWindowLongPtrW(hWnd, GWLP_USERDATA) == LONG_PTR(this)) ? true : false;
		}

		bool registerClass(PCWSTR className)
		{
			WNDCLASSEXW wcex = { 0 };

			wcex.cbSize = sizeof(WNDCLASSEX);
			wcex.style = CS_HREDRAW | CS_VREDRAW | CS_CLASSDC;
			wcex.lpfnWndProc = staticWndProc;
			wcex.hInstance = hInstance;
			wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wcex.hbrBackground = HBRUSH(DC_BRUSH);
			wcex.cbWndExtra = sizeof(this);
			wcex.lpszClassName = className;

			return RegisterClassExW(&wcex);
		}

		bool createWindow(PCWSTR className, PCWSTR wndName, int width, int height)
		{
			registerClass(className);

			auto Wnd = CreateWindowExW(0L, className, wndName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, PVOID(this));
			this->attach(Wnd);
			return hWnd == nullptr ? false : true;
		}

		bool show(int nShowCmd)
		{
			ShowWindow(hWnd, nShowCmd);
			return true;
		}

		explicit operator HWND() const
		{
			return this->hWnd;
		}

	};

	static class Application {
	private:
		HINSTANCE hInst = nullptr;
	public:
		Application() = default;

		bool init()
		{
			this->hInst = GetModuleHandleW(nullptr);
			return (this->hInst == nullptr) ? false : true;;
		}

		bool attach(HINSTANCE inst)
		{
			return inst == nullptr && ((this->hInst = inst)) ? false : true;
		}

		Window * createWindow(PCWSTR windowName, int width, int height)
		{
			Window *wnd = new Window(hInst);
			wchar_t* className = new wchar_t[20]{ 0 };
			wcscat_s(className, 20, windowName);
			wcscat_s(className, 20, L"_class_");

			wnd->createWindow(className, windowName, width, height);
			return wnd;
		}

		int Run(const std::function<void()> func)
		{
			MSG msg;
			static DWORD lastTime = GetTickCount();

			//初始化消息结构
			ZeroMemory(&msg, sizeof(MSG));

			while (msg.message != WM_QUIT)
			{
				if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}
				else
				{
					DWORD  currTime = GetTickCount();
					func();
					lastTime = currTime;
					Sleep(10);
				}
			}
			return int(msg.wParam);
		}

	} app;


}
