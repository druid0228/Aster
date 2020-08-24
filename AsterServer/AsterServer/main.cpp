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

void send_packet(int user_id, Packet* p)
{
	Player* u = reinterpret_cast<Player*>(clients[user_id]);
	char* buf = reinterpret_cast<char*>(p);

	EXOVER* exover = over_manager.get();
	exover->Init(OP_SEND, buf[0]);
	memcpy(exover->io_buf, buf, buf[0]);
	WSASend(u->m_socket, &exover->wsabuf, 1, NULL, 0, &exover->over, NULL);
}

void send_packet_login()
{

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

		Player* u = reinterpret_cast<Player*>(clients[user_id]);

		switch (exover->op)
		{
		case OP_RECV:
		{
			if (io_byte == 0)disconnect(user_id);
			else
			{

				ZeroMemory(&u->m_recv_over.over, sizeof(u->m_recv_over.over));
				DWORD flags = 0;
				WSARecv(u->m_socket, &u->m_recv_over.wsabuf, 1, NULL, &flags, &u->m_recv_over.over, NULL);
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
				Player& nc = reinterpret_cast<Player&>(clients[user_id]);

				nc.m_socket = c_socket;
				nc.m_recv_over.Init();

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