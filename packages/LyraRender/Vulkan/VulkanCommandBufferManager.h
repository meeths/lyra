#pragma once

// Created on 2020-05-21 by sisco
#include <String/StringHash.h>
#include <Vulkan/VulkanCommandBuffer.h>
#include <Vulkan/VulkanQueue.h>
#include <Containers/UnorderedMap.h>

namespace lyra
{
    class VulkanDevice;

    class VulkanCommandBufferManager
    {
    public:
        enum class PoolFlags : int
        {
            StaticCommandBuffers = 1 << 0,
            TransientCommandBuffers = 1 << 1,
            ResettableCommandBuffers = 1 << 2
        };

        typedef struct InitInfo
        {
            VulkanDevice& mVulkanDevice;
            unsigned int mMultiBufferedCount = 0;
        } InitInfo;

        VulkanCommandBufferManager(InitInfo _initInfo);
        ~VulkanCommandBufferManager();

        void CreateCommandPool(StringHash _commandPoolName, VulkanQueues::FamilyType _type, int _commandPoolFlags);
        void DestroyCommandPool(StringHash _commandPoolName);
        VkCommandPool GetVkCommandPool(StringHash _commandPoolName);

        VulkanCommandBuffer& CreateMultiBufferedCommandBuffer(StringHash _commandPoolName, StringHash _commandBufferName, bool _useProfiler = false, VkQueue _profileQueue = nullptr, const char* _profilingName = nullptr);
        VulkanCommandBuffer& CreateSingleBufferedCommandBuffer(StringHash _commandPoolName, StringHash _commandBufferName, bool _useProfiler = false, VkQueue _profileQueue = nullptr, const char* _profilingName = nullptr);

        void DestroyCommandBuffer(StringHash _commandBufferName);

        const VulkanCommandBuffer& GetCommandBuffer(StringHash _commandBufferName);

        void SetCurrentBufferIndex(unsigned int _currentIndex);

        DefineGlobalConstexprVariableAccessor(StringView, GraphicsCommandPoolName, "GraphicsCommandPool")
        DefineGlobalConstexprVariableAccessor(StringView, TransferCommandPoolName, "TransferCommandPool")

    protected:

        VkDevice mDevice = nullptr;
        VkPhysicalDevice m_PhysicalDevice = nullptr;

        VulkanQueues& m_VulkanQueues;

        UnorderedMap<StringHash, UniquePointer<VulkanCommandBuffer>> m_CommandBuffers;
        UnorderedMap<StringHash, VkCommandPool> m_CommandPools;

        unsigned int m_MultiBufferedBufferCount = 0;
        unsigned int m_CurrentIndex = 0;
    };
}
