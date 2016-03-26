#pragma once
#include "Buffer.h"

namespace Panther
{
	class DX12CommandList;
	class DX12Renderer;

	class DX12Buffer final : public Buffer
	{
	public:
		DX12Buffer(DX12Renderer& a_Renderer, size_t a_BufferSize);
		DX12Buffer(DX12Renderer& a_Renderer, DX12CommandList& a_CommandList, const void* a_Data, size_t a_Size, size_t a_ElementSize);
		DX12Buffer(const DX12Buffer&) = delete;
		virtual ~DX12Buffer() final override {}

	private:
		friend class DX12DescriptorHeap;
		friend class DX12Mesh;

		DX12Renderer& m_Renderer;

		// Constant buffer
		D3D12_CONSTANT_BUFFER_VIEW_DESC m_CBufferViewDescriptor = {};
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBuffer = nullptr;

		// Upload buffer
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_GPUBuffer = nullptr;
	};
}