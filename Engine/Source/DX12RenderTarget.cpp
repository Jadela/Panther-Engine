#include "DX12RenderTarget.h"

#include "Exceptions.h"

namespace Panther
{
	DX12RenderTarget::DX12RenderTarget(IDXGISwapChain3& a_SwapChain, uint32 a_BufferIndex)
	{
		ThrowIfFailed(a_SwapChain.GetBuffer(a_BufferIndex, IID_PPV_ARGS(&m_TargetBuffer)));
	}

	DX12RenderTarget::~DX12RenderTarget()
	{
		if (m_TargetBuffer != nullptr)
			m_TargetBuffer.Reset();
	}
}