
#pragma once
#include <Renderer/IDevice.h>

// Created on 2024-11-06 by sisco

namespace lyra
{

class Renderer
{
public:
    bool Initialize(IDevice::InitInfoBase deviceDescriptor);
    SharedPointer<IDevice> GetDevice() const { return m_device; }
private:
    SharedPointer<IDevice> m_device;
};

}

