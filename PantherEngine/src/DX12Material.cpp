#include "DX12Material.h"

#include "DX12CommandList.h"
#include "DX12DescriptorHeap.h"
#include "DX12Shader.h"
#include "DX12Renderer.h"
#include "Exceptions.h"

#include <D3DCompiler.h>

namespace Panther
{
	DX12Material::DX12Material(DX12Renderer& a_Renderer, DX12Shader& a_Shader, DepthWrite a_DepthWriteEnabled)
		: m_Renderer(a_Renderer), m_Shader(a_Shader)
	{
		D3D12_DEPTH_STENCIL_DESC DSDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		DSDesc.DepthWriteMask = (a_DepthWriteEnabled == DepthWrite::On) ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

		D3D12_GRAPHICS_PIPELINE_STATE_DESC PSDesc = {};
		PSDesc.InputLayout = { m_Shader.GetInputLayoutPointer(), m_Shader.GetInputLayoutCount() };
		PSDesc.pRootSignature = m_Shader.GetRootSignature();
		PSDesc.VS = CD3DX12_SHADER_BYTECODE(m_Shader.GetVertexShaderBlob());
		PSDesc.PS = CD3DX12_SHADER_BYTECODE(m_Shader.GetPixelShaderBlob());
		PSDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PSDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PSDesc.DepthStencilState = DSDesc;
		PSDesc.SampleMask = UINT_MAX;
		PSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PSDesc.NumRenderTargets = 1;
		PSDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		PSDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		PSDesc.SampleDesc.Count = 1;
		ThrowIfFailed(m_Renderer.GetDevice().CreateGraphicsPipelineState(&PSDesc, IID_PPV_ARGS(&m_PipelineState)));
	}

	DX12Material::~DX12Material()
	{
	}

	void DX12Material::SetResource(std::string a_ResourceNameInShader, DescriptorHeap& a_ResourceHeap, uint32 a_HeapOffset)
	{
		uint32 shaderRootParameterIndex = m_Shader.GetRootParameterIndex(a_ResourceNameInShader);

		DX12DescriptorHeap* resourceHeap = static_cast<DX12DescriptorHeap*>(&a_ResourceHeap);
		CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeapGPUHandle(resourceHeap->GetDescriptorHeap().GetGPUDescriptorHandleForHeapStart(),
			a_HeapOffset, m_Renderer.GetDevice().GetDescriptorHandleIncrementSize(resourceHeap->GetType()));

		m_RootParameterBindings[shaderRootParameterIndex] = descriptorHeapGPUHandle;
	}

	void DX12Material::SetResource(std::string a_ResourceNameInShader, DescriptorHeap& a_ResourceHeap, uint32 a_HeapOffset, CommandList& a_CommandList)
	{
		uint32 shaderRootParameterIndex = m_Shader.GetRootParameterIndex(a_ResourceNameInShader);

		SetResource(a_ResourceNameInShader, a_ResourceHeap, a_HeapOffset);

		DX12CommandList* commandList = static_cast<DX12CommandList*>(&a_CommandList);
		ID3D12GraphicsCommandList& d3dCommandList(commandList->GetCommandList());
		d3dCommandList.SetGraphicsRootDescriptorTable(shaderRootParameterIndex, m_RootParameterBindings[shaderRootParameterIndex]);
	}

	void DX12Material::Use(CommandList& a_CommandList)
	{
		DX12CommandList* commandList = static_cast<DX12CommandList*>(&a_CommandList);
		ID3D12GraphicsCommandList& d3dCommandList(commandList->GetCommandList());

		d3dCommandList.SetGraphicsRootSignature(m_Shader.GetRootSignature());
		d3dCommandList.SetPipelineState(m_PipelineState.Get());

		for (auto iterator : m_RootParameterBindings)
		{
			d3dCommandList.SetGraphicsRootDescriptorTable(iterator.first, iterator.second);
		}
	}
}