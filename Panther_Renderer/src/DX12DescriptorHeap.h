#pragma once
#include "DescriptorHeap.h"

namespace Panther
{
	class DX12DescriptorHeap final : public DescriptorHeap
	{
	public:
		DX12DescriptorHeap() = delete;
		DX12DescriptorHeap(ID3D12Device& a_D3DDevice, uint32 a_Capacity, D3D12_DESCRIPTOR_HEAP_TYPE a_Type);
		DX12DescriptorHeap(const DX12DescriptorHeap&) = delete;
		~DX12DescriptorHeap();

		void RegisterConstantBuffer(D3D12_CONSTANT_BUFFER_VIEW_DESC& a_ConstantBufferDesc) final override;
		void RegisterConstantBuffer(Buffer& a_ConstantBuffer) final override;
		void RegisterTexture(Texture& a_Texture) final override;
		void RegisterSampler(Sampler& a_Sampler) final override;
		void RegisterRenderTarget(ID3D12Resource& a_RenderTarget) final override;
		void RegisterDepthStencil(ID3D12Resource& a_DepthStencil, D3D12_DEPTH_STENCIL_VIEW_DESC& a_DSVDesc) final override;

		void SetCPUHandleIndex(uint32 a_Index) final override;

	private:
		friend class DemoScene;
		friend class DX12Renderer;
		friend class DX12CommandList;

		std::string GetTypeString();

		ID3D12Device& m_D3DDevice;
		D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
		uint32 m_Capacity;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_D3DDescriptorHeap = nullptr;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_DescriptorHandle;
	};
}