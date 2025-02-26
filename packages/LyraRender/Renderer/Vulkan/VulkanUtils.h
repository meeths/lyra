
#pragma once
#include <Log/Log.h>
#include <Renderer/Format.h>
#include <vulkan/vulkan.hpp>

#include "Renderer/ITexture.h"

// Created on 2024-11-06 by sisco

#define CHECK_VK_RESULTVALUE(x) do{if ((x).result != vk::Result::eSuccess) { lyraLogError("Vulkan Error: %s", to_string((x).result).c_str()); lyraLogFlush(); lyraAssert(0); }} while(0)
#define CHECK_VK_RESULT(x) do{if ((x) != vk::Result::eSuccess) { lyraLogError("Vulkan Error: %s", to_string(x).c_str()); lyraLogFlush(); lyraAssert(0); }} while(0)

namespace lyra
{

class VulkanUtils
{
public:
    static vk::Format TranslateToVkFormat(Format format);
    static Format TranslateFromVkFormat(vk::Format format);
    static vk::ImageUsageFlags TranslateToVkImageUsageFlags(TextureUsage usage);
    static ITexture::Descriptor SanitizeTextureDescriptor(const ITexture::Descriptor& desc);
    static int CalculateMipLevels(const ITexture::Extents& extents);
    static vk::ImageAspectFlags GetVkAspectFlagsFromFormat(vk::Format format);
    static vk::ImageLayout GetImageLayoutFromState(ResourceState state);
    static vk::AccessFlags GetAccessFlagsFromImageLayout(vk::ImageLayout layout);
    static vk::PipelineStageFlags GetPipelineStageFlagsFromImageLayout(vk::ImageLayout layout);
    static vk::ImageAspectFlags GetAspectMaskFromFormat(vk::Format format, TextureAspect aspect = TextureAspect::All);
    static vk::ImageUsageFlagBits GetImageUsageFlags(TextureUsage usage);
    static vk::ImageUsageFlags GetImageUsageFlags(TextureUsage usage, MemoryType memoryType, const void* initData);
    static vk::ImageUsageFlagBits GetImageUsageFlags(ResourceState state);

};

}

