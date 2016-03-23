#pragma once

#include "../Panther_Core/src/Core.h"

namespace Panther
{
	class Renderer;
	class Scene;
	class Window;

	class Application
	{
	public:
		enum RendererType { DX12RENDERER, VKRENDERER };

		static void			Create(HINSTANCE hInstance);
		static void			Destroy();
		static Application& Get();

		bool CreateGameWindow(const std::wstring& a_WindowName, Panther::uint32 a_Width, Panther::uint32 a_Height, bool a_VSync, bool a_Windowed);
		bool CreateRenderer(RendererType a_RendererType);
		bool LoadDemoScene();

		Panther::int32 Run();

	private:
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		Application() = delete;
		Application(HINSTANCE hInstance);
		Application(const Application& src) = delete;
		~Application();

		static Application*	m_Singleton;

		HINSTANCE			m_hInstance = nullptr;
		Window*				m_Window = nullptr;
		Panther::Renderer*	m_Renderer = nullptr;
		Scene*				m_Scene = nullptr;
	};
}