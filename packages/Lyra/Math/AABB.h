#pragma once
#include <Math/Vector3f.h>
#include <Math/Matrix44.h>
namespace lyra
{
namespace Math
{
	class AABB
	{
	public:
		AABB() : mValid{false} {};
		AABB(Vector3f _min, Vector3f _max) : mMin(_min), mMax(_max), mValid{true} {};
		bool IsValid() const { return mValid; };
		const Vector3f& GetMin() const {return mMin;};
		const Vector3f& GetMax() const {return mMax;};

		bool operator==(const AABB& other) const;
        AABB& operator+=(const Vector3f& point);
        AABB& operator+=(const AABB& _other);
        AABB operator+(const Vector3f& point) const { return AABB(*this) += point; };
        AABB operator+(const AABB& other) const { return AABB(*this) += other; };

        Vector3f GetCenter() const;
        Vector3f GetExtent() const;
        Vector3f GetFullExtent() const;

        AABB GetTransformed(const Matrix44& _matrix) const;
	private:
		Vector3f mMin;
		Vector3f mMax;
		bool mValid;

	};

inline bool AABB::operator==(const AABB & other) const
{
	return mMax == other.mMax && mMin == other.mMin;
}

inline AABB& AABB::operator+=(const Vector3f & point)
{
        if (mValid)
        {
            mMin.MakeFloor( point );
            mMax.MakeCeil( point );
        }
        else
        {
            mMin = mMax = point;
            mValid = true;
        }

        return *this;
}

inline AABB& AABB::operator+=(const AABB& _other)
{
    if (mValid)
    {
        if (_other.mMin.x < mMin.x) mMin.x = _other.mMin.x;
        if (_other.mMin.y < mMin.y) mMin.y = _other.mMin.y;
        if (_other.mMin.z < mMin.z) mMin.z = _other.mMin.z;
        if (_other.mMax.x > mMax.x) mMax.x = _other.mMax.x;
        if (_other.mMax.y > mMax.y) mMax.y = _other.mMax.y;
        if (_other.mMax.z > mMax.z) mMax.z = _other.mMax.z;
    }
    else
    {
        *this = _other;
        mValid = true;
    }

    return *this;
}


// Returns the midpoint between the min and max points.
inline Vector3f AABB::GetCenter() const
{
    return (mMin + mMax) * 0.5f;
}

// Returns the extent around the center
inline Vector3f AABB::GetExtent() const
{
    return GetFullExtent() * 0.5f;
}

// Returns the full size of the box, from corner to corner.
inline Vector3f AABB::GetFullExtent() const
{
    return mMax - mMin;
}
}


}