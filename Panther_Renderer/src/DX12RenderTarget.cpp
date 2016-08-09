#include "DX12RendererPCH.h"
#include "DX12RenderTarget.h"

namespace Panther
{
	DX12RenderTarget::DX12RenderTarget(IDXGISwapChain3& a_SwapChain, uint32 a_BufferIndex)
	{
		if (FAILED(a_SwapChain.GetBuffer(a_BufferIndex, IID_PPV_ARGS(&m_TargetBuffer))))
			throw std::runtime_error("DX12RenderTarget: Could't get SwapChain buffer while constructing render target!");
	}

	DX12RenderTarget::~DX12RenderTarget()
	{
		if (m_TargetBuffer != nullptr)
			m_TargetBuffer.Reset();
	}
}