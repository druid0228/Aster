#pragma once
#include"BaseObject.h"

class Player : public BaseObject
{
public:
	//Network
	SOCKET m_socket;
	EXOVER m_recv_over;
	int m_prev_size;
	char m_packet_buf[MAX_PACKET_SIZE];

	//
	unordered_set<int> m_viewlist;
	mutex m_mtx;
public:
	int speed;

public:
	virtual void initialize()override;
	virtual void destroy()override;
	virtual void collide(BaseObject& other)override;
	virtual void move()override;
	virtual void attack()override;
	virtual void damaged()override;
};

