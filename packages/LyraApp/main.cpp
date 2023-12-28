#include <Application/Windows/WindowsApplication.h>
#include <Log/Log.h>
#include <Log/LoggerStdout.h>
#include <Vulkan/VulkanForwardRenderingPath.h>
#include <Vulkan/VulkanSystem.h>

int main(int, char[])
{
    const auto logger = MakeSharedPointer<lyra::LoggerStdout>();
    lyra::Log::Instance().RegisterLogger(logger);

    
    lyra::WindowsApplication::CreationInfo windowCreationInfo;
    windowCreationInfo.m_fullScreen = false;
    windowCreationInfo.m_mainWindowSize = { 800, 600 };
    windowCreationInfo.m_windowName = "lyra App";
    auto application = lyra::WindowsApplication(windowCreationInfo);

    const lyra::VulkanSystem::InitInfo vulkanInitInfo
    {
         application.GetMainWindow()
    };
    
    lyra::VulkanSystem vulkanInstance(vulkanInitInfo);

    const lyra::VulkanForwardRenderingPath::InitInfo renderPathInitInfo
    {
        vulkanInstance
    };
    lyra::VulkanForwardRenderingPath renderingPath(renderPathInitInfo);

    renderingPath.Configure();
    
    application.GetEngineLoop().AddExecutionUnit(
        lyra::EngineLoop::Phase::PreRender,
        lyra::Task([&vulkanInstance](float){vulkanInstance.BeginFrame();}));

    application.GetEngineLoop().AddExecutionUnit(
        lyra::EngineLoop::Phase::Render,
        lyra::Task([&renderingPath](float){renderingPath.Execute();}));

    application.GetEngineLoop().AddExecutionUnit(
        lyra::EngineLoop::Phase::EndFrame,
        lyra::Task([&vulkanInstance](float)
        {
            vulkanInstance.EndFrame();
            vulkanInstance.Present();
        }));

    application.Run();
    return 0;
}
