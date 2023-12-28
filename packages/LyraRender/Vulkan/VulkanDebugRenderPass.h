
#pragma once
#include <Vulkan/VulkanRenderPass.h>

// Created on 2023-12-27 by sisco

namespace lyra
{
class VulkanTexture;
class VulkanSwapchain;

class VulkanDebugRenderPass : public VulkanRenderPass
{
public:
    VulkanDebugRenderPass(VulkanSystem& _vulkanSystem);
    ~VulkanDebugRenderPass() override;
    void RecreatePipelines() override;

    void BuildCommandBuffer(const RenderList& _renderList, const VulkanCommandBuffer& _commandBuffer) override;
private:

    void CreateRenderPass(VulkanSwapchain& _vulkanSwapchain) override;
    void CreateFramebuffers(VulkanSwapchain& _vulkanSwapchain) override;

    VulkanSystem& m_vulkanSystem;
    
    VkFormat m_depthFormat {};
    VulkanTexture* m_depthTexture = nullptr;
    VulkanTexture* m_colorTexture = nullptr;
    
    unsigned int m_currentSubPass = 0;
};

}

