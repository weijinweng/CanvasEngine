#include "CanvasCore.h"

using namespace Canvas;

int Timer::frequency(-1);

Timer::Timer()
{
#ifdef CVS_WIN32
	if (frequency == -1)
	{
		QueryPerformanceFrequency(&counter);
		
		frequency = counter.QuadPart;
	}
	QueryPerformanceCounter(&counter);
	m_lasttime = counter.QuadPart;
#endif
}

void Timer::setFrame()
{
#ifdef CVS_WIN32
	QueryPerformanceCounter(&counter);
	m_deltatime = (m_lasttime - counter.QuadPart)/frequency);
	
	m_lasttime = counter.QuadPart;
#endif
}

float Timer::getDeltaTimeMS()
{
	return m_deltatime / 1000;
}

float Timer::getDeltaTimeMC()
{
	return m_deltatime;
}

void Timer::StartStopWatch()
{
#ifdef CVS_WIN32
	QueryPerformanceCounter(&counter);
	m_stopwatchtime = counter.QuadPart;
#endif
}

int Timer::EndStopWatch()
{
#ifdef CVS_WIN32
	QueryPerformanceCounter(&counter);
	return (m_stopwatchtime - counter.QuadPart) / frequency;
#endif
}