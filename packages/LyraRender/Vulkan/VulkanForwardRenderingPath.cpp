
#include <Vulkan/VulkanForwardRenderingPath.h>
#include <String/StringHash.h>
#include <Vulkan/VulkanCommandBufferManager.h>
#include <Vulkan/VulkanSystem.h>
#include <RenderList/RenderList.h>
#include <Vulkan/VulkanDevice.h>

#include "VulkanRenderPassManager.h"

// Created on 2023-12-28 by sisco

namespace
{
    
    //lyra::ConstStringHash g_debugPassName = "DebugRenderPass"; 
    lyra::ConstStringHash g_debugPassCommandBufferName = "DebugPassCommandBuffer"; 
}

namespace lyra
{
    VulkanForwardRenderingPath::VulkanForwardRenderingPath(const InitInfo& _initInfo) : m_vulkanSystem(_initInfo.m_vulkanSystem)
    {
        
    }

    void VulkanForwardRenderingPath::Configure()
    {
        auto& commandBufferManager = m_vulkanSystem.GetVulkanCommandBufferManager();

        m_debugRenderPass = MakeUniquePointer<VulkanDebugRenderPass>(m_vulkanSystem);
        
        commandBufferManager.CreateMultiBufferedCommandBuffer(VulkanCommandBufferManager::GraphicsCommandPoolName(),
            g_debugPassCommandBufferName);
        
    }

    void VulkanForwardRenderingPath::Execute()
    {
        auto& commandBufferManager = m_vulkanSystem.GetVulkanCommandBufferManager();
        auto& currentCommandBuffer = commandBufferManager.GetCommandBuffer(g_debugPassCommandBufferName);
        auto& graphicsQueue = m_vulkanSystem.GetVulkanDevice().GetQueues().GetGraphicsQueue();

        currentCommandBuffer.BeginRecording();
        
        m_debugRenderPass->BuildCommandBuffer({}, currentCommandBuffer);
        
        currentCommandBuffer.EndRecording();
        currentCommandBuffer.Execute(graphicsQueue,
            {m_vulkanSystem.GetVulkanDevice().GetRenderFinishedSemaphore()},
            {m_vulkanSystem.GetVulkanDevice().GetRenderBeginAvailableSemaphore()},
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, m_vulkanSystem.GetVulkanDevice().GetNextFrameReadyFence(), false);
    }
}

