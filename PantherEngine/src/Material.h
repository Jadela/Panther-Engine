#pragma once
#include "Core.h"

#include <string>

namespace Panther
{
	enum class DepthWrite : ubyte8 { Off, On };
	class CommandList;
	class DescriptorHeap;

	class Material
	{
	public:
		virtual ~Material() {}

		virtual void SetWireframe(bool a_Enabled) = 0;
		virtual void SetResource(std::string a_ResourceNameInShader, DescriptorHeap& a_ResourceHeap, uint32 a_HeapOffset) = 0;
		virtual void SetResource(std::string a_ResourceNameInShader, DescriptorHeap& a_ResourceHeap, uint32 a_HeapOffset, CommandList& a_CommandList) = 0;
		virtual void Use(CommandList& a_CommandList) = 0;

	protected:
		Material() {}

	private:
		Material(const Material&) = delete;
	};
}