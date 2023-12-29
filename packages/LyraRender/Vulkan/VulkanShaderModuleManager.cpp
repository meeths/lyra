#include <ranges>
#include <Vulkan/VulkanShaderModuleManager.h>
#include <Vulkan/VulkanShaderCompiler.h>
#include <Shaders/ShaderSourceLoader.h>
#include <Log/Log.h>
// Created on 2020-08-13 by Sisco

namespace lyra
{
    VulkanShaderModuleManager::VulkanShaderModuleManager(VkDevice _device)
    {
        m_Device = _device;
    }

    VulkanShaderModuleManager::~VulkanShaderModuleManager()
    {
        for (auto& module : m_ShaderModules | std::views::values)
        {
            module.Unload();
        }
        m_ShaderModules.clear();
    }

    void VulkanShaderModuleManager::CreateShaderModule(const String& moduleName, ShaderModuleCreationInfo& _info)
    {
        lyraAssert(!m_ShaderModules.contains(moduleName));
        String code;
        if(_info.mCode)
            code = _info.mCode.value();
        else
        {
            auto shaderSource = ShaderSourceLoader::LoadShaderSource(_info.mProgram.GetFilename()); 
            code = shaderSource.value_or(shaderSource.error());
        }

        const auto compiledShader = VulkanShaderCompiler::CompileShader(
            _info.mProgram,
            code
            );
        
        if(compiledShader.empty())
        {
            return;
        }
        
        VulkanShaderModule module;
        module.Load(m_Device, compiledShader);
        m_ShaderModules[moduleName] = module;
        m_ShaderModuleInfos[moduleName] = _info;
    }

    void VulkanShaderModuleManager::DestroyShaderModule(const String& moduleName)
    {
        m_ShaderModules[moduleName].Unload();
        m_ShaderModules.erase(moduleName);
        m_ShaderModuleInfos.erase(moduleName);
    }

    void VulkanShaderModuleManager::RecompileShaderModule(StringHash moduleName)
    {
        lyraAssert(m_ShaderModuleInfos.contains(moduleName));
        const auto& moduleInfo = m_ShaderModuleInfos[moduleName];
        
        String code;
        if(moduleInfo.mCode)
            code = moduleInfo.mCode.value();
        else
        {
            auto shaderSource = ShaderSourceLoader::LoadShaderSource(moduleInfo.mProgram.GetFilename()); 
            code = shaderSource.value_or(shaderSource.error());
        }

        const auto compiledShader = VulkanShaderCompiler::CompileShader(
            moduleInfo.mProgram,
            code
            );

        if(compiledShader.size() == 0)
        {
            Log::Instance().LogError("Error compiling shader");
            return;
        }

        m_ShaderModules[moduleName].Unload();
        m_ShaderModules.erase(moduleName);
        
        VulkanShaderModule module;
        module.Load(m_Device, compiledShader);
        m_ShaderModules[moduleName] = module;
    }

    void VulkanShaderModuleManager::RecompileAllShaderModules()
    {
        for (const auto& key : m_ShaderModuleInfos | std::views::keys)
        {
            RecompileShaderModule(key);
        }
    }

    VkShaderModule VulkanShaderModuleManager::GetShaderModule(const String& _shaderModule) const
    {
        const auto shaderModule = m_ShaderModules.find(_shaderModule);
        lyraAssert(shaderModule != m_ShaderModules.end());
        return shaderModule->second.GetVkShaderModule();
    }
}

