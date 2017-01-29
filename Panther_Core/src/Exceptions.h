#pragma once

#include <winerror.h>
#include <string>

#include "Core.h"

namespace Panther
{
	struct WinException
	{
		WinException() = default;
		WinException(HRESULT a_HR, const std::wstring& a_FunctionName, const std::wstring& a_FileName, int32 a_LineNumber);

		std::wstring ToString() const;

		HRESULT m_ErrorCode = S_OK;
		std::wstring m_FunctionName;
		std::wstring m_FileName;
		int32 m_LineNumber = -1;
	};
}

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
	HRESULT hr__ = (x);                                               \
	std::wstring wfn = AnsiToWString(__FILE__);                       \
	if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif