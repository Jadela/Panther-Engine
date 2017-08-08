#include "Application.h"

#include "Input.h"
#include "RendererFactory.h"
#include "Renderer.h"
#include "Window.h"
#include "DemoScene.h"

#include <assert.h>
#include <chrono>

namespace Panther
{
	Application* Application::m_App = nullptr;

	Application::Application(HINSTANCE hInstance)
		: m_InstanceHandle(hInstance)
	{
		assert(m_App == nullptr);
		m_App = this;
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

	bool Application::Initialize(EGraphicsAPI a_GraphicsAPI)
	{
		Input::Initialize();

		m_Window = new Window(L"DX12 Demo", 640, 480, true, true);

		if (!CreateRenderer(a_GraphicsAPI))
			return false;

		// NOTE (JDL): This should be done differently.
		if (!LoadDemoScene())
			return false;

		return true;
	}

	int32 Application::Run()
	{
		MSG msg = { 0 };
		auto startTimePoint = std::chrono::system_clock::now();
		auto endTimePoint = startTimePoint;
		static float totalTimeSeconds = 0.0f;

		while (!m_RequestQuit)
		{
			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
				{
					m_RequestQuit = true;
				}
			}
			else
			{
				startTimePoint = std::chrono::system_clock::now();
				std::chrono::duration<float> timeDeltaSeconds = endTimePoint - startTimePoint;
				endTimePoint = startTimePoint;

				totalTimeSeconds += timeDeltaSeconds.count();

				m_Scene->Update(totalTimeSeconds, timeDeltaSeconds.count());

				CommandList& commandList(m_Renderer->StartRender());
				m_Scene->Render(commandList);
				m_Renderer->EndRender(commandList);
			}
		}
		return static_cast<int32>(msg.wParam);
	}

	void Application::Quit()
	{
		Get().m_RequestQuit = true;
	}

	void Application::OnResize(int32 a_Width, int32 a_Height)
	{
		if (m_Renderer)
			m_Renderer->OnResize(a_Width, a_Height);
		if (m_Scene)
			m_Scene->OnResize(a_Width, a_Height);
	}

	void Application::OnKeyDown(Key a_Key)
	{
		Input::KeyDown(a_Key);
	}

	void Application::OnKeyUp(Key a_Key)
	{
		Input::KeyUp(a_Key);
	}

	void Application::OnMouseMove(int32 a_X, int32 a_Y)
	{
		Input::MouseMove(a_X, a_Y);
	}

	bool Application::CreateRenderer(EGraphicsAPI a_GraphicsAPI)
	{
		if (!m_Window || !m_Window->IsValid())
		{
			MessageBox(NULL, L"Window not found.", L"Application Error", MB_OK | MB_ICONERROR);
			return false;
		}

		switch (a_GraphicsAPI)
		{
		case EGraphicsAPI::DIRECTX12:
			m_Renderer = RendererFactory::CreateRenderer(*m_Window, RendererFactory::RendererType::DX12RENDERER);
			break;
		case EGraphicsAPI::VULKAN:
			m_Renderer = RendererFactory::CreateRenderer(*m_Window, RendererFactory::RendererType::VKRENDERER);
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
}
