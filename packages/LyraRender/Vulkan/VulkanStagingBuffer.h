
#pragma once
#include <vulkan/vulkan.h>
#include <Vulkan/VulkanBuffer.h>
// Created on 2018-12-20 by fmacias

namespace lyra
{
class VulkanQueue;
class VulkanTexture;
class VulkanStagingBuffer
{
public:
	VulkanStagingBuffer(VkDevice _device, VkPhysicalDevice _physicalDevice, const VulkanQueue& _transferQueue, uint64_t _size);
    ~VulkanStagingBuffer() { Destroy(); }
    void* Map();
    void Unmap();
    void CopyToBuffer(VulkanBuffer& _dstBuffer) { CopyToBuffer(_dstBuffer, m_Size, 0, 0); };
    void CopyToBuffer(VulkanTexture& _dstBuffer, int _layerCount);
    void CopyToBuffer(VulkanBuffer& _dstBuffer, uint64_t _size, uint64_t _srcOffset, uint64_t _dstOffset);
private:

    void Create(uint64_t _size, VkPhysicalDevice _physicalDevice);
    void Destroy();

    VkDevice m_Device;
    VkCommandPool m_CommandPool;
    const VulkanQueue& m_TransferQueue;
    VulkanBuffer m_Buffer;
	uint64_t m_Size = 0;
};
}

