#include "DemoScene.h"

#include "Window.h"
#include "Application.h"
#include "Camera.h"
#include "Transform.h"
#include "TextureManager.h"
#include "Buffer.h"
#include "CommandList.h"
#include "Renderer.h"
#include "Texture.h"
#include "Sampler.h"
#include "Shader.h"
#include "DescriptorHeap.h"
#include "Material.h"
#include "Mesh.h"
#include "Input.h"

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
		L"../rsc/textures/WaterNormal.tga"
	};

	struct SkydomeVertexCB
	{
		XMMATRIX m_MVP;
		Vector m_SunPos;
	};

	struct SkydomePixelCB
	{
		Vector m_ScreenResolution;
	};

	struct WaterPixelCB
	{
		Vector m_LightDirection;
		Vector m_CameraPosition;
		XMMATRIX m_M;
		float m_WaterOffset;
	};

	struct DefaultVertexCB
	{
		XMMATRIX m_WorldMatrix;
		XMMATRIX m_InverseTransposeMatrix;
		XMMATRIX m_MVP;
	};

	struct DefaultPixelCB
	{
		Vector m_LightDirection;
		Vector m_CameraPosition;
	};

	DemoScene::DemoScene(Renderer& a_Renderer) 
		: Scene(a_Renderer) 
	{
		m_TextureSlots = std::make_unique<TextureID[]>(Countof(g_Textures));
		m_TextureManager = std::make_unique<TextureManager>(a_Renderer, (uint32)Countof(g_Textures));
	}

	DemoScene::~DemoScene()
	{
	}

	void DemoScene::Load()
	{
		CommandList& commandList(m_Renderer.StartRecording());

		LoadShaders();
		CreateGeometry(commandList);	
		CreateConstantBuffers();
		CreateDescriptorHeaps();
		CreateDescriptors();
		CreateMaterials();

		commandList.Close();

		CommandList* commandLists[] = {&commandList};
		m_Renderer.SubmitCommandLists(commandLists, 1);
		m_Renderer.Synchronize();

		m_Camera = std::make_unique<Camera>(Transform(Vector(0, 0, -10)));
		m_Camera->SetAspectRatio(static_cast<float>(m_Renderer.GetWindow().GetWidth()) / m_Renderer.GetWindow().GetHeight());

		m_WaterTransform = std::make_unique<Transform>(Vector(0, -3, 0), Vector(XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(-90))), Vector(10, 10, 10));
		m_CubeTransform = std::make_unique<Transform>(Vector(-3, 0, 0));
		m_SphereTransform = std::make_unique<Transform>(Vector(0, 0, 0));
		m_DuckTransform = std::make_unique<Transform>(Vector(3, 0, 0));
	}

	void DemoScene::LoadShaders()
	{
		m_SkyShader = std::unique_ptr<Shader>(m_Renderer.CreateShader());
		m_SkyShader->LoadVertexShader(L"../rsc/shaders/skydome.hlsl", "VSMain");
		m_SkyShader->LoadPixelShader(L"../rsc/shaders/skydome.hlsl", "PSMain");
		m_SkyShader->Finalize();

		m_WaterShader = std::unique_ptr<Shader>(m_Renderer.CreateShader());
		m_WaterShader->LoadVertexShader(L"../rsc/shaders/water.hlsl", "VSMain");
		m_WaterShader->LoadPixelShader(L"../rsc/shaders/water.hlsl", "PSMain");
		m_WaterShader->Finalize();

		m_DefaultShader = std::unique_ptr<Shader>(m_Renderer.CreateShader());
		m_DefaultShader->LoadVertexShader(L"../rsc/shaders/shaders.hlsl", "VSMain");
		m_DefaultShader->LoadPixelShader(L"../rsc/shaders/shaders.hlsl", "PSMain");
		m_DefaultShader->Finalize();
	}

	void DemoScene::CreateMaterials()
	{
		m_SkyDomeMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_SkyShader.get(), DepthWrite::Off));
		m_SkyDomeMaterial->SetResource("VertexCB", *m_CBVSRVUAVDescriptorHeap.get(), m_SkydomeVertexCBufferSlot);
		m_SkyDomeMaterial->SetResource("PixelCB", *m_CBVSRVUAVDescriptorHeap.get(), m_SkydomePixelCBufferSlot);
		m_SkyDomeMaterial->SetResource("dayTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[2]);
		m_SkyDomeMaterial->SetResource("nightTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[3]);
		m_SkyDomeMaterial->SetResource("sunTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[4]);
		m_SkyDomeMaterial->SetResource("moonTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[5]);
		m_SkyDomeMaterial->SetResource("clampedSampler", *m_SamplerDescriptorHeap.get(), m_SkyboxSamplerSlot);

		m_WaterMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_WaterShader.get(), DepthWrite::On));
		m_WaterMaterial->SetResource("VertexConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_WaterVertexCBufferSlot);
		m_WaterMaterial->SetResource("PixelConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_WaterPixelCBufferSlot);
		m_WaterMaterial->SetResource("waterTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[6]);
		m_WaterMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);

		m_TestMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_DefaultShader.get(), DepthWrite::On));
		m_TestMaterial->SetResource("diffuseTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[0]);
		m_TestMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);
		m_TestMaterial->SetResource("PixelConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_LightPositionBufferSlot);

		m_DuckMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_DefaultShader.get(), DepthWrite::On));
		m_DuckMaterial->SetResource("diffuseTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[1]);
		m_DuckMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);
		m_DuckMaterial->SetResource("PixelConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_LightPositionBufferSlot);
		m_DuckMaterial->SetResource("VertexConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_DuckMatrixBufferSlot);
	}

	void DemoScene::CreateGeometry(CommandList& a_CommandList)
	{
		m_PlaneMesh = std::unique_ptr<Mesh>(m_Renderer.CreateMesh());
		m_PlaneMesh->InitAsPlane(a_CommandList);

		m_CubeMesh = std::unique_ptr<Mesh>(m_Renderer.CreateMesh());
		m_CubeMesh->InitAsCube(a_CommandList);

		m_SphereMesh = std::unique_ptr<Mesh>(m_Renderer.CreateMesh());
		m_SphereMesh->InitAsSphere(a_CommandList);

		m_DuckMesh = std::unique_ptr<Mesh>(m_Renderer.CreateMesh());
		m_DuckMesh->InitViaASSIMP(a_CommandList, "../rsc/models/duck.fbx");
	}

	void DemoScene::CreateConstantBuffers()
	{
		m_DefaultVertexCBuffer	= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(4, sizeof(DefaultVertexCB)));
		m_WaterPixelCBuffer		= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(1, sizeof(WaterPixelCB)));
		m_SkydomeVertexCBuffer	= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(1, sizeof(SkydomeVertexCB)));
		m_LightPositionBuffer	= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(1, sizeof(DefaultPixelCB)));
		m_SkydomePixelCBuffer	= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(1, sizeof(SkydomePixelCB)));
	}

	void DemoScene::CreateDescriptorHeaps()
	{		
		uint32 CBVSRVUAVHeapSize = 8 + (uint32)Countof(g_Textures);
		m_CBVSRVUAVDescriptorHeap = std::unique_ptr<DescriptorHeap>(m_Renderer.CreateDescriptorHeap(CBVSRVUAVHeapSize, DescriptorHeapType::ConstantBufferView));

		uint32 samplerHeapSize = 2;
		m_SamplerDescriptorHeap = std::unique_ptr<DescriptorHeap>(m_Renderer.CreateDescriptorHeap(samplerHeapSize, DescriptorHeapType::Sampler));
	}

	void DemoScene::CreateDescriptors()
	{
		m_WaterPixelCBufferSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_WaterPixelCBuffer.get(), 0);
		m_CubeMatrixBufferSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_DefaultVertexCBuffer.get(), 0);
		m_SphereMatrixBufferSlot	= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_DefaultVertexCBuffer.get(), 1);
		m_DuckMatrixBufferSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_DefaultVertexCBuffer.get(), 2);
		m_WaterVertexCBufferSlot	= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_DefaultVertexCBuffer.get(), 3);
		m_SkydomeVertexCBufferSlot	= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SkydomeVertexCBuffer.get(), 0);
		m_SkydomePixelCBufferSlot	= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_SkydomePixelCBuffer.get(), 0);
		m_LightPositionBufferSlot	= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_LightPositionBuffer.get(), 0);

		LoadTextures();

		m_DefaultSampler = std::unique_ptr<Sampler>(m_Renderer.CreateSampler(SamplerTextureCoordinateMode::Wrap));
		m_SkyboxSampler = std::unique_ptr<Sampler>(m_Renderer.CreateSampler(SamplerTextureCoordinateMode::Clamp));

		m_DefaultSamplerSlot = m_SamplerDescriptorHeap->RegisterSampler(*m_DefaultSampler.get());
		m_SkyboxSamplerSlot = m_SamplerDescriptorHeap->RegisterSampler(*m_SkyboxSampler.get());
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
		Vector mousePosition(Input::GetMousePosition());
		Vector mouseDelta = mousePosition - m_PreviousMousePosition;
		if (Input::GetKey(Key::RButton))
		{
			m_Camera->Rotate(0.0f, -mouseDelta.Y() * 0.1f, -mouseDelta.X() * 0.1f);
		}

		Vector rotation(XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), XMConvertToRadians(90 * a_DT)));

		m_CubeTransform->Rotate(rotation);
		m_SphereTransform->Rotate(rotation);
		m_DuckTransform->Rotate(rotation);

		float speedMultipler = Input::GetKey(Key::ShiftKey) ? 8.0f : 4.0f;
		Vector cameraTranslate = Vector(static_cast<float>(Input::GetKey(Key::D) - Input::GetKey(Key::A)), 0.0f, static_cast<float>(Input::GetKey(Key::W) - Input::GetKey(Key::S)), 1.0f) * speedMultipler * a_DT;
		Vector cameraPan = Vector(0.0f, static_cast<float>(Input::GetKey(Key::E) - Input::GetKey(Key::Q)), 0.0f, 1.0f) * speedMultipler * a_DT;
		m_Camera->Translate(cameraTranslate, Space::Local);
		m_Camera->Translate(cameraPan, Space::World);

		const float pi = 3.14159265359f;
		m_SunAngle += XMConvertToRadians(5) * a_DT; 
		if (m_SunAngle > (pi * 2))
		{
			m_SunAngle -= (pi * 2);
		}

		m_WaterOffset = fmodf(m_WaterOffset + 0.05f * a_DT, 1.0f);

		if (Input::GetKey(Key::Escape))
		{
			Application::Quit();
		}

		m_PreviousMousePosition = mousePosition;
	}

	void DemoScene::Render(CommandList& a_CommandList)
	{
		a_CommandList.UseDefaultViewport();

		DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
		a_CommandList.UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));

		XMMATRIX vpMatrix = m_Camera->GetViewProjectionMatrix();
		
		// Sky
		SkydomeVertexCB skydomeCB;
		skydomeCB.m_MVP = m_Camera->GetSkyMatrix() * vpMatrix;
		skydomeCB.m_SunPos = Vector(0, std::sinf(m_SunAngle), std::cosf(m_SunAngle), 0);
		m_SkydomeVertexCBuffer->CopyTo(0, &skydomeCB, sizeof(SkydomeVertexCB));

		SkydomePixelCB skydomeCB2;
		skydomeCB2.m_ScreenResolution = Vector((float)m_Renderer.GetWindow().GetWidth(), (float)m_Renderer.GetWindow().GetHeight());
		m_SkydomePixelCBuffer->CopyTo(0, &skydomeCB2, sizeof(SkydomePixelCB));

		m_SkyDomeMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_SphereMesh);
		a_CommandList.Draw(m_SphereMesh->GetNumIndices());

		// Water
		DefaultVertexCB defaultVertexCB;
		defaultVertexCB.m_WorldMatrix = m_WaterTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_WaterTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_WaterTransform->GetTransformMatrix() * vpMatrix;
		m_DefaultVertexCBuffer->CopyTo(3, &defaultVertexCB, sizeof(DefaultVertexCB));

		WaterPixelCB waterPixelCB;
		waterPixelCB.m_LightDirection = skydomeCB.m_SunPos;
		waterPixelCB.m_CameraPosition = m_Camera->GetTransform().GetPosition();
		waterPixelCB.m_M = m_WaterTransform->GetTransformMatrix();
		waterPixelCB.m_WaterOffset = m_WaterOffset;
		m_WaterPixelCBuffer->CopyTo(0, &waterPixelCB, sizeof(WaterPixelCB));

		m_WaterMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_PlaneMesh);
		a_CommandList.Draw(m_PlaneMesh->GetNumIndices());
		
		// Default material
		DefaultPixelCB defaultPixelCB;
		defaultPixelCB.m_LightDirection = skydomeCB.m_SunPos;
		defaultPixelCB.m_CameraPosition = m_Camera->GetTransform().GetPosition();
		m_LightPositionBuffer->CopyTo(0, &defaultPixelCB, sizeof(DefaultPixelCB));

		// Cube
		defaultVertexCB.m_WorldMatrix = m_CubeTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_CubeTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_CubeTransform->GetTransformMatrix() * vpMatrix;
		m_DefaultVertexCBuffer->CopyTo(0, &defaultVertexCB, sizeof(DefaultVertexCB));

		m_TestMaterial->SetResource("VertexConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_CubeMatrixBufferSlot);
		m_TestMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_CubeMesh);
		a_CommandList.Draw(m_CubeMesh->GetNumIndices());

		// Sphere
		defaultVertexCB.m_WorldMatrix = m_SphereTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_SphereTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_SphereTransform->GetTransformMatrix() * vpMatrix;
		m_DefaultVertexCBuffer->CopyTo(1, &defaultVertexCB, sizeof(DefaultVertexCB));

		m_TestMaterial->SetResource("VertexConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_SphereMatrixBufferSlot, a_CommandList);
		a_CommandList.SetMesh(*m_SphereMesh);
		a_CommandList.Draw(m_SphereMesh->GetNumIndices());

		// Duck
		defaultVertexCB.m_WorldMatrix = m_DuckTransform->GetTransformMatrix();
		defaultVertexCB.m_InverseTransposeMatrix = XMMatrixTranspose(XMMatrixInverse(nullptr, m_DuckTransform->GetTransformMatrix()));
		defaultVertexCB.m_MVP = m_DuckTransform->GetTransformMatrix() * vpMatrix;
		m_DefaultVertexCBuffer->CopyTo(2, &defaultVertexCB, sizeof(DefaultVertexCB));

		m_DuckMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_DuckMesh);
		a_CommandList.Draw(m_DuckMesh->GetNumIndices());
	}

	void DemoScene::OnResize(uint32 a_Width, uint32 a_Height)
	{
		m_Camera->SetAspectRatio(static_cast<float>(m_Renderer.GetWindow().GetWidth()) / m_Renderer.GetWindow().GetHeight());
	}
}
