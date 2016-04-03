#include "Scene.h"

namespace Panther
{
	class Buffer;
	class CommandList;
	class DescriptorHeap;
	class Material;
	class Mesh;
	class Sampler;
	class Texture;

	class DemoScene : public Scene
	{
	public:
		DemoScene(Panther::Renderer& a_Renderer);

		void Load() override;
		void Unload() override;
		void Update(float a_DT) override;
		void Render() override;
		void OnResize(Panther::uint32 a_Width, Panther::uint32 a_Height) override;

	private:
		std::unique_ptr<Panther::DescriptorHeap> m_CBVSRVUAVDescriptorHeap = nullptr;
		std::unique_ptr<Panther::DescriptorHeap> m_SamplerDescriptorHeap = nullptr;

		std::unique_ptr<CommandList> m_CubeBundle = nullptr;
		std::unique_ptr<CommandList> m_SphereBundle = nullptr;
		std::unique_ptr<CommandList> m_DuckBundle = nullptr;

		std::unique_ptr<Mesh> m_CubeMesh = nullptr;
		std::unique_ptr<Mesh> m_SphereMesh = nullptr;
		std::unique_ptr<Mesh> m_DuckMesh = nullptr;

		std::unique_ptr<Panther::Buffer> m_CubeMatrixBuffer = nullptr;
		std::unique_ptr<Panther::Buffer> m_SphereMatrixBuffer = nullptr;
		std::unique_ptr<Panther::Buffer> m_DuckMatrixBuffer = nullptr;

		std::unique_ptr<Panther::Material> m_TestMaterial = nullptr;

		std::vector<std::unique_ptr<Panther::Texture>> m_Textures;
		std::unique_ptr<Panther::Sampler> m_Sampler = nullptr;

		DirectX::XMMATRIX m_ProjectionMatrix;
		DirectX::XMMATRIX m_ViewMatrix;
		DirectX::XMMATRIX m_ModelMatrix;

		float m_Angle = 0;
	};
}
