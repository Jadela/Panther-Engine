#include "Scene.h"

#include "Window.h"
#include "TextureManager.h"

namespace Panther
{
	Scene::Scene(Panther::Renderer& renderer)
		: m_Renderer(renderer)
	{
	}

	Scene::~Scene()
	{
	}
}
