
#pragma once
#include <Shaders/ShaderProgram.h>
#include <Containers/Vector.h>
#include <String/String.h>

// Created on 2020-08-12 by Sisco

namespace lyra
{

namespace VulkanShaderCompiler
{
    Vector<char> CompileShader(const ShaderProgram& _program, StringView _code);
};

}

