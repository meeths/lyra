
#pragma once

// Created on 2020-04-20 by sisco
#include <vulkan/vulkan.h>
#include <Containers/Vector.h>

namespace lyra
{
class VulkanQueues;
class VulkanInstance;
class VulkanDevice;

class VulkanSwapchain
{
public:

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR m_Capabilities = {};
		Vector<VkSurfaceFormatKHR> m_Formats;
		Vector<VkPresentModeKHR> m_PresentModes;
	};

	typedef struct InitInfo
	{
		VulkanDevice& m_Device;
		VulkanInstance& m_Instance;
		bool mVSync = true;
		bool mMSAA = false;
	}InitInfo;
	
	VulkanSwapchain(InitInfo& _initInfo);
	~VulkanSwapchain();

	VkSwapchainKHR GetVkSwapchain() const { return m_SwapChain; };
	const VkExtent2D& GetExtents() const { return m_SwapChainExtent; };
	VkSampleCountFlagBits GetMSAASamples() const { return m_MSAASamples; };
	VkFormat GetColorFormat() const { return m_SwapChainImageFormat; };
	VkImageView GetVkImageView(unsigned int _index) { return m_SwapChainImageViews[_index]; };
	unsigned int GetNumSwapchainBuffers() const { return m_NumSwapchainBuffers; };

	static VkSampleCountFlagBits GetSupportedMSAA(VkPhysicalDevice _physicalDevice);
	static SwapChainSupportDetails GetSupportDetails(VkPhysicalDevice device, const VkSurfaceKHR& presentationSurface);

private:
	void CreateImageViews();
	unsigned int GetMaxSupportedSwapchainBuffers() const;

	VkSampleCountFlagBits m_MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	uint32_t m_NumSwapchainBuffers = 0;
	VkSwapchainKHR m_SwapChain = {};
	Vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat = {};
	VkExtent2D m_SwapChainExtent = {};
	Vector<VkImageView> m_SwapChainImageViews;
		
	VkDevice m_Device = {};

	SwapChainSupportDetails m_SupportDetails;
};

}

