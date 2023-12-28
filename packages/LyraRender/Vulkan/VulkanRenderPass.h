
#pragma once
#include <vulkan/vulkan_core.h>
#include <Containers/Vector.h>

// Created on 2023-12-27 by sisco

namespace lyra
{
    class VulkanSwapchain;
    class VulkanCommandBuffer;
}

namespace lyra
{
class VulkanSystem;
class RenderList;   
    
class VulkanRenderPass
{
public:
    virtual ~VulkanRenderPass() = default;
    virtual VkRenderPass GetVkRenderPass() const { return m_vkRenderPass; }
    virtual void RecreatePipelines() = 0;

    virtual void BuildCommandBuffer(const RenderList& _renderList, const VulkanCommandBuffer& _commandBuffer) = 0;
    
    void SetCurrentBufferIndex(unsigned int _index) { m_currentBuffer = _index; }
protected:
    virtual void CreateRenderPass(VulkanSwapchain& _vulkanSwapchain) = 0;
    virtual void CreateFramebuffers(VulkanSwapchain& _vulkanSwapchain) = 0;

    VkFramebuffer GetCurrentFramebuffer() const { return m_framebuffers[m_currentBuffer]; }
    
    VkRenderPass m_vkRenderPass = nullptr;

    VkExtent2D m_swapchainExtents {};
    Vector<VkFramebuffer> m_framebuffers;

    unsigned int m_currentBuffer = 0;
    
    
};

}

