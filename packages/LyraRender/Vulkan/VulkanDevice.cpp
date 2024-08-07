#include <Vulkan/VulkanDevice.h>

//#include <iostream>
#include <Containers/Multimap.h>
#include <Core/CoreDefines.h>
#include <Profiler/Profiler.h>
#include <Vulkan/VulkanInstance.h>
#include <Vulkan/VulkanQueueFamily.h>
#include <Vulkan/VulkanStagingBuffer.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanValidatorLayers.h>

#include "Containers/Set.h"


namespace
{
#ifdef DEBUG
    void LogDeviceSpecs(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        std::cout << "Device specs:" << std::endl;
        std::cout << "\t Name - \t\t " << deviceProperties.deviceName << std::endl;
    }
#endif
    lyra::Vector<const char*> kRequiredExtensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    bool CheckExtensionsSupport(VkPhysicalDevice& device, const lyra::Vector<const char*>& _extensions)
    {
        // Device needs to support swapchain extension
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
        lyra::Vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        for (const auto& extensionName : _extensions)
        {
            if (std::find_if(std::begin(availableExtensions), std::end(availableExtensions),
                             [&extensionName](const VkExtensionProperties& ext)
                             {
                                 return std::strcmp(ext.extensionName, extensionName) == 0;
                             }) == std::end(availableExtensions))
                return false;
        }

        return true;
    }
    
}

namespace lyra
{
Vector<VkPhysicalDevice> VulkanDevice::EnumeratePhysicalDevices(VulkanInstance& _instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance.GetVkInstance(), &deviceCount, nullptr);
    if (deviceCount == 0) 
    {
        return {};
    }

    Vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance.GetVkInstance(), &deviceCount, devices.data());
    return devices;
}

int VulkanDevice::RateDeviceSuitability(VkPhysicalDevice& device, VulkanInstance& _vulkanInstance)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if(!CheckExtensionsSupport(device, kRequiredExtensionNames))
        return false;

    // Swap chain support needs to be adequate
    const auto& swapChainSupport = VulkanSwapchain::GetSupportDetails(device, 
        _vulkanInstance.GetVkPresentationSurface());
    if(swapChainSupport.m_Formats.empty() || swapChainSupport.m_PresentModes.empty())
        return 0;

    // Application can't function without geometry shaders
    if (!deviceFeatures.geometryShader)
        return 0;

    if(!VulkanQueueFamily::FindQueueFamilies(device, _vulkanInstance.GetVkPresentationSurface()).IsComplete())
        return 0;
    
    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) 
    {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    // TODO Calculate score better
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

VkPhysicalDevice VulkanDevice::GetMostSuitedPhysicalDevice(VulkanInstance& _instance)
{
    auto devices = EnumeratePhysicalDevices(_instance);
    // Use an ordered map to automatically sort candidates by increasing score
    Multimap<int, VkPhysicalDevice> candidates;

    for (auto& device : devices) 
    {
        auto score = RateDeviceSuitability(device, _instance);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) 
    {
        return candidates.rbegin()->second;
    }
    return nullptr;
}

VkFormat VulkanDevice::FindSupportedImageFormat(const Vector<VkFormat>& _candidates, VkImageTiling _tiling,
    VkFormatFeatureFlags _features) const
{
    for (VkFormat format : _candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &props);

        if (_tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & _features) == _features) {
            return format;
        } else if (_tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & _features) == _features) {
            return format;
        }
    }

    lyraAssert(0 && "failed to find supported format!");
    return {};
}

void VulkanDevice::BeginFrame(VulkanSwapchain& _swapchain)
{
    {
        ProfileScopedN("Wait for fences");
        vkWaitForFences(m_Device, 1, &m_InflightFences[m_CurrentSynchronizationObject], VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(m_Device, 1, &m_InflightFences[m_CurrentSynchronizationObject]);
    }

    {
        ProfileScopedN("Acquire next image");

        VkResult result = vkAcquireNextImageKHR(
           m_Device,
           _swapchain.GetVkSwapchain(),
           std::numeric_limits<uint64_t>::max(),
           m_ImageAvailableSemaphores[m_CurrentSynchronizationObject],
           VK_NULL_HANDLE,
           &m_CurrentAcquiredImageIndex);

        lyraAssert(result == VK_SUCCESS);
    }
}

void VulkanDevice::OnEndFrame(VulkanSwapchain& _swapchain)
{
    ProfileScoped;
}

VulkanBuffer VulkanDevice::CreateVulkanBuffer(unsigned long long _size, int _usageBits, VkMemoryPropertyFlags _memoryFlags)
{
    auto buffer = VulkanBuffer();
    buffer.Create(m_Device, m_PhysicalDevice, _size, _usageBits, VK_SHARING_MODE_EXCLUSIVE, _memoryFlags);
    return buffer;

}

VulkanBuffer VulkanDevice::CreateVulkanBuffer(void* _data, unsigned long long _size, int _usageBits,
    VkMemoryPropertyFlags _memoryFlags)
{

    auto stagingBuffer = VulkanStagingBuffer(m_Device, m_PhysicalDevice, m_Queues->GetTransferQueue(), _size);

    auto buffer = CreateVulkanBuffer(_size, _usageBits | VK_BUFFER_USAGE_TRANSFER_DST_BIT, _memoryFlags);

    auto data = stagingBuffer.Map();
    memcpy(data, _data, static_cast<size_t>(_size));
    stagingBuffer.Unmap();

    stagingBuffer.CopyToBuffer(buffer);

    return buffer;
}

void VulkanDevice::DestroySynchronizationObjects()
{
    for (auto& inflightFence : m_InflightFences)
        if(inflightFence)
            vkDestroyFence(m_Device, inflightFence, nullptr);
    m_InflightFences.clear();
    
    for (auto& renderSemaphore : m_RenderFinishedSemaphores)
        if(renderSemaphore)
            vkDestroySemaphore(m_Device, renderSemaphore, nullptr);
    m_RenderFinishedSemaphores.clear();
    
    for (auto& imageSemaphore : m_ImageAvailableSemaphores)
        if(imageSemaphore)
            vkDestroySemaphore(m_Device, imageSemaphore, nullptr);
    m_ImageAvailableSemaphores.clear();
}

void VulkanDevice::PresentFrame(VulkanSwapchain& _swapchain)
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    const VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentSynchronizationObject] };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    const VkSwapchainKHR swapChains[] = { _swapchain.GetVkSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_CurrentAcquiredImageIndex;
    presentInfo.pResults = nullptr;

    vkQueuePresentKHR(m_Queues->GetPresentQueue().m_Queue, &presentInfo);
    m_CurrentSynchronizationObject = (m_CurrentSynchronizationObject + 1) % m_NumSynchronizationObjects;
}

void VulkanDevice::CreateLogicalDevice(VulkanInstance& _vulkanInstance)
{
    const auto queueFamilies = VulkanQueueFamily::FindQueueFamilies(m_PhysicalDevice, _vulkanInstance.GetVkPresentationSurface());

    Vector<VkDeviceQueueCreateInfo> queueCreateInfos = {};
    Set<int> uniqueQueueFamilies = { queueFamilies.m_GraphicsFamily, queueFamilies.m_PresentFamily };

    float queuePriority = 1.0f;
    for(auto queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }


    VkPhysicalDeviceFeatures deviceFeatures = {};
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    // Enable features here
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.fillModeNonSolid = VK_TRUE;
    deviceFeatures.fullDrawIndexUint32 = VK_TRUE;

    createInfo.pEnabledFeatures = &deviceFeatures;

    bool debugMarkerExtensionRequested = false;
    if(CheckExtensionsSupport(m_PhysicalDevice, {VK_EXT_DEBUG_MARKER_EXTENSION_NAME}))
    {
        kRequiredExtensionNames.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
        debugMarkerExtensionRequested = true;

    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(kRequiredExtensionNames.size());
    createInfo.ppEnabledExtensionNames = kRequiredExtensionNames.data();


    auto validationLayers = _vulkanInstance.GetVulkanValidationLayers();
    if(validationLayers)
    {
        validationLayers->Prepare(createInfo);
    }

    if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS) 
    {
        lyraAssert(0 && "failed to create logical device!");
    }

    if(debugMarkerExtensionRequested)
    {
        vkDebugMarkerSetObjectTag = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(m_Device, "vkDebugMarkerSetObjectTagEXT"));
        vkDebugMarkerSetObjectName = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(m_Device, "vkDebugMarkerSetObjectNameEXT"));
        vkCmdDebugMarkerBegin = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(m_Device, "vkCmdDebugMarkerBeginEXT"));
        vkCmdDebugMarkerEnd = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(m_Device, "vkCmdDebugMarkerEndEXT"));
        vkCmdDebugMarkerInsert = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(m_Device, "vkCmdDebugMarkerInsertEXT"));

        lyraAssert(DebugMarkersAvailable());
    }

}

VulkanDevice::VulkanDevice(InitInfo _initInfo)
{
    const auto devices = EnumeratePhysicalDevices(_initInfo.m_VulkanInstance);
    if(devices.empty())
    {
        lyraAssert(0 && "No devices with Vulkan support found.");
    }

    m_PhysicalDevice = GetMostSuitedPhysicalDevice(_initInfo.m_VulkanInstance);
    
#ifdef DEBUG
    LogDeviceSpecs(m_PhysicalDevice);
    std::cout << "Device score: " << RateDeviceSuitability(m_PhysicalDevice, _initInfo.m_VulkanInstance) << std::endl;
#endif
    
    CreateLogicalDevice(_initInfo.m_VulkanInstance);

    VulkanQueues::InitInfo queuesInitInfo
    {
        m_Device,
        VulkanQueueFamily::FindQueueFamilies(m_PhysicalDevice, _initInfo.m_VulkanInstance.GetVkPresentationSurface())
    };
    m_Queues = MakeUniquePointer<VulkanQueues>(queuesInitInfo);

}

VulkanDevice::~VulkanDevice()
{
    WaitForDevice();

    DestroySynchronizationObjects();
    if(m_Device)
    {
        vkDestroyDevice(m_Device, nullptr);
        m_Device = nullptr;
    }
}

void VulkanDevice::InitializeSynchronizationObjects(size_t _numSynchronizationObjects)
{
    DestroySynchronizationObjects();
    
    m_NumSynchronizationObjects = _numSynchronizationObjects;
    
    m_CurrentSynchronizationObject = 0;

    m_ImageAvailableSemaphores.resize(m_NumSynchronizationObjects);
    m_RenderFinishedSemaphores.resize(m_NumSynchronizationObjects);
    m_InflightFences.resize(m_NumSynchronizationObjects);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i = 0; i < m_NumSynchronizationObjects; ++i)
    {
        if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_Device, &fenceInfo, nullptr, &m_InflightFences[i]) != VK_SUCCESS)
                lyraAssert(0 && "failed to create semaphores!");
    }
}

void VulkanDevice::WaitForDevice() const
{
    vkDeviceWaitIdle(m_Device);
}

}
