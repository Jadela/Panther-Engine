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
		Entity(const Entity& a_Rhs) = delete;
		Entity& operator=(const Entity& a_Rhs) = delete;
		~Entity();

		void AddComponent(Component* a_Component);

	private:
		std::vector<std::unique_ptr<Component>> m_Components;
	};
}
