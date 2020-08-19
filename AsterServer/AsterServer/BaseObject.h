#pragma once
class BaseObject
{
public:
	virtual void move()		= 0;
	virtual void attack()	= 0;
	virtual void damaged()	= 0;
};

