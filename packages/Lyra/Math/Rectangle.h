#pragma once
#include <Math/Vector2f.h>
#include <Math/Vector2i.h>
namespace lyra
{
namespace Math
{
    class Rectangle
    {

    public:
        Vector2f GetOrigin() const { return mMin; }
        Vector2f GetDimensions() const { return mMax - mMin; }

        Math::Vector2f mMin = {};
        Math::Vector2f mMax = {};

        friend bool operator ==(const Rectangle& a, const Rectangle& b)
        {
            return a.mMax == b.mMax && a.mMin == b.mMin;
        }

        friend bool operator !=(const Rectangle& a, const Rectangle& b)
        {
            return !(a == b);
        }

        bool IsPointInsideRect(const Vector2f& _point) const
        {
            return
                mMin.x <= _point.x &&
                mMin.y <= _point.y &&
                mMax.x >= _point.x &&
                mMax.y >= _point.y;
    }

    };

    class Rectanglei
    {

    public:
        Vector2i GetOrigin() const { return mMin; }
        Vector2i GetDimensions() const { return mMax - mMin; }

        Math::Vector2i mMin = {};
        Math::Vector2i mMax = {};

        friend bool operator ==(const Rectanglei& a, const Rectanglei& b)
        {
            return a.mMax == b.mMax && a.mMin == b.mMin;
        }

        friend bool operator !=(const Rectanglei& a, const Rectanglei& b)
        {
            return !(a == b);
        }
    };
}
}