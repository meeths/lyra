
#include <Vulkan/VulkanRenderPassManager.h>
#include <Vulkan/VulkanSystem.h>
#include <ranges>
#include <Vulkan/VulkanRenderPass.h>
#include <Vulkan/VulkanDebugRenderPass.h>
// Created on 2022-01-17 by sisco

namespace lyra
{
    VulkanRenderPassManager::VulkanRenderPassManager(VulkanSystem& _vulkanSystem)
    {
    }

    VulkanRenderPassManager::~VulkanRenderPassManager()
    {
    }

    void VulkanRenderPassManager::SetCurrentBufferIndex(unsigned _index)
    {
        for (auto& renderPass : m_renderPasses | std::views::values)
        {
            renderPass->SetCurrentBufferIndex(_index);
        }
    }

    void VulkanRenderPassManager::RecreatePipelines()
    {
        // Recreate render passes and associated pipelines
        for (auto& renderPass : m_renderPasses | std::views::values)
        {
            renderPass->RecreatePipelines();
        }
    }

    VulkanRenderPass& VulkanRenderPassManager::GetRenderPass(StringHash _renderPass)
    {
        lyraAssert(m_renderPasses.contains(_renderPass));
        return *m_renderPasses[_renderPass];
    }
    
}

