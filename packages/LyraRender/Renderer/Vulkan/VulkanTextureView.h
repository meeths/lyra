
#pragma once
#include <vulkan/vulkan.hpp>
#include "Renderer/Vulkan/VulkanResourceView.h"

// Created on 2025-02-20 by franciscom

namespace lyra
{
class VulkanTexture;

class VulkanTextureView : public IVulkanResourceView
{
public:
	~VulkanTextureView() override;
	bool Initialize(VulkanTexture& texture);
private:
	vk::ImageView m_imageView;
};

}

