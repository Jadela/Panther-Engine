#pragma once
#include "Component.h"

namespace Panther
{
	class Mesh;

	class MeshComponent final : public Component
	{
	public:
		MeshComponent(Mesh* a_Mesh);

	private:
		Mesh* m_Mesh;
	};
}