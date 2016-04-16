#include "PCH.h"
#include "Application.h"
#include "../resource.h"
#include "Window.h"
#include "../Panther_Renderer/src/DX12Renderer.h"
#include "DemoScene.h"

#define WINDOW_CLASS_NAME L"DX12DemoWindow"

namespace Panther
{
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	Application* Application::m_Singleton = nullptr;

	Application::Application(HINSTANCE hInstance)
		: m_hInstance(hInstance)
	{
		WNDCLASSEX wndClass = { 0 };
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WndProc;
		wndClass.hInstance = m_hInstance;
		wndClass.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = WINDOW_CLASS_NAME;

		if (!RegisterClassEx(&wndClass))
		{
			MessageBox(NULL, L"Unable to register the window class.", L"Error", MB_OK | MB_ICONERROR);
			throw std::runtime_error("Unable to register the window class.");
		}
	}

	Application::~Application()
	{
		if (m_Scene)
		{
			m_Scene->Unload();
			delete m_Scene;
		}
		if (m_Renderer)
			delete m_Renderer;
		if (m_Window)
			delete m_Window;
	}

	void Application::Create(HINSTANCE hInstance)
	{
		if (!m_Singleton)
			m_Singleton = new Application(hInstance);
	}

	void Application::Destroy()
	{
		if (!m_Singleton)
		{
			delete m_Singleton;
			m_Singleton = nullptr;
		}
	}

	Application& Application::Get()
	{
		assert(m_Singleton);
		return *m_Singleton;
	}

	bool Application::CreateGameWindow(const std::wstring& a_WindowName, Panther::uint32 a_Width, Panther::uint32 a_Height, bool a_VSync, bool a_Windowed)
	{
		RECT windowRect = { 0, 0, (Panther::int32)a_Width, (Panther::int32)a_Height };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, WINDOW_CLASS_NAME,
			a_WindowName.c_str(), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr, nullptr, m_hInstance, nullptr);

		if (!hWnd)
		{
			MessageBox(NULL, L"Could not create the render window.", L"Windowing Error", MB_OK | MB_ICONERROR);
			return false;
		}

		m_Window = new Window(hWnd, a_WindowName, a_Width, a_Height, a_VSync, a_Windowed);

		ShowWindow(hWnd, SW_SHOW);
		UpdateWindow(hWnd);

		return true;
	}

	bool Application::CreateRenderer(RendererType a_RendererType)
	{
		if (!m_Window || !m_Window->IsValid())
		{
			MessageBox(NULL, L"Window not found.", L"Application Error", MB_OK | MB_ICONERROR);
			return false;
		}

		switch (a_RendererType)
		{
		case DX12RENDERER:
			m_Renderer = new Panther::DX12Renderer(*m_Window);
			break;
		case VKRENDERER:
			MessageBox(m_Window->GetHandle(), L"Vulkan renderer is not implemented yet.", L"Application Error", MB_OK | MB_ICONERROR);
			return false;
			break;
		default:
			MessageBox(m_Window->GetHandle(), L"Invalid renderer specified.", L"Application Error", MB_OK | MB_ICONERROR);
			return false;
			break;
		}
		return m_Renderer->Initialize();
	}

	bool Application::LoadDemoScene()
	{
		if (!m_Renderer)
		{
			MessageBox(m_Window->GetHandle(), L"Renderer needs to be initialized before scene can be loaded.", L"Application Error", MB_OK | MB_ICONERROR);
			return false;
		}

		m_Scene = new DemoScene(*m_Renderer);
		m_Scene->Load();
		return true;
	}

	Panther::int32 Application::Run()
	{
		MSG msg = { 0 };

		static DWORD previousTime = timeGetTime();
		static float totalTime = 0.0f;
		static const float targetFramerate = 30.0f;
		static const float maxTimeStep = 1.0f / targetFramerate;

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				DWORD currentTime = timeGetTime();
				float deltaTime = (currentTime - previousTime) / 1000.0f;
				previousTime = currentTime;

				// Cap the delta time to the max time step (useful if your 
				// debugging and you don't want the deltaTime value to explode.
				deltaTime = std::min<float>(deltaTime, maxTimeStep);
				totalTime += deltaTime;

				// Update
				m_Scene->Update(deltaTime);

				// Render
				m_Renderer->StartRender();
				m_Scene->Render();
				m_Renderer->EndRender();
			}
		}

		return static_cast<Panther::int32>(msg.wParam);
	}

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		Panther::Application& app = Panther::Application::Get();
		PAINTSTRUCT paintStruct;
		HDC hDC;

		switch (message)
		{
		case WM_PAINT:
		{
			hDC = BeginPaint(hwnd, &paintStruct);
			EndPaint(hwnd, &paintStruct);
		}
		break;
		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
		case WM_SIZE:
		{
			Panther::uint32 width = (Panther::uint32)LOWORD(lParam);
			Panther::uint32 height = (Panther::uint32)HIWORD(lParam);

			app.m_Window->Resize(width, height);
			if (app.m_Renderer)
				app.m_Renderer->OnResize(width, height);
			if (app.m_Scene)
				app.m_Scene->OnResize(width, height);
		}
		break;
		case WM_DPICHANGED:
		{
			Panther::uint32 x = (Panther::uint32)LOWORD(wParam);
			Panther::uint32 y = (Panther::uint32)HIWORD(wParam);
			RECT rect = *reinterpret_cast<RECT *>(lParam);
			if (!SetWindowPos(hwnd, 0, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOACTIVATE | SWP_NOZORDER))
				return 1;
		}
		break;
		case WM_MOUSEMOVE:
		{
			int32 x = LOWORD(lParam);
			int32 y = HIWORD(lParam);
			if (app.m_Scene)
			{
				app.m_Scene->OnMouseMove(x, y, (wParam & MK_LBUTTON) != 0);
			}
		}
		break;
		default:
			return DefWindowProc(hwnd, message, wParam, lParam);
		}
		return 0;
	}
}
