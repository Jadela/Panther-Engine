#include "Window.h"

#include <assert.h>

#include "../../Panther_Core/src/Exceptions.h"
#include "../../Panther_Core/src/Keys.h"
#include "../resource.h"
#include "Application.h"

namespace Panther
{
	LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return Window::Get().WindowProc(hwnd, msg, wParam, lParam);
	}

	Window* Window::m_Instance = nullptr;

	Window::Window(const std::wstring& a_WindowName, uint32 a_Width, uint32 a_Height, bool a_VSync, bool a_Windowed) :
		m_WindowName(a_WindowName), 
		m_Width(a_Width), 
		m_Height(a_Height), 
		m_VSync(a_VSync), 
		m_Windowed(a_Windowed)
	{
		assert(m_Instance == nullptr);
		m_Instance = this;

		WNDCLASSEX wndClass = { 0 };
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = MainWndProc;
		wndClass.hInstance = Application::Get().GetInstanceHandle();
		wndClass.hIcon = LoadIcon(wndClass.hInstance, MAKEINTRESOURCE(IDI_ICON1));
		wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = L"DX12DemoWindow";

		RegisterClassEx(&wndClass);
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));

		RECT windowRect = { 0, 0, (int32)m_Width, (int32)m_Height };
		AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, false);

		m_hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"DX12DemoWindow",
			m_WindowName.c_str(), WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			nullptr, nullptr, wndClass.hInstance, nullptr);
		ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));

		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);
	}

	Window::~Window()
	{
		if (IsValid())
			DestroyWindow(m_hWnd);
	}

	bool Window::IsValid() const
	{
		return (m_hWnd != nullptr);
	}

	HWND Window::GetHandle() const
	{
		return m_hWnd;
	}

	uint32 Window::GetWidth() const
	{
		return m_Width;
	}

	uint32 Window::GetHeight() const
	{
		return m_Height;
	}

	bool Window::GetVSync() const
	{
		return m_VSync;
	}

	bool Window::GetWindowed() const
	{
		return m_Windowed;
	}

	void Window::Resize(uint32 a_Width, uint32 a_Height)
	{
		m_Width = a_Width;
		m_Height = a_Height;
	}

	LRESULT Window::WindowProc(HWND a_WindowHandle, UINT a_Message, WPARAM a_WParam, LPARAM a_LParam)
	{
		Application& app = Application::Get();

		switch (a_Message)
		{
			// WM_DESTROY is sent when the window is being destroyed.
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
			// WM_SIZE is sent when the user resizes the window.  
			case WM_SIZE:
			{
				uint32 width = (uint32)LOWORD(a_LParam);
				uint32 height = (uint32)HIWORD(a_LParam);
				Resize(width, height);
				app.OnResize(width, height);
				return 0;
			}
			// Catch this message so to prevent the window from becoming too small.
			case WM_GETMINMAXINFO:
			{
				((MINMAXINFO*)a_LParam)->ptMinTrackSize.x = 200;
				((MINMAXINFO*)a_LParam)->ptMinTrackSize.y = 200;
				return 0;
			}
			case WM_DPICHANGED:
			{
				const RECT* rect = (RECT*)a_LParam;
				SetWindowPos(a_WindowHandle,
					nullptr,
					rect->left,
					rect->top,
					rect->right - rect->left,
					rect->bottom - rect->top,
					SWP_NOZORDER | SWP_NOACTIVATE);
				return 0;
			}
			case WM_KEYDOWN:
			{
				MSG charMsg;
				// Get the unicode character (UTF-16)
				uint32 c = 0;
				// For printable characters, the next a_Message will be WM_CHAR.
				// This message contains the character code we need to send the KeyPressed event.
				// Inspired by the SDL implementation.
				if (PeekMessage(&charMsg, a_WindowHandle, 0, 0, PM_NOREMOVE) && charMsg.message == WM_CHAR)
				{
					GetMessage(&charMsg, a_WindowHandle, 0, 0);
					c = (uint32)charMsg.wParam;
				}
				bool shift = GetAsyncKeyState(VK_SHIFT) > 0;
				bool control = GetAsyncKeyState(VK_CONTROL) > 0;
				bool alt = GetAsyncKeyState(VK_MENU) > 0;
				Key key = (Key)a_WParam;
				app.OnKeyDown(key, c, control, shift, alt);
				return 0;
			}
			case WM_KEYUP:
			{
				uint32 c = 0;
				uint32 scanCode = (a_LParam & 0x00FF0000) >> 16;

				// Determine which key was released by converting the key code and the scan code
				// to a printable character (if possible).
				// Inspired by the SDL implementation.
				ubyte8 keyboardState[256];
				GetKeyboardState(keyboardState);
				wchar_t translatedCharacters[4];
				if (int32 result = ToUnicodeEx((uint32)a_WParam, scanCode, keyboardState, translatedCharacters, 4, 0, NULL) > 0)
				{
					c = translatedCharacters[0];
				}
				bool shift = GetAsyncKeyState(VK_SHIFT) > 0;
				bool control = GetAsyncKeyState(VK_CONTROL) > 0;
				bool alt = GetAsyncKeyState(VK_MENU) > 0;
				Key key = (Key)a_WParam;
				app.OnKeyUp(key, c, control, shift, alt);
				return 0;
			}
			case WM_MOUSEMOVE:
			{
				int32 x = LOWORD(a_LParam);
				int32 y = HIWORD(a_LParam);
				app.OnMouseMove(x, y, (a_WParam & MK_LBUTTON) != 0, (a_WParam & MK_RBUTTON) != 0);
				return 0;
			}
		}
		return DefWindowProc(a_WindowHandle, a_Message, a_WParam, a_LParam);
	}
}