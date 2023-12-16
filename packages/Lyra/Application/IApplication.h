#pragma once
#include <Core/Types.h>

namespace lyra
{
class IApplicationConfig;
class ApplicationLoop;
	
class IApplication
{
public:
	virtual ~IApplication() = default;
	virtual void Run() = 0;
};

}
