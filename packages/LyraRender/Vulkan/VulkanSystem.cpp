#include <Vulkan/VulkanSystem.h>

#include <Profiler/Profiler.h>
#include <Vulkan/VulkanCommandBufferManager.h>
#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanInstance.h>
#include <Vulkan/VulkanSwapchain.h>
#include <Vulkan/VulkanRenderPassManager.h>

#include "VulkanShaderModuleManager.h"
#include "VulkanTextureManager.h"

// Created on 2020-08-06 by Sisco

namespace lyra
{
    VulkanSystem::VulkanSystem(InitInfo _initInfo)
    {
        ProfileScoped;

        VulkanInstance::InitInfo instanceInitInfo
        {
            _initInfo.m_ApplicationWindow   
        };
        m_vulkanInstance = MakeUniquePointer<VulkanInstance>(instanceInitInfo);

        VulkanDevice::InitInfo deviceInitInfo
        {
            *m_vulkanInstance
        };
        m_vulkanDevice = MakeUniquePointer<VulkanDevice>(deviceInitInfo);

        VulkanSwapchain::InitInfo swapchanInitInfo
        {
            *m_vulkanDevice,
            *m_vulkanInstance
        };
        m_swapchain = MakeUniquePointer<VulkanSwapchain>(swapchanInitInfo);

        m_vulkanDevice->InitializeSynchronizationObjects(m_swapchain->GetNumSwapchainBuffers());

        VulkanCommandBufferManager::InitInfo commandBufferManagerInitInfo
        {
            *m_vulkanDevice,
            m_swapchain->GetNumSwapchainBuffers()
        };
        
        m_commandBufferManager = MakeUniquePointer<VulkanCommandBufferManager>(commandBufferManagerInitInfo);

        m_commandBufferManager->CreateCommandPool(
            VulkanCommandBufferManager::TransferCommandPoolName(),
            VulkanQueues::FamilyType::Transfer,
            static_cast<int>(VulkanCommandBufferManager::PoolFlags::TransientCommandBuffers) );

        m_commandBufferManager->CreateCommandPool(VulkanCommandBufferManager::GraphicsCommandPoolName(),
            VulkanQueues::FamilyType::Graphics,
            static_cast<int>(VulkanCommandBufferManager::PoolFlags::ResettableCommandBuffers));

        VulkanTextureManager::InitInfo textureManagerInitInfo
        {
            *m_vulkanDevice,
            m_commandBufferManager->GetVkCommandPool(VulkanCommandBufferManager::TransferCommandPoolName())
            
        };
        m_textureManager = MakeUniquePointer<VulkanTextureManager>(textureManagerInitInfo);

        m_renderPassManager = MakeUniquePointer<VulkanRenderPassManager>(*this);

        m_shaderModuleManager = MakeUniquePointer<VulkanShaderModuleManager>(m_vulkanDevice->GetVkDevice());
    }    

    VulkanSystem::~VulkanSystem()
    {
        m_vulkanDevice->WaitForDevice();
        m_shaderModuleManager = nullptr;
        m_renderPassManager = nullptr;
        m_commandBufferManager = nullptr;
        m_swapchain = nullptr;
        m_vulkanDevice = nullptr;
        m_vulkanInstance = nullptr;
    }

    void VulkanSystem::OnResize(glm::vec2 _size)
    {
        ProfileScoped;

        m_vulkanDevice->WaitForDevice();

        // Recreate fresh swapchain
        m_swapchain = nullptr;

        VulkanSwapchain::InitInfo swapchanInitInfo
        {
            *m_vulkanDevice,
            *m_vulkanInstance
        };
        m_swapchain = MakeUniquePointer<VulkanSwapchain>(swapchanInitInfo);
        
        m_vulkanDevice->InitializeSynchronizationObjects(m_swapchain->GetNumSwapchainBuffers());

        m_renderPassManager->RecreatePipelines();

    }

    void VulkanSystem::BeginFrame()
    {
        ProfileScoped;

        m_commandBufferManager->SetCurrentBufferIndex(static_cast<unsigned>(m_vulkanDevice->GetCurrentBufferIndex()));
        m_renderPassManager->SetCurrentBufferIndex(static_cast<unsigned>(m_vulkanDevice->GetCurrentBufferIndex()));

        // Frame index synchronization point
        m_vulkanDevice->BeginFrame(*m_swapchain);
    }

    void VulkanSystem::EndFrame()
    {
        ProfileScoped;
    }

    void VulkanSystem::Present()
    {
        ProfileScoped;
        m_vulkanDevice->PresentFrame(*m_swapchain);
    }
}

