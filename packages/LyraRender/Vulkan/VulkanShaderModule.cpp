#include <Vulkan/VulkanShaderModule.h>
#include <Core/CoreDefines.h>

namespace lyra
{
	VulkanShaderModule::~VulkanShaderModule()
	{
	}

	void VulkanShaderModule::Load(VkDevice _device, const Vector<char>& _code)
	{
		Unload();
		mDevice = _device;

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = _code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());
		if (vkCreateShaderModule(mDevice, &createInfo, nullptr, &mShaderModule) != VK_SUCCESS) 
			lyraAssert(0 && "failed to create shader module!");
	}

	void VulkanShaderModule::Unload()
	{
		if(mDevice && mShaderModule)
		{
			vkDestroyShaderModule(mDevice, mShaderModule, nullptr);
			mShaderModule = nullptr;
			mDevice = nullptr;
		}
	}
}
