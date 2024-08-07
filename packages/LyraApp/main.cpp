#include <Application/Windows/WindowsApplication.h>
#include <Log/Log.h>
#include <Log/LoggerStdout.h>
#include <Vulkan/VulkanForwardRenderingPath.h>
#include <Vulkan/VulkanSystem.h>
#include <Vulkan/VulkanDevice.h>

#include <Shaders/ShaderProgram.h>
#include <Filesystem/FileSystem.h>
#include <Vulkan/VulkanShaderModuleManager.h>

int main(int, char**)
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


    {
        lyra::ShaderProgram program(lyra::ShaderConstants::ShaderType::VertexShader, lyra::FileSystem::GetExecutablePath() + "/shaders/common.vert", "main");
            
        program.AddDefine("PROJ_MATRIX_ONLY");
        lyra::VulkanShaderModuleManager::ShaderModuleCreationInfo vsInfo {program};
        vulkanInstance.GetVulkanShaderModuleManager().CreateShaderModule(lyra::String(lyra::ShaderBuiltinNames::kVertexShader_2D_PCT()), vsInfo);
    }
    {
        lyra::ShaderProgram program(lyra::ShaderConstants::ShaderType::FragmentShader, lyra::FileSystem::GetExecutablePath() + "/shaders/basic.frag", "main");
        lyra::VulkanShaderModuleManager::ShaderModuleCreationInfo fsInfo {program};
        vulkanInstance.GetVulkanShaderModuleManager().CreateShaderModule(lyra::String(lyra::ShaderBuiltinNames::kFragmentShader_TextureColor()), fsInfo);
    }

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
    vulkanInstance.GetVulkanDevice().WaitForDevice();
    return 0;
}
