#include "DX12RendererPCH.h"
#include "Adapter.h"

#include "../../Panther_Core/src/Exceptions.h"
#include "Output.h"

using namespace Microsoft::WRL;

namespace Panther
{
	std::unique_ptr<Adapter> Adapter::GetAdapter(IDXGIFactory4& a_Factory, int32 a_Index, bool a_WarpAdapter)
	{
		IDXGIAdapter* adapter1;
		IDXGIAdapter3* adapter;
		if (a_WarpAdapter)
		{
			ThrowIfFailed(a_Factory.EnumWarpAdapter(IID_PPV_ARGS(&adapter)))
		}
		else
		{
			ThrowIfFailed(a_Factory.EnumAdapters(a_Index, &adapter1));
			ThrowIfFailed(adapter1->QueryInterface(&adapter));
		}
		return std::unique_ptr<Adapter>(new Adapter(adapter));
	}

	void Adapter::LogProperties()
	{
		DXGI_ADAPTER_DESC2 adapterDesc = { 0 };
		m_Adapter->GetDesc2(&adapterDesc);

		OutputDebugString((std::wstring(L"\tAdapter info:")
			+ L"\n\tAdapter:\t\t\t\t\t" + std::wstring(adapterDesc.Description)
			+ L"\n\tVendor ID:\t\t\t\t\t" + std::to_wstring(adapterDesc.VendorId)
			+ L"\n\tDevice ID:\t\t\t\t\t" + std::to_wstring(adapterDesc.DeviceId)
			+ L"\n\tSubsystem ID:\t\t\t\t" + std::to_wstring(adapterDesc.SubSysId)
			+ L"\n\tRevision:\t\t\t\t\t" + std::to_wstring(adapterDesc.Revision)
			+ L"\n\tDedicated Video Memory:\t\t" + std::to_wstring(adapterDesc.DedicatedVideoMemory / 1000000) + L" MB"
			+ L"\n\tDedicated System Memory:\t" + std::to_wstring(adapterDesc.DedicatedSystemMemory / 1000000) + L" MB"
			+ L"\n\tShared System Memory:\t\t" + std::to_wstring(adapterDesc.SharedSystemMemory / 1000000) + L" MB \n").c_str());
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