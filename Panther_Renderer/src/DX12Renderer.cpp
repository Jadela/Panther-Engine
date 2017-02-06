#include "DX12RendererPCH.h"
#include "DX12Renderer.h"

#include <sstream>

#include "DX12Buffer.h"
#include "DX12CommandList.h"
#include "DX12DescriptorHeap.h"
#include "DX12Material.h"
#include "DX12Mesh.h"
#include "DX12RenderTarget.h"
#include "DX12Sampler.h"
#include "DX12Texture.h"
#include "../../Panther_Core/src/Exceptions.h"
// TODO: Move over to new Panther projects.
#include "../../Panther_Demo/src/Window.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Panther
{
	// This function was inspired by:
	// http://www.rastertek.com/dx11tut03.html
	// and is a refreshed version of Jeremiah van Oosten.
	DXGI_RATIONAL QueryRefreshRate(const Window& window)
	{
		DXGI_RATIONAL refreshRate = { 0, 1 };
		if (window.GetVSync())
		{
			ComPtr<IDXGIFactory2> factory;
			ComPtr<IDXGIAdapter> adapter;
			ComPtr<IDXGIOutput> adapterOutput;

			// Create a DirectX graphics interface factory.
			ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&factory)));

			// Get primary GPU.
			ThrowIfFailed(factory->EnumAdapters(0, &adapter));

			// Get primary display.
			ThrowIfFailed(adapter->EnumOutputs(0, &adapterOutput));

			// Get amount of display modes.
			uint32 numDisplayModes;
			ThrowIfFailed(adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr));
			if (numDisplayModes <= 0) throw new std::runtime_error("No available display modes.");

			// Get display modes.
			std::unique_ptr<DXGI_MODE_DESC[]> displayModeList(new DXGI_MODE_DESC[numDisplayModes]);
			ThrowIfFailed(adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList.get()));

			// Now store the refresh rate of the monitor that matches the width and height of the requested screen.
			for (uint32 i = 0; i < numDisplayModes; ++i)
			{
				if (displayModeList[i].Width == window.GetWidth() && displayModeList[i].Height == window.GetHeight())
				{
					refreshRate = displayModeList[i].RefreshRate;
				}
			}
		}
		return refreshRate;
	}

	DX12Renderer::DX12Renderer(Window& window)
		: Renderer(window)
	{}

	DX12Renderer::~DX12Renderer()
	{
		// Wait for the GPU to be done with all resources.
		WaitForPreviousFrame();
	}

	bool DX12Renderer::Initialize()
	{
		if (!m_Window.IsValid()) return false; // Window used to initialize is invalid, jump out.
		if (m_APIInitialized) return true; // Renderer is already initialized.
		if (!XMVerifyCPUSupport()) throw std::runtime_error("CPU is not supporting SSE2 or NEON!");

#if defined(DEBUG) || defined(_DEBUG)
		// Enable D3D12 debug layer.
		{
			ComPtr<ID3D12Debug> debugController;
			ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))
			debugController->EnableDebugLayer();
		}
#endif

		ComPtr<IDXGIFactory4> DXGIFactory = CreateDXGIFactory();

		// Create hardware-based D3D12 device. If that fails, fallback to WARP/software.
		{
			ComPtr<IDXGIAdapter1> adapterHolder;
			ComPtr<IDXGIAdapter3> adapter;
			D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
			D3D_FEATURE_LEVEL chosenFeatureLevel = (D3D_FEATURE_LEVEL)0;
			if (SUCCEEDED(DXGIFactory->EnumAdapters1(0, &adapterHolder)) && SUCCEEDED(adapterHolder.As(&adapter)))
			{
				m_D3DDevice = TryCreateD3D12DeviceForAdapter(*adapter.Get(), featureLevels, (uint32)Countof(featureLevels), &chosenFeatureLevel);
			}

			if (!m_D3DDevice)
			{
				ComPtr<IDXGIAdapter3> WARPAdapter;
				ThrowIfFailed(DXGIFactory->EnumWarpAdapter(IID_PPV_ARGS(&WARPAdapter)))
				m_D3DDevice = TryCreateD3D12DeviceForAdapter(*WARPAdapter.Get(), featureLevels, (uint32)Countof(featureLevels), &chosenFeatureLevel);
				if (!m_D3DDevice) throw std::runtime_error("Could not create Direct3D Device.");
			}
		}

		// Create a command queue for handling command lists.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ThrowIfFailed(m_D3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_D3DCommandQueue)))

		// Swap chain
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
			swapChainDesc.Width = m_Window.GetWidth();
			swapChainDesc.Height = m_Window.GetHeight();
			swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // Use Alt-Enter to switch between full screen and windowed mode.

			DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
			swapChainFullScreenDesc.RefreshRate = QueryRefreshRate(m_Window);
			swapChainFullScreenDesc.Windowed = m_Window.GetWindowed();

			m_D3DSwapChain = CreateSwapChain(DXGIFactory, m_D3DCommandQueue, m_Window.GetHandle(), swapChainDesc, &swapChainFullScreenDesc);
		}

		// Render target view (RTV) heap.
		uint32 numDescriptors = 2;
		m_RTVDescriptorHeap = std::make_unique<DX12DescriptorHeap>(*m_D3DDevice.Get(), numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		// Depth stencil view (DSV) heap.
		numDescriptors = 1;
		m_DSVDescriptorHeap = std::make_unique<DX12DescriptorHeap>(*m_D3DDevice.Get(), numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		// Create command allocators.
		ThrowIfFailed(m_D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_D3DCommandAllocator)))
		ThrowIfFailed(m_D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&m_D3DBundleAllocator)))

		ZeroMemory(&m_PresentParameters, sizeof(DXGI_PRESENT_PARAMETERS));

		if (!ResizeSwapChain(m_Window.GetWidth(), m_Window.GetHeight())) throw std::runtime_error("Failed to resize the swap chain.");

		// Create synchronization objects.
		ThrowIfFailed(m_D3DDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_D3DFence)))

		m_APIInitialized = true;
		return true;
	}

	CommandList& DX12Renderer::StartRecording()
	{
		// Validate before continuing
		assert(m_Window.IsValid());
		assert(m_D3DDevice);
		assert(m_APIInitialized);

		// Command list allocators can only be reset when the associated
		// command lists have finished execution on the GPU; apps should use
		// fences to determine GPU execution progress.
		ThrowIfFailed(m_D3DCommandAllocator->Reset())

		if (m_CommandList)
			m_CommandList->Reset();
		else
			m_CommandList = std::make_unique<DX12CommandList>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);

		return *m_CommandList.get();
	}

	void DX12Renderer::SubmitCommandLists(CommandList** a_CommandLists, uint32 a_NumCommandLists)
	{
		ID3D12CommandList** commandLists = new ID3D12CommandList*[a_NumCommandLists];
		for (uint32 i = 0; i < a_NumCommandLists; ++i)
			commandLists[i] = static_cast<DX12CommandList*>(a_CommandLists[i])->m_CommandList.Get();

		m_D3DCommandQueue->ExecuteCommandLists(a_NumCommandLists, commandLists);
		delete[] commandLists;
	}

	bool DX12Renderer::Synchronize()
	{
		return WaitForPreviousFrame();
	}

	std::unique_ptr<Buffer> DX12Renderer::CreateBuffer(const size_t a_Capacity)
	{
		assert(a_Capacity > 0);
		std::unique_ptr<Buffer> buffer = std::make_unique<DX12Buffer>(*this, a_Capacity);
		// TODO (JDL): Implement descriptor heaps in DX12Renderer.
		//m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*buffer.get());
		return std::move(buffer);
	}

	std::unique_ptr<Buffer> DX12Renderer::CreateBuffer(CommandList& a_CommandList, const void* a_Data, const size_t a_Size, const size_t a_ElementSize)
	{
		assert(a_Data != 0 && a_Size > 0 && a_ElementSize > 0);
		return std::make_unique<DX12Buffer>(*this, *static_cast<DX12CommandList*>(&a_CommandList), a_Data, a_Size, a_ElementSize);
	}

	std::unique_ptr<DescriptorHeap> DX12Renderer::CreateDescriptorHeap(uint32 a_Capacity, DescriptorHeap::DescriptorHeapType a_Type)
	{
		D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES;
		switch (a_Type)
		{
		case DescriptorHeap::DescriptorHeapType::ConstantBufferView:
		case DescriptorHeap::DescriptorHeapType::ShaderResourceView:
			type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			break;
		case DescriptorHeap::DescriptorHeapType::Sampler:
			type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			break;
		default:
			throw std::runtime_error("Attempting to create descriptor heap of unsupported type!");
			break;
		}

		return std::make_unique<DX12DescriptorHeap>(*m_D3DDevice.Get(), a_Capacity, type);
	}

	std::unique_ptr<Texture> DX12Renderer::CreateTexture(const std::wstring & a_Path)
	{
		// Load texture from disk and upload to GPU.
		std::unique_ptr<Texture> texture = std::make_unique<DX12Texture>(*this);
		texture->LoadFromDisk(a_Path);
		texture->Upload();
		return std::move(texture);
	}

	std::unique_ptr<Material> DX12Renderer::CreateMaterial(uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity)
	{
		return std::make_unique<DX12Material>(*this, a_ConstantsCapacity, a_InputParameterCapacity);
	}

	std::unique_ptr<Mesh> DX12Renderer::CreateMesh()
	{
		return std::make_unique<DX12Mesh>(*this);
	}

	std::unique_ptr<Sampler> DX12Renderer::CreateSampler(Sampler::TextureCoordinateMode a_TextureCoordinateMode)
	{
		return std::make_unique<DX12Sampler>((D3D12_TEXTURE_ADDRESS_MODE)((int)a_TextureCoordinateMode + 1));
	}

	std::unique_ptr<CommandList> DX12Renderer::CreateCommandList(D3D12_COMMAND_LIST_TYPE a_Type, Material* a_Material)
	{
		return std::make_unique<DX12CommandList>(*this, a_Type, static_cast<DX12Material*>(a_Material));
	}

	void DX12Renderer::StartRender()
	{
	}

	void DX12Renderer::EndRender()
	{
		Present();
		Synchronize();
	}

	void DX12Renderer::OnResize(uint32 a_Width, uint32 a_Height)
	{
		assert(m_D3DSwapChain);
		ResizeSwapChain(a_Width, a_Height);
	}

	ComPtr<IDXGIFactory4> DX12Renderer::CreateDXGIFactory()
	{
		ComPtr<IDXGIFactory4> DXGIFactory = nullptr;
#if defined(_DEBUG)
		// NOTE (JDL): Creating a debug version of the DXGI factory fails if the debug layer is not enabled.
		// In this case, we're throwing an exception.
		ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&DXGIFactory)))
#else
		ThrowIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&DXGIFactory)))
#endif
		return DXGIFactory;
	}

	ComPtr<IDXGISwapChain3> DX12Renderer::CreateSwapChain(ComPtr<IDXGIFactory4>& a_DXGIFactory, ComPtr<ID3D12CommandQueue>& a_CommandQueue,
		HWND a_Window, const DXGI_SWAP_CHAIN_DESC1& a_SwapChainDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* a_SwapChainFullscreenDesc)
	{
		ComPtr<IDXGISwapChain3> swapChainV3 = nullptr;
		ComPtr<IDXGISwapChain1> swapChainV1 = nullptr;
		// Swap chain needs the queue so that it can force a flush on it.
		ThrowIfFailed(a_DXGIFactory->CreateSwapChainForHwnd(a_CommandQueue.Get(), a_Window, &a_SwapChainDesc, a_SwapChainFullscreenDesc, nullptr, &swapChainV1))
		ThrowIfFailed(swapChainV1.As(&swapChainV3))
		return swapChainV3;
	}

	ComPtr<ID3D12Device> DX12Renderer::TryCreateD3D12DeviceForAdapter(IDXGIAdapter3& a_Adapter, const D3D_FEATURE_LEVEL* a_FeatureLevels,
		uint32 a_FeatureLevelCount, D3D_FEATURE_LEVEL* out_FeatureLevel)
	{
		ComPtr<ID3D12Device> D3D12Device = nullptr;
		for (uint32 i = 0; i < a_FeatureLevelCount; ++i)
		{
			if (SUCCEEDED(D3D12CreateDevice(&a_Adapter, a_FeatureLevels[i], IID_PPV_ARGS(&D3D12Device))))
			{
				DXGI_ADAPTER_DESC2 adapterDesc = { 0 };
				a_Adapter.GetDesc2(&adapterDesc);
				std::wstring featureLevelString = std::to_wstring((a_FeatureLevels[i] >> 12) & 15) + L"." + std::to_wstring((a_FeatureLevels[i] >> 8) & 15);

				OutputDebugString((L"\tSuccessfully created D3D12 device:\n\tAdapter:\t\t\t\t\t" + std::wstring(adapterDesc.Description)
					+ L"\n\tVendor ID:\t\t\t\t\t" + std::to_wstring(adapterDesc.VendorId)
					+ L"\n\tDevice ID:\t\t\t\t\t" + std::to_wstring(adapterDesc.DeviceId)
					+ L"\n\tSubsystem ID:\t\t\t\t" + std::to_wstring(adapterDesc.SubSysId)
					+ L"\n\tRevision:\t\t\t\t\t" + std::to_wstring(adapterDesc.Revision)
					+ L"\n\tDedicated Video Memory:\t\t" + std::to_wstring(adapterDesc.DedicatedVideoMemory / 1000000) + L" MB"
					+ L"\n\tDedicated System Memory:\t" + std::to_wstring(adapterDesc.DedicatedSystemMemory / 1000000) + L" MB"
					+ L"\n\tShared System Memory:\t\t" + std::to_wstring(adapterDesc.SharedSystemMemory / 1000000) + L" MB"
					+ L"\n\tFeature Level:\t\t\t\t" + featureLevelString + L"\n").c_str());
				*out_FeatureLevel = a_FeatureLevels[i];
				break;
			}
		}
		return D3D12Device;
	}

	inline bool DX12Renderer::ResizeSwapChain(uint32 width, uint32 height)
	{
		// Don't allow for 0 size swap chain buffers.
		if (width <= 0) width = 1;
		if (height <= 0) height = 1;

		// Release the render target views.
		for (int32 i = 0; i < 2; i++)
		{
			m_RenderTargets[i].reset();
		}
		m_depthStencil.Reset();

		// Resize the swap chain buffers.
		ThrowIfFailed(m_D3DSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0))

		m_FrameIndex = m_D3DSwapChain->GetCurrentBackBufferIndex();

		// Initialize new render target views.
		{
			m_RTVDescriptorHeap->SetCPUHandleIndex(0);
			// Create a RTV for each frame.
			for (uint32 n = 0; n < 2; n++)
			{
				m_RenderTargets[n] = std::make_unique<DX12RenderTarget>(*m_D3DSwapChain.Get(), n);
				m_RTVDescriptorHeap->RegisterRenderTarget(*m_RenderTargets[n].get());
			}
		}

		// Create the depth stencil view.
		{
			m_DSVDescriptorHeap->SetCPUHandleIndex(0);

			D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
			depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

			D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
			depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
			depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
			depthOptimizedClearValue.DepthStencil.Stencil = 0;

			ThrowIfFailed(m_D3DDevice->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &CD3DX12_RESOURCE_DESC::Tex2D(
					DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
				D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthOptimizedClearValue, IID_PPV_ARGS(&m_depthStencil)))

			m_DSVDescriptorHeap->RegisterDepthStencil(*m_depthStencil.Get(), depthStencilDesc);
		}

		// Initialize viewport and scissor rect.
		ZeroMemory(&m_D3DViewport, sizeof(m_D3DViewport));
		m_D3DViewport.Width = static_cast<float>(m_Window.GetWidth());
		m_D3DViewport.Height = static_cast<float>(m_Window.GetHeight());
		m_D3DViewport.MaxDepth = 1.0f;

		ZeroMemory(&m_D3DRectScissor, sizeof(m_D3DRectScissor));
		m_D3DRectScissor.right = static_cast<LONG>(m_Window.GetWidth());
		m_D3DRectScissor.bottom = static_cast<LONG>(m_Window.GetHeight());

		return true;
	}

	void DX12Renderer::Present()
	{
		if (m_Window.GetVSync())
			ThrowIfFailed(m_D3DSwapChain->Present1(1, 0, &m_PresentParameters))
		else
			ThrowIfFailed(m_D3DSwapChain->Present1(0, 0, &m_PresentParameters))
	}

	bool DX12Renderer::WaitForPreviousFrame()
	{
		// WAITING FOR THE FRAME TO COMPLETE BEFORE CONTINUING IS NOT BEST PRACTICE.
		// This is code implemented as such for simplicity. More advanced samples 
		// illustrate how to use fences for efficient resource usage.

		// Advance the fence value to mark commands up to this fence point.
		m_FenceValue++;

		// Add an instruction to the command queue to set a new fence point.  Because we 
		// are on the GPU timeline, the new fence point won't be set until the GPU finishes
		// processing all the commands prior to this Signal().
		ThrowIfFailed(m_D3DCommandQueue->Signal(m_D3DFence.Get(), m_FenceValue));

		// Wait until the GPU has completed commands up to this fence point.
		if (m_D3DFence->GetCompletedValue() < m_FenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

			// Fire event when GPU hits current fence.  
			ThrowIfFailed(m_D3DFence->SetEventOnCompletion(m_FenceValue, eventHandle));

			// Wait until the GPU hits current fence event is fired.
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}

		m_FrameIndex = m_D3DSwapChain->GetCurrentBackBufferIndex();
		return true;
	}
}