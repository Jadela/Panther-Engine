#include "Renderer.h"

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
		bool Synchronize() final override;

		std::unique_ptr<Buffer> CreateBuffer(const size_t a_Capacity) final override;
		std::unique_ptr<Buffer> CreateBuffer(CommandList& a_CommandList, const void* a_Data, const size_t a_Size, const size_t a_ElementSize) final override;
		std::unique_ptr<DescriptorHeap> CreateDescriptorHeap(uint32 a_Capacity, DescriptorHeap::DescriptorHeapType a_Type) final override;
		std::unique_ptr<Texture> CreateTexture(const std::wstring& a_Path) final override;
		std::unique_ptr<Material> CreateMaterial(uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity) final override;
		std::unique_ptr<Mesh> CreateMesh() final override;
		std::unique_ptr<Sampler> CreateSampler(Sampler::TextureCoordinateMode a_TextureCoordinateMode = Sampler::TextureCoordinateMode::Wrap) final override;
		std::unique_ptr<CommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE a_Type, Material* a_Material) final override;

		void StartRender() final override;
		void EndRender() final override;
		void OnResize(uint32 a_Width, uint32 a_Height) final override;

		DX12DescriptorHeap& GetRTVDescriptorHeap() { return *m_RTVDescriptorHeap.get(); }
		DX12DescriptorHeap& GetDSVDescriptorHeap() { return *m_DSVDescriptorHeap.get(); }
		ID3D12Device& GetDevice() { return *m_D3DDevice.Get(); }
		SwapChain& GetSwapChain() { return *m_SwapChain.get(); }
		ID3D12CommandAllocator* GetCommandAllocatorDirect() { return m_D3DCommandAllocator.Get(); }
		ID3D12CommandAllocator* GetCommandAllocatorBundle() { return m_D3DBundleAllocator.Get(); }
		DX12CommandList& GetCommandList() { return *m_CommandList.get(); }
		D3D12_VIEWPORT& GetViewport() { return m_D3DViewport; }
		D3D12_RECT& GetScissorRect() { return m_D3DRectScissor; }

	private:
		DX12Renderer(const DX12Renderer& other) = delete;

		Microsoft::WRL::ComPtr<IDXGIFactory5> CreateDXGIFactory();
		Microsoft::WRL::ComPtr<ID3D12Device> TryCreateD3D12DeviceForAdapter(IDXGIAdapter3& a_Adapter, const D3D_FEATURE_LEVEL* a_FeatureLevels, 
			uint32 a_FeatureLevelCount, D3D_FEATURE_LEVEL* out_FeatureLevel);
		void ResizeSwapChain(uint32 width, uint32 height);
		bool WaitForPreviousFrame();

	private:
		std::unique_ptr<Adapter> m_Adapter = nullptr;
		std::unique_ptr<DX12DescriptorHeap> m_RTVDescriptorHeap = nullptr;
		std::unique_ptr<DX12DescriptorHeap> m_DSVDescriptorHeap = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Device> m_D3DDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_D3DCommandQueue = nullptr;
		std::unique_ptr<SwapChain> m_SwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_D3DCommandAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_D3DBundleAllocator = nullptr;
		D3D12_VIEWPORT m_D3DViewport;
		D3D12_RECT m_D3DRectScissor;
		std::unique_ptr<DX12CommandList> m_CommandList = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_D3DFence;
		uint64 m_FenceValue = 0;
	};
}