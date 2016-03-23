#pragma once

#include "../../Panther_Core/src/Core.h"

namespace Panther
{
	class Buffer;
	class CommandList;
	class Renderer;

	class Mesh
	{
	public:
		typedef ushort16 Index;
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT2 uv;
		};

		explicit Mesh(Renderer& a_Renderer);
		virtual ~Mesh();

		void InitAsCube(CommandList& a_CommandList, DirectX::XMFLOAT3 a_Scale = DirectX::XMFLOAT3(1,1,1));
		void InitAsSphere(CommandList& a_CommandList, float a_Radius = 0.5f, uint32 a_Slices = 32, uint32 a_Stacks = 32);

		uint32 GetNumIndices();

	protected:
		virtual void Initialize(CommandList& a_CommandList) = 0;

		Renderer& m_Renderer;

		std::vector<Vertex> m_Vertices;
		std::vector<Index> m_Indices;		

		std::unique_ptr<Buffer> m_VertexBuffer = nullptr;
		std::unique_ptr<Buffer> m_IndexBuffer = nullptr;

	private:
		Mesh() = delete;
	};
}