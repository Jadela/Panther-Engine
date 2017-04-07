#include "Scene.h"

#include "Window.h"
#include "TextureManager.h"

namespace Panther
{
	Scene::Scene(Panther::Renderer& renderer)
		: m_Renderer(renderer)
	{
		for (int keyInt = (int)Key::None; keyInt != (int)Key::KeyCount; keyInt++)
		{
			m_KeyStates[static_cast<Key>(keyInt)] = false;
		}
	}

	Scene::~Scene()
	{
	}
	
	void Scene::UpdateMouseDelta(Vector a_NewMousePosition)
	{
		m_MousePositionDelta = m_PreviousMousePosition - a_NewMousePosition;
		m_PreviousMousePosition = a_NewMousePosition;
	}

	void Scene::OnKeyDown(Key a_Key)
	{
		m_KeyStates[a_Key] = true;
	}

	void Scene::OnKeyUp(Key a_Key)
	{
		m_KeyStates[a_Key] = false;
	}
}