#pragma once
#pragma warning(disable:26495)

enum {
	ps2c_none,
	ps2c_login, ps2c_disconnect, ps2c_enter,
	ps2c_move, ps2c_attack
};

enum {
	pc2s_none,
};

#pragma pack(push,1)
struct Packet
{
	int	 size;
	char type;
};

// test
struct loginPacket : Packet
{
	loginPacket() { size = sizeof(loginPacket), type = ps2c_login; }
	int id;

	int x, y;
	int hp[10];
	unsigned char mainIdx;

	// 주 캐릭터 이외의 위치는 상대위치로 전달
	char als_dx[9], als_dy[9];

	int elseSomething;

};

struct disconnectPacket : Packet
{
	disconnectPacket() { size = sizeof(disconnectPacket), type = ps2c_disconnect; }


};

// Server to Client Packet
struct sc_packet_none : Packet
{
	sc_packet_none() { size = sizeof(sc_packet_none), type = ps2c_none; }

};

struct sc_packet_login : Packet
{
	sc_packet_login() { size = sizeof(sc_packet_login), type = ps2c_login; }
	int x, y;
};


// Client to Server Packet
struct cs_packet_none : Packet
{
	cs_packet_none() { size = sizeof(cs_packet_none), type = pc2s_none; }
};

#pragma pack(pop)