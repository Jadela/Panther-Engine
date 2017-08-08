#pragma once

namespace Panther
{
	class Renderer;
	class Window;

	class RendererFactory
	{
	public:
		enum class RendererType { DX12RENDERER, VKRENDERER };

		static Renderer* CreateRenderer(Window& a_Window, RendererType a_RendererType);
	};
}