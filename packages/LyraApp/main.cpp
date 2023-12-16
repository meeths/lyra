#include <Application/Windows/WindowsApplication.h>

int main(int, char[])
{

    lyra::WindowsApplication::CreationInfo windowCreationInfo;
    windowCreationInfo.m_fullScreen = false;
    windowCreationInfo.m_mainWindowSize = { 800, 600 };
    windowCreationInfo.m_windowName = "lyra App";
    auto application = lyra::WindowsApplication(windowCreationInfo);
    application.Run();
    return 0;
}
