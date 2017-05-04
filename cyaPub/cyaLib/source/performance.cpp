#include "cyaTime.h"
#include "performance.h"

PerformanceCounter::PerformanceCounter()
{
#if defined(OS_IS_WINDOWS)
	LARGE_INTEGER nCounter;
	QueryPerformanceCounter(&nCounter);
	m_startCounter = nCounter.QuadPart;
#else
	m_startCounter = GetMsel();
#endif
}

PerformanceCounter::~PerformanceCounter()
{

}

INT64 PerformanceCounter::GetExcuteUsec()
{
#if defined(OS_IS_WINDOWS)
	INT64 cpuFreq = GetCpuFrequency();
	if (cpuFreq <= 0)
		return 0;

	LARGE_INTEGER nCounter;
	QueryPerformanceCounter(&nCounter);
	INT64 nowCounter = nCounter.QuadPart;
	return (nowCounter - m_startCounter) * 1000000 / cpuFreq;
#else
	INT64 nowMSel = GetMsel();
	return (nowMSel - m_startCounter) * 1000;
#endif
}
