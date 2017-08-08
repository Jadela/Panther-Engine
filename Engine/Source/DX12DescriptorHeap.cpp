#include "DX12DescriptorHeap.h"

#include "Exceptions.h"
#include "DX12Buffer.h"
#include "DX12Sampler.h"
#include "DX12Texture.h"
#include "DX12RenderTarget.h"

namespace Panther
{
	DX12DescriptorHeap::DX12DescriptorHeap(ID3D12Device& a_D3DDevice, uint32 a_Capacity, D3D12_DESCRIPTOR_HEAP_TYPE a_Type) :
		m_D3DDevice(a_D3DDevice), m_Type(a_Type), m_Capacity(a_Capacity), m_Offset(0)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDescriptor = {};
		heapDescriptor.NumDescriptors = m_Capacity;
		heapDescriptor.Type = m_Type;
		heapDescriptor.Flags = (m_Type <= D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_D3DDevice.CreateDescriptorHeap(&heapDescriptor, IID_PPV_ARGS(&m_D3DDescriptorHeap)));

		m_DescriptorHandle = m_D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	}

	DX12DescriptorHeap::~DX12DescriptorHeap()
	{
	}

	uint32 DX12DescriptorHeap::RegisterConstantBuffer(Buffer& a_ConstantBuffer, const uint32 a_OffsetInElements)
	{
		DX12Buffer& constantBuffer(*static_cast<DX12Buffer*>(&a_ConstantBuffer));
		if (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			D3D12_CONSTANT_BUFFER_VIEW_DESC description = constantBuffer.GetDescription();
			description.BufferLocation += a_OffsetInElements * description.SizeInBytes;

			m_D3DDevice.CreateConstantBufferView(&description, m_DescriptorHandle);
			m_DescriptorHandle.Offset(1, m_D3DDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			return m_Offset++;
		}
		else
		{
			throw std::runtime_error("Panther DX12 ERROR: Trying to register constant buffer in descriptor heap with wrong type.\nType: "
				+ GetTypeString());
		}
	}

	uint32 DX12DescriptorHeap::RegisterTexture(Texture& a_Texture)
	{
		DX12Texture& texture(*static_cast<DX12Texture*>(&a_Texture));
		if (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)
		{
			m_D3DDevice.CreateShaderResourceView(texture.GetGPUResource(), &texture.GetSRVDesc(), m_DescriptorHandle);
			m_DescriptorHandle.Offset(1, m_D3DDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
			return m_Offset++;
		}
		else
		{
			throw std::runtime_error("Panther DX12 ERROR: Trying to register texture in descriptor heap with wrong type.\nType: "
				+ GetTypeString());
		}
	}

	uint32 DX12DescriptorHeap::RegisterSampler(Sampler& a_Sampler)
	{
		if (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)
		{
			m_D3DDevice.CreateSampler(&static_cast<DX12Sampler*>(&a_Sampler)->GetSamplerDesc(), m_DescriptorHandle);
			m_DescriptorHandle.Offset(1, m_D3DDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER));
			return m_Offset++;
		}
		else
		{
			throw std::runtime_error("Panther DX12 ERROR: Trying to register sampler in descriptor heap with wrong type.\nType: "
				+ GetTypeString());
		}
	}

	uint32 DX12DescriptorHeap::RegisterRenderTarget(RenderTarget& a_RenderTarget)
	{
		if (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		{
			m_D3DDevice.CreateRenderTargetView(static_cast<DX12RenderTarget*>(&a_RenderTarget)->GetTargetBuffer(), nullptr, m_DescriptorHandle);
			m_DescriptorHandle.Offset(1, m_D3DDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
			return m_Offset++;
		}
		else
		{
			throw std::runtime_error("Panther DX12 ERROR: Trying to register render target in descriptor heap with wrong type.\nType: "
				+ GetTypeString());
		}
	}

	uint32 DX12DescriptorHeap::RegisterDepthStencil(ID3D12Resource & a_DepthStencil, D3D12_DEPTH_STENCIL_VIEW_DESC& a_DSVDesc)
	{
		if (m_Type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV)
		{
			m_D3DDevice.CreateDepthStencilView(&a_DepthStencil, &a_DSVDesc, m_DescriptorHandle);
			m_DescriptorHandle.Offset(1, m_D3DDevice.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
			return m_Offset++;
		}
		else
		{
			throw std::runtime_error("Panther DX12 ERROR: Trying to register depth-stencil in descriptor heap with wrong type.\nType: "
				+ GetTypeString());
		}
	}

	std::string DX12DescriptorHeap::GetTypeString()
	{
		switch (m_Type)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return "CBV/SRV/UAV";
			break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
			return "Sampler";
			break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			return "RTV";
			break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			return "DSV";
			break;
		default:
			return "UNKNOWN";
			break;
		}
	}

	void DX12DescriptorHeap::SetCPUHandleIndex(uint32 a_Index)
	{
		m_DescriptorHandle.InitOffsetted(m_D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), a_Index, m_D3DDevice.GetDescriptorHandleIncrementSize(m_Type));
	}
}