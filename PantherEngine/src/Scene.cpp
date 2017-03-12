#include "Scene.h"

#include "Window.h"
#include <../src/TextureManager.h>

namespace Panther
{
	Scene::Scene(Panther::Renderer& renderer)
		: m_Renderer(renderer)
	{}

	Scene::~Scene()
	{
	}
	
	void Scene::UpdateMouseDelta(Vector2<int> a_NewMousePosition)
	{
		m_MousePositionDelta = m_PreviousMousePosition - a_NewMousePosition;
		m_PreviousMousePosition = a_NewMousePosition;
	}
}