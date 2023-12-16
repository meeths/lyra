#pragma once
#include <Application/IApplication.h>
#include <Application/Windows/ApplicationWindow.h>
#include <Memory/SharedPointer.h>
#include <Threading/TaskGraphExecutorThreadObserver.h>

namespace lyra
{

class WindowsApplication : public IApplication
{
public:
	struct CreationInfo
	{
		Math::Vector2i m_mainWindowSize{};
		uint32 m_maxCoresPerExecutor = 4;		
		bool m_fullScreen = false;
		String m_windowName = "Lyra Application";
	};

	WindowsApplication(CreationInfo _creationInfo);
	~WindowsApplication() override;
	
	void Run() override;
	ApplicationWindow& GetMainWindow() const { return *m_applicationWindow; }
	TaskGraphExecutor& GetTaskGraphExecutor() override { return m_taskGraphExecutor; };
private:
	CreationInfo m_creationInfo;
	
	UniquePointer<ApplicationWindow> m_applicationWindow;
	TaskGraphExecutor m_taskGraphExecutor;
	SharedPointer<TaskGraphExecutorThreadObserver> m_taskGraphThreadObserver;
};

}
