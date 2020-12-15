#pragma once
#include<chrono>
using namespace std::chrono;
class GameTimer
{
	high_resolution_clock::time_point t_prev;
	float t_elapsed;
	bool b_running;
	
public:
	GameTimer();
	virtual ~GameTimer();
	void Start();
	float Tick();
	void Stop();
};

