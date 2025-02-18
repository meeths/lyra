
#include <Renderer/Vulkan/VulkanTexture.h>

// Created on 2024-11-12 by sisco

namespace lyra
{
    VulkanTexture::VulkanTexture(const Descriptor& desc, vk::Device* device)
        : m_vkDevice(device), m_descriptor(desc)
    {
    }

    VulkanTexture::~VulkanTexture()
    {
        if (m_ownsMemory)
        {
            Destroy();
        }
    }

    void VulkanTexture::Destroy()
    {
        m_vkDevice->destroyImage(m_vkImage, nullptr);
        m_vkImage = nullptr;
    }
}

