#pragma once

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class DisplayModeList
	{
	public:
		DisplayModeList(DXGI_MODE_DESC1* a_Modes, size_t a_NumModes) : m_Modes(a_Modes), m_NumModes(a_NumModes) {}
		~DisplayModeList() {}

		DXGI_MODE_DESC1* GetBestMatchingDisplayMode(uint32 a_Width, uint32 a_Height);

	private:
		DisplayModeList() = delete;
		std::unique_ptr<DXGI_MODE_DESC1[]> m_Modes;
		size_t m_NumModes;
	};
}