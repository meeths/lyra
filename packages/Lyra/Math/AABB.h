#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace lyra
{
    namespace Math
{
	class AABB
	{
	public:
		AABB() : m_Valid{false} {};
		AABB(glm::vec3 _min, glm::vec3 _max) : m_Min(std::move(_min)), m_Max(std::move(_max)), m_Valid{true} {};
		bool IsValid() const { return m_Valid; };
		const glm::vec3& GetMin() const {return m_Min;};
		const glm::vec3& GetMax() const {return m_Max;};

		bool operator==(const AABB& other) const;
        AABB& operator+=(const glm::vec3& point);
        AABB& operator+=(const AABB& _other);
        AABB operator+(const glm::vec3& point) const { return AABB(*this) += point; };
        AABB operator+(const AABB& other) const { return AABB(*this) += other; };

        glm::vec3 GetCenter() const;
        glm::vec3 GetExtent() const;
        glm::vec3 GetFullExtent() const;

        AABB GetTransformed(const glm::mat4x4& _matrix) const;
	private:
		glm::vec3 m_Min;
		glm::vec3 m_Max;
		bool m_Valid;

	};

inline bool AABB::operator==(const AABB & other) const
{
	return m_Max == other.m_Max && m_Min == other.m_Min;
}

inline AABB& AABB::operator+=(const glm::vec3 & point)
{
        if (m_Valid)
        {
            m_Min = glm::min(m_Min, point);
            m_Max = glm::max(m_Max, point);
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
inline glm::vec3 AABB::GetCenter() const
{
    return (m_Min + m_Max) * 0.5f;
}

// Returns the extent around the center
inline glm::vec3 AABB::GetExtent() const
{
    return GetFullExtent() * 0.5f;
}

// Returns the full size of the box, from corner to corner.
inline glm::vec3 AABB::GetFullExtent() const
{
    return m_Max - m_Min;
}
}


}