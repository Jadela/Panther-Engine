#pragma once
#include "RenderTarget.h"

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class DX12RenderTarget final : public RenderTarget
	{
	public:
		DX12RenderTarget() = delete;
		DX12RenderTarget(IDXGISwapChain3& a_SwapChain, uint32 a_BufferIndex);
		~DX12RenderTarget();

	protected:
		friend class DX12CommandList;
		friend class DX12DescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_TargetBuffer = nullptr;
	};
}