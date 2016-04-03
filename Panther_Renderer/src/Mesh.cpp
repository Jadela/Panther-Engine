#include "DX12RendererPCH.h"
#include "Mesh.h"

#include "Buffer.h"
#include "CommandList.h"
#include "Renderer.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

using namespace DirectX;

namespace Panther
{
	Mesh::Mesh(Renderer& a_Renderer)
		: m_Renderer(a_Renderer)
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::InitAsCube(CommandList& a_CommandList, XMFLOAT3 a_Scale)
	{
		XMFLOAT3 p0 = XMFLOAT3(-1.0f * a_Scale.x, -1.0f * a_Scale.y, -1.0f * a_Scale.z);
		XMFLOAT3 p1 = XMFLOAT3(-1.0f * a_Scale.x, 1.0f * a_Scale.y, -1.0f * a_Scale.z);
		XMFLOAT3 p2 = XMFLOAT3(1.0f * a_Scale.x, 1.0f * a_Scale.y, -1.0f * a_Scale.z);
		XMFLOAT3 p3 = XMFLOAT3(1.0f * a_Scale.x, -1.0f * a_Scale.y, -1.0f * a_Scale.z);
		XMFLOAT3 p4 = XMFLOAT3(-1.0f * a_Scale.x, -1.0f * a_Scale.y, 1.0f * a_Scale.z);
		XMFLOAT3 p5 = XMFLOAT3(-1.0f * a_Scale.x, 1.0f * a_Scale.y, 1.0f * a_Scale.z);
		XMFLOAT3 p6 = XMFLOAT3(1.0f * a_Scale.x, 1.0f * a_Scale.y, 1.0f * a_Scale.z);
		XMFLOAT3 p7 = XMFLOAT3(1.0f * a_Scale.x, -1.0f * a_Scale.y, 1.0f * a_Scale.z);
		m_Vertices =
		{
			{ p0,	XMFLOAT3(-1.0f, 0.0f, 0.0f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ p0,	XMFLOAT3(0.0f, -1.0f, 0.0f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ p0,	XMFLOAT3(0.0f, 0.0f, -1.0f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

			{ p1,	XMFLOAT3(-1.0f, 0.0f, 0.0f), 	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ p1,	XMFLOAT3(0.0f, 1.0f, 0.0f), 	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ p1,	XMFLOAT3(0.0f, 0.0f, -1.0f), 	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

			{ p2,	XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ p2,	XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ p2,	XMFLOAT3(0.0f, 0.0f, -1.0f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

			{ p3,	XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ p3,	XMFLOAT3(0.0f, -1.0f, 0.0f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ p3,	XMFLOAT3(0.0f, 0.0f, -1.0f),	XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

			{ p4,	XMFLOAT3(-1.0f, 0.0f, 0.0f),	XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
			{ p4,	XMFLOAT3(0.0f, -1.0f, 0.0f),	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ p4,	XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

			{ p5,	XMFLOAT3(-1.0f, 0.0f, 0.0f),	XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ p5,	XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
			{ p5,	XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

			{ p6,	XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ p6,	XMFLOAT3(0.0f, 1.0f, 0.0f),		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ p6,	XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

			{ p7,	XMFLOAT3(1.0f, 0.0f, 0.0f),		XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
			{ p7,	XMFLOAT3(0.0f, -1.0f, 0.0f),	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
			{ p7,	XMFLOAT3(0.0f, 0.0f, 1.0f),		XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }
		};

		m_Indices =
		{
			2, 5, 8, 8, 11, 2,
			23, 20, 17, 17, 14, 23,
			12, 15, 3, 3, 0, 12,
			9, 6, 18, 18, 21, 9,
			4, 16, 19, 19, 7, 4,
			13, 1, 10, 10, 22, 13
		};

		Initialize(a_CommandList);
	}

	void Mesh::InitAsSphere(CommandList & a_CommandList, float a_Radius, uint32 a_Slices, uint32 a_Stacks)
	{
		const float pi = 3.1415926535897932384626433832795f;
		const float _2pi = 2.0f * pi;

		for (uint32 i = 0; i <= a_Stacks; ++i)
		{
			// V texture coordinate.
			float V = i / (float)a_Stacks;
			float phi = V * pi;

			for (uint32 j = 0; j <= a_Slices; ++j)
			{
				// U texture coordinate.
				float U = j / (float)a_Stacks;
				float theta = U * _2pi;

				float X = cos(theta) * sin(phi);
				float Y = cos(phi);
				float Z = sin(theta) * sin(phi);

				m_Vertices.push_back({ XMFLOAT3(X * a_Radius, Y * a_Radius, Z * a_Radius), XMFLOAT3(X, Y, Z), XMFLOAT4(U, V, 0.0f, 1.0f), XMFLOAT2(U, V) });
			}
		}

		// Now generate the index buffer
		for (uint32 i = 0; i < a_Slices * a_Stacks + a_Slices; ++i)
		{
			m_Indices.push_back(i);
			m_Indices.push_back(i + a_Slices + 1);
			m_Indices.push_back(i + a_Slices);

			m_Indices.push_back(i + a_Slices + 1);
			m_Indices.push_back(i);
			m_Indices.push_back(i + 1);
		}

		Initialize(a_CommandList);
	}

	void Mesh::InitViaASSIMP(CommandList & a_CommandList, std::string a_FileName)
	{
		Assimp::Importer importer;

		const aiScene* scene(importer.ReadFile(a_FileName, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded));
		if (!scene) throw std::runtime_error("Panther ASSIMP Error: Import failed, ASSIMP error string: " + std::string(importer.GetErrorString()));

		aiMesh* mesh = scene->mMeshes[0];
		if (!mesh) throw std::runtime_error("Panther ASSIMP Error: Imported model contains no meshes!");

		m_Vertices.resize(mesh->mNumVertices);
		m_Indices.resize(mesh->mNumFaces * mesh->mFaces->mNumIndices);

		for (uint32 i = 0; i < mesh->mNumVertices; ++i)
		{
			m_Vertices[i] = { 
				XMFLOAT3(mesh->mVertices[i].x * 0.01f, mesh->mVertices[i].y * 0.01f, mesh->mVertices[i].z * 0.01f),
				XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z),
				XMFLOAT4(1,1,1,1), 
				XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) 
			};
		}

		for (uint32 i = 0; i < mesh->mNumFaces; ++i)
		{
			m_Indices[i * 3 + 0] = mesh->mFaces[i].mIndices[0];
			m_Indices[i * 3 + 1] = mesh->mFaces[i].mIndices[1];
			m_Indices[i * 3 + 2] = mesh->mFaces[i].mIndices[2];
		}

		Initialize(a_CommandList);
	}

	uint32 Mesh::GetNumIndices()
	{
		return static_cast<uint32>(m_Indices.size());
	}
}
