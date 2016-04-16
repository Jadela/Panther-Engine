#include "Scene.h"

namespace Panther
{
	// Panther Renderer
	class Buffer;
	class CommandList;
	class DescriptorHeap;
	class Material;
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

		void Load() final override;
		void Unload() final override;
		void Update(float a_DT) final override;
		void Render() final override;
		void OnResize(uint32 a_Width, uint32 a_Height) final override;
		void OnMouseMove(int32 a_DeltaX, int32 a_DeltaY, bool a_LMBDown) final override;

	private:
		std::unique_ptr<DescriptorHeap> m_CBVSRVUAVDescriptorHeap = nullptr;
		std::unique_ptr<DescriptorHeap> m_SamplerDescriptorHeap = nullptr;

		std::unique_ptr<CommandList> m_CubeBundle = nullptr;
		std::unique_ptr<CommandList> m_SphereBundle = nullptr;
		std::unique_ptr<CommandList> m_DuckBundle = nullptr;

		std::unique_ptr<Mesh> m_CubeMesh = nullptr;
		std::unique_ptr<Mesh> m_SphereMesh = nullptr;
		std::unique_ptr<Mesh> m_DuckMesh = nullptr;

		std::unique_ptr<Buffer> m_CubeMatrixBuffer = nullptr;
		std::unique_ptr<Buffer> m_SphereMatrixBuffer = nullptr;
		std::unique_ptr<Buffer> m_DuckMatrixBuffer = nullptr;

		std::unique_ptr<Material> m_TestMaterial = nullptr;

		std::vector<std::unique_ptr<Texture>> m_Textures;
		std::unique_ptr<Sampler> m_Sampler = nullptr;

		std::unique_ptr<Camera> m_Camera = nullptr;

		std::unique_ptr<Transform> m_CubeTransform = nullptr;
		std::unique_ptr<Transform> m_SphereTransform = nullptr;
		std::unique_ptr<Transform> m_DuckTransform = nullptr;
	};
}
