
#include <Renderer/Vulkan/VulkanTextureView.h>

#include "VulkanTexture.h"
#include "VulkanUtils.h"

// Created on 2025-02-20 by franciscom

namespace lyra
{
    VulkanTextureView::~VulkanTextureView()
    {
        if (m_imageView)
        {
            m_device->destroyImageView(m_imageView);
        }
    }

    bool VulkanTextureView::Initialize(vk::Device* device, VulkanTexture* texture, const Descriptor& desc)
    {
        m_device = device;
        m_texture = texture;
        
        if (!texture)
        {
            m_imageView = nullptr;
            return true;
        }

        bool isArray = texture->GetDescriptor()->arraySize > 1;
        vk::ImageViewCreateInfo createInfo;
        createInfo.flags = {};
        lyraAssert(!IsTypelessFormat(texture->GetDescriptor()->format));
        createInfo.format = texture->m_vkFormat;
        createInfo.image = texture->m_vkImage;
        createInfo.components = vk::ComponentMapping{
            vk::ComponentSwizzle::eR,
            vk::ComponentSwizzle::eG,
            vk::ComponentSwizzle::eB,
            vk::ComponentSwizzle::eA};
        
        switch (texture->GetTextureType())
        {
        case TextureType::Texture1D:
            createInfo.viewType = isArray ? vk::ImageViewType::e1DArray : vk::ImageViewType::e1D;
            break;
        case TextureType::Texture2D:
            createInfo.viewType = isArray ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D;
            break;
        case TextureType::Texture3D:
            createInfo.viewType = vk::ImageViewType::e3D;
            break;
        case TextureType::TextureCube:
            createInfo.viewType = isArray ? vk::ImageViewType::eCubeArray : vk::ImageViewType::eCube;
            break;
        default:
            lyraLogError("Unknown Texture type.");
            return false;
            break;
        }

        createInfo.subresourceRange.aspectMask = VulkanUtils::GetVkAspectFlagsFromFormat(texture->m_vkFormat);

        createInfo.subresourceRange.baseArrayLayer = desc.subresourceRange.baseArrayLayer;
        createInfo.subresourceRange.baseMipLevel = desc.subresourceRange.mipLevel;
        createInfo.subresourceRange.layerCount = desc.subresourceRange.layerCount;
        if (createInfo.subresourceRange.layerCount == 0)
        {
            createInfo.subresourceRange.layerCount = isArray ? VK_REMAINING_ARRAY_LAYERS : 1;
            if (createInfo.viewType == vk::ImageViewType::eCube)
            {
                createInfo.subresourceRange.layerCount = 6;
            }
        }
        createInfo.subresourceRange.levelCount = desc.subresourceRange.mipLevelCount == 0
                                                     ? VK_REMAINING_MIP_LEVELS : desc.subresourceRange.mipLevelCount;

        switch (desc.type)
        {
        case ViewType::DepthStencil:
            m_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
            createInfo.subresourceRange.levelCount = 1;
            break;
        case ViewType::RenderTarget:
            m_layout = vk::ImageLayout::eColorAttachmentOptimal;
            createInfo.subresourceRange.levelCount = 1;
            break;
        case ViewType::ShaderResource:
            m_layout = vk::ImageLayout::eShaderReadOnlyOptimal;
            break;
        case ViewType::UnorderedAccess:
            m_layout = vk::ImageLayout::eGeneral;
            break;
        default:
            lyraAssert(0 && "Unknown TextureViewDesc type.");
            break;
        }

        auto createImageViewResult = m_device->createImageView(createInfo);
        CHECK_VK_RESULTVALUE(createImageViewResult);
        m_imageView = createImageViewResult.value;
        
        return true;
    }
}

