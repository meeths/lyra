
#pragma once
#include <vulkan/vulkan.h>
#include <Vulkan/VulkanQueue.h>
#include <Tracy/TracyVulkan.hpp>
// Created on 2018-12-27 by sisco

namespace lyra
{
class VulkanQueue;

class VulkanCommandBuffer
{
public:

    typedef struct InitInfo
    {
        VkDevice mDevice = nullptr;
        VkCommandPool mCommandPool = nullptr;
        size_t mBufferCount = 0;
        bool mProfilingEnabled = false;
        VkPhysicalDevice mPhysicalDevice = nullptr;
        VkQueue mQueueToProfile = nullptr;
        const char* mProfilingSectionName = nullptr;
    }InitInfo;

    VulkanCommandBuffer() = default;
    VulkanCommandBuffer(InitInfo _initInfo);
    DeclareDefaultMoveable(VulkanCommandBuffer);
    DeclareNonCopyable(VulkanCommandBuffer);
    
    void Destroy();
    
    void BeginRecording(bool _reusableRecording = false) const;
    void EndRecording() const;
    void Execute(const VulkanQueue& _queue, const Vector<VkSemaphore>& _signalSemaphores, const Vector<VkSemaphore>& _waitSemaphores, VkPipelineStageFlags _waitStage, VkFence _fence, bool _waitForQueueIdle) const;
    void SetCurrentBuffer(unsigned int _buffer) { mCurrentBuffer = _buffer % mCommandBuffers.size(); }

    VkCommandBuffer GetVkCommandBuffer() const { return mCommandBuffers[mCurrentBuffer]; };
    TracyVkCtx GetTracyContext() const { return mInitInfo.mProfilingEnabled ? mTracyContexts[mCurrentBuffer] : nullptr; }
private:

    InitInfo mInitInfo;
    Vector<VkCommandBuffer> mCommandBuffers;
    Vector<TracyVkCtx> mTracyContexts;
    unsigned int mCurrentBuffer = 0;
};

class ScopedVulkanCommandBuffer
{
public:
    ScopedVulkanCommandBuffer(VkDevice _device, VkCommandPool _commandPool, const VulkanQueue& _queue) :
        mCommandBuffer({_device, _commandPool, 1}),
        mQueue(_queue)
    {
        mCommandBuffer.BeginRecording();
    }

    ~ScopedVulkanCommandBuffer()
    {
        mCommandBuffer.EndRecording();
        mCommandBuffer.Execute(mQueue, {}, {}, {}, nullptr, true);
        mCommandBuffer.Destroy();   // TODO delay destroy this to avoid waiting

    }

    operator VulkanCommandBuffer&() { return mCommandBuffer; }

    VkCommandBuffer GetVkCommandBuffer() const { return mCommandBuffer.GetVkCommandBuffer(); };

protected:
    const VulkanQueue& mQueue;
    VulkanCommandBuffer mCommandBuffer;
    
};

}

