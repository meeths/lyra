
#pragma once
#include <vulkan/vulkan.h>
// Created on 2018-12-20 by fmacias

namespace lyra
{
class VulkanDeviceMemory
{
public:
	void Allocate(VkDevice _device, VkPhysicalDevice _physicalDevice, uint64_t _size, uint32_t _memoryType);
	void* Map(uint64_t size);
	void* Map() { return Map(m_Size); }
	void Unmap();
	void Free();
    void Flush() const;

	VkDeviceMemory GetVkDeviceMemory() const { return m_DeviceMemory; }
	uint64_t GetAllocatedSize() const { return m_Size; }
	
    static uint32_t FindMemoryType(VkPhysicalDevice _physicalDevice, uint32_t _typeFilter, VkMemoryPropertyFlags _properties);
private:
	VkDevice m_Device = nullptr;
	VkPhysicalDevice m_PhysicalDevice = nullptr;
	VkDeviceMemory m_DeviceMemory = nullptr;
	uint64_t m_Size = 0;
};

}

