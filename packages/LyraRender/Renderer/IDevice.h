
#pragma once
#include <glm/ext/matrix_float4x4.hpp>

#include "Renderer/ITexture.h"
#include "Application/WindowHandle.h"
#include "Containers/Array.h"
#include "Containers/Vector.h"
#include "Core/Types.h"
#include "Memory/SharedPointer.h"
#include "Resources/BufferResource.h"
#include "String/String.h"

// Created on 2024-11-06 by sisco

namespace lyra
{
    class ISurface;
}

namespace lyra
{

class IDevice
{
    friend class Renderer;
public:
    virtual ~IDevice() = default;

    enum class DeviceType : uint8
    {
        Default,
        Vulkan,
    };

    struct DeviceLimits
    {
        uint32 maxTextureDimension1D;
        uint32 maxTextureDimension2D;
        uint32 maxTextureDimension3D;
        uint32 maxTextureDimensionCube;
        uint32 maxTextureArrayLayers;

        uint32 maxVertexInputElements;
        uint32 maxVertexInputElementOffset;
        uint32 maxVertexStreams;
        uint32 maxVertexStreamStride;

        uint32 maxComputeThreadsPerGroup;
        uint32 maxComputeThreadGroupSize[3];
        uint32 maxComputeDispatchThreadGroups[3];

        uint32 maxViewports;
        uint32 maxViewportDimensions[2];
        uint32 maxFramebufferDimensions[3];

        uint32 maxShaderVisibleSamplers;
    };
    
    struct InitInfoBase
    {
        DeviceType deviceType = DeviceType::Default;
        StringView applicationName;
        bool enableValidation = true;
    };

    struct DeviceInfo
    {
        DeviceType deviceType;

        StringView name;
        StringView adapterName;

        DeviceLimits limits;

        glm::mat4 identityProjection;
    };

    
    
    virtual const DeviceInfo& GetDeviceInfo() const = 0;

    virtual void WaitForGPU() = 0;
    virtual Expected<SharedPointer<ISurface>,StringView> CreateSurface(WindowHandle windowHandle) = 0;
    virtual Expected<SharedPointer<BufferResource>,StringView> CreateBufferResource(const BufferResource::Descriptor& desc, const void* initData) = 0;
    virtual Expected<SharedPointer<ITexture>,StringView> CreateTextureResource(ITexture::Descriptor& desc) = 0;
    

protected:
    virtual bool Initialize(const InitInfoBase& desc) = 0;

};

}

