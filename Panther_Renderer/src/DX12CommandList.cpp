#include "DX12RendererPCH.h"
#include "DX12CommandList.h"

#include "DX12Renderer.h"
#include "DX12Material.h"
#include "DX12Mesh.h"

namespace Panther
{
	DX12CommandList::DX12CommandList(DX12Renderer& a_Renderer, D3D12_COMMAND_LIST_TYPE a_CommandListType, DX12Material* a_Material)
		: m_Renderer(a_Renderer)
		, m_CommandListType(a_CommandListType)
	{
		if (a_CommandListType != D3D12_COMMAND_LIST_TYPE_DIRECT && a_CommandListType != D3D12_COMMAND_LIST_TYPE_BUNDLE)
			throw std::runtime_error("Panther DX12 ERROR: Trying to create a command list of unsupported type!");

		ID3D12CommandAllocator* allocator = (m_CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT) 
			? m_Renderer.m_D3DCommandAllocator.Get() : m_Renderer.m_D3DBundleAllocator.Get();
		ID3D12PipelineState* PSO = a_Material ? a_Material->GetPSO() : nullptr;

		HRESULT hr = m_Renderer.m_D3DDevice->CreateCommandList(0, m_CommandListType, allocator, PSO, IID_PPV_ARGS(&m_CommandList));
		if (FAILED(hr))
			throw std::runtime_error("Panther DX12 ERROR: Creation of command list failed!");
	}

	DX12CommandList::~DX12CommandList()
	{
	}

	void DX12CommandList::SetMaterial(Material& a_Material, bool a_ResetState)
	{
		DX12Material* mat = static_cast<DX12Material*>(&a_Material);

		if (a_ResetState)
			m_CommandList->SetPipelineState(mat->GetPSO());
		m_CommandList->SetGraphicsRootSignature(mat->GetRootSig());
	}

	void DX12CommandList::SetMesh(Mesh& a_Mesh)
	{
		DX12Mesh* mesh = static_cast<DX12Mesh*>(&a_Mesh);

		m_CommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_CommandList->IASetVertexBuffers(0, 1, &mesh->m_VertexBufferView);
		m_CommandList->IASetIndexBuffer(&mesh->m_IndexBufferView);
	}

	void DX12CommandList::Reset(DX12Material* a_Material)
	{
		ID3D12CommandAllocator* allocator = (m_CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT)
			? m_Renderer.m_D3DCommandAllocator.Get() : m_Renderer.m_D3DBundleAllocator.Get();
		ID3D12PipelineState* PSO = a_Material ? a_Material->GetPSO() : nullptr;
		
		// However, when ExecuteCommandList() is called on a particular command
		// list, that command list can then be reset at any time and must be before
		// re-recording.
		m_CommandList->Reset(allocator, PSO);
	}
}
