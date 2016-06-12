#include "Transform.h"

using namespace DirectX;

namespace Panther
{
	Transform::Transform()
		: m_Position(XMVectorZero()), m_Rotation(XMQuaternionIdentity()), m_Scale(XMVectorSet(1, 1, 1, 1))
	{}

	Transform::Transform(XMFLOAT3 a_Position, XMVECTOR a_Rotation, XMFLOAT3 a_Scale)
		: m_Position(XMLoadFloat3(&a_Position)), m_Rotation(a_Rotation), m_Scale(XMLoadFloat3(&a_Scale))
	{}

	Transform::~Transform()
	{}

	const DirectX::XMVECTOR & Transform::GetPosition() const
	{
		return m_Position;
	}

	void Transform::SetPosition(XMVECTOR a_NewPosition)
	{
		m_Position = a_NewPosition;
	}

	void Transform::SetRotation(XMVECTOR a_NewRotation)
	{
		m_Rotation = a_NewRotation;
	}

	void Transform::SetScale(XMVECTOR a_NewScale)
	{
		m_Scale = a_NewScale;
	}

	void Transform::Translate(XMVECTOR a_Translation, Space a_RelativeTo)
	{
		switch (a_RelativeTo)
		{
		case Space::World:
			m_Position += a_Translation;
			break;
		case Space::Local:
			m_Position += XMVector3Rotate(a_Translation, m_Rotation);
			break;
		}
	}

	void Transform::Rotate(XMVECTOR a_Rotation)
	{
		m_Rotation = XMQuaternionMultiply(m_Rotation, a_Rotation);
	}

	void Transform::Scale(XMVECTOR a_Scale)
	{
		m_Scale += a_Scale;
	}

	XMMATRIX Transform::GetTransformMatrix()
	{
		return XMMatrixScalingFromVector(m_Scale) * XMMatrixRotationQuaternion(m_Rotation) * XMMatrixTranslationFromVector(m_Position);
	}
}