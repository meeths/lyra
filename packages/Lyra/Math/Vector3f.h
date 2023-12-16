#pragma once
#include <Math/Vector2f.h>
#include <Math/Functions.h>
#include <Core/CoreDefines.h>

namespace lyra
{
namespace Math
{
	class Vector3f
	{
	public:
		static const Vector3f Zero;
		static const Vector3f UnitX;
		static const Vector3f UnitY;
		static const Vector3f UnitZ;
		static const Vector3f One;
		DeclareDefaultMoveable(Vector3f);
		~Vector3f() = default;

		Vector3f() = default;
		Vector3f(float x, float y, float z) : x{ x }, y{ y }, z{ z } {};
		Vector3f(const Vector3f& v) : x{ v.x }, y{ v.y }, z{ v.z } {};
		explicit Vector3f(float f) : x{ f }, y{ f }, z{ f } {};

		Vector3f& operator = (const Vector3f& v) { x = v.x; y = v.y; z = v.z; return *this; };
		Vector3f& operator = (float f) { x = f; y = f; z = f; return *this; };

		void Set(float x, float y, float z){ *this = {x, y, z}; };
		void Set(float f){ *this = f; };
		void Set(const Vector3f& pt) { *this = pt; };

		Vector3f operator +(float f) const;
		Vector3f operator -(float f) const;
		Vector3f operator *(const Vector3f& v) const;
		Vector3f operator *(float f) const;
		Vector3f operator /(const Vector3f& v) const;
		Vector3f operator /(float f) const;

		Vector3f& operator +=(const Vector3f& v);
		Vector3f& operator -=(const Vector3f& v);
		Vector3f& operator +=(float f);
		Vector3f& operator -=(float f);
		Vector3f& operator *=(float s);
		Vector3f& operator /=(float s);

        float& operator[](int i) { return v[i]; };
        const float& operator[](int i) const { return v[i]; };

		Vector3f operator -() const;

		friend bool operator ==(const Vector3f &a, const Vector3f &b);
		friend bool operator !=(const Vector3f &a, const Vector3f &b);
		friend Vector3f operator +(const Vector3f &a, const Vector3f &b);
		friend Vector3f operator -(const Vector3f &a, const Vector3f &b);
		friend Vector3f operator *(float f, const Vector3f &v);

		friend Vector3f operator * (float f, const Vector3f& v)
		{
			return{ f * v.x, f * v.y, f * v.z };
		};

		float SquaredLength() const;
		float Length() const;
		float SquaredDistance(const Vector3f& v) const;
		float Distance(const Vector3f& v) const;
		float AbsoluteDot(const Vector3f& v) const;
		float Dot(const Vector3f& v) const;
		Vector3f Cross(const Vector3f& v) const;
		float Normalize();
		Vector3f NormalizedCopy() const;
		Vector3f Reflect(const Vector3f& normal) const;
		Vector3f Bounce(const Vector3f& normal) const;
		void MakeFloor(const Vector3f& reference);
		void MakeCeil(const Vector3f& reference);

        // transform vector by matrix
		union
		{
			struct
			{
				float x, y, z;
			};
			float v[3]{0};
		};

	};
	//----------------------------------------------------------------------------------------------------------------------
	inline bool operator==(const Vector3f &a, const Vector3f &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline bool operator!=(const Vector3f &a, const Vector3f &b)
	{
		return !(a == b);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::operator+(float f) const
	{
		return{ x + f, y + f, z + f };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::operator-(float f) const
	{
		return{ x - f, y - f, z - f };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::operator*(const Vector3f& v) const
	{
		return{ x * v.x, y * v.y, z * v.z };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::operator*(float f) const
	{
		return{ x * f, y * f, z * f };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::operator/(const Vector3f& v) const
	{
		return{ x / v.x, y / v.y, z / v.z };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::operator/(float f) const
	{
		const auto inv = 1.0f / f;
		return{ x * inv, y * inv, z * inv };
	}

	inline Vector3f& Vector3f::operator+=(const Vector3f& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	} //----------------------------------------------------------------------------------------------------------------------
	inline Vector3f& Vector3f::operator+=(float f)
	{
		x += f;
		y += f;
		z += f;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f& Vector3f::operator-=(const Vector3f& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f& Vector3f::operator-=(float f)
	{
		x -= f;
		y -= f;
		z -= f;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::operator-() const
	{
		return{ -x, -y, -z };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector3f::SquaredLength() const
	{
		return Math::Pow2(x) + Math::Pow2(y) + Math::Pow2(z);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector3f::Length() const
	{
		return Math::Sqrt(Math::Pow2(x) + Math::Pow2(y) + Math::Pow2(z));
	} 
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector3f::SquaredDistance(const Vector3f& v) const
	{
		return Math::Pow2(x - v.x) + Math::Pow2(y - v.y) + Math::Pow2(z - v.z);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector3f::Distance(const Vector3f& v) const
	{
		return Math::Sqrt(Math::Pow2(x - v.x) + Math::Pow2(y - v.y) + Math::Pow2(z - v.z));
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector3f::AbsoluteDot(const Vector3f& v) const
	{
		return Math::Abs(x * v.x) + Math::Abs(y * v.y) + Math::Abs(z * v.z);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector3f::Dot(const Vector3f& v) const
	{
		return x * v.x + y * v.y + z * v.z;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::Cross(const Vector3f& v) const
	{
		return{	y * v.z - z * v.y,
				z * v.x - x * v.z,
				x * v.y - y * v.x };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector3f::Normalize()
	{
		const auto len = Length();
		if (len > 0)
		{
			auto invLen = 1.0f / len;
			x *= invLen;
			y *= invLen;
			z *= invLen;
		}
		return len;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::NormalizedCopy() const
	{
		auto normalized = *this;
		normalized.Normalize();
		return normalized;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::Reflect(const Vector3f& normal) const
	{
		return *this - 2.0f * Dot(normal) * normal;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f Vector3f::Bounce(const Vector3f& normal) const
	{
		return 2.0f * Dot(normal) * normal - *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline void Vector3f::MakeFloor(const Vector3f& reference)
	{
		if (reference.x < x) x = reference.x;
		if (reference.y < y) y = reference.y;
		if (reference.z < z) z = reference.z;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline void Vector3f::MakeCeil(const Vector3f& reference)
	{
		if (reference.x > x) x = reference.x;
		if (reference.y > y) y = reference.y;
		if (reference.z > z) z = reference.z;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f &Vector3f::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f &Vector3f::operator/=(float s)
	{
		x /= s;
		y /= s;
		z /= s;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f operator+(const Vector3f &a, const Vector3f &b)
	{
		Vector3f res(a.x + b.x, a.y + b.y, a.z + b.z);
		return res;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector3f operator-(const Vector3f &a, const Vector3f &b)
	{
		Vector3f res(a.x - b.x, a.y - b.y, a.z - b.z);
		return res;
	}
}}
