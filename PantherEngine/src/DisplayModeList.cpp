#include "DX12RendererPCH.h"
#include "DisplayModeList.h"

namespace Panther
{
	DXGI_MODE_DESC1* DisplayModeList::GetBestMatchingDisplayMode(uint32 a_Width, uint32 a_Height)
	{
		DXGI_MODE_DESC1* bestMatch = nullptr;
		float bestRefresh = 0;

		for (int i = 0; i < m_NumModes; ++i)
		{
			float refresh = (float)m_Modes[i].RefreshRate.Numerator / m_Modes[i].RefreshRate.Denominator;
			if (m_Modes[i].Width == a_Width && m_Modes[i].Height == a_Height && refresh >= bestRefresh)
			{
				bestMatch = &m_Modes[i];
				bestRefresh = refresh;
			}
		}
		return bestMatch;
	}

	DXGI_MODE_DESC1* DisplayModeList::GetHighestDisplayMode()
	{
		DXGI_MODE_DESC1* bestMatch = nullptr;
		float bestRefresh = 0;

		for (int i = 0; i < m_NumModes; ++i)
		{
			float refresh = (float)m_Modes[i].RefreshRate.Numerator / m_Modes[i].RefreshRate.Denominator;
			if (bestMatch == nullptr || m_Modes[i].Width > bestMatch->Width && m_Modes[i].Height > bestMatch->Height)
			{
				bestMatch = &m_Modes[i];
				bestRefresh = refresh;
			}
			else if (m_Modes[i].Width == bestMatch->Width && m_Modes[i].Height == bestMatch->Height && refresh > bestRefresh)
			{
				bestMatch = &m_Modes[i];
				bestRefresh = refresh;
			}
		}
		return bestMatch;
	}
}