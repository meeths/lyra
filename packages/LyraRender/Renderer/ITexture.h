#pragma once

// Created on 2024-11-12 by sisco
#include "Resources/IBaseResource.h"
#include "Renderer/Format.h"

namespace lyra
{

class ITexture : public IBaseResource
{
public:
    struct SampleDesc
    {
        int32 numSamples = 1;
        int32 quality = 0;
    };

    struct Extents
    {
        int32 width = 0;
        int32 height = 0;
        int32 depth = 0;
    };

    struct Descriptor : DescriptorBase
    {
        Extents extents;
        int32 arraySize = 0;
        int mipLevels = 0;
        Format format = Format::Unknown;
        SampleDesc sampleDesc;
        ResourceState state = ResourceState::Undefined;
    };

    struct SubResourceData
    {
        void const* data;
        size_t strideY;
        size_t strideZ;
    };

    virtual Descriptor* GetDescriptor() = 0;
    BaseResourceType GetBaseResourceType() const override { return BaseResourceType::Texture; }
    virtual TextureType GetTextureType() = 0;
};

}

