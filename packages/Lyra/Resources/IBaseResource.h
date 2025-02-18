#pragma once
#include <Core/Types.h>

// Created on 2024-11-06 by sisco

namespace lyra
{

class IBaseResource
{
public:
    virtual ~IBaseResource() = default;

    enum class BaseResourceType : uint8
    {
        Unknown,
        Buffer,
        Texture,
    };

    struct DescriptorBase
    {
        BaseResourceType type = BaseResourceType::Unknown;
        bool isShared = false;
    };

    virtual BaseResourceType GetBaseResourceType() const = 0;
};

}

