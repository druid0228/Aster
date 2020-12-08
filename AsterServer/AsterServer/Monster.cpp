#include"BaseObject.h"
#include "Monster.h"

void Monster::initialize()
{
	m_clStatus = CL_STATUS::CS_SLEEP;
	speed = 10;
	x = rand() % WORLD_WIDTH;
	y = rand() % WORLD_HEIGHT;
}

void Monster::destroy()
{
}

void Monster::collide(BaseObject& other)
{
}

void Monster::move()
{
}

void Monster::attack()
{
}

void Monster::damaged()
{
}
