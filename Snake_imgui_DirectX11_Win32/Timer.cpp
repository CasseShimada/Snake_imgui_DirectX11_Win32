#include "Timer.h"

void Timer::Start()
{
	startTime = std::chrono::high_resolution_clock::now();
	isRunning = true;
}

void Timer::Stop()
{
	endTime = std::chrono::high_resolution_clock::now();
	isRunning = false;
}

double Timer::GetElapsedSeconds() const
{
	if (isRunning)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double>(currentTime - startTime).count();
	}
	else
	{
		return std::chrono::duration<double>(endTime - startTime).count();
	}
}

void Timer::Reset()
{
	isRunning = false;
}