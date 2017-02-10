#include "DX12RendererPCH.h"
#include "DisplayModeList.h"

DXGI_MODE_DESC1* Panther::DisplayModeList::GetBestMatchingDisplayMode(uint32 a_Width, uint32 a_Height)
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
