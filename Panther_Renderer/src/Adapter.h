#pragma once

#include <vector>

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class Output;

	class Adapter
	{
	public:
		static std::unique_ptr<Adapter> GetAdapter(IDXGIFactory4& a_Factory, int32 a_Index = 0);

		int32 GetNumOutputs() { return (int32)m_Outputs.size(); }
		Output& GetOutput(int32 a_Index) { return *m_Outputs[a_Index]; }

	private:
		Adapter() = delete;
		Adapter(IDXGIAdapter3* a_Adapter);
		Adapter(const Adapter&) = delete;

		const Microsoft::WRL::ComPtr<IDXGIAdapter3> m_Adapter;
		std::vector<std::unique_ptr<Output>> m_Outputs;
	};
}