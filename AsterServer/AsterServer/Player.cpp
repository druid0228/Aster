#include "Player.h"



void Player::initialize()
{
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

//================================================================================================================================//
// inline functions
inline void Player::view_insert(int id)
{
	m_mtx.lock();
	m_viewlist.insert(id);
	m_mtx.unlock();
}

inline void Player::view_erase(int id)
{
	m_mtx.lock();
	m_viewlist.erase(id);
	m_mtx.unlock();
}