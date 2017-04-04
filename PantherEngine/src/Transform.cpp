#include "Transform.h"

using namespace DirectX;

namespace Panther
{
	Transform::Transform()
		: m_Position(Vector()), m_Rotation(Vector(0, 0, 0, 1)), m_Scale(Vector(1, 1, 1))
	{}

	Transform::Transform(Vector a_Position, Vector a_Rotation, Vector a_Scale)
		: m_Position(a_Position), m_Rotation(a_Rotation), m_Scale(a_Scale)
	{}

	Transform::~Transform()
	{}

	const Vector& Transform::GetPosition() const
	{
		return m_Position;
	}

	void Transform::SetPosition(Vector a_NewPosition)
	{
		m_Position = a_NewPosition;
	}

	void Transform::SetRotation(Vector a_NewRotation)
	{
		m_Rotation = a_NewRotation;
	}

	void Transform::SetScale(Vector a_NewScale)
	{
		m_Scale = a_NewScale;
	}

	void Transform::Translate(Vector a_Translation, Space a_RelativeTo)
	{
		switch (a_RelativeTo)
		{
		case Space::World:
			m_Position += a_Translation;
			break;
		case Space::Local:
			XMVECTOR translationVector(XMVectorSet(a_Translation.X(), a_Translation.Y(), a_Translation.Z(), a_Translation.W()));
			XMVECTOR rotationVector(XMVectorSet(m_Rotation.X(), m_Rotation.Y(), m_Rotation.Z(), m_Rotation.W()));
			m_Position += Vector(XMVector3Rotate(translationVector, rotationVector));
			break;
		}
	}

	void Transform::Rotate(Vector a_Rotation)
	{
		XMVECTOR rotationVector1(XMVectorSet(m_Rotation.X(), m_Rotation.Y(), m_Rotation.Z(), m_Rotation.W()));
		XMVECTOR rotationVector2(XMVectorSet(a_Rotation.X(), a_Rotation.Y(), a_Rotation.Z(), a_Rotation.W()));
		m_Rotation = Vector(XMQuaternionMultiply(rotationVector1, rotationVector2));
	}

	void Transform::Scale(Vector a_Scale)
	{
		m_Scale += a_Scale;
	}

	XMMATRIX Transform::GetTransformMatrix()
	{
		XMVECTOR scaleVector(XMVectorSet(m_Scale.X(), m_Scale.Y(), m_Scale.Z(), m_Scale.W()));
		XMVECTOR rotationVector(XMVectorSet(m_Rotation.X(), m_Rotation.Y(), m_Rotation.Z(), m_Rotation.W()));
		XMVECTOR positionVector(XMVectorSet(m_Position.X(), m_Position.Y(), m_Position.Z(), m_Position.W()));
		return XMMatrixScalingFromVector(scaleVector) * XMMatrixRotationQuaternion(rotationVector) * XMMatrixTranslationFromVector(positionVector);
	}
}