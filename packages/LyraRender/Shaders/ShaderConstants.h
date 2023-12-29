
#pragma once

// Created on 2020-08-13 by Sisco
#include <String/String.h>
#include <Core/CoreDefines.h>
namespace lyra
{

namespace ShaderConstants
{
    enum class ShaderType
    {
        VertexShader,
        FragmentShader,
        ComputeShader,
        Geometry,
        TessellationControl,
        TessellationEvaluation,
        InvalidType
    };

};

namespace ShaderBuiltinNames
{
    DefineGlobalConstexprVariableAccessor(StringView, kVertexShader_2D_PCT, "VS_2D_PosColTex");
    DefineGlobalConstexprVariableAccessor(StringView, kVertexShader_3D_PC, "VS_3D_PosCol");
    DefineGlobalConstexprVariableAccessor(StringView, kVertexShader_3D_PCT, "VS_3D_PosColTex");
    DefineGlobalConstexprVariableAccessor(StringView, kVertexShader_3D_PCTN, "VS_3D_PosColTexNrm");
    DefineGlobalConstexprVariableAccessor(StringView, kVertexShader_3D_PCTNT, "VS_3D_PosColTexNrmTan");
    DefineGlobalConstexprVariableAccessor(StringView, kVertexShader_3D_PCTNT_PBR, "VS_3D_PosColTexNrmTan_PBR");
    DefineGlobalConstexprVariableAccessor(StringView, kVertexShader_3D_PCT_Skybox, "VS_3D_PosColTex_Skybox");

    DefineGlobalConstexprVariableAccessor(StringView, kFragmentShader_TextureColor, "FS_TextureColor");
    DefineGlobalConstexprVariableAccessor(StringView, kFragmentShader_PBR, "FS_PBR");
    DefineGlobalConstexprVariableAccessor(StringView, kFragmentShader_Color, "FS_Color");
    DefineGlobalConstexprVariableAccessor(StringView, kFragmentShader_DebugColor, "FS_DebugColor");
    DefineGlobalConstexprVariableAccessor(StringView, kFragmentShader_SkyboxColor, "FS_SkyboxColor");
    DefineGlobalConstexprVariableAccessor(StringView, kFragmentShader_DeferredResolve, "FS_DeferredResolve");

    DefineGlobalConstexprVariableAccessor(StringView, kFragmentShader_TextureColor_Deferred, "FS_TextureColor_Deferred");

    
}
}

