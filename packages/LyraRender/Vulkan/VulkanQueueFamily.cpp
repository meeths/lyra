#include <Vulkan/VulkanQueueFamily.h>
#include <Containers/Vector.h>

namespace lyra
{
VulkanQueueFamily::QueueFamilyIndices VulkanQueueFamily::FindQueueFamilies(const VkPhysicalDevice& device, const VkSurfaceKHR& presentationSurface)
{
    QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) 
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
			indices.m_GraphicsFamily = i;

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            indices.m_ComputeFamily = i;

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && indices.m_TransferFamily < 0)
            indices.m_TransferFamily = i;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, presentationSurface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) 
			indices.m_PresentFamily = i;

		i++;
	}
    return indices;
}

VulkanQueueFamily::QueueFamilyIndices VulkanQueueFamily::FindQueueFamilies(const VkPhysicalDevice& device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    Vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.m_GraphicsFamily = i;

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
            indices.m_ComputeFamily = i;

        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT && indices.m_TransferFamily < 0)
            indices.m_TransferFamily = i;

        i++;
    }
    return indices;
}
}
