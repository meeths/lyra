#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <Memory/SharedPointer.h>

namespace lyra
{
class ApplicationWindow;
class VulkanValidatorLayers;
class VulkanDevice;

class VulkanInstance
{
public:
	typedef struct InitInfo
	{
		ApplicationWindow& m_Window;
#ifdef DEBUG
		const bool m_UseValidationLayers = true;
#else
		const bool m_UseValidationLayers = false;
#endif
		
		bool mBreakOnValidationErrors = m_UseValidationLayers;
	}InitInfo;
	
	VulkanInstance(InitInfo _initInfo);
	~VulkanInstance();

	VkInstance GetVkInstance() const { return m_Instance; }
	VkSurfaceKHR GetVkPresentationSurface() const { return m_Surface; }
	SharedPointer<VulkanValidatorLayers> GetVulkanValidationLayers() const { return m_ValidationLayers; }
private:
	VkInstance m_Instance = {};
	SharedPointer<VulkanValidatorLayers> m_ValidationLayers;

	VkSurfaceKHR m_Surface = {};
};
}
