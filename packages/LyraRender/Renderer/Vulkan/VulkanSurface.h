
#pragma once
#include <vulkan/vulkan.hpp>
#include <Renderer/Vulkan/VulkanCommandQueue.h>
#include <Renderer/Vulkan/VulkanTexture.h>
#include <Containers/Vector.h>
#include <Application/WindowHandle.h>
#include <Renderer/ISurface.h>

// Created on 2024-11-12 by sisco

namespace lyra
{
class VulkanDevice;

class VulkanSurface : public ISurface
{
public:
    ~VulkanSurface() override;
    bool Initialize(vk::Device* device, vk::PhysicalDevice* physicalDevice, vk::Instance* instance, WindowHandle windowHandle, Format preferredFormat);
    bool CreateSwapchain();
    void DestroySwapchain();
    
    const SurfaceInfo& GetSurfaceInfo() override { return m_info; }
    const SurfaceConfig& GetSurfaceConfig() override { return m_config; }
    bool Configure(SurfaceConfig config) override;
    ITexture* GetTexture() override;
    bool Present() override;

private:

    SurfaceInfo m_info {};
    SurfaceConfig m_config = {};
    
    vk::Device* m_vkDevice = nullptr;
    vk::PhysicalDevice* m_vkPhysicalDevice = nullptr;
    vk::Instance* m_vkInstance = nullptr;
    vk::SwapchainKHR m_vkSwapchain;
    vk::SurfaceKHR m_vkSurface;
    vk::Semaphore m_vkNextImageAcquireSemaphore;
    vk::Format m_vkFormat = {};
    VulkanCommandQueue m_commandQueue;
    Vector<VulkanTexture> m_images;
    size_t m_currentImageIndex = 0;
    WindowHandle m_windowHandle;

};

}

