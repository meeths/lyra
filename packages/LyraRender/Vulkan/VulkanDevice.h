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
        VulkanInstance& m_VulkanInstance;
    }InitInfo;	
	
    VulkanDevice(InitInfo _initInfo);
    ~VulkanDevice();

    void InitializeSynchronizationObjects(size_t _numSynchronizationObjects);
	
    void WaitForDevice() const;

    VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
    VkDevice GetVkDevice() const { return m_Device; }
	
    static Vector<VkPhysicalDevice> EnumeratePhysicalDevices(VulkanInstance& _instance);
    static VkPhysicalDevice GetMostSuitedPhysicalDevice(VulkanInstance& _instance);

    VkFormat FindSupportedImageFormat(const Vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    void BeginFrame(VulkanSwapchain& _swapchain);
    void OnEndFrame(VulkanSwapchain& _swapchain);
    void PresentFrame(VulkanSwapchain& swapchain);

    VulkanQueues& GetQueues() { return *m_Queues; }

    size_t GetCurrentBufferIndex() const { return m_CurrentSynchronizationObject; };

    VulkanBuffer CreateVulkanBuffer(unsigned long long _size, int _usageBits, VkMemoryPropertyFlags _memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VulkanBuffer CreateVulkanBuffer(void* _data, unsigned long long _size, int _usageBits, VkMemoryPropertyFlags _memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    static bool DebugMarkersAvailable() { return vkCmdDebugMarkerBegin != VK_NULL_HANDLE; }

    VkSemaphore GetRenderBeginAvailableSemaphore() const { return m_ImageAvailableSemaphores[m_CurrentSynchronizationObject]; }
    VkSemaphore GetRenderFinishedSemaphore() const { return m_RenderFinishedSemaphores[m_CurrentSynchronizationObject]; }
    VkFence GetNextFrameReadyFence() const { return m_InflightFences[m_CurrentSynchronizationObject]; } 
private:
	void DestroySynchronizationObjects();
	void CreateLogicalDevice(VulkanInstance& _vulkanInstance);
	static int RateDeviceSuitability(VkPhysicalDevice& device, VulkanInstance& _vulkanInstance);

	VkPhysicalDevice m_PhysicalDevice = nullptr;
	VkDevice m_Device = nullptr;

	unsigned int m_CurrentAcquiredImageIndex = 0;
	size_t m_NumSynchronizationObjects = 0;
	size_t m_CurrentSynchronizationObject = 0;
	
	Vector<VkSemaphore> m_ImageAvailableSemaphores;
	Vector<VkSemaphore> m_RenderFinishedSemaphores;
	Vector<VkFence> m_InflightFences;

	UniquePointer<VulkanQueues> m_Queues;

};

}
