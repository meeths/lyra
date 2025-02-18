
#pragma once
#include "Renderer/Format.h"
#include "Renderer/IResourceView.h"
#include "Containers/Vector.h"

// Created on 2024-11-14 by sisco

namespace lyra
{

class IFramebuffer
{
public:

    class IFramebufferLayout
    {
    public:
        struct TargetLayout
        {
            Format format;
            size_t sampleCount;
        };
        struct LayoutInfo
        {
            Vector<TargetLayout*> renderTargets;
            TargetLayout* depthStencil = nullptr;
        };
    };
    
    struct IFramebufferInfo
    {
        Vector<IResourceView*> const* renderTargetViews;
        IResourceView* depthStencilView;
        IFramebufferLayout* layout;
    };
};

}

