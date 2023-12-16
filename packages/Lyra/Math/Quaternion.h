#pragma once
#include <Math/Vector4f.h>
#include <Math/Vector3f.h>

namespace lyra {
namespace Math{
	class Matrix33;
}
}

namespace lyra
{
namespace Math
{
class Quaternion
{
public:
	Quaternion() = default;
	Quaternion(float x, float y, float z, float w) : x{ x }, y{ y }, z{ z }, w{w} {}
	Quaternion(const Quaternion& v) : x{ v.x }, y{ v.y }, z{ v.z }, w{v.w} {}
	Quaternion(const Vector4f& v) : x{ v.x }, y{ v.y }, z{ v.z }, w{v.w} {}
	explicit Quaternion(float f) : x{ f }, y{ f }, z{ f }, w{ f } {}
	DeclareDefaultMoveable(Quaternion);
	~Quaternion() = default;


	Quaternion& operator= (const Quaternion& q)
	{
		w = q.w;
		x = q.x;
		y = q.y;
		z = q.z;
		return *this;
	}


	bool operator== (const Quaternion& r) const
	{
		return r.x == x && r.y == y && r.z == z && r.w == w;

	}
	bool operator!= (const Quaternion& r) const
	{
		return !operator==(r);
	}

	Quaternion operator* (float _scalar) const
	{
		return Quaternion ( x * _scalar, y * _scalar, z * _scalar, w * _scalar );
	}

    Quaternion operator* (const Quaternion& _other) const
    {
        return {
	        w * _other.x + x * _other.w + y * _other.z - z * _other.y,
            w * _other.y + y * _other.w + z * _other.x - x * _other.z,
            w * _other.z + z * _other.w + x * _other.y - y * _other.x,
            w * _other.w - x * _other.x - y * _other.y - z * _other.z
        };
    }

    Quaternion operator/ (const Quaternion& _other) const
	{
	    return {
		    w *  _other.x + x * _other.w + y * _other.z - z * _other.y,
        w * _other.y + y * _other.w + z * _other.x - x * _other.z,
        w * _other.z + z * _other.w + x * _other.y - y * _other.x,
        w * _other.w - x * _other.x - y * _other.y - z * _other.z
	    };
	}

    Quaternion operator/ (float _f) const
	{
	    return {x/_f, y/_f, z/_f, w/_f};
	}

    Vector3f operator* (const Vector3f& _vector) const
    {
	    const Vector3f qvec(x, y, z);
        Vector3f uv = qvec.Cross(_vector);
        Vector3f uuv = qvec.Cross(uv);
        uv = uv * (2.0f * w);
        uuv = uv * 2.0f;

        return (_vector + uv + uuv);
    }
	void FromAngleAxis (float angle, const Vector3f& axis);
	void FromAxis (const Vector3f& axisX, const Vector3f& axisY, const Vector3f& axisZ);
	void FromEulerAngles (float yaw, float pitch, float roll);
	void FromMatrix33 (const Matrix33& matrix);
	void ToMatrix33 (Matrix33& matrix) const;

    float GetYaw(bool reprojectAxis = true) const;
    float GetPitch(bool reprojectAxis = true) const;
    float GetRoll(bool reprojectAxis = true) const;

	float Dot (const Quaternion& q) const;
	float Norm () const;
	float SquaredLength	() const;
	float Normalize(); 
	Quaternion Inverse () const;
	Quaternion UnitInverse () const;
	Quaternion Exp () const;
	Quaternion Log () const;
    Quaternion Conjugate() const;

    Quaternion Difference(const Quaternion& q);

	union
	{
		struct { float x, y, z, w;};
		float v[4] {0, 0, 0, 1.0f};
	};

	static const Quaternion Identity;
	static const Quaternion Zero;
};
}
}
