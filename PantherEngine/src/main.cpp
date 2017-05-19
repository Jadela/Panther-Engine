#include "Core.h"
#include "Exceptions.h"
#include "Application.h"

#include <windows.h>
#include <stdexcept>

using namespace Panther;

int WINAPI wWinMain(HINSTANCE a_InstanceHandle, HINSTANCE a_Nothing, PWSTR a_CommandLineArguments, int a_ShowCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

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