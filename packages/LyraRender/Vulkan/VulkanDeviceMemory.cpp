#include <Vulkan/VulkanDeviceMemory.h>
#include <stdexcept>
#include <Core/CoreDefines.h>

// Created on 2018-12-20 by fmacias

namespace lyra
{
	void VulkanDeviceMemory::Allocate(VkDevice _device, VkPhysicalDevice _physicalDevice, uint64_t _size, uint32_t _memoryType)
	{
		m_Device = _device;
		m_PhysicalDevice = _physicalDevice;
		
		if(m_DeviceMemory != nullptr)
		{
			lyraAssert(0 && "trying to allocate an already allocated VkDeviceMemory object");
		}

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = _size;
		allocInfo.memoryTypeIndex = _memoryType;

		if (vkAllocateMemory(m_Device, &allocInfo, nullptr, &m_DeviceMemory) != VK_SUCCESS) 
		{
			lyraAssert(0 && "failed to allocate device memory");
		}
	    
		m_Size = _size;
	}

	void * VulkanDeviceMemory::Map(uint64_t _size)
	{
		void* data = nullptr;
		vkMapMemory(m_Device, m_DeviceMemory, 0, _size, 0, &data);
		return data;
	}

	void VulkanDeviceMemory::Unmap()
	{
		vkUnmapMemory(m_Device, m_DeviceMemory);
	}

	void VulkanDeviceMemory::Free()
	{
		    vkFreeMemory(m_Device, m_DeviceMemory, nullptr);
			m_DeviceMemory = nullptr;
			m_Size = 0;
	}

    void VulkanDeviceMemory::Flush() const
    {
        // Flush to make changes visible to the host 
        VkMappedMemoryRange memoryRange = {};
        memoryRange.memory = m_DeviceMemory;
        memoryRange.size = m_Size;
        memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        vkFlushMappedMemoryRanges(m_Device, 1, &memoryRange);
    }

    uint32_t VulkanDeviceMemory::FindMemoryType(VkPhysicalDevice _physicalDevice, uint32_t _typeFilter, VkMemoryPropertyFlags _properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((_typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & _properties) == _properties)
            {
                return i;
            }
        }
        lyraAssert(0 && "failed to find suitable memory type!");
		return 0;
    }

}

