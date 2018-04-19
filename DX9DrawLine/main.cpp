#include "stdafx.h"
#include "main.h"
#include <functional>




// 8 像素窗口边框 25像素标题栏

using namespace BlankWindow;
using namespace Microsoft::WRL;

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
			if (window==nullptr)
			{
				return DefWindowProcW(hWnd, uMsg, wParam, lParam);
			}
			if (WM_NCCREATE == uMsg) {
			
				if (window != nullptr)
				{
					window->attach(hWnd);
					SetWindowLongPtrW(hWnd, GWLP_USERDATA, LONG_PTR(window));
				}
				else
				{
					/*createwindow没有正确传递Window指针 */
				}
			}
			else
			{
		
				return window->dynamicWndProc(uMsg, wParam, lParam);
			}

			return 0;
		}


		LRESULT dynamicWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			RECT rc;
			switch (uMsg)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_KEYUP:
				DestroyWindow(hWnd);
				break;
			case WM_CREATE:
				//SetWindowPos(hWnd, HWND_TOPMOST, 300, 300, 480, 360, FALSE);
				//MoveWindow(hWnd, 300, 300, 480, 360, TRUE);
			case WM_PAINT:
				PAINTSTRUCT pc;

				BeginPaint(hWnd, &pc);
				GetWindowRect(hWnd, &rc);
				GetClientRect(hWnd, &rc);
				rc.top += 2;
				rc.left += 2;
				rc.bottom -= 2;
				rc.right -= 2;
				FillRect(pc.hdc, &rc, HBRUSH(BLACK_BRUSH));
				EndPaint(hWnd, &pc);
				break;
			default:
				return DefWindowProcW(hWnd, uMsg, wParam, lParam);
			}
		}

		bool attach(HWND wnd)
		{
			if (hWnd != nullptr)
			{
				DestroyWindow(hWnd);
			}
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
			wcex.hbrBackground = HBRUSH(GRAY_BRUSH);
			wcex.cbWndExtra = sizeof(this);
			wcex.lpszClassName = className;

			return RegisterClassExW(&wcex);
		}

		bool createWindow(PCWSTR className, PCWSTR wndName, int width, int height)
		{
			registerClass(className);
			hWnd = CreateWindowW(className, wndName, WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, PVOID(this));
			//hWnd = CreateWindowExW(0L, className, wndName, WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, PVOID(this));
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


	struct _Application
	{
		HINSTANCE hInst;
		PCWSTR className;
		//PWSTR windowName;
		HWND hWnd;
		WNDCLASSEXW wc = { 0 };

		DWORD wndStyle;
		UINT wndClassStyle;

		BOOL init(PCWSTR clsName)
		{
			wndClassStyle = NULL | CS_DBLCLKS /*CS_VREDRAW | CS_HREDRAW ||CS_CLASSDC*/; // 窗口改变大小时重绘整个窗口
			wndStyle = NULL | WS_THICKFRAME /*WS_POPUP | WS_VISIBLE | WS_CAPTION*/;

			hInst = GetModuleHandleW(nullptr);
			if (hInst == nullptr)
			{
				return FALSE;
			}
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = wndClassStyle;
			wc.lpfnWndProc = 0;
			wc.hInstance = hInst;
			wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
			wc.lpszClassName = className = clsName;

			return RegisterClassExW(&wc);
			//return TRUE;
		}





		WORD create(PCWSTR wndName, int X, int Y)
		{
			hWnd = CreateWindowExW(0L, className, wndName, wndStyle,
				CW_USEDEFAULT, CW_USEDEFAULT, X, Y,
				nullptr, nullptr, hInst, this);
			if (hWnd == nullptr)
			{
				return FALSE;
			}
			return TRUE;
		}




	};



	bool InitializeD3D(HWND hWnd, bool fullscreen, ComPtr<IDirect3D9> &dx9Obj, ComPtr<IDirect3DDevice9> &dx9Device)
	{
		D3DCAPS9 caps;
		int vp = 0; // 保存定点处理单元（硬件/软件）
		IDirect3DDevice9  *pDevice;
		D3DPRESENT_PARAMETERS d3dpp = { 0 };
		//
		d3dpp.BackBufferWidth = 1280;
		d3dpp.BackBufferHeight = 960;
		d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
		d3dpp.BackBufferCount = 1;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.hDeviceWindow = hWnd;
		d3dpp.Windowed = BOOL(!fullscreen);
		d3dpp.EnableAutoDepthStencil = true;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		//
		dx9Obj.Attach(Direct3DCreate9(D3D_SDK_VERSION));
		dx9Obj->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
		vp = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING;
		HRESULT hr = dx9Obj->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, vp, &d3dpp, &pDevice);
		dx9Device.Attach(pDevice);
		// 设置设备状态
		//dx9Device->SetRenderState(D3DRS_LIGHTING, FALSE);// 没有设置光源，关掉关照处理。

		return FAILED(hr) ? false : true;
	}

	bool InitializeObjects(ComPtr<IDirect3DDevice9> &dx9Device, ComPtr<IDirect3DVertexBuffer9> &dx9VB,
		size_t vertexArrSize, void * vertexArrPtr, DWORD d3dfvfStyle)
	{
		IDirect3DVertexBuffer9 *pVB;
		// Create the vertex buffer.  
		if (FAILED(dx9Device->CreateVertexBuffer(vertexArrSize, 0, d3dfvfStyle, D3DPOOL_DEFAULT, &pVB, NULL))) return false;
		// Fill the vertex buffer.  
		void *ptr;
		if (FAILED(pVB->Lock(0, vertexArrSize, static_cast<void**>(&ptr), 0))) return false;
		memcpy(ptr, vertexArrPtr, vertexArrSize);
		pVB->Unlock();
		dx9VB.Attach(pVB);
		return true;
	}
}


void RenderScene(ComPtr<IDirect3DDevice9> &dx9Device, ComPtr<IDirect3DVertexBuffer9> &dx9VB, size_t VBobjSize, DWORD d3dfvfStyle)
{
	//**临时局部viewport
	D3DVIEWPORT9 vp;
	vp.Height = 480;
	vp.Width = 360;
	vp.X = 0;
	vp.Y = 0;

	//** end
	// Clear the backbuffer.  
	dx9Device->Clear(0, nullptr, D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0x66, 0x99, 0xff), 1.0f, 0);
	// Begin the scene. Start rendering.  
	dx9Device->BeginScene();

	// Render object.  
	dx9Device->SetStreamSource(0, dx9VB.Get(), 0, VBobjSize);
	dx9Device->SetFVF(d3dfvfStyle);
	dx9Device->SetViewport(&vp);
	dx9Device->DrawPrimitive(D3DPT_LINELIST, 0, 2);

	// End the scene. Stop rendering.  
	dx9Device->EndScene();
	// Display the scene.  
	dx9Device->Present(nullptr, nullptr, nullptr, nullptr);

}

void Shutdown(ComPtr<IDirect3D9> dx9Obj, ComPtr<IDirect3DDevice9> dx9Device, ComPtr<IDirect3DVertexBuffer9> dx9VB)
{
	dx9Obj.Reset();
	dx9Device.Reset();
	dx9VB.Reset();
}

int WINAPI wWinMain(
	_In_ HINSTANCE hInst,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	app.init();
	Window *window = app.createWindow(L"BlankWindow", 600, 900);
	window->show(nShowCmd);



	// Direct3D object and device.
	ComPtr<IDirect3D9> dx9Obj;
	ComPtr<IDirect3DDevice9> dx9Device;
	ComPtr<IDirect3DSurface9> dx9Surface;
	ComPtr<IDirect3DVertexBuffer9> dx9VB;


	// Game Object
	struct ColorVertex
	{
		D3DVECTOR pos;
		D3DCOLOR color;
	};
	const D3DCOLOR yellow = D3DCOLOR_XRGB(255, 255, 255);

	ColorVertex objData[] = {
		{-0.5, 0.5, 0.5, yellow},
		{-0.5, -0.5, 0.5, yellow},
		{0.5, 0.5, 0.5, yellow},
		{0.5, -0.5, 0.5, yellow},
	};

	const DWORD FVF_COLOR = (D3DFVF_XYZ | D3DFVF_DIFFUSE);


	InitializeD3D(HWND(window), FALSE, dx9Obj, dx9Device);
	InitializeObjects(dx9Device, dx9VB, sizeof(objData), objData, FVF_COLOR);

	app.Run([&]() // 值传递指针
	{
		//RenderScene(g_D3DDevice,g_D3DVB,sizeof(stD3DVertex), d3dfvfStyle);

		// Clear the backbuffer.  
		dx9Device->Clear(0, NULL, D3DCLEAR_TARGET,
			D3DCOLOR_XRGB(0x66, 0x99, 0xff), 0.5f, 0);
		// Begin the scene. Start rendering.  
		dx9Device->BeginScene();

		RenderScene(dx9Device, dx9VB, sizeof(objData), FVF_COLOR);
		// End the scene. Stop rendering.  
		dx9Device->EndScene();
		// Display the scene.  
		dx9Device->Present(nullptr, nullptr, nullptr, nullptr);

		return;
	});

	Shutdown(dx9Obj, dx9Device, dx9VB);
	return 0;
}

