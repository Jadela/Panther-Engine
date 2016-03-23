#include "PCH.h"
#include "Scene.h"

#include "../Panther_Renderer/src/DX12Renderer.h"
#include "Window.h"

namespace Panther
{
	Scene::Scene(Panther::Renderer& renderer)
		: m_Renderer(renderer)
	{}
}