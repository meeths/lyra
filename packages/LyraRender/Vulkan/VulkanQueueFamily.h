#pragma once
#include <vulkan/vulkan.h>

namespace lyra
{

class VulkanQueueFamily
{
public:
	typedef struct QueueFamilyIndices
	{
		int m_GraphicsFamily = -1;
		int m_ComputeFamily = -1;
		int m_PresentFamily = -1;
        int m_TransferFamily = -1;
		
		bool IsComplete() const
		{
			return m_GraphicsFamily >= 0 && m_PresentFamily >= 0 && m_TransferFamily >= 0;
		}
	} QueueFamilyIndices;

    static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& presentationSurface);
    static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& device);
};

}