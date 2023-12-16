#pragma once
#include <Application/IApplication.h>
#include <Application/Windows/ApplicationWindow.h>

namespace lyra
{

class WindowsApplication : public IApplication
{
public:
	struct CreationInfo
	{
		Math::Vector2i m_mainWindowSize{};
		bool m_fullScreen = false;
		String m_windowName;
	};

	WindowsApplication(CreationInfo _creationInfo);
	~WindowsApplication() override;
	
	void Run() override;
	ApplicationWindow& GetMainWindow() { return *m_applicationWindow; }

private:
	CreationInfo m_creationInfo;
	
	UniquePointer<ApplicationWindow> m_applicationWindow;
};

}
