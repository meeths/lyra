#pragma once

// Comment this line to automatically check results within vulkan.hpp
#define VULKAN_HPP_ASSERT_ON_RESULT(x) ignore(x)

#include <vulkan/vulkan.hpp>
#include <Renderer/Vulkan/VulkanDeviceQueue.h>
#include <Renderer/IDevice.h>

// Created on 2024-11-06 by sisco

namespace lyra
{

class VulkanDevice : public IDevice
{
public:
    ~VulkanDevice() override;
    bool Initialize(const InitInfoBase& desc) override;

    const DeviceInfo& GetDeviceInfo() const override;

    Expected<SharedPointer<ISurface>, StringView> CreateSurface(WindowHandle windowHandle) override;
    Expected<SharedPointer<BufferResource>, StringView> CreateBufferResource(const BufferResource::Descriptor& desc, const void* initData) override;
    Expected<SharedPointer<ITexture>, StringView> CreateTextureResource(ITexture::Descriptor& desc) override;

    void WaitForGPU() override;

private:

    bool InitializeInstanceAndDevice(const InitInfoBase& desc);


private:
    DeviceInfo m_deviceInfo = {};

    vk::Instance m_vkInstance;
    vk::PhysicalDevice m_vkPhysicalDevice;
    vk::Device m_vkDevice;

    VulkanDeviceQueue m_vulkanDeviceQueue;
    
    vk::Sampler m_vkDefaultSampler;
    
    vk::DebugUtilsMessengerEXT m_vkDebugMessenger;
};

}

