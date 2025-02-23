
#include <Renderer/Vulkan/VulkanDevice.h>
#include <glm/ext/matrix_transform.hpp>
#include <Log/Log.h>
#include <String/StringUtils.h>
#include <Renderer/Vulkan/VulkanUtils.h>
#include <Renderer/Vulkan/VulkanDeviceProperties.h>
#include <Profiler/Profiler.h>

#include "VulkanSurface.h"
#include "VulkanTextureView.h"

#ifdef _WIN32
#include <dxgi1_2.h>
#endif

#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#error VULKAN_HPP_DISPATCH_LOADER_DYNAMIC is required
#endif

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

// Created on 2024-11-06 by sisco
namespace Details
{
    bool CheckLayers(const lyra::Vector<char const *>& layers,const lyra::Vector<vk::LayerProperties>& properties )
    {
        // return true if all layers are listed in the properties
        return std::ranges::all_of(layers, [&properties]( char const * name )
        {
           bool found = std::ranges::any_of(properties, [&name]( vk::LayerProperties const & property ) { return strcmp( property.layerName, name ) == 0; } );
            if(!found)
            {
                lyraLogError("Vulkan Layer not found: %s", name);    
            }
            return found;
        });
    }

    vk::PhysicalDevice& PickBestDevice(lyra::Vector<vk::PhysicalDevice>& devices)
    {
        lyraAssert(!devices.empty());
        uint32 bestDeviceIndex = 0;
        size_t mostVRAMfound = 0;

        // Find the best one if there's more than one
        if(devices.size() > 1)
        {
            for (uint32 deviceIndex = 0; deviceIndex < devices.size(); ++deviceIndex)
            {
                auto& device = devices.at(deviceIndex);
                auto deviceProperties = device.getProperties();

                // Always favor discrete gpus over anything else
                if (devices[bestDeviceIndex].getProperties().deviceType != vk::PhysicalDeviceType::eDiscreteGpu &&
                    deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
                {
                    bestDeviceIndex = deviceIndex;
                    continue;
                }

                // Next best indicator is memory
                auto memoryProperties = device.getMemoryProperties();
                for (auto& heap : memoryProperties.memoryHeaps)
                {
                    if(heap.flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                    {
                        if(heap.size > mostVRAMfound)
                        {
                            mostVRAMfound = heap.size;
                            bestDeviceIndex = deviceIndex;
                        }
                    }
                }
            }
        }

        return devices.at(bestDeviceIndex);
    }
    
    VkBool32 DebugUtilsMessengerCallback( vk::DebugUtilsMessageSeverityFlagBitsEXT       messageSeverity,
                                                            vk::DebugUtilsMessageTypeFlagsEXT              messageTypes,
                                                            vk::DebugUtilsMessengerCallbackDataEXT const * pCallbackData,
                                                            void * /*pUserData*/ )
    {
        lyra::String debugMessage = lyra::StringUtils::StringFormat("[Vulkan %s] %s: %s [ID%s%d]",
                vk::to_string(messageSeverity).c_str(),
                vk::to_string(messageTypes).c_str(),
                pCallbackData->pMessage,
                pCallbackData->pMessageIdName,
                pCallbackData->messageIdNumber       
        );

        if(pCallbackData->queueLabelCount > 0)
        {
            debugMessage += "\n{Queues: ";
            for ( uint32_t i = 0; i < pCallbackData->queueLabelCount; i++ )
            {
                debugMessage += pCallbackData->pQueueLabels[i].pLabelName;
                debugMessage += " ";
            }
            debugMessage += "}";
        }

        if(pCallbackData->cmdBufLabelCount  > 0)
        {
            debugMessage += "\n{CmdBufs: ";
            for ( uint32_t i = 0; i < pCallbackData->cmdBufLabelCount ; i++ )
            {
                debugMessage += pCallbackData->pCmdBufLabels[i].pLabelName;
                debugMessage += " ";
            }
            debugMessage += "}";
        }

        if(pCallbackData->objectCount  > 0)
        {
            debugMessage += "\n{Objects: ";
            for ( uint32_t i = 0; i < pCallbackData->objectCount ; i++ )
            {
                debugMessage += lyra::StringUtils::StringFormat("%s %llx [%s],  ",
                    vk::to_string(static_cast<vk::ObjectType>( pCallbackData->pObjects[i].objectType)).c_str(),
                    pCallbackData->pObjects[i].objectHandle,
                    pCallbackData->pObjects[i].pObjectName ? pCallbackData->pObjects[i].pObjectName : "???");
            }
            debugMessage += "}";
        }


        
        if(messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
        {
            lyraLogError(debugMessage.c_str());
        }
        else if(messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
        {
            lyraLogWarning(debugMessage.c_str());
        }
        else
        {
            lyraLogInfo(debugMessage.c_str());
        }

        return false;
    }

    int FindMemoryTypeIndex(const vk::PhysicalDevice& device, uint32_t typeBits, vk::MemoryPropertyFlags properties)
    {
        lyraAssert(typeBits);

        const int numMemoryTypes = static_cast<int>(device.getMemoryProperties().memoryTypeCount);

        // bit holds current test bit against typeBits. Ie bit == 1 << typeBits

        uint32_t bit = 1;
        for (int i = 0; i < numMemoryTypes; ++i, bit += bit)
        {
            auto const& memoryType = device.getMemoryProperties().memoryTypes[i];
            if ((typeBits & bit) && (memoryType.propertyFlags & properties) == properties)
            {
                return i;
            }
        }
        return -1;
    }
    
    int FindQueue(const vk::PhysicalDevice& device, vk::QueueFlags queueFlags)
    {
        for(uint32 i = 0; i < device.getQueueFamilyProperties().size(); i++)
        {
            if(device.getQueueFamilyProperties()[i].queueFlags & queueFlags)
            {
                return i;
            }
        }
        return -1;
    }
}

namespace lyra
{
    const IDevice::DeviceInfo& VulkanDevice::GetDeviceInfo() const
    {
        return m_deviceInfo;
    }

    Expected<SharedPointer<BufferResource>, StringView> VulkanDevice::CreateBufferResource(const BufferResource::Descriptor& desc, const void* initData)
    {
        return {};
    }

    Expected<SharedPointer<ITexture>, StringView> VulkanDevice::CreateTextureResource(ITexture::Descriptor& desc)
    {
        auto vkFormat = VulkanUtils::TranslateToVkFormat(desc.format);
        desc = VulkanUtils::SanitizeTextureDescriptor(desc);
        
        if (vkFormat == vk::Format::eUndefined)
        {
            return Unexpected<StringView>("Failed to translate format to Vulkan format");
        }

        auto vulkanTexture = MakeSharedPointer<VulkanTexture>(desc, &m_vkDevice);
        vulkanTexture->m_vkFormat = vkFormat;
        
        vk::ImageCreateInfo imageInfo;
        switch (desc.type)
        {
        case IBaseResource::BaseResourceType::Texture:
            imageInfo.imageType = vk::ImageType::e2D;
            imageInfo.extent.width = desc.extents.width;
            imageInfo.extent.height = desc.extents.height;
            imageInfo.extent.depth = 1;
            break;
        default:
            return Unexpected<StringView>("Failed to translate resource type to Vulkan texture format");
        }

        imageInfo.format = vkFormat;
        imageInfo.mipLevels = desc.mipLevels;
        imageInfo.arrayLayers = desc.arraySize;
        imageInfo.samples = static_cast<vk::SampleCountFlagBits>(desc.sampleDesc.numSamples);

        imageInfo.tiling = vk::ImageTiling::eOptimal;
        imageInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled; // TODO: calculate usage
        imageInfo.sharingMode = vk::SharingMode::eExclusive;

        vk::ExternalMemoryImageCreateInfo externalMemoryImageCreateInfo;
        vk::ExternalMemoryHandleTypeFlags extMemoryHandleType =
#ifdef _WIN32
            vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32;
#else
            vk::ExternalMemoryHandleTypeFlagBits::eOpaqueFd;
#endif
        if (desc.isShared)
        {
            externalMemoryImageCreateInfo.pNext = nullptr;
            externalMemoryImageCreateInfo.handleTypes = extMemoryHandleType;
            imageInfo.pNext = &externalMemoryImageCreateInfo;
        }
        
        auto createImageResult = m_vkDevice.createImage(imageInfo);
        if (createImageResult.result != vk::Result::eSuccess)
        {
            return Unexpected<StringView>(to_string(createImageResult.result));
        }
        
        vulkanTexture->m_vkImage = createImageResult.value;

        auto memoryRequirements = m_vkDevice.getImageMemoryRequirements(vulkanTexture->m_vkImage);
        if (memoryRequirements.size == 0)
        {
            return Unexpected<StringView>("Failed to get image memory requirements");
        }

        // Allocate the memory
        vk::MemoryPropertyFlags reqMemoryProperties;
        int memoryTypeIndex = Details::FindMemoryTypeIndex(m_vkPhysicalDevice, memoryRequirements.memoryTypeBits, reqMemoryProperties);
        assert(memoryTypeIndex >= 0);

        //vk::MemoryPropertyFlags actualMemoryProperites = m_vkPhysicalDevice.getMemoryProperties().memoryTypes[memoryTypeIndex].propertyFlags;
        vk::MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memoryRequirements.size;
        allocInfo.memoryTypeIndex = memoryTypeIndex;
#ifdef _WIN32
        vk::ExportMemoryWin32HandleInfoKHR exportMemoryWin32HandleInfo;
#endif
        vk::ExportMemoryAllocateInfoKHR exportMemoryAllocateInfo;
        
        if (desc.isShared)
        {
#ifdef _WIN32
            exportMemoryWin32HandleInfo.pNext = nullptr;
            exportMemoryWin32HandleInfo.pAttributes = nullptr;
            exportMemoryWin32HandleInfo.dwAccess =
                DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE;
            exportMemoryWin32HandleInfo.name = nullptr;

            exportMemoryAllocateInfo.pNext =
                extMemoryHandleType & vk::ExternalMemoryHandleTypeFlagBits::eOpaqueWin32 ? &exportMemoryWin32HandleInfo : nullptr;
#endif
            exportMemoryAllocateInfo.handleTypes = extMemoryHandleType;
            allocInfo.pNext = &exportMemoryAllocateInfo;
        }
        
        auto allocateResults = m_vkDevice.allocateMemory(&allocInfo, nullptr, &vulkanTexture->m_vkDeviceMemory);
        CHECK_VK_RESULT(allocateResults);

        auto bindImageMemoryResult = m_vkDevice.bindImageMemory(vulkanTexture->m_vkImage, vulkanTexture->m_vkDeviceMemory, 0);
        CHECK_VK_RESULT(bindImageMemoryResult);
/*
        {
            auto defaultLayout = VulkanUtil::getImageLayoutFromState(desc.defaultState);
            if (defaultLayout != VK_IMAGE_LAYOUT_UNDEFINED)
            {
                _transitionImageLayout(
                    texture->m_image,
                    format,
                    *texture->getDesc(),
                    VK_IMAGE_LAYOUT_UNDEFINED,
                    defaultLayout);
            }
        }
        m_deviceQueue.flushAndWait();
  */      
        return vulkanTexture;
    }

    Expected<SharedPointer<IRenderResourceView>, StringView> VulkanDevice::CreateTextureView(ITexture* texture, const IRenderResourceView::Descriptor& desc)
    {
        auto textureView = MakeSharedPointer<VulkanTextureView>();
        if (textureView->Initialize(&m_vkDevice, static_cast<VulkanTexture*>(texture), desc))
        {
            
            return Unexpected<StringView>("Failed to initialize VulkanTextureView");
        }
        return textureView;
    }

    Expected<SharedPointer<ISurface>, StringView> VulkanDevice::CreateSurface(WindowHandle windowHandle)
    {
        auto surfacePtr = MakeSharedPointer<VulkanSurface>();
        bool success = surfacePtr->Initialize(&m_vkDevice, &m_vkPhysicalDevice, &m_vkInstance, windowHandle, Format::R8G8B8A8_UNORM);
        if (!success)
        {
            return Unexpected<StringView>("Failed to initialize Vulkan surface");
        }
        return surfacePtr;
    }

    void VulkanDevice::WaitForGPU()
    {
        ProfileScoped;
        m_vulkanDeviceQueue.Flush();
        m_vulkanDeviceQueue.WaitForGPU();
    }

    bool VulkanDevice::InitializeInstanceAndDevice(const InitInfoBase& desc)
    {
        // Init minimum set of functions
        VULKAN_HPP_DEFAULT_DISPATCHER.init( vkGetInstanceProcAddr );
        
        vk::ApplicationInfo applicationInfo( desc.applicationName.data(), 1, "LyraEngine", 1, VK_API_VERSION_1_2 );

        // Validation layers
        auto instanceLayerProperties = vk::enumerateInstanceLayerProperties();
        CHECK_VK_RESULTVALUE(instanceLayerProperties);
        Vector<const char*> instanceLayerNames;
        if(desc.enableValidation)
            instanceLayerNames.push_back( "VK_LAYER_KHRONOS_validation" );
        
        if ( !Details::CheckLayers( instanceLayerNames, instanceLayerProperties.value ) )
        {
            lyraLogError("Layer check failed. Tip: Set the environment variable VK_LAYER_PATH to point to the location of your layers");
            return false;
        }

        // Extensions
        Vector<const char*> instanceExtensionNames;
        if(desc.enableValidation)
        {
            instanceExtensionNames.push_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
            instanceExtensionNames.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
        }

        instanceExtensionNames.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
        instanceExtensionNames.push_back(VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME);
        instanceExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
        
#ifdef VK_USE_PLATFORM_WIN32_KHR
        instanceExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#else
#error Implement the correct surface extension for this platform
#endif
        
        // Instance creation
        vk::InstanceCreateInfo instanceCreateInfo( {}, &applicationInfo, instanceLayerNames, instanceExtensionNames );
        auto instance = vk::createInstance( instanceCreateInfo );
        CHECK_VK_RESULTVALUE(instance);
        m_vkInstance = instance.value;

        // Init all dynamically loaded functions for instance
        VULKAN_HPP_DEFAULT_DISPATCHER.init( instance.value, vkGetInstanceProcAddr );

        // Set up validation messenger
        if(desc.enableValidation)
        {
            vk::DebugUtilsMessengerCreateInfoEXT debugInfo;
            debugInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
            debugInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
            debugInfo.pfnUserCallback = Details::DebugUtilsMessengerCallback;

        
            auto debugMessenger = instance.value.createDebugUtilsMessengerEXT(debugInfo);
            CHECK_VK_RESULTVALUE(debugMessenger);
            m_vkDebugMessenger = debugMessenger.value;
        }

        // Device creation
        auto enumeratedDevices = m_vkInstance.enumeratePhysicalDevices();
        CHECK_VK_RESULTVALUE(enumeratedDevices);
        m_vkPhysicalDevice = Details::PickBestDevice(enumeratedDevices.value);

        m_deviceInfo.adapterName = m_vkPhysicalDevice.getProperties().deviceName;

        // Collect device properties
        vk::PhysicalDeviceProperties deviceProperties = m_vkPhysicalDevice.getProperties();
        auto& limits = m_deviceInfo.limits;
        limits.maxTextureDimension1D = deviceProperties.limits.maxImageDimension1D;
        limits.maxTextureDimension2D = deviceProperties.limits.maxImageDimension2D;
        limits.maxTextureDimension3D = deviceProperties.limits.maxImageDimension3D;
        limits.maxTextureDimensionCube = deviceProperties.limits.maxImageDimensionCube;
        limits.maxTextureArrayLayers = deviceProperties.limits.maxImageArrayLayers;

        limits.maxVertexInputElements = deviceProperties.limits.maxVertexInputAttributes;
        limits.maxVertexInputElementOffset = deviceProperties.limits.maxVertexInputAttributeOffset;
        limits.maxVertexStreams = deviceProperties.limits.maxVertexInputBindings;
        limits.maxVertexStreamStride = deviceProperties.limits.maxVertexInputBindingStride;

        limits.maxComputeThreadsPerGroup = deviceProperties.limits.maxComputeWorkGroupInvocations;
        limits.maxComputeThreadGroupSize[0] = deviceProperties.limits.maxComputeWorkGroupSize[0];
        limits.maxComputeThreadGroupSize[1] = deviceProperties.limits.maxComputeWorkGroupSize[1];
        limits.maxComputeThreadGroupSize[2] = deviceProperties.limits.maxComputeWorkGroupSize[2];
        limits.maxComputeDispatchThreadGroups[0] = deviceProperties.limits.maxComputeWorkGroupCount[0];
        limits.maxComputeDispatchThreadGroups[1] = deviceProperties.limits.maxComputeWorkGroupCount[1];
        limits.maxComputeDispatchThreadGroups[2] = deviceProperties.limits.maxComputeWorkGroupCount[2];

        limits.maxViewports = deviceProperties.limits.maxViewports;
        limits.maxViewportDimensions[0] = deviceProperties.limits.maxViewportDimensions[0];
        limits.maxViewportDimensions[1] = deviceProperties.limits.maxViewportDimensions[1];
        limits.maxFramebufferDimensions[0] = deviceProperties.limits.maxFramebufferWidth;
        limits.maxFramebufferDimensions[1] = deviceProperties.limits.maxFramebufferHeight;
        limits.maxFramebufferDimensions[2] = deviceProperties.limits.maxFramebufferLayers;

        limits.maxShaderVisibleSamplers = deviceProperties.limits.maxPerStageDescriptorSamplers;

        // Configure all available extensions
        Vector<const char*> deviceExtensionNames;
        Vector<const char*> deviceFeatures;
        auto extensionProperties = m_vkPhysicalDevice.enumerateDeviceExtensionProperties();
        EnumerateAllExtensionsAndFeatures( m_vkPhysicalDevice, deviceExtensionNames, deviceFeatures, extensionProperties.value );

        // Queues
        int queueFamilyIndex = Details::FindQueue( m_vkPhysicalDevice, vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eCompute);
        if (queueFamilyIndex < 0)
        {
            lyraLogError("Could not find a suitable family index in the Vulkan physical device.");
            return false;
        }

        float                     queuePriority = 0.0f;
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo( vk::DeviceQueueCreateFlags(), static_cast<uint32_t>( queueFamilyIndex ), 1, &queuePriority );
        vk::DeviceCreateInfo deviceCreateInfo(vk::DeviceCreateFlags(), deviceQueueCreateInfo, {}, deviceExtensionNames);
        auto createDeviceResults = m_vkPhysicalDevice.createDevice( deviceCreateInfo );
        CHECK_VK_RESULTVALUE(createDeviceResults);
        m_vkDevice = createDeviceResults.value;
        // Init all dynamically loaded functions for device
        VULKAN_HPP_DEFAULT_DISPATCHER.init(m_vkDevice);

        auto vkQueue = m_vkDevice.getQueue(queueFamilyIndex, 0);
        m_vulkanDeviceQueue.Initialize(m_vkDevice, vkQueue, queueFamilyIndex);

        
        return true;
    }

    VulkanDevice::~VulkanDevice()
    {
        VulkanDevice::WaitForGPU();

        m_vulkanDeviceQueue.Destroy();
        
        m_vkDevice.destroySampler(m_vkDefaultSampler);
        m_vkDevice.destroy();
        m_vkInstance.destroyDebugUtilsMessengerEXT(m_vkDebugMessenger);
        m_vkInstance.destroy();
    }

    bool VulkanDevice::Initialize(const InitInfoBase& desc)
    {
        m_deviceInfo.name = "lyra::VulkanDevice";
        m_deviceInfo.identityProjection = glm::identity<glm::mat4>();
        m_deviceInfo.deviceType = desc.deviceType;

        if(!InitializeInstanceAndDevice(desc))
        {
            lyraLogError("Vulkan device initialization failed.");
            return false;
        }

        vk::SamplerCreateInfo samplerCreateInfo({},
                                            vk::Filter::eNearest,
                                            vk::Filter::eNearest,
                                            vk::SamplerMipmapMode::eNearest,
                                            vk::SamplerAddressMode::eRepeat,
                                            vk::SamplerAddressMode::eRepeat,
                                            vk::SamplerAddressMode::eRepeat,
                                            0,
                                            false,
                                            1.0f,
                                            false,
                                            vk::CompareOp::eNever,
                                            0.0f,
                                            0.0f,
                                            vk::BorderColor::eFloatOpaqueBlack,
                                            false);
        
        auto samplerResult = m_vkDevice.createSampler(samplerCreateInfo);
        CHECK_VK_RESULTVALUE(samplerResult);
        m_vkDefaultSampler = samplerResult.value;
       
        return true;
    }
}

