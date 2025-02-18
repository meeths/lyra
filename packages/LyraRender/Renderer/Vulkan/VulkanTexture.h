
#pragma once
#include "Renderer/ITexture.h"
#include <vulkan/vulkan.hpp>

// Created on 2024-11-12 by sisco

namespace lyra
{

class VulkanTexture : public ITexture
{
public:

    VulkanTexture(const Descriptor& desc, vk::Device* device);
    ~VulkanTexture() override;
    
    BaseResourceType GetBaseResourceType() const override { return BaseResourceType::Texture; }
    Descriptor* GetDescriptor() override { return &m_descriptor; }

    vk::Image m_vkImage;
    vk::Format m_vkFormat = vk::Format::eR8G8B8A8Unorm;
    vk::DeviceMemory m_vkDeviceMemory;
    vk::Device* m_vkDevice;
    
    bool m_ownsMemory = true;
protected:
    void Destroy();
    Descriptor m_descriptor;
};

}

