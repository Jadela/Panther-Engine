#pragma once
#include "Texture.h"

namespace Panther
{
	class DX12Renderer;

	class DX12Texture final : public Texture
	{
	public:
		DX12Texture() = delete;
		DX12Texture(const DX12Texture&) = delete;
		DX12Texture(DX12Renderer& a_Renderer);
		DX12Texture(DX12Renderer& a_Renderer, D3D12_SHADER_RESOURCE_VIEW_DESC& a_ShaderResourceViewDescriptor);

		virtual void Upload() final override;

		ID3D12Resource* GetGPUResource() { return m_GPUResource.Get(); }
		D3D12_SHADER_RESOURCE_VIEW_DESC& GetSRVDesc() { return m_ShaderResourceViewDescriptor; }
	private:
		DX12Renderer& m_Renderer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_GPUResource = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_UploadResource = nullptr;
		D3D12_SHADER_RESOURCE_VIEW_DESC m_ShaderResourceViewDescriptor = {};
	};
}