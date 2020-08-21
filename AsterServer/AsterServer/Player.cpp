#include "Player.h"

void Player::initialize()
{
	cout << "Player::initialize()\n";
	m_clStatus = CL_STATUS::CS_FREE;
}

void Player::destroy()
{
}

void Player::collide(BaseObject& other)
{
}

void Player::move()
{
}

void Player::attack()
{
}

void Player::damaged()
{
}
