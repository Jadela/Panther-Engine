#pragma once
#include "Material.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

#include <unordered_map>
#include <vector>

namespace Panther
{
	class DX12Renderer;
	class DX12Shader;

	class DX12Material final : public Material
	{
	public:
		DX12Material(DX12Renderer& a_Renderer, DX12Shader& a_Shader, DepthWrite a_DepthWriteEnabled);
		~DX12Material();

		void SetResource(std::string a_ResourceNameInShader, DescriptorHeap& a_ResourceHeap, uint32 a_HeapOffset) final override;
		void SetResource(std::string a_ResourceNameInShader, DescriptorHeap& a_ResourceHeap, uint32 a_HeapOffset, CommandList& a_CommandList) final override;
		void Use(CommandList& a_CommandList) final override;

	private:
		DX12Material() = delete;
		DX12Material(const DX12Material&) = delete;

		DX12Renderer& m_Renderer;
		DX12Shader& m_Shader;
		std::unordered_map<uint32, CD3DX12_GPU_DESCRIPTOR_HANDLE> m_RootParameterBindings;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
	};
}