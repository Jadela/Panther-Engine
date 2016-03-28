#pragma once
#include "../Panther_Core/src/Core.h"

namespace Panther
{
	class Renderer;

	class Scene
	{
	public:
		Scene() = delete;
		Scene(Panther::Renderer& renderer);
		virtual ~Scene() {}

		virtual void Load() = 0;
		virtual void Unload() = 0;
		virtual void Update(float a_DT) = 0;
		virtual void Render() = 0;
		virtual void OnResize(Panther::uint32 a_Width, Panther::uint32 a_Height) = 0;

	protected:
		Panther::Renderer& m_Renderer;
	};
}