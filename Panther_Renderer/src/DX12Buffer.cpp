#include "DX12RendererPCH.h"
#include "DX12Buffer.h"

#include "DX12CommandList.h"
#include "DX12Renderer.h"

namespace Panther
{
	Panther::DX12Buffer::DX12Buffer(DX12Renderer& a_Renderer, size_t a_BufferSize)
		: Buffer(a_BufferSize, BufferType::ConstantBuffer), m_Renderer(a_Renderer)
	{
		HRESULT hr = m_Renderer.m_D3DDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(1024 * a_BufferSize), D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&m_ConstantBuffer));
		if (FAILED(hr))
			throw std::runtime_error("Panther DX12 ERROR: Creating resource for Buffer failed!");

		m_CBufferViewDescriptor.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		m_CBufferViewDescriptor.SizeInBytes = (a_BufferSize + 255) & ~255;	// CB size is required to be 256-byte aligned.

		// Map the constant buffers. We don't unmap this until the
		// app closes. Keeping things mapped for the lifetime of the resource is okay.
		hr = m_ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_CPUBuffer));
		if (FAILED(hr))
			throw std::runtime_error("Panther DX12 ERROR: Mapping of constant buffer failed!");
	}

	DX12Buffer::DX12Buffer(DX12Renderer& a_Renderer, DX12CommandList& a_CommandList, const void* a_Data, size_t a_Size, size_t a_ElementSize)
		: Buffer(a_Size, BufferType::UploadBuffer), m_Renderer(a_Renderer)
	{
		HRESULT hr = m_Renderer.m_D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(a_Size),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_GPUBuffer));
		if (FAILED(hr))
			throw std::runtime_error("Panther DX12 ERROR: Creating GPU buffer failed!");

		hr = m_Renderer.m_D3DDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(a_Size),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer));
		if (FAILED(hr))
			throw std::runtime_error("Panther DX12 ERROR: Creating upload buffer failed!");

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the vertex buffer.
		D3D12_SUBRESOURCE_DATA subRscData = {};
		subRscData.pData = a_Data;
		subRscData.RowPitch = a_ElementSize;
		subRscData.SlicePitch = a_ElementSize;

		UpdateSubresources(a_CommandList.m_CommandList.Get(), m_GPUBuffer.Get(), m_UploadBuffer.Get(), 0, 0, 1,
			&subRscData);
		a_CommandList.m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_GPUBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}
}