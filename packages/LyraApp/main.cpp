#include <Application/Windows/WindowsApplication.h>
#include <Log/Log.h>
#include <Log/LoggerStdout.h>
#include <Filesystem/FileSystem.h>

#include "Renderer/ISurface.h"
#include "Renderer/ITexture.h"
#include "Renderer/Renderer.h"

int main(int, char**)
{
    const auto logger = MakeSharedPointer<lyra::LoggerStdout>();
    lyra::Log::Instance().RegisterLogger(logger);

    
    lyra::WindowsApplication::CreationInfo windowCreationInfo;
    windowCreationInfo.m_fullScreen = false;
    windowCreationInfo.m_mainWindowSize = { 800, 600 };
    windowCreationInfo.m_windowName = "lyra App";
    auto application = lyra::WindowsApplication(windowCreationInfo);

    lyra::Renderer renderer;
    if(!renderer.Initialize({lyra::IDevice::DeviceType::Vulkan, "lyra sandbox app"}))
    {
        lyraLogError("Could not initialize renderer");
        return -1;
    }

    auto swapchain = renderer.GetDevice()->CreateSurface(application.GetMainWindow().GetWindowHandle());
    if(!swapchain)
    {
        lyraLogError("Could not create swapchain");
        return -1;
    }

    lyra::ISurface::Descriptor surfaceDescriptor
    {
        .format= lyra::Format::R8G8B8A8_UNORM,
        .size= windowCreationInfo.m_mainWindowSize,
        .vsync= true
    };
    
    if (!(*swapchain)->Configure(surfaceDescriptor))
    {
        lyraLogError("Could not configure swapchain");
        return -1;
    }

    constexpr uint32_t frameBufferCount = 2;
    
    for (uint32_t i = 0; i < frameBufferCount; ++i)
    {
        lyra::ITexture::Descriptor depthBufferDesc;
        depthBufferDesc.type = lyra::IBaseResource::BaseResourceType::Texture;
        depthBufferDesc.format = lyra::Format::D32_FLOAT;
        depthBufferDesc.extents.width = windowCreationInfo.m_mainWindowSize.x;
        depthBufferDesc.extents.height = windowCreationInfo.m_mainWindowSize.y;
        depthBufferDesc.extents.depth = 1;

        renderer.GetDevice()->CreateTextureResource(depthBufferDesc);
    }
        
    
    // application.GetEngineLoop().AddExecutionUnit(
    //     lyra::EngineLoop::Phase::PreRender,
    //     lyra::Task([&vulkanInstance](float){vulkanInstance.BeginFrame();}));
    //
    // application.GetEngineLoop().AddExecutionUnit(
    //     lyra::EngineLoop::Phase::Render,
    //     lyra::Task([&renderingPath](float){renderingPath.Execute();}));
    //
    // application.GetEngineLoop().AddExecutionUnit(
    //     lyra::EngineLoop::Phase::EndFrame,
    //     lyra::Task([&vulkanInstance](float)
    //     {
    //         vulkanInstance.EndFrame();
    //         vulkanInstance.Present();
    //     }));

    application.Run();
    return 0;
}
