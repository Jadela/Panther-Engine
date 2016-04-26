#pragma once
#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class Window
	{
	public:
		Window();
		Window(HWND a_hWnd, const std::wstring& a_WindowName, Panther::uint32 a_Width, Panther::uint32 a_Height, bool a_VSync, bool a_Windowed);
		Window(const Window& src) = delete;
		~Window();

		bool			IsValid()		const;
		HWND			GetHandle()		const;
		Panther::uint32	GetWidth()		const;
		Panther::uint32	GetHeight()		const;
		bool			GetVSync()		const;
		bool			GetWindowed()	const;
		void			Resize(Panther::uint32 a_Width, Panther::uint32 a_Height);

	private:
		HWND			m_hWnd;
		std::wstring	m_WindowName;
		Panther::uint32	m_Width;
		Panther::uint32	m_Height;
		bool			m_VSync;
		bool			m_Windowed;
	};
}