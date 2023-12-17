
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanQueueFamily.h>
#include <Math/Functions.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanInstance.h>
#include <Core/CoreDefines.h>
#include <Containers/Vector.h>
// Created on 2020-04-20 by sisco

namespace 
{
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const lyra::Vector<struct VkSurfaceFormatKHR>& surfaceFormats)
	{
		if (surfaceFormats.size() == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };

		for (const auto& availableFormat : surfaceFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}

		return surfaceFormats[0];
	}

	VkPresentModeKHR ChooseSwapPresentMode(const lyra::Vector<enum VkPresentModeKHR>& presentModes, bool _vsync)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		if(!_vsync)
		{
			for (const auto& availablePresentMode : presentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
					return availablePresentMode;

				if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					bestMode = availablePresentMode;
			}
		}

		return bestMode;
	}

	VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		lyraAssert(0 && "Max extents not implemented");
		return {};
	}
}

namespace lyra
{
	VkSampleCountFlagBits VulkanSwapchain::GetSupportedMSAA(VkPhysicalDevice _physicalDevice)
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(_physicalDevice, &physicalDeviceProperties);


		VkSampleCountFlags counts = Math::Min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physicalDeviceProperties.limits.framebufferDepthSampleCounts);
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		else if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		else if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		else if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		else if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		else if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
		else return VK_SAMPLE_COUNT_1_BIT;
	}

	VulkanSwapchain::SwapChainSupportDetails VulkanSwapchain::GetSupportDetails(VkPhysicalDevice device, const VkSurfaceKHR& presentationSurface)
	{
		SwapChainSupportDetails details = {};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, presentationSurface, &details.m_Capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, presentationSurface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.m_Formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, presentationSurface, &formatCount, details.m_Formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, presentationSurface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.m_PresentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, presentationSurface, &presentModeCount, details.m_PresentModes.data());
		}

		return details;
	}

	unsigned int VulkanSwapchain::GetMaxSupportedSwapchainBuffers() const
	{
		auto imageCount = m_SupportDetails.m_Capabilities.minImageCount + 1; // +1 for triple buffering
		if (m_SupportDetails.m_Capabilities.maxImageCount > 0 && imageCount > m_SupportDetails.m_Capabilities.maxImageCount)
		{
			imageCount = m_SupportDetails.m_Capabilities.maxImageCount;
		}
		return imageCount;
	}

	VulkanSwapchain::VulkanSwapchain(InitInfo& _initInfo)
	{
		m_SupportDetails = GetSupportDetails(_initInfo.m_Device.GetVkPhysicalDevice(), _initInfo.m_Instance.GetVkPresentationSurface());
		
		const auto surfaceFormat = ChooseSwapSurfaceFormat(m_SupportDetails.m_Formats);
		const auto presentMode = ChooseSwapPresentMode(m_SupportDetails.m_PresentModes, _initInfo.mVSync);

		m_Device = _initInfo.m_Device.GetVkDevice();

		m_SwapChainExtent = ChooseSwapExtent(m_SupportDetails.m_Capabilities);
		m_SwapChainImageFormat = surfaceFormat.format;

		m_MSAASamples = _initInfo.mMSAA ? GetSupportedMSAA(_initInfo.m_Device.GetVkPhysicalDevice()) : VK_SAMPLE_COUNT_1_BIT;
		
		m_NumSwapchainBuffers = GetMaxSupportedSwapchainBuffers();
		
		auto imageCount = m_NumSwapchainBuffers;

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = _initInfo.m_Instance.GetVkPresentationSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = m_SwapChainExtent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const auto graphicsFamiliy = _initInfo.m_Device.GetQueues().GetGraphicsFamilyIndex();
		const auto presentFamily = _initInfo.m_Device.GetQueues().GetPresentFamilyIndex();
		uint32_t queueFamilyIndices[] = { graphicsFamiliy, presentFamily };

		if (graphicsFamiliy != presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = m_SupportDetails.m_Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
		if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
			lyraAssert(0 && "failed to create swap chain!");

		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());


		CreateImageViews();
	}
	
	VulkanSwapchain::~VulkanSwapchain()
	{
		if(m_Device)
		{
			for (const auto imageView : m_SwapChainImageViews)
				vkDestroyImageView(m_Device, imageView, nullptr);
			m_SwapChainImageViews.clear();
			vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
		}
	}

	void VulkanSwapchain::CreateImageViews()
	{
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		int i = 0;
		for (auto& imageView : m_SwapChainImageViews)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			if (vkCreateImageView(m_Device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
				lyraAssert(0 && "failed to create image views!");

			++i;
		}
	}


}

