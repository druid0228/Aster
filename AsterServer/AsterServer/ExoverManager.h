#pragma once
#include"Headers.h"
// �ӽ�
class ExoverManager
{
public:
	EXOVER* get()const {
		EXOVER* over = new EXOVER;
		ZeroMemory(&over->over, sizeof(over->over));
		return over;
	}
	void release(EXOVER* over)
	{
		delete over;
	}
};

