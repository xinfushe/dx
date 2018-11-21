#pragma once

//--------------------计时器头文件-----------------//
//时间；2018-7-24  by;淡一抹夕霞(lastSunlight)
//说明：此文件包含游戏程序运行时的计时器
//------------------------------------------------//

#include <Windows.h>


class GameTimer 
{
public:
	GameTimer();
	float TotalTime() const;//计算游戏时长
	float DeltaTime() const;//计算帧时延
	void Reset();//初始化计时器
	void Start();//重新开始计时
	void Stop();//暂停计时
	void Tick();//每帧调用
private:
	double m_SecondsPerCount;
	double m_DeltaTime;
	_int64 m_BaseTime;
	_int64 m_PauseTime;
	_int64 m_StopTime;
	_int64 m_PrevTime;
	_int64 m_CurrentTime;
	bool m_Stopped;
};

