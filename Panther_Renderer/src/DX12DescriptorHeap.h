#pragma once

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class DX12Texture;
	class DX12Buffer;
	class DX12Sampler;

	class DX12DescriptorHeap
	{
	public:
		DX12DescriptorHeap() = delete;
		DX12DescriptorHeap(ID3D12Device& a_D3DDevice, uint32 a_Capacity, D3D12_DESCRIPTOR_HEAP_TYPE a_Type);
		DX12DescriptorHeap(const DX12DescriptorHeap&) = delete;
		~DX12DescriptorHeap();

		void RegisterConstantBuffer(D3D12_CONSTANT_BUFFER_VIEW_DESC& a_ConstantBufferDesc);
		void RegisterConstantBuffer(DX12Buffer& a_ConstantBuffer);
		void RegisterTexture(DX12Texture& a_Texture);
		void RegisterSampler(DX12Sampler& a_Sampler);
		void RegisterRenderTarget(ID3D12Resource& a_RenderTarget);
		void RegisterDepthStencil(ID3D12Resource& a_DepthStencil, D3D12_DEPTH_STENCIL_VIEW_DESC& a_DSVDesc);

		void SetCPUHandleIndex(uint32 a_Index);

	private:
		friend class DemoScene;
		friend class DX12Renderer;

		std::string GetTypeString();

		ID3D12Device& m_D3DDevice;
		D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
		uint32 m_Capacity;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_D3DDescriptorHeap = nullptr;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_DescriptorHandle;
	};
}