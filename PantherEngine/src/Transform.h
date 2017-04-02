#pragma once
#include "Vector.h"
#include "Space.h"

#include <DirectXMath.h>

namespace Panther
{
	class Transform
	{
	public:
		Transform();
		Transform(Vector a_Position, Vector a_Rotation = Vector(0, 0, 0, 1), Vector a_Scale = Vector(1, 1, 1));
		Transform(const Transform& a_Rhs) = default;
		~Transform();

		const Vector& GetPosition() const;

		void SetPosition(Vector a_NewPosition);
		void SetRotation(Vector a_NewOrientation);
		void SetScale(Vector a_NewScale);

		void Translate(Vector a_Translation, Space a_RelativeTo = Space::Local);
		void Rotate(Vector a_Rotation);
		void Scale(Vector a_Scale);

		Vector GetPosition() { return m_Position; }
		Vector GetRotation() { return m_Rotation; }
		Vector GetScale() { return m_Scale; }
		DirectX::XMMATRIX GetTransformMatrix();

	private:
		Vector m_Position;
		Vector m_Rotation;
		Vector m_Scale;
	};
}