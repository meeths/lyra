
#pragma once
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <Memory/UniquePointer.h>

namespace lyra
{
    class VulkanShaderModuleManager;
}

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

    void OnResize(glm::vec2 _size);

    void BeginFrame();
    void EndFrame();
    void Present();

    VulkanDevice& GetVulkanDevice() { return *m_vulkanDevice; } 
    VulkanCommandBufferManager& GetVulkanCommandBufferManager() { return *m_commandBufferManager; }
    VulkanSwapchain& GetVulkanSwapchain() { return *m_swapchain; }
    VulkanTextureManager& GetVulkanTextureManager() { return *m_textureManager; }
    VulkanRenderPassManager& GetVulkanRenderPassManager() { return *m_renderPassManager; }
    VulkanShaderModuleManager& GetVulkanShaderModuleManager() { return *m_shaderModuleManager; }

    static glm::mat4x4 FixProjectionMatrix(const glm::mat4x4 & _matrix)
    {
        static const glm::mat4x4 clip
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
    UniquePointer<VulkanShaderModuleManager> m_shaderModuleManager;
};

}

