#include "Vector.h"

using namespace DirectX;

namespace Panther
{
	Vector::Vector() : m_Vector(0.0f, 0.0f, 0.0f, 0.0f)
	{
	}

	Vector::Vector(float a_X) : m_Vector(a_X, a_X, a_X, a_X)
	{
	}

	Vector::Vector(float a_X, float a_Y) : m_Vector(a_X, a_Y, 0.0f, 0.0f)
	{
	}

	Vector::Vector(float a_X, float a_Y, float a_Z) : m_Vector(a_X, a_Y, a_Z, 0.0f)
	{
	}

	Vector::Vector(float a_X, float a_Y, float a_Z, float a_W) : m_Vector(a_X, a_Y, a_Z, a_W)
	{
	}

	Vector::Vector(const float *a_Array) : m_Vector(a_Array)
	{
	}

	Vector::Vector(XMVECTOR a_Vector)
	{
		XMStoreFloat4(&m_Vector, a_Vector);
	}

	float Vector::X()
	{
		return m_Vector.x;
	}

	float Vector::Y()
	{
		return m_Vector.y;
	}

	float Vector::Z()
	{
		return m_Vector.z;
	}

	float Vector::W()
	{
		return m_Vector.w;
	}

	XMVECTOR Vector::AsXMVECTOR()
	{
		return XMLoadFloat4(&m_Vector);
	}

	bool Vector::operator==(const Vector& a_Rhs) const
	{
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		return XMVector4Equal(v1, v2);
	}

	bool Vector::operator!=(const Vector& a_Rhs) const
	{
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		return XMVector4NotEqual(v1, v2);
	}

	Vector& Vector::operator=(const Vector& a_Rhs)
	{
		m_Vector.x = a_Rhs.m_Vector.x;
		m_Vector.y = a_Rhs.m_Vector.y; 
		m_Vector.z = a_Rhs.m_Vector.z; 
		m_Vector.w = a_Rhs.m_Vector.w; 
		return *this;
	}

	Vector& Vector::operator+=(const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR X = XMVectorAdd(v1, v2);
		XMStoreFloat4(&m_Vector, X);
		return *this;
	}

	Vector& Vector::operator-=(const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR X = XMVectorSubtract(v1, v2);
		XMStoreFloat4(&m_Vector, X);
		return *this;
	}

	Vector& Vector::operator*=(const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR X = XMVectorMultiply(v1, v2);
		XMStoreFloat4(&m_Vector, X);
		return *this;
	}

	Vector& Vector::operator*=(float a_Scalar)
	{
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR X = XMVectorScale(v1, a_Scalar);
		XMStoreFloat4(&m_Vector, X);
		return *this;
	}

	Vector& Vector::operator/=(float a_Scalar)
	{
		assert(a_Scalar != 0.0f);
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR X = XMVectorScale(v1, 1.f / a_Scalar);
		XMStoreFloat4(&m_Vector, X);
		return *this;
	}

	Vector Vector::operator-() const
	{
		XMVECTOR v1 = XMLoadFloat4(&m_Vector);
		XMVECTOR result = XMVectorNegate(v1);
		return Vector(result);
	}

	Vector operator+ (const Vector& a_Lhs, const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&a_Lhs.m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR result = XMVectorAdd(v1, v2);
		return Vector(result);
	}

	Vector operator- (const Vector& a_Lhs, const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&a_Lhs.m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR result = XMVectorSubtract(v1, v2);
		return Vector(result);
	}

	Vector operator* (const Vector& a_Lhs, const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&a_Lhs.m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR result = XMVectorMultiply(v1, v2);
		return Vector(result);
	}

	Vector operator* (const Vector& a_Lhs, float a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&a_Lhs.m_Vector);
		XMVECTOR result = XMVectorScale(v1, a_Rhs);
		return Vector(result);
	}

	Vector operator/ (const Vector& a_Lhs, const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&a_Lhs.m_Vector);
		XMVECTOR v2 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR result = XMVectorDivide(v1, v2);
		return Vector(result);
	}

	Vector operator* (float a_Lhs, const Vector& a_Rhs)
	{
		XMVECTOR v1 = XMLoadFloat4(&a_Rhs.m_Vector);
		XMVECTOR result = XMVectorScale(v1, a_Lhs);
		return Vector(result);
	}
}