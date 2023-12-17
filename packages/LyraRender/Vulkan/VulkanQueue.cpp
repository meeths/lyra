#include <Vulkan/VulkanQueue.h>
#include <Core/CoreDefines.h>
#include <Profiler/Profiler.h>

namespace lyra
{
void VulkanQueue::SubmitQueue(unsigned _infoCount, const VkSubmitInfo* _submitInfo, VkFence _fence, bool _waitForIdle) const
{
    ProfileScoped;
    
    lyraAssert(m_Queue && "Trying to submit null Vulkan Queue");
    if(m_Queue == nullptr)
        return;
    
    if (vkQueueSubmit(m_Queue, _infoCount, _submitInfo, _fence) != VK_SUCCESS)
        lyraAssert(0 && "failed to submit queue");

    if(_waitForIdle)
        WaitForQueueIdle();
}

void VulkanQueue::WaitForQueueIdle() const
{
    ProfileScoped;
    vkQueueWaitIdle(m_Queue);
}

VulkanQueues::VulkanQueues(InitInfo _initInfo)
{
    m_FamilyIndices = _initInfo.m_QueueFamilyIndices;
    VkQueue queue;
    vkGetDeviceQueue(_initInfo.m_Device, m_FamilyIndices.m_GraphicsFamily, 0, &queue);
    m_GraphicsQueueIndex = FindOrAddQueue(queue);
    vkGetDeviceQueue(_initInfo.m_Device, m_FamilyIndices.m_PresentFamily, 0, &queue);
    m_PresentQueueIndex = FindOrAddQueue(queue);
    vkGetDeviceQueue(_initInfo.m_Device, m_FamilyIndices.m_TransferFamily, 0, &queue);
    m_TransferQueueIndex = FindOrAddQueue(queue);
    if(m_FamilyIndices.m_ComputeFamily != -1)
    {
        vkGetDeviceQueue(_initInfo.m_Device, m_FamilyIndices.m_ComputeFamily, 0, &queue);
        m_ComputeQueueIndex = FindOrAddQueue(queue);
    }
        
}

unsigned int VulkanQueues::FindOrAddQueue(VkQueue _queue)
{
    unsigned int queueIndex = 0;
    for (const auto& queue : m_Queues)
    {
        if(queue->m_Queue == _queue)
            return queueIndex;
        ++queueIndex;
    }
    
    m_Queues.push_back(MakeUniquePointer<VulkanQueue>(_queue));
    
    return static_cast<unsigned int>(m_Queues.size())-1;
}
}
