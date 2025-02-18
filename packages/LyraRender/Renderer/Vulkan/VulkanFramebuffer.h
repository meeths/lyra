
#pragma once
#include <glm/vec2.hpp>
#include <vulkan/vulkan.hpp>

#include "Renderer/IFramebuffer.h"

// Created on 2024-11-14 by sisco

namespace lyra
{
class VulkanDevice;

class VulkanFramebuffer : public IFramebuffer
{
public:
    bool Initialize(VulkanDevice* device, IFramebufferInfo& info);
private:
    vk::Framebuffer m_vkFramebuffer;
    Vector<IResourceView*> m_renderTargetViews;
    IResourceView* m_depthStencilView{};
    glm::ivec2 m_dimensions{};
    VulkanDevice* m_vulkanDevice{};
};

}

