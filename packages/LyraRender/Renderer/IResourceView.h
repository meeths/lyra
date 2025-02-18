
#pragma once
#include "Renderer/Format.h"

// Created on 2024-11-14 by sisco

namespace lyra
{

class IResourceView
{
    enum class ViewType
    {
        Unknown,
        RenderTarget,
        DepthStencil,
        ShaderResource,
        UnorderedAccess,
        AccelerationStructure,
    };
    
    struct RenderTargetType
    {
        ResourceType resourceType;
    };
    
    struct ResourceViewInfo
    {
        ViewType type;
        Format format;

        RenderTargetType renderTarget;
        SubresourceRange subresourceRange;
        BufferRange bufferRange;
    };
};

}

