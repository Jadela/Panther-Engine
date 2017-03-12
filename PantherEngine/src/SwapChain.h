#pragma once

#include "../../Panther_Demo/src/Core.h"

namespace Panther
{
	class DX12DescriptorHeap;
	class DX12RenderTarget;

	class SwapChain
	{
	public:
		~SwapChain();

		static std::unique_ptr<SwapChain> CreateSwapchain(IDXGIFactory4& a_DXGIFactory, ID3D12Device& a_Device, 
			ID3D12CommandQueue& a_CommandQueue, HWND a_WindowHandle, const DXGI_SWAP_CHAIN_DESC1& a_Desc, 
			const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* a_FullscreenDesc, DX12DescriptorHeap& a_RTVDescHeap, 
			DX12DescriptorHeap& a_DSVDescHeap);
		
		void Present(bool a_Vsync);
		void Resize(uint32 a_NumBuffers, uint32 a_Width, uint32 a_Height, DXGI_FORMAT a_NewFormat);
		D3D12_RESOURCE_BARRIER SetResourceState(D3D12_RESOURCE_STATES a_NewState);
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetRTVDescriptorHandle();
		CD3DX12_CPU_DESCRIPTOR_HANDLE GetDSVDescriptorHandle();

	private:
		SwapChain() = delete;
		SwapChain(IDXGISwapChain3& a_SwapChain, ID3D12Device& a_Device, DX12DescriptorHeap& a_RTVDescHeap, DX12DescriptorHeap& a_DSVDescHeap);

		static uint32 s_RTVDescriptorSize;

		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
		DXGI_PRESENT_PARAMETERS m_PresentParameters;
		std::unique_ptr<DX12RenderTarget>* m_RenderTargets = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DepthStencil = nullptr;
		D3D12_RESOURCE_STATES m_ResourceState = D3D12_RESOURCE_STATE_COMMON;
		ID3D12Device& m_Device;
		DX12DescriptorHeap& m_RTVDescHeap;
		DX12DescriptorHeap& m_DSVDescHeap;
	};
}