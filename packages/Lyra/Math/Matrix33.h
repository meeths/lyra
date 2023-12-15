#pragma once
#include <Math/Vector3f.h>

namespace lyra
{
namespace Math
{
class Matrix33
{
public:
    static const Matrix33 Zero;
    static const Matrix33 Identity;

	Matrix33() = default;
    Matrix33(const Matrix33& v) : m{ v.m[0], v.m[1], v.m[2] }{};
    Matrix33(const Vector3f& i, const Vector3f& j, const Vector3f& k) : m{ i, j, k } {};
    explicit Matrix33(float f) : m{ {f,f,f} ,{f,f,f} ,{f,f,f} } {};

    Matrix33(float a11, float a12, float a13,
        float a21, float a22, float a23,
        float a31, float a32, float a33);

    void Set(float a11, float a12, float a13,
        float a21, float a22, float a23, 
        float a31, float a32, float a33);

    float* AsPointer();
    const float* AsPointer() const;
    const float* AsConstPointer() const;

    Matrix33 operator-() const;

    Matrix33& operator*=(const Matrix33& _o);
    Matrix33& operator+=(const Matrix33& _o);
    Matrix33& operator-=(const Matrix33& _o);
    Matrix33& operator*=(const float k);
    Matrix33& operator/=(const float k);
    Matrix33 operator*(const Matrix33& _o) const;
    Matrix33 operator+(const Matrix33& _o) const;
    Matrix33 operator-(const Matrix33& _o) const;
    Matrix33 operator*(const float k) const;
    Matrix33 operator/(const float k) const;

    Vector3f operator * (const Vector3f &v) const
    {
        Vector3f r;

        r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z);
        r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z);
        r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);

        return r;
    }

    Matrix33 Inverse()
    {
        // Invert a 3x3 using cofactors.  This is about 8 times faster than
        // the Numerical Recipes code which uses Gaussian elimination.

        Matrix33 inv;
        
        inv[0][0] = m[1][1]*m[2][2] -
            m[1][2]*m[2][1];
        inv[0][1] = m[0][2]*m[2][1] -
            m[0][1]*m[2][2];
        inv[0][2] = m[0][1]*m[1][2] -
            m[0][2]*m[1][1];
        inv[1][0] = m[1][2]*m[2][0] -
            m[1][0]*m[2][2];
        inv[1][1] = m[0][0]*m[2][2] -
            m[0][2]*m[2][0];
        inv[1][2] = m[0][2]*m[1][0] -
            m[0][0]*m[1][2];
        inv[2][0] = m[1][0]*m[2][1] -
            m[1][1]*m[2][0];
        inv[2][1] = m[0][1]*m[2][0] -
            m[0][0]*m[2][1];
        inv[2][2] = m[0][0]*m[1][1] -
            m[0][1]*m[1][0];

        float fDet =
            m[0][0]*inv[0][0] +
            m[0][1]*inv[1][0]+
            m[0][2]*inv[2][0];

        float fInvDet = 1.0f/fDet;
        for (int iRow = 0; iRow < 3; iRow++)
        {
            for (int iCol = 0; iCol < 3; iCol++)
                inv[iRow][iCol] *= fInvDet;
        }

        return inv;
    }
    
    float& operator()(unsigned i, unsigned j);
    const float& operator()(unsigned i, unsigned j) const;
    float& Get(unsigned i, unsigned j);
    const float& Get(unsigned i, unsigned j) const;

    Vector3f& operator[](int i) { return m[i]; };
    const Vector3f& operator[](int i) const { return m[i]; };

private:
    Vector3f m[3];

};
inline Matrix33::Matrix33(float a11, float a12, float a13,
    float a21, float a22, float a23,
    float a31, float a32, float a33)
{
    Set(a11, a12, a13,
        a21, a22, a23,
        a31, a32, a33);
}

inline void Matrix33::Set(float a11, float a12, float a13,
    float a21, float a22, float a23,
    float a31, float a32, float a33)
{
    m[0].Set(a11, a12, a13);
    m[1].Set(a21, a22, a23);
    m[2].Set(a31, a32, a33);
}

inline Matrix33 lyra::Math::Matrix33::operator*(const Matrix33 & _o) const
{
    Matrix33 Result;

    Result[0][0] = (m[0][0] * _o[0][0]) + (m[1][0] * _o[0][1]) + (m[2][0] * _o[0][2]);
    Result[1][0] = (m[0][0] * _o[1][0]) + (m[1][0] * _o[1][1]) + (m[2][0] * _o[1][2]);
    Result[2][0] = (m[0][0] * _o[2][0]) + (m[1][0] * _o[2][1]) + (m[2][0] * _o[2][2]);
    Result[0][1] = (m[0][1] * _o[0][0]) + (m[1][1] * _o[0][1]) + (m[2][1] * _o[0][2]);
    Result[1][1] = (m[0][1] * _o[1][0]) + (m[1][1] * _o[1][1]) + (m[2][1] * _o[1][2]);
    Result[2][1] = (m[0][1] * _o[2][0]) + (m[1][1] * _o[2][1]) + (m[2][1] * _o[2][2]);
    Result[0][2] = (m[0][2] * _o[0][0]) + (m[1][2] * _o[0][1]) + (m[2][2] * _o[0][2]);
    Result[1][2] = (m[0][2] * _o[1][0]) + (m[1][2] * _o[1][1]) + (m[2][2] * _o[1][2]);
    Result[2][2] = (m[0][2] * _o[2][0]) + (m[1][2] * _o[2][1]) + (m[2][2] * _o[2][2]);

    return Result;
}
}
}
