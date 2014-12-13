#include "CanvasCore.h"

using namespace Canvas;

LONGLONG Timer::frequency(-1);

long long milliseconds_now() {
	static LARGE_INTEGER s_frequency;
	static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
	if (s_use_qpc) {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return (1000LL * now.QuadPart) / s_frequency.QuadPart;
	}
	else {
		return GetTickCount();
	}
}

Timer::Timer()
{
#ifdef CVS_WIN32
	m_lasttime = milliseconds_now();
#endif
}

void Timer::setFrame()
{
#ifdef CVS_WIN32
	LONGLONG newtime = milliseconds_now();
	m_deltatime = newtime - m_lasttime;
	m_lasttime = newtime;
#endif
}

float Timer::getDeltaTimeMS()
{
	return (float)m_deltatime;
}

float Timer::getDeltaTimeMC()
{
	return m_deltatime;
}

void Timer::StartStopWatch()
{
#ifdef CVS_WIN32
	m_stopwatchtime = milliseconds_now();
#endif
}

int Timer::EndStopWatch()
{
#ifdef CVS_WIN32
	return milliseconds_now() - m_stopwatchtime;
#endif
}

Time Canvas::GetOSTime()
{
#ifdef CVS_WIN32
	SYSTEMTIME t;
	GetSystemTime(&t);
	Time time = { t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds };
#endif
	return time;
}