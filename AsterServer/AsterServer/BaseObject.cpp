#include "BaseObject.h"

void BaseObject::initialize()
{
	m_clStatus = CL_STATUS::CS_FREE;
}

void BaseObject::destroy()
{
}

void BaseObject::move()
{
}

void BaseObject::collide(BaseObject& other)
{
}

void BaseObject::attack()
{
}

void BaseObject::damaged()
{
}
