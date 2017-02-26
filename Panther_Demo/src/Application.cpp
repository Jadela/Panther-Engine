#include "Application.h"

#include <algorithm>
#include <assert.h>

#include "../../Panther_Renderer/src/RendererFactory.h"
#include "../resource.h"
#include "Window.h"
#include "DemoScene.h"

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

		static DWORD previousTime = timeGetTime();
		static float totalTime = 0.0f;
		static const float targetFramerate = 30.0f;
		static const float maxTimeStep = 1.0f / targetFramerate;

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
				DWORD currentTime = timeGetTime();
				float deltaTime = (currentTime - previousTime) / 1000.0f;
				previousTime = currentTime;

				// Cap the delta time to the max time step (useful if your 
				// debugging and you don't want the deltaTime value to explode.
				deltaTime = std::min<float>(deltaTime, maxTimeStep);
				totalTime += deltaTime;

				m_Scene->Update(deltaTime);

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

	void Application::OnKeyDown(Key a_Key, uint32 a_Character, bool a_Ctrl, bool a_Shift, bool a_Alt)
	{
		if (m_Scene)
		{
			m_Scene->OnKeyDown(a_Key, a_Character, KeyState::Pressed, a_Ctrl, a_Shift, a_Alt);
		}
	}

	void Application::OnKeyUp(Key a_Key, uint32 a_Character, bool a_Ctrl, bool a_Shift, bool a_Alt)
	{
		if (m_Scene)
		{
			m_Scene->OnKeyUp(a_Key, a_Character, KeyState::Released, a_Ctrl, a_Shift, a_Alt);
		}
	}

	void Application::OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown, bool a_RMBDown)
	{
		if (m_Scene)
		{
			m_Scene->OnMouseMove(a_DeltaX, a_DeltaY, a_LMBDown, a_RMBDown);
		}
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
