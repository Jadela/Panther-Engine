#include "DX12RendererPCH.h"
#include "Adapter.h"

#include "../../Panther_Core/src/Exceptions.h"

using namespace Microsoft::WRL;

namespace Panther
{
	std::unique_ptr<Adapter> Adapter::GetAdapter(IDXGIFactory4& a_Factory, int32 a_Index)
	{
		IDXGIAdapter* adapter1;
		IDXGIAdapter3* adapter;
		ThrowIfFailed(a_Factory.EnumAdapters(a_Index, &adapter1));
		ThrowIfFailed(adapter1->QueryInterface(&adapter));
		return std::unique_ptr<Adapter>(new Adapter(adapter));
	}

	Adapter::Adapter(IDXGIAdapter3* a_Adapter) : m_Adapter(a_Adapter)
	{
		UINT i = 0;
		IDXGIOutput* output1;
		while (a_Adapter->EnumOutputs(i, &output1) != DXGI_ERROR_NOT_FOUND)
		{
			IDXGIOutput4* output;
			ThrowIfFailed(output1->QueryInterface(&output));
			m_Outputs.push_back(std::unique_ptr<Output>(new Output(output)));
			++i;
		}
	}
}