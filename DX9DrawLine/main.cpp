#include "stdafx.h"
#include "main.h"
#include <functional>

// 7 像素窗口边框 25像素标题栏
#define null nullptr

using namespace BlankWindow;

namespace BlankWindow
{
	struct Application
	{
		HINSTANCE hInst;
		PCWSTR className;
		//PWSTR windowName;
		HWND hThis;
		WNDCLASSEXW wc = { sizeof(WNDCLASSEX), /*CS_VREDRAW | CS_HREDRAW ||CS_CLASSDC*/NULL // 窗口改变大小时重绘整个窗口
			, MsgProc, 0L, 0L,
			hInst, NULL, NULL,  CreateSolidBrush(RGB(255, 255, 255)), NULL,
			className, NULL };
		WORD create(PCWSTR wndName, int X, int Y)
		{
			hThis = CreateWindowExW(0L, className, wndName,  /*WS_POPUP | WS_VISIBLE | WS_CAPTION*/ WS_THICKFRAME | NULL,
				CW_USEDEFAULT, CW_USEDEFAULT, X, Y,
				null, null, hInst, this);
			if (hThis == null)
			{
				return FALSE;
			}
			return TRUE;
		}
		BOOL show(int nShowCmd)
		{
			ShowWindow(hThis, nShowCmd);
			UpdateWindow(hThis);
			return TRUE;
		}
		WORD createAndShow(PCWSTR wndName, int nShowCmd)
		{

			create(wndName, 480, 360);
			show(nShowCmd);
			return TRUE;
		}
		BOOL init(PCWSTR clsName)
		{
			hInst = GetModuleHandleW(nullptr);
			if (hInst == null)
			{
				return FALSE;
			}
			wc.hInstance = hInst;
			wc.lpszClassName = className = clsName;
			return RegisterClassExW(&wc);
			//return TRUE;
		}

		static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			RECT rc;
			switch (msg)
			{
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_KEYUP:
				DestroyWindow(hWnd);
				break;
			case WM_CREATE:
				SetWindowPos(hWnd, HWND_TOPMOST, 300, 300, 480, 360, FALSE);
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
				FillRect(pc.hdc, &rc, CreateSolidBrush(RGB(0, 0, 0)));
				EndPaint(hWnd, &pc);
				break;
			default:
				return DefWindowProcW(hWnd, msg, wParam, lParam);
			}
			return 0;


		}
		int Run(const std::function<void()> func)
		{
			MSG msg;

			//初始化消息结构
			ZeroMemory(&msg, sizeof(MSG));

			while (msg.message != WM_QUIT)
			{
				if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
				{
					func();
					TranslateMessage(&msg);
					DispatchMessageW(&msg);
				}
				else
				{
					func();
					Sleep(10);
				}
			}
			return msg.wParam;
		}

	};
	struct Window {};


}


bool InitializeD3D(HWND hWnd, bool fullscreen, IDirect3D9 ** D3D, IDirect3DDevice9 ** D3DDevice)
{
	D3DDISPLAYMODE displayMode;
	IDirect3D9 * g_D3D;
	IDirect3DDevice9 * g_D3DDevice;
	// Create the D3D object.  
	g_D3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_D3D == NULL) return false;
	// Get the desktop display mode.  
	if (FAILED(g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)))
		return false;
	// Set up the structure used to create the D3DDevice  
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	if (fullscreen)
	{
		d3dpp.Windowed = FALSE;
		d3dpp.BackBufferWidth = 1920;
		d3dpp.BackBufferHeight = 1080;
	}
	else
		d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = displayMode.Format;
	// Create the D3DDevice  
	HRESULT hr = g_D3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_D3DDevice);
	*D3D = g_D3D;
	*D3DDevice = g_D3DDevice;
	return SUCCEEDED(hr);
}

bool InitializeObjects(IDirect3DDevice9 * Device, IDirect3DVertexBuffer9 ** VB, size_t vertexArrSize, void * vertexArrPtr, DWORD d3dfvfStyle)
{

	// Create the vertex buffer.  
	if (FAILED(Device->CreateVertexBuffer(vertexArrSize, 0, d3dfvfStyle, D3DPOOL_DEFAULT, VB, NULL))) return false;
	// Fill the vertex buffer.  
	void *ptr;
	if (FAILED((*VB)->Lock(0, vertexArrSize, (void**)&ptr, 0))) return false;
	memcpy(ptr, vertexArrPtr, vertexArrSize);
	(*VB)->Unlock();
	return true;
}

void RenderScene(IDirect3DDevice9 * Device, IDirect3DVertexBuffer9 * VB, size_t VBobjSize, DWORD d3dfvfStyle)
{
	// Clear the backbuffer.  
	Device->Clear(0, NULL, D3DCLEAR_TARGET,
		D3DCOLOR_XRGB(0x66, 0x99, 0xff), 1.0f, 0);
	// Begin the scene. Start rendering.  
	Device->BeginScene();

	// Render object.  
	Device->SetStreamSource(0, VB, 0, VBobjSize);
	Device->SetFVF(d3dfvfStyle);
	Device->DrawPrimitive(D3DPT_LINELIST, 0, 2);

	//
	Device->SetFVF(d3dfvfStyle);
	Device->DrawPrimitive(D3DPT_LINELIST, 0, 2);
	//

	// End the scene. Stop rendering.  
	Device->EndScene();
	// Display the scene.  
	Device->Present(NULL, NULL, NULL, NULL);

}

void Shutdown(IDirect3D9 * g_D3D, IDirect3DDevice9 * g_D3DDevice, IDirect3DVertexBuffer9 * VB)
{
	if (g_D3DDevice != NULL) g_D3DDevice->Release();
	if (g_D3D != NULL) g_D3D->Release();
	if (VB != NULL) g_D3D->Release();

}
int WINAPI wWinMain(
	_In_ HINSTANCE hInst,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd)
{
	Application app;
	app.init(L"BlankWindowClass");
	app.createAndShow(L"标题党", nShowCmd);

	// Direct3D object and device.  
	LPDIRECT3D9 g_D3D = NULL;
	LPDIRECT3DDEVICE9 g_D3DDevice = NULL;
	LPDIRECT3DVERTEXBUFFER9 g_D3DVB = NULL;

	// Game Object
	struct stD3DVertex
	{

		D3DVECTOR pos;
		D3DCOLOR color;
	};
	const D3DCOLOR yellow = D3DCOLOR_ARGB(255, 255, 255, 255);

	stD3DVertex objData[] = {
		{ -0.5f, 0.5f, 0.5f, yellow, },
		{ -0.5f, -0.5f, 0.5f, yellow, },
		{ 0.5f, 0.5f, 0.5f, yellow, },
		{ 0.5f, -0.5f, 0.5f, yellow, } };

	const DWORD d3dfvfStyle = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);


	InitializeD3D(app.hThis, FALSE, &g_D3D, &g_D3DDevice);
	InitializeObjects(g_D3DDevice, &g_D3DVB, sizeof(objData), objData, d3dfvfStyle);

	app.Run([=]() // 值传递指针
	{
		//RenderScene(g_D3DDevice,g_D3DVB,sizeof(stD3DVertex), d3dfvfStyle);

		// Clear the backbuffer.  
		g_D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET,
			D3DCOLOR_XRGB(0x66, 0x99, 0xff), 1.0f, 0);
		// Begin the scene. Start rendering.  
		g_D3DDevice->BeginScene();

		DWORD assdasd;

		g_D3DDevice->GetFVF(&assdasd);
		// Render object.  
		//g_D3DDevice->SetStreamSource(0, VB, 0, VBobjSize);
		g_D3DDevice->SetFVF(d3dfvfStyle);
		auto as = sizeof stD3DVertex;
		auto hr=g_D3DDevice->DrawPrimitiveUP(D3DPT_LINELIST, 2, (void *)objData, sizeof stD3DVertex);

		if (FAILED(hr))return FALSE;

		// End the scene. Stop rendering.  
		g_D3DDevice->EndScene();
		// Display the scene.  
		g_D3DDevice->Present(NULL, NULL, NULL, NULL);

	});

	Shutdown(g_D3D, g_D3DDevice, g_D3DVB);
	return 0;
}

