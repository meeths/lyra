#include <Vulkan/VulkanInstance.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Application/Windows/ApplicationWindow.h>
#include <iostream>
#include <Vulkan/VulkanValidatorLayers.h>
#include <Core/CoreDefines.h>
#include <Log/Log.h>

namespace lyra
{
VulkanInstance::VulkanInstance(InitInfo _initInfo)
{
	// GLFW initialization
	unsigned int glfwExtensionCount = 0;
	if(!glfwInit())
	{
		lyraAssert(0 && "Could not initialize GLFW");
	}

	const auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	Vector<const char*> extensionNames(glfwExtensions, glfwExtensions + glfwExtensionCount);

	// Vulkan instance
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "lyra_application";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "lyra";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;


	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;


	if(_initInfo.m_UseValidationLayers)
	{
		m_ValidationLayers = MakeSharedPointer<VulkanValidatorLayers>();
		m_ValidationLayers->Prepare(createInfo);
		extensionNames.push_back(VulkanValidatorLayers::GetRequiredExtensionName());
	}
	
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensionNames.size());
	createInfo.ppEnabledExtensionNames = extensionNames.data();


	if(vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
	{
		lyraAssert(0 && "Could not create vulkan instance");
	}

	// Extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	Vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "Vulkan extensions:" << std::endl;

	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
	}

	// Surface
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = _initInfo.m_Window.GetWindowHandle();
	surfaceCreateInfo.hinstance = _initInfo.m_Window.GetInstanceHandle();

	const auto CreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(m_Instance, "vkCreateWin32SurfaceKHR"));

	if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(m_Instance, &surfaceCreateInfo, nullptr, &m_Surface) != VK_SUCCESS) 
	{
		lyraAssert(0 && "failed to create window surface!");
	}

	// Device
	if(_initInfo.m_UseValidationLayers)
	{
		if(m_ValidationLayers->CreateValidatorCallbacks(m_Instance) != VK_SUCCESS)
		{
			lyraLogWarning("VulkanInstance: Could not create validator callbacks");
		}
		m_ValidationLayers->SetBreakOnError(_initInfo.mBreakOnValidationErrors);
	}
}

VulkanInstance::~VulkanInstance()
{
	if(m_ValidationLayers)
		m_ValidationLayers->Shutdown(m_Instance);
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	vkDestroyInstance(m_Instance, nullptr);
	glfwTerminate();
	lyraLogInfo("VulkanInstance shut down successfully");
}

}
