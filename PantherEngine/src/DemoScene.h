#include "Scene.h"
#include "Material.h"

namespace Panther
{
	class Buffer;
	class Camera;
	class CommandList;
	class DescriptorHeap;
	class Entity;
	class Mesh;
	class Sampler;
	class Shader;
	class StaticMeshRendererComponent;
	class Texture;
	class Transform;

	class DemoScene final : public Scene
	{
	public:
		DemoScene(Renderer& a_Renderer);
		~DemoScene();

		void Load() final override;
		void Unload() final override;
		void Update(float a_Time, float a_DT) final override;
		void Render(CommandList& a_CommandList) final override;
		void OnResize(uint32 a_Width, uint32 a_Height) final override;

	private:
		void LoadShaders();
		void CreateMaterials();
		void CreateGeometry(CommandList& a_CommandList);
		void CreateConstantBuffers();
		void CreateDescriptorHeaps();
		void CreateDescriptors();
		void LoadTextures();
		void CreateEntities();

		std::unique_ptr<DescriptorHeap> m_CBVSRVUAVDescriptorHeap = nullptr;
		std::unique_ptr<DescriptorHeap> m_SamplerDescriptorHeap = nullptr;

		std::unique_ptr<Mesh> m_PlaneMesh = nullptr;
		std::unique_ptr<Mesh> m_CubeMesh = nullptr;
		std::unique_ptr<Mesh> m_SphereMesh = nullptr;
		std::unique_ptr<Mesh> m_DuckMesh = nullptr;

		std::unique_ptr<Buffer> m_AppCBuffer = nullptr;
		std::unique_ptr<Buffer> m_FrameCBuffer = nullptr;
		std::unique_ptr<Buffer> m_ObjectCBuffer = nullptr;

		uint32 m_AppCBElementSlot = 0;
		uint32 m_FrameCBElementSlot = 0;

		uint32 m_AppCBHeapSlot = 0;
		uint32 m_FrameCBHeapSlot = 0;

		std::unique_ptr<Shader> m_SkyShader = nullptr;
		std::unique_ptr<Shader> m_WaterShader = nullptr;
		std::unique_ptr<Shader> m_DefaultShader = nullptr;

		std::unique_ptr<Material> m_SkyDomeMaterial = nullptr;
		std::unique_ptr<Material> m_WaterMaterial = nullptr;
		std::unique_ptr<Material> m_TestMaterial = nullptr;
		std::unique_ptr<Material> m_DuckMaterial = nullptr;

		std::unique_ptr<uint32[]> m_TextureSlots = nullptr;
		std::unique_ptr<Sampler> m_DefaultSampler = nullptr;
		std::unique_ptr<Sampler> m_SkyboxSampler = nullptr;

		uint32 m_DefaultSamplerSlot = 0;
		uint32 m_SkyboxSamplerSlot = 0;

		std::unique_ptr<Camera> m_Camera = nullptr;

		std::unique_ptr<Transform> m_WaterTransform = nullptr;
		std::unique_ptr<Transform> m_CubeTransform = nullptr;
		std::unique_ptr<Transform> m_SphereTransform = nullptr;
		std::unique_ptr<Transform> m_DuckTransform = nullptr;

		std::unique_ptr<Entity> m_Sky = nullptr;
		std::unique_ptr<Entity> m_Water = nullptr;
		std::unique_ptr<Entity> m_Cube = nullptr;
		std::unique_ptr<Entity> m_Sphere = nullptr;
		std::unique_ptr<Entity> m_Duck = nullptr;

		StaticMeshRendererComponent* m_SkyMeshComponent = nullptr;
		StaticMeshRendererComponent* m_WaterMeshComponent = nullptr;
		StaticMeshRendererComponent* m_CubeMeshComponent = nullptr;
		StaticMeshRendererComponent* m_SphereMeshComponent = nullptr;
		StaticMeshRendererComponent* m_DuckMeshComponent = nullptr;
		
		float m_SunAngle = 90.0f;

		float m_Time = 0.0f;

		Vector m_PreviousMousePosition = Vector();
	};
}
