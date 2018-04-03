#pragma once

#include "resource.h"

// 错误处理宏
#define ASSERT ATLASSERT
#define VERIFY ATLVERIFY
#ifdef _DEBUG
#define HR(expression) ASSERT(S_OK == (expression))
#else
struct ComException
{
	HRESULT const hr;
	ComException(HRESULT const value) : hr(value) {}
};
inline void HR(HRESULT const hr)
{
	if (S_OK != hr) throw ComException(hr);
}
#endif


#define __X_

#ifndef __X_
#define __X_


using namespace Microsoft::WRL;
using namespace D2D1;

namespace DXTest {
	// 窗口主类
	template <typename T>
	struct DesktopWindow :
		CWindowImpl<DesktopWindow<T>, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW | WS_VISIBLE>>
	{
		ComPtr<ID2D1DeviceContext> m_target;
		ComPtr<IDXGISwapChain1> m_swapChain;
		ComPtr<ID2D1Factory1> m_factory;
		ComPtr<ID3D11Device> d3device;
		ComPtr<IDXGIDevice> dxdevice;
		HWND m_hWnd;


		DECLARE_WND_CLASS_EX(nullptr, 0, -1);
		BEGIN_MSG_MAP(c)
			MESSAGE_HANDLER(WM_PAINT, PaintHandler)
			MESSAGE_HANDLER(WM_SIZE, SizeHandler)
			MESSAGE_HANDLER(WM_DISPLAYCHANGE, DisplayChangeHandler)
			MESSAGE_HANDLER(WM_DESTROY, DestroyHandler)
		END_MSG_MAP()

		LRESULT PaintHandler(UINT, WPARAM, LPARAM, BOOL &)
		{
			PAINTSTRUCT ps;
			
			VERIFY(BeginPaint(m_hWnd,&ps));
			Render();
			EndPaint(m_hWnd, &ps);
			return 0;
		}

		LRESULT DisplayChangeHandler(UINT, WPARAM, LPARAM, BOOL &)
		{
			Render();
			return 0;
		}

		LRESULT SizeHandler(UINT, WPARAM wparam, LPARAM, BOOL &)
		{
			if (m_target && SIZE_MINIMIZED != wparam)
			{
				ResizeSwapChainBitmap();
				Render();
			}
			return 0;
		}

		LRESULT DestroyHandler(UINT, WPARAM, LPARAM, BOOL &)
		{
			PostQuitMessage(0);
			return 0;
		}

		void Run()
		{
			D2D1_FACTORY_OPTIONS fo = {};
#ifdef _DEBUG
			fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
			HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
				fo,
				m_factory.GetAddressOf()));
			static_cast<T *>(this)->CreateDeviceIndependentResources();
			VERIFY(__super::Create(nullptr, nullptr, L"Introducing Direct2D 1.1"));
			MSG message;
			BOOL result;
			while (result = GetMessage(&message, 0, 0, 0))
			{
				if (-1 != result)
				{
					DispatchMessage(&message);
				}
			}
		}

		void ReleaseDevice()
		{
			m_target.Reset();
			m_swapChain.Reset();
			//static_cast<T *>(this)->ReleaseDeviceResources();
		}

		HRESULT CreateDevice(D3D_DRIVER_TYPE const type,
			ComPtr<ID3D11Device> & device)
		{
			UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
			flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
			return D3D11CreateDevice(nullptr,
				type,
				nullptr,
				flags,
				nullptr, 0,
				D3D11_SDK_VERSION,
				device.GetAddressOf(),
				nullptr,
				nullptr);
		}

		DesktopWindow() {

			auto hr = CreateDevice(D3D_DRIVER_TYPE_HARDWARE, d3device);
			if (DXGI_ERROR_UNSUPPORTED == hr)
			{
				hr = CreateDevice(D3D_DRIVER_TYPE_WARP, d3device);
			}
			HR(hr);


		}
		void CreateDeviceSwapChainBitmap() {
			if (true) {
				// 创建交换链
				
				HR(d3device.As(&dxdevice));

				// 检索显示适配器
				ComPtr<IDXGIAdapter> adapter;
				HR(dxdevice->GetAdapter(adapter.GetAddressOf()));
				ComPtr<IDXGIFactory2> factory;
				HR(adapter->GetParent(__uuidof(factory),
					reinterpret_cast<void **>(factory.GetAddressOf())));


				DXGI_SWAP_CHAIN_DESC1 props = {};
				props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				props.SampleDesc.Count = 1;
				props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				props.BufferCount = 2;

				HR(factory->CreateSwapChainForHwnd(d3device.Get(),
					m_hWnd,
					&props,
					nullptr,
					nullptr,
					m_swapChain.GetAddressOf()));
			}

			ComPtr<ID2D1Device> device;
			HR(m_factory->CreateDevice(dxdevice.Get(),
				device.GetAddressOf()));
			HR(device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
				m_target.GetAddressOf()));
			ComPtr<IDXGISurface> surface;
			HR(m_swapChain->GetBuffer(0, // buffer index
				__uuidof(surface),
				reinterpret_cast<void **>(surface.GetAddressOf())));

			auto props = BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				PixelFormat(
					DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
			ComPtr<ID2D1Bitmap1> bitmap;
			HR(m_target->CreateBitmapFromDxgiSurface(surface.Get(),
				props,
				bitmap.GetAddressOf()));
			m_target->SetTarget(bitmap.Get());
			float dpiX, dpiY;
			m_factory->GetDesktopDpi(&dpiX, &dpiY);
			m_target->SetDpi(dpiX, dpiY);
		}
		void Render()
		{
			if (!m_target)
			{
				CreateDevice();
				CreateDeviceSwapChainBitmap();
			}
			m_target->BeginDraw();
			static_cast<T *>(this)->Draw();
			m_target->EndDraw();
			auto const hr = m_swapChain->Present(1, 0);
			if (S_OK != hr && DXGI_STATUS_OCCLUDED != hr)
			{
				ReleaseDevice();
			}
		}
		void ResizeSwapChainBitmap()
		{
			m_target->SetTarget(nullptr);
			if (S_OK == m_swapChain->ResizeBuffers(0,
				0, 0,
				DXGI_FORMAT_UNKNOWN,
				0))
			{
				CreateDeviceSwapChainBitmap();
			}
			else
			{
				ReleaseDevice();
			}
		}

	};

	struct SimpleWindow :
		DesktopWindow<SimpleWindow>
	{

	};
	ATOM WndClassRegister(HINSTANCE hInstance);
	int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
}

#endif // !__X_