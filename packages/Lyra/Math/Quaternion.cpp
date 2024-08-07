#include <Math/Quaternion.h>
#include <Math/Matrix33.h>

namespace lyra
{
namespace Math
{

const Quaternion Quaternion::Identity {0, 0, 0, 1.0f};
const Quaternion Quaternion::Zero {0, 0, 0, 0};

void Quaternion::FromAngleAxis (float angle, const Vector3f& axis)
{
	const auto halfAngle ( 0.5f * angle );
	const auto sinAngle = Math::Sin(halfAngle);
	x = sinAngle * axis.x;
	y = sinAngle * axis.y;
	z = sinAngle * axis.z;
	w = Math::Cos(halfAngle);
}

void Quaternion::FromAxis (const Vector3f& axisX, const Vector3f& axisY, const Vector3f& axisZ)
{
	const Matrix33 matrix{axisX, axisY, axisZ};
	FromMatrix33(matrix);
}

void Quaternion::FromMatrix33 (const Matrix33& matrix)
{
	// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".

	const float trace = matrix[0][0]+matrix[1][1]+matrix[2][2];
    float root;

    if ( trace > 0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
        root = Math::Sqrt(trace + 1.0f);  // 2w
        w = 0.5f * root;
        root = 0.5f/root;  // 1/(4w)
        x = (matrix[2][1]-matrix[1][2])*root;
        y = (matrix[0][2]-matrix[2][0])*root;
        z = (matrix[1][0]-matrix[0][1])*root;
    }
    else
    {
        // |w| <= 1/2
        static constexpr int NextIndex[3] = { 1, 2, 0 };
        int i = 0;
        if ( matrix[1][1] > matrix[0][0] )
        {
	        i = 1;
        }
        if ( matrix[2][2] > matrix[i][i] )
        {
	        i = 2;
        }
    	
	    const auto j = NextIndex[i];
	    const auto k = NextIndex[j];

        root = Math::Sqrt(matrix[i][i] - matrix[j][j] - matrix[k][k] + 1.0f);
        v[i] = 0.5f*root;
        root = 0.5f/root;
        w = (matrix[k][j] - matrix[j][k]) * root;
        v[j] = (matrix[j][i] + matrix[i][j]) * root;
        v[k] = (matrix[k][i] + matrix[i][k]) * root;
    }
}

void Quaternion::ToMatrix33 (Matrix33& matrix) const
{
	const float tx  = 2.0f * x;
	const float ty  = 2.0f * y;
	const float tz  = 2.0f * z;
	const float twx = tx * w;
	const float twy = ty * w;
	const float twz = tz * w;
	const float txx = tx * x;
	const float txy = ty * x;
	const float txz = tz * x;
	const float tyy = ty * y;
	const float tyz = tz * y;
	const float tzz = tz * z;

	matrix[0][0] = 1.0f - (tyy + tzz);
	matrix[0][1] = txy - twz;
	matrix[0][2] = txz + twy;
	matrix[1][0] = txy + twz;
	matrix[1][1] = 1.0f - (txx + tzz);
	matrix[1][2] = tyz - twx;
	matrix[2][0] = txz - twy;
	matrix[2][1] = tyz + twx;
	matrix[2][2] = 1.0f - (txx + tyy);
}

void Quaternion::FromEulerAngles (float yaw, float pitch, float roll)
{
	const auto sinPitch = Math::Sin(pitch*0.5f);
	const auto sosPitch = Math::Cos(pitch*0.5f);
	const auto sinYaw = Math::Sin(yaw*0.5f);
	const auto cosYaw = Math::Cos(yaw*0.5f);
	const auto sinRoll = Math::Sin(roll*0.5f);
	const auto cosRoll = Math::Cos(roll*0.5f);
	const auto cosPitchCosYaw(sosPitch*cosYaw);
	const auto sinPitchSinYaw(sinPitch*sinYaw);
	x = sinRoll * cosPitchCosYaw - cosRoll * sinPitchSinYaw;
	y = cosRoll * sinPitch * cosYaw + sinRoll * sosPitch * sinYaw;
	z = cosRoll * sosPitch * sinYaw - sinRoll * sinPitch * cosYaw;
	w = cosRoll * cosPitchCosYaw + sinRoll * sinPitchSinYaw;
}

float Quaternion::Dot (const Quaternion& q) const
{
	return w*q.w+x*q.x+y*q.y+z*q.z;
}

float Quaternion::Norm() const
{
	return w*w+x*x+y*y+z*z;
}

float Quaternion::Normalize()
{
	float len = Norm();
	float factor = 1.0f / Math::Sqrt(len);
	*this = *this * factor;
	return len;
}

Quaternion Quaternion::Inverse() const
{
    Quaternion q = Conjugate();
    q = q / Dot(*this);
    return q;
}

Quaternion Quaternion::Conjugate() const
{
    Quaternion q;
    q.x = -x;
    q.y = -y;
    q.z = -z;
    q.w = w;

    return q;
}

Quaternion Quaternion::Difference(const Quaternion& q)
{
    Quaternion inv = Inverse();
    return inv * q;
}

//-----------------------------------------------------------------------
float Quaternion::GetRoll(bool reprojectAxis) const
{
    if (reprojectAxis)
    {
        // roll = atan2(localx.y, localx.x)
        // pick parts of xAxis() implementation that we need
        //float fTx = 2.0f*x;
        float fTy = 2.0f*y;
        float fTz = 2.0f*z;
        float fTwz = fTz * w;
        float fTxy = fTy * x;
        float fTyy = fTy * y;
        float fTzz = fTz * z;

        // Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

        return float(Math::ATan2(fTxy + fTwz, 1.0f - (fTyy + fTzz)));

    }
    else
    {
        return float(Math::ATan2(2 * (x*y + w * z), w*w + x * x - y * y - z * z));
    }
}
//-----------------------------------------------------------------------
float Quaternion::GetPitch(bool reprojectAxis) const
{
    if (reprojectAxis)
    {
        // pitch = atan2(localy.z, localy.y)
        // pick parts of yAxis() implementation that we need
        float fTx = 2.0f*x;
        //float fTy = 2.0f*y;
        float fTz = 2.0f*z;
        float fTwx = fTx * w;
        float fTxx = fTx * x;
        float fTyz = fTz * y;
        float fTzz = fTz * z;

        // Vector3(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
        return float(Math::ATan2(fTyz + fTwx, 1.0f - (fTxx + fTzz)));
    }
    else
    {
        // internal version
        return float(Math::ATan2(2 * (y*z + w * x), w*w - x * x - y * y + z * z));
    }
}
//-----------------------------------------------------------------------
float Quaternion::GetYaw(bool reprojectAxis) const
{
    if (reprojectAxis)
    {
        // yaw = atan2(localz.x, localz.z)
        // pick parts of zAxis() implementation that we need
        float fTx = 2.0f*x;
        float fTy = 2.0f*y;
        float fTz = 2.0f*z;
        float fTwy = fTy * w;
        float fTxx = fTx * x;
        float fTxz = fTz * x;
        float fTyy = fTy * y;

        // Vector3(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

        return float(Math::ATan2(fTxz + fTwy, 1.0f - (fTxx + fTyy)));

    }
    else
    {
        // internal version
        return float(Math::ASin(-2.0f * (x*z - w * y)));
    }
}
}
}
