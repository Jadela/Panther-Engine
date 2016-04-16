#pragma once

#include <DirectXMath.h>

namespace Panther
{
	class Transform
	{
	public:
		Transform();
		Transform(DirectX::XMFLOAT3 a_Position, DirectX::XMVECTOR a_Rotation = DirectX::XMQuaternionIdentity(), DirectX::XMFLOAT3 a_Scale = DirectX::XMFLOAT3(1, 1, 1));
		~Transform();

		void SetPosition(DirectX::XMVECTOR a_NewPosition);
		void SetRotation(DirectX::XMVECTOR a_NewOrientation);
		void SetScale(DirectX::XMVECTOR a_NewScale);

		void AddTranslation(DirectX::XMVECTOR a_Translation);
		void AddRotation(DirectX::XMVECTOR a_Rotation);
		void AddScale(DirectX::XMVECTOR a_Scale);

		DirectX::XMMATRIX GetTransformMatrix();

	protected:

	private:
		friend class Camera;

		DirectX::XMVECTOR m_Position;
		DirectX::XMVECTOR m_Rotation;
		DirectX::XMVECTOR m_Scale;
	};
}