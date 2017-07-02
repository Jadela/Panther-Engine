#include "DemoScene.h"

#include "Application.h"
#include "Buffer.h"
#include "Camera.h"
#include "CommandList.h"
#include "DescriptorHeap.h"
#include "Entity.h"
#include "Input.h"
#include "Material.h"
#include "Mesh.h"
#include "Sampler.h"
#include "Shader.h"
#include "StaticMeshRendererComponent.h"
#include "Renderer.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Transform.h"
#include "Window.h"

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
		CreateEntities();

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
		m_SkyShader->LoadVertexShader(L"../rsc/shaders/Sky.hlsl", "VSMain");
		m_SkyShader->LoadPixelShader(L"../rsc/shaders/Sky.hlsl", "PSMain");
		m_SkyShader->Finalize();

		m_WaterShader = std::unique_ptr<Shader>(m_Renderer.CreateShader());
		m_WaterShader->LoadVertexShader(L"../rsc/shaders/Water.hlsl", "VSMain");
		m_WaterShader->LoadPixelShader(L"../rsc/shaders/Water.hlsl", "PSMain");
		m_WaterShader->Finalize();

		m_DefaultShader = std::unique_ptr<Shader>(m_Renderer.CreateShader());
		m_DefaultShader->LoadVertexShader(L"../rsc/shaders/Default.hlsl", "VSMain");
		m_DefaultShader->LoadPixelShader(L"../rsc/shaders/Default.hlsl", "PSMain");
		m_DefaultShader->Finalize();
	}

	void DemoScene::CreateMaterials()
	{
		m_SkyDomeMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_SkyShader.get(), DepthWrite::Off));
		m_SkyDomeMaterial->SetResource("AppCB", *m_CBVSRVUAVDescriptorHeap.get(), m_AppCBHeapSlot);
		m_SkyDomeMaterial->SetResource("FrameCB", *m_CBVSRVUAVDescriptorHeap.get(), m_FrameCBHeapSlot);
		m_SkyDomeMaterial->SetResource("dayTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[2]);
		m_SkyDomeMaterial->SetResource("nightTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[3]);
		m_SkyDomeMaterial->SetResource("sunTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[4]);
		m_SkyDomeMaterial->SetResource("moonTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[5]);
		m_SkyDomeMaterial->SetResource("clampedSampler", *m_SamplerDescriptorHeap.get(), m_SkyboxSamplerSlot);

		m_WaterMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_WaterShader.get(), DepthWrite::On));
		m_WaterMaterial->SetResource("FrameCB", *m_CBVSRVUAVDescriptorHeap.get(), m_FrameCBHeapSlot);
		m_WaterMaterial->SetResource("waterTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[6]);
		m_WaterMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);

		m_TestMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_DefaultShader.get(), DepthWrite::On));
		m_TestMaterial->SetResource("FrameCB", *m_CBVSRVUAVDescriptorHeap.get(), m_FrameCBHeapSlot);
		m_TestMaterial->SetResource("diffuseTexture", *m_CBVSRVUAVDescriptorHeap.get(), m_TextureSlots[0]);
		m_TestMaterial->SetResource("defaultSampler", *m_SamplerDescriptorHeap.get(), m_DefaultSamplerSlot);

		m_DuckMaterial = std::unique_ptr<Material>(m_Renderer.CreateMaterial(*m_DefaultShader.get(), DepthWrite::On));
		m_DuckMaterial->SetResource("FrameCB", *m_CBVSRVUAVDescriptorHeap.get(), m_FrameCBHeapSlot);
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
		m_ObjectCBuffer		= std::unique_ptr<Buffer>(m_Renderer.CreateBuffer(5, sizeof(StaticMeshRendererComponent::ObjectCB)));

		m_AppCBElementSlot		= m_AppCBuffer->GetSlot();
		m_FrameCBElementSlot	= m_FrameCBuffer->GetSlot();
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
		m_AppCBHeapSlot				= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_AppCBuffer.get(), m_AppCBElementSlot);
		m_FrameCBHeapSlot			= m_CBVSRVUAVDescriptorHeap->RegisterConstantBuffer(*m_FrameCBuffer.get(), m_FrameCBElementSlot);

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

	void DemoScene::CreateEntities()
	{
		m_SkyMeshComponent = new StaticMeshRendererComponent(*m_ObjectCBuffer.get(), *m_CBVSRVUAVDescriptorHeap.get(), m_SphereMesh.get(), m_SkyDomeMaterial.get());
		m_WaterMeshComponent = new StaticMeshRendererComponent(*m_ObjectCBuffer.get(), *m_CBVSRVUAVDescriptorHeap.get(), m_PlaneMesh.get(), m_WaterMaterial.get());
		m_CubeMeshComponent = new StaticMeshRendererComponent(*m_ObjectCBuffer.get(), *m_CBVSRVUAVDescriptorHeap.get(), m_CubeMesh.get(), m_TestMaterial.get());
		m_SphereMeshComponent = new StaticMeshRendererComponent(*m_ObjectCBuffer.get(), *m_CBVSRVUAVDescriptorHeap.get(), m_SphereMesh.get(), m_TestMaterial.get());
		m_DuckMeshComponent = new StaticMeshRendererComponent(*m_ObjectCBuffer.get(), *m_CBVSRVUAVDescriptorHeap.get(), m_DuckMesh.get(), m_DuckMaterial.get());

		m_Sky = std::make_unique<Entity>();
		m_Sky->AddComponent(m_SkyMeshComponent);
		m_Water = std::make_unique<Entity>();
		m_Water->AddComponent(m_WaterMeshComponent);
		m_Cube = std::make_unique<Entity>();
		m_Cube->AddComponent(m_CubeMeshComponent);
		m_Sphere = std::make_unique<Entity>();
		m_Sphere->AddComponent(m_SphereMeshComponent);
		m_Duck = std::make_unique<Entity>();
		m_Duck->AddComponent(m_DuckMeshComponent);
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
		m_FrameCBuffer->CopyTo(m_FrameCBElementSlot, &frameCB, sizeof(FrameCB));

		// Sky
		m_SkyMeshComponent->UpdateObjectCB(m_Camera->GetSkyMatrix(), vpMatrix);
		m_SkyMeshComponent->Record(a_CommandList);

		// Water
		m_WaterMeshComponent->UpdateObjectCB(m_WaterTransform->GetTransformMatrix(), vpMatrix);
		m_WaterMeshComponent->Record(a_CommandList);
		
		// Cube
		m_CubeMeshComponent->UpdateObjectCB(m_CubeTransform->GetTransformMatrix(), vpMatrix);
		m_CubeMeshComponent->Record(a_CommandList);

		// Sphere
		m_SphereMeshComponent->UpdateObjectCB(m_SphereTransform->GetTransformMatrix(), vpMatrix);
		m_SphereMeshComponent->Record(a_CommandList);

		// Duck
		m_DuckMeshComponent->UpdateObjectCB(m_DuckTransform->GetTransformMatrix(), vpMatrix);
		m_DuckMeshComponent->Record(a_CommandList);
	}

	void DemoScene::OnResize(uint32 a_Width, uint32 a_Height)
	{
		m_Camera->SetAspectRatio(static_cast<float>(m_Renderer.GetWindow().GetWidth()) / m_Renderer.GetWindow().GetHeight());

		AppCB appCB;
		appCB.m_ScreenResolution = Vector((float)m_Renderer.GetWindow().GetWidth(), (float)m_Renderer.GetWindow().GetHeight());
		m_AppCBuffer->CopyTo(m_AppCBElementSlot, &appCB, sizeof(AppCB));
	}
}
