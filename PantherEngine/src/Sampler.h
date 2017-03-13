#pragma once

namespace Panther
{
	enum class SamplerTextureCoordinateMode { Wrap, Mirror, Clamp };

	class Sampler
	{
	public:
		Sampler() = default;
		~Sampler() = default;
	};
}