#pragma once
#include <Threading/TaskGraphExecutor.h>

namespace lyra
{
class IApplicationConfig;
class ApplicationLoop;
	
class IApplication
{
public:
	virtual ~IApplication() = default;
	virtual void Run() = 0;
	virtual TaskGraphExecutor& GetTaskGraphExecutor() = 0;
};

}
