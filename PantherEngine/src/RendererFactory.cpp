#include "DX12RendererPCH.h"
#include "RendererFactory.h"
#include "DX12Renderer.h"

// TODO: Move over to new Panther projects.
#include "Window.h"

namespace Panther
{
	Renderer* RendererFactory::CreateRenderer(Window& a_Window, RendererType a_RendererType)
	{
		switch (a_RendererType)
		{
		case Panther::RendererFactory::RendererType::DX12RENDERER:
			return new Panther::DX12Renderer(a_Window);
			break;
		case Panther::RendererFactory::RendererType::VKRENDERER:
			MessageBox(a_Window.GetHandle(), L"Vulkan renderer is not implemented yet.", L"Application Error", MB_OK | MB_ICONERROR);
			return nullptr;
			break;
		default:
			MessageBox(a_Window.GetHandle(), L"Invalid renderer specified.", L"Application Error", MB_OK | MB_ICONERROR);
			return nullptr;
			break;
		}
	}
}