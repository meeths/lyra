
#include <Vulkan/VulkanRenderPass.h>

#include "VulkanRenderPassManager.h"
#include "VulkanSystem.h"

// Created on 2023-12-27 by sisco

namespace lyra
{
    VulkanRenderPass::VulkanRenderPass(StringView passName, VulkanSystem& _vulkanSystem) : m_vulkanSystem(_vulkanSystem)
    {
        m_vulkanSystem.GetVulkanRenderPassManager().AddRenderPass(passName, this);
    }
}

