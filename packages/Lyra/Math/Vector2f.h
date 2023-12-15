#pragma once

#include <Math/Functions.h>

namespace lyra
{
namespace Math
{
class Vector2f
{
public:

	static const Vector2f Zero;
	static const Vector2f UnitX;
	static const Vector2f UnitY;
	static const Vector2f One;

    Vector2f() = default;
    Vector2f(float x, float y) : x{ x }, y{ y } {};
    Vector2f(const Vector2f& v) : x{ v.x }, y{ v.y } {};
    explicit Vector2f(float f) : x{ f }, y{ f } {};

    Vector2f& operator = (const Vector2f& v) { x = v.x; y = v.y; return *this; }
    Vector2f& operator = (float f) { x = f; y = f; return *this; };

	Vector2f operator *(const Vector2f& f) const;
	Vector2f operator *(float f) const;
	friend Vector2f operator *(float f, const Vector2f &v);

    friend bool operator ==(const Vector2f &a, const Vector2f &b);
    friend bool operator !=(const Vector2f &a, const Vector2f &b);


	friend Vector2f operator * (float f, const Vector2f& v)
	{
		return{ f * v.x, f * v.y};
	};


    void Set(float x, float y) { *this = { x, y }; };
    void Set(float f) { *this = f; };
    void Set(const Vector2f& pt) { *this = pt; };

    float& operator[](int i) { return v[i]; };
    const float& operator[](int i) const { return v[i]; };

    friend Vector2f operator +(const Vector2f &a, const Vector2f &b);
    friend Vector2f operator -(const Vector2f &a, const Vector2f &b);

	float Length() const;

    union
    {
        struct
        {
            float x, y;
        };
        float v[2]{ 0 };
    };

};

inline bool operator==(const Vector2f& a, const Vector2f& b)
{
    return a.x == b.x && a.y == b.y;
}

inline bool operator!=(const Vector2f& a, const Vector2f& b)
{
    return !(a == b);
}

inline Vector2f operator+(const Vector2f &a, const Vector2f &b)
{
    Vector2f res(a.x + b.x, a.y + b.y);
    return res;
}

inline Vector2f operator-(const Vector2f &a, const Vector2f &b)
{
    Vector2f res(a.x - b.x, a.y - b.y);
    return res;
}

//----------------------------------------------------------------------------------------------------------------------
inline Vector2f Vector2f::operator*(float f) const
{
	return{ x * f, y * f };
}
//----------------------------------------------------------------------------------------------------------------------
inline Vector2f Vector2f::operator*(const Vector2f& f) const
{
	return{ x * f.x, y * f.y };
}
//----------------------------------------------------------------------------------------------------------------------
inline float Vector2f::Length() const
{
	return Math::Sqrt(Math::Pow2(x) + Math::Pow2(y));
}
}

}