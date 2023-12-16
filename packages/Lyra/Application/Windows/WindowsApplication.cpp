#include <Application/Windows/WindowsApplication.h>
#include <Time/Chronometer.h>
#include <Time/TimeTypes.h>

#include <utility>

namespace lyra
{
WindowsApplication::WindowsApplication(CreationInfo _creationInfo)	
	: m_creationInfo{std::move(_creationInfo)}
{
	m_applicationWindow = MakeUniquePointer<ApplicationWindow>(m_creationInfo.m_mainWindowSize, m_creationInfo.m_fullScreen, m_creationInfo.m_windowName);
}
//----------------------------------------------------------------------------------------------------------------------
WindowsApplication::~WindowsApplication()
{
	m_applicationWindow->Shutdown();
}

	
void WindowsApplication::Run()
{
	Chronometer<> chrono;
	const auto windowHandle = m_applicationWindow->GetWindowHandle();
	chrono.Start();

	while (!m_applicationWindow->CloseRequested())
	{
		const auto deltaTime = chrono.Lap<float, TimeTypes::Seconds>();
		m_applicationWindow->Update();
	};
}
}
