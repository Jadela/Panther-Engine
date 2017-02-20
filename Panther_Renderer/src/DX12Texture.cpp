#include "DX12RendererPCH.h"
#include "DX12Texture.h"

#include "DX12CommandList.h"
#include "DX12Renderer.h"

#include "../../Panther_Core/src/Core.h"
#include "../../Panther_Demo/src/Window.h"

namespace Panther
{
	DX12Texture::DX12Texture(DX12Renderer& a_Renderer) : 
		m_Renderer(a_Renderer)
	{
		m_ShaderResourceViewDescriptor.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		m_ShaderResourceViewDescriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_ShaderResourceViewDescriptor.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		m_ShaderResourceViewDescriptor.Texture2D.MipLevels = 1U;
	}

	DX12Texture::DX12Texture(DX12Renderer & a_Renderer, D3D12_SHADER_RESOURCE_VIEW_DESC & a_ShaderResourceViewDescriptor) :
		m_Renderer(a_Renderer), 
		m_ShaderResourceViewDescriptor(a_ShaderResourceViewDescriptor)
	{
	}

	void DX12Texture::Upload()
	{
		CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM, m_Width, m_Height,
			1U, 1U);
		HRESULT hr = m_Renderer.GetDevice().CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_GPUResource));
		if (FAILED(hr))
		{
			throw std::runtime_error("Panther DX12 ERROR: Could not create texture GPU resource.");
		}

		const uint32 subresourceCount = textureDesc.DepthOrArraySize * textureDesc.MipLevels;
		const uint64 uploadBufferSize = GetRequiredIntermediateSize(m_GPUResource.Get(), 0, subresourceCount);
		hr = m_Renderer.GetDevice().CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&m_UploadResource));
		if (FAILED(hr))
		{
			throw std::runtime_error("Panther DX12 ERROR: Could not create texture upload resource.");
		}

		// Copy data to the intermediate upload heap and then schedule a copy 
		// from the upload heap to the Texture2D.
		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = m_Data.get();
		textureData.RowPitch = m_Width * 4;
		textureData.SlicePitch = m_Width * m_Height * 4;

		UpdateSubresources(m_Renderer.GetCommandList().m_CommandList.Get(), m_GPUResource.Get(), m_UploadResource.Get(), 0, 0, subresourceCount,
			&textureData);
		m_Renderer.GetCommandList().m_CommandList.Get()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_GPUResource.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
	}
}