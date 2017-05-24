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

	struct AppCB
	{
		Vector m_ScreenResolution;
	};

	struct FrameCB
	{
		Vector m_Light0Direction;
		Vector m_CameraPosition;
		float m_Time;
	};

	struct ObjectCB
	{
		XMMATRIX m_MVP;
		XMMATRIX m_M;
		XMMATRIX m_IT_M;
	};

	struct WaterPixelCB
	{
		Vector m_LightDirection;
		Vector m_CameraPosition;
		XMMATRIX m_M;
		float m_WaterOffset;
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

		m_WaterTransform = std::make_unique<Transform>(Vector(0, -3, 0), Vector(XMQuaternionRotationAxis(XMVectorSet(1, 0, 0, 0), XMConvertToRadians(-90))), Vector(10, 10, 10));
		m_CubeTransform = std::make_unique<Transform>(Vector(-3, 0, 0));
		m_SphereTransform = std::make_unique<Transform>(Vector(0, 0, 0));
		m_DuckTransform = std::make_unique<Transform>(Vector(3, 0, 0));

		OnResize(m_Renderer.GetWindow().GetWidth(), m_Renderer.GetWindow().GetHeight());
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
		m_SkyDomeMaterial->SetResource("AppCB", *m_CBVSRVUAVDescriptorHeap.get(), m_AppCBSlot);
		m_SkyDomeMaterial->SetResource("FrameCB", *m_CBVSRVUAVDescriptorHeap.get(), m_FrameCBSlot);
		m_SkyDomeMaterial->SetResource("ObjectCB", *m_CBVSRVUAVDescriptorHeap.get(), m_SkyObjectCBSlot);
		m_SkyDomeMaterial->SetResource("dayTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[2]);
		m_SkyDomeMaterial->SetResource("nightTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[3]);
		m_SkyDomeMaterial->SetResource("sunTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[4]);
		m_SkyDomeMaterial->SetResource("moonTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[5]);
		m_SkyDomeMaterial->SetResource("clampedSampler", *m_SamplerDescriptorHeap.get(), m_SkyboxSamplerSlot);

		m_WaterMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_WaterShader.get(), DepthWrite::On));
		m_WaterMaterial->SetResource("ObjectCB", *m_CBVSRVUAVDescriptorHeap.get(), m_WaterObjectCBSlot);
		m_WaterMaterial->SetResource("PixelConstants", *m_CBVSRVUAVDescriptorHeap.get(), m_WaterPixelCBufferSlot);
		m_WaterMaterial->SetResource("waterTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[6]);
		m_WaterMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);

		m_TestMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_DefaultShader.get(), DepthWrite::On));
		m_TestMaterial->SetResource("FrameCB", *m_CBVSRVUAVDescriptorHeap.get(), m_FrameCBSlot);
		m_TestMaterial->SetResource("diffuseTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[0]);
		m_TestMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);

		m_DuckMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_DefaultShader.get(), DepthWrite::On));
		m_DuckMaterial->SetResource("FrameCB", *m_CBVSRVUAVDescriptorHeap.get(), m_FrameCBSlot);
		m_DuckMaterial->SetResource("ObjectCB", *m_CBVSRVUAVDescriptorHeap.get(), m_DuckObjectCBSlot);
		m_DuckMaterial->SetResource("diffuseTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[1]);
		m_DuckMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);
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
		m_AppCBuffer		= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(1, sizeof(AppCB)));
		m_FrameCBuffer		= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(1, sizeof(FrameCB)));
		m_ObjectCBuffer		= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(5, sizeof(ObjectCB)));

		m_WaterPixelCBuffer		= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(1, sizeof(WaterPixelCB)));
	}

	void DemoScene::CreateDescriptorHeaps()
	{		
		uint32 CBVSRVUAVHeapSize = 9 + (uint32)Countof(g_Textures);
		m_CBVSRVUAVDescriptorHeap = std::unique_ptr<DescriptorHeap>(m_Renderer.CreateDescriptorHeap(CBVSRVUAVHeapSize, DescriptorHeapType::ConstantBufferView));

		uint32 samplerHeapSize = 2;
		m_SamplerDescriptorHeap = std::unique_ptr<DescriptorHeap>(m_Renderer.CreateDescriptorHeap(samplerHeapSize, DescriptorHeapType::Sampler));
	}

	void DemoScene::CreateDescriptors()
	{
		m_AppCBSlot				= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_AppCBuffer.get(), 0);
		m_FrameCBSlot			= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_FrameCBuffer.get(), 0);
		m_SkyObjectCBSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ObjectCBuffer.get(), 0);
		m_WaterObjectCBSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ObjectCBuffer.get(), 1);
		m_CubeObjectCBSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ObjectCBuffer.get(), 2);
		m_SphereObjectCBSlot	= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ObjectCBuffer.get(), 3);
		m_DuckObjectCBSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_ObjectCBuffer.get(), 4);

		m_WaterPixelCBufferSlot		= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_WaterPixelCBuffer.get(), 0);

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

		if (Input::GetKey(Key::D1))
		{
			m_TestMaterial->SetWireframe(true);
			m_DuckMaterial->SetWireframe(true);
			m_WaterMaterial->SetWireframe(true);
			m_SkyDomeMaterial->SetWireframe(true);
		}

		if (Input::GetKey(Key::D2))
		{
			m_TestMaterial->SetWireframe(false);
			m_DuckMaterial->SetWireframe(false);
			m_WaterMaterial->SetWireframe(false);
			m_SkyDomeMaterial->SetWireframe(false);
		}

		m_PreviousMousePosition = mousePosition;
	}

	void DemoScene::Render(CommandList& a_CommandList)
	{
		a_CommandList.UseDefaultViewport();

		DescriptorHeap* usedHeaps[] = { m_CBVSRVUAVDescriptorHeap.get(), m_SamplerDescriptorHeap.get() };
		a_CommandList.UseDescriptorHeaps(usedHeaps, (uint32)Countof(usedHeaps));

		XMMATRIX vpMatrix = m_Camera->GetViewProjectionMatrix();
		
		FrameCB frameCB;
		frameCB.m_Light0Direction = Vector(0, std::sinf(m_SunAngle), std::cosf(m_SunAngle), 0);
		frameCB.m_CameraPosition = m_Camera->GetTransform().GetPosition();
		frameCB.m_Time = m_WaterOffset; // TODO (JDL): Change this.
		m_FrameCBuffer->CopyTo(0, &frameCB, sizeof(FrameCB));

		// Sky
		ObjectCB objectCB;
		objectCB.m_MVP = m_Camera->GetSkyMatrix() * vpMatrix;
		objectCB.m_M = m_Camera->GetSkyMatrix();
		objectCB.m_IT_M = XMMatrixTranspose(XMMatrixInverse(nullptr, m_Camera->GetSkyMatrix()));
		m_ObjectCBuffer->CopyTo(0, &objectCB, sizeof(ObjectCB));

		m_SkyDomeMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_SphereMesh);
		a_CommandList.Draw(m_SphereMesh->GetNumIndices());

		// Water
		objectCB.m_MVP = m_WaterTransform->GetTransformMatrix() * vpMatrix;
		objectCB.m_M = m_WaterTransform->GetTransformMatrix();
		objectCB.m_IT_M = XMMatrixTranspose(XMMatrixInverse(nullptr, m_WaterTransform->GetTransformMatrix()));
		m_ObjectCBuffer->CopyTo(1, &objectCB, sizeof(ObjectCB));

		WaterPixelCB waterPixelCB;
		waterPixelCB.m_LightDirection = frameCB.m_Light0Direction;
		waterPixelCB.m_CameraPosition = m_Camera->GetTransform().GetPosition();
		waterPixelCB.m_M = m_WaterTransform->GetTransformMatrix();
		waterPixelCB.m_WaterOffset = m_WaterOffset;
		m_WaterPixelCBuffer->CopyTo(0, &waterPixelCB, sizeof(WaterPixelCB));

		m_WaterMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_PlaneMesh);
		a_CommandList.Draw(m_PlaneMesh->GetNumIndices());
		
		// Cube
		objectCB.m_MVP = m_CubeTransform->GetTransformMatrix() * vpMatrix;
		objectCB.m_M = m_CubeTransform->GetTransformMatrix();
		objectCB.m_IT_M = XMMatrixTranspose(XMMatrixInverse(nullptr, m_CubeTransform->GetTransformMatrix()));
		m_ObjectCBuffer->CopyTo(2, &objectCB, sizeof(ObjectCB));

		m_TestMaterial->SetResource("ObjectCB", *m_CBVSRVUAVDescriptorHeap.get(), m_CubeObjectCBSlot);
		m_TestMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_CubeMesh);
		a_CommandList.Draw(m_CubeMesh->GetNumIndices());

		// Sphere
		objectCB.m_MVP = m_SphereTransform->GetTransformMatrix() * vpMatrix;
		objectCB.m_M = m_SphereTransform->GetTransformMatrix();
		objectCB.m_IT_M = XMMatrixTranspose(XMMatrixInverse(nullptr, m_SphereTransform->GetTransformMatrix()));
		m_ObjectCBuffer->CopyTo(3, &objectCB, sizeof(ObjectCB));

		m_TestMaterial->SetResource("ObjectCB", *m_CBVSRVUAVDescriptorHeap.get(), m_SphereObjectCBSlot, a_CommandList);
		a_CommandList.SetMesh(*m_SphereMesh);
		a_CommandList.Draw(m_SphereMesh->GetNumIndices());

		// Duck
		objectCB.m_MVP = m_DuckTransform->GetTransformMatrix() * vpMatrix;
		objectCB.m_M = m_DuckTransform->GetTransformMatrix();
		objectCB.m_IT_M = XMMatrixTranspose(XMMatrixInverse(nullptr, m_DuckTransform->GetTransformMatrix()));
		m_ObjectCBuffer->CopyTo(4, &objectCB, sizeof(ObjectCB));

		m_DuckMaterial->Use(a_CommandList);
		a_CommandList.SetMesh(*m_DuckMesh);
		a_CommandList.Draw(m_DuckMesh->GetNumIndices());
	}

	void DemoScene::OnResize(uint32 a_Width, uint32 a_Height)
	{
		m_Camera->SetAspectRatio(static_cast<float>(m_Renderer.GetWindow().GetWidth()) / m_Renderer.GetWindow().GetHeight());

		AppCB appCB;
		appCB.m_ScreenResolution = Vector((float)m_Renderer.GetWindow().GetWidth(), (float)m_Renderer.GetWindow().GetHeight());
		m_AppCBuffer->CopyTo(0, &appCB, sizeof(AppCB));
	}
}
