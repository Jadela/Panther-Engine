#include "PCH.h"
#include "Application.h"

#include "../Panther_Core/src/Core.h"

const Panther::Application::RendererType rendertype = Panther::Application::RendererType::DX12RENDERER;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try
	{
		// Create and get the application
		Panther::Application::Create(hInstance);
		Panther::Application& app = Panther::Application::Get();

		// Create a window
		if (!app.CreateGameWindow(L"DX12 Demo", 800, 600, false, true))
			return -1;

		// Initialize the rendering system.
		if (!app.CreateRenderer(rendertype))
			return -1;

		// Load the demo scene.
		if (!app.LoadDemoScene())
			return -1;

		// Enter game loop.
		Panther::int32 exitCode = app.Run();

		// Cleanup resources.
		Panther::Application::Destroy();

		return exitCode;
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(NULL, e.what(), "Runtime Error", MB_OK | MB_ICONERROR);
		return -1;
	}
}