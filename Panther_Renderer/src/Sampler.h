#pragma once

namespace Panther
{
	class Sampler
	{
	public:
		Sampler() = default;
		~Sampler() = default;

		enum class TextureCoordinateMode { Wrap, Mirror, Clamp };
	};
}