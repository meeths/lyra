
#include <Renderer/Vulkan/VulkanSurface.h>
#include <Renderer/Vulkan/VulkanUtils.h>
#include <Containers/UnorderedMap.h>

#include "Containers/Array.h"

// Created on 2024-11-12 by sisco

namespace lyra
{
    VulkanSurface::~VulkanSurface()
    {
        DestroySwapchain();
        m_vkInstance->destroySurfaceKHR(m_vkSurface);
        m_vkDevice->destroySemaphore(m_vkNextImageAcquireSemaphore);
    }

    bool VulkanSurface::Initialize(vk::Device* device, vk::PhysicalDevice* physicalDevice, vk::Instance* instance, WindowHandle windowHandle, Format preferredFormat)
    {
        m_vkDevice = device;
        m_vkPhysicalDevice = physicalDevice;
        m_vkInstance = instance;
        m_windowHandle = windowHandle;

        vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
        auto createSemaphoreResult = m_vkDevice->createSemaphore(semaphoreCreateInfo);
        CHECK_VK_RESULTVALUE(createSemaphoreResult);
        m_vkNextImageAcquireSemaphore = createSemaphoreResult.value;

#ifdef _WIN32
        vk::Win32SurfaceCreateInfoKHR surfaceCreateInfo = {};
        surfaceCreateInfo.hinstance = ::GetModuleHandle(nullptr);
        surfaceCreateInfo.hwnd = windowHandle.ConvertToPlatformWindowHandle<HWND>();
        auto surfaceCreateResults = instance->createWin32SurfaceKHR(surfaceCreateInfo);
        CHECK_VK_RESULTVALUE(surfaceCreateResults);
        m_vkSurface = surfaceCreateResults.value;
       
#else
#error Implement relevant surface creation for this platform
#endif
        auto surfaceFormatsResult = physicalDevice->getSurfaceFormatsKHR(m_vkSurface);
        CHECK_VK_RESULTVALUE(surfaceFormatsResult);
        auto surfaceFormats = surfaceFormatsResult.value;

        for (const auto& vkFormat : surfaceFormats)
        {
            Format format = VulkanUtils::TranslateFromVkFormat(vkFormat.format);
            if (format != Format::Unknown)
                m_info.supportedFormats.push_back(format);
            if (format == preferredFormat)
                m_info.preferredFormat = format;
        }
        if (m_info.preferredFormat == Format::Unknown && !m_info.supportedFormats.empty())
        {
            m_info.preferredFormat = m_info.supportedFormats.front();
        }
        m_info.supportedUsage = TextureUsage::Present | TextureUsage::RenderTarget | TextureUsage::CopyDestination;

        return true;
    }

    bool VulkanSurface::CreateSwapchain()
    {

        VkExtent2D imageExtent = {static_cast<uint32_t>(m_config.size.x), static_cast<uint32_t>(m_config.size.y)};

        auto surfaceCapsResult = m_vkPhysicalDevice->getSurfaceCapabilitiesKHR(m_vkSurface);
        CHECK_VK_RESULTVALUE(surfaceCapsResult);

        auto presentModesResult = m_vkPhysicalDevice->getSurfacePresentModesKHR(m_vkSurface);
        CHECK_VK_RESULTVALUE(presentModesResult);

        static const Vector<vk::PresentModeKHR> kVsyncOffModes {
            vk::PresentModeKHR::eImmediate,
            vk::PresentModeKHR::eMailbox,
            vk::PresentModeKHR::eFifo
        };
        static const Vector<vk::PresentModeKHR> kVsyncOnModes {
            vk::PresentModeKHR::eFifoRelaxed,
            vk::PresentModeKHR::eFifo,
            vk::PresentModeKHR::eImmediate,
            vk::PresentModeKHR::eMailbox
        };
        const Vector<vk::PresentModeKHR>& checkPresentModes = m_config.vsync ? kVsyncOnModes : kVsyncOffModes;
        vk::PresentModeKHR selectedPresentMode{};
        bool presentAvailable = false;
        for (auto checkPresentMode : checkPresentModes)
        {
            if (std::ranges::find(presentModesResult.value, checkPresentMode) != presentModesResult.value.end())
            {
                selectedPresentMode = checkPresentMode;
                presentAvailable = true;
                break;
            }
        }
        if (!presentAvailable)
        {
            lyraLogError("VulkanSurface::CreateSwapchain Present mode unavailable");
            return false;
        }

        vk::Format format = VulkanUtils::TranslateToVkFormat(m_config.format);
        vk::SwapchainCreateInfoKHR swapchainCreateInfo {
            {},
            m_vkSurface,
            m_config.desiredImageCount,
            format,
            vk::ColorSpaceKHR::eSrgbNonlinear,
            imageExtent,
            1,
            VulkanUtils::TranslateToVkImageUsageFlags(m_config.usage),
            vk::SharingMode::eExclusive,
            {},
            {},
            vk::SurfaceTransformFlagBitsKHR::eIdentity,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            selectedPresentMode,
            true,
            {},            
        };

        auto createSwapchainResult = m_vkDevice->createSwapchainKHR(swapchainCreateInfo);
        CHECK_VK_RESULTVALUE(createSwapchainResult);
        m_vkSwapchain = createSwapchainResult.value;

        auto getSwapchainImagesResult = m_vkDevice->getSwapchainImagesKHR(m_vkSwapchain);
        CHECK_VK_RESULTVALUE(getSwapchainImagesResult);
        auto swapchainImages = getSwapchainImagesResult.value;

        for (auto& image : swapchainImages)
        {
            ITexture::Descriptor desc;
            desc.type = IBaseResource::BaseResourceType::Texture;
            desc.arraySize = 0;
            desc.format = m_config.format;
            desc.extents.depth = 1;
            desc.extents.height = m_config.size.y;
            desc.extents.width = m_config.size.x;
            desc.mipLevels = 1;
            
            VulkanTexture texture(desc, m_vkDevice);
            texture.m_vkImage = image;
            texture.m_ownsMemory = false;
            texture.m_vkDeviceMemory = VK_NULL_HANDLE;
            texture.m_vkFormat = m_vkFormat;
            
            m_images.push_back(std::move(texture));
        }
        
        return true;
    }

    void VulkanSurface::DestroySwapchain()
    {
        m_vkDevice->destroySwapchainKHR(m_vkSwapchain);
    }

    bool VulkanSurface::Configure(SurfaceConfig config)
    {
        m_config = config;
        if (m_config.size.x == 0|| m_config.size.y == 0)
        {
            lyraLogError("VulkanSurface::Configure: Invalid width/height");
            return false;
        }
        if (std::ranges::find(m_info.supportedFormats, m_config.format) == m_info.supportedFormats.end())
        {
            lyraLogWarning("VulkanSurface::Configure: unsupported format %s. Using default %s.", ToString(config.format), ToString(m_info.preferredFormat));
            m_config.format = m_info.preferredFormat;
        }
        DestroySwapchain();
        return CreateSwapchain();
    }

    ITexture* VulkanSurface::GetTexture()
    {
        lyraAssert(!m_images.empty() && "VulkanSurface::GetTexture: No images available");
        lyraAssert(m_images.size() > m_currentImageIndex && "VulkanSurface::GetTexture: Incorrect current image index");
        
        return &m_images[m_currentImageIndex];
    }

    bool VulkanSurface::Present()
    {
        lyraAssert(!m_images.empty() && "VulkanSurface::Present: No images available to present");
        
        return false;
    }
}

