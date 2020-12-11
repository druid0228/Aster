#pragma once
#include<chrono>
using namespace std::chrono;
class Timer
{
	high_resolution_clock::time_point t;
	high_resolution_clock::time_point t1;

public:
	void Tick();
	void Start();
	void Stop();
	void Pause();
};


// Cool time
// timer랑 따로
//ㅁ - > n :ㅁ
//ㅂ - > m :ㅂ
// priority queue?
// 이동,공격,스킬 => 50개가 안됨
// 시간순으로 sort
// 앞에 부터만 확인
