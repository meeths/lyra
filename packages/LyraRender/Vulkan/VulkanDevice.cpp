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
    void LogDeviceSpecs(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        std::cout << "Device specs:" << std::endl;
        std::cout << "\t Name - \t\t " << deviceProperties.deviceName << std::endl;
    }

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
        vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &props);

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
        vkWaitForFences(mDevice, 1, &mInflightFences[mCurrentSynchronizationObject], VK_TRUE, std::numeric_limits<uint64_t>::max());
        vkResetFences(mDevice, 1, &mInflightFences[mCurrentSynchronizationObject]);
    }

    {
        ProfileScopedN("Acquire next image");

        vkAcquireNextImageKHR(
           mDevice,
           _swapchain.GetVkSwapchain(),
           std::numeric_limits<uint64_t>::max(),
           mImageAvailableSemaphores[mCurrentSynchronizationObject],
           VK_NULL_HANDLE,
           &mCurrentAcquiredImageIndex);
    }
}

void VulkanDevice::OnEndFrame(VulkanSwapchain& _swapchain)
{
    ProfileScoped;
}

VulkanBuffer VulkanDevice::CreateVulkanBuffer(unsigned long long _size, int _usageBits, VkMemoryPropertyFlags _memoryFlags)
{
    auto buffer = VulkanBuffer();
    buffer.Create(mDevice, mPhysicalDevice, _size, _usageBits, VK_SHARING_MODE_EXCLUSIVE, _memoryFlags);
    return buffer;

}

VulkanBuffer VulkanDevice::CreateVulkanBuffer(void* _data, unsigned long long _size, int _usageBits,
    VkMemoryPropertyFlags _memoryFlags)
{

    auto stagingBuffer = VulkanStagingBuffer(mDevice, mPhysicalDevice, mQueues->GetTransferQueue(), _size);

    auto buffer = CreateVulkanBuffer(_size, _usageBits | VK_BUFFER_USAGE_TRANSFER_DST_BIT, _memoryFlags);

    auto data = stagingBuffer.Map();
    memcpy(data, _data, static_cast<size_t>(_size));
    stagingBuffer.Unmap();

    stagingBuffer.CopyToBuffer(buffer);

    return buffer;
}

void VulkanDevice::DestroySynchronizationObjects()
{
    for (auto& inflightFence : mInflightFences)
        if(inflightFence)
            vkDestroyFence(mDevice, inflightFence, nullptr);
    mInflightFences.clear();
    
    for (auto& renderSemaphore : mRenderFinishedSemaphores)
        if(renderSemaphore)
            vkDestroySemaphore(mDevice, renderSemaphore, nullptr);
    mRenderFinishedSemaphores.clear();
    
    for (auto& imageSemaphore : mImageAvailableSemaphores)
        if(imageSemaphore)
            vkDestroySemaphore(mDevice, imageSemaphore, nullptr);
    mImageAvailableSemaphores.clear();
}

void VulkanDevice::PresentFrame(VulkanSwapchain& _swapchain)
{
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentSynchronizationObject] };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { _swapchain.GetVkSwapchain() };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &mCurrentAcquiredImageIndex;
    presentInfo.pResults = nullptr;

    vkQueuePresentKHR(mQueues->GetPresentQueue().m_Queue, &presentInfo);
    mCurrentSynchronizationObject = (mCurrentSynchronizationObject + 1) % mNumSynchronizationObjects;
}

void VulkanDevice::CreateLogicalDevice(VulkanInstance& _vulkanInstance)
{
    const auto queueFamilies = VulkanQueueFamily::FindQueueFamilies(mPhysicalDevice, _vulkanInstance.GetVkPresentationSurface());

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
    if(CheckExtensionsSupport(mPhysicalDevice, {VK_EXT_DEBUG_MARKER_EXTENSION_NAME}))
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

    if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) 
    {
        lyraAssert(0 && "failed to create logical device!");
    }

    if(debugMarkerExtensionRequested)
    {
        vkDebugMarkerSetObjectTag = reinterpret_cast<PFN_vkDebugMarkerSetObjectTagEXT>(vkGetDeviceProcAddr(mDevice, "vkDebugMarkerSetObjectTagEXT"));
        vkDebugMarkerSetObjectName = reinterpret_cast<PFN_vkDebugMarkerSetObjectNameEXT>(vkGetDeviceProcAddr(mDevice, "vkDebugMarkerSetObjectNameEXT"));
        vkCmdDebugMarkerBegin = reinterpret_cast<PFN_vkCmdDebugMarkerBeginEXT>(vkGetDeviceProcAddr(mDevice, "vkCmdDebugMarkerBeginEXT"));
        vkCmdDebugMarkerEnd = reinterpret_cast<PFN_vkCmdDebugMarkerEndEXT>(vkGetDeviceProcAddr(mDevice, "vkCmdDebugMarkerEndEXT"));
        vkCmdDebugMarkerInsert = reinterpret_cast<PFN_vkCmdDebugMarkerInsertEXT>(vkGetDeviceProcAddr(mDevice, "vkCmdDebugMarkerInsertEXT"));

        lyraAssert(DebugMarkersAvailable());
    }

}

VulkanDevice::VulkanDevice(InitInfo _initInfo)
{
    const auto devices = EnumeratePhysicalDevices(_initInfo.mVulkanInstance);
    if(devices.empty())
    {
        lyraAssert(0 && "No devices with Vulkan support found.");
    }

    mPhysicalDevice = GetMostSuitedPhysicalDevice(_initInfo.mVulkanInstance);
    
#ifdef DEBUG
    LogDeviceSpecs(mPhysicalDevice);
    std::cout << "Device score: " << RateDeviceSuitability(mPhysicalDevice, _initInfo.mVulkanInstance) << std::endl;
#endif
    
    CreateLogicalDevice(_initInfo.mVulkanInstance);

    VulkanQueues::InitInfo queuesInitInfo
    {
        mDevice,
        VulkanQueueFamily::FindQueueFamilies(mPhysicalDevice, _initInfo.mVulkanInstance.GetVkPresentationSurface())
    };
    mQueues = MakeUniquePointer<VulkanQueues>(queuesInitInfo);

}

VulkanDevice::~VulkanDevice()
{
    WaitForDevice();

    DestroySynchronizationObjects();
    if(mDevice)
    {
        vkDestroyDevice(mDevice, nullptr);
        mDevice = nullptr;
    }
}

void VulkanDevice::InitializeSynchronizationObjects(size_t _numSynchronizationObjects)
{
    DestroySynchronizationObjects();
    
    mNumSynchronizationObjects = _numSynchronizationObjects;
    
    mCurrentSynchronizationObject = 0;

    mImageAvailableSemaphores.resize(mNumSynchronizationObjects);
    mRenderFinishedSemaphores.resize(mNumSynchronizationObjects);
    mInflightFences.resize(mNumSynchronizationObjects);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i = 0; i < mNumSynchronizationObjects; ++i)
    {
        if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(mDevice, &fenceInfo, nullptr, &mInflightFences[i]) != VK_SUCCESS)
                lyraAssert(0 && "failed to create semaphores!");
    }
}

void VulkanDevice::WaitForDevice() const
{
    vkDeviceWaitIdle(mDevice);
}

}
