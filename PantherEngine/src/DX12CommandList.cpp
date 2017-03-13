#include "DX12CommandList.h"

#include "DX12DescriptorHeap.h"
#include "DX12Renderer.h"
#include "DX12Material.h"
#include "DX12Mesh.h"
#include "DX12RenderTarget.h"
#include "SwapChain.h"

namespace Panther
{
	DX12CommandList::DX12CommandList(DX12Renderer& a_Renderer, D3D12_COMMAND_LIST_TYPE a_CommandListType, DX12Material* a_Material)
		: m_Renderer(a_Renderer)
		, m_CommandListType(a_CommandListType)
	{
		if (a_CommandListType != D3D12_COMMAND_LIST_TYPE_DIRECT && a_CommandListType != D3D12_COMMAND_LIST_TYPE_BUNDLE)
			throw std::runtime_error("Panther DX12 ERROR: Trying to create a command list of unsupported type!");

		ID3D12CommandAllocator* allocator = (m_CommandListType == D3D12_COMMAND_LIST_TYPE_DIRECT) 
			? m_Renderer.GetCommandAllocatorDirect() : m_Renderer.GetCommandAllocatorBundle();
		ID3D12PipelineState* PSO = a_Material ? a_Material->GetPSO() : nullptr;

		HRESULT hr = m_Renderer.GetDevice().CreateCommandList(0, m_CommandListType, allocator, PSO, IID_PPV_ARGS(&m_CommandList));
		if (FAILED(hr))
			throw std::runtime_error("Panther DX12 ERROR: Creation of command list failed!");
	}

	DX12CommandList::~DX12CommandList()
	{
	}

	void DX12CommandList::SetAndClearRenderTarget(SwapChain& a_SwapChain, const float a_Color[4])
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(a_SwapChain.GetRTVDescriptorHandle());
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(a_SwapChain.GetDSVDescriptorHandle());

		m_CommandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
		m_CommandList->ClearRenderTargetView(rtvHandle, a_Color, 0, nullptr);
		m_CommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
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
		m_CommandList->IASetVertexBuffers(0, 1, &mesh->GetVertexBufferView());
		m_CommandList->IASetIndexBuffer(&mesh->GetIndexBufferView());
	}

	void DX12CommandList::SetDescriptorHeap(DescriptorHeap& a_DescriptorHeap, Material::DescriptorSlot& a_Slot, uint32 a_HeapElementOffset)
	{
		DX12DescriptorHeap* descriptorHeap = static_cast<DX12DescriptorHeap*>(&a_DescriptorHeap);
		CD3DX12_GPU_DESCRIPTOR_HANDLE descriptorHeapGPUHandle(descriptorHeap->GetDescriptorHeap().GetGPUDescriptorHandleForHeapStart(),
			a_HeapElementOffset, m_Renderer.GetDevice().GetDescriptorHandleIncrementSize(descriptorHeap->GetType()));

		m_CommandList->SetGraphicsRootDescriptorTable(a_Slot.m_Slot, descriptorHeapGPUHandle);
	}

	void DX12CommandList::UseDescriptorHeaps(DescriptorHeap** a_DescriptorHeaps, const uint32 a_NumDescriptorHeaps)
	{
		ID3D12DescriptorHeap** D3DDescriptorHeaps = new ID3D12DescriptorHeap*[a_NumDescriptorHeaps];
		for (uint32 i = 0; i < a_NumDescriptorHeaps; ++i)
			D3DDescriptorHeaps[i] = &static_cast<DX12DescriptorHeap*>(a_DescriptorHeaps[i])->GetDescriptorHeap();

		m_CommandList->SetDescriptorHeaps(a_NumDescriptorHeaps, D3DDescriptorHeaps);
		delete[] D3DDescriptorHeaps;
	}

	void DX12CommandList::UseDefaultViewport()
	{
		m_CommandList->RSSetViewports(1, &m_Renderer.GetViewport());
		m_CommandList->RSSetScissorRects(1, &m_Renderer.GetScissorRect());
	}

	void DX12CommandList::ExecuteBundle(CommandList& a_Bundle)
	{
		DX12CommandList* bundle = static_cast<DX12CommandList*>(&a_Bundle);
		if (bundle->m_CommandListType != D3D12_COMMAND_LIST_TYPE_BUNDLE) 
			throw std::runtime_error("Panther DX12 ERROR: ExecuteBundle received a commandlist that was not a bundle!");

		m_CommandList->ExecuteBundle(bundle->m_CommandList.Get());
	}

	void DX12CommandList::Draw(uint32 a_NumIndices)
	{
		m_CommandList->DrawIndexedInstanced(a_NumIndices, 1, 0, 0, 0);
	}

	void DX12CommandList::SetTransitionBarrier(SwapChain& a_SwapChain, D3D12_RESOURCE_STATES a_NewState)
	{
		m_CommandList->ResourceBarrier(1, &a_SwapChain.SetResourceState(a_NewState));
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
			? m_Renderer.GetCommandAllocatorDirect() : m_Renderer.GetCommandAllocatorBundle();
		ID3D12PipelineState* PSO = a_Material ? a_Material->GetPSO() : nullptr;
		
		// However, when ExecuteCommandList() is called on a particular command
		// list, that command list can then be reset at any time and must be before
		// re-recording.
		m_CommandList->Reset(allocator, PSO);
	}
}
