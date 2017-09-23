#pragma once

#include "BuildConfigDefinitions.h"

#include <cstdlib> 
#include <cstdint>

namespace Panther
{
	// Typedefs
	typedef uint64_t	uint64;
	typedef int64_t		int64;
	typedef uint32_t	uint32;
	typedef int32_t		int32;
	typedef uint16_t	ushort16;
	typedef int16_t		short16;
	typedef uint8_t		ubyte8;
	typedef int8_t		byte8;

	// countof constant expression: http://www.g-truc.net/post-0708.html
	template <typename T, std::size_t N>
	constexpr std::size_t Countof(T const (&)[N]) noexcept { return N; }
}