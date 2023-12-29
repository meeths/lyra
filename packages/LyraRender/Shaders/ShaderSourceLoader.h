#pragma once
#include <String/String.h>
#include <Core/Expected.h>

namespace lyra
{
    class ShaderSourceLoader
    {
    public:
        // Return the source code of the complete shader
        static Expected<String, String> LoadShaderSource(StringView path);
    };
}
