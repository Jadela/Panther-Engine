#pragma once

#include "Transform.h"
#include "Space.h"

namespace Panther
{
	class Camera final
	{
	public:
		Camera();
		Camera(Transform a_Transform);
		~Camera();

		const Transform& GetTransform() const;

		void SetFOV(float a_NewFOV);
		void SetAspectRatio(float a_NewAspectRatio);
		void SetNearZ(float a_NewNearZ);
		void SetFarZ(float a_NewFarZ);

		void Translate(DirectX::XMVECTOR a_Translation, Space a_RelativeTo = Space::Local);
		void Rotate(float a_Roll, float a_Pitch, float a_Yaw);

		DirectX::XMMATRIX GetSkyMatrix();
		DirectX::XMMATRIX GetViewMatrix();
		DirectX::XMMATRIX GetViewProjectionMatrix();
	private:
		Transform m_Transform = Transform();
		float m_Roll = 0;
		float m_Pitch = 0;
		float m_Yaw = 0;

		float m_FOV = DirectX::XMConvertToRadians(60.0f);
		float m_AspectRatio = 1.0f;
		float m_NearZ = 0.1f;
		float m_FarZ = 100.0f;
		DirectX::XMMATRIX m_ProjectionMatrix = DirectX::XMMatrixIdentity();

		void CalculateProjectionMatrix();
	};
}