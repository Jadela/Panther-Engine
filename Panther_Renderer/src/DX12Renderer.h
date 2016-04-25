#include "Renderer.h"

namespace Panther
{
	class DX12CommandList;
	class DX12DescriptorHeap;

	class DX12Renderer final : public Renderer
	{
	public:
		DX12Renderer(Window& window);
		virtual ~DX12Renderer() final override;

		// Public methods
		bool Initialize() final override;
		CommandList& StartRecording() final override;
		void SubmitCommandLists(CommandList** a_CommandLists, uint32 a_NumCommandLists) final override;
		bool Synchronize() final override;

		std::unique_ptr<Buffer> CreateBuffer(const size_t a_Capacity) final override;
		std::unique_ptr<Buffer> CreateBuffer(CommandList& a_CommandList, const void* a_Data, const size_t a_Size, const size_t a_ElementSize) final override;
		std::unique_ptr<DescriptorHeap> CreateDescriptorHeap(uint32 a_Capacity, D3D12_DESCRIPTOR_HEAP_TYPE a_Type) final override;
		std::unique_ptr<Texture> CreateTexture(const std::wstring& a_Path) final override;
		std::unique_ptr<Material> CreateMaterial(uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity) final override;
		std::unique_ptr<Mesh> CreateMesh() final override;
		std::unique_ptr<Sampler> CreateSampler(Sampler::TextureCoordinateMode a_TextureCoordinateMode = Sampler::TextureCoordinateMode::Wrap) final override;
		std::unique_ptr<CommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE a_Type, Material* a_Material) final override;

		void StartRender() final override;
		void EndRender() final override;
		void OnResize(uint32 a_Width, uint32 a_Height) final override;

	private:
		friend class Scene;
		friend class DemoScene;

		friend class DX12Texture;
		friend class DX12Sampler;
		friend class DX12Buffer;
		friend class DX12Material;
		friend class DX12CommandList;

		// Private methods
		bool ResizeSwapChain(uint32 width, uint32 height);
		void Present();
		bool WaitForPreviousFrame();

		// Pipeline objects.
		std::unique_ptr<Panther::DX12DescriptorHeap> m_RTVDescriptorHeap = nullptr;
		std::unique_ptr<Panther::DX12DescriptorHeap> m_DSVDescriptorHeap = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Device> m_D3DDevice = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_D3DCommandQueue = nullptr;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_D3DSwapChain = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[2] = { nullptr, nullptr };
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencil = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_D3DCommandAllocator = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_D3DBundleAllocator = nullptr;
		D3D12_VIEWPORT m_D3DViewport;
		D3D12_RECT m_D3DRectScissor;
		DXGI_PRESENT_PARAMETERS m_PresentParameters;
		std::unique_ptr<DX12CommandList> m_CommandList = nullptr;

		// Synchronization objects.
		uint32 m_FrameIndex;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_D3DFence;
		uint64 m_FenceValue;
		HANDLE m_FenceEvent;

		DX12Renderer(const DX12Renderer& other) = delete;
	};
}