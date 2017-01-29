#pragma once

#include <windows.h>
#include <string>

#include "../../Panther_Core/src/Core.h"

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

		static Application& Get();
		bool Initialize(EGraphicsAPI a_GraphicsAPI);
		int32 Run();

		static void Quit();

		LRESULT WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		bool CreateGameWindow(const std::wstring& a_WindowName, uint32 a_Width, uint32 a_Height, bool a_VSync, bool a_Windowed);
		bool CreateRenderer(EGraphicsAPI a_GraphicsAPI);
		bool LoadDemoScene();

	private:
		static Application*	m_App;

		HINSTANCE	m_hInstance = nullptr;
		Window*		m_Window = nullptr;
		Renderer*	m_Renderer = nullptr;
		Scene*		m_Scene = nullptr;
		bool		m_RequestQuit = false;
	};
}