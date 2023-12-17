#include <Core/CoreDefines.h>
#include <Vulkan/VulkanBuffer.h>
#include <Vulkan/VulkanDeviceMemory.h>
// Created on 2018-12-20 by fmacias


namespace lyra
{
	void VulkanBuffer::Create(VkDevice _device, VkPhysicalDevice _physicalDevice,uint64_t _size, uint32_t _usage,  VkSharingMode _sharingMode, VkMemoryPropertyFlags _memoryFlags)
	{
		m_Device = _device;
		m_Size = _size;
	    m_Usage = _usage;
	    
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = _size;
		bufferInfo.usage = _usage;
		bufferInfo.sharingMode = _sharingMode;

		if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) 
		{
			lyraAssert(0 && "failed to create buffer!");
		}
		
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_Device, m_Buffer, &memRequirements);

		m_OwnsDeviceMemory = true;
		VulkanDeviceMemory deviceMemory;
		const auto bestSuitedMemoryType = VulkanDeviceMemory::FindMemoryType(_physicalDevice, memRequirements.memoryTypeBits, _memoryFlags);
        deviceMemory.Allocate(m_Device, _physicalDevice, memRequirements.size, bestSuitedMemoryType);
		BindDeviceMemory(deviceMemory);
	}

	void VulkanBuffer::Create(VkDevice _device,uint64_t _size, uint32_t _usage, const VulkanDeviceMemory& _deviceMemory, uint64_t _offset, VkSharingMode _sharingMode)
	{
		m_Device = _device;
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = _size;
		bufferInfo.usage = _usage;
		bufferInfo.sharingMode = _sharingMode;

		if (vkCreateBuffer(m_Device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) 
		{
			lyraAssert(0 && "failed to create buffer!");
		}

		BindDeviceMemory(_deviceMemory, _offset);
	}

	void VulkanBuffer::Destroy()
	{
		if(!m_Device)
			return;
		
        vkDestroyBuffer(m_Device, m_Buffer, nullptr);
		m_Buffer = nullptr;
		if(m_OwnsDeviceMemory)
			m_DeviceMemory.Free();

		m_Device = nullptr;
	}

	void VulkanBuffer::BindDeviceMemory(const VulkanDeviceMemory& _deviceMemory, uint64_t _offset)
	{
		if(m_Buffer == nullptr)
		{
			lyraAssert(0 && "Trying to bind an uninitialized Buffer object");
		}

		if(_deviceMemory.GetVkDeviceMemory() == nullptr)
		{
			lyraAssert(0 && "Trying to bind into an uninitialized DeviceMemory object");
		}

		vkBindBufferMemory(m_Device, m_Buffer, _deviceMemory.GetVkDeviceMemory(), _offset);
		m_DeviceMemory = _deviceMemory;
	}
}

