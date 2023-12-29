
#pragma once
#include <optional>
#include <Containers/UnorderedMap.h>
#include <Core/Optional.h>
#include <Shaders/ShaderProgram.h>
#include <String/String.h>
#include <String/StringHash.h>
#include <Vulkan/VulkanShaderModule.h>
#include <vulkan/vulkan_core.h>

// Created on 2020-08-13 by Sisco

namespace lyra
{

class VulkanShaderModuleManager
{
public:
    typedef struct
    {
        ShaderProgram mProgram;
        Optional<String> mCode;
    }
    ShaderModuleCreationInfo;

    VulkanShaderModuleManager(VkDevice _device);
    ~VulkanShaderModuleManager();
    
    void CreateShaderModule(const String& moduleName, ShaderModuleCreationInfo& _info);
    void DestroyShaderModule(const String& moduleName);
    
    void RecompileShaderModule(StringHash moduleName);
    void RecompileAllShaderModules();

    VkShaderModule GetShaderModule(const String& _shaderModule) const;
private:
    
    VkDevice m_Device = nullptr;
    UnorderedMap<StringHash, VulkanShaderModule> m_ShaderModules; 
    UnorderedMap<StringHash, ShaderModuleCreationInfo> m_ShaderModuleInfos; 
};

}

