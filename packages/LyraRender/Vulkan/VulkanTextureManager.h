
#pragma once
#include <Vulkan/VulkanCommandBufferManager.h>
#include <Vulkan/VulkanQueue.h>
#include <Vulkan/VulkanTexture.h>
#include <Core/Function.h>

// Created on 2020-08-10 by Sisco

namespace lyra
{

class VulkanTextureManager
{
public:
    typedef struct
    {
        VulkanDevice& m_Device;
        VkCommandPool mTransferCommandPool;
    }InitInfo;

    typedef struct BlitInfo
    {
        Math::Vector2i m_Dimensions {};
        VkFormat m_SourceFormat = VK_FORMAT_UNDEFINED;
        const char* m_Data = nullptr;
    }BlitInfo;
    
    VulkanTextureManager(InitInfo _initInfo);
    ~VulkanTextureManager();

    VulkanTexture& CreateTexture(StringHash _textureName, VulkanTexture::CreationInfo _creationInfo);
    void BlitToTexture(StringHash _textureName, BlitInfo _blitInfo);
    VulkanTexture& GetTexture(StringHash _textureName);
    bool HasTexture(StringHash _textureName) const { return mTextures.find(_textureName) != mTextures.end(); }
    VkDescriptorImageInfo GetVkDescriptorImageInfo(StringHash _textureName);

    void DestroyTexture(StringHash _textureName);

    bool TexturesChangedLastFrame() const { return mTexturesChanged; };
    void ClearTexturesChanged() { mTexturesChanged = false; };
    
    void DoForAllTextures(TextureType _type, Function<void(StringHash, VulkanTexture&)> _function);
private:
    void GenerateMipmaps(StringHash _textureName);
    void CreateSampler(StringHash _textureName, const VulkanTexture::CreationInfo& _creationInfo);

    void CreateDefaultTextures();
    void CreateBRDFLUTTexture();
    
    VkDevice m_Device = nullptr;
    VkPhysicalDevice mPhysicalDevice = nullptr;
    VkCommandPool mTransferCommandPool = nullptr;
    const VulkanQueue& mTransferQueue;

    UnorderedMap<StringHash, UniquePointer<VulkanTexture>> mTextures;
    UnorderedMap<StringHash, VkSampler> mTextureSamplers;

    bool mTexturesChanged = true;
    mutable SRWLock mTexturesMutex;
};

}

