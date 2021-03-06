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
			XMVECTOR translationVector(a_Translation.AsXMVECTOR());
			XMVECTOR rotationVector(m_Rotation.AsXMVECTOR());
			m_Position += Vector(XMVector3Rotate(translationVector, rotationVector));
			break;
		}
	}

	void Transform::Rotate(Vector a_Rotation)
	{
		XMVECTOR rotationVector1(m_Rotation.AsXMVECTOR());
		XMVECTOR rotationVector2(a_Rotation.AsXMVECTOR());
		m_Rotation = Vector(XMQuaternionMultiply(rotationVector1, rotationVector2));
	}

	void Transform::Scale(Vector a_Scale)
	{
		m_Scale += a_Scale;
	}

	XMMATRIX Transform::GetTransformMatrix()
	{
		XMVECTOR scaleVector(m_Scale.AsXMVECTOR());
		XMVECTOR rotationVector(m_Rotation.AsXMVECTOR());
		XMVECTOR positionVector(m_Position.AsXMVECTOR());
		return XMMatrixScalingFromVector(scaleVector) * XMMatrixRotationQuaternion(rotationVector) * XMMatrixTranslationFromVector(positionVector);
	}
}