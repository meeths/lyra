
#pragma once
#include <Resources/IBaseResource.h>
#include <Core/Expected.h>
#include <String/String.h>

// Created on 2024-11-06 by sisco

namespace lyra
{

class BufferResource : public IBaseResource
{
public:
    struct Descriptor : DescriptorBase
    {
        size_t sizeInBytes = 0;
        size_t elementSize = 0;
    };
    
    BaseResourceType GetBaseResourceType() const override { return BaseResourceType::Buffer; }
    virtual Descriptor* GetDescriptor() const = 0; 
    virtual Expected<void*, StringView> Map(size_t sizeToMap) = 0;
    virtual void Unmap() = 0;
};

}

