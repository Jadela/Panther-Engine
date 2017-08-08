#include "Entity.h"

#include "Component.h"

namespace Panther
{
	Entity::Entity()
	{
	}

	Entity::~Entity()
	{
	}

	void Entity::AddComponent(Component* a_Component)
	{
		a_Component->m_Entity = this;
		m_Components.push_back(std::unique_ptr<Component>(a_Component));
	}
}
