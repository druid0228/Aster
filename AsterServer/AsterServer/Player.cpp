#include "Player.h"

void Player::initialize()
{
	cout << "Player::initialize()\n";
	m_clStatus = CL_STATUS::CS_FREE;
	speed = 10;
	x = rand() % WORLD_WIDTH;
	y = rand() % WORLD_HEIGHT;
	m_prev_size = 0;
	m_recv_over.init();
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
