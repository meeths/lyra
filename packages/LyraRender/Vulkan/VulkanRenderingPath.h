
#pragma once

// Created on 2023-12-28 by sisco

namespace lyra
{

class VulkanRenderingPath
{
public:
    virtual ~VulkanRenderingPath() = default;
    virtual void Configure() = 0;
    virtual void Execute() = 0;
};

}

