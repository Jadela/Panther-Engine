#pragma once
#include "CommandList.h"

#include <wrl.h>

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

		void SetAndClearRenderTarget(SwapChain& a_SwapChain, const float a_Color[4]) final override;
		void SetMesh(Mesh& a_Mesh) final override;
		void UseDescriptorHeaps(DescriptorHeap** a_DescriptorHeaps, const uint32 a_NumDescriptorHeaps) final override;
		void UseDefaultViewport() final override;
		void ExecuteBundle(CommandList& a_Bundle) final override;
		void Draw(uint32 a_NumIndices) final override;
		void SetTransitionBarrier(SwapChain& a_SwapChain, D3D12_RESOURCE_STATES a_NewState) final override;
		void Close() final override;

		void Reset(DX12Material* a_Material = nullptr);

		ID3D12GraphicsCommandList& GetCommandList() { return *m_CommandList.Get(); }

	private:
		DX12Renderer& m_Renderer;
		D3D12_COMMAND_LIST_TYPE m_CommandListType;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_CommandList = nullptr;
	};
}