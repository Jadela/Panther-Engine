#pragma once

#include <cstdint>

namespace Panther
{
	// Unsigned
	typedef uint8_t		uint8;
	typedef uint16_t	uint16;
	typedef uint32_t	uint32;
	typedef uint64_t	uint64;

	// Signed
	typedef int8_t		int8;
	typedef int16_t		int16;
	typedef int32_t		int32;
	typedef int64_t		int64;

	// Characters
	typedef char		ANSIChar;	// Character representation, platforms could use 7-bit ASCII, 8-bit EASCII or something else.
	typedef wchar_t		WideChar;	// Wide character representation, width varies per platform (Windows: 16-bit, Linux: 32-bit).
	typedef WideChar	Character;	// Character type used in the engine, change value here if desired.
}
