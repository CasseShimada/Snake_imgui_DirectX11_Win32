#pragma once

class Timer
{
public:
	void Start(); // 开始计时
	void Stop(); // 停止计时

	double GetElapsedSeconds() const; // 获取经过的时间（秒）

	void Reset(); // 重置计时器

private:
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;
	bool isRunning = false;
};