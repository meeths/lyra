#include "base/system.glsl"

layout(location = 0) in vec4 fragColor;
#ifndef NO_TEXTURE
    layout(binding = 1) uniform sampler2D texSampler[1024];
    layout(location = 1) in vec2 fragTexCoord;
    layout(push_constant) uniform PER_OBJECT
    {
        int imgIdx;
    }pc;
#else
    layout(push_constant) uniform PER_OBJECT
    {
        vec4 inColor;
    }pc;
#endif


layout(location = 0) out vec4 outColor;

void main() 
{
    outColor = fragColor;
    
    #ifndef NO_TEXTURE
        outColor *= texture(texSampler[pc.imgIdx], fragTexCoord);
    #else
        outColor *= pc.inColor;
    #endif
}