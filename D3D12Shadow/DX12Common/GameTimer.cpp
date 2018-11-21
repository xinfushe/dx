#include "GameTimer.h"


GameTimer::GameTimer()
{
	 m_SecondsPerCount = 0;
	 m_DeltaTime = -1;
	 m_BaseTime = 0;
	 m_PauseTime = 0;
	 m_StopTime = 0;
	 m_PrevTime = 0;
	 m_CurrentTime = 0;
	 m_Stopped = false;

	 _int64 CountPerSecond;
	 QueryPerformanceFrequency((LARGE_INTEGER*)&CountPerSecond);
	 m_SecondsPerCount = 1.0 / (double)CountPerSecond;
}


float GameTimer::TotalTime() const
{
	//如果处于暂停状态，那么总共消耗的时间为开始暂停的时间减去暂停时长，再减去游戏开始时间。
	if (m_Stopped)
	{
		return (float)(((m_StopTime - m_PauseTime) - m_BaseTime)*m_SecondsPerCount);
	}
	//如果处于运行状态，用当前时间减去暂停时长和游戏开始时间
	else
	{
		return (float)(((m_CurrentTime - m_PauseTime) - m_BaseTime)*m_SecondsPerCount);
	}
}

float GameTimer::DeltaTime() const
{
	return (float)m_DeltaTime;
}

void GameTimer::Reset()
{
	//初始化计时器
	_int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	m_PrevTime = currentTime;
	m_BaseTime = currentTime;
	m_StopTime = 0;
	m_Stopped = false;
}

void GameTimer::Start()
{
	_int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	//从暂停状态下开始游戏
	if (m_Stopped)
	{
		//暂停时长等于开始时间减去暂停时间
		m_PauseTime += (startTime - m_StopTime);
		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}

}

void GameTimer::Stop()
{
	if (!m_Stopped)
	{
		_int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		m_StopTime = currentTime;
		m_Stopped = true;
	}
}

void GameTimer::Tick()
{
	_int64 currentTime;
	//获取当前调用的时间
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	m_CurrentTime = currentTime;
	//计算延迟时间
	m_DeltaTime = (m_CurrentTime - m_PrevTime)*m_SecondsPerCount;
	//记录当前时间为下一次调用的前一时刻
	m_PrevTime = m_CurrentTime;

	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}



}
