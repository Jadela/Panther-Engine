#pragma once

#include "Core.h"
#include "Keys.h"

#ifndef _WINDEF_
struct HINSTANCE__; // Forward or never
typedef HINSTANCE__* HINSTANCE;
#endif

namespace Panther
{
	class Renderer;
	class Scene;
	class Window;

	class Application
	{
	public:
		enum class EGraphicsAPI { DIRECTX12, VULKAN };

		Application() = delete;
		Application(HINSTANCE hInstance);
		Application(const Application& rhs) = delete;
		Application& operator=(const Application& rhs) = delete;
		~Application();

		static Application& Get() { return *m_App; }
		bool Initialize(EGraphicsAPI a_GraphicsAPI);
		int32 Run();
		static void Quit();

		HINSTANCE& GetInstanceHandle() { return m_InstanceHandle; }

		void OnResize(int32 a_Width, int32 a_Height);		
		void OnKeyDown(Key a_Key);
		void OnKeyUp(Key a_Key);
		void OnMouseMove(int32 a_X, int32 a_Y);

	private:
		bool CreateRenderer(EGraphicsAPI a_GraphicsAPI);
		bool LoadDemoScene();

	private:
		static Application*	m_App;

		HINSTANCE	m_InstanceHandle = nullptr;
		Window*		m_Window = nullptr;
		Renderer*	m_Renderer = nullptr;
		Scene*		m_Scene = nullptr;
		bool		m_RequestQuit = false;
	};
}
