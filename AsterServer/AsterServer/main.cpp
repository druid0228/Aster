#include"Headers.h"
#include"ExoverManager.h"
#include"protocol.h"
#include"Objects.h"

HANDLE g_iocp;
SOCKET l_socket;
ExoverManager over_manager;

BaseObject* clients[MAX_CLEINTS];

void initialize_clients() 
{
	cout << "initialize_clients\n";
	for (int i = OBJECT_BEGIN; i < OBJECT_END; ++i)
	{
		// initilize_object
		clients[i] = new BaseObject;
		clients[i]->initialize();
	}
	cout << " --initialize objects\n";
	for (int i = NPC_BEGIN; i < NPC_END; ++i)
	{
		// initilize_npc
		clients[i] = new BaseObject;
		clients[i]->initialize();
	}
	cout << " --initialize npc\n";
	for (int i = USER_BEGIN; i < USER_END; ++i)
	{
		// initilize_clients
		clients[i] = new Player;
		clients[i]->initialize();
	}
	cout << " --initialize user\n";
}

void send_packet(int user_id, void* p)
{
	Player& u = reinterpret_cast<Player&>(clients[user_id]);
	char* buf = reinterpret_cast<char*>(p);

	EXOVER* exover = over_manager.get();
	exover->init(OP_SEND, buf[0]);

	memcpy(exover->io_buf, buf, buf[0]);
	WSASend(u.m_socket, &exover->wsabuf, 1, NULL, 0, &exover->over, NULL);
}

void send_packet_login()
{

}

void send_none_packet(int user_id)
{
	sc_packet_none p;
	send_packet(user_id, &p);
}

void test_ping(int user_id)
{
	send_none_packet(user_id);
}

void packet_process(int user_id, char* _packet)
{
	switch (_packet[1])
	{
	case pc2s_none:
	{
		cout << "pc2s_none\n";
		cs_packet_none* packet = reinterpret_cast<cs_packet_none*>(_packet);
		test_ping(user_id);
		break;
	}
	default:
		break;
	}
}

void packet_assembly(int user_id,int io_byte)
{
	Player& u = reinterpret_cast<Player&>(clients[user_id]);
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
	Player* u = reinterpret_cast<Player*>(clients[user_id]);
	u->m_clStatus = CL_STATUS::CS_FREE;
	closesocket(u->m_socket);
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

		Player& u = reinterpret_cast<Player&>(clients[user_id]);

		switch (exover->op)
		{
		case OP_RECV:
		{
			if (io_byte == 0)disconnect(user_id);
			else
			{
				cout << "OP_RECV[" << user_id << "]\n";
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
			cout << "OP_SEND[" << user_id << "]\n";
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
				Player& nc = reinterpret_cast<Player&>(clients[user_id]);

				nc.m_socket = c_socket;
				nc.x = 0;
				nc.y = 0;
				nc.m_prev_size = 0;
				nc.m_recv_over.init();

				cout << "nc socket:" << nc.m_socket << endl;
				DWORD flags = 0;
				WSARecv(nc.m_socket, &nc.m_recv_over.wsabuf, 1, NULL, &flags, &nc.m_recv_over.over, NULL);
				cout << "user_id: " << user_id << endl;
			}
			accept_async();

			cout << "ACCEPT\n";
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