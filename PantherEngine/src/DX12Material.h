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
		DX12Material() = delete;
		DX12Material(const DX12Material&) = delete;
		DX12Material(DX12Renderer& a_Renderer, uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity);
		DX12Material(DX12Renderer& a_Renderer, DX12Shader& a_Shader, DepthWrite a_DepthWriteEnabled);
		~DX12Material();

		DescriptorSlot DeclareShaderDescriptor(DescriptorType a_Type, uint32 a_Amount, uint32 a_BaseShaderRegister, ShaderType a_VisibleToShader) final override;
		void DeclareInputParameter(std::string a_Semantic, InputType a_Type, uint32 a_VectorElementCount) final override;
		void LoadShader(std::wstring a_Path, std::string a_EntryPoint, ShaderType a_Type) final override;
		void Compile(DepthWrite a_DepthWrite = DepthWrite::On) final override;

		void SetResource(std::string a_ResourceNameInShader, DescriptorHeap& a_ResourceHeap, uint32 a_HeapOffset) final override;
		void Use(CommandList& a_CommandList) final override;

		ID3D12PipelineState* GetPSO();
		ID3D12RootSignature* GetRootSig();

	private:
		DX12Shader* m_Shader;
		std::unordered_map<uint32, CD3DX12_GPU_DESCRIPTOR_HANDLE> m_RootParameterBindings;

		// Renderer reference
		DX12Renderer& m_Renderer;

		// Shaders
		Microsoft::WRL::ComPtr<ID3DBlob> m_VertexBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> m_PixelBlob = nullptr;

		// Constants
		uint32 m_ConstantsCapacity = 0;
		std::vector<CD3DX12_DESCRIPTOR_RANGE> m_DescriptorRanges = {};
		std::vector<CD3DX12_ROOT_PARAMETER> m_RootParameters = {};

		// Input layout
		uint32 m_InputParameterCapacity = 0;
		std::vector<std::string> m_SemanticNames = {};
		std::vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout = {};
		uint32 m_InputLayoutOffset = 0;

		// Pipeline state
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState = nullptr;
	};
}