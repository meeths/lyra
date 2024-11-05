#pragma once
#include <vulkan/vulkan.h>
#include <Vulkan/VulkanBuffer.h>
#include <glm/vec2.hpp>
#include <TextureTypes.h>
// Created on 2018-12-24 by sisco

namespace lyra
{
    class VulkanQueue;
    class VulkanCommandBuffer;

class VulkanTexture
{
public:
    friend class VulkanTextureManager;
    
    typedef struct CreationInfo
    {
        glm::ivec2 m_Dimensions = {};
        unsigned int m_MipLevels = 1;
        unsigned int m_numSamples = 1;
        TextureType m_TextureType = TextureType::Texture;
        VkFormat m_Format = VK_FORMAT_UNDEFINED;
        VkImageTiling m_Tiling = VK_IMAGE_TILING_OPTIMAL;

        VkFilter m_MagFilter = VK_FILTER_LINEAR;
        VkFilter m_MinFilter = VK_FILTER_LINEAR;
        VkSamplerMipmapMode m_MipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        VkSamplerAddressMode m_AddressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode m_AddressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSamplerAddressMode m_AddressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        float m_MipLodBias = 0;
        VkBool32 m_AnisotropyEnable  = false;
        float m_MaxAnisotropy = 0;

    }CreationInfo;
    
    
    const VkImageCreateInfo& GetImageInfo() const { return m_ImageInfo; };

    VkImage GetVkImage() const { return m_Image; }
    VkImageView GetVkImageView() const { return m_ImageView; }
    VkImageLayout GetVkImageLayout() const { return m_ImageLayout; };
    auto GetTextureType() const { return m_TextureType; }
 private:
    void Create(VkDevice _device, VkPhysicalDevice _physicalDevice, VkCommandPool _transferCommandPool,
        const VulkanQueue& _transferQueue, CreationInfo _creationInfo);
    void Destroy();
    void TransitionImageLayout(VulkanCommandBuffer& _commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, unsigned int _mipLevels, unsigned int _layerCount);

    void CreateTextureView(VkImageAspectFlags aspectFlags);

    VkDevice m_Device = nullptr;
    VkImage m_Image = nullptr;
    VkImageView m_ImageView = nullptr;
    VkImageLayout m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;;
    VkImageCreateInfo m_ImageInfo = {};
    TextureType m_TextureType = TextureType::Undefined;
    VulkanDeviceMemory m_DeviceMemory;


};
}

