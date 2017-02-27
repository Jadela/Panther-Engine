#include "DX12RendererPCH.h"
#include "DX12Renderer.h"

#include <sstream>

#include "Adapter.h"
#include "Output.h"
#include "DisplayModeList.h"
#include "SwapChain.h"

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
	DXGI_RATIONAL QueryRefreshRate(bool a_VSync, Adapter& a_Adapter)
	{
		DXGI_RATIONAL refreshRate = { 0, 1 };
		if (a_VSync)
		{
			Output& output(a_Adapter.GetOutput(0)); // Get primary output.

			DisplayModeList* modeList(output.GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM));
			if (modeList == nullptr)
			{
				throw new std::runtime_error("Primary display does not support DXGI_FORMAT_B8G8R8A8_UNORM.");
			}

			DXGI_MODE_DESC1* displayMode(modeList->GetHighestDisplayMode());
			if (displayMode == nullptr)
			{
				throw new std::runtime_error("Unable to get highest display mode.");
			}

			refreshRate = displayMode->RefreshRate;
		}
		return refreshRate;
	}

	DX12Renderer::DX12Renderer(Window& window)
		: Renderer(window)
	{}

	DX12Renderer::~DX12Renderer()
	{
		Synchronize();
	}

	bool DX12Renderer::Initialize()
	{
		if (!m_Window.IsValid()) return false; // Window used to initialize is invalid, jump out.
		if (m_APIInitialized) return true; // Renderer is already initialized.
		if (!XMVerifyCPUSupport()) throw std::runtime_error("CPU is not supporting SSE2 or NEON!");

#if defined(DEBUG) || defined(_DEBUG)
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))
		debugController->EnableDebugLayer();
#endif

		ComPtr<IDXGIFactory5> DXGIFactory = CreateDXGIFactory();
		m_Adapter = Adapter::GetAdapter(*DXGIFactory.Get(), 0); // Get primary adapter.

		// Create hardware-based D3D12 device. If that fails, fallback to WARP/software.
		{
			D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
			D3D_FEATURE_LEVEL chosenFeatureLevel = (D3D_FEATURE_LEVEL)0;
			m_D3DDevice = TryCreateD3D12DeviceForAdapter(*m_Adapter.get(), featureLevels, (uint32)Countof(featureLevels), &chosenFeatureLevel);

			if (!m_D3DDevice)
			{
				m_Adapter = Adapter::GetAdapter(*DXGIFactory.Get(), 0, true);
				m_D3DDevice = TryCreateD3D12DeviceForAdapter(*m_Adapter.get(), featureLevels, (uint32)Countof(featureLevels), &chosenFeatureLevel);
				if (!m_D3DDevice) throw std::runtime_error("Could not create Direct3D Device.");
			}
		}

		// Create a command queue for handling command lists.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ThrowIfFailed(m_D3DDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_D3DCommandQueue)))

		m_RTVDescriptorHeap = std::make_unique<DX12DescriptorHeap>(*m_D3DDevice.Get(), 2, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_DSVDescriptorHeap = std::make_unique<DX12DescriptorHeap>(*m_D3DDevice.Get(), 1, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

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
			swapChainFullScreenDesc.RefreshRate = QueryRefreshRate(m_Window.GetVSync(), *m_Adapter.get());
			swapChainFullScreenDesc.Windowed = m_Window.GetWindowed();

			m_SwapChain = SwapChain::CreateSwapchain(*DXGIFactory.Get(), *m_D3DDevice.Get(), *m_D3DCommandQueue.Get(), m_Window.GetHandle(), swapChainDesc, 
				&swapChainFullScreenDesc, *m_RTVDescriptorHeap.get(), *m_DSVDescriptorHeap.get());
		}

		ResizeSwapChain(m_Window.GetWidth(), m_Window.GetHeight());

		// Create command allocators.
		ThrowIfFailed(m_D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_D3DCommandAllocator)))
		ThrowIfFailed(m_D3DDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_BUNDLE, IID_PPV_ARGS(&m_D3DBundleAllocator)))

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

		// NOTE: Allocators can only be reset when the associated command lists have finished execution on the GPU.
		ThrowIfFailed(m_D3DCommandAllocator->Reset())

		if (m_CommandList == nullptr)
			m_CommandList = std::make_unique<DX12CommandList>(*this, D3D12_COMMAND_LIST_TYPE_DIRECT);
		else
			m_CommandList->Reset();

		return *m_CommandList.get();
	}

	void DX12Renderer::SubmitCommandLists(CommandList** a_CommandLists, uint32 a_NumCommandLists)
	{
		ID3D12CommandList** commandLists = new ID3D12CommandList*[a_NumCommandLists];
		for (uint32 i = 0; i < a_NumCommandLists; ++i)
			commandLists[i] = &static_cast<DX12CommandList*>(a_CommandLists[i])->GetCommandList();

		m_D3DCommandQueue->ExecuteCommandLists(a_NumCommandLists, commandLists);
		delete[] commandLists;
	}

	void DX12Renderer::Synchronize()
	{
		// TODO: Use frame resources instead of flushing the render queue every frame.
		m_FenceValue++;
		ThrowIfFailed(m_D3DCommandQueue->Signal(m_D3DFence.Get(), m_FenceValue));

		if (m_D3DFence->GetCompletedValue() < m_FenceValue)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			ThrowIfFailed(m_D3DFence->SetEventOnCompletion(m_FenceValue, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	std::unique_ptr<Buffer> DX12Renderer::CreateBuffer(const size_t a_Capacity)
	{
		assert(a_Capacity > 0);
		std::unique_ptr<Buffer> buffer = std::make_unique<DX12Buffer>(*this, a_Capacity);
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

	CommandList& DX12Renderer::StartRender()
	{
		CommandList& commandList(StartRecording());

		commandList.SetTransitionBarrier(*m_SwapChain.get(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList.SetAndClearRenderTarget(*m_SwapChain.get(), DirectX::Colors::CornflowerBlue);

		return commandList;
	}

	void DX12Renderer::EndRender(CommandList& a_CommandList)
	{
		a_CommandList.SetTransitionBarrier(*m_SwapChain.get(), D3D12_RESOURCE_STATE_PRESENT);

		a_CommandList.Close();
		CommandList* commandLists[] = { &a_CommandList };
		SubmitCommandLists(commandLists, 1);

		m_SwapChain->Present(m_Window.GetVSync());
		Synchronize();
	}

	void DX12Renderer::OnResize(uint32 a_Width, uint32 a_Height)
	{
		ResizeSwapChain(a_Width, a_Height);
	}

	ComPtr<IDXGIFactory5> DX12Renderer::CreateDXGIFactory()
	{
		ComPtr<IDXGIFactory5> DXGIFactory = nullptr;
		UINT flags = 0;
#if defined(_DEBUG)
		flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		ThrowIfFailed(CreateDXGIFactory2(flags, IID_PPV_ARGS(&DXGIFactory)));
		return DXGIFactory;
	}

	ComPtr<ID3D12Device> DX12Renderer::TryCreateD3D12DeviceForAdapter(Adapter& a_Adapter, const D3D_FEATURE_LEVEL* a_FeatureLevels,
		uint32 a_FeatureLevelCount, D3D_FEATURE_LEVEL* out_FeatureLevel)
	{
		ComPtr<ID3D12Device> D3D12Device = nullptr;
		for (uint32 i = 0; i < a_FeatureLevelCount; ++i)
		{
			if (SUCCEEDED(D3D12CreateDevice(&a_Adapter.GetAdapter(), a_FeatureLevels[i], IID_PPV_ARGS(&D3D12Device))))
			{
				*out_FeatureLevel = a_FeatureLevels[i];

				a_Adapter.LogProperties();
				std::wstring featureLevelString = std::to_wstring((a_FeatureLevels[i] >> 12) & 15) + L"." + std::to_wstring((a_FeatureLevels[i] >> 8) & 15);
				OutputDebugString((std::wstring(L"\n\tSuccessfully created a device with feature level:\t") + featureLevelString + L"\n").c_str());
				break;
			}
		}
		return D3D12Device;
	}

	void DX12Renderer::ResizeSwapChain(uint32 width, uint32 height)
	{
		m_SwapChain->Resize(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM);

		// Initialize viewport and scissor rect.
		ZeroMemory(&m_D3DViewport, sizeof(m_D3DViewport));
		m_D3DViewport.Width = static_cast<float>(m_Window.GetWidth());
		m_D3DViewport.Height = static_cast<float>(m_Window.GetHeight());
		m_D3DViewport.MaxDepth = 1.0f;

		ZeroMemory(&m_D3DRectScissor, sizeof(m_D3DRectScissor));
		m_D3DRectScissor.right = static_cast<LONG>(m_Window.GetWidth());
		m_D3DRectScissor.bottom = static_cast<LONG>(m_Window.GetHeight());
	}
}