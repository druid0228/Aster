#pragma once
enum {
	p_none,
	p_login, p_disconnect,
	p_move, p_attack
};

#pragma pack(push,1)
struct Packet
{
	int	 size;
	char type;
};

struct loginPacket : Packet
{
	loginPacket() { size = sizeof(loginPacket), type = p_login; }
	int id;

	int x, y;
	int hp[10];
	unsigned char mainIdx;

	// �� ĳ���� �̿��� ��ġ�� �����ġ�� ����
	char als_dx[9], als_dy[9];

	int elseSomething;

};

struct disconnectPacket : Packet
{
	disconnectPacket() { size = sizeof(disconnectPacket), type = p_disconnect; }


};

#pragma pack(pop)