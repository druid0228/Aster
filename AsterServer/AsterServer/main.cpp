#include<iostream>
#include<WS2tcpip.h>

#pragma comment(lib,"Ws2_32.lib")
#define SERVERPORT	9000

using namespace std;

HANDLE g_iocp;

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET l_socket;

	l_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN l_address;
	memset(&l_address, 0, sizeof(SOCKADDR_IN));
	l_address.sin_family = AF_INET;
	l_address.sin_port = htons(SERVERPORT);
	l_address.sin_addr.s_addr = htonl(INADDR_ANY);
	::bind(l_socket, reinterpret_cast<const sockaddr*>(&l_address), sizeof(l_address));

	listen(l_socket, SOMAXCONN);

	SOCKET c_sock = accept(l_socket, NULL, NULL);
	if (c_sock != INVALID_SOCKET)
	{
		cout << "connect " << c_sock << "\n";
	}
	else {
		cout << "INVALID_SOCKET\n";
	}




	/*g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(l_socket), g_iocp, ULONG_MAX, 0);*/



	WSACleanup();
};