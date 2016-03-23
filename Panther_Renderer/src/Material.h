#pragma once

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class Material
	{
	public:
		virtual ~Material();

		enum class ConstantType : ubyte8 { ShaderResource = 0, ConstantBuffer, Sampler };
		enum class ShaderType : ubyte8 { Vertex = 0, Pixel };
		enum class InputType : ubyte8 { Float = 0 };

		virtual void DeclareShaderConstant(ConstantType a_Type, uint32 a_Amount, uint32 a_BaseShaderRegister, ShaderType a_VisibleToShader) = 0;
		virtual void DeclareInputParameter(std::string a_Semantic, InputType a_Type, uint32 a_VectorElementCount) = 0;
		virtual void LoadShader(std::wstring a_Path, std::string a_EntryPoint, ShaderType a_Type) = 0;
		virtual void Compile() = 0;
	protected:
		Material();

	private:
		Material(const Material&) = delete;
	};
}