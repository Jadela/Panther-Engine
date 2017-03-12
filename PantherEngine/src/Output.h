#pragma once

#include "../../Panther_Demo/src/Core.h"

namespace Panther
{
	class DisplayModeList;

	class Output
	{
	public:
		Output(IDXGIOutput4* a_Output);

		DisplayModeList* GetDisplayModeList(DXGI_FORMAT a_Format);

	private:
		Output(const Output&) = delete;

		const Microsoft::WRL::ComPtr<IDXGIOutput4> m_Output;
	};
}