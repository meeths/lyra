
#include <Vulkan/VulkanDebugRenderPass.h>
#include <Vulkan/VulkanSystem.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanTexture.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanTextureManager.h>

// Created on 2023-12-27 by sisco

namespace
{
    lyra::ConstStringHash g_depthAttachmentName = "DebugRenderPassDepthBuffer";
}

namespace lyra
{
    VulkanDebugRenderPass::VulkanDebugRenderPass(VulkanSystem& _vulkanSystem) : m_vulkanSystem(_vulkanSystem)
    {
        auto& textureManager = m_vulkanSystem.GetVulkanTextureManager();
        auto& vulkanDevice = m_vulkanSystem.GetVulkanDevice();
        auto& vulkanSwapchain = m_vulkanSystem.GetVulkanSwapchain();

        m_swapchainExtents = vulkanSwapchain.GetExtents();
        
        m_depthFormat = vulkanDevice.FindSupportedImageFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

        {
            const VulkanTexture::CreationInfo depthAttachmentTextureInfo
            {
                        {static_cast<int>(m_swapchainExtents.width), static_cast<int>(m_swapchainExtents.height)},
                        1,
                        static_cast<unsigned>(vulkanSwapchain.GetMSAASamples()),
                        TextureType::DepthAttachmentInternal,
                        m_depthFormat,
                        VK_IMAGE_TILING_OPTIMAL
                   };

            if(textureManager.HasTexture(StringHash(g_depthAttachmentName)))
                textureManager.DestroyTexture(StringHash(g_depthAttachmentName));

            m_depthTexture = &textureManager.CreateTexture(StringHash(g_depthAttachmentName), depthAttachmentTextureInfo);
            
        }

        CreateRenderPass(vulkanSwapchain);
        CreateFramebuffers(vulkanSwapchain);
    }

    VulkanDebugRenderPass::~VulkanDebugRenderPass()
    {
        auto& textureManager = m_vulkanSystem.GetVulkanTextureManager();
        textureManager.DestroyTexture(StringHash(g_depthAttachmentName));
        vkDestroyRenderPass(m_vulkanSystem.GetVulkanDevice().GetVkDevice(), m_vkRenderPass, nullptr);
        for (auto& frameBuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(m_vulkanSystem.GetVulkanDevice().GetVkDevice(), frameBuffer, nullptr);
        }
    }

    void VulkanDebugRenderPass::RecreatePipelines()
    {
    }

    void VulkanDebugRenderPass::BuildCommandBuffer(const RenderList& _renderList, const VulkanCommandBuffer& _commandBuffer)
    {
        
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_vkRenderPass;
        renderPassInfo.framebuffer = GetCurrentFramebuffer();
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_swapchainExtents;

        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = { 0.22f, 0.22f, 0.55f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(_commandBuffer.GetVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdEndRenderPass(_commandBuffer.GetVkCommandBuffer());
        
    }

    void VulkanDebugRenderPass::CreateRenderPass(VulkanSwapchain& _vulkanSwapchain)
    {
        // Color attachment
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = _vulkanSwapchain.GetColorFormat();
        colorAttachment.samples = _vulkanSwapchain.GetMSAASamples();
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef;
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // Depth attachment
        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = m_depthFormat;
        depthAttachment.samples = _vulkanSwapchain.GetMSAASamples();
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;     // Use this if this pass owns this depth buffer
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef = {};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        /////////////////////////////////////////////////////////////////////
        // Subpass setup
     
        Vector<VkSubpassDescription> subPasses;
        Vector<VkSubpassDependency> subPassDependencies;
        
        {
            // Resolve
            subPasses.push_back({});
            auto& subpass = subPasses.back();
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pDepthStencilAttachment = &depthAttachmentRef;

            // Dependency
            subPassDependencies.push_back({});
            auto& dependency = subPassDependencies.back();

            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dependency.dependencyFlags = {};
        }

        // Render pass attachments
        Vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };
        
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = static_cast<unsigned>(subPasses.size());
        renderPassInfo.pSubpasses = subPasses.data();

        renderPassInfo.dependencyCount = static_cast<unsigned>(subPassDependencies.size());
        renderPassInfo.pDependencies = subPassDependencies.data();

        if (vkCreateRenderPass(m_vulkanSystem.GetVulkanDevice().GetVkDevice(), &renderPassInfo, nullptr, &m_vkRenderPass) != VK_SUCCESS)
            lyraAssert(0 && "failed to create render pass!");
    }

    void VulkanDebugRenderPass::CreateFramebuffers(VulkanSwapchain& _vulkanSwapchain)
    {
        m_framebuffers.resize(_vulkanSwapchain.GetNumSwapchainBuffers());

        for (size_t i = 0; i < _vulkanSwapchain.GetNumSwapchainBuffers(); i++)
        {
            Vector<VkImageView> attachments;
            attachments.push_back(_vulkanSwapchain.GetVkImageView(static_cast<int>(i)));
            attachments.push_back(m_depthTexture->GetVkImageView());
            
            VkFramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_vkRenderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = m_swapchainExtents.width;
            framebufferInfo.height = m_swapchainExtents.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_vulkanSystem.GetVulkanDevice().GetVkDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
                lyraAssert(0 && "failed to create framebuffer!");
        }
    }
}

