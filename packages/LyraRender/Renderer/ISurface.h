#pragma once
#include <Renderer/Format.h>
#include <Application/WindowHandle.h>
#include <glm/vec2.hpp>

// Created on 2024-11-12 by sisco

namespace lyra
{
    class IDevice;
}

namespace lyra
{
class ITexture;

    
class ISurface
{
    public:

    struct SurfaceInfo
    {
        Format preferredFormat;
        TextureUsage supportedUsage;
        Vector<Format> supportedFormats;
    };    

    struct Descriptor
    {
        Format format = Format::Unknown;
        glm::ivec2 size = glm::ivec2(0);
        bool vsync = true;
        uint32_t desiredImageCount = 3;
        TextureUsage usage = TextureUsage::RenderTarget;
    };

    virtual ~ISurface() = default;
    virtual const SurfaceInfo& GetSurfaceInfo() = 0;
    virtual const Descriptor& GetSurfaceConfig() = 0;
    virtual size_t GetImageCount() const = 0; 
    virtual bool Configure(Descriptor config) = 0;
    virtual ITexture* GetTexture() = 0;
    virtual bool Present() = 0;
};

}

