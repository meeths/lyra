#include <Application/Windows/WindowsApplication.h>
#include <Log/Log.h>
#include <Log/LoggerStdout.h>

#include "../LyraRender/Vulkan/VulkanInstance.h"

int main(int, char[])
{
    const auto logger = MakeSharedPointer<lyra::LoggerStdout>();
    lyra::Log::Instance().RegisterLogger(logger);

    
    lyra::WindowsApplication::CreationInfo windowCreationInfo;
    windowCreationInfo.m_fullScreen = false;
    windowCreationInfo.m_mainWindowSize = { 800, 600 };
    windowCreationInfo.m_windowName = "lyra App";
    auto application = lyra::WindowsApplication(windowCreationInfo);

    lyra::VulkanInstance::InitInfo vulkanInitInfo
    {
        application.GetMainWindow()
    };
    lyra::VulkanInstance vulkanInstance(vulkanInitInfo);

    application.Run();
    return 0;
}
