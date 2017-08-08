#include "TextureManager.h"

#include "Renderer.h"
#include "Texture.h"

namespace Panther
{
	TextureManager::TextureManager(Renderer& a_Renderer, uint32 a_TextureCapacity)
		: m_Renderer(a_Renderer), m_TextureCapacity(a_TextureCapacity)
	{
		m_Textures = new ManagedTexture*[a_TextureCapacity];
	}

	TextureManager::~TextureManager()
	{
		delete m_Textures;
	}

	TextureID TextureManager::LoadTexture(const std::wstring& a_File)
	{
		ManagedTexture*& managedTexture = m_Textures[m_NextFreeSlot];
		managedTexture = new ManagedTexture();
		managedTexture->m_ID = m_NextTextureID++;
		managedTexture->m_Texture = std::unique_ptr<Texture>(m_Renderer.CreateTexture(a_File));
		return managedTexture->m_ID;
	}

	Texture* TextureManager::GetTexture(TextureID a_ID)
	{
		for (uint32 i = 0; i < m_TextureCapacity; ++i)
			if (m_Textures[i] != nullptr && m_Textures[i]->m_ID == a_ID)
				return m_Textures[i]->m_Texture.get();
		return nullptr;
	}
}