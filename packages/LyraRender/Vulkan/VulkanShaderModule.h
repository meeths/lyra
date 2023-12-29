#pragma once
#include <vulkan/vulkan.h>
#include <Containers/Vector.h>

namespace lyra
{

class VulkanShaderModule
{
public:
	~VulkanShaderModule();
	void Load(VkDevice _device, const Vector<char>& _code);
	void Unload();
	VkShaderModule GetVkShaderModule() const { return mShaderModule; }
protected:
	VkDevice mDevice = nullptr;
	VkShaderModule mShaderModule = nullptr;
};

}
