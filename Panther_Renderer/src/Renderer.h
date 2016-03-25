#pragma once
#include "../../Panther_Core/src/Core.h"

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
		virtual bool StopRecordingAndSubmit() = 0;
		virtual bool Synchronize() = 0;

		virtual std::unique_ptr<Buffer> CreateBuffer(const size_t a_Capacity) = 0;
		virtual std::unique_ptr<Buffer> CreateBuffer(CommandList& a_CommandList, const void* a_Data, const size_t a_Size, const size_t a_ElementSize) = 0;
		virtual std::unique_ptr<Texture> CreateTexture(const std::wstring& a_Path) = 0;
		virtual std::unique_ptr<Material> CreateMaterial(uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity) = 0;
		virtual std::unique_ptr<Mesh> CreateMesh() = 0;
		virtual std::unique_ptr<CommandList> CreateCommandList(D3D12_COMMAND_LIST_TYPE a_Type, Material* a_Material) = 0;

		virtual void StartRender() = 0;
		virtual void EndRender() = 0;
		virtual void OnResize(uint32 a_Width, uint32 a_Height) = 0;

	protected:
		friend class Scene;
		friend class DemoScene;

		bool m_APIInitialized = false;
		Window &m_Window;

	private:
		Renderer() = delete;
		Renderer(const Renderer& other) = delete;
	};
}