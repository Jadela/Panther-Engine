#include "DX12Buffer.h"

#include "Exceptions.h"
#include "DX12CommandList.h"
#include "DX12Renderer.h"

#include <d3dx12.h>

namespace Panther
{
	DX12Buffer::DX12Buffer(DX12Renderer& a_Renderer, uint32 a_NumElements, size_t a_ElementSize)
		: Buffer(a_NumElements, CalculateConstantBufferSize(a_ElementSize), BufferType::ConstantBuffer), m_Renderer(a_Renderer)
	{
		size_t elementSize = CalculateConstantBufferSize(a_ElementSize);

		ThrowIfFailed(m_Renderer.GetDevice().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, 
			&CD3DX12_RESOURCE_DESC::Buffer(a_NumElements * elementSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, 
			IID_PPV_ARGS(&m_ConstantBuffer)));

		ThrowIfFailed(m_ConstantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_CPUBuffer)));

		m_CBufferViewDescription.BufferLocation = m_ConstantBuffer->GetGPUVirtualAddress();
		m_CBufferViewDescription.SizeInBytes = (UINT)elementSize;
	}

	DX12Buffer::DX12Buffer(DX12Renderer& a_Renderer, DX12CommandList& a_CommandList, const void* a_Data, uint32 a_NumElements, size_t a_ElementSize)
		: Buffer(a_NumElements, a_ElementSize, BufferType::UploadBuffer), m_Renderer(a_Renderer)
	{
		size_t bufferSize = GetSize();

		ThrowIfFailed(m_Renderer.GetDevice().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&m_GPUBuffer)));

		ThrowIfFailed(m_Renderer.GetDevice().CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_UploadBuffer)));

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the vertex buffer.
		D3D12_SUBRESOURCE_DATA subRscData = {};
		subRscData.pData = a_Data;
		subRscData.RowPitch = a_ElementSize;
		subRscData.SlicePitch = a_ElementSize;

		UpdateSubresources(&a_CommandList.GetCommandList(), m_GPUBuffer.Get(), m_UploadBuffer.Get(), 0, 0, 1,
			&subRscData);
		a_CommandList.GetCommandList().ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_GPUBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	}

	DX12Buffer::~DX12Buffer()
	{
		if (m_ConstantBuffer != nullptr)
		{
			m_ConstantBuffer->Unmap(0, nullptr);
		}
		m_CPUBuffer = nullptr;
	}
}