#pragma once

#include <string>

namespace Panther
{
	class Shader
	{
	public:
		virtual ~Shader() {};

		virtual void LoadVertexShader(std::wstring a_Path, std::string a_EntryPoint) = 0;
		virtual void LoadPixelShader(std::wstring a_Path, std::string a_EntryPoint) = 0;
		virtual void Finalize() = 0;

	protected:

	private:

	};
}