#pragma once
#include "DescriptorHeap.h"

#include <d3dx12.h>
#include <wrl.h>

#include <string>

namespace Panther
{
	class DX12DescriptorHeap final : public DescriptorHeap
	{
	public:
		DX12DescriptorHeap() = delete;
		DX12DescriptorHeap(ID3D12Device& a_D3DDevice, uint32 a_Capacity, D3D12_DESCRIPTOR_HEAP_TYPE a_Type);
		DX12DescriptorHeap(const DX12DescriptorHeap&) = delete;
		~DX12DescriptorHeap();

		uint32 RegisterConstantBuffer(Buffer& a_ConstantBuffer, const uint32 a_OffsetInElements) final override;
		uint32 RegisterTexture(Texture& a_Texture) final override;
		uint32 RegisterSampler(Sampler& a_Sampler) final override;
		uint32 RegisterRenderTarget(RenderTarget& a_RenderTarget) final override;
		uint32 RegisterDepthStencil(ID3D12Resource& a_DepthStencil, D3D12_DEPTH_STENCIL_VIEW_DESC& a_DSVDesc) final override;

		void SetCPUHandleIndex(uint32 a_Index) final override;

		ID3D12DescriptorHeap& GetDescriptorHeap() { return *m_D3DDescriptorHeap.Get(); }
		D3D12_DESCRIPTOR_HEAP_TYPE GetType() { return m_Type; }

	private:
		std::string GetTypeString();

		ID3D12Device& m_D3DDevice;
		D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
		uint32 m_Capacity;
		uint32 m_Offset;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_D3DDescriptorHeap = nullptr;
		CD3DX12_CPU_DESCRIPTOR_HANDLE m_DescriptorHandle;
	};
}