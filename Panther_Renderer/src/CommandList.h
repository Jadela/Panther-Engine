#pragma once
#include "../../Panther_Core/src/Core.h"
#include "Material.h"

namespace Panther
{
	class DescriptorHeap;
	class Mesh;
	class SwapChain;

	class CommandList
	{
	public:
		virtual ~CommandList();

		virtual void SetAndClearRenderTarget(SwapChain& a_SwapChain, const float a_Color[4]) = 0;
		virtual void SetMaterial(Material& a_Material, bool a_ResetState) = 0;
		virtual void SetMesh(Mesh& a_Mesh) = 0;
		virtual void SetDescriptorHeap(DescriptorHeap& a_DescriptorHeap, Material::DescriptorSlot& a_Slot, uint32 a_HeapElementOffset) = 0;
		virtual void UseDescriptorHeaps(DescriptorHeap** a_DescriptorHeaps, const uint32 a_NumDescriptorHeaps) = 0;
		virtual void UseDefaultViewport() = 0;
		virtual void ExecuteBundle(CommandList& a_Bundle) = 0;
		virtual void Draw(uint32 a_NumIndices) = 0;
		virtual void SetTransitionBarrier(D3D12_RESOURCE_STATES a_OldState, D3D12_RESOURCE_STATES a_NewState) = 0;
		virtual void Close() = 0;

	protected:
		CommandList();
		CommandList(const CommandList&);

	private:

	};
}