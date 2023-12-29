#include <Vulkan/VulkanShaderCompiler.h>
#include <shaderc/shaderc.hpp>
#include <Log/Log.h>

// Created on 2020-08-12 by Sisco

namespace lyra
{
    namespace VulkanShaderCompiler
    {
        Vector<char> CompileShader(const ShaderProgram& _program, StringView _code)
        {
            const shaderc::Compiler compiler;
            shaderc::CompileOptions options;
            for (auto& [defineName, defineValue] : _program.GetDefines())
            {
                if (defineName.empty())
                    options.AddMacroDefinition(defineName.data());
                else
                    options.AddMacroDefinition(defineName.data(), defineValue.data());
            }

            shaderc_shader_kind shaderKind{};
            switch (_program.GetType())
            {
            case ShaderConstants::ShaderType::VertexShader: shaderKind = shaderc_shader_kind::shaderc_glsl_vertex_shader;
                break;
            case ShaderConstants::ShaderType::FragmentShader: shaderKind = shaderc_shader_kind::shaderc_glsl_fragment_shader;
                break;
            case ShaderConstants::ShaderType::ComputeShader: shaderKind = shaderc_shader_kind::shaderc_glsl_compute_shader;
                break;
            case ShaderConstants::ShaderType::Geometry: shaderKind = shaderc_shader_kind::shaderc_glsl_geometry_shader;
                break;
            case ShaderConstants::ShaderType::TessellationControl: shaderKind = shaderc_shader_kind::shaderc_glsl_tess_control_shader;
                break;
            case ShaderConstants::ShaderType::TessellationEvaluation: shaderKind = shaderc_shader_kind::shaderc_glsl_default_tess_evaluation_shader;
                break;
            default: lyraAssert(0);
            }

            const auto result = compiler.CompileGlslToSpv(_code.data(),
                                                          shaderKind,
                                                          _program.GetFilename().c_str(),
                                                          _program.GetEntryPoint().c_str(),
                                                          options);

            if (result.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                lyraLogError(result.GetErrorMessage().c_str());
                return {};
            }
            Vector<uint32_t> assemblySPRV;
            Vector<char> assemblySPRVByte;
            assemblySPRV.assign(result.cbegin(), result.cend());
            assemblySPRVByte.assign(reinterpret_cast<char*>(assemblySPRV.data()),
                                    reinterpret_cast<char*>(assemblySPRV.data()) + assemblySPRV.size() * sizeof(assemblySPRV[0]));
            return assemblySPRVByte;
        }
    }
}