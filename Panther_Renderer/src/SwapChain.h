#pragma once

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class SwapChain
	{
	public:
		static std::unique_ptr<SwapChain> CreateSwapchain(IDXGIFactory4& a_DXGIFactory, IUnknown& a_DeviceOrCommandQueue, 
			HWND a_WindowHandle, const DXGI_SWAP_CHAIN_DESC1& a_Desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* a_FullscreenDesc);

		IDXGISwapChain3& GetSwapChain() { return *m_SwapChain.Get(); }

		void Present(bool a_Vsync);
		void Resize(uint32 a_NumBuffers, uint32 a_Width, uint32 a_Height, DXGI_FORMAT a_NewFormat);

	private:
		SwapChain() = delete;
		SwapChain(IDXGISwapChain3& a_SwapChain);

		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
		DXGI_PRESENT_PARAMETERS m_PresentParameters;
	};
}