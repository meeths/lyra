#pragma once
#include <mutex>
#include <vulkan/vulkan.h>
#include <Core/Types.h>
#include <Vulkan/VulkanQueue.h>
#include <Vulkan/VulkanBuffer.h>
#include <Containers/Vector.h>

inline PFN_vkDebugMarkerSetObjectTagEXT vkDebugMarkerSetObjectTag = VK_NULL_HANDLE;
    inline PFN_vkDebugMarkerSetObjectNameEXT vkDebugMarkerSetObjectName = VK_NULL_HANDLE;
    inline PFN_vkCmdDebugMarkerBeginEXT vkCmdDebugMarkerBegin = VK_NULL_HANDLE;
    inline PFN_vkCmdDebugMarkerEndEXT vkCmdDebugMarkerEnd = VK_NULL_HANDLE;
    inline PFN_vkCmdDebugMarkerInsertEXT vkCmdDebugMarkerInsert = VK_NULL_HANDLE;

namespace lyra
{
class VulkanSwapchain;
class VulkanInstance;

class VulkanDevice
{
public:
    typedef struct
    {
        VulkanInstance& mVulkanInstance;
    }InitInfo;	
	
    VulkanDevice(InitInfo _initInfo);
    ~VulkanDevice();

    void InitializeSynchronizationObjects(size_t _numSynchronizationObjects);
	
    void WaitForDevice() const;

    VkPhysicalDevice GetVkPhysicalDevice() const { return mPhysicalDevice; }
    VkDevice GetVkDevice() const { return mDevice; }
	
    static Vector<VkPhysicalDevice> EnumeratePhysicalDevices(VulkanInstance& _instance);
    static VkPhysicalDevice GetMostSuitedPhysicalDevice(VulkanInstance& _instance);

    VkFormat FindSupportedImageFormat(const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    void BeginFrame(VulkanSwapchain& _swapchain);
    void OnEndFrame(VulkanSwapchain& _swapchain);
    void PresentFrame(VulkanSwapchain& swapchain);

    VulkanQueues& GetQueues() { return *mQueues; }

    size_t GetCurrentBufferIndex() const { return mCurrentSynchronizationObject; };

    VulkanBuffer CreateVulkanBuffer(unsigned long long _size, int _usageBits, VkMemoryPropertyFlags _memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VulkanBuffer CreateVulkanBuffer(void* _data, unsigned long long _size, int _usageBits, VkMemoryPropertyFlags _memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    static bool DebugMarkersAvailable() { return vkCmdDebugMarkerBegin != VK_NULL_HANDLE; }

    VkSemaphore GetNextFrameImageAcquiredSemaphore() const { return mImageAvailableSemaphores[mCurrentSynchronizationObject]; }
    VkSemaphore GetNextFrameRenderedSemaphore() const { return mRenderFinishedSemaphores[mCurrentSynchronizationObject]; }
    VkFence GetNextFrameReadyFence() const { return mInflightFences[mCurrentSynchronizationObject]; } 
private:
	void DestroySynchronizationObjects();
	void CreateLogicalDevice(VulkanInstance& _vulkanInstance);
	static int RateDeviceSuitability(VkPhysicalDevice& device, VulkanInstance& _vulkanInstance);

	VkPhysicalDevice mPhysicalDevice = nullptr;
	VkDevice mDevice = nullptr;

	unsigned int mCurrentAcquiredImageIndex = 0;
	size_t mNumSynchronizationObjects = 0;
	size_t mCurrentSynchronizationObject = 0;
	
	Vector<VkSemaphore> mImageAvailableSemaphores;
	Vector<VkSemaphore> mRenderFinishedSemaphores;
	Vector<VkFence> mInflightFences;

	UniquePointer<VulkanQueues> mQueues;

};

}
