#include "Scene.h"

#include "Window.h"
#include "TextureManager.h"

namespace Panther
{
	Scene::Scene(Panther::Renderer& renderer)
		: m_Renderer(renderer)
	{}

	Scene::~Scene()
	{
	}
	
	void Scene::UpdateMouseDelta(Vector a_NewMousePosition)
	{
		m_MousePositionDelta = m_PreviousMousePosition - a_NewMousePosition;
		m_PreviousMousePosition = a_NewMousePosition;
	}
}