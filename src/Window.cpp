#include "PCH.h"
#include "Window.h"

namespace Panther
{
	Window::Window()
		: m_hWnd(nullptr), m_Width(0), m_Height(0)
	{
	}

	Window::Window(HWND a_hWnd, const std::wstring& a_WindowName, Panther::uint32 a_Width, Panther::uint32 a_Height, bool a_VSync, bool a_Windowed)
		: m_hWnd(a_hWnd), m_WindowName(a_WindowName), m_Width(a_Width), m_Height(a_Height), m_VSync(a_VSync), m_Windowed(a_Windowed)
	{
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

	Panther::uint32 Window::GetWidth() const
	{
		return m_Width;
	}

	Panther::uint32 Window::GetHeight() const
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

	void Window::Resize(Panther::uint32 a_Width, Panther::uint32 a_Height)
	{
		m_Width = a_Width;
		m_Height = a_Height;
	}
}