#pragma once
#include"globalHeader.h"

enum :unsigned {
	at_move, at_attack,
};

class temp
{
public:
	// milliseconds
	int cooltime; 
	unsigned actType;
};

class CoolTime
{
	// time
	priority_queue<int> queue;
	map<unsigned, bool> dict;
public:
	CoolTime();
	virtual ~CoolTime();
	void insert(unsigned actType);
	void update(float elapsed);
	bool checkCool(unsigned actType);
};


// 객체들이 자기 쿨타임 넣고
// 내부에서 관리
// 시간 체크만 확인
// 외부에서 쿨타임 객체 확인만
