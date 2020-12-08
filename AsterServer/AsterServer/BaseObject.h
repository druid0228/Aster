#pragma once
#include"Headers.h"
class BaseObject
{
public:
	int m_id;
	atomic<CL_STATUS> m_clStatus;
public:
	// all object has position
	int x, y;
public:
	virtual void initialize();
	virtual void destroy();

	virtual void move();
	virtual void collide(BaseObject& other);
	virtual void attack();
	virtual void damaged();
};

