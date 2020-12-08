#pragma once
class Monster : public BaseObject
{
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


