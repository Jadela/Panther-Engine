#pragma once
#include "Core.h"

#include <memory>
#include <string>

namespace Panther
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		void LoadFromDisk(const std::wstring& a_Path);
		virtual void Upload() = 0;

		uint32 GetWidth() { return m_Width; }
		uint32 GetHeight() { return m_Height; }
		ubyte8* GetDataPointer() { return m_Data.get(); }

	protected:
		void LoadTarga(const std::wstring& a_Path);

		std::unique_ptr<ubyte8[]> m_Data = nullptr;
		uint32 m_Width = 0;
		uint32 m_Height = 0;
	};
}