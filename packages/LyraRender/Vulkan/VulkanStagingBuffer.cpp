#include <Vulkan/VulkanStagingBuffer.h>
#include <Vulkan/VulkanCommandBuffer.h>
#include <Vulkan/VulkanTexture.h>
#include <Vulkan/VulkanQueueFamily.h>
#include <Vulkan/VulkanQueue.h>
// Created on 2018-12-20 by fmacias


namespace lyra
{
    void VulkanStagingBuffer::Create(uint64_t _size, VkPhysicalDevice _physicalDevice)
    {
        m_Size = _size;

        m_Buffer.Create(m_Device, _physicalDevice, m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        const auto queueFamilies = VulkanQueueFamily::FindQueueFamilies(_physicalDevice);

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilies.m_TransferFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
            lyraAssert(0 && "failed to create command pool!");


    }
    void VulkanStagingBuffer::Destroy()
    {
        if (m_Size == 0)
            return;

        vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
        m_Buffer.Destroy();
        m_Size = 0;
    }
    VulkanStagingBuffer::VulkanStagingBuffer(VkDevice _device, VkPhysicalDevice _physicalDevice,const VulkanQueue& _transferQueue, uint64_t _size)
        :m_Device(_device), m_TransferQueue(_transferQueue)
    {
        Create(_size, _physicalDevice);
    }
    void* VulkanStagingBuffer::Map()
    {
        return m_Buffer.GetDeviceMemory().Map();
    }

    void VulkanStagingBuffer::Unmap()
    {
        m_Buffer.GetDeviceMemory().Unmap();
    }

    void VulkanStagingBuffer::CopyToBuffer(VulkanTexture& _dstBuffer, int _layerCount)
    {
        ScopedVulkanCommandBuffer commandBuffer(m_Device, m_CommandPool, m_TransferQueue);

        const auto& imageInfo = _dstBuffer.GetImageInfo();
        VkBufferImageCopy region;
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = _layerCount;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            imageInfo.extent.width,
            imageInfo.extent.height,
            imageInfo.extent.depth
        };

        vkCmdCopyBufferToImage(
            commandBuffer.GetVkCommandBuffer(),
            m_Buffer.m_Buffer,
            _dstBuffer.GetVkImage(),
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

    }

    void VulkanStagingBuffer::CopyToBuffer(VulkanBuffer& _dstBuffer, uint64_t _size, uint64_t _srcOffset, uint64_t _dstOffset)
    {
        ScopedVulkanCommandBuffer commandBuffer(m_Device, m_CommandPool, m_TransferQueue);

        VkBufferCopy copyRegion;
        copyRegion.srcOffset = _srcOffset;
        copyRegion.dstOffset = _dstOffset; // Optional
        copyRegion.size = _size;
        vkCmdCopyBuffer(commandBuffer.GetVkCommandBuffer(), m_Buffer.GetVkBuffer(), _dstBuffer.GetVkBuffer(), 1, &copyRegion);

    }
}

