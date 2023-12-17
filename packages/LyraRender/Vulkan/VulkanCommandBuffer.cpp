
#include <Vulkan/VulkanCommandBuffer.h>
#include <Vulkan/VulkanQueue.h>
#include <String/StringUtils.h>
// Created on 2018-12-27 by sisco

namespace lyra
{
    VulkanCommandBuffer::VulkanCommandBuffer(InitInfo _initInfo):
        mInitInfo(std::move(_initInfo))
    {
        mCommandBuffers.resize(mInitInfo.mBufferCount);
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = mInitInfo.mCommandPool;
        allocInfo.commandBufferCount = static_cast<uint32_t>(mInitInfo.mBufferCount);

        vkAllocateCommandBuffers(mInitInfo.mDevice, &allocInfo, mCommandBuffers.data());

        if(_initInfo.mProfilingEnabled)
        {
            lyraAssert(_initInfo.mQueueToProfile);
            for(unsigned int cmdBufferIndex = 0; cmdBufferIndex < mCommandBuffers.size(); ++cmdBufferIndex )
            {
                auto tracyContext = TracyVkContext(mInitInfo.mPhysicalDevice, mInitInfo.mDevice, mInitInfo.mQueueToProfile, mCommandBuffers[cmdBufferIndex]);
                if(mInitInfo.mProfilingSectionName)
                {
                    auto contextName = StringUtils::StringFormat("%s(%d)", mInitInfo.mProfilingSectionName, cmdBufferIndex);
                    TracyVkContextName(tracyContext, contextName.c_str(), (int)contextName.size());
                }
                mTracyContexts.push_back(tracyContext);
            }
        }
    }

    void VulkanCommandBuffer::BeginRecording(bool _reusableRecording) const
    {
        const auto commandBuffer = GetVkCommandBuffer();

        
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = _reusableRecording ? VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;


        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			lyraAssert(0 && "failed to begin recording command buffer!");

        if(mInitInfo.mProfilingEnabled)
        {
            TracyVkCollect(GetTracyContext(), commandBuffer);
        }
    }

    void VulkanCommandBuffer::EndRecording() const
    {
        if(vkEndCommandBuffer(GetVkCommandBuffer()) != VK_SUCCESS)
			lyraAssert(0 && "failed to end recording command buffer!");

    }

    void VulkanCommandBuffer::Execute(const VulkanQueue& _queue, const Vector<VkSemaphore>& _signalSemaphores, const Vector<VkSemaphore>& _waitSemaphores, VkPipelineStageFlags _waitStage, VkFence _fence, bool _waitForQueueIdle) const
    {
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &mCommandBuffers[mCurrentBuffer];

        submitInfo.signalSemaphoreCount = static_cast<uint32_t>(_signalSemaphores.size());
        if(submitInfo.signalSemaphoreCount > 0)
            submitInfo.pSignalSemaphores = _signalSemaphores.data();

        submitInfo.waitSemaphoreCount = static_cast<uint32_t>(_waitSemaphores.size());
        if(submitInfo.waitSemaphoreCount > 0)
        {
            submitInfo.pWaitSemaphores = _waitSemaphores.data();
            submitInfo.pWaitDstStageMask = &_waitStage;
        }


        _queue.SubmitQueue(1, &submitInfo, _fence, _waitForQueueIdle);
        
    }

	void VulkanCommandBuffer::Destroy()
    {
        vkFreeCommandBuffers(mInitInfo.mDevice, mInitInfo.mCommandPool, static_cast<unsigned>(mCommandBuffers.size()), mCommandBuffers.data());
        if(mInitInfo.mProfilingEnabled)
        {
            for (auto& tracyContext : mTracyContexts)
                TracyVkDestroy(tracyContext);
        }
        mTracyContexts.clear();
            
    }

}

