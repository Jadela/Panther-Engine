#include "DX12RendererPCH.h"
#include "Output.h"

#include "Exceptions.h"
#include "DisplayModeList.h"

namespace Panther
{
	Output::Output(IDXGIOutput4* a_Output) : m_Output(a_Output)
	{
	}

	DisplayModeList* Output::GetDisplayModeList(DXGI_FORMAT a_Format)
	{
		uint32 numDisplayModes;
		ThrowIfFailed(m_Output->GetDisplayModeList1(a_Format, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr));
		if (numDisplayModes <= 0)
		{
			return nullptr;
		}

		DXGI_MODE_DESC1* dxgiModes(new DXGI_MODE_DESC1[numDisplayModes]);
		ThrowIfFailed(m_Output->GetDisplayModeList1(a_Format, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, dxgiModes));

		return new DisplayModeList(dxgiModes, numDisplayModes);
	}
}
