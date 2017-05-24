#include "Scene.h"
#include "Material.h"

namespace Panther
{
	class Buffer;
	class CommandList;
	class DescriptorHeap;
	class Mesh;
	class Sampler;
	class Shader;
	class Texture;
	class Camera;
	class Transform;

	class DemoScene final : public Scene
	{
	public:
		DemoScene(Renderer& a_Renderer);
		~DemoScene();

		void Load() final override;
		void Unload() final override;
		void Update(float a_DT) final override;
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

		std::unique_ptr<DescriptorHeap> m_CBVSRVUAVDescriptorHeap = nullptr;
		std::unique_ptr<DescriptorHeap> m_SamplerDescriptorHeap = nullptr;

		std::unique_ptr<Mesh> m_PlaneMesh = nullptr;
		std::unique_ptr<Mesh> m_CubeMesh = nullptr;
		std::unique_ptr<Mesh> m_SphereMesh = nullptr;
		std::unique_ptr<Mesh> m_DuckMesh = nullptr;

		std::unique_ptr<Buffer> m_AppCBuffer = nullptr;
		std::unique_ptr<Buffer> m_FrameCBuffer = nullptr;
		std::unique_ptr<Buffer> m_ObjectCBuffer = nullptr;
		// Legacy below
		std::unique_ptr<Buffer> m_WaterPixelCBuffer = nullptr;
		std::unique_ptr<Buffer> m_LightPositionBuffer = nullptr;

		uint32 m_AppCBSlot = 0;
		uint32 m_FrameCBSlot = 0;
		uint32 m_SkyObjectCBSlot = 0;
		uint32 m_WaterObjectCBSlot = 0;
		uint32 m_CubeObjectCBSlot = 0;
		uint32 m_SphereObjectCBSlot = 0;
		uint32 m_DuckObjectCBSlot = 0;
		// Legacy below
		uint32 m_WaterPixelCBufferSlot = 0;
		uint32 m_LightPositionBufferSlot = 0;

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
		
		float m_SunAngle = 90.0f;

		float m_WaterOffset = 0.0f;

		Vector m_PreviousMousePosition = Vector();
	};
}
