#include "PCH.h"
#include "DemoScene.h"
#include "Window.h"
#include "Application.h"

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
		L"../rsc/textures/duckCM.tga",
		L"../rsc/textures/SkyDay.tga",
		L"../rsc/textures/SkyNight.tga",
		L"../rsc/textures/Sun.tga",
		L"../rsc/textures/Moon.tga"
	};

	DemoScene::DemoScene(Renderer& a_Renderer) 
		: Scene(a_Renderer) 
	{}

	void DemoScene::Load()
	{
		// Create skysphere material.
		{
			m_SkyDomeMaterial = m_Renderer.CreateMaterial(7, 2);
			m_SkyDomeMaterial->LoadShader(L"../rsc/shaders/skydome.hlsl", "VSMain", Material::ShaderType::Vertex);
			m_SkyDomeMaterial->LoadShader(L"../rsc/shaders/skydome.hlsl", "PSMain", Material::ShaderType::Pixel);

			m_SkyDomeVertexCBSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 0, Material::ShaderType::Vertex);
			m_SkyDomePixelCBSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 1, Material::ShaderType::Pixel);
			m_SkyDomeDayTextureSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 1, 0, Material::ShaderType::Pixel);
			m_SkyDomeDuskTextureSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 1, 1, Material::ShaderType::Pixel);
			m_SkyDomeSunTextureSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 1, 2, Material::ShaderType::Pixel);
			m_SkyDomeMoonTextureSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 1, 3, Material::ShaderType::Pixel);
			m_SkyDomeClampedSamplerSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::Sampler, 1, 0, Material::ShaderType::Pixel);

			m_SkyDomeMaterial->DeclareInputParameter("POSITION", Material::InputType::Float, 3);
			m_SkyDomeMaterial->DeclareInputParameter("UV", Material::InputType::Float, 2);

			m_SkyDomeMaterial->Compile(Material::DepthWrite::Off);
		}

		// Create diffuse material.
		{
			m_DefaultMaterial = m_Renderer.CreateMaterial(4, 4);
			m_DefaultMaterial->LoadShader(L"../rsc/shaders/shaders.hlsl", "VSMain", Material::ShaderType::Vertex);
			m_DefaultMaterial->LoadShader(L"../rsc/shaders/shaders.hlsl", "PSMain", Material::ShaderType::Pixel);

			m_DefaultVertexCBSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 0, Material::ShaderType::Vertex);
			m_DefaultPixelCBSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 1, Material::ShaderType::Pixel);
			m_DefaultTextureSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 1, 0, Material::ShaderType::Pixel);
			m_DefaultSamplerSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::Sampler, 1, 0, Material::ShaderType::Pixel);

			m_DefaultMaterial->DeclareInputParameter("POSITION", Material::InputType::Float, 3);
			m_DefaultMaterial->DeclareInputParameter("NORMAL", Material::InputType::Float, 3);
			m_DefaultMaterial->DeclareInputParameter("COLOR", Material::InputType::Float, 4);
			m_DefaultMaterial->DeclareInputParameter("UV", Material::InputType::Float, 2);

			m_DefaultMaterial->Compile();
		}

		// Constant buffer + Shader resource heap.
		uint32 CBVSRVUAVHeapSize = 6 + (uint32)Countof(g_Textures);
		m_CBVSRVUAVDescriptorHeap = m_Renderer.CreateDescriptorHeap(CBVSRVUAVHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		// Sampler heap.
		uint32 samplerHeapSize = 2;
		m_SamplerDescriptorHeap = m_Renderer.CreateDescriptorHeap(samplerHeapSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		CommandList& commandList(m_Renderer.StartRecording());

		m_CubeMesh = m_Renderer.CreateMesh();
		m_CubeMesh->InitAsCube(commandList, XMFLOAT3(1.0f, 1.0f, 1.0f));

		m_SphereMesh = m_Renderer.CreateMesh();
		m_SphereMesh->InitAsSphere(commandList, 1.0f);

		m_DuckMesh = m_Renderer.CreateMesh();
		m_DuckMesh->InitViaASSIMP(commandList, "../rsc/models/duck.fbx");

		// Create the constant buffers.
		m_CubeMatrixBuffer = m_Renderer.CreateBuffer(128);
		m_SphereMatrixBuffer = m_Renderer.CreateBuffer(128);
		m_DuckMatrixBuffer = m_Renderer.CreateBuffer(128);
		m_SkydomeVertexConstantBuffer = m_Renderer.CreateBuffer(64 + 16);
		m_LightPositionBuffer = m_Renderer.CreateBuffer(16);
		m_SkydomePixelConstantBuffer = m_Renderer.CreateBuffer(16);

		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_CubeMatrixBuffer.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SphereMatrixBuffer.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_DuckMatrixBuffer.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SkydomeVertexConstantBuffer.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_LightPositionBuffer.get());
		m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SkydomePixelConstantBuffer.get());

		// Create the textures
		for (auto filePath : g_Textures)
		{
			m_Textures.push_back(m_Renderer.CreateTexture(filePath));
			m_CBVSRVUAVDescriptorHeap->RegisterTexture(*m_Textures.back().get());
		}

		// Create sampler
		m_DefaultSampler = m_Renderer.CreateSampler();
		m_SkyboxSampler = m_Renderer.CreateSampler(Sampler::TextureCoordinateMode::Clamp);

		m_SamplerDescriptorHeap->RegisterSampler(*m_DefaultSampler.get());
		m_SamplerDescriptorHeap->RegisterSampler(*m_SkyboxSampler.get());

		commandList.Close();

		CommandList* commandLists[] = {&commandList};
		m_Renderer.SubmitCommandLists(commandLists, 1);

		// Create and record the bundles.
		{
			m_SkySphereBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_SkyDomeMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_SkySphereBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_SkySphereBundle->SetMaterial(*m_SkyDomeMaterial, false);
			m_SkySphereBundle->SetMesh(*m_SphereMesh);
			m_SkySphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomeDayTextureSlot, 8);
			m_SkySphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomeDuskTextureSlot, 9);
			m_SkySphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomeSunTextureSlot, 10);
			m_SkySphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomeMoonTextureSlot, 11);
			m_SkySphereBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_SkyDomeClampedSamplerSlot, 1);
			m_SkySphereBundle->Draw(m_SphereMesh->GetNumIndices());

			m_SkySphereBundle->Close();
		}

		{
			m_CubeBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_DefaultMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_CubeBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_CubeBundle->SetMaterial(*m_DefaultMaterial, false);
			m_CubeBundle->SetMesh(*m_CubeMesh);
			m_CubeBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultTextureSlot, 6);
			m_CubeBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot, 0);
			m_CubeBundle->Draw(m_CubeMesh->GetNumIndices());

			m_CubeBundle->Close();
		}

		{
			m_SphereBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_DefaultMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_SphereBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_SphereBundle->SetMaterial(*m_DefaultMaterial, false);
			m_SphereBundle->SetMesh(*m_SphereMesh);
			m_SphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultTextureSlot, 6);
			m_SphereBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot, 0);
			m_SphereBundle->Draw(m_SphereMesh->GetNumIndices());

			m_SphereBundle->Close();
		}

		{
			m_DuckBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_DefaultMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_DuckBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_DuckBundle->SetMaterial(*m_DefaultMaterial, false);
			m_DuckBundle->SetMesh(*m_DuckMesh);
			m_DuckBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultTextureSlot, 7);
			m_DuckBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot, 0);
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
		m_CubeTransform->Rotate(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90 * a_DT)));
		m_SphereTransform->Rotate(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90 * a_DT)));
		m_DuckTransform->Rotate(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90 * a_DT)));

		float speedMultipler = (m_Shift ? 8.0f : 4.0f);
		XMVECTOR cameraTranslate = XMVectorSet(static_cast<float>(m_D - m_A), 0.0f, static_cast<float>(m_W - m_S), 1.0f) * speedMultipler * a_DT;
		XMVECTOR cameraPan = XMVectorSet(0.0f, static_cast<float>(m_E - m_Q), 0.0f, 1.0f) * speedMultipler * a_DT;
		m_Camera->Translate(cameraTranslate, Space::Local);
		m_Camera->Translate(cameraPan, Space::World);

		const float pi = 3.14159265359f;
		m_SunAngle += XMConvertToRadians(5) * a_DT; 
		if (m_SunAngle > (pi * 2))
		{
			m_SunAngle -= (pi * 2);
		}
	}

	void DemoScene::Render()
	{
		CommandList& commandList(m_Renderer.StartRecording());
		commandList.UseDefaultViewport();
		commandList.SetTransitionBarrier(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		commandList.SetAndClearRenderTarget(DirectX::Colors::CornflowerBlue);

		DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
		commandList.UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));

		XMMATRIX vpMatrix = m_Camera->GetViewProjectionMatrix();

		struct SkydomeVertexCB
		{
			XMMATRIX m_MVP;
			XMVECTOR m_SunPos;
		};

		struct SkydomePixelCB
		{
			XMVECTOR m_ScreenResolution;
		};
	
		commandList.SetMaterial(*m_SkyDomeMaterial, true);

		// Skydome	
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomeVertexCBSlot, 3);
		SkydomeVertexCB skydomeCB;
		skydomeCB.m_MVP = m_Camera->GetSkyMatrix() * vpMatrix;
		skydomeCB.m_SunPos = XMVectorSet(0, std::sinf(m_SunAngle), std::cosf(m_SunAngle), 0);
		m_SkydomeVertexConstantBuffer->CopyTo(&skydomeCB, sizeof(SkydomeVertexCB));
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomePixelCBSlot, 5);
		SkydomePixelCB skydomeCB2;
		skydomeCB2.m_ScreenResolution = XMVectorSet((float)m_Renderer.m_Window.GetWidth(), (float)m_Renderer.m_Window.GetHeight(), 0, 0);
		m_SkydomePixelConstantBuffer->CopyTo(&skydomeCB2, sizeof(SkydomePixelCB));
		commandList.ExecuteBundle(*m_SkySphereBundle.get());

		commandList.SetMaterial(*m_DefaultMaterial, true);
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultPixelCBSlot, 4);
		XMVECTOR lightPos = skydomeCB.m_SunPos;
		m_LightPositionBuffer->CopyTo(&lightPos, sizeof(XMVECTOR));

		struct DefaultObjectCB
		{
			XMMATRIX m_InverseTransposeMatrix;
			XMMATRIX m_MVP;
		};

		// Cube
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultVertexCBSlot, 0);
		DefaultObjectCB defaultObjectCB;
		defaultObjectCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_CubeTransform->GetTransformMatrix()));
		defaultObjectCB.m_MVP = m_CubeTransform->GetTransformMatrix() * vpMatrix;
		m_CubeMatrixBuffer->CopyTo(&defaultObjectCB, sizeof(DefaultObjectCB));
		commandList.ExecuteBundle(*m_CubeBundle.get());

		// Sphere
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultVertexCBSlot, 1);
		defaultObjectCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_SphereTransform->GetTransformMatrix()));
		defaultObjectCB.m_MVP = m_SphereTransform->GetTransformMatrix() * vpMatrix;
		m_SphereMatrixBuffer->CopyTo(&defaultObjectCB, sizeof(DefaultObjectCB));
		commandList.ExecuteBundle(*m_SphereBundle.get());

		// Duck
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultVertexCBSlot, 2);
		defaultObjectCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_DuckTransform->GetTransformMatrix()));
		defaultObjectCB.m_MVP = m_DuckTransform->GetTransformMatrix() * vpMatrix;
		m_DuckMatrixBuffer->CopyTo(&defaultObjectCB, sizeof(DefaultObjectCB));
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

	void DemoScene::OnKeyDown(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt)
	{
		switch (a_Key)
		{
		case Key::W:
		{
			m_W = 1;
		}
		break;
		case Key::S:
		{
			m_S = 1;
		}
		break;
		case Key::A:
		{
			m_A = 1;
		}
		break;
		case Key::D:
		{
			m_D = 1;
		}
		break;
		case Key::Q:
		{
			m_Q = 1;
		}
		break;
		case Key::E:
		{
			m_E = 1;
		}
		break;
		case Key::ShiftKey:
		{
			m_Shift = 1;
		}
		break;
		case Key::Escape:
		{
			Application::Get().Quit();
		}
		break;
		}
	}

	void DemoScene::OnKeyUp(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt)
	{
		switch (a_Key)
		{
		case Key::W:
		{
			m_W = 0;
		}
		break;
		case Key::S:
		{
			m_S = 0;
		}
		break;
		case Key::A:
		{
			m_A = 0;
		}
		break;
		case Key::D:
		{
			m_D = 0;
		}
		break;
		case Key::Q:
		{
			m_Q = 0;
		}
		break;
		case Key::E:
		{
			m_E = 0;
		}
		break;
		case Key::ShiftKey:
		{
			m_Shift = 0;
		}
		break;
		}
	}

	void DemoScene::OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown, bool a_RMBDown)
	{
		UpdateMouseDelta(XMINT2(a_DeltaX, a_DeltaY));
		if (a_RMBDown)
		{
			m_Camera->Rotate(0.0f, -m_MousePositionDelta.y * 0.1f, -m_MousePositionDelta.x * 0.1f);
		}
	}
}
