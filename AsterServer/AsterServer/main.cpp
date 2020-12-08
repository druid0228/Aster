#include"Headers.h"
#include"ExoverManager.h"
#include"protocol.h"
#include"Objects.h"

HANDLE g_iocp;
SOCKET l_socket;
ExoverManager over_manager;

BaseObject* clients[MAX_CLEINTS];
inline Player& player_cast(int user_id) { return reinterpret_cast<Player&>(*clients[user_id]);}
inline Monster& monster_cast(int user_id) { return reinterpret_cast<Monster&>(*clients[user_id]); }

inline void nearby_monster(int user_id)
{
	Player& u = player_cast(user_id);
	int u_x = u.x;
	int u_y = u.y;

	// 나중에 Sector로 find 이건 임시
	for (int i = MONSTER_BEGIN; i < MONSTER_END; ++i)
	{
		Monster& monster = monster_cast(i);
		int m_x = monster.x;
		int m_y = monster.y;
		//find
		if (VIEW_RANGE < abs(u_x - m_x))continue;
		if (VIEW_RANGE < abs(u_y - m_y))continue;

		monster.m_clStatus = CL_STATUS::CS_ACTIVE;


	}
}
inline bool in_sight(int lid, int rid)
{
	BaseObject* lbo = clients[lid], * rbo = clients[rid];
	if (abs(lbo->x - rbo->x) > VIEW_RANGE)return false;
	if (abs(lbo->y - rbo->y) > VIEW_RANGE)return false;
	return true;
}

void initialize_clients() 
{
	cout << "initialize_clients\n";
	for (int i = OBJECT_BEGIN; i < OBJECT_END; ++i)
	{
		clients[i] = new BaseObject;
		clients[i]->m_id = i;
		clients[i]->initialize();
	}
	cout << " --initialize objects:"<< OBJECT_END - OBJECT_BEGIN << "\n";
	for (int i = NPC_BEGIN; i < NPC_END; ++i)
	{
		clients[i] = new BaseObject;
		clients[i]->m_id = i;
		clients[i]->initialize();
	}
	cout << " --initialize npc:" << NPC_END - NPC_BEGIN<< "\n";
	for (int i = MONSTER_BEGIN; i < MONSTER_END; ++i)
	{
		clients[i] = new Monster;
		clients[i]->m_id = i;
		clients[i]->initialize();
	}
	cout << " --initialize monster:" << MONSTER_END - MONSTER_BEGIN << "\n";
	for (int i = USER_BEGIN; i < USER_END; ++i)
	{
		clients[i] = new Player;
		clients[i]->m_id = i;
		clients[i]->initialize();
	}
	cout << " --initialize user:" << USER_END - USER_BEGIN << "\n";
}

//================================================================================================================================//
// PACKET
void send_packet(int user_id, void* p)
{
	Player& u = player_cast(user_id);;
	char* buf = reinterpret_cast<char*>(p);

	EXOVER* exover = over_manager.get();
	exover->init(OP_SEND, buf[0]);

	memcpy(exover->io_buf, buf, buf[0]);
	WSASend(u.m_socket, &exover->wsabuf, 1, NULL, 0, &exover->over, NULL);
}

void send_packet_none(int user_id)
{
	sc_packet_none p;
	send_packet(user_id, &p);
}

void send_packet_login(int user_id)
{
	Player& u = player_cast(user_id);
	sc_packet_login p;
	p.x = u.x;
	p.y = u.y;
	send_packet(user_id, &p);
}

void send_packet_move(int user_id)
{
	Player& u = player_cast(user_id);
	sc_packet_move p;
	p.x = u.x;
	p.y = u.y;
	send_packet(user_id, &p);
}

void send_packet_enter(int user_id,int entrant_id)
{
	Player& u = player_cast(user_id);
	Player& ent = player_cast(entrant_id);
	sc_packet_enter p;
	p.id = entrant_id;
	p.x = ent.x;
	p.y = ent.y;

	u.m_mtx.lock();
	u.m_viewlist.insert(entrant_id);
	u.m_mtx.unlock();

	send_packet(user_id, &p);
}

void send_packet_leave(int user_id, int leaver_id)
{
	Player& u = player_cast(user_id);
	sc_packet_leave p;
	p.id = leaver_id;

	u.m_mtx.lock();
	u.m_viewlist.erase(leaver_id);
	u.m_mtx.unlock();

	send_packet(user_id, &p);
}

//================================================================================================================================//

inline void enterWorld_player(int user_id)
{
	// Searching near clinets
	// Temporary skip
	for (int i = USER_BEGIN; i < USER_END; ++i)
	{
		if (i != user_id)
		{
			Player& u = player_cast(i);
			if (u.m_clStatus != CL_STATUS::CS_ACTIVE)continue;
			send_packet_enter(i, user_id);
			send_packet_enter(user_id, i);
		}
	}

	// clientStatus를 어디서 볼까
}

void test_ping(int user_id)
{
	send_packet_none(user_id);
}

void move_process(int user_id, cs_packet_move* _packet)
{
	cs_packet_move packet = *_packet;
	Player& u = player_cast(user_id);;
	// valid check

	// calc movement
	if (packet.dir == dir_left)
	{
		u.x -= u.speed;
	}
	else if (packet.dir == dir_right)
	{
		u.x += u.speed;
	}
	if (packet.dir == dir_up)
	{
		u.y -= u.speed;
	}
	else if (packet.dir == dir_down)
	{
		u.y += u.speed;
	}

	// viewlist
	u.m_mtx.lock();
	unordered_set<int> old_vlist = u.m_viewlist;
	u.m_mtx.unlock();
	unordered_set<int> new_vlist;
	// 임시 나중에 Sector 단위로
	for (int i = USER_BEGIN; i < USER_END; ++i)
	{
		Player& oth = player_cast(i);
		if (oth.m_clStatus != CL_STATUS::CS_ACTIVE)continue;
		if (in_sight(user_id, oth.m_id) == false)continue;
		if (oth.m_id == user_id)continue;
		new_vlist.insert(oth.m_id);
	}
	send_packet_move(user_id);
}

void packet_process(int user_id, char* _packet)
{
	switch (_packet[1])
	{
	case pc2s_none:
	{
		cs_packet_none* packet = reinterpret_cast<cs_packet_none*>(_packet);
		test_ping(user_id);
		break;
	}
	case pc2s_move:
	{
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(_packet);
		move_process(user_id, packet);

	}
	default:
		break;
	}
}

void packet_assembly(int user_id,int io_byte)
{
	Player& u = player_cast(user_id);;
	EXOVER& r_o = u.m_recv_over;

	char* p = r_o.io_buf;
	int rest_byte = io_byte;
	int packet_size = 0;
	if (u.m_prev_size != 0)packet_size = u.m_packet_buf[0];
	while (rest_byte > 0)
	{
		if (packet_size == 0) packet_size = *p;
		if (u.m_prev_size + rest_byte >= packet_size)
		{
			int required = packet_size - u.m_prev_size;
			memcpy(u.m_packet_buf+u.m_prev_size, r_o.io_buf, required);
			p += required;
			rest_byte -= required;
			packet_size = 0;
			packet_process(user_id, u.m_packet_buf);
			u.m_prev_size = 0;
		}
		else 
		{
			memcpy(u.m_packet_buf + u.m_prev_size, p, rest_byte);
			u.m_prev_size += rest_byte;
			rest_byte = 0;
			p += rest_byte;
		}
	}
}

void disconnect(int user_id)
{
	//send_leave_packet()
	Player& u = player_cast(user_id);
	u.m_clStatus = CL_STATUS::CS_FREE;
	closesocket(u.m_socket);
	cout << "disconnect:" << user_id << "\n";
}

inline void accept_async()
{
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	EXOVER* accept_over = over_manager.get();
	accept_over->op = OP_ACCEPT;
	accept_over->c_socket = c_socket;
	AcceptEx(l_socket, c_socket, accept_over->io_buf, NULL,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		NULL, &accept_over->over);
}

void worker_thread()
{
	while (true)
	{
		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE);

		EXOVER* exover = reinterpret_cast<EXOVER*>(over);
		int user_id = static_cast<int>(key);

		Player& u = reinterpret_cast<Player&>(*clients[user_id]);

		switch (exover->op)
		{
		case OP_RECV:
		{
			if (io_byte == 0)disconnect(user_id);
			else
			{
				packet_assembly(user_id,io_byte);

				ZeroMemory(&u.m_recv_over.over, sizeof(u.m_recv_over.over));
				DWORD flags = 0;
				WSARecv(u.m_socket, &u.m_recv_over.wsabuf, 1, NULL, &flags, &u.m_recv_over.over, NULL);
			}
			break;
		}
		case OP_SEND:
		{
			if (io_byte == 0)disconnect(user_id);
			over_manager.release(exover);
			break;
		}
		case OP_ACCEPT:
		{
			int user_id = -1;
			SOCKET c_socket = exover->c_socket;

			// Search
			for (int i = USER_BEGIN; i < USER_END; ++i)
			{
				if (clients[i]->m_clStatus != CL_STATUS::CS_FREE)continue;
				clients[i]->m_clStatus = CL_STATUS::CS_ACTIVE;
				user_id = i;
				break;
			}

			if (user_id == -1)
			{
				closesocket(c_socket);
			}
			else
			{
				CreateIoCompletionPort(reinterpret_cast<HANDLE>(c_socket), g_iocp, user_id, 0);
				Player& nc = player_cast(user_id);
				nc.m_socket = c_socket;

				send_packet_login(user_id);
				cout << "Accept[" << user_id << "]\n";

				enterWorld_player(user_id);

				DWORD flags = 0;
				WSARecv(nc.m_socket, &nc.m_recv_over.wsabuf, 1, NULL, &flags, &nc.m_recv_over.over, NULL);
			}
			accept_async();
			break;
		}
		default:
			break;
		}
	}
}

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	initialize_clients();

	l_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN l_address;
	memset(&l_address, 0, sizeof(SOCKADDR_IN));
	l_address.sin_family = AF_INET;
	l_address.sin_port = htons(SERVERPORT);
	l_address.sin_addr.s_addr = htonl(INADDR_ANY);
	::bind(l_socket, reinterpret_cast<const sockaddr*>(&l_address), sizeof(l_address));

	listen(l_socket, SOMAXCONN);

	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, ULONG_MAX, 0);
	accept_async();

	vector<thread> worker_threads;
	for (int i = 0; i < THREAD_NUM; ++i)worker_threads.emplace_back(worker_thread);

	for (auto& th : worker_threads) th.join();
	WSACleanup();
};
