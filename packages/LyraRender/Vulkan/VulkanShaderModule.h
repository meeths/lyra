#pragma once
#include <vulkan/vulkan.h>
#include <Containers/Vector.h>
#include <Core/CoreDefines.h>

namespace lyra
{

class VulkanShaderModule
{
public:
	VulkanShaderModule() = default;
	DeclareNonCopyable(VulkanShaderModule);
	DeclareDefaultMoveable(VulkanShaderModule);
	
	~VulkanShaderModule();
	void Load(VkDevice _device, const Vector<char>& _code);
	void Unload();
	VkShaderModule GetVkShaderModule() const { return m_ShaderModule; }
protected:
	VkDevice m_Device = nullptr;
	VkShaderModule m_ShaderModule = nullptr;
};

}
