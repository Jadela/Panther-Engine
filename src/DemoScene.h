#include "Scene.h"

namespace Panther
{
	class Buffer;
	class CommandList;
	class DX12DescriptorHeap;
	class Material;
	class Mesh;
	class Texture;

	class DemoScene : public Scene
	{
	public:
		DemoScene(Panther::Renderer& renderer);

		bool Load() override;
		void Unload() override;
		void Update(float a_DT) override;
		bool Render() override;
		void OnResize(Panther::uint32 a_Width, Panther::uint32 a_Height) override;

	private:
		std::unique_ptr<Panther::DX12DescriptorHeap> m_CBVSRVUAVDescriptorHeap = nullptr;
		std::unique_ptr<Panther::DX12DescriptorHeap> m_SamplerDescriptorHeap = nullptr;

		std::unique_ptr<CommandList> m_CubeBundle = nullptr;
		std::unique_ptr<CommandList> m_SphereBundle = nullptr;

		std::unique_ptr<Mesh> m_CubeMesh = nullptr;
		std::unique_ptr<Mesh> m_SphereMesh = nullptr;

		std::unique_ptr<Panther::Buffer> m_ConstantBuffer1 = nullptr;
		std::unique_ptr<Panther::Buffer> m_ConstantBuffer2 = nullptr;

		std::unique_ptr<Panther::Material> m_TestMaterial = nullptr;

		// Textures
		std::vector<std::unique_ptr<Panther::Texture>> m_Textures;

		DirectX::XMMATRIX m_ProjectionMatrix;
		DirectX::XMMATRIX m_ViewMatrix;
		DirectX::XMMATRIX m_ModelMatrix;

		float m_Angle = 0;
	};
}
