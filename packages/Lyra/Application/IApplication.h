#pragma once
#include <Threading/TaskGraphExecutor.h>

namespace lyra
{
class IApplicationConfig;
class EngineLoop;
	
class IApplication
{
public:
	virtual ~IApplication() = default;
	virtual void Run() = 0;
	virtual TaskGraphExecutor& GetTaskGraphExecutor() = 0;
	virtual EngineLoop& GetEngineLoop() = 0;
};

}
