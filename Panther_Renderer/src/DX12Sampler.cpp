#include "DX12RendererPCH.h"
#include "DX12Sampler.h"
#include "DX12Renderer.h"

namespace Panther
{
	DX12Sampler::DX12Sampler()
	{
		m_SamplerDescriptor.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		m_SamplerDescriptor.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		m_SamplerDescriptor.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		m_SamplerDescriptor.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		m_SamplerDescriptor.MinLOD = 0;
		m_SamplerDescriptor.MaxLOD = D3D12_FLOAT32_MAX;
		m_SamplerDescriptor.MipLODBias = 0.0f;
		m_SamplerDescriptor.MaxAnisotropy = 1;
		m_SamplerDescriptor.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	}

	DX12Sampler::DX12Sampler(D3D12_SAMPLER_DESC& a_SamplerDescriptor) 
		: m_SamplerDescriptor(a_SamplerDescriptor)
	{}
}