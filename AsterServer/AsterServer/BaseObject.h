#pragma once
#include"Headers.h"
class BaseObject
{
public:
	atomic<CL_STATUS> m_clStatus;

public:
	virtual void initialize();
	virtual void destroy();

	virtual void move();
	virtual void collide(BaseObject& other);
	virtual void attack();
	virtual void damaged();
};

