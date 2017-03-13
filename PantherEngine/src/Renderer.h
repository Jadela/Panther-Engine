#pragma once
#include "Core.h"
#include "DescriptorHeap.h"
#include "Sampler.h"

namespace Panther
{
	class Buffer;
	class CommandList;
	class Material;
	class Mesh;
	class Texture;
	class Window;

	class Renderer
	{
	public:
		Renderer(Window& window);
		virtual ~Renderer();

		virtual bool Initialize() = 0;
		virtual CommandList& StartRecording() = 0;
		virtual void SubmitCommandLists(CommandList** a_CommandLists, uint32 a_NumCommandLists) = 0;
		virtual void Synchronize() = 0;

		virtual Buffer* CreateBuffer(const uint32 a_NumElements, const size_t a_ElementSize) = 0;
		virtual Buffer* CreateBuffer(CommandList& a_CommandList, const void* a_Data, const size_t a_Size, const size_t a_ElementSize) = 0;
		virtual DescriptorHeap* CreateDescriptorHeap(uint32 a_Capacity, DescriptorHeap::DescriptorHeapType a_Type) = 0;
		virtual Texture* CreateTexture(const std::wstring& a_Path) = 0;
		virtual Material* CreateMaterial(uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity) = 0;
		virtual Mesh* CreateMesh() = 0;
		virtual Sampler* CreateSampler(Sampler::TextureCoordinateMode a_TextureCoordinateMode = Sampler::TextureCoordinateMode::Wrap) = 0;
		virtual CommandList* CreateCommandList(D3D12_COMMAND_LIST_TYPE a_Type, Material* a_Material) = 0;

		virtual CommandList& StartRender() = 0;
		virtual void EndRender(CommandList& a_CommandList) = 0;
		virtual void OnResize(uint32 a_Width, uint32 a_Height) = 0;

		Window& GetWindow() { return m_Window; }

		static const int32 NumBackBuffers = 2;
	protected:
		bool m_APIInitialized = false;
		Window& m_Window;

	private:
		Renderer() = delete;
		Renderer(const Renderer& other) = delete;
	};
}