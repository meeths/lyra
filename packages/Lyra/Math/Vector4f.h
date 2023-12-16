#pragma once
#include <Math/Functions.h>
#include <Math/Vector3f.h>
#include <Core/CoreDefines.h>

namespace lyra
{
namespace Math
{
	class Vector4f
	{
	public:

		static const Vector4f Zero;
		static const Vector4f UnitX;
		static const Vector4f UnitY;
		static const Vector4f UnitZ;
		static const Vector4f UnitW;
		static const Vector4f One;

		Vector4f() = default;
		Vector4f(float x, float y, float z, float w) : x{ x }, y{ y }, z{ z }, w{ w } {}
		Vector4f(const Vector4f& v) : x{ v.x }, y{ v.y }, z{ v.z }, w{ v.w } {}
		explicit Vector4f(const Vector3f& v) : x{ v.x }, y{ v.y }, z{ v.z }, w{ 1.0f } {}
		explicit Vector4f(const float f) : x{ f }, y{ f }, z{ f }, w{ f } {}
		DeclareDefaultMoveable(Vector4f);
		~Vector4f() = default;

        Vector4f& operator = (const Vector4f& v) { x = v.x; y = v.y; z = v.z; w = v.w; return *this; };
        Vector4f& operator = (float f) { x = f; y = f; z = f; w = f; return *this; };

		void Set(float x, float y, float z, float w) { *this = { x, y, z , w }; };
		void Set(float f) { *this = f; };
		void Set(const Vector4f& pt) { *this = pt; };

		Vector4f operator +(float f) const;
		Vector4f operator -(float f) const;
		Vector4f operator *(const Vector4f& v) const;
		Vector4f operator *(float f) const;
		Vector4f operator /(const Vector4f& v) const;
		Vector4f operator /(float f) const;

		Vector4f& operator +=(const Vector4f& v);
		Vector4f& operator -=(const Vector4f& v);
		Vector4f& operator +=(float f);
		Vector4f& operator -=(float f);
		Vector4f& operator *=(float s);
		Vector4f& operator /=(float s);

        float& operator[](int i) { return v[i]; };
        const float& operator[](int i) const { return v[i]; };

		Vector4f operator -() const;

		friend bool operator ==(const Vector4f &a, const Vector4f &b);
		friend bool operator !=(const Vector4f &a, const Vector4f &b);
		friend Vector4f operator +(const Vector4f &a, const Vector4f &b);
		friend Vector4f operator -(const Vector4f &a, const Vector4f &b);
		friend Vector4f operator *(float f, const Vector4f &v);

		friend Vector4f operator * (float f, const Vector4f& v)
		{
			return{ f * v.x, f * v.y, f * v.z, f * v.w };
		};

		float SquaredLength() const;
		float Length() const;
		float SquaredDistance(const Vector4f& v) const;
		float Distance(const Vector4f& v) const;
		float AbsoluteDot(const Vector4f& v) const;
		float Dot(const Vector4f& v) const;
		Vector4f Cross(const Vector4f& v) const;
		float Normalize();
		Vector4f NormalizedCopy() const;
		Vector4f Reflect(const Vector4f& normal) const;
		Vector4f Bounce(const Vector4f& normal) const;
		void MakeFloor(const Vector4f& reference);
		void MakeCeil(const Vector4f& reference);


		union
		{
			struct { float x, y, z, w; };
			float v[4]{ 0 };
		};
	};

	//----------------------------------------------------------------------------------------------------------------------
	inline bool operator==(const Vector4f &a, const Vector4f &b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline bool operator!=(const Vector4f &a, const Vector4f &b)
	{
		return !(a == b);
	}
	inline Vector4f operator+(const Vector4f & a, const Vector4f & b)
	{
		return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
	}
	inline Vector4f operator-(const Vector4f & a, const Vector4f & b)
	{
		return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f Vector4f::operator+(float f) const
	{
		return{ x + f, y + f, z + f, w + f };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f Vector4f::operator-(float f) const
	{
		return{ x - f, y - f, z - f, w - f };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f Vector4f::operator*(const Vector4f& v) const
	{
		return{ x * v.x, y * v.y, z * v.z, w * v.w };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f Vector4f::operator*(float f) const
	{
		return{ x * f, y * f, z * f, w * f };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f Vector4f::operator/(const Vector4f& v) const
	{
		return{ x / v.x, y / v.y, z / v.z, w / v.w };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f Vector4f::operator/(float f) const
	{
		const auto inv = 1.0f / f;
		return{ x * inv, y * inv, z * inv, w * inv };
	}

	inline Vector4f& Vector4f::operator+=(const Vector4f& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.z;
		return *this;
	} //----------------------------------------------------------------------------------------------------------------------
	inline Vector4f& Vector4f::operator+=(float f)
	{
		x += f;
		y += f;
		z += f;
		w += f;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f& Vector4f::operator-=(const Vector4f& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f& Vector4f::operator-=(float f)
	{
		x -= f;
		y -= f;
		z -= f;
		w -= f;
		return *this;
	}
	inline Vector4f & Vector4f::operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
	inline Vector4f & Vector4f::operator/=(float s)
	{
		x /= s;
		y /= s;
		z /= s;
		w /= s;
		return *this;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline Vector4f Vector4f::operator-() const
	{
		return{ -x, -y, -z, -w };
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector4f::SquaredLength() const
	{
		return Math::Pow2(x) + Math::Pow2(y) + Math::Pow2(z) + Math::Pow2(w);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector4f::Length() const
	{
		return Math::Sqrt(Math::Pow2(x) + Math::Pow2(y) + Math::Pow2(z) + Math::Pow2(w));
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector4f::SquaredDistance(const Vector4f& v) const
	{
		return Math::Pow2(x - v.x) + Math::Pow2(y - v.y) + Math::Pow2(z - v.z) + Math::Pow2(w - v.w);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector4f::Distance(const Vector4f& v) const
	{
		return Math::Sqrt(Math::Pow2(x - v.x) + Math::Pow2(y - v.y) + Math::Pow2(z - v.z) + Math::Pow2(w - v.w));
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector4f::AbsoluteDot(const Vector4f& v) const
	{
		return Math::Abs(x * v.x) + Math::Abs(y * v.y) + Math::Abs(z * v.z) + Math::Abs(w * v.w);
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline float Vector4f::Dot(const Vector4f& v) const
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}
	//----------------------------------------------------------------------------------------------------------------------
	inline  Vector4f Vector4f::Cross(const Vector4f& v) const
	{
		Vector4f Result;
		Result.x = (y * v.z - z * v.y);
		Result.y = (z * v.x - x * v.z);
		Result.z = (x * v.y - y * v.x);
		Result.w = 0;
		return Result;
	}
	inline float Vector4f::Normalize()
	{
        const auto length = Length();
        const auto invLength = 1.0f / length;
		*this *= invLength;
		return length;
	}
}
}
