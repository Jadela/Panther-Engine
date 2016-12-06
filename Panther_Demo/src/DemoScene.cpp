#include "DemoScene.h"
#include "Window.h"
#include "Application.h"

#include "../../Panther_Renderer/src/Buffer.h"
#include "../../Panther_Renderer/src/CommandList.h"
#include "../../Panther_Renderer/src/Renderer.h"
#include "../../Panther_Renderer/src/Texture.h"
#include "../../Panther_Renderer/src/Sampler.h"
#include "../../Panther_Renderer/src/DescriptorHeap.h"
#include "../../Panther_Renderer/src/Material.h"
#include "../../Panther_Renderer/src/Mesh.h"
#include "../../Panther_Utilities/src/Camera.h"
#include "../../Panther_Utilities/src/Transform.h"

using namespace DirectX;

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
		L"../rsc/textures/Moon.tga",
		L"../rsc/textures/WaterNormal.tga",
		L"../rsc/textures/ICMm B Color.tga"
	};

	DemoScene::DemoScene(Renderer& a_Renderer) 
		: Scene(a_Renderer) 
	{
		m_TextureSlots = new TextureID[Countof(g_Textures)];
		m_TextureManager = std::make_unique<TextureManager>(a_Renderer, (uint32)Countof(g_Textures));
	}

	DemoScene::~DemoScene()
	{
		delete[] m_TextureSlots;
	}

	void DemoScene::Load()
	{
		// Create skysphere material.
		{
			m_SkyDomeMaterial = m_Renderer.CreateMaterial(7, 2);
			m_SkyDomeMaterial->LoadShader(L"../rsc/shaders/skydome.hlsl", "VSMain", Material::ShaderType::Vertex);
			m_SkyDomeMaterial->LoadShader(L"../rsc/shaders/skydome.hlsl", "PSMain", Material::ShaderType::Pixel);

			m_SkyDomeVertexCBSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 0, Material::ShaderType::Vertex);
			m_SkyDomePixelCBSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 1, Material::ShaderType::Pixel);
			m_SkyDomeTexturesSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 4, 0, Material::ShaderType::Pixel);
			m_SkyDomeClampedSamplerSlot = m_SkyDomeMaterial->DeclareShaderDescriptor(Material::DescriptorType::Sampler, 1, 0, Material::ShaderType::Pixel);

			m_SkyDomeMaterial->DeclareInputParameter("POSITION", Material::InputType::Float, 3);
			m_SkyDomeMaterial->DeclareInputParameter("TEXCOORD", Material::InputType::Float, 2);

			m_SkyDomeMaterial->Compile(Material::DepthWrite::Off);
		}

		// Create water material
		{
			m_WaterMaterial = m_Renderer.CreateMaterial(4, 3);
			m_WaterMaterial->LoadShader(L"../rsc/shaders/water.hlsl", "VSMain", Material::ShaderType::Vertex);
			m_WaterMaterial->LoadShader(L"../rsc/shaders/water.hlsl", "PSMain", Material::ShaderType::Pixel);

			m_WaterVertexCBSlot = m_WaterMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 0, Material::ShaderType::Vertex);
			m_WaterPixelCBSlot = m_WaterMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 1, Material::ShaderType::Pixel);
			m_WaterTexture0Slot = m_WaterMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 1, 0, Material::ShaderType::Pixel);
			m_WaterSamplerSlot = m_WaterMaterial->DeclareShaderDescriptor(Material::DescriptorType::Sampler, 1, 0, Material::ShaderType::Pixel);

			m_WaterMaterial->DeclareInputParameter("POSITION", Material::InputType::Float, 3);
			m_WaterMaterial->DeclareInputParameter("NORMAL", Material::InputType::Float, 3);
			m_WaterMaterial->DeclareInputParameter("TEXCOORD", Material::InputType::Float, 2);

			m_WaterMaterial->Compile();
		}

		// Create diffuse material.
		{
			m_DefaultMaterial = m_Renderer.CreateMaterial(4, 4);
			m_DefaultMaterial->LoadShader(L"../rsc/shaders/shaders.hlsl", "VSMain", Material::ShaderType::Vertex);
			m_DefaultMaterial->LoadShader(L"../rsc/shaders/shaders.hlsl", "PSMain", Material::ShaderType::Pixel);

			m_DefaultVertexCBSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 0, Material::ShaderType::Vertex);
			m_DefaultPixelCBSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::ConstantBuffer, 1, 1, Material::ShaderType::Pixel);
			m_DefaultTextureSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::ShaderResource, 1, 0, Material::ShaderType::Pixel);
			m_DefaultSamplerDescriptorSlot = m_DefaultMaterial->DeclareShaderDescriptor(Material::DescriptorType::Sampler, 1, 0, Material::ShaderType::Pixel);

			m_DefaultMaterial->DeclareInputParameter("POSITION", Material::InputType::Float, 3);
			m_DefaultMaterial->DeclareInputParameter("NORMAL", Material::InputType::Float, 3);
			m_DefaultMaterial->DeclareInputParameter("COLOR", Material::InputType::Float, 4);
			m_DefaultMaterial->DeclareInputParameter("TEXCOORD", Material::InputType::Float, 2);

			m_DefaultMaterial->Compile();
		}

		// Constant buffer + Shader resource heap.
		uint32 CBVSRVUAVHeapSize = 9 + (uint32)Countof(g_Textures);
		m_CBVSRVUAVDescriptorHeap = m_Renderer.CreateDescriptorHeap(CBVSRVUAVHeapSize, DescriptorHeap::DescriptorHeapType::ConstantBufferView); // Using type constant buffer view but shouldn't really matter.

		// Sampler heap.
		uint32 samplerHeapSize = 2;
		m_SamplerDescriptorHeap = m_Renderer.CreateDescriptorHeap(samplerHeapSize, DescriptorHeap::DescriptorHeapType::Sampler);

		CommandList& commandList(m_Renderer.StartRecording());

		m_PlaneMesh = m_Renderer.CreateMesh();
		m_PlaneMesh->InitAsPlane(commandList);

		m_CubeMesh = m_Renderer.CreateMesh();
		m_CubeMesh->InitAsCube(commandList);

		m_SphereMesh = m_Renderer.CreateMesh();
		m_SphereMesh->InitAsSphere(commandList);

		m_DuckMesh = m_Renderer.CreateMesh();
		m_DuckMesh->InitViaASSIMP(commandList, "../rsc/models/duck.fbx");

		m_ICMmMesh = m_Renderer.CreateMesh();
		m_ICMmMesh->InitViaASSIMP(commandList, "../rsc/models/ICMm B.fbx");

		// Create the constant buffers.
		m_WaterVertexCBuffer = m_Renderer.CreateBuffer(192);
		m_WaterPixelCBuffer = m_Renderer.CreateBuffer(112);
		m_CubeMatrixBuffer = m_Renderer.CreateBuffer(192);
		m_SphereMatrixBuffer = m_Renderer.CreateBuffer(192);
		m_DuckMatrixBuffer = m_Renderer.CreateBuffer(192);
		m_SkydomeVertexCBuffer = m_Renderer.CreateBuffer(64 + 16);
		m_LightPositionBuffer = m_Renderer.CreateBuffer(32);
		m_SkydomePixelCBuffer = m_Renderer.CreateBuffer(16);
		m_ICMmMatrixBuffer = m_Renderer.CreateBuffer(192);
		
		m_WaterVertexCBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_WaterVertexCBuffer.get());
		m_WaterPixelCBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_WaterPixelCBuffer.get());
		m_CubeMatrixBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_CubeMatrixBuffer.get());
		m_SphereMatrixBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SphereMatrixBuffer.get());
		m_DuckMatrixBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_DuckMatrixBuffer.get());
		m_ICMmMatrixBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ICMmMatrixBuffer.get());
		m_SkydomeVertexCBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SkydomeVertexCBuffer.get());
		m_SkydomePixelCBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SkydomePixelCBuffer.get());
		m_LightPositionBufferSlot = m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_LightPositionBuffer.get());

		// Create the textures
		LoadTextures();

		// Create sampler
		m_DefaultSampler = m_Renderer.CreateSampler();
		m_SkyboxSampler = m_Renderer.CreateSampler(Sampler::TextureCoordinateMode::Clamp);

		m_DefaultSamplerSlot = m_SamplerDescriptorHeap->RegisterSampler(*m_DefaultSampler.get());
		m_SkyboxSamplerSlot = m_SamplerDescriptorHeap->RegisterSampler(*m_SkyboxSampler.get());

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
			m_SkySphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomeTexturesSlot, m_TextureSlots[2]);
			m_SkySphereBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_SkyDomeClampedSamplerSlot, m_SkyboxSamplerSlot);
			m_SkySphereBundle->Draw(m_SphereMesh->GetNumIndices());

			m_SkySphereBundle->Close();
		}

		{
			m_WaterBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_WaterMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_WaterBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_WaterBundle->SetMaterial(*m_WaterMaterial, false);
			m_WaterBundle->SetMesh(*m_PlaneMesh);
			m_WaterBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_WaterTexture0Slot, m_TextureSlots[6]);
			m_WaterBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_WaterSamplerSlot, m_DefaultSamplerSlot);
			m_WaterBundle->Draw(m_PlaneMesh->GetNumIndices());

			m_WaterBundle->Close();
		}

		{
			m_CubeBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_DefaultMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_CubeBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_CubeBundle->SetMaterial(*m_DefaultMaterial, false);
			m_CubeBundle->SetMesh(*m_CubeMesh);
			m_CubeBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultTextureSlot, m_TextureSlots[0]);
			m_CubeBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_DefaultSamplerDescriptorSlot, m_DefaultSamplerSlot);
			m_CubeBundle->Draw(m_CubeMesh->GetNumIndices());

			m_CubeBundle->Close();
		}

		{
			m_SphereBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_DefaultMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_SphereBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_SphereBundle->SetMaterial(*m_DefaultMaterial, false);
			m_SphereBundle->SetMesh(*m_SphereMesh);
			m_SphereBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultTextureSlot, m_TextureSlots[0]);
			m_SphereBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_DefaultSamplerDescriptorSlot, m_DefaultSamplerSlot);
			m_SphereBundle->Draw(m_SphereMesh->GetNumIndices());

			m_SphereBundle->Close();
		}

		{
			m_DuckBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_DefaultMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_DuckBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_DuckBundle->SetMaterial(*m_DefaultMaterial, false);
			m_DuckBundle->SetMesh(*m_DuckMesh);
			m_DuckBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultTextureSlot, m_TextureSlots[1]);
			m_DuckBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_DefaultSamplerDescriptorSlot, m_DefaultSamplerSlot);
			m_DuckBundle->Draw(m_DuckMesh->GetNumIndices());

			m_DuckBundle->Close();
		}

		{
			m_ICMmBundle = m_Renderer.CreateCommandList(D3D12_COMMAND_LIST_TYPE_BUNDLE, m_DefaultMaterial.get());

			DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
			m_ICMmBundle->UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));
			m_ICMmBundle->SetMaterial(*m_DefaultMaterial, false);
			m_ICMmBundle->SetMesh(*m_ICMmMesh);
			m_ICMmBundle->SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultTextureSlot, m_TextureSlots[7]);
			m_ICMmBundle->SetDescriptorHeap(*m_SamplerDescriptorHeap.get(), m_DefaultSamplerDescriptorSlot, m_DefaultSamplerSlot);
			m_ICMmBundle->Draw(m_ICMmMesh->GetNumIndices());

			m_ICMmBundle->Close();
		}

		m_Renderer.Synchronize();

		m_Camera = std::make_unique<Camera>(Transform(XMFLOAT3(0, 0, -10)));
		m_Camera->SetAspectRatio(static_cast<float>(m_Renderer.m_Window.GetWidth()) / m_Renderer.m_Window.GetHeight());

		m_WaterTransform = std::make_unique<Transform>(XMFLOAT3(0, -3, 0), XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(-90)), XMFLOAT3(10, 10, 10));
		m_CubeTransform = std::make_unique<Transform>(XMFLOAT3(-3, 0, 0));
		m_SphereTransform = std::make_unique<Transform>(XMFLOAT3(0, 0, 0));
		m_DuckTransform = std::make_unique<Transform>(XMFLOAT3(3, 0, 0));
		m_ICMmTransform = std::make_unique<Transform>(XMFLOAT3(0, -1.5, 0), XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(90)), XMFLOAT3(100, 954, 109));

		m_ICMmTransform->Rotate(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90)));
	}

	void DemoScene::LoadTextures()
	{
		for (int i = 0; i < Countof(g_Textures); ++i)
		{
			const std::wstring filePath = g_Textures[i];
			m_TextureSlots[i] = m_CBVSRVUAVDescriptorHeap->RegisterTexture(*m_TextureManager->GetTexture(m_TextureManager->LoadTexture(filePath)));
		}
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

		m_WaterOffset = fmodf(m_WaterOffset + 0.05f * a_DT, 1.0f);
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

		// Use skydome shader
		commandList.SetMaterial(*m_SkyDomeMaterial, true);

		struct SkydomeVertexCB
		{
			XMMATRIX m_MVP;
			XMVECTOR m_SunPos;
		};

		struct SkydomePixelCB
		{
			XMVECTOR m_ScreenResolution;
		};
	
		// Skydome	
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomeVertexCBSlot, m_SkydomeVertexCBufferSlot);
		SkydomeVertexCB skydomeCB;
		skydomeCB.m_MVP = m_Camera->GetSkyMatrix() * vpMatrix;
		skydomeCB.m_SunPos = XMVectorSet(0, std::sinf(m_SunAngle), std::cosf(m_SunAngle), 0);
		m_SkydomeVertexCBuffer->CopyTo(&skydomeCB, sizeof(SkydomeVertexCB));
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_SkyDomePixelCBSlot, m_SkydomePixelCBufferSlot);
		SkydomePixelCB skydomeCB2;
		skydomeCB2.m_ScreenResolution = XMVectorSet((float)m_Renderer.m_Window.GetWidth(), (float)m_Renderer.m_Window.GetHeight(), 0, 0);
		m_SkydomePixelCBuffer->CopyTo(&skydomeCB2, sizeof(SkydomePixelCB));
		commandList.ExecuteBundle(*m_SkySphereBundle.get());

		// Use water shader
		commandList.SetMaterial(*m_WaterMaterial, true);

		struct WaterVertexCB
		{
			XMMATRIX m_WorldMatrix;
			XMMATRIX m_InverseTransposeMatrix;
			XMMATRIX m_MVP;
		};

		struct WaterPixelCB
		{
			XMVECTOR m_LightDirection;
			XMVECTOR m_CameraPosition;
			XMMATRIX m_M;
			float m_WaterOffset;
		};

		// Water
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_WaterVertexCBSlot, m_WaterVertexCBufferSlot);
		WaterVertexCB waterVertexCB;
		waterVertexCB.m_WorldMatrix = m_WaterTransform->GetTransformMatrix();
		waterVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_WaterTransform->GetTransformMatrix()));
		waterVertexCB.m_MVP = m_WaterTransform->GetTransformMatrix() * vpMatrix;
		m_WaterVertexCBuffer->CopyTo(&waterVertexCB, sizeof(WaterVertexCB));
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_WaterPixelCBSlot, m_WaterPixelCBufferSlot);
		WaterPixelCB waterPixelCB;
		waterPixelCB.m_LightDirection = skydomeCB.m_SunPos;
		waterPixelCB.m_CameraPosition = m_Camera->GetTransform().GetPosition();
		waterPixelCB.m_M = m_WaterTransform->GetTransformMatrix();
		waterPixelCB.m_WaterOffset = m_WaterOffset;
		m_WaterPixelCBuffer->CopyTo(&waterPixelCB, sizeof(WaterPixelCB));
		commandList.ExecuteBundle(*m_WaterBundle.get());

		// Use default shader
		commandList.SetMaterial(*m_DefaultMaterial, true);

		struct DefaultVertexCB
		{
			XMMATRIX m_WorldMatrix;
			XMMATRIX m_InverseTransposeMatrix;
			XMMATRIX m_MVP;
		};

		struct DefaultPixelCB
		{
			XMVECTOR m_LightDirection;
			XMVECTOR m_CameraPosition;
		};

		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultPixelCBSlot, m_LightPositionBufferSlot);
		DefaultPixelCB defaultPixelCB;
		defaultPixelCB.m_LightDirection = skydomeCB.m_SunPos;
		defaultPixelCB.m_CameraPosition = m_Camera->GetTransform().GetPosition();
		m_LightPositionBuffer->CopyTo(&defaultPixelCB, sizeof(DefaultPixelCB));

		// Cube
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultVertexCBSlot, m_CubeMatrixBufferSlot);
		DefaultVertexCB defaultVertexCB;
		defaultVertexCB.m_WorldMatrix = m_CubeTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_CubeTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_CubeTransform->GetTransformMatrix() * vpMatrix;
		m_CubeMatrixBuffer->CopyTo(&defaultVertexCB, sizeof(DefaultVertexCB));
		commandList.ExecuteBundle(*m_CubeBundle.get());

		// Sphere
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultVertexCBSlot, m_SphereMatrixBufferSlot);
		defaultVertexCB.m_WorldMatrix = m_SphereTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_SphereTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_SphereTransform->GetTransformMatrix() * vpMatrix;
		m_SphereMatrixBuffer->CopyTo(&defaultVertexCB, sizeof(DefaultVertexCB));
		commandList.ExecuteBundle(*m_SphereBundle.get());

		// Duck
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultVertexCBSlot, m_DuckMatrixBufferSlot);
		defaultVertexCB.m_WorldMatrix = m_DuckTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_DuckTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_DuckTransform->GetTransformMatrix() * vpMatrix;
		m_DuckMatrixBuffer->CopyTo(&defaultVertexCB, sizeof(DefaultVertexCB));
		commandList.ExecuteBundle(*m_DuckBundle.get());

		// ICMm
		commandList.SetDescriptorHeap(*m_CBVSRVUAVDescriptorHeap.get(), m_DefaultVertexCBSlot, m_ICMmMatrixBufferSlot);
		defaultVertexCB.m_WorldMatrix = m_ICMmTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_ICMmTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_ICMmTransform->GetTransformMatrix() * vpMatrix;
		m_ICMmMatrixBuffer->CopyTo(&defaultVertexCB, sizeof(DefaultVertexCB));
		commandList.ExecuteBundle(*m_ICMmBundle.get());

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
			Application::Quit();
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
		UpdateMouseDelta(Vector2<int>(a_DeltaX, a_DeltaY));
		if (a_RMBDown)
		{
			m_Camera->Rotate(0.0f, -m_MousePositionDelta.GetY() * 0.1f, -m_MousePositionDelta.GetX() * 0.1f);
		}
	}
}
