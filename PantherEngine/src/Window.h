#pragma once

#include <windows.h>
#include <string>

#include "Core.h"

namespace Panther
{
	class Application;

	class Window
	{
	public:
		Window() = delete;
		Window(const std::wstring& a_WindowName, uint32 a_Width, uint32 a_Height, bool a_VSync, bool a_Windowed);
		Window(const Window& src) = delete;
		~Window();

		static Window& Get() { return *m_Instance; }

		bool IsValid() const;
		HWND GetHandle() const;
		uint32 GetWidth() const;
		uint32 GetHeight() const;
		bool GetVSync() const;
		bool GetWindowed() const;
		void Resize(uint32 a_Width, uint32 a_Height);

		LRESULT WindowProc(HWND a_WindowHandle, UINT a_Message, WPARAM a_WParam, LPARAM a_LParam);

	private:
		static Window* m_Instance;

		HWND m_hWnd = nullptr;
		std::wstring m_WindowName = L"Default window";
		uint32 m_Width = 800;
		uint32 m_Height = 600;
		bool m_VSync = false;
		bool m_Windowed = true;
	};
}