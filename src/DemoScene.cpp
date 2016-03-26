#include "PCH.h"
#include "DemoScene.h"
#include "Window.h"

#include "../Panther_Renderer/src/Buffer.h"
#include "../Panther_Renderer/src/DX12CommandList.h"
#include "../Panther_Renderer/src/DX12Renderer.h"
#include "../Panther_Renderer/src/Texture.h"
#include "../Panther_Renderer/src/Sampler.h"
#include "../Panther_Renderer/src/DX12DescriptorHeap.h"
#include "../Panther_Renderer/src/Material.h"
#include "../Panther_Renderer/src/Mesh.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Panther
{
	// Hardcoded texture file locations.
	const std::wstring g_Textures[1] =
	{
		L"..\\rsc\\textures\\test.tga"
	};

	DemoScene::DemoScene(Renderer& a_Renderer) 
		: Scene(a_Renderer) 
	{}

	bool DemoScene::Load()
	{
		// Create a material.
		{
			m_TestMaterial = m_Renderer.CreateMaterial(3, 4);
			m_TestMaterial->LoadShader(L"..\\rsc\\shaders\\shaders.hlsl", "VSMain", Material::ShaderType::Vertex);
			m_TestMaterial->LoadShader(L"..\\rsc\\shaders\\shaders.hlsl", "PSMain", Material::ShaderType::Pixel);

			m_TestMaterial->DeclareShaderConstant(Material::ConstantType::ConstantBuffer, 1, 0, Material::ShaderType::Vertex);
			m_TestMaterial->DeclareShaderConstant(Material::ConstantType::ShaderResource, 1, 0, Material::ShaderType::Pixel);
			m_TestMaterial->DeclareShaderConstant(Material::ConstantType::Sampler, 1, 0, Material::ShaderType::Pixel);

			m_TestMaterial->DeclareInputParameter("POSITION", Material::InputType::Float, 3);
			m_TestMaterial->DeclareInputParameter("NORMAL", Material::InputType::Float, 3);
			m_TestMaterial->DeclareInputParameter("COLOR", Material::InputType::Float, 4);
			m_TestMaterial->DeclareInputParameter("UV", Material::InputType::Float, 2);

			m_TestMaterial->Compile();
		}

		// Constant buffer + Shader resource heap.
		uint32 CBVSRVUAVHeapSize = (uint32)Countof(g_Textures) + 2;
		m_CBVSRVUAVDescriptorHeap = m_Renderer.CreateDescriptorHeap(CBVSRVUAVHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Sampler heap.
		uint32 samplerHeapSize = 1;
		m_SamplerDescriptorHeap = m_Renderer.CreateDescriptorHeap(samplerHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		CommandList& commandList(m_Renderer.StartRecording());

		m_CubeMesh = m_Renderer.CreateMesh();
		m_CubeMesh->InitAsCube(commandList, XMFLOAT3(1.0f, 1.0f, 1.0f));

		m_SphereMesh = m_Renderer.CreateMesh();
		m_SphereMesh->InitAsSphere(commandList, 1.0f);

		// Create the constant buffers.
		m_ConstantBuffer1 = m_Renderer.CreateBuffer(64);
		m_ConstantBuffer2 = m_Renderer.CreateBuffer(64);

		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ConstantBuffer1.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ConstantBuffer2.get());

		// Create the textures
		for (auto filePath : g_Textures)
		{
			m_Textures.push_back(m_Renderer.CreateTexture(filePath));
			m_CBVSRVUAVDescriptorHeap->RegisterTexture(*m_Textures.back().get());
		}

		// Create sampler
		m_Sampler = m_Renderer.CreateSampler();
		m_SamplerDescriptorHeap->RegisterSampler(*m_Sampler.get());

		if (!m_Renderer.StopRecordingAndSubmit()) return false;

		// Create and record the bundles.
		{
			m_CubeBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_TestMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_CubeBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_CubeBundle->SetMaterial(*m_TestMaterial, false);
			m_CubeBundle->SetMesh(*m_CubeMesh);
			m_CubeBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 1, 2);
			m_CubeBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), 2, 0);
			m_CubeBundle->Draw(m_CubeMesh->GetNumIndices());

			m_CubeBundle->Close();
		}

		{
			m_SphereBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_TestMaterial.get());
			
			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_SphereBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_SphereBundle->SetMaterial(*m_TestMaterial, false);
			m_SphereBundle->SetMesh(*m_SphereMesh);
			m_SphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 1, 2);
			m_SphereBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), 2, 0);
			m_SphereBundle->Draw(m_SphereMesh->GetNumIndices());

			m_SphereBundle->Close();
		}

		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(m_Renderer.m_Window.GetWidth()) / m_Renderer.m_Window.GetHeight(), 0.1f, 100.0f);

		m_Renderer.Synchronize();

		return true;
	}

	void DemoScene::Unload()
	{
	}

	void DemoScene::Update(float a_DT)
	{
		XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
		XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
		XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
		m_ViewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

		m_Angle += 90.0f * a_DT;
	}

	bool DemoScene::Render()
	{
		DX12Renderer* renderer = dynamic_cast<DX12Renderer*>(&m_Renderer);

		CommandList& commandList(m_Renderer.StartRecording());
		ID3D12GraphicsCommandList& D_GCL = *static_cast<DX12CommandList*>(&commandList)->m_CommandList.Get();

		commandList.SetMaterial(*m_TestMaterial, true);
		
		DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
		commandList.UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));

		D_GCL.RSSetViewports(1, &renderer->m_D3DViewport);
		D_GCL.RSSetScissorRects(1, &renderer->m_D3DRectScissor);

		// Indicate that the back buffer will be used as a render target.
		D_GCL.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderer->m_renderTargets[renderer->m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(renderer->m_RTVDescriptorHeap->m_D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			renderer->m_FrameIndex, renderer->m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(renderer->m_DSVDescriptorHeap->m_D3DDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		D_GCL.OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

		// Record commands.
		D_GCL.ClearRenderTargetView(rtvHandle, DirectX::Colors::CornflowerBlue, 0, nullptr);
		D_GCL.ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 0, 0);

		XMVECTOR rotationAxis = XMVectorSet(0, 1, 0, 0);
		m_ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(m_Angle)) * XMMatrixTranslation(0, 0, 0);
		XMMATRIX mvp = m_ModelMatrix * m_ViewMatrix * m_ProjectionMatrix;
		m_ConstantBuffer1->CopyTo(&mvp, sizeof(XMMATRIX));

		// Execute the commands stored in the bundle.
		D_GCL.ExecuteBundle(static_cast<DX12CommandList*>(m_CubeBundle.get())->m_CommandList.Get());

		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 0, 1);

		m_ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(m_Angle)) * XMMatrixTranslation(2, 0, 0);
		mvp = m_ModelMatrix * m_ViewMatrix * m_ProjectionMatrix;
		m_ConstantBuffer2->CopyTo(&mvp, sizeof(mvp));

		// Execute the commands stored in the bundle.
		D_GCL.ExecuteBundle(static_cast<DX12CommandList*>(m_SphereBundle.get())->m_CommandList.Get());

		// Indicate that the back buffer will now be used to present.
		D_GCL.ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderer->m_renderTargets[renderer->m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

		if (!m_Renderer.StopRecordingAndSubmit()) return false;

		return true;
	}

	void DemoScene::OnResize(uint32 a_Width, uint32 a_Height)
	{
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(m_Renderer.m_Window.GetWidth()) / m_Renderer.m_Window.GetHeight(), 0.1f, 100.0f);
	}
}
