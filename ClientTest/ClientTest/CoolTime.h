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


// ��ü���� �ڱ� ��Ÿ�� �ְ�
// ���ο��� ����
// �ð� üũ�� Ȯ��
// �ܺο��� ��Ÿ�� ��ü Ȯ�θ�
