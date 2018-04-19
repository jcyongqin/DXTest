#include "stdafx.h"
#include "main.h"





// 8 像素窗口边框 25像素标题栏

using namespace BlankWindow;
using namespace Microsoft::WRL;

namespace BlankWindow
{

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
		dx9Device->SetRenderState(D3DRS_VERTEXBLEND, FALSE);// 没有设置光源，关掉关照处理。

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
	dx9Device->DrawPrimitive(D3DPT_LINELIST, 0, 1);

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
	Window *window = app.createWindow(L"BlankWindow", 900, 600);
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

	ColorVertex objData[] = { 0

	};

	const DWORD FVF_COLOR = (D3DFVF_XYZ | D3DFVF_DIFFUSE);

	InitializeD3D(HWND(*window), FALSE, dx9Obj, dx9Device);
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

