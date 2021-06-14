#include "DX12Mesh.h"

#include "CommandList.h"
#include "DX12Buffer.h"
#include "Renderer.h"
#include <stdexcept>

namespace Panther
{
	DX12Mesh::DX12Mesh(Renderer& a_Renderer)
		: Mesh(a_Renderer)
	{
	}

	void DX12Mesh::Initialize(CommandList& a_CommandList)
	{
		if (m_Vertices.empty() || m_Indices.empty())
			throw std::runtime_error("No vertices or indices were assigned before calling Initialize!");

		// Create the buffers.
		m_VertexBuffer = std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(a_CommandList, &m_Vertices[0], (uint32)m_Vertices.size(), sizeof(Vertex)));
		m_IndexBuffer = std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(a_CommandList, &m_Indices[0], (uint32)m_Indices.size(), sizeof(Index)));
		
		// Initialize the vertex buffer view.
		m_VertexBufferView.BufferLocation = static_cast<DX12Buffer*>(m_VertexBuffer.get())->GetGPUBuffer().GetGPUVirtualAddress();
		m_VertexBufferView.StrideInBytes = sizeof(Vertex);
		m_VertexBufferView.SizeInBytes = (UINT)m_VertexBuffer->GetSize();
		
		// Describe the index buffer view.
		m_IndexBufferView.BufferLocation = static_cast<DX12Buffer*>(m_IndexBuffer.get())->GetGPUBuffer().GetGPUVirtualAddress();
		m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
		m_IndexBufferView.SizeInBytes = (UINT)m_IndexBuffer->GetSize();
	}
}