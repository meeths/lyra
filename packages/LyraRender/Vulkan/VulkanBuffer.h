
#pragma once
#include <vulkan/vulkan.h>
#include <Vulkan/VulkanDeviceMemory.h>
// Created on 2018-12-20 by fmacias

namespace lyra
{
class VulkanDeviceMemory;

class VulkanBuffer
{
public:
    friend class VulkanStagingBuffer;

	void Create(VkDevice _device, VkPhysicalDevice _physicalDevice,uint64_t _size, uint32_t _usage,  VkSharingMode _sharingMode = VK_SHARING_MODE_EXCLUSIVE, VkMemoryPropertyFlags _memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	void Create(VkDevice _device,uint64_t _size, uint32_t _usage, const VulkanDeviceMemory& _deviceMemory, uint64_t _offset = 0, VkSharingMode _sharingMode = VK_SHARING_MODE_EXCLUSIVE);
	void Destroy();
	VkBuffer GetVkBuffer() const { return m_Buffer; }
    const VulkanDeviceMemory& GetDeviceMemory() const { return m_DeviceMemory; }
    VulkanDeviceMemory& GetDeviceMemory() { return m_DeviceMemory; }
	uint64_t GetSize() const { return m_Size; };
private:
    void BindDeviceMemory(const VulkanDeviceMemory& _deviceMemory, uint64_t _offset = 0);

	VkDevice m_Device = nullptr;
	VkBuffer m_Buffer = nullptr;
	VulkanDeviceMemory m_DeviceMemory;
	bool m_OwnsDeviceMemory = false;
	uint64_t m_Size = 0;
	uint32_t m_Usage = 0;


};
}

