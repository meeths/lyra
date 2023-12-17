#include <Vulkan/VulkanTexture.h>
#include <Vulkan/VulkanCommandBuffer.h>
#include <Vulkan/VulkanDeviceMemory.h>
#include <TextureTypes.h>
// Created on 2018-12-24 by sisco

namespace lyra
{

    VkImageAspectFlags GetAspectFlagsFromLayoutAndFormat(VkImageLayout _layout, VkFormat _format)
    {
        if (_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || 
        _layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL || 
        _layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL)
        {
            if (_format == VK_FORMAT_D32_SFLOAT_S8_UINT || _format == VK_FORMAT_D24_UNORM_S8_UINT)
            {
                return VK_IMAGE_ASPECT_STENCIL_BIT;
            }
            return  VK_IMAGE_ASPECT_DEPTH_BIT;

        }
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }
    
    void VulkanTexture::Create(VkDevice _device, VkPhysicalDevice _physicalDevice, VkCommandPool _transferCommandPool,
        const VulkanQueue& _transferQueue, CreationInfo _creationInfo)
    {
        lyraAssert(_device && _physicalDevice && _transferCommandPool);
        m_Device = _device;
        m_TextureType = _creationInfo.m_TextureType;

        VkImageUsageFlags usageFlags = 0;
        VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT;
        switch (_creationInfo.m_TextureType)
        {
            case TextureType::Texture: usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; break;
            case TextureType::TextureCube: usageFlags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT; break;
            case TextureType::ColorAttachment:
                usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;    // Automatically transitioned in subpass            
                break;
            case TextureType::ColorAttachmentInternal:
                usageFlags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                m_ImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;    // Automatically transitioned in subpass            
                break;
            case TextureType::DepthAttachmentInternal:
                usageFlags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                m_ImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;    // Automatically transitioned in subpass            
                break;
            default: lyraAssert(0) ;
        }
        
        m_ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        m_ImageInfo.imageType = VK_IMAGE_TYPE_2D;
        m_ImageInfo.extent.width = m_TextureType == TextureType::TextureCube ? _creationInfo.m_Dimensions.x / 6 : _creationInfo.m_Dimensions.x;
        m_ImageInfo.extent.height = _creationInfo.m_Dimensions.y;
        m_ImageInfo.extent.depth = 1;
        m_ImageInfo.mipLevels = _creationInfo.m_MipLevels;
        m_ImageInfo.arrayLayers = m_TextureType == TextureType::TextureCube ? 6 : 1;
        m_ImageInfo.format = _creationInfo.m_Format;
        m_ImageInfo.tiling = _creationInfo.m_Tiling;
        m_ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        m_ImageInfo.usage = usageFlags;
        m_ImageInfo.samples = static_cast<VkSampleCountFlagBits>(_creationInfo.m_numSamples);
        m_ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (m_TextureType == TextureType::TextureCube)
            m_ImageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        
        if (vkCreateImage(m_Device, &m_ImageInfo, nullptr, &m_Image) != VK_SUCCESS)
        {
            lyraAssert(0 && "failed to create image!");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(m_Device, m_Image, &memRequirements);

        const auto bestSuitedMemoryType = VulkanDeviceMemory::FindMemoryType(_physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        m_DeviceMemory.Allocate(m_Device, _physicalDevice, memRequirements.size, bestSuitedMemoryType);

        vkBindImageMemory(m_Device, m_Image, m_DeviceMemory.GetVkDeviceMemory(), 0);
        
        VkImageLayout targetLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

        if(usageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        { 
            targetLayout = usageFlags & VK_IMAGE_USAGE_SAMPLED_BIT ? VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        else if (usageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            targetLayout = usageFlags & VK_IMAGE_USAGE_SAMPLED_BIT ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        if(!(usageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) && !(usageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT))
        {
            ScopedVulkanCommandBuffer commandBuffer(m_Device, _transferCommandPool, _transferQueue);
            TransitionImageLayout(commandBuffer, _creationInfo.m_Format, VK_IMAGE_LAYOUT_UNDEFINED, targetLayout, m_ImageInfo.mipLevels, m_ImageInfo.arrayLayers);
        }
        


        CreateTextureView(aspect);
    }
    
    void VulkanTexture::Destroy()
    {
        if(m_Device)
        {
            vkDestroyImageView(m_Device, m_ImageView, nullptr);
            vkDestroyImage(m_Device, m_Image, nullptr);
            m_DeviceMemory.Free();
            m_ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        }
        m_Device = nullptr;
        m_ImageView = nullptr;
        m_Image = nullptr;
    }
    
    void VulkanTexture::TransitionImageLayout(VulkanCommandBuffer & _commandBuffer, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, unsigned int _mipLevels, unsigned int _layerCount)
    {

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = m_Image;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = _mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = _layerCount;
        barrier.subresourceRange.aspectMask = GetAspectFlagsFromLayoutAndFormat(newLayout, format);
        
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL || newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL))
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) 
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        }
        else 
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(
            _commandBuffer.GetVkCommandBuffer(),
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        m_ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }
    void VulkanTexture::CreateTextureView(VkImageAspectFlags aspectFlags)
    {
        lyraAssert(m_ImageView == nullptr);
        
        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_Image;
        viewInfo.viewType = m_TextureType == TextureType::TextureCube ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = m_ImageInfo.format;
        viewInfo.subresourceRange = {};
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = m_ImageInfo.mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = m_TextureType == TextureType::TextureCube ? 6 : 1;

        if (vkCreateImageView(m_Device, &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
            lyraAssert(0 && "failed to create texture image view!");
        }
    }
}

