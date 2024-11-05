#pragma once
#include <glm/vec2.hpp>
#include <glm/vec2.hpp>
namespace lyra
{
namespace Math
{
    class Rectangle
    {

    public:
        Vector2f GetOrigin() const { return m_Min; }
        Vector2f GetDimensions() const { return m_Max - m_Min; }

        Vector2f m_Min = {};
        Vector2f m_Max = {};

        friend bool operator ==(const Rectangle& a, const Rectangle& b)
        {
            return a.m_Max == b.m_Max && a.m_Min == b.m_Min;
        }

        friend bool operator !=(const Rectangle& a, const Rectangle& b)
        {
            return !(a == b);
        }

        bool IsPointInsideRect(const Vector2f& _point) const
        {
            return
                m_Min.x <= _point.x &&
                m_Min.y <= _point.y &&
                m_Max.x >= _point.x &&
                m_Max.y >= _point.y;
    }

    };

    class Rectanglei
    {

    public:
        Vector2i GetOrigin() const { return m_Min; }
        Vector2i GetDimensions() const { return mMax - m_Min; }

        Vector2i m_Min = {};
        Vector2i m_max = {};

        friend bool operator ==(const Rectanglei& a, const Rectanglei& b)
        {
            return a.mMax == b.mMax && a.m_Min == b.m_Min;
        }

        friend bool operator !=(const Rectanglei& a, const Rectanglei& b)
        {
            return !(a == b);
        }
    };
}
}