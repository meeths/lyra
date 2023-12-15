#pragma once
#include <Math/Vector4f.h>
#include <Math/Matrix33.h>
#include <Math/Quaternion.h>

namespace lyra
{
namespace Math
{
	class Matrix44
	{
	public:

		static const Matrix44 Zero;
		static const Matrix44 Identity;

		Matrix44() {};
        Matrix44(const Matrix44& o);
        Matrix44(const Matrix33& o);

		Matrix44(float a11, float a12, float a13, float a14,
			float a21, float a22, float a23, float a24,
			float a31, float a32, float a33, float a34,
			float a41, float a42, float a43, float a44);

		void Set(float a11, float a12, float a13, float a14,
			float a21, float a22, float a23, float a24,
			float a31, float a32, float a33, float a34,
			float a41, float a42, float a43, float a44);

        inline Vector3f operator * (const Vector3f &v) const
        {
            Vector3f r;

            float fInvW = 1.0f / (m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3]);

            r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0]) * fInvW;
            r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1]) * fInvW;
            r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]) * fInvW;

            return r;
        }
        inline Vector4f operator * (const Vector4f& v) const
        {
            return Vector4f(
                m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
                m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
                m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
                m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
            );
        }

        Matrix44 operator*(const Matrix44& _other) const
        {
            Matrix44 r;
            r[0][0] = (*this)[0][0] * _other[0][0] + (*this)[0][1] * _other[1][0] + (*this)[0][2] * _other[2][0] + (*this)[0][3] * _other[3][0];
            r[0][1] = (*this)[0][0] * _other[0][1] + (*this)[0][1] * _other[1][1] + (*this)[0][2] * _other[2][1] + (*this)[0][3] * _other[3][1];
            r[0][2] = (*this)[0][0] * _other[0][2] + (*this)[0][1] * _other[1][2] + (*this)[0][2] * _other[2][2] + (*this)[0][3] * _other[3][2];
            r[0][3] = (*this)[0][0] * _other[0][3] + (*this)[0][1] * _other[1][3] + (*this)[0][2] * _other[2][3] + (*this)[0][3] * _other[3][3];

            r[1][0] = (*this)[1][0] * _other[0][0] + (*this)[1][1] * _other[1][0] + (*this)[1][2] * _other[2][0] + (*this)[1][3] * _other[3][0];
            r[1][1] = (*this)[1][0] * _other[0][1] + (*this)[1][1] * _other[1][1] + (*this)[1][2] * _other[2][1] + (*this)[1][3] * _other[3][1];
            r[1][2] = (*this)[1][0] * _other[0][2] + (*this)[1][1] * _other[1][2] + (*this)[1][2] * _other[2][2] + (*this)[1][3] * _other[3][2];
            r[1][3] = (*this)[1][0] * _other[0][3] + (*this)[1][1] * _other[1][3] + (*this)[1][2] * _other[2][3] + (*this)[1][3] * _other[3][3];

            r[2][0] = (*this)[2][0] * _other[0][0] + (*this)[2][1] * _other[1][0] + (*this)[2][2] * _other[2][0] + (*this)[2][3] * _other[3][0];
            r[2][1] = (*this)[2][0] * _other[0][1] + (*this)[2][1] * _other[1][1] + (*this)[2][2] * _other[2][1] + (*this)[2][3] * _other[3][1];
            r[2][2] = (*this)[2][0] * _other[0][2] + (*this)[2][1] * _other[1][2] + (*this)[2][2] * _other[2][2] + (*this)[2][3] * _other[3][2];
            r[2][3] = (*this)[2][0] * _other[0][3] + (*this)[2][1] * _other[1][3] + (*this)[2][2] * _other[2][3] + (*this)[2][3] * _other[3][3];

            r[3][0] = (*this)[3][0] * _other[0][0] + (*this)[3][1] * _other[1][0] + (*this)[3][2] * _other[2][0] + (*this)[3][3] * _other[3][0];
            r[3][1] = (*this)[3][0] * _other[0][1] + (*this)[3][1] * _other[1][1] + (*this)[3][2] * _other[2][1] + (*this)[3][3] * _other[3][1];
            r[3][2] = (*this)[3][0] * _other[0][2] + (*this)[3][1] * _other[1][2] + (*this)[3][2] * _other[2][2] + (*this)[3][3] * _other[3][2];
            r[3][3] = (*this)[3][0] * _other[0][3] + (*this)[3][1] * _other[1][3] + (*this)[3][2] * _other[2][3] + (*this)[3][3] * _other[3][3];

            return r;
        }

		Matrix44& operator/=(float _factor)
		{
			m[0] /= _factor;
			m[1] /= _factor;
			m[2] /= _factor;
			m[3] /= _factor;
			return *this;
		}

		[[nodiscard]] Matrix44 Transposed() const 
		{
			Matrix44 mx;
			mx.m[0][0] = m[0][0]; mx.m[1][0] = m[0][1]; mx.m[2][0] = m[0][2]; mx.m[3][0] = m[0][3];
			mx.m[0][1] = m[1][0]; mx.m[1][1] = m[1][1]; mx.m[2][1] = m[1][2]; mx.m[3][1] = m[1][3];
			mx.m[0][2] = m[2][0]; mx.m[1][2] = m[2][1]; mx.m[2][2] = m[2][2]; mx.m[3][2] = m[2][3];
			mx.m[0][3] = m[3][0]; mx.m[1][3] = m[3][1]; mx.m[2][3] = m[3][2]; mx.m[3][3] = m[3][3];
			return mx;
		}

		// Returns the inverse of this matrix
		[[nodiscard]] Matrix44 Inverted() const
		{
			Matrix44 b;

			b.m[0][0] = det3x3(m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
			b.m[0][1] = -det3x3(m[0][1], m[0][2], m[0][3], m[2][1], m[2][2], m[2][3], m[3][1], m[3][2], m[3][3]);
			b.m[0][2] = det3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[3][1], m[3][2], m[3][3]);
			b.m[0][3] = -det3x3(m[0][1], m[0][2], m[0][3], m[1][1], m[1][2], m[1][3], m[2][1], m[2][2], m[2][3]);

			b.m[1][0] = -det3x3(m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
			b.m[1][1] = det3x3(m[0][0], m[0][2], m[0][3], m[2][0], m[2][2], m[2][3], m[3][0], m[3][2], m[3][3]);
			b.m[1][2] = -det3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[3][0], m[3][2], m[3][3]);
			b.m[1][3] = det3x3(m[0][0], m[0][2], m[0][3], m[1][0], m[1][2], m[1][3], m[2][0], m[2][2], m[2][3]);

			b.m[2][0] = det3x3(m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
			b.m[2][1] = -det3x3(m[0][0], m[0][1], m[0][3], m[2][0], m[2][1], m[2][3], m[3][0], m[3][1], m[3][3]);
			b.m[2][2] = det3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[3][0], m[3][1], m[3][3]);
			b.m[2][3] = -det3x3(m[0][0], m[0][1], m[0][3], m[1][0], m[1][1], m[1][3], m[2][0], m[2][1], m[2][3]);

			b.m[3][0] = -det3x3(m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
			b.m[3][1] = det3x3(m[0][0], m[0][1], m[0][2], m[2][0], m[2][1], m[2][2], m[3][0], m[3][1], m[3][2]);
			b.m[3][2] = -det3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[3][0], m[3][1], m[3][2]);
			b.m[3][3] = det3x3(m[0][0], m[0][1], m[0][2], m[1][0], m[1][1], m[1][2], m[2][0], m[2][1], m[2][2]);

			double det = (m[0][0] * b.m[0][0]) + (m[1][0] * b.m[0][1]) + (m[2][0] * b.m[0][2]) + (m[3][0] * b.m[0][3]);

			double epsilon = Math::EpsilonSmall;
			if (det + epsilon >= 0.0f && det - epsilon <= 0.0f) det = Math::Sign(det) * 0.0001f;
			float oodet = (float)(1.0 / det);

			b.m[0][0] *= oodet;
			b.m[0][1] *= oodet;
			b.m[0][2] *= oodet;
			b.m[0][3] *= oodet;

			b.m[1][0] *= oodet;
			b.m[1][1] *= oodet;
			b.m[1][2] *= oodet;
			b.m[1][3] *= oodet;

			b.m[2][0] *= oodet;
			b.m[2][1] *= oodet;
			b.m[2][2] *= oodet;
			b.m[2][3] *= oodet;

			b.m[3][0] *= oodet;
			b.m[3][1] *= oodet;
			b.m[3][2] *= oodet;
			b.m[3][3] *= oodet;

			return b;
		}

		[[nodiscard]] Matrix44 InverseTransposed() const
		{
			float SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
			float SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
			float SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
			float SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
			float SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
			float SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
			float SubFactor06 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
			float SubFactor07 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
			float SubFactor08 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
			float SubFactor09 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
			float SubFactor10 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
			float SubFactor11 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
			float SubFactor12 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
			float SubFactor13 = m[1][2] * m[2][3] - m[2][2] * m[1][3];
			float SubFactor14 = m[1][1] * m[2][3] - m[2][1] * m[1][3];
			float SubFactor15 = m[1][1] * m[2][2] - m[2][1] * m[1][2];
			float SubFactor16 = m[1][0] * m[2][3] - m[2][0] * m[1][3];
			float SubFactor17 = m[1][0] * m[2][2] - m[2][0] * m[1][2];
			float SubFactor18 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

			Matrix44 inverseTransposed;
			inverseTransposed[0][0] = +(m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02);
			inverseTransposed[0][1] = -(m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04);
			inverseTransposed[0][2] = +(m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05);
			inverseTransposed[0][3] = -(m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05);

			inverseTransposed[1][0] = -(m[0][1] * SubFactor00 - m[0][2] * SubFactor01 + m[0][3] * SubFactor02);
			inverseTransposed[1][1] = +(m[0][0] * SubFactor00 - m[0][2] * SubFactor03 + m[0][3] * SubFactor04);
			inverseTransposed[1][2] = -(m[0][0] * SubFactor01 - m[0][1] * SubFactor03 + m[0][3] * SubFactor05);
			inverseTransposed[1][3] = +(m[0][0] * SubFactor02 - m[0][1] * SubFactor04 + m[0][2] * SubFactor05);

			inverseTransposed[2][0] = +(m[0][1] * SubFactor06 - m[0][2] * SubFactor07 + m[0][3] * SubFactor08);
			inverseTransposed[2][1] = -(m[0][0] * SubFactor06 - m[0][2] * SubFactor09 + m[0][3] * SubFactor10);
			inverseTransposed[2][2] = +(m[0][0] * SubFactor11 - m[0][1] * SubFactor09 + m[0][3] * SubFactor12);
			inverseTransposed[2][3] = -(m[0][0] * SubFactor08 - m[0][1] * SubFactor10 + m[0][2] * SubFactor12);

			inverseTransposed[3][0] = -(m[0][1] * SubFactor13 - m[0][2] * SubFactor14 + m[0][3] * SubFactor15);
			inverseTransposed[3][1] = +(m[0][0] * SubFactor13 - m[0][2] * SubFactor16 + m[0][3] * SubFactor17);
			inverseTransposed[3][2] = -(m[0][0] * SubFactor14 - m[0][1] * SubFactor16 + m[0][3] * SubFactor18);
			inverseTransposed[3][3] = +(m[0][0] * SubFactor15 - m[0][1] * SubFactor17 + m[0][2] * SubFactor18);

			float determinant =
				+m[0][0] * inverseTransposed[0][0]
				+ m[0][1] * inverseTransposed[0][1]
				+ m[0][2] * inverseTransposed[0][2]
				+ m[0][3] * inverseTransposed[0][3];

			inverseTransposed /= determinant;

			return inverseTransposed;
		}

		Vector4f& operator[](int i) { return m[i]; };
		const Vector4f& operator[](int i) const { return m[i]; };

        [[nodiscard]] static Matrix44 MakePerspective(float _aspect, float _fov, float _near, float _far);
        [[nodiscard]] static Matrix44 MakeOrtho(float _left, float _right, float _top, float _bottom, float _near, float _far);
        [[nodiscard]] static Matrix44 MakeLookAt(const Vector3f& _pos, const Vector3f& _lookAt, const Vector3f& _up);
        [[nodiscard]] static Matrix44 MakeTransform(const Vector3f& _pos, const Quaternion& _orientation, const Vector3f& _scale);

		[[nodiscard]] Vector3f GetRight() const { return {m[0].x, m[0].y, m[0].z}; }
		[[nodiscard]] Vector3f GetUp() const { return {m[1].x, m[1].y, m[1].z}; }
		[[nodiscard]] Vector3f GetForward() const { return {m[2].x, m[2].y, m[2].z}; }
		[[nodiscard]] Vector3f GetTranslation() const {return {m[3].x, m[3].y, m[3].z}; }

		void SetTranslation(const Vector3f& translation)
		{
			m[3][0] = translation.x;
			m[3][1] = translation.y;
			m[3][2] = translation.z;
		}

        void Decompose(Vector3f& scaling, Quaternion& rotation, Vector3f& position) const
        {
            const Matrix44& _this = *this;

            // extract translation
            position.x = _this[3][0];
            position.y = _this[3][1];
            position.z = _this[3][2];

            // extract the rows of the matrix
            Vector3f vRows[3] = {
                Vector3f(_this[0][0],_this[0][1],_this[0][2]),
                Vector3f(_this[1][0],_this[1][1],_this[1][2]),
                Vector3f(_this[2][0],_this[2][1],_this[2][2])
            };

            // extract the scaling factors
            scaling.x = vRows[0].Length();
            scaling.y = vRows[1].Length();
            scaling.z = vRows[2].Length();

            // and remove all scaling from the matrix
            if (scaling.x != 0)
            {
                vRows[0] /= scaling.x;
            }
            if (scaling.y != 0)
            {
                vRows[1] /= scaling.y;
            }
            if (scaling.z != 0)
            {
                vRows[2] /= scaling.z;
            }

            // build a 3x3 rotation matrix
            Matrix33 m(
                vRows[0][0], vRows[0][1], vRows[0][2],
                vRows[1][0], vRows[1][1], vRows[1][2],
                vRows[2][0], vRows[2][1], vRows[2][2]);

            // and generate the rotation quaternion from it
            rotation.FromMatrix33(m);
        }

	    Matrix33 GetMatrix33() const
		{
		    const Matrix44& _this = *this;

		    Matrix33 mat;
		    mat[0][0] = _this[0][0];
		    mat[0][1] = _this[0][1];
		    mat[0][2] = _this[0][2];
		    mat[1][0] = _this[1][0];
		    mat[1][1] = _this[1][1];
		    mat[1][2] = _this[1][2];
		    mat[2][0] = _this[2][0];
		    mat[2][1] = _this[2][1];
		    mat[2][2] = _this[2][2];
		    return mat;
		}
		
        Vector4f m[4];

    private:

		// Code taken and adapted from nVidia's nv_algebra: det2x2, det3x3, invert, multiply
		// -----
		// Calculate the determinant of a 2x2 matrix
		static float det2x2(float a1, float a2, float b1, float b2)
		{
			return a1 * b2 - b1 * a2;
		}

		// Calculate the determinant of a 3x3 matrix
		static float det3x3(float a1, float a2, float a3, float b1, float b2, float b3, float c1, float c2, float c3)
		{
			return a1 * det2x2(b2, b3, c2, c3) - b1 * det2x2(a2, a3, c2, c3) + c1 * det2x2(a2, a3, b2, b3);
		}
	};

}

inline lyra::Math::Matrix44::Matrix44(const Matrix44 & o)
{
	m[0] = o.m[0];
	m[1] = o.m[1];
	m[2] = o.m[2];
	m[3] = o.m[3];
}


inline lyra::Math::Matrix44::Matrix44(const Matrix33 & o)
{
    this->Set(o[0][0], o[0][1], o[0][2], 0,
        o[1][0], o[1][1], o[1][2], 0,
        o[2][0], o[2][1], o[2][2], 0,
        0, 0, 0, 1);
}

inline lyra::Math::Matrix44::Matrix44(float a11, float a12, float a13, float a14,
	float a21, float a22, float a23, float a24,
	float a31, float a32, float a33, float a34,
	float a41, float a42, float a43, float a44)
{
	this->Set(a11, a12, a13, a14,
		a21, a22, a23, a24,
		a31, a32, a33, a34,
		a41, a42, a43, a44);
}

inline void lyra::Math::Matrix44::Set(float a11, float a12, float a13, float a14,
	float a21, float a22, float a23, float a24,
	float a31, float a32, float a33, float a34,
	float a41, float a42, float a43, float a44)
{
	m[0].Set(a11, a12, a13, a14);
	m[1].Set(a21, a22, a23, a24);
	m[2].Set(a31, a32, a33, a43);
	m[3].Set(a41, a42, a43, a44);
}

inline Math::Matrix44 Math::Matrix44::MakePerspective(float _aspect, float _fov, float _near, float _far)
{
    Math::Matrix44 matrix = Zero;

    float rad = _fov;
    float tanHalfFovy = tan(rad / 2.0f);

    matrix[0][0] = 1.0f / (_aspect * tanHalfFovy);
    matrix[1][1] = 1.0f / (tanHalfFovy);
    matrix[2][2] = -(_far + _near) / (_far - _near);
    matrix[2][3] = -1.0f;
    matrix[3][2] = -(2.0f * _far * _near) / (_far - _near);

    return matrix;
}

inline Math::Matrix44 Math::Matrix44::MakeOrtho(float _left, float _right, float _top, float _bottom, float _near, float _far)
{
    Math::Matrix44 matrix = Zero;
    matrix[0][0] = 2.0f / (_right - _left);
    matrix[1][1] = 2.0f / (_bottom - _top);
    matrix[2][2] = 1.0f / (_near - _far);
	matrix[3].Set(      
		-(_right + _left) / (_right - _left),
      -(_bottom + _top) / (_bottom - _top),
      _near / (_near - _far),
      1.0f);
	
	return matrix;
}

inline Math::Matrix44 Math::Matrix44::MakeLookAt(const Math::Vector3f& _pos, const Math::Vector3f& _lookAt, const Math::Vector3f& _up)
{
    Math::Vector3f oZaxis = _pos - _lookAt;
    oZaxis.Normalize();
    Math::Vector3f oXaxis = _up.Cross(oZaxis);
    oXaxis.Normalize();
    Math::Vector3f oYaxis = oZaxis.Cross(oXaxis);

    Math::Matrix44 matrix;
    matrix[0][0] = oXaxis.x;          matrix[0][1] = oYaxis.x;          matrix[0][2] = oZaxis.x;          matrix[0][3] = 0;
    matrix[1][0] = oXaxis.y;          matrix[1][1] = oYaxis.y;          matrix[1][2] = oZaxis.y;          matrix[1][3] = 0;
    matrix[2][0] = oXaxis.z;          matrix[2][1] = oYaxis.z;          matrix[2][2] = oZaxis.z;          matrix[2][3] = 0;

    matrix[3][0] = -oXaxis.Dot(_pos); matrix[3][1] = -oYaxis.Dot(_pos); matrix[3][2] = -oZaxis.Dot(_pos); matrix[3][3] = 1;
    return matrix;
}

inline Math::Matrix44 Math::Matrix44::MakeTransform(const Math::Vector3f& _pos, const Math::Quaternion& _orientation, const Math::Vector3f& _scale)
{
    Math::Matrix33 rot3x3, scale3x3;
    _orientation.ToMatrix33(rot3x3);
    scale3x3 = Math::Matrix33::Zero;
    scale3x3[0][0] = _scale.x;
    scale3x3[1][1] = _scale.y;
    scale3x3[2][2] = _scale.z;

    Math::Matrix44 matrix;
    matrix = rot3x3 * scale3x3;

    matrix[3][0] = _pos.x;
    matrix[3][1] = _pos.y;
    matrix[3][2] = _pos.z;

    // No projection term
    matrix[0][3] = 0;
    matrix[1][3] = 0;
    matrix[2][3] = 0;

    matrix[3][3] = 1;
    return matrix;
}

}