
#include <Renderer/Vulkan/VulkanFramebuffer.h>

// Created on 2024-11-14 by sisco

namespace lyra
{
    bool VulkanFramebuffer::Initialize(VulkanDevice* device, IFramebufferInfo& info)
    {
        m_vulkanDevice = device;
        m_depthStencilView = info.depthStencilView;
        return true;
    }
}

