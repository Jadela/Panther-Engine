#pragma once
#include "Mesh.h"

#include <d3d12.h>

namespace Panther
{
	class DX12Mesh final : public Mesh
	{
	public:
		DX12Mesh() = delete;
		DX12Mesh(Renderer& a_Renderer);

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return m_VertexBufferView; }
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() { return m_IndexBufferView; }

	protected:
		void Initialize(CommandList& a_CommandList) final override;
		
	private:
		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
	};
}