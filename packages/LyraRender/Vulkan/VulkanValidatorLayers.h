#pragma once
#include <vulkan/vulkan.h>
#include <iostream>
#include <Containers/Vector.h>

namespace lyra
{
class VulkanValidatorLayers
{
public:
	VulkanValidatorLayers() = default;
	template <typename T> void Prepare(T& createInfo);
	VkResult CreateValidatorCallbacks(VkInstance& instance);
	void Shutdown(VkInstance& instance);

	static const char* GetRequiredExtensionName() { return VK_EXT_DEBUG_REPORT_EXTENSION_NAME; };

	static void SetBreakOnError(bool _breakOnError);
private:
	static bool CheckValidationSupport(const Vector<const char*>& validationLayers);
	bool m_ValidationLayersEnabled = false;
	VkDebugReportCallbackEXT m_DebugCallback;

};

template <typename T>
void VulkanValidatorLayers::Prepare(T& createInfo)
{
	// Validation layers on debug
	static const Vector<const char*> validationLayers = 
	{
		"VK_LAYER_KHRONOS_validation"
	};

	if(CheckValidationSupport(validationLayers))
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
		std::cout << "Validation layers enabled:" << std::endl;
		for (const auto& validationLayer : validationLayers) 
		{
			std::cout << "\t" << validationLayer << std::endl;
		}
		m_ValidationLayersEnabled = true;
	} 
	else 
	{
		createInfo.enabledLayerCount = 0;
		std::cout << "Validation layers disabled:" << std::endl;
	}

}
}
