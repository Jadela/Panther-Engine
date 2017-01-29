#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers.
#endif

#include <windows.h>
#include <stdexcept>

#include "Application.h"
#include "../../Panther_Core/src/Core.h"

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
	catch (const std::runtime_error& e)
	{
		MessageBoxA(nullptr, e.what(), "Runtime Error", MB_OK | MB_ICONERROR);
		return 1;
	}
}