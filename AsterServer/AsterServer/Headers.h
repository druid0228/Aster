#pragma once
#include<iostream>
#include<WS2tcpip.h>
#include<MSWSock.h>
#include<vector>
#include<thread>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

using namespace std;

#define SERVERPORT	9000

constexpr auto THREAD_NUM = 4;
constexpr auto MAX_BUF_SIZE = 1024;

enum OVEROP {
	OP_RECV,OP_SEND,OP_ACCEPT
};

struct EXOVER {
	WSAOVERLAPPED over;
	OVEROP op;
	char io_buf[MAX_BUF_SIZE];
	union 
	{
		WSABUF wsabuf;
		SOCKET c_socket;
	};
};