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
		m_Device = _device;

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = _code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(_code.data());
		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS) 
			lyraAssert(0 && "failed to create shader module!");
	}

	void VulkanShaderModule::Unload()
	{
		if(m_Device && m_ShaderModule)
		{
			vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
			m_ShaderModule = nullptr;
			m_Device = nullptr;
		}
	}
}
