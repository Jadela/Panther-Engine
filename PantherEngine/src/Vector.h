#pragma once
#include <DirectXMath.h>

namespace Panther
{
	class Vector final
	{
	public:
		Vector();
		explicit Vector(float a_X);
		Vector(float a_X, float a_Y);
		Vector(float a_X, float a_Y, float a_Z);
		Vector(float a_X, float a_Y, float a_Z, float a_W);
		explicit Vector(const float *a_Array);
		explicit Vector(DirectX::XMVECTOR a_Vector);

		float X();
		float Y();
		float Z();
		float W();
		DirectX::XMVECTOR AsXMVECTOR();

		bool operator == (const Vector& a_Rhs) const;
		bool operator != (const Vector& a_Rhs) const;

		Vector& operator= (const Vector& a_Rhs);
		Vector& operator+= (const Vector& a_Rhs);
		Vector& operator-= (const Vector& a_Rhs);
		Vector& operator*= (const Vector& a_Rhs);
		Vector& operator*= (float a_Scalar);
		Vector& operator/= (float a_Scalar);

		Vector operator+ () const { return *this; }
		Vector operator- () const;

		friend Vector operator+ (const Vector& a_Lhs, const Vector& a_Rhs);
		friend Vector operator- (const Vector& a_Lhs, const Vector& a_Rhs);
		friend Vector operator* (const Vector& a_Lhs, const Vector& a_Rhs);
		friend Vector operator* (const Vector& a_Lhs, float a_Rhs);
		friend Vector operator/ (const Vector& a_Lhs, const Vector& a_Rhs);
		friend Vector operator* (float a_Lhs, const Vector& a_Rhs);

	private:
		DirectX::XMFLOAT4 m_Vector;
	};

	Vector operator+ (const Vector& a_Lhs, const Vector& a_Rhs);
	Vector operator- (const Vector& a_Lhs, const Vector& a_Rhs);
	Vector operator* (const Vector& a_Lhs, const Vector& a_Rhs);
	Vector operator* (const Vector& a_Lhs, float a_Rhs);
	Vector operator/ (const Vector& a_Lhs, const Vector& a_Rhs);
	Vector operator* (float a_Lhs, const Vector& a_Rhs);
}