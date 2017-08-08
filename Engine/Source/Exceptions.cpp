#include "Exceptions.h"

#include <comdef.h>

namespace Panther
{
	WinException::WinException(HRESULT a_HR, const std::wstring& a_FunctionName, const std::wstring& a_FileName, int32 a_LineNumber) :
		m_ErrorCode(a_HR),
		m_FunctionName(a_FunctionName),
		m_FileName(a_FileName),
		m_LineNumber(a_LineNumber)
	{
	}

	std::wstring WinException::ToString() const
	{
		// Get the string description of the error code.
		_com_error err(m_ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return m_FunctionName + L" failed in " + m_FileName + L"; line " + std::to_wstring(m_LineNumber) + L"; error: " + msg;
	}
}