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
		AABB() : m_Valid{false} {};
		AABB(Vector3f _min, Vector3f _max) : m_Min(_min), m_Max(_max), m_Valid{true} {};
		bool IsValid() const { return m_Valid; };
		const Vector3f& GetMin() const {return m_Min;};
		const Vector3f& GetMax() const {return m_Max;};

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
		Vector3f m_Min;
		Vector3f m_Max;
		bool m_Valid;

	};

inline bool AABB::operator==(const AABB & other) const
{
	return m_Max == other.m_Max && m_Min == other.m_Min;
}

inline AABB& AABB::operator+=(const Vector3f & point)
{
        if (m_Valid)
        {
            m_Min.MakeFloor( point );
            m_Max.MakeCeil( point );
        }
        else
        {
            m_Min = m_Max = point;
            m_Valid = true;
        }

        return *this;
}

inline AABB& AABB::operator+=(const AABB& _other)
{
    if (m_Valid)
    {
        if (_other.m_Min.x < m_Min.x) m_Min.x = _other.m_Min.x;
        if (_other.m_Min.y < m_Min.y) m_Min.y = _other.m_Min.y;
        if (_other.m_Min.z < m_Min.z) m_Min.z = _other.m_Min.z;
        if (_other.m_Max.x > m_Max.x) m_Max.x = _other.m_Max.x;
        if (_other.m_Max.y > m_Max.y) m_Max.y = _other.m_Max.y;
        if (_other.m_Max.z > m_Max.z) m_Max.z = _other.m_Max.z;
    }
    else
    {
        *this = _other;
        m_Valid = true;
    }

    return *this;
}


// Returns the midpoint between the min and max points.
inline Vector3f AABB::GetCenter() const
{
    return (m_Min + m_Max) * 0.5f;
}

// Returns the extent around the center
inline Vector3f AABB::GetExtent() const
{
    return GetFullExtent() * 0.5f;
}

// Returns the full size of the box, from corner to corner.
inline Vector3f AABB::GetFullExtent() const
{
    return m_Max - m_Min;
}
}


}