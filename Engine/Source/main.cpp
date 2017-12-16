#include "Core.hpp"
#include "Exceptions.h"
#include "Application.h"

#include <windows.h>
#include <stdexcept>

using namespace Panther;

int WINAPI wWinMain(HINSTANCE a_InstanceHandle, HINSTANCE a_Nothing, PWSTR a_CommandLineArguments, int a_ShowCmd)
{
#if CONFIG_DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
	// Using this awareness context allows the client area of the window 
	// to achieve 100% scaling while still allowing non-client window content to 
	// be rendered in a DPI sensitive fashion.
	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	try
	{
		Application app(a_InstanceHandle);
		if (!app.Initialize(Application::EGraphicsAPI::DIRECTX12))
			return 1;

		return app.Run();
	}
	catch (WinException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"Panther::WinException", MB_OK | MB_ICONERROR | MB_DEFAULT_DESKTOP_ONLY);
		return 1;
	}
}