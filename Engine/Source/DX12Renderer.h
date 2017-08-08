#pragma once
#include "Renderer.h"

#include "d3dx12.h"
#include <dxgi1_5.h>
#include <wrl.h>

#include <memory>

namespace Panther
{
	class Adapter;
	class SwapChain;
	class DX12CommandList;
	class DX12DescriptorHeap;
	class DX12RenderTarget;

	class DX12Renderer final : public Renderer
	{
	public:
		DX12Renderer(Window& window);
		virtual ~DX12Renderer() final override;

		bool Initialize() final override;
		CommandList& StartRecording() final override;
		void SubmitCommandLists(CommandList** a_CommandLists, uint32 a_NumCommandLists) final override;
		void Synchronize() final override;

		Buffer* CreateBuffer(const uint32 a_NumElements, const size_t a_ElementSize) final override;
		Buffer* CreateBuffer(CommandList& a_CommandList, const void* a_Data, const uint32 a_NumElements, const size_t a_ElementSize) final override;
		DescriptorHeap* CreateDescriptorHeap(uint32 a_Capacity, DescriptorHeapType a_Type) final override;
		Texture* CreateTexture(const std::wstring& a_Path) final override;
		Material* CreateMaterial(Shader& a_Shader, DepthWrite a_DepthWriteEnabled) final override;
		Mesh* CreateMesh() final override;
		Sampler* CreateSampler(SamplerTextureCoordinateMode a_TextureCoordinateMode) final override;
		Shader* CreateShader() final override;
		CommandList* CreateCommandList(D3D12_COMMAND_LIST_TYPE a_Type, Material* a_Material) final override;

		CommandList& StartRender() final override;
		void EndRender(CommandList& a_CommandList) final override;
		void OnResize(uint32 a_Width, uint32 a_Height) final override;

		ID3D12Device& GetDevice() { return *m_D3DDevice.Get(); }
		ID3D12CommandAllocator* GetCommandAllocatorDirect() { return m_D3DCommandAllocator.Get(); }
		ID3D12CommandAllocator* GetCommandAllocatorBundle() { return m_D3DBundleAllocator.Get(); }
		DX12CommandList& GetCommandList() { return *m_CommandList.get(); }
		CD3DX12_VIEWPORT& GetViewport() { return m_D3DViewport; }
		CD3DX12_RECT& GetScissorRect() { return m_D3DRectScissor; }

	private:
		DX12Renderer(const DX12Renderer& other) = delete;

		Microsoft::WRL::ComPtr<IDXGIFactory5> CreateDXGIFactory();
		Microsoft::WRL::ComPtr<ID3D12Device> TryCreateD3D12DeviceForAdapter(Adapter& a_Adapter, const D3D_FEATURE_LEVEL* a_FeatureLevels, 
			uint32 a_FeatureLevelCount, D3D_FEATURE_LEVEL* out_FeatureLevel);
		void ResizeSwapChain(uint32 a_Width, uint32 a_Height);

	private:
		std::unique_ptr<Adapter> m_Adapter = nullptr;
		std::unique_ptr<DX12DescriptorHeap> m_RTVDescriptorHeap = nullptr;
		std::unique_ptr<DX12DescriptorHeap> m_DSVDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device> m_D3DDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_D3DCommandQueue = nullptr;
		std::unique_ptr<SwapChain> m_SwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_D3DCommandAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_D3DBundleAllocator = nullptr;
		CD3DX12_VIEWPORT m_D3DViewport;
		CD3DX12_RECT m_D3DRectScissor;
		std::unique_ptr<DX12CommandList> m_CommandList = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_D3DFence;
		uint64 m_FenceValue = 0;
	};
}