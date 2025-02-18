
#include <Core/CoreDefines.h>
#include <Renderer/Renderer.h>
#include <Renderer/Vulkan/VulkanDevice.h>

// Created on 2024-11-06 by sisco

namespace lyra
{
    bool Renderer::Initialize(IDevice::InitInfoBase deviceDescriptor)
    {
        switch (deviceDescriptor.deviceType)
        {
        case IDevice::DeviceType::Vulkan:
            m_device = MakeSharedPointer<VulkanDevice>();
            break;
        case IDevice::DeviceType::Default:
        default: ;
            return false;
            break;
        }

        if(!m_device->Initialize(deviceDescriptor))
        {
            return false;
        }

        return true;
    }
}

