#include "DX12RendererPCH.h"
#include "DX12Material.h"
#include "DX12Renderer.h"

namespace Panther
{
	DX12Material::DX12Material(DX12Renderer& a_Renderer, uint32 a_ConstantsCapacity, uint32 a_InputParameterCapacity)
		: m_ConstantsCapacity(a_ConstantsCapacity),
		m_InputParameterCapacity(a_InputParameterCapacity),
		m_Renderer(a_Renderer)
	{
		m_DescriptorRanges.reserve(m_ConstantsCapacity);
		m_RootParameters.reserve(m_ConstantsCapacity);
		m_SemanticNames.reserve(m_InputParameterCapacity);
		m_InputLayout.reserve(m_InputParameterCapacity);
	}

	DX12Material::~DX12Material()
	{
	}

	Material::DescriptorSlot DX12Material::DeclareShaderDescriptor(DescriptorType a_Type, uint32 a_Amount, uint32 a_BaseShaderRegister, ShaderType a_VisibleToShader)
	{
		if (m_RootParameters.size() >= m_ConstantsCapacity)
		{
			throw std::runtime_error("Surpassed constants capacity!");
		}

		D3D12_DESCRIPTOR_RANGE_TYPE descRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		switch (a_Type)
		{
		case DescriptorType::ShaderResource:
			descRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			break;
		case DescriptorType::ConstantBuffer:
			descRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
			break;
		case DescriptorType::Sampler:
			descRangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
			break;
		default:
			throw std::runtime_error("Trying to define shader constant of unknown type!");
			break;
		}

		D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		switch (a_VisibleToShader)
		{
		case ShaderType::Vertex:
			shaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
			break;
		case ShaderType::Pixel:
			shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			break;
		default:
			throw std::runtime_error("Unknown shader visibility specified!");
			break;
		}

		DescriptorSlot descriptorSlot;
		descriptorSlot.m_Slot = (uint32)m_DescriptorRanges.size();

		// Define a single CBV parameter.
		CD3DX12_DESCRIPTOR_RANGE descriptorRange(descRangeType, a_Amount, a_BaseShaderRegister);
		m_DescriptorRanges.push_back(descriptorRange);

		CD3DX12_ROOT_PARAMETER rootParameter;
		rootParameter.InitAsDescriptorTable(1, &m_DescriptorRanges.back(), shaderVisibility);
		m_RootParameters.push_back(rootParameter);

		return descriptorSlot;
	}

	void DX12Material::DeclareInputParameter(std::string a_Semantic, InputType a_Type, uint32 a_VectorElementCount)
	{
		if (m_InputLayout.size() >= m_InputParameterCapacity)
		{
			throw std::runtime_error("Surpassed input paramater capacity!");
		}

		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		uint32 typeOffset = 0;
		switch (a_Type)
		{
		case InputType::Float:
			if (a_VectorElementCount == 1)
			{
				format = DXGI_FORMAT_R32_FLOAT;
				typeOffset = 4;
			}
			else if (a_VectorElementCount == 2)
			{
				format = DXGI_FORMAT_R32G32_FLOAT;
				typeOffset = 8;
			}
			else if (a_VectorElementCount == 3)
			{
				format = DXGI_FORMAT_R32G32B32_FLOAT;
				typeOffset = 12;
			}
			else if (a_VectorElementCount == 4)
			{
				format = DXGI_FORMAT_R32G32B32A32_FLOAT;
				typeOffset = 16;
			}
			else
				throw std::runtime_error("Trying to define input parameter with invalid vector element count!");
			break;
		default:
			throw std::runtime_error("Trying to define input parameter of unknown type!");
			break;
		}

		m_SemanticNames.push_back(a_Semantic.c_str());

		D3D12_INPUT_ELEMENT_DESC inputElement;
		inputElement.SemanticName = m_SemanticNames.back().c_str();
		inputElement.SemanticIndex = 0;
		inputElement.Format = format;
		inputElement.InputSlot = 0;
		inputElement.AlignedByteOffset = m_InputLayoutOffset;
		inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate = 0;

		m_InputLayout.push_back(inputElement);
		m_InputLayoutOffset += typeOffset;
	}

	void DX12Material::LoadShader(std::wstring a_Path, std::string a_EntryPoint, ShaderType a_Type)
	{
		HRESULT hr = S_OK;
#ifdef _DEBUG
		Panther::uint32 compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
		Panther::uint32 compileFlags = 0;
#endif
		ID3DBlob* errorBlob = nullptr;
		switch (a_Type)
		{
		case ShaderType::Vertex:
			if (m_VertexBlob != nullptr) throw std::runtime_error("Vertex shader has already been loaded for this material before!");
			hr = D3DCompileFromFile(a_Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, a_EntryPoint.c_str(), "vs_5_0", compileFlags, 0, &m_VertexBlob, &errorBlob);
			break;
		case ShaderType::Pixel:
			if (m_PixelBlob != nullptr) throw std::runtime_error("Pixel shader has already been loaded for this material before!");
			hr = D3DCompileFromFile(a_Path.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, a_EntryPoint.c_str(), "ps_5_0", compileFlags, 0, &m_PixelBlob, &errorBlob);
			break;
		default:
			throw std::runtime_error("Trying to load shader of unknown type!");
			break;
		}
		if (FAILED(hr)) throw std::runtime_error("Could not load shader: " + std::string((char*)errorBlob->GetBufferPointer()));
	}

	void DX12Material::Compile(DepthWrite a_DepthWrite)
	{
		// Compile the root signature.
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init((uint32)m_RootParameters.size(), &m_RootParameters[0], 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> signature;
		Microsoft::WRL::ComPtr<ID3DBlob> error;
		HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		if (FAILED(hr))
		{
			std::string errorString = static_cast<char*>(error->GetBufferPointer());
			throw std::runtime_error("Could not serialize root signature: " + errorString);
		}
		hr = m_Renderer.m_D3DDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		if (FAILED(hr)) throw std::runtime_error("Could not create root signature.");

		D3D12_DEPTH_STENCIL_DESC DSDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		DSDesc.DepthWriteMask = (a_DepthWrite == DepthWrite::On) ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;

		// Describe and create a graphics pipeline state object (PSO).
		D3D12_GRAPHICS_PIPELINE_STATE_DESC PSDesc = {};
		PSDesc.InputLayout = { &m_InputLayout[0], (uint32)m_InputLayout.size() };
		PSDesc.pRootSignature = m_RootSignature.Get();
		PSDesc.VS = { m_VertexBlob->GetBufferPointer(),	m_VertexBlob->GetBufferSize() };
		PSDesc.PS = { m_PixelBlob->GetBufferPointer(), m_PixelBlob->GetBufferSize() };
		PSDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		PSDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		PSDesc.DepthStencilState = DSDesc;
		PSDesc.SampleMask = UINT_MAX;
		PSDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		PSDesc.NumRenderTargets = 1;
		PSDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		PSDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		PSDesc.SampleDesc.Count = 1;
		hr = m_Renderer.m_D3DDevice->CreateGraphicsPipelineState(&PSDesc, IID_PPV_ARGS(&m_PipelineState));
		if (FAILED(hr)) throw std::runtime_error("Could not create graphics pipeline state.");
	}

	ID3D12PipelineState* DX12Material::GetPSO()
	{
		if (m_PipelineState == nullptr)
		{
			throw std::runtime_error("Pipeline state is null, did you call Compile?");
		}

		return m_PipelineState.Get();
	}

	ID3D12RootSignature* DX12Material::GetRootSig()
	{
		if (m_RootSignature == nullptr)
		{
			throw std::runtime_error("Root signature is null, did you call Compile?");
		}

		return m_RootSignature.Get();
	}
}