#pragma once
#include "Shader.h"

#include "Core.h"

#include <D3DCompiler.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>

#include <memory>
#include <vector>
#include <unordered_map>

namespace Panther
{
	class DX12Shader final : public Shader
	{
	public:
		DX12Shader() = delete;
		DX12Shader(ID3D12Device& a_Device);
		~DX12Shader();

		void LoadVertexShader(std::wstring a_Path, std::string a_EntryPoint) final override;
		void LoadPixelShader(std::wstring a_Path, std::string a_EntryPoint) final override;
		void Finalize() final override;

		D3D12_INPUT_ELEMENT_DESC* GetInputLayoutPointer() { return m_InputLayout.get(); }
		uint32 GetInputLayoutCount() { return m_InputParameterCount; }
		ID3D12RootSignature* GetRootSignature() { return m_RootSignature.Get(); }
		ID3DBlob* GetVertexShaderBlob() { return m_VertexCode.Get(); }
		ID3DBlob* GetPixelShaderBlob() { return m_PixelCode.Get(); }
		uint32 GetRootParameterIndex(std::string a_RootParameterName);

	private:
		ID3DBlob* LoadHLSLShader(std::wstring a_Path, std::string a_EntryPoint, std::string a_ShaderTarget, bool a_Debug);
		ID3D12ShaderReflection* ReflectShader(ID3DBlob* a_ShaderCode);
		void ObtainInputLayout(ID3D12ShaderReflection* a_Reflection);
		DXGI_FORMAT GetInputElementFormatFromRegisterInfo(BYTE a_Mask, D3D_REGISTER_COMPONENT_TYPE a_Type);
		void ObtainRootParameters(ID3D12ShaderReflection* a_Reflection, D3D12_SHADER_VISIBILITY a_ShaderVisibility);
		D3D12_DESCRIPTOR_RANGE_TYPE GetDescriptorRangeTypeFromShaderInputType(D3D_SHADER_INPUT_TYPE a_InputType);

		ID3D12Device& m_Device;

		Microsoft::WRL::ComPtr<ID3DBlob> m_VertexCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> m_PixelCode = nullptr;
		std::unique_ptr<D3D12_INPUT_ELEMENT_DESC[]> m_InputLayout;
		uint32 m_InputParameterCount;
		std::vector<std::unique_ptr<CD3DX12_DESCRIPTOR_RANGE1>> m_DescriptorRanges;
		std::vector<CD3DX12_ROOT_PARAMETER1> m_RootParameters; 
		std::unordered_map<std::string, uint32> m_RootParameterIndices;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature = nullptr;
	};
}