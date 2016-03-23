#pragma once
#include "Mesh.h"

namespace Panther
{
	class DX12Mesh final : public Mesh
	{
	public:
		DX12Mesh() = delete;
		DX12Mesh(Renderer& a_Renderer);

	protected:
		void Initialize(CommandList& a_CommandList) final override;

	private:
		friend class DX12CommandList;

		D3D12_VERTEX_BUFFER_VIEW m_VertexBufferView = {};
		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
	};
}