
#pragma once
#include "Renderer/Format.h"

// Created on 2024-11-14 by sisco

namespace lyra
{

class IRendererResourceView
{
public:
    virtual ~IRendererResourceView() = default;

    enum class ViewType
    {
        Unknown,
        RenderTarget,
        DepthStencil,
        ShaderResource,
        UnorderedAccess,
        AccelerationStructure,
    };
    
    struct Descriptor
    {
        ViewType type;
        Format format;

        SubresourceRange subresourceRange;
        BufferRange bufferRange;
    };

    virtual ViewType GetViewType() const = 0;
    virtual Descriptor* GetDescriptor() const = 0;
};

}

