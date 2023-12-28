#include <Application/Windows/WindowsApplication.h>
#include <Profiler/Profiler.h>
#include <String/StringUtils.h>
#include <Time/Chronometer.h>
#include <Time/TimeTypes.h>

namespace
{
	lyra::Vector<lyra::String> threadNames;
	
	void OnFrameGraphSetup(int numThreads)
	{
		for (int i = 0; i < numThreads; ++i)
		{
			threadNames.emplace_back(lyra::StringUtils::StringFormat("TaskGraphThread %d", i));
		}
	}

	void OnFrameGraphStart(int id, const lyra::String& taskName)
	{
		thread_local bool threadNameSet = false;
		if(!threadNameSet)
		{
			ProfileSetThreadName(threadNames[id].c_str());
			threadNameSet = true;
		}
	}

	void OnFrameGraphEnd(int id, const lyra::String& taskName)
	{
		
	}
}

namespace lyra
{
WindowsApplication::WindowsApplication(CreationInfo _creationInfo)	
	: m_creationInfo{std::move(_creationInfo)}, m_taskGraphExecutor(_creationInfo.m_maxCoresPerExecutor)
{
	m_applicationWindow = MakeUniquePointer<ApplicationWindow>(m_creationInfo.m_mainWindowSize, m_creationInfo.m_fullScreen, m_creationInfo.m_windowName);
	m_taskGraphThreadObserver = m_taskGraphExecutor.make_observer<TaskGraphExecutorThreadObserver>("Observer", OnFrameGraphSetup, OnFrameGraphStart, OnFrameGraphEnd);

	// End frame
	m_engineLoop.AddExecutionUnit(EngineLoop::Phase::EndFrame,
		Task([&applicationWindow = *m_applicationWindow](float){ applicationWindow.Update();}));
}
//----------------------------------------------------------------------------------------------------------------------
WindowsApplication::~WindowsApplication()
{
	m_taskGraphExecutor.remove_observer(std::move(m_taskGraphThreadObserver));
	m_applicationWindow->Shutdown();
}

	
void WindowsApplication::Run()
{
	Chronometer<> chrono;
	const auto windowHandle = m_applicationWindow->GetWindowHandle();
	chrono.Start();

	while (!m_applicationWindow->CloseRequested())
	{
		ProfileScopedN("WindowsApplicationFrame");
		const auto deltaTime = chrono.Lap<float, TimeTypes::Seconds>();
		m_engineLoop.Step(deltaTime);
		ProfileFrameMark;
	}
}
}
