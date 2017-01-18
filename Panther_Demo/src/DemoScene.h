#include "Scene.h"
#include "../../Panther_Renderer/src/Material.h"

namespace Panther
{
	// Panther Renderer
	class Buffer;
	class CommandList;
	class DescriptorHeap;
	class Mesh;
	class Sampler;
	class Texture;
	// Panther Utilities
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
		void Render() final override;
		void OnResize(uint32 a_Width, uint32 a_Height) final override;
		void OnKeyDown(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt) final override;
		void OnKeyUp(Key a_Key, uint32 a_Character, KeyState a_KeyState, bool a_Ctrl, bool a_Shift, bool a_Alt) final override;
		void OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown, bool a_RMBDown) final override;

	private:
		void LoadTextures();

		std::unique_ptr<DescriptorHeap> m_CBVSRVUAVDescriptorHeap = nullptr;
		std::unique_ptr<DescriptorHeap> m_SamplerDescriptorHeap = nullptr;

		std::unique_ptr<CommandList> m_SkySphereBundle = nullptr;
		std::unique_ptr<CommandList> m_WaterBundle = nullptr;
		std::unique_ptr<CommandList> m_CubeBundle = nullptr;
		std::unique_ptr<CommandList> m_SphereBundle = nullptr;
		std::unique_ptr<CommandList> m_DuckBundle = nullptr;

		std::unique_ptr<Mesh> m_PlaneMesh = nullptr;
		std::unique_ptr<Mesh> m_CubeMesh = nullptr;
		std::unique_ptr<Mesh> m_SphereMesh = nullptr;
		std::unique_ptr<Mesh> m_DuckMesh = nullptr;

		std::unique_ptr<Buffer> m_WaterVertexCBuffer = nullptr;
		std::unique_ptr<Buffer> m_WaterPixelCBuffer = nullptr;
		std::unique_ptr<Buffer> m_CubeMatrixBuffer = nullptr;
		std::unique_ptr<Buffer> m_SphereMatrixBuffer = nullptr;
		std::unique_ptr<Buffer> m_DuckMatrixBuffer = nullptr;
		std::unique_ptr<Buffer> m_SkydomeVertexCBuffer = nullptr;
		std::unique_ptr<Buffer> m_LightPositionBuffer = nullptr;
		std::unique_ptr<Buffer> m_SkydomePixelCBuffer = nullptr;

		uint32 m_WaterVertexCBufferSlot = 0;
		uint32 m_WaterPixelCBufferSlot = 0;
		uint32 m_CubeMatrixBufferSlot = 0;
		uint32 m_SphereMatrixBufferSlot = 0;
		uint32 m_DuckMatrixBufferSlot = 0;
		uint32 m_SkydomeVertexCBufferSlot = 0;
		uint32 m_LightPositionBufferSlot = 0;
		uint32 m_SkydomePixelCBufferSlot = 0;

		// Skybox Material
		std::unique_ptr<Material> m_SkyDomeMaterial = nullptr;
		Material::DescriptorSlot m_SkyDomeVertexCBSlot = {};
		Material::DescriptorSlot m_SkyDomePixelCBSlot = {};
		Material::DescriptorSlot m_SkyDomeTexturesSlot = {};
		Material::DescriptorSlot m_SkyDomeClampedSamplerSlot = {};

		// Water Material
		std::unique_ptr<Material> m_WaterMaterial = nullptr;
		Material::DescriptorSlot m_WaterVertexCBSlot = {};
		Material::DescriptorSlot m_WaterPixelCBSlot = {};
		Material::DescriptorSlot m_WaterTexture0Slot = {};
		Material::DescriptorSlot m_WaterSamplerSlot = {};

		// Default Material
		std::unique_ptr<Material> m_DefaultMaterial = nullptr;
		Material::DescriptorSlot m_DefaultVertexCBSlot = {};
		Material::DescriptorSlot m_DefaultPixelCBSlot = {};
		Material::DescriptorSlot m_DefaultTextureSlot = {};
		Material::DescriptorSlot m_DefaultSamplerDescriptorSlot = {};

		uint32* m_TextureSlots = nullptr;
		std::unique_ptr<Sampler> m_DefaultSampler = nullptr;
		std::unique_ptr<Sampler> m_SkyboxSampler = nullptr;

		uint32 m_DefaultSamplerSlot = 0;
		uint32 m_SkyboxSamplerSlot = 0;

		std::unique_ptr<Camera> m_Camera = nullptr;

		std::unique_ptr<Transform> m_WaterTransform = nullptr;
		std::unique_ptr<Transform> m_CubeTransform = nullptr;
		std::unique_ptr<Transform> m_SphereTransform = nullptr;
		std::unique_ptr<Transform> m_DuckTransform = nullptr;
		
		// For panning the camera.
		int32 m_W = 0, m_A = 0, m_S = 0, m_D = 0;
		int32 m_Q = 0, m_E = 0;
		bool m_Shift = true;

		// Skydome
		float m_SunAngle = 90.0f;

		// Water
		float m_WaterOffset = 0.0f;
	};
}
