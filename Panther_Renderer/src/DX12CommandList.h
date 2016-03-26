#pragma once
#include "CommandList.h"

namespace Panther
{
	class DX12Renderer;
	class DX12Material;

	class DX12CommandList final : public CommandList
	{
	public:
		DX12CommandList() = delete;
		DX12CommandList(DX12Renderer& a_Renderer, D3D12_COMMAND_LIST_TYPE a_CommandListType, DX12Material* a_Material = nullptr);
		virtual ~DX12CommandList() final override;

		void SetMaterial(Material& a_Material, bool a_ResetState) final override;
		void SetMesh(Mesh& a_Mesh) final override;
		void SetDescriptorHeap(DescriptorHeap& a_DescriptorHeap, uint32 a_Slot, uint32 a_HeapElementOffset) final override;
		void UseDescriptorHeaps(DescriptorHeap** a_DescriptorHeaps, const uint32 a_NumDescriptorHeaps) final override;
		void Draw(uint32 a_NumIndices) final override;
		void Close() final override;

		void Reset(DX12Material* a_Material = nullptr);
	private:
		// TODO (JDL): Get rid of this.
		friend class DemoScene;
		friend class DX12Buffer;
		friend class DX12Renderer;
		friend class DX12Texture;

		DX12Renderer& m_Renderer;
		D3D12_COMMAND_LIST_TYPE m_CommandListType;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
	};
}