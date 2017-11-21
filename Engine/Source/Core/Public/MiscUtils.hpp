#pragma once

#include <cstdlib>

namespace Panther
{
	// countof constant expression: http://www.g-truc.net/post-0708.html
	template <typename T, std::size_t N>
	constexpr std::size_t Countof(T const (&)[N]) noexcept { return N; }
}
