#pragma once

namespace Panther
{
	template <typename T>
	class Vector2 final
	{
	public:
		Vector2() {}
		Vector2(const Vector2& a_Other) : m_X(a_Other.GetX()), m_Y(a_Other.GetY()) {}
		Vector2(T a_X, T a_Y) : m_X(a_X), m_Y(a_Y) {}
		~Vector2() noexcept {}

		Vector2& operator= (const Vector2& a_Other)
		{
			if (&a_Other != this)
			{
				m_X = a_Other.GetX();
				m_Y = a_Other.GetY();
			}
			return *this;
		}

		Vector2 operator+ (const Vector2& a_Other) const
		{
			return Vector2(m_X + a_Other.GetX(), m_Y + a_Other.GetY());
		}

		Vector2 operator- (const Vector2& a_Other) const
		{
			return Vector2(m_X - a_Other.GetX(), m_Y - a_Other.GetY());
		}

		T GetX() const { return m_X; }
		T GetY() const { return m_Y; }

	private:
		T m_X;
		T m_Y;
	};	
	
	template <typename T>
	class Vector3 final
	{
	public:
		Vector3() {}
		Vector3(const Vector3& a_Other) : m_X(a_Other.GetX()), m_Y(a_Other.GetY()), m_Z(a_Other.GetZ()) {}
		Vector3(T a_X, T a_Y, T a_Z) : m_X(a_X), m_Y(a_Y), m_Z(a_Z) {}
		~Vector3() noexcept {}

		Vector3& operator= (const Vector3& a_Other)
		{
			if (&a_Other != this)
			{
				m_X = a_Other.GetX();
				m_Y = a_Other.GetY();
				m_Z = a_Other.GetZ();
			}
			return *this;
		}

		Vector3 operator+ (const Vector3& a_Other) const
		{
			return Vector3(m_X + a_Other.GetX(), m_Y + a_Other.GetY(), m_Z + a_Other.GetZ());
		}

		Vector3 operator- (const Vector3& a_Other) const
		{
			return Vector3(m_X - a_Other.GetX(), m_Y - a_Other.GetY(), m_Z - a_Other.GetZ());
		}

		T GetX() const { return m_X; }
		T GetY() const { return m_Y; }
		T GetZ() const { return m_Z; }

	private:
		T m_X;
		T m_Y;
		T m_Z;
	};

	template <typename T>
	class Vector4 final
	{
	public:
		Vector4() {}
		Vector4(const Vector4& a_Other) : m_X(a_Other.GetX()), m_Y(a_Other.GetY()), m_Z(a_Other.GetZ()), m_W(a_Other.GetW()) {}
		Vector4(T a_X, T a_Y, T a_Z, T a_W) : m_X(a_X), m_Y(a_Y), m_Z(a_Z), m_W(a_W) {}
		~Vector4() noexcept {}

		Vector4& operator= (const Vector4& a_Other)
		{
			if (&a_Other != this)
			{
				m_X = a_Other.GetX();
				m_Y = a_Other.GetY();
				m_Z = a_Other.GetZ();
				m_W = a_Other.GetW();
			}
			return *this;
		}

		Vector4 operator+ (const Vector4& a_Other) const
		{
			return Vector3(m_X + a_Other.GetX(), m_Y + a_Other.GetY(), m_Z + a_Other.GetZ(), m_W + a_Other.GetW());
		}

		Vector4 operator- (const Vector4& a_Other) const
		{
			return Vector3(m_X - a_Other.GetX(), m_Y - a_Other.GetY(), m_Z - a_Other.GetZ(), m_W - a_Other.GetW());
		}

		T GetX() const { return m_X; }
		T GetY() const { return m_Y; }
		T GetZ() const { return m_Z; }
		T GetW() const { return m_W; }

	private:
		T m_X;
		T m_Y;
		T m_Z;
		T m_W;
	};
}