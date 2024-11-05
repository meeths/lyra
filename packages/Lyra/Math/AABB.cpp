#include <Math/AABB.h>
#include <Containers/Array.h>

namespace lyra
{
    Math::AABB Math::AABB::GetTransformed(const glm::mat4x4& _matrix) const
    {
        AABB ret;
        Array<glm::vec3, 8> boxVertices;

        boxVertices[0].x = m_Min.x; boxVertices[0].y = m_Min.y; boxVertices[0].z = m_Min.z;
        boxVertices[1].x = m_Min.x; boxVertices[1].y = m_Min.y; boxVertices[1].z = m_Max.z;
        boxVertices[2].x = m_Min.x; boxVertices[2].y = m_Max.y; boxVertices[2].z = m_Min.z;
        boxVertices[3].x = m_Min.x; boxVertices[3].y = m_Max.y; boxVertices[3].z = m_Max.z;
        boxVertices[4].x = m_Max.x; boxVertices[4].y = m_Min.y; boxVertices[4].z = m_Min.z;
        boxVertices[5].x = m_Max.x; boxVertices[5].y = m_Min.y; boxVertices[5].z = m_Max.z;
        boxVertices[6].x = m_Max.x; boxVertices[6].y = m_Max.y; boxVertices[6].z = m_Min.z;
        boxVertices[7].x = m_Max.x; boxVertices[7].y = m_Max.y; boxVertices[7].z = m_Max.z;

        for (const auto& vertex : boxVertices)
        {
            const glm::vec3 convertedVtx = _matrix * glm::vec4(vertex.x, vertex.y, vertex.z, 1.0);
            ret += glm::vec3(convertedVtx.x, convertedVtx.y, convertedVtx.z);
        }

        return ret;
    }

}

