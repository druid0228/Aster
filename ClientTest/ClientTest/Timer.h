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
// timer�� ����
//�� - > n :��
//�� - > m :��
// priority queue?
// �̵�,����,��ų => 50���� �ȵ�
// �ð������� sort
// �տ� ���͸� Ȯ��
