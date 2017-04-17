#include "DX12Shader.h"

#include "DX12Renderer.h"

#include "Exceptions.h"

using Microsoft::WRL::ComPtr;

namespace Panther
{
	DX12Shader::DX12Shader(ID3D12Device& a_Device) : m_Device(a_Device)
	{
	}

	DX12Shader::~DX12Shader()
	{
	}

	void DX12Shader::LoadVertexShader(std::wstring a_Path, std::string a_EntryPoint)
	{
		if (m_VertexCode != nullptr)
		{
			throw std::runtime_error("DX12SHADER: Vertex code not null while loading HLSL shader!");
		}
		m_VertexCode = LoadHLSLShader(a_Path, a_EntryPoint, "vs_5_0", (bool)(_DEBUG));
		ComPtr<ID3D12ShaderReflection> shaderReflection(ReflectShader(m_VertexCode.Get()));
		ObtainInputLayout(shaderReflection.Get());
		ObtainRootParameters(shaderReflection.Get(), D3D12_SHADER_VISIBILITY_VERTEX);
	}

	void DX12Shader::LoadPixelShader(std::wstring a_Path, std::string a_EntryPoint)
	{
		if (m_PixelCode != nullptr)
		{
			throw std::runtime_error("DX12SHADER: Pixel code not null while loading HLSL shader!");
		}
		m_PixelCode = LoadHLSLShader(a_Path, a_EntryPoint, "ps_5_0", (bool)(_DEBUG));
		ComPtr<ID3D12ShaderReflection> shaderReflection(ReflectShader(m_PixelCode.Get()));
		ObtainRootParameters(shaderReflection.Get(), D3D12_SHADER_VISIBILITY_PIXEL);
	}

	ID3DBlob* DX12Shader::LoadHLSLShader(std::wstring a_Path, std::string a_EntryPoint, std::string a_ShaderTarget, bool a_Debug)
	{
		uint32 compileFlags = a_Debug ? D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION : 0;
		ID3DBlob* shaderCode = nullptr;
		ComPtr<ID3DBlob> errorMessages = nullptr;

		// TODO (JDL): Throw a different kind of exception which includes errorMessages.
		ThrowIfFailed(D3DCompileFromFile(a_Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, a_EntryPoint.c_str(), a_ShaderTarget.c_str(), compileFlags, 0, &shaderCode, &errorMessages));

		return shaderCode;
	}

	ID3D12ShaderReflection* DX12Shader::ReflectShader(ID3DBlob* a_ShaderCode)
	{
		ID3D12ShaderReflection* shaderReflection;
		ThrowIfFailed(D3DReflect(a_ShaderCode->GetBufferPointer(), a_ShaderCode->GetBufferSize(), IID_PPV_ARGS(&shaderReflection)));
		return shaderReflection;
	}

	void DX12Shader::ObtainInputLayout(ID3D12ShaderReflection* a_Reflection)
	{
		D3D12_SHADER_DESC shaderInfo;
		a_Reflection->GetDesc(&shaderInfo);

		m_NumInputParameters = shaderInfo.InputParameters;
		m_InputLayout = std::make_unique<D3D12_INPUT_ELEMENT_DESC[]>(m_NumInputParameters);
		for (uint32 i = 0; i < m_NumInputParameters; ++i)
		{
			D3D12_SIGNATURE_PARAMETER_DESC inputParameterInfo;
			a_Reflection->GetInputParameterDesc(i, &inputParameterInfo);

			m_InputLayout[i].SemanticName = inputParameterInfo.SemanticName;
			m_InputLayout[i].SemanticIndex = inputParameterInfo.SemanticIndex;
			m_InputLayout[i].Format = GetInputElementFormatFromRegisterInfo(inputParameterInfo.Mask, inputParameterInfo.ComponentType);
			m_InputLayout[i].InputSlot = 0;
			m_InputLayout[i].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
			m_InputLayout[i].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			m_InputLayout[i].InstanceDataStepRate = 0;
		}
	}

	DXGI_FORMAT DX12Shader::GetInputElementFormatFromRegisterInfo(BYTE a_Mask, D3D_REGISTER_COMPONENT_TYPE a_Type)
	{
		if (a_Mask == 1)
		{
			if (a_Type == D3D_REGISTER_COMPONENT_UINT32) 
				return DXGI_FORMAT_R32_UINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_SINT32) 
				return DXGI_FORMAT_R32_SINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_FLOAT32) 
				return DXGI_FORMAT_R32_FLOAT;
		}
		else if (a_Mask <= 3)
		{
			if (a_Type == D3D_REGISTER_COMPONENT_UINT32) 
				return DXGI_FORMAT_R32G32_UINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_SINT32) 
				return DXGI_FORMAT_R32G32_SINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_FLOAT32) 
				return DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (a_Mask <= 7)
		{
			if (a_Type == D3D_REGISTER_COMPONENT_UINT32) 
				return DXGI_FORMAT_R32G32B32_UINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_SINT32) 
				return DXGI_FORMAT_R32G32B32_SINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_FLOAT32) 
				return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (a_Mask <= 15)
		{
			if (a_Type == D3D_REGISTER_COMPONENT_UINT32) 
				return DXGI_FORMAT_R32G32B32A32_UINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_SINT32) 
				return DXGI_FORMAT_R32G32B32A32_SINT;
			else if (a_Type == D3D_REGISTER_COMPONENT_FLOAT32) 
				return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		
		throw std::runtime_error("DX12SHADER: Unable to get input element format, supplied register info is not recognised!");
	}

	void DX12Shader::ObtainRootParameters(ID3D12ShaderReflection* a_Reflection, D3D12_SHADER_VISIBILITY a_ShaderVisibility)
	{
		D3D12_SHADER_DESC shaderInfo;
		a_Reflection->GetDesc(&shaderInfo);

		for (uint32 i = 0; i < shaderInfo.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC resourceInfo;
			a_Reflection->GetResourceBindingDesc(i, &resourceInfo);

			m_DescriptorRanges.push_back(
				std::make_unique<CD3DX12_DESCRIPTOR_RANGE1>(GetDescriptorRangeTypeFromShaderInputType(resourceInfo.Type), 1, resourceInfo.BindPoint));

			CD3DX12_ROOT_PARAMETER1 rootParameter;
			rootParameter.InitAsDescriptorTable(1U, m_DescriptorRanges.back().get(), a_ShaderVisibility);
			m_RootParameters.push_back(rootParameter);
		}
	}

	D3D12_DESCRIPTOR_RANGE_TYPE DX12Shader::GetDescriptorRangeTypeFromShaderInputType(D3D_SHADER_INPUT_TYPE a_InputType)
	{
		switch (a_InputType)
		{
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_CBUFFER:
			return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_TEXTURE:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case D3D_SHADER_INPUT_TYPE::D3D_SIT_SAMPLER:
			return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default:
			throw std::runtime_error("DX12SHADER: Unrecognised shader input type!");
		}
	}

	void DX12Shader::Finalize()
	{
		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc((uint32)m_RootParameters.size(), &m_RootParameters[0], 0U, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		HRESULT hr = D3D12SerializeVersionedRootSignature(&rootSignatureDesc, &signature, &error);
		if (FAILED(hr))
		{
			std::string errorString = static_cast<char*>(error->GetBufferPointer());
			throw std::runtime_error("Could not serialize root signature: " + errorString);
		}
		hr = m_Device.CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		if (FAILED(hr)) throw std::runtime_error("Could not create root signature.");
	}
}