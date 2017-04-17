#pragma once
#include "Core.h"

#include <string>

namespace Panther
{
	enum class DepthWrite : ubyte8 { Off, On };

	class Material
	{
	public:
		struct DescriptorSlot
		{
			uint32 m_Slot;
		};

		virtual ~Material();

		enum class DescriptorType : ubyte8 { ShaderResource, ConstantBuffer, Sampler };
		enum class ShaderType : ubyte8 { Vertex, Pixel };
		enum class InputType : ubyte8 { Float };

		virtual DescriptorSlot DeclareShaderDescriptor(DescriptorType a_Type, uint32 a_Amount, uint32 a_BaseShaderRegister, ShaderType a_VisibleToShader) = 0;
		virtual void DeclareInputParameter(std::string a_Semantic, InputType a_Type, uint32 a_VectorElementCount) = 0;
		virtual void LoadShader(std::wstring a_Path, std::string a_EntryPoint, ShaderType a_Type) = 0;
		virtual void Compile(DepthWrite a_DepthWrite = DepthWrite::On) = 0;
	protected:
		Material();

	private:
		Material(const Material&) = delete;
	};
}