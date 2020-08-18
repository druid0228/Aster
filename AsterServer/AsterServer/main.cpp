#include "Headers.h"
#include "ExoverManager.h"

HANDLE g_iocp;
SOCKET l_socket;
ExoverManager over_manager;

void worker_thread()
{
	while (true)
	{
		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE);

		EXOVER* exover = reinterpret_cast<EXOVER*>(over);

		switch (exover->op)
		{
		case OP_RECV:
		{
			break;
		}
		case OP_SEND:
		{
			break;
		}
		case OP_ACCEPT:
		{
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
	SOCKET c_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	EXOVER* accept_over = over_manager.get();
	accept_over->op = OP_ACCEPT;
	accept_over->c_socket = c_socket;
	AcceptEx(l_socket, c_socket, accept_over->io_buf, NULL, 
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, 
		NULL, &accept_over->over);

	vector<thread> worker_threads;
	for (int i = 0; i < THREAD_NUM; ++i)worker_threads.emplace_back(worker_thread);

	for (auto& th : worker_threads) th.join();
	WSACleanup();
};