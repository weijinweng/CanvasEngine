#include <windows.h>
#include <stdio.h>
#include "Timer.h"

static LARGE_INTEGER currentCounter;

//Windows specific time query macros
#define QUERY_TIME()												\
if (!QueryPerformanceCounter(&currentCounter))						\
{																	\
	fprintf(stderr, "Clock: Fatal Error: Could not query time.\n"); \
}

#define TIME_DIFF(currentCounter, prevCounter) (currentCounter.QuadPart - prevCounter.QuadPart) / frequency;

long int Timer::split(void) const
{
	QUERY_TIME();
	return TIME_DIFF(currentCounter, this->prevCounter);
}

long int Timer::delta(void)
{
	long int result = this->split();
	prevCounter = currentCounter;

	return result;
}

Timer::Timer()
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
	{
		fprintf(stderr, "Clock: Fatal Error: Could not query frequency.\n");
	}
	else
	{
		this->frequency = double(li.QuadPart) / 1000.0; //Divide by 1000000.0 if microsec is desired.
	}

	this->delta(); // initialize delta time
}