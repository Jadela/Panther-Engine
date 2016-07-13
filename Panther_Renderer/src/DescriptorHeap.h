#pragma once
#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class Buffer;
	class Texture;
	class Sampler;

	class DescriptorHeap
	{
	public:
		DescriptorHeap() {};
		DescriptorHeap(const DescriptorHeap&) = delete;
		~DescriptorHeap() {};

		virtual uint32 RegisterConstantBuffer(D3D12_CONSTANT_BUFFER_VIEW_DESC& a_ConstantBufferDesc) = 0;
		virtual uint32 RegisterConstantBuffer(Buffer& a_ConstantBuffer) = 0;
		virtual uint32 RegisterTexture(Texture& a_Texture) = 0;
		virtual uint32 RegisterSampler(Sampler& a_Sampler) = 0;
		virtual uint32 RegisterRenderTarget(ID3D12Resource& a_RenderTarget) = 0;
		virtual uint32 RegisterDepthStencil(ID3D12Resource& a_DepthStencil, D3D12_DEPTH_STENCIL_VIEW_DESC& a_DSVDesc) = 0;

		virtual void SetCPUHandleIndex(uint32 a_Index) = 0;

	protected:

	private:

	};
}