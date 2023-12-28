
#pragma once
#include <Vulkan/VulkanRenderingPath.h>
#include <Memory/UniquePointer.h>
#include <Vulkan/VulkanDebugRenderPass.h>

// Created on 2023-12-28 by sisco

namespace lyra
{
    class VulkanDebugRenderPass;
}

namespace lyra
{
class VulkanSystem;

class VulkanForwardRenderingPath : public VulkanRenderingPath
{
public:
    struct InitInfo
    {
        VulkanSystem& m_vulkanSystem;
    };

    VulkanForwardRenderingPath(const InitInfo& _initInfo);
    void Configure() override;
    void Execute() override;
protected:
    VulkanSystem& m_vulkanSystem;
};

}

