
#include <Vulkan/VulkanCommandBufferManager.h>
// Created on 2020-05-21 by sisco

#include <ranges>
#include <Vulkan/VulkanDevice.h>
#include <Core/CoreDefines.h>

namespace lyra
{
    VulkanCommandBufferManager::VulkanCommandBufferManager(InitInfo _initInfo) :
    m_VulkanQueues(_initInfo.mVulkanDevice.GetQueues())
    {
        mDevice = _initInfo.mVulkanDevice.GetVkDevice();
        m_PhysicalDevice = _initInfo.mVulkanDevice.GetVkPhysicalDevice();
        m_MultiBufferedBufferCount = _initInfo.mMultiBufferedCount;
        m_CurrentIndex = 0;
    }

    VulkanCommandBufferManager::~VulkanCommandBufferManager()
    {
        for (auto& commandBuffer : m_CommandBuffers | std::views::values)
        {
            commandBuffer->Destroy();
        }
        m_CommandBuffers.clear();
        
        for (auto& commandPool : m_CommandPools)
        {
            vkDestroyCommandPool(mDevice, commandPool.second, nullptr);
        }
        m_CommandPools.clear();
    }

    VulkanCommandBuffer& VulkanCommandBufferManager::CreateMultiBufferedCommandBuffer(StringHash _commandPoolName, StringHash _commandBufferName, bool _useProfiler, VkQueue _profileQueue, const char* _profilingName)
    {
        lyraAssert(m_CommandBuffers.find(_commandBufferName) == m_CommandBuffers.end());
        auto commandBuffer = MakeUniquePointer<VulkanCommandBuffer>(
            VulkanCommandBuffer::InitInfo{mDevice, m_CommandPools[_commandPoolName], m_MultiBufferedBufferCount, _useProfiler, m_PhysicalDevice, _profileQueue, _profilingName}
            );
        m_CommandBuffers.emplace(_commandBufferName,std::move(commandBuffer));
        return *m_CommandBuffers[_commandBufferName];
    }

    VulkanCommandBuffer& VulkanCommandBufferManager::CreateSingleBufferedCommandBuffer(StringHash _commandPoolName, StringHash _commandBufferName, bool _useProfiler, VkQueue _profileQueue, const char* _profilingName)
    {
        lyraAssert(m_CommandBuffers.find(_commandBufferName) == m_CommandBuffers.end());
        auto commandBuffer = MakeUniquePointer<VulkanCommandBuffer>(
            VulkanCommandBuffer::InitInfo{mDevice, m_CommandPools[_commandPoolName], 1, _useProfiler, m_PhysicalDevice, _profileQueue, _profilingName}
            );
        m_CommandBuffers.emplace(_commandBufferName,std::move(commandBuffer));
        return *m_CommandBuffers[_commandBufferName];
    }

    void VulkanCommandBufferManager::DestroyCommandBuffer(StringHash _commandBufferName)
    {
        lyraAssert(m_CommandBuffers.find(_commandBufferName) != m_CommandBuffers.end());
        m_CommandBuffers[_commandBufferName]->Destroy();
        m_CommandBuffers.erase(_commandBufferName);
    }

    const VulkanCommandBuffer& VulkanCommandBufferManager::GetCommandBuffer(StringHash _commandBufferName)
    {
        lyraAssert(m_CommandBuffers.find(_commandBufferName) != m_CommandBuffers.end());
        return *m_CommandBuffers[_commandBufferName];
    }

    void VulkanCommandBufferManager::SetCurrentBufferIndex(unsigned int _currentIndex)
    {
        m_CurrentIndex = _currentIndex;
        for (auto& [key, commandBuffer] : m_CommandBuffers)
        {
            commandBuffer->SetCurrentBuffer(_currentIndex);
        }
    }
    void VulkanCommandBufferManager::CreateCommandPool(StringHash _commandPoolName,  VulkanQueues::FamilyType _type, int _commandPoolFlags)
    {

        VkCommandPool commandPool;

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags =
            ((_commandPoolFlags & static_cast<int>(PoolFlags::TransientCommandBuffers)) ? VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : 0) |
            ((_commandPoolFlags & static_cast<int>(PoolFlags::ResettableCommandBuffers)) ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0);

        switch (_type)
        {
        case VulkanQueues::FamilyType::Graphics: poolInfo.queueFamilyIndex = m_VulkanQueues.GetGraphicsFamilyIndex(); break;
        case VulkanQueues::FamilyType::Present: poolInfo.queueFamilyIndex = m_VulkanQueues.GetPresentFamilyIndex(); break;
        case VulkanQueues::FamilyType::Transfer: poolInfo.queueFamilyIndex = m_VulkanQueues.GetTransferFamilyIndex(); break;
        case VulkanQueues::FamilyType::Compute: poolInfo.queueFamilyIndex = m_VulkanQueues.GetComputeFamilyIndex(); break;
        default: ;
        }
        
        if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            lyraAssert(0 && "failed to create command pool!");

        m_CommandPools[_commandPoolName] = commandPool;
    }

    void VulkanCommandBufferManager::DestroyCommandPool(StringHash _commandPoolName)
    {
        lyraAssert(m_CommandPools.find(_commandPoolName) == m_CommandPools.end());
        vkDestroyCommandPool(mDevice, m_CommandPools[_commandPoolName], nullptr);
        m_CommandPools.erase(_commandPoolName);
    }

    VkCommandPool VulkanCommandBufferManager::GetVkCommandPool(StringHash _commandPoolName)
    {
        lyraAssert(m_CommandPools.find(_commandPoolName) != m_CommandPools.end());
        return m_CommandPools[_commandPoolName];
    }
}

