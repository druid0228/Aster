#include "GameTimer.h"

GameTimer::GameTimer()
{
	t_elapsed = 0;
	b_running = false;
}

GameTimer::~GameTimer()
{
}

void GameTimer::Start()
{
	t_prev = high_resolution_clock::now();
	b_running = true;
}

float GameTimer::Tick()
{
	auto t_now = high_resolution_clock::now();
	t_elapsed = duration_cast<milliseconds>(t_now - t_prev).count() * 0.001;
	//prev    now     next
	// |       |       |
	//-|-------|-------|--
	// t1     t2      t3
	t_prev = t_now;
	return t_elapsed;
}

void GameTimer::Stop()
{
	b_running = false;
}
