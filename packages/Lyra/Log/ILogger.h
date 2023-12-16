#pragma once
#include <String/String.h>

namespace lyra
{
	enum class LogType
	{
		LOG_INFO,
        LOG_ERROR,
        LOG_WARNING
    };

	class ILogger
	{
	public:
		virtual ~ILogger() {}
		virtual void Log(LogType logType, const String& _log) = 0;
    };
}
