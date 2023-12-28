
#pragma once
#include <Math/Vector2f.h>
#include <Math/Matrix44.h>
#include <Memory/UniquePointer.h>

// Created on 2020-08-06 by Sisco
namespace lyra
{
class VulkanInstance;
class VulkanSwapchain;
class VulkanTextureManager;
class VulkanDevice;
class VulkanRenderPassManager;
class RenderManager;
class VulkanCommandBufferManager;
    
class ApplicationWindow;
    
class VulkanSystem
{
public:
    
    typedef struct
    {
        ApplicationWindow& m_ApplicationWindow;
    }InitInfo;
    
    VulkanSystem(InitInfo _initInfo);
    ~VulkanSystem();

    void OnResize(Math::Vector2f _size);

    void BeginFrame();
    void EndFrame();
    void Present();

    VulkanDevice& GetVulkanDevice() { return *m_vulkanDevice; } 
    VulkanCommandBufferManager& GetVulkanCommandBufferManager() { return *m_commandBufferManager; }
    VulkanSwapchain& GetVulkanSwapchain() { return *m_swapchain; }
    VulkanTextureManager& GetVulkanTextureManager() { return *m_textureManager; }
    VulkanRenderPassManager& GetVulkanRenderPassManager() { return *m_renderPassManager; }

    static Math::Matrix44 FixProjectionMatrix(const Math::Matrix44 & _matrix)
    {
        static const Math::Matrix44 clip
        {
            1.f,0.f,0.f,0.f,
            0.f,-1.f,0.f,0.f,
            0.f,0.f,0.5f,0.5f,
            0.f,0.f,0.f,1.f
        };
        return _matrix * clip;
    }

private:
    UniquePointer<VulkanInstance> m_vulkanInstance;
    UniquePointer<VulkanDevice> m_vulkanDevice;
    UniquePointer<VulkanSwapchain> m_swapchain;
    UniquePointer<VulkanCommandBufferManager> m_commandBufferManager;
    UniquePointer<VulkanRenderPassManager> m_renderPassManager;
    UniquePointer<VulkanTextureManager> m_textureManager;
};

}

