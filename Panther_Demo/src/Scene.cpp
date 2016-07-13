#include "Panther_Demo_PCH.h"
#include "Scene.h"

#include "../../Panther_Renderer/src/DX12Renderer.h"
#include "Window.h"

using namespace DirectX;

// No minus operator for vector types in the DirectX Math library? I guess we'll create our own!
XMINT2 operator-(const XMINT2& x0, const XMINT2& x1)
{
	return XMINT2(x0.x - x1.x, x0.y - x1.y);
}

namespace Panther
{
	Scene::Scene(Panther::Renderer& renderer)
		: m_Renderer(renderer)
	{}
	
	void Scene::UpdateMouseDelta(DirectX::XMINT2 a_NewMousePosition)
	{
		m_MousePositionDelta = m_PreviousMousePosition - a_NewMousePosition;
		m_PreviousMousePosition = a_NewMousePosition;
	}
}