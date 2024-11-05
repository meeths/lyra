
#include <Vulkan/VulkanTextureManager.h>

#include <Vulkan/VulkanDevice.h>
#include <Vulkan/VulkanStagingBuffer.h>
#include <Vulkan/VulkanTexture.h>
#include <Vulkan/VulkanUtils.h>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <Filesystem/FileSystem.h>
// #include <Resources/Image/ImageDescriptor.h>
// #include <Resources/Image/ImageImporter.h>
#include <String/StringUtils.h>
#include <Containers/Array.h>
//#include <Resources/Image/EmbeddedCubemap.h>
// Created on 2020-08-10 by Sisco
namespace
{
    unsigned int GetCubemapDstPixelIndex(unsigned int face, unsigned int x, unsigned int y, unsigned int faceWidth, unsigned int faceHeight)
    {
        const unsigned int fullWidth = faceWidth;
        const unsigned int fullFace = fullWidth * faceHeight;
        return face * fullFace + fullWidth * y + x;
    }

    unsigned int GetCubemapSrcPixelIndex(unsigned int face, unsigned int x, unsigned int y, unsigned int faceWidth, unsigned int faceHeight)
    {
        const unsigned int fullWidth = faceWidth * 6;
        return fullWidth * y + face * faceWidth + x;
    }

    struct PixelColor { unsigned char r, g, b, a; };
    // void CreateSolidColorTexture(lyra::StringView _textureName, PixelColor _color, lyra::glm::ivec2 _dimensions, lyra::VulkanTextureManager& _textureManager)
    // {
    //     lyra::Array<PixelColor, 8*8> pixels;
    //     pixels.fill(_color);
    //     const lyra::VulkanTexture::CreationInfo textureCreationInfo
    //     {
    //         _dimensions,
    //         lyra::VulkanUtils::GetMipLevels(_dimensions),
    //         1,
    //         lyra::TextureType::Texture,
    //         VK_FORMAT_R8G8B8A8_UNORM,
    //         VK_IMAGE_TILING_OPTIMAL,
    //     };
    //     _textureManager.CreateTexture(_textureName, textureCreationInfo);
    //     const lyra::VulkanTextureManager::BlitInfo blitInfo
    //     {
    //         textureCreationInfo.m_Dimensions,
    //         textureCreationInfo.m_Format,
    //         reinterpret_cast<char*>(pixels.data())
    //     };
    //     _textureManager.BlitToTexture(_textureName, blitInfo);
    //
    // }
    //
    // void CreateCheckerboardTexture(lyra::StringView _textureName, PixelColor _color1, PixelColor _color2, lyra::glm::ivec2 _dimensions, lyra::VulkanTextureManager& _textureManager)
    // {
    //     lyra::Vector<PixelColor> pixels(_dimensions.x*_dimensions.y);
    //
    //     for (int i = 0; i < _dimensions.x; i++)
    //         for (int j = 0; j < _dimensions.y; j++)
    //             pixels[j * _dimensions.x + i] = i & 1 ? (j & 1 ? _color1 : _color2) : (j & 1 ? _color2 : _color1);
    //     
    //     const lyra::VulkanTexture::CreationInfo textureCreationInfo
    //     {
    //         _dimensions,
    //         lyra::VulkanUtils::GetMipLevels(_dimensions),
    //         1,
    //         lyra::TextureType::Texture,
    //         VK_FORMAT_R8G8B8A8_UNORM,
    //         VK_IMAGE_TILING_OPTIMAL,
    //         VK_FILTER_NEAREST,
    //         VK_FILTER_NEAREST,
    //         VK_SAMPLER_MIPMAP_MODE_NEAREST
    //     };
    //     _textureManager.CreateTexture(_textureName, textureCreationInfo);
    //     const lyra::VulkanTextureManager::BlitInfo blitInfo
    //     {
    //         textureCreationInfo.m_Dimensions,
    //         textureCreationInfo.m_Format,
    //         reinterpret_cast<char*>(pixels.data())
    //     };
    //     _textureManager.BlitToTexture(_textureName, blitInfo);
    //
    // }
}
namespace lyra
{
    VulkanTextureManager::VulkanTextureManager(InitInfo _initInfo) :
    mTransferQueue(_initInfo.m_Device.GetQueues().GetTransferQueue())
    {
        m_Device = _initInfo.m_Device.GetVkDevice();
        mPhysicalDevice = _initInfo.m_Device.GetVkPhysicalDevice();
        mTransferCommandPool = _initInfo.mTransferCommandPool;

        CreateDefaultTextures();
    }

    VulkanTextureManager::~VulkanTextureManager()
    {
        for (auto& texture : mTextures)
        {
            texture.second->Destroy();
        }
        for (auto& sampler : mTextureSamplers)
        {
            vkDestroySampler(m_Device, sampler.second, nullptr);
        }
        mTexturesChanged = true;
        
        mTextures.clear();
        mTextureSamplers.clear();
    }

    VulkanTexture& VulkanTextureManager::CreateTexture(StringHash _textureName, VulkanTexture::CreationInfo _creationInfo)
    {
        if(HasTexture(_textureName))
        {
            DestroyTexture(_textureName);
        }
        
        UniquePointer<VulkanTexture> newTexture = MakeUniquePointer<VulkanTexture>();

        const VulkanTexture::CreationInfo textureCreationInfo
        {
            _creationInfo.m_Dimensions,
            _creationInfo.m_MipLevels,
            _creationInfo.m_numSamples,
            _creationInfo.m_TextureType,
            _creationInfo.m_Format,
            VK_IMAGE_TILING_OPTIMAL
        };

        newTexture->Create(m_Device, mPhysicalDevice, mTransferCommandPool, mTransferQueue, textureCreationInfo);

        if(VulkanDevice::DebugMarkersAvailable())
        {
            VkDebugMarkerObjectNameInfoEXT nameInfo = {};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT;
            nameInfo.object = (uint64_t)newTexture->GetVkImage(); // this cast may vary by platform/compiler
#ifdef KEEP_ORIGINAL_STRING            
            nameInfo.pObjectName = _textureName.GetOriginalString().data();
#else
            nameInfo.pObjectName = "Name information unavailable";
#endif
            vkDebugMarkerSetObjectName(m_Device, &nameInfo);
        }

        {
            SRWScopedWriteLock lock(mTexturesMutex);
            mTextures[_textureName] = std::move(newTexture);
        }
        CreateSampler(_textureName, _creationInfo);
        
        mTexturesChanged = true;
        
        return *mTextures[_textureName];
    }

    void VulkanTextureManager::BlitToTexture(StringHash _textureName, BlitInfo _blitInfo)
    {
        lyraAssert(HasTexture(_textureName));
        lyraAssert(mTextureSamplers.find(_textureName) != mTextureSamplers.end());

        auto dstTexture = GetTexture(_textureName);

        const auto srcBPP = VulkanUtils::GetFormatBpp(_blitInfo.m_SourceFormat);
        lyraAssert( srcBPP && "Unsupported format");

        const auto imageSize = _blitInfo.m_Dimensions.x * _blitInfo.m_Dimensions.y * srcBPP;
        VulkanStagingBuffer stagingBuffer(m_Device, mPhysicalDevice, mTransferQueue, imageSize);
        const auto data = stagingBuffer.Map();
        if(dstTexture.m_TextureType != TextureType::TextureCube)
        {
            memcpy(data, _blitInfo.m_Data, imageSize);
        }
        else
        {
            const unsigned int pixelSize = srcBPP;
            const Vector<Array<float, 4>> faceColors
            {
                {1.0f, 0, 0, 1.0f},
                {0.0f, 1.0f, 0, 1.0f},
                {0.0f, 0, 1.0f, 1.0f},
                {1.0f, 0, 1.0f, 1.0f},
                {1.0f, 1.0f, 0, 1.0f},
                {0.0f, 1.0f, 1.0f, 1.0f}
            };
            for (unsigned int cubeFace = 0; cubeFace < 6; ++cubeFace)
            {
                for (int faceY = 0; faceY < _blitInfo.m_Dimensions.y; ++faceY)
                {
                    for (int faceX = 0; faceX < _blitInfo.m_Dimensions.x / 6; ++faceX)
                    {
                        memcpy(
                            &((unsigned char*)data)[GetCubemapDstPixelIndex(
                                cubeFace, faceX, faceY, _blitInfo.m_Dimensions.x / 6,
                                _blitInfo.m_Dimensions.y) * pixelSize],
                            (unsigned char*)&_blitInfo.m_Data[GetCubemapSrcPixelIndex(
                                cubeFace, faceX, faceY, _blitInfo.m_Dimensions.x / 6,
                                _blitInfo.m_Dimensions.y) * pixelSize],
                            pixelSize
                        );
                        /*
                        memcpy(
                            &((unsigned char*)data)[GetCubemapDstPixelIndex(
                                cubeFace, faceX, faceY, _blitInfo.m_Dimensions.x / 6,
                                _blitInfo.m_Dimensions.y) * pixelSize],
                                faceColors[cubeFace].data()
                            ,
                            pixelSize
                        );
                        */
                        
                    }
                }
            }
        }
        stagingBuffer.Unmap();

        {
            ScopedVulkanCommandBuffer commandBuffer(m_Device, mTransferCommandPool, mTransferQueue);
            dstTexture.TransitionImageLayout(commandBuffer,
                dstTexture.GetImageInfo().format,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                dstTexture.GetImageInfo().mipLevels, dstTexture.m_TextureType != TextureType::TextureCube ? 1 : 6);
        }

        stagingBuffer.CopyToBuffer(dstTexture, dstTexture.m_TextureType != TextureType::TextureCube ? 1 : 6);

        GenerateMipmaps(_textureName);
    }

    void VulkanTextureManager::GenerateMipmaps(StringHash _textureName)
    {
        lyraAssert(HasTexture(_textureName));
        lyraAssert(mTextureSamplers.find(_textureName) != mTextureSamplers.end());

        auto dstTexture = GetTexture(_textureName);

        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, dstTexture.m_ImageInfo.format, &formatProperties);

        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) 
        {
            lyraAssert(0 && "texture image format does not support linear blitting!");
        }
        
        // Desired layout for each mip
        const auto dstImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        ScopedVulkanCommandBuffer commandBuffer(m_Device, mTransferCommandPool, mTransferQueue);

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = dstTexture.m_Image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = dstTexture.m_TextureType != TextureType::TextureCube ? 1 : 6;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = dstTexture.m_ImageInfo.extent.width;
        int32_t mipHeight = dstTexture.m_ImageInfo.extent.height;

        for (uint32_t i = 1; i < dstTexture.m_ImageInfo.mipLevels; i++) 
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer.GetVkCommandBuffer(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = dstTexture.m_TextureType != TextureType::TextureCube ? 1 : 6;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = dstTexture.m_TextureType != TextureType::TextureCube ? 1 : 6;

            vkCmdBlitImage(commandBuffer.GetVkCommandBuffer(),
                dstTexture.m_Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                dstTexture.m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = dstImageLayout;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer.GetVkCommandBuffer(),
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = dstTexture.m_ImageInfo.mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = dstImageLayout;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer.GetVkCommandBuffer(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    void VulkanTextureManager::CreateSampler(StringHash _textureName, const VulkanTexture::CreationInfo& _creationInfo)
    {
        SRWScopedWriteLock lock(mTexturesMutex);

        lyraAssert(mTextureSamplers.find(_textureName) == mTextureSamplers.end());
        
        VkSampler sampler;

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

        samplerInfo.magFilter = _creationInfo.m_MagFilter;
        samplerInfo.minFilter = _creationInfo.m_MinFilter;
        samplerInfo.addressModeU = _creationInfo.m_AddressModeU;
        samplerInfo.addressModeV = _creationInfo.m_AddressModeV;
        samplerInfo.addressModeW = _creationInfo.m_AddressModeW;
        samplerInfo.anisotropyEnable = _creationInfo.m_AnisotropyEnable;
        samplerInfo.maxAnisotropy = _creationInfo.m_MaxAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = _creationInfo.m_MipmapMode;
        samplerInfo.mipLodBias = _creationInfo.m_MipLodBias;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = static_cast<float>(mTextures[_textureName]->GetImageInfo().mipLevels);

        if (vkCreateSampler(m_Device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS)
        {
            lyraAssert(0 && "failed to create texture sampler!");
        }
        mTextureSamplers[_textureName] = sampler;

        mTexturesChanged = true;
        
    }

    void VulkanTextureManager::CreateDefaultTextures()
    {
        // // Default pink texture
        // CreateSolidColorTexture( VulkanConstants::DefaultBuiltinTexture(),{255, 0, 255, 255}, {8, 8}, *this);
        //
        // // Default albedo texture
        // CreateSolidColorTexture( VulkanConstants::DefaultBuiltinTextureAlbedo(),{128, 128, 128, 255}, {8, 8}, *this);
        // // Default normalAO texture
        // CreateSolidColorTexture( VulkanConstants::DefaultBuiltinTextureNormalAO(),{128, 128, 255, 255}, {8, 8}, *this);
        // // Default RoughHeightMetal texture
        // CreateSolidColorTexture( VulkanConstants::DefaultBuiltinTextureRoughHeightMetal(),{102, 128, 0, 255}, {8, 8}, *this);
        // // Default Checkerboard texture
        // CreateCheckerboardTexture( VulkanConstants::DefaultBuiltinTextureCheckerboard(),{155, 155, 155, 255}, {200, 200, 200, 255}, {32, 32}, *this);
        //
        // CreateBRDFLUTTexture();
        //
        // {
        //     const VulkanTexture::CreationInfo textureCreationInfo
        //     {
        //         {embeddedCubemapWidth, embeddedCubemapHeight},
        //         VulkanUtils::GetMipLevels({embeddedCubemapHeight,embeddedCubemapHeight}),
        //         1,
        //         TextureType::TextureCube,
        //         VK_FORMAT_R32G32B32A32_SFLOAT,
        //         VK_IMAGE_TILING_OPTIMAL,
        //     };
        //     CreateTexture(VulkanConstants::DefaultBuiltinCubemap(), textureCreationInfo);
        //     CreateTexture(VulkanConstants::DefaultBuiltinCubemapIrradiance(), textureCreationInfo);
        //     const BlitInfo blitInfo
        //     {
        //         textureCreationInfo.m_Dimensions,
        //         textureCreationInfo.m_Format,
        //         reinterpret_cast<char*>(embeddedCubemap)
        //     };
        //     BlitToTexture(VulkanConstants::DefaultBuiltinCubemap(), blitInfo);
        //     BlitToTexture(VulkanConstants::DefaultBuiltinCubemapIrradiance(), blitInfo);
        // }

    }

    VulkanTexture& VulkanTextureManager::GetTexture(StringHash _textureName)
    {
        const auto textureIter = mTextures.find(_textureName);
        lyraAssert(textureIter != mTextures.end());
        return *textureIter->second;
    }

    VkDescriptorImageInfo VulkanTextureManager::GetVkDescriptorImageInfo(StringHash _textureName)
    {
        lyraAssert(HasTexture(_textureName));
        lyraAssert(mTextureSamplers.find(_textureName) != mTextureSamplers.end());

        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = mTextures[_textureName]->GetVkImageLayout();
        imageInfo.imageView = mTextures[_textureName]->GetVkImageView();
        imageInfo.sampler = mTextureSamplers[_textureName];
        return imageInfo;
    }

    void VulkanTextureManager::DestroyTexture(StringHash _textureName)
    {
        SRWScopedWriteLock lock(mTexturesMutex);
        lyraAssert(HasTexture(_textureName));
        lyraAssert(mTextureSamplers.find(_textureName) != mTextureSamplers.end());

        mTextures[_textureName]->Destroy();
        vkDestroySampler(m_Device, mTextureSamplers[_textureName], nullptr);
        mTextures.erase(_textureName);
        mTextureSamplers.erase(_textureName);

        mTexturesChanged = true;
    }

    void VulkanTextureManager::DoForAllTextures(TextureType _type, std::function<void(StringHash, VulkanTexture&)> _function)
    {
        SRWScopedWriteLock lock(mTexturesMutex);

        for (auto& [key, texture] : mTextures)
        {
            if(static_cast<int>(texture->m_TextureType) & static_cast<int>(_type))
                _function(key, *texture);
        }
    }

    // Texture genenation


    float RadicalInverse_VdC(unsigned int bits)
    {
        bits = (bits << 16u) | (bits >> 16u);
        bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
        bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
        bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
        bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
        return float(double(bits) * 2.3283064365386963e-10);
    }

    glm::vec2 Hammersley(unsigned int i, unsigned int N)
    {
        return glm::vec2(float(i) / float(N), RadicalInverse_VdC(i));
    }

    glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, float roughness, glm::vec3 N)
    {
        float a = roughness * roughness;

        float phi = 2.0f * Math::Pi * Xi.x;
        float cosTheta = sqrt((1.0f - Xi.y) / (1.0f + (a * a - 1.0f) * Xi.y));
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

        // from spherical coordinates to cartesian coordinates
        glm::vec3 H;
        H.x = cos(phi) * sinTheta;
        H.y = sin(phi) * sinTheta;
        H.z = cosTheta;

        // from tangent-space vector to world-space sample vector
        glm::vec3 up = abs(N.z) < 0.999f ? glm::vec3(0.0, 0.0, 1.0) : glm::vec3(1.0f, 0.0, 0.0);
        glm::vec3 tangent = glm::normalize(glm::cross(up, N));
        glm::vec3 bitangent = glm::cross(N, tangent);

        glm::vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
        return glm::normalize(sampleVec);
    }

    float GeometrySchlickGGX(float NdotV, float roughness)
    {
        float a = roughness;
        float k = (a * a) / 2.0f;

        float nom = NdotV;
        float denom = NdotV * (1.0f - k) + k;

        return nom / denom;
    }

    float GeometrySmith(float roughness, float NoV, float NoL)
    {
        float ggx2 = GeometrySchlickGGX(NoV, roughness);
        float ggx1 = GeometrySchlickGGX(NoL, roughness);

        return ggx1 * ggx2;
    }

    glm::vec2 IntegrateBRDF(float NdotV, float roughness, unsigned int samples)
    {
        glm::vec3 V;
        V.x = sqrt(1.0f - NdotV * NdotV);
        V.y = 0.0;
        V.z = NdotV;

        float A = 0.0;
        float B = 0.0;

        glm::vec3 N = glm::vec3(0.0, 0.0, 1.0);

        for (unsigned int i = 0u; i < samples; ++i)
        {
            glm::vec2 Xi = Hammersley(i, samples);
            glm::vec3 H = ImportanceSampleGGX(Xi, roughness, N);
            glm::vec3 L = glm::normalize(2.0f * glm::dot(V, H) * H - V);

            float NoL = Math::Max(L.z, 0.0f);
            float NoH = Math::Max(H.z, 0.0f);
            float VoH = Math::Max(glm::dot(V,H), 0.0f);
            float NoV = Math::Max(glm::dot(N, V), 0.0f);

            if (NoL > 0.0)
            {
                float G = GeometrySmith(roughness, NoV, NoL);

                float G_Vis = (G * VoH) / (NoH * NoV);
                float Fc = pow(1.0f - VoH, 5.0f);

                A += (1.0f - Fc) * G_Vis;
                B += Fc * G_Vis;
            }
        }

        return glm::vec2(A / float(samples), B / float(samples));
    }
    
    void VulkanTextureManager::CreateBRDFLUTTexture()
    {
        // int samples = 512;
        // const int size = 128;
        //
        // Array<glm::vec2, size * size> pixels;  
        //
        // for (int y = 0; y < size; y++)
        // {
        //     for (int x = 0; x < size; x++)
        //     {
        //         float NoV = (y + 0.5f) * (1.0f / size);
        //         float roughness = (x + 0.5f) * (1.0f / size);
        //
        //         pixels[x*size + y] = IntegrateBRDF(NoV, roughness, samples);
        //     }
        // }
        //
        // const VulkanTexture::CreationInfo textureCreationInfo
        // {
        //     {size, size},
        //     1,
        //     1,
        //     TextureType::Texture,
        //     VK_FORMAT_R32G32_SFLOAT,
        //     VK_IMAGE_TILING_OPTIMAL,
        // };
        // CreateTexture(VulkanConstants::DefaultBuiltinTextureBRDFLUT(), textureCreationInfo);
        // const BlitInfo blitInfo
        // {
        //     textureCreationInfo.m_Dimensions,
        //     textureCreationInfo.m_Format,
        //     reinterpret_cast<char*>(pixels.data())
        // };
        // BlitToTexture(VulkanConstants::DefaultBuiltinTextureBRDFLUT(), blitInfo);

    }

}

