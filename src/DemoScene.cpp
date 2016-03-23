#include "PCH.h"
#include "DemoScene.h"
#include "Window.h"

#include "../Panther_Renderer/src/DX12Buffer.h"
#include "../Panther_Renderer/src/DX12CommandList.h"
#include "../Panther_Renderer/src/DX12Renderer.h"
#include "../Panther_Renderer/src/DX12Texture.h"
#include "../Panther_Renderer/src/DX12Sampler.h"
#include "../Panther_Renderer/src/DX12DescriptorHeap.h"
#include "../Panther_Renderer/src/DX12Material.h"
#include "../Panther_Renderer/src/DX12Mesh.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Panther
{
	// Hardcoded texture file locations.
	const std::wstring g_Textures[1] =
	{
		L"..\\rsc\\textures\\test.tga"
	};

	DemoScene::DemoScene(Renderer& renderer) : Scene(renderer) {}

	bool DemoScene::Load()
	{
		DX12Renderer* renderer = dynamic_cast<DX12Renderer*>(&m_Renderer);

		HRESULT hr;
		// Create a material.
		{
			m_TestMaterial = std::move(std::unique_ptr<Material>(new DX12Material(*renderer, 3, 4)));
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
		uint32 numDescriptors = (uint32)Countof(g_Textures) + 2;
		m_CBVSRVUAVDescriptorHeap = std::make_unique<DX12DescriptorHeap>(*renderer->m_D3DDevice.Get(), numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Sampler heap.
		numDescriptors = 1;
		m_SamplerDescriptorHeap = std::make_unique<DX12DescriptorHeap>(*renderer->m_D3DDevice.Get(), numDescriptors, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		CommandList& commandList(m_Renderer.StartRecording());

		m_CubeMesh = m_Renderer.CreateMesh();
		m_CubeMesh->InitAsCube(commandList, XMFLOAT3(1.0f, 1.0f, 1.0f));

		m_SphereMesh = m_Renderer.CreateMesh();
		m_SphereMesh->InitAsSphere(commandList, 1.0f);

		// Create the constant buffers.
		m_ConstantBuffer1 = m_Renderer.CreateBuffer(64);
		m_ConstantBuffer2 = m_Renderer.CreateBuffer(64);

		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*static_cast<DX12Buffer*>(m_ConstantBuffer1.get()));
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*static_cast<DX12Buffer*>(m_ConstantBuffer2.get()));

		// Create the textures
		for (auto filePath : g_Textures)
		{
			m_Textures.push_back(m_Renderer.CreateTexture(filePath));
			m_CBVSRVUAVDescriptorHeap->RegisterTexture(*static_cast<DX12Texture*>(m_Textures.back().get()));
		}

		// Create sampler
		DX12Sampler textureSampler;
		m_SamplerDescriptorHeap->RegisterSampler(textureSampler);

		if (!m_Renderer.StopRecordingAndSubmit()) return false;

		// Create and record the bundles.
		{
			m_CubeBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_TestMaterial.get());
			ID3D12GraphicsCommandList& B_GCL = *static_cast<DX12CommandList*>(m_CubeBundle.get())->m_CommandList.Get();

			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle(m_CBVSRVUAVDescriptorHeap->m_D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 2, renderer->m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

			ID3D12DescriptorHeap* ppHeaps[] = { m_CBVSRVUAVDescriptorHeap->m_D3DDescriptorHeap.Get(), m_SamplerDescriptorHeap->m_D3DDescriptorHeap.Get() };
			B_GCL.SetDescriptorHeaps((uint32)Countof(ppHeaps), ppHeaps);
			m_CubeBundle->SetMaterial(*m_TestMaterial, false);
			m_CubeBundle->SetMesh(*m_CubeMesh);
			B_GCL.SetGraphicsRootDescriptorTable(1, cbvGPUHandle);
			B_GCL.SetGraphicsRootDescriptorTable(2, m_SamplerDescriptorHeap->m_D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			B_GCL.DrawIndexedInstanced(m_CubeMesh->GetNumIndices(), 1, 0, 0, 0);

			hr = B_GCL.Close();
			if (FAILED(hr)) throw std::runtime_error("Could not close bundle list.");
		}

		{
			m_SphereBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_TestMaterial.get());
			ID3D12GraphicsCommandList& B_GCL = *static_cast<DX12CommandList*>(m_SphereBundle.get())->m_CommandList.Get();

			CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle(m_CBVSRVUAVDescriptorHeap->m_D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 2, renderer->m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

			ID3D12DescriptorHeap* ppHeaps[] = { m_CBVSRVUAVDescriptorHeap->m_D3DDescriptorHeap.Get(), m_SamplerDescriptorHeap->m_D3DDescriptorHeap.Get() };
			B_GCL.SetDescriptorHeaps((uint32)Countof(ppHeaps), ppHeaps);
			m_SphereBundle->SetMaterial(*m_TestMaterial, false);
			m_SphereBundle->SetMesh(*m_SphereMesh);
			B_GCL.SetGraphicsRootDescriptorTable(1, cbvGPUHandle);
			B_GCL.SetGraphicsRootDescriptorTable(2, m_SamplerDescriptorHeap->m_D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
			B_GCL.DrawIndexedInstanced(m_SphereMesh->GetNumIndices(), 1, 0, 0, 0);

			hr = B_GCL.Close();
			if (FAILED(hr)) throw std::runtime_error("Could not close bundle list.");
		}

		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(renderer->m_Window.GetWidth()) / m_Renderer.m_Window.GetHeight(), 0.1f, 100.0f);

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
		
		ID3D12DescriptorHeap* ppHeaps[] = { m_CBVSRVUAVDescriptorHeap->m_D3DDescriptorHeap.Get(), m_SamplerDescriptorHeap->m_D3DDescriptorHeap.Get() };
		D_GCL.SetDescriptorHeaps((uint32)Countof(ppHeaps), ppHeaps);

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

		CD3DX12_GPU_DESCRIPTOR_HANDLE cbvGPUHandle(m_CBVSRVUAVDescriptorHeap->m_D3DDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		D_GCL.SetGraphicsRootDescriptorTable(0, cbvGPUHandle);

		XMVECTOR rotationAxis = XMVectorSet(0, 1, 0, 0);
		m_ModelMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(m_Angle)) * XMMatrixTranslation(0, 0, 0);
		XMMATRIX mvp = m_ModelMatrix * m_ViewMatrix * m_ProjectionMatrix;
		m_ConstantBuffer1->CopyTo(&mvp, sizeof(XMMATRIX));

		// Execute the commands stored in the bundle.
		D_GCL.ExecuteBundle(static_cast<DX12CommandList*>(m_CubeBundle.get())->m_CommandList.Get());

		cbvGPUHandle.Offset(renderer->m_D3DDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		D_GCL.SetGraphicsRootDescriptorTable(0, cbvGPUHandle);

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
		DX12Renderer* renderer = (dynamic_cast<DX12Renderer*>(&m_Renderer));

		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), static_cast<float>(renderer->m_Window.GetWidth()) / renderer->m_Window.GetHeight(), 0.1f, 100.0f);
	}
}
