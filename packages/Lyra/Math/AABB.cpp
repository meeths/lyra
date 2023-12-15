#include <Math/AABB.h>
#include <Core/Types.h>
namespace lyra
{
    Math::AABB Math::AABB::GetTransformed(const Math::Matrix44& _matrix) const
    {
        AABB ret;
        Array<Vector3f, 8> boxVertices;

        boxVertices[0].x = mMin.x; boxVertices[0].y = mMin.y; boxVertices[0].z = mMin.z;
        boxVertices[1].x = mMin.x; boxVertices[1].y = mMin.y; boxVertices[1].z = mMax.z;
        boxVertices[2].x = mMin.x; boxVertices[2].y = mMax.y; boxVertices[2].z = mMin.z;
        boxVertices[3].x = mMin.x; boxVertices[3].y = mMax.y; boxVertices[3].z = mMax.z;
        boxVertices[4].x = mMax.x; boxVertices[4].y = mMin.y; boxVertices[4].z = mMin.z;
        boxVertices[5].x = mMax.x; boxVertices[5].y = mMin.y; boxVertices[5].z = mMax.z;
        boxVertices[6].x = mMax.x; boxVertices[6].y = mMax.y; boxVertices[6].z = mMin.z;
        boxVertices[7].x = mMax.x; boxVertices[7].y = mMax.y; boxVertices[7].z = mMax.z;

        for (const auto& vertex : boxVertices)
        {
            const Vector3f convertedVtx = _matrix * Vector3f(vertex.x, vertex.y, vertex.z);
            ret += Vector3f(convertedVtx.x, convertedVtx.y, convertedVtx.z);
        }

        return ret;
    }

}

