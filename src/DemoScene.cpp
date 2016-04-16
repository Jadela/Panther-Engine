#include "PCH.h"
#include "DemoScene.h"
#include "Window.h"

#include "../Panther_Renderer/src/Buffer.h"
#include "../Panther_Renderer/src/CommandList.h"
#include "../Panther_Renderer/src/Renderer.h"
#include "../Panther_Renderer/src/Texture.h"
#include "../Panther_Renderer/src/Sampler.h"
#include "../Panther_Renderer/src/DescriptorHeap.h"
#include "../Panther_Renderer/src/Material.h"
#include "../Panther_Renderer/src/Mesh.h"
#include "../Panther_Utilities/src/Camera.h"
#include "../Panther_Utilities/src/Transform.h"

using namespace DirectX;
using namespace Microsoft::WRL;

namespace Panther
{
	// Hardcoded texture file locations.
	const std::wstring g_Textures[] =
	{
		L"../rsc/textures/test.tga",
		L"../rsc/textures/duckCM.tga"
	};

	DemoScene::DemoScene(Renderer& a_Renderer) 
		: Scene(a_Renderer) 
	{}

	void DemoScene::Load()
	{
		// Create a material.
		{
			m_TestMaterial = m_Renderer.CreateMaterial(3, 4);
			m_TestMaterial->LoadShader(L"../rsc/shaders/shaders.hlsl", "VSMain", Material::ShaderType::Vertex);
			m_TestMaterial->LoadShader(L"../rsc/shaders/shaders.hlsl", "PSMain", Material::ShaderType::Pixel);

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
		uint32 CBVSRVUAVHeapSize = (uint32)Countof(g_Textures) + 3;
		m_CBVSRVUAVDescriptorHeap = m_Renderer.CreateDescriptorHeap(CBVSRVUAVHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Sampler heap.
		uint32 samplerHeapSize = 1;
		m_SamplerDescriptorHeap = m_Renderer.CreateDescriptorHeap(samplerHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		CommandList& commandList(m_Renderer.StartRecording());

		m_CubeMesh = m_Renderer.CreateMesh();
		m_CubeMesh->InitAsCube(commandList, XMFLOAT3(1.0f, 1.0f, 1.0f));

		m_SphereMesh = m_Renderer.CreateMesh();
		m_SphereMesh->InitAsSphere(commandList, 1.0f);

		m_DuckMesh = m_Renderer.CreateMesh();
		m_DuckMesh->InitViaASSIMP(commandList, "../rsc/models/duck.fbx");

		// Create the constant buffers.
		m_CubeMatrixBuffer = m_Renderer.CreateBuffer(64);
		m_SphereMatrixBuffer = m_Renderer.CreateBuffer(64);
		m_DuckMatrixBuffer = m_Renderer.CreateBuffer(64);

		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_CubeMatrixBuffer.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SphereMatrixBuffer.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_DuckMatrixBuffer.get());

		// Create the textures
		for (auto filePath : g_Textures)
		{
			m_Textures.push_back(m_Renderer.CreateTexture(filePath));
			m_CBVSRVUAVDescriptorHeap->RegisterTexture(*m_Textures.back().get());
		}

		// Create sampler
		m_Sampler = m_Renderer.CreateSampler();
		m_SamplerDescriptorHeap->RegisterSampler(*m_Sampler.get());

		commandList.Close();

		CommandList* commandLists[] = {&commandList};
		m_Renderer.SubmitCommandLists(commandLists, 1);

		// Create and record the bundles.
		{
			m_CubeBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_TestMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_CubeBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_CubeBundle->SetMaterial(*m_TestMaterial, false);
			m_CubeBundle->SetMesh(*m_CubeMesh);
			m_CubeBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 1, 3);
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
			m_SphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 1, 3);
			m_SphereBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), 2, 0);
			m_SphereBundle->Draw(m_SphereMesh->GetNumIndices());

			m_SphereBundle->Close();
		}

		{
			m_DuckBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_TestMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_DuckBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_DuckBundle->SetMaterial(*m_TestMaterial, false);
			m_DuckBundle->SetMesh(*m_DuckMesh);
			m_DuckBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 1, 4);
			m_DuckBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), 2, 0);
			m_DuckBundle->Draw(m_DuckMesh->GetNumIndices());

			m_DuckBundle->Close();
		}
		m_Renderer.Synchronize();

		m_Camera = std::make_unique<Camera>(Transform(XMFLOAT3(0, 0, -10)));
		m_Camera->SetAspectRatio(static_cast<float>(m_Renderer.m_Window.GetWidth()) / m_Renderer.m_Window.GetHeight());

		m_CubeTransform = std::make_unique<Transform>(XMFLOAT3(-3, 0, 0));
		m_SphereTransform = std::make_unique<Transform>(XMFLOAT3(0, 0, 0));
		m_DuckTransform = std::make_unique<Transform>(XMFLOAT3(3, 0, 0));
	}

	void DemoScene::Unload()
	{
	}

	void DemoScene::Update(float a_DT)
	{
		m_CubeTransform->AddRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90 * a_DT)));
		m_SphereTransform->AddRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90 * a_DT)));
		m_DuckTransform->AddRotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90 * a_DT)));
	}

	void DemoScene::Render()
	{
		CommandList& commandList(m_Renderer.StartRecording());

		commandList.SetMaterial(*m_TestMaterial, true);
		
		DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
		commandList.UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));

		commandList.UseDefaultViewport();
		commandList.SetTransitionBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList.SetAndClearRenderTarget(DirectX::Colors::CornflowerBlue);

		XMMATRIX vpMatrix = m_Camera->GetViewProjectionMatrix();

		// Cube
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 0, 0);
		XMMATRIX mvp = m_CubeTransform->GetTransformMatrix() * vpMatrix;
		m_CubeMatrixBuffer->CopyTo(&mvp, sizeof(XMMATRIX));
		commandList.ExecuteBundle(*m_CubeBundle.get());

		// Sphere
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 0, 1);
		mvp = m_SphereTransform->GetTransformMatrix() * vpMatrix;
		m_SphereMatrixBuffer->CopyTo(&mvp, sizeof(mvp));
		commandList.ExecuteBundle(*m_SphereBundle.get());

		// Duck
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), 0, 2);
		mvp = m_DuckTransform->GetTransformMatrix() * vpMatrix;
		m_DuckMatrixBuffer->CopyTo(&mvp, sizeof(mvp));
		commandList.ExecuteBundle(*m_DuckBundle.get());

		// Indicate that the back buffer will now be used to present.
		commandList.SetTransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

		commandList.Close();
		CommandList* commandLists[] = { &commandList };
		m_Renderer.SubmitCommandLists(commandLists, 1);
	}

	void DemoScene::OnResize(uint32 a_Width, uint32 a_Height)
	{
		m_Camera->SetAspectRatio(static_cast<float>(m_Renderer.m_Window.GetWidth()) / m_Renderer.m_Window.GetHeight());
	}

	void DemoScene::OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown)
	{
		UpdateMouseDelta(XMINT2(a_DeltaX, a_DeltaY));
		if (a_LMBDown)
		{
			m_Camera->ApplyRotation(0.0f, -m_MousePositionDelta.y * 0.1f, -m_MousePositionDelta.x * 0.1f);
		}
	}
}
