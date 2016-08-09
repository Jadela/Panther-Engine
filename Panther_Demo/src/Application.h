#pragma once

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class Renderer;
	class Scene;
	class Window;

	class Application
	{
	public:
		enum class GraphicsAPI { DIRECTX12, VULKAN };

		static void			Create(HINSTANCE hInstance);
		static void			Destroy();
		static Application& Get(); 

		bool CreateGameWindow(const std::wstring& a_WindowName, uint32 a_Width, uint32 a_Height, bool a_VSync, bool a_Windowed);
		bool CreateRenderer(GraphicsAPI a_GraphicsAPI);
		bool LoadDemoScene();
		int32 Run();
		void Quit();

	private:
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		Application() = delete;
		Application(HINSTANCE hInstance);
		Application(const Application& src) = delete;
		~Application();

		static Application*	m_Singleton;

		HINSTANCE	m_hInstance = nullptr;
		Window*		m_Window = nullptr;
		Renderer*	m_Renderer = nullptr;
		Scene*		m_Scene = nullptr;
		bool		m_RequestQuit = false;
	};
}