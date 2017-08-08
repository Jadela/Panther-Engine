#include "StaticMeshRendererComponent.h"

#include "Buffer.h"
#include "CommandList.h"
#include "DescriptorHeap.h"
#include "Material.h"
#include "Mesh.h"

using namespace DirectX;

namespace Panther
{
	StaticMeshRendererComponent::StaticMeshRendererComponent(Buffer& a_ObjectCBuffer, DescriptorHeap& a_CBHeap, Mesh* a_Mesh, Material* a_Material) :
		m_ObjectCBuffer(a_ObjectCBuffer),
		m_CBHeap(a_CBHeap),
		m_Mesh(a_Mesh),
		m_Material(a_Material)
	{
		m_ObjectCBElementSlot = m_ObjectCBuffer.GetSlot();
		m_ObjectCBHeapSlot = m_CBHeap.RegisterConstantBuffer(m_ObjectCBuffer, m_ObjectCBElementSlot);
	}

	void StaticMeshRendererComponent::UpdateObjectCB(DirectX::XMMATRIX a_WorldMatrix, DirectX::XMMATRIX a_ViewProjectionMatrix)
	{
		ObjectCB objectCB;
		objectCB.m_MVP = a_WorldMatrix * a_ViewProjectionMatrix;
		objectCB.m_M = a_WorldMatrix;
		objectCB.m_IT_M = XMMatrixTranspose(XMMatrixInverse(nullptr, a_WorldMatrix));
		m_ObjectCBuffer.CopyTo(m_ObjectCBElementSlot, &objectCB, sizeof(ObjectCB));
	}

	void StaticMeshRendererComponent::Record(CommandList& a_CommandList)
	{
		m_Material->SetResource("ObjectCB", m_CBHeap, m_ObjectCBHeapSlot);
		m_Material->Use(a_CommandList);
		a_CommandList.SetMesh(*m_Mesh);
		a_CommandList.Draw(m_Mesh->GetNumIndices());
	}
}