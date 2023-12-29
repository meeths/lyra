#include "base/system.glsl"

layout(binding = 0) uniform UniformBufferObject 
{
    mat4 proj;
#ifndef PROJ_MATRIX_ONLY
    mat4 model;
    mat4 view;
#endif
} ubo;

// INPUT
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
#ifndef NO_TEXTURE
    layout(location = 2) in vec2 inTexCoord;
#endif

#ifdef NORMAL_IN 
    layout(location = 3) in vec3 inNormal;
#endif

#ifdef TANGENT_IN
    layout(location = 4) in vec3 inTangent;
    layout(location = 5) in vec3 inBitangent;
#endif

// OUTPUT
layout(location = 0) out vec4 fragColor;

#ifndef NO_TEXTURE
    layout(location = 1) out vec2 fragTexCoord;
#endif

#ifdef EYEPOS_OUT
    layout(location = 2) out vec3 fragEyePos;
#endif

#ifdef FRAGPOS_OUT
    layout(location = 3) out vec4 fragPos;
#endif

#ifdef TBN_MATRIX_OUT
    layout(location = 4) out mat3 normalMatrix;
#endif


// Main function
void main() 
{
    // World position 
    vec4 worldPos =
#ifndef PROJ_MATRIX_ONLY
    (ubo.model) * 
#endif
    vec4(inPosition, 1.0);
    
    // Clip space position
    gl_Position = ubo.proj *
#ifndef PROJ_MATRIX_ONLY 
    ubo.view *
#endif
    worldPos;
    
    // Frag color
    fragColor = inColor;

    // Position out (to fragment shader)
#ifdef FRAGPOS_OUT
    fragPos = worldPos;
#endif
    
    // TBN matrix for normal mapping
#ifdef TBN_MATRIX_OUT
    vec3 T = normalize(inTangent);
    vec3 N = normalize(inNormal);
    vec3 B = normalize(cross(N, T));
    normalMatrix = mat3(ubo.model) * mat3(T, B, N);
#endif
    
    // Eye position
#ifdef EYEPOS_OUT
    mat4 invView = inverse(ubo.view);
    fragEyePos = invView[3].xyz;
#endif
    
#ifndef NO_TEXTURE
    fragTexCoord = inTexCoord;
#endif
}
