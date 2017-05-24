#pragma once

namespace Panther
{
	class Entity;

	class Component
	{
	public:
		Component();
		virtual ~Component();

	protected:
		friend class Entity;
		Entity* m_Entity = nullptr;

	private:

	};
}
