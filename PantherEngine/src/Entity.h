#pragma once
#include <memory>
#include <vector>

namespace Panther
{
	class Component;

	class Entity
	{
	public:
		Entity();
		~Entity();

		void AddComponent(Component* a_Component);

	private:
		Entity(const Entity& a_Rhs) = delete;
		Entity& operator=(const Entity& a_Rhs) = delete;

		std::vector<std::unique_ptr<Component>> m_Components;
	};
}
