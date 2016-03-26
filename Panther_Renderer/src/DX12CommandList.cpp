#include "DX12RendererPCH.h"
#include "DX12CommandList.h"

#include "DX12DescriptorHeap.h"
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

	void DX12CommandList::SetDescriptorHeap(DescriptorHeap& a_DescriptorHeap, uint32 a_Slot, uint32 a_HeapElementOffset)
	{
		DX12DescriptorHeap* descriptorHeap = static_cast<DX12DescriptorHeap*>(&a_DescriptorHeap);
		CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeapGPUHandle(descriptorHeap->m_D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 
			a_HeapElementOffset, m_Renderer.m_D3DDevice->GetDescriptorHandleIncrementSize(descriptorHeap->m_Type));

		m_CommandList->SetGraphicsRootDescriptorTable(a_Slot, descriptorHeapGPUHandle);
	}

	void DX12CommandList::UseDescriptorHeaps(DescriptorHeap** a_DescriptorHeaps, const uint32 a_NumDescriptorHeaps)
	{
		ID3D12DescriptorHeap** D3DDescriptorHeaps = new ID3D12DescriptorHeap*[a_NumDescriptorHeaps];

		for (uint32 i = 0; i < a_NumDescriptorHeaps; ++i)
		{
			D3DDescriptorHeaps[i] = static_cast<DX12DescriptorHeap*>(a_DescriptorHeaps[i])->m_D3DDescriptorHeap.Get();
		}
		m_CommandList->SetDescriptorHeaps(a_NumDescriptorHeaps, D3DDescriptorHeaps);
		delete[] D3DDescriptorHeaps;
	}

	void DX12CommandList::Draw(uint32 a_NumIndices)
	{
		m_CommandList->DrawIndexedInstanced(a_NumIndices, 1, 0, 0, 0);
	}

	void DX12CommandList::Close()
	{
		HRESULT hr = m_CommandList->Close();
		if (FAILED(hr))
			throw std::runtime_error("Panther DX12 ERROR: Closure of DX12 command list failed, check logs for errors!");
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
