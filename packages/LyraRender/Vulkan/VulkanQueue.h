#pragma once
#include <Vulkan/VulkanQueueFamily.h>
#include <Threading/SRWLock.h>
#include <Containers/Vector.h>
#include <Memory/UniquePointer.h>

namespace lyra
{
class VulkanQueue
{
public:
    friend class VulkanQueues;
    friend class VulkanDevice;
    explicit VulkanQueue(VkQueue _queue) : m_Queue(_queue) {}
    void SubmitQueue(unsigned int _infoCount, const VkSubmitInfo* _submitInfo, VkFence _fence, bool _waitForIdle) const;
    void WaitForQueueIdle() const;
    VkQueue GetVulkanQueue() const { return m_Queue; }
private:
    VkQueue m_Queue = nullptr;
};
    
class VulkanQueues
{
public:
    enum class FamilyType : int
    {
        Graphics,
        Present,
        Transfer,
        Compute
    };

    typedef struct
    {
        VkDevice& m_Device;
        const VulkanQueueFamily::QueueFamilyIndices& m_QueueFamilyIndices;
    }InitInfo;

    
    VulkanQueues(InitInfo _initInfo);

    uint32_t GetGraphicsFamilyIndex() const { return m_FamilyIndices.m_GraphicsFamily; }
    uint32_t GetPresentFamilyIndex() const { return m_FamilyIndices.m_PresentFamily; }
    uint32_t GetTransferFamilyIndex() const { return m_FamilyIndices.m_TransferFamily; }
    uint32_t GetComputeFamilyIndex() const { return m_FamilyIndices.m_ComputeFamily; }

    const VulkanQueue& GetGraphicsQueue() const { return *m_Queues[m_GraphicsQueueIndex]; }
    const VulkanQueue& GetPresentQueue() const { return *m_Queues[m_PresentQueueIndex]; }
    const VulkanQueue& GetTransferQueue() const { return *m_Queues[m_TransferQueueIndex]; }
    const VulkanQueue& GetComputeQueue() const { lyraAssert(m_ComputeQueueIndex != -1); return *m_Queues[m_ComputeQueueIndex]; }

private:
    unsigned int FindOrAddQueue(VkQueue _queue);
    VulkanQueueFamily::QueueFamilyIndices m_FamilyIndices;

    Vector<UniquePointer<VulkanQueue>> m_Queues;
    unsigned int m_GraphicsQueueIndex = -1;
    unsigned int m_PresentQueueIndex = -1;
    unsigned int m_TransferQueueIndex = -1;
    unsigned int m_ComputeQueueIndex = -1;
};

}
