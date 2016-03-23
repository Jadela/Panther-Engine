#pragma once

#include "Material.h"

namespace Panther
{
	class DX12Renderer;

	class DX12Material : Material
	{
	public:
		DX12Material(DX12Renderer& a_Renderer, uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity);
		~DX12Material();

		void DeclareShaderConstant(ConstantType a_Type, uint32 a_Amount, uint32 a_BaseShaderRegister, ShaderType a_VisibleToShader) final override;
		void DeclareInputParameter(std::string a_Semantic, InputType a_Type, uint32 a_VectorElementCount) final override;
		void LoadShader(std::wstring a_Path, std::string a_EntryPoint, ShaderType a_Type) final override;
		void Compile() final override;

		ID3D12PipelineState* GetPSO();
		ID3D12RootSignature* GetRootSig();
	protected:

	private:
		DX12Material(const DX12Material&) = delete;

		// TODO (JDL): Get rid of this.
		friend class DemoScene;
		friend class DX12CommandList;

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