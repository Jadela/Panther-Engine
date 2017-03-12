#pragma once

#include <string>
#include <memory>
#include "D3D12.h" // TODO: Remove DX12 types in Descriptorheap.h & get rid of this include.

#include "Core.h"
#include "Renderer.h"
#include "Texture.h"

namespace Panther
{
	using TextureID = uint32;

	class TextureManager final
	{
		struct ManagedTexture
		{
			TextureID m_ID;
			std::unique_ptr<Texture> m_Texture;
		};

	public:
		TextureManager(Renderer& a_Renderer, uint32 a_TextureCapacity);
		~TextureManager();

		TextureID LoadTexture(const std::wstring& a_File);

		Texture* GetTexture(TextureID a_ID);
	private:
		Renderer& m_Renderer;
		const uint32 m_TextureCapacity;
		ManagedTexture** m_Textures;
		uint32 m_NextFreeSlot = 0;
		uint32 m_NextTextureID = 0;
	};
}