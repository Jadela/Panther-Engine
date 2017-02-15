#include "DX12RendererPCH.h"
#include "SwapChain.h"

#include "../../Panther_Core/src/Exceptions.h"

using namespace Microsoft::WRL;

namespace Panther
{
	std::unique_ptr<SwapChain> SwapChain::CreateSwapchain(IDXGIFactory4& a_DXGIFactory, IUnknown& a_DeviceOrCommandQueue, 
		HWND a_WindowHandle, const DXGI_SWAP_CHAIN_DESC1& a_Desc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* a_FullscreenDesc)
	{
		IDXGISwapChain3* swapChain = nullptr;
		IDXGISwapChain1* swapChain1 = nullptr;
		// Swap chain needs the queue so that it can force a flush on it.
		ThrowIfFailed(a_DXGIFactory.CreateSwapChainForHwnd(&a_DeviceOrCommandQueue, a_WindowHandle, &a_Desc, a_FullscreenDesc, nullptr, 
			&swapChain1))
		ThrowIfFailed(swapChain1->QueryInterface(&swapChain));
		return std::unique_ptr<SwapChain>(new SwapChain(*swapChain));
	}

	void SwapChain::Present(bool a_Vsync)
	{
		ThrowIfFailed(m_SwapChain->Present1(a_Vsync ? 1 : 0, 0, &m_PresentParameters))
	}

	void SwapChain::Resize(uint32 a_NumBuffers, uint32 a_Width, uint32 a_Height, DXGI_FORMAT a_NewFormat)
	{
		ThrowIfFailed(m_SwapChain->ResizeBuffers(a_NumBuffers, a_Width, a_Height, a_NewFormat, 0))
	}

	SwapChain::SwapChain(IDXGISwapChain3& a_SwapChain) : m_SwapChain(&a_SwapChain)
	{
		ZeroMemory(&m_PresentParameters, sizeof(DXGI_PRESENT_PARAMETERS));
	}
}