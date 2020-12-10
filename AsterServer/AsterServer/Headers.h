#pragma once
#include<iostream>
#include<WS2tcpip.h>
#include<MSWSock.h>
#include<vector>
#include<unordered_set>
#include<thread>
#include<atomic>
#include<mutex>


#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")

using namespace std;

#define SERVERPORT	9000

constexpr auto THREAD_NUM = 4;
constexpr auto MAX_BUF_SIZE = 1024;
constexpr auto MAX_PACKET_SIZE = 255;
//================================================================================================================================//
constexpr auto WORLD_WIDTH = 100;
constexpr auto WORLD_HEIGHT = 100;
constexpr auto VIEW_RANGE = 70;

//================================================================================================================================//
constexpr auto MAX_OBJECT = 10;
constexpr auto OBJECT_BEGIN = 0;
constexpr auto OBJECT_END = OBJECT_BEGIN + MAX_OBJECT;

constexpr auto MAX_NPC = 10;
constexpr auto NPC_BEGIN = OBJECT_END;
constexpr auto NPC_END = NPC_BEGIN + MAX_NPC;

constexpr auto MAX_MONSTER = 100;
constexpr auto MONSTER_BEGIN = NPC_END;
constexpr auto MONSTER_END = MONSTER_BEGIN + MAX_MONSTER;

constexpr auto MAX_USER = 100;
constexpr auto USER_BEGIN = MONSTER_END;
constexpr auto USER_END = USER_BEGIN + MAX_USER;

constexpr auto MAX_CLEINTS = MAX_OBJECT + MAX_NPC + MAX_USER;
//================================================================================================================================//

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

	// initialize EXOVER (ZeroMem OVERLAPPED,set WSABUF,set OP)
	inline void init(OVEROP _op = OP_RECV, int buflen = MAX_BUF_SIZE)
	{
		ZeroMemory(&over, sizeof(over));
		wsabuf.buf = io_buf;
		wsabuf.len = buflen;
		op = _op;
	}
};

// Client status (not user)
enum class CL_STATUS
{
	CS_FREE, CS_SLEEP, CS_ACTIVE
};

//================================================================================================================================//