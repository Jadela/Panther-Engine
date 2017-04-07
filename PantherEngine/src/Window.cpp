#include "Window.h"

#include "../resource.h"
#include "Exceptions.h"
#include "Keys.h"
#include "Application.h"

#include <assert.h>

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
				Key key = (Key)a_WParam;
				app.OnKeyDown(key);
				return 0;
			}
			case WM_KEYUP:
			{
				Key key = (Key)a_WParam;
				app.OnKeyUp(key);
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