#pragma once

namespace Panther
{
	class DX12Renderer;

	class DX12Sampler
	{
	public:
		DX12Sampler();
		explicit DX12Sampler(D3D12_SAMPLER_DESC& a_SamplerDescriptor);

	private:
		friend class DX12DescriptorHeap;

		D3D12_SAMPLER_DESC m_SamplerDescriptor = {};
	};
}