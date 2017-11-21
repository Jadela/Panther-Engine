#pragma once
#include "Component.h"
#include "Core.hpp"

#include <DirectXMath.h>

namespace Panther
{
	class Buffer;
	class CommandList;
	class DescriptorHeap;
	class Material;
	class Mesh;

	class StaticMeshRendererComponent final : public Component
	{
	public:
		struct ObjectCB
		{
			DirectX::XMMATRIX m_MVP;
			DirectX::XMMATRIX m_M;
			DirectX::XMMATRIX m_IT_M;
		};

		StaticMeshRendererComponent(Buffer& a_ObjectCBuffer, DescriptorHeap& a_CBHeap, Mesh* a_Mesh, Material* a_Material);

		void UpdateObjectCB(DirectX::XMMATRIX a_WorldMatrix, DirectX::XMMATRIX a_ViewProjectionMatrix);
		void Record(CommandList& a_CommandList);

	private:
		Buffer& m_ObjectCBuffer;
		DescriptorHeap& m_CBHeap;
		uint32 m_ObjectCBElementSlot;
		uint32 m_ObjectCBHeapSlot;
		Mesh* m_Mesh;
		Material* m_Material;
	};
}