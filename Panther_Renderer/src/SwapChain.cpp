#include "DX12RendererPCH.h"
#include "SwapChain.h"

#include "../../Panther_Core/src/Exceptions.h"
#include "DX12DescriptorHeap.h"
#include "DX12RenderTarget.h"
#include "Renderer.h"

using namespace Microsoft::WRL;

namespace Panther
{
	uint32 SwapChain::s_RTVDescriptorSize = -1;

	std::unique_ptr<SwapChain> SwapChain::CreateSwapchain(IDXGIFactory4& a_DXGIFactory, ID3D12Device& a_Device,
		ID3D12CommandQueue& a_CommandQueue, HWND a_WindowHandle, const DXGI_SWAP_CHAIN_DESC1& a_Desc, 
		const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* a_FullscreenDesc, DX12DescriptorHeap& a_RTVDescHeap, 
		DX12DescriptorHeap& a_DSVDescHeap)
	{
		if (s_RTVDescriptorSize == -1)
		{
			s_RTVDescriptorSize = a_Device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		IDXGISwapChain3* swapChain = nullptr;
		IDXGISwapChain1* swapChain1 = nullptr;
		// Swap chain needs the queue so that it can force a flush on it.
		ThrowIfFailed(a_DXGIFactory.CreateSwapChainForHwnd(&a_CommandQueue, a_WindowHandle, &a_Desc, a_FullscreenDesc, nullptr,
			&swapChain1))
		ThrowIfFailed(swapChain1->QueryInterface(&swapChain));
		return std::unique_ptr<SwapChain>(new SwapChain(*swapChain, a_Device, a_RTVDescHeap, a_DSVDescHeap));
	}

	void SwapChain::Present(bool a_Vsync)
	{
		ThrowIfFailed(m_SwapChain->Present1(a_Vsync ? 1 : 0, 0, &m_PresentParameters))
	}

	void SwapChain::Resize(uint32 a_NumBuffers, uint32 a_Width, uint32 a_Height, DXGI_FORMAT a_NewFormat)
	{
		// Release the render target views.
		for (int32 i = 0; i < Renderer::NumBackBuffers; i++)
		{
			m_RenderTargets[i].reset();
		}
		m_DepthStencil.Reset();

		ThrowIfFailed(m_SwapChain->ResizeBuffers(a_NumBuffers, a_Width, a_Height, a_NewFormat, 0));

		// Initialize new render target views.
		{
			m_RTVDescHeap.SetCPUHandleIndex(0);
			// Create a RTV for each frame.
			for (uint32 n = 0; n < Renderer::NumBackBuffers; n++)
			{
				m_RenderTargets[n] = std::unique_ptr<DX12RenderTarget>(new DX12RenderTarget(*m_SwapChain.Get(), n));
				m_RTVDescHeap.RegisterRenderTarget(*m_RenderTargets[n]);
			}
		}

		// Create the depth stencil view.
		{
			m_DSVDescHeap.SetCPUHandleIndex(0);

			D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
			depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
			depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0;

			ThrowIfFailed(m_Device.CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Tex2D(
					DXGI_FORMAT_D32_FLOAT, a_Width, a_Height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, IID_PPV_ARGS(&m_DepthStencil)));

			m_DSVDescHeap.RegisterDepthStencil(*m_DepthStencil.Get(), depthStencilDesc);
		}
	}

	D3D12_RESOURCE_BARRIER SwapChain::SetResourceState(D3D12_RESOURCE_STATES a_NewState)
	{
		D3D12_RESOURCE_STATES prevState = m_ResourceState;
		m_ResourceState = a_NewState;
		return CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_SwapChain->GetCurrentBackBufferIndex()]->GetTargetBuffer(), 
			prevState, m_ResourceState);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE SwapChain::GetRTVDescriptorHandle()
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_RTVDescHeap.GetDescriptorHeap().GetCPUDescriptorHandleForHeapStart(),
			m_SwapChain->GetCurrentBackBufferIndex(), s_RTVDescriptorSize);
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE SwapChain::GetDSVDescriptorHandle()
	{
		return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DSVDescHeap.GetDescriptorHeap().GetCPUDescriptorHandleForHeapStart());
	}

	SwapChain::SwapChain(IDXGISwapChain3& a_SwapChain, ID3D12Device& a_Device, DX12DescriptorHeap& a_RTVDescHeap, DX12DescriptorHeap& a_DSVDescHeap) :
		m_SwapChain(&a_SwapChain), m_Device(a_Device), m_RTVDescHeap(a_RTVDescHeap), m_DSVDescHeap(a_DSVDescHeap)
	{
		m_RenderTargets = new std::unique_ptr<DX12RenderTarget>[Renderer::NumBackBuffers];
		ZeroMemory(&m_PresentParameters, sizeof(DXGI_PRESENT_PARAMETERS));
	}

	SwapChain::~SwapChain()
	{
		delete[] m_RenderTargets;
	}
}