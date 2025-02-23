
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
	bool Initialize(vk::Device* device, VulkanTexture* texture, const Descriptor& desc);
	Descriptor* GetDescriptor() const override { return nullptr; }
	ViewType GetViewType() const override { return ViewType::ShaderResource; }
private:
	vk::Device* m_device = nullptr;
	VulkanTexture* m_texture = nullptr;
	vk::ImageLayout m_layout = vk::ImageLayout::eUndefined;
	vk::ImageView m_imageView;
};

}

