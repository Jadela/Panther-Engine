#pragma once
#include "Sampler.h"

#include <d3d12.h>

namespace Panther
{
	class DX12Renderer;

	class DX12Sampler final : public Sampler
	{
	public:
		DX12Sampler() = delete;
		explicit DX12Sampler(D3D12_TEXTURE_ADDRESS_MODE a_TextureCoordinateMode = D3D12_TEXTURE_ADDRESS_MODE_WRAP);
		explicit DX12Sampler(D3D12_SAMPLER_DESC& a_SamplerDescriptor);

		D3D12_SAMPLER_DESC& GetSamplerDesc() { return m_SamplerDescriptor; }

	private:
		D3D12_SAMPLER_DESC m_SamplerDescriptor = {};
	};
}