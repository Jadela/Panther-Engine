#pragma once
#include "RenderTarget.h"
#include "Core.h"

#include <dxgi1_5.h>
#include <d3d12.h>
#include <wrl.h>

namespace Panther
{
	class DX12RenderTarget final : public RenderTarget
	{
	public:
		DX12RenderTarget() = delete;
		DX12RenderTarget(IDXGISwapChain3& a_SwapChain, uint32 a_BufferIndex);
		~DX12RenderTarget();

		ID3D12Resource* GetTargetBuffer() { return m_TargetBuffer.Get(); }

	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_TargetBuffer = nullptr;
	};
}