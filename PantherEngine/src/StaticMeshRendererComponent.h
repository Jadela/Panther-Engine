#pragma once
#include "Component.h"

namespace Panther
{
	class Mesh;

	class StaticMeshRendererComponent final : public Component
	{
	public:
		StaticMeshRendererComponent(Mesh* a_Mesh);

	private:
		Mesh* m_Mesh;
	};
}