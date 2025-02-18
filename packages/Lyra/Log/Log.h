#pragma once
#include <Core/ISingleton.h>
#include <Log/ILogger.h>
#include <Memory/SharedPointer.h>
#include <String/String.h>
#include <Containers/Vector.h>
#include <Threading/SRWLock.h>

#define lyraLogError(x, ...) lyra::Log::Instance().LogError(x, __VA_ARGS__)
#define lyraLogWarning(x, ...) lyra::Log::Instance().LogWarning(x, __VA_ARGS__)
#define lyraLogInfo(x, ...) lyra::Log::Instance().LogInfo(x, __VA_ARGS__)
#define lyraLogFlush() lyra::Log::Instance().Flush()

namespace lyra
{
class ILogger;
	
class Log : public ISingleton<Log>
{
public:

	void RegisterLogger(SharedPointer<ILogger> _logger);
	
	void LogWarning(const char* fmt, ...) ;
	void LogError(const char* fmt, ...) ;
	void LogInfo(const char* fmt, ...) ;

	void NotifyWarning(StringView title, const char* fmt, ...) ;
	void NotifyError(StringView title, const char* fmt, ...) ;
	void NotifyInfo(StringView title, const char* fmt, ...) ;
	void NotifyProgress(StringView title, StringView body, uint32_t step, uint32_t numSteps);

	void Flush();
private:
	
	void LogDetail(LogType logType, const char* fmt, va_list args);
	void NotifyDetail(LogType logType, StringView title, const char* fmt, va_list args);
	void NotifyDetail(LogType logType, StringView title, StringView body, uint32_t step, uint32_t numSteps);
	
	Vector<SharedPointer<ILogger>> m_Loggers;
	SRWLock m_loggersLock;
};
}