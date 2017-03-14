#pragma once
#include <memory>
#include <vector>

namespace Panther
{
	class ActorComponent;

	class Actor
	{
	public:
		Actor();
		Actor(const Actor& a_Rhs) = delete;
		Actor& operator=(const Actor& a_Rhs) = delete;
		~Actor();

	private:
		std::vector<std::unique_ptr<ActorComponent>> m_Components;
	};
}