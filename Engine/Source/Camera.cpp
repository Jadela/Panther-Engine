#include "Camera.h"

using namespace DirectX;

namespace Panther
{
	Camera::Camera()
	{
		CalculateProjectionMatrix();
	}

	Camera::Camera(Transform a_Transform)
		: m_Transform(a_Transform)
	{
		CalculateProjectionMatrix();
	}

	Camera::~Camera()
	{}
	
	const Transform& Camera::GetTransform() const
	{
		return m_Transform;
	}

	void Camera::SetFOV(float a_NewFOV)
	{
		m_FOV = a_NewFOV;
		CalculateProjectionMatrix();
	}

	void Camera::SetAspectRatio(float a_NewAspectRatio)
	{
		m_AspectRatio = a_NewAspectRatio;
		CalculateProjectionMatrix();
	}
	
	void Camera::SetNearZ(float a_NewNearZ)
	{
		m_NearZ = a_NewNearZ;
		CalculateProjectionMatrix();
	}

	void Camera::SetFarZ(float a_NewFarZ)
	{
		m_FarZ = a_NewFarZ;
		CalculateProjectionMatrix();
	}

	void Camera::Translate(Vector a_Translation, Space a_RelativeTo)
	{
		m_Transform.Translate(a_Translation, a_RelativeTo);
	}

	void Camera::Rotate(float a_Roll, float a_Pitch, float a_Yaw)
	{
		m_Roll += a_Roll;
		m_Pitch += a_Pitch;
		m_Yaw += a_Yaw;
		m_Transform.SetRotation(Vector(XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_Pitch), XMConvertToRadians(m_Yaw), XMConvertToRadians(m_Roll))));
	}

	XMMATRIX Camera::GetSkyMatrix()
	{
		XMVECTOR skyPosition = m_Transform.GetPosition().AsXMVECTOR();
		return XMMatrixScaling(-1, 1, 1) * XMMatrixTranslationFromVector(skyPosition);
	}

	XMMATRIX Camera::GetViewMatrix()
	{
		XMVECTOR cameraPosition = (-m_Transform.GetPosition()).AsXMVECTOR();
		XMVECTOR cameraRotation = m_Transform.GetRotation().AsXMVECTOR();
		XMMATRIX rotationMatrix = XMMatrixTranspose(XMMatrixRotationQuaternion(cameraRotation));
		XMMATRIX translationMatrix = XMMatrixTranslationFromVector(cameraPosition);

		XMMATRIX viewMatrix = translationMatrix * rotationMatrix;
		return viewMatrix;
	}

	XMMATRIX Camera::GetViewProjectionMatrix()
	{
		return GetViewMatrix() * m_ProjectionMatrix;
	}

	void Camera::CalculateProjectionMatrix()
	{
		m_ProjectionMatrix = XMMatrixPerspectiveFovLH(m_FOV, m_AspectRatio, m_NearZ, m_FarZ);
	}
}