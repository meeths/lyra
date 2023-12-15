#include <Math/Matrix33.h>

namespace lyra
{
namespace Math
{ 
    const Matrix33 Matrix33::Zero = 
    {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };

    const Matrix33 Matrix33::Identity = 
    {
        1.0f, 0, 0, 
        0, 1.0f, 0, 
        0, 0, 1.0f
    };

}
}
