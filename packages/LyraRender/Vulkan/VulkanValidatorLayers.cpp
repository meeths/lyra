#include <Vulkan/VulkanValidatorLayers.h>
#include <Log/Log.h>
#include <ostream>
#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Containers/Vector.h>

namespace
{
    bool breakOnError = false;
    
    VKAPI_ATTR VkBool32 VKAPI_CALL validationDebugCallback(
        VkDebugReportFlagsEXT flags,
        VkDebugReportObjectTypeEXT objType,
        uint64_t obj,
        size_t location,
        int32_t code,
        const char* layerPrefix,
        const char* msg,
        void* userData) 
    {
        if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
        {
            lyraLogError(msg);
            if(breakOnError && ::IsDebuggerPresent())
            {
                __debugbreak();
            }
        }
        else if (flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
        {
            lyraLogInfo(msg);
        }
        else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
        {
            lyraLogWarning(msg);
        }
        else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
        {
            lyraLogWarning(msg);
        }
        else if (flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
        {
            lyraLogInfo(msg);
        }
        return VK_FALSE;
    }

    VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) 
    {
        const auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
        if (func != nullptr) 
        {
            return func(instance, pCreateInfo, pAllocator, pCallback);
        } 
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) 
    {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
        if (func != nullptr) 
        {
            func(instance, callback, pAllocator);
        }
    }
}

namespace lyra
{

VkResult VulkanValidatorLayers::CreateValidatorCallbacks(VkInstance& instance)
{
    if(!m_ValidationLayersEnabled)
    {
        return VK_ERROR_FEATURE_NOT_PRESENT;
    }

    VkDebugReportCallbackCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        

    debugCreateInfo.flags = 
        VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_DEBUG_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;

    debugCreateInfo.pfnCallback = validationDebugCallback;

    const auto result = CreateDebugReportCallbackEXT(instance, &debugCreateInfo, nullptr, &m_DebugCallback);

    if(result == VK_SUCCESS)
    {
        lyraLogInfo("Validation callbacks created");
    }
    return result;
}

void VulkanValidatorLayers::Shutdown(VkInstance& instance)
{
    if(m_ValidationLayersEnabled)
    {
        DestroyDebugReportCallbackEXT(instance, m_DebugCallback, nullptr);
        m_ValidationLayersEnabled = false;
    }
}

void VulkanValidatorLayers::SetBreakOnError(bool _breakOnError)
{
    breakOnError = _breakOnError;
}

bool VulkanValidatorLayers::CheckValidationSupport(const Vector<const char*>& validationLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    Vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto layerName : validationLayers) 
    {
        auto layerFound = false;

        for (const auto& layerProperties : availableLayers) 
        {
            if (strcmp(layerName, layerProperties.layerName) == 0) 
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

}
