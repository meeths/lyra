
#pragma once

#include <Containers/UnorderedMap.h>
#include <Memory/UniquePointer.h>
#include <String/StringHash.h>
#include <Core/CoreDefines.h>

// Created on 2022-01-17 by sisco

namespace lyra
{
class VulkanRenderPass;
class VulkanSystem;
    
class VulkanForwardRenderPass;

class VulkanRenderPassManager
{
public:
    VulkanRenderPassManager(VulkanSystem& _vulkanSystem);
    ~VulkanRenderPassManager();
    
    void SetCurrentBufferIndex(unsigned int _index);

    void RecreatePipelines();

    template <class T, class INIT_INFO>
    void AddRenderPass(StringHash _name, INIT_INFO& _initInfo){ lyraAssert(!m_renderPasses.contains(_name)); m_renderPasses[_name] = MakeUniquePointer<T>(_initInfo); }
    
    VulkanRenderPass& GetRenderPass(StringHash _renderPass);

private:
    UnorderedMap<StringHash, UniquePointer<VulkanRenderPass>> m_renderPasses;


};

}

