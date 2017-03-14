#pragma once
#include "Space.h"

#include <DirectXMath.h>

namespace Panther
{
	class Transform
	{
	public:
		Transform();
		Transform(DirectX::XMFLOAT3 a_Position, DirectX::XMVECTOR a_Rotation = DirectX::XMQuaternionIdentity(), DirectX::XMFLOAT3 a_Scale = DirectX::XMFLOAT3(1, 1, 1));
		Transform(const Transform& a_Rhs) = default;
		~Transform();

		const DirectX::XMVECTOR& GetPosition() const;

		void SetPosition(DirectX::XMVECTOR a_NewPosition);
		void SetRotation(DirectX::XMVECTOR a_NewOrientation);
		void SetScale(DirectX::XMVECTOR a_NewScale);

		void Translate(DirectX::XMVECTOR a_Translation, Space a_RelativeTo = Space::Local);
		void Rotate(DirectX::XMVECTOR a_Rotation);
		void Scale(DirectX::XMVECTOR a_Scale);

		DirectX::XMVECTOR GetPosition() { return m_Position; }
		DirectX::XMVECTOR GetRotation() { return m_Rotation; }
		DirectX::XMVECTOR GetScale() { return m_Scale; }
		DirectX::XMMATRIX GetTransformMatrix();

	private:
		DirectX::XMVECTOR m_Position;
		DirectX::XMVECTOR m_Rotation;
		DirectX::XMVECTOR m_Scale;
	};
}