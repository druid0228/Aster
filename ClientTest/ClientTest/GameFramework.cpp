#include"globalHeader.h"
#include "GameFramework.h"

void GameFramework::Initialize()
{
	sf::err().rdbuf(NULL);
	sf_window.create(sf::VideoMode(CLIENTWIDHT, CLIENTHEIGHT), "Aster");

	sf_socket.connect("127.0.0.1", 9000);
	sf_socket.setBlocking(false);

	sf_window.setView(sf_view);
	sf_view.setCenter(400, 200);
	sf_view.zoom(0.5);

	gid = 0;

	m_ground.Initialize();
	m_player.Initialize(gid++);

	std::cout << "Initialize\n";
}

void GameFramework::Destroy()
{
	std::cout << "Destroy\n";
}

int GameFramework::Run()
{
	Initialize();
	while (sf_window.isOpen())
	{
		WindowEvent();
		Network();
		Update();
		Draw();
	}
	Destroy();
	return EXIT_SUCCESS;
}

void GameFramework::Update()
{
	// For Test
	static int cnt = 0;
	if (cnt++ >= 60)
	{
		m_player.Update();
		if (m_others.size())
			for (auto ot : m_others) ot.second.Update();
		cnt = 0;
	}
}

void GameFramework::Draw()
{
	sf_window.clear();
	sf_window.setView(sf_view);
	// Draw Here
	m_ground.Draw(sf_window, sf::Vector2f(-m_player.x, -m_player.y));
	if (m_others.size())
	{
		for (auto ot : m_others)
		{
			ot.second.Draw(sf_window);
		}
	}
	m_player.Draw(sf_window);

	//
	sf_window.display();
}

void GameFramework::Network()
{
	char data[MAX_BUF_SIZE];
	size_t received;
	sf::Socket::Status ret = sf_socket.receive(data, MAX_BUF_SIZE, received);

	if (ret == sf::Socket::Error)
	{
		std::cout << "sf::Socket::Error\n";
	}
	if (ret == sf::Socket::Disconnected)
	{
		std::cout << "sf::Socket::Disconnected\n";
	}

	if (ret == sf::Socket::Done || ret == sf::Socket::Partial)
	{
		if (received > 0) {
			Packet_assembler(data, received);
		}
	}
}

void GameFramework::WindowEvent()
{
	static sf::Event event;
	while (sf_window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			sf_window.close();
			break;
		case sf::Event::KeyPressed:
			KeyboardInput();
			break;
		default:
			break;
		}
	}
}

void GameFramework::KeyboardInput()
{
	MoveInput();
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
	{
		sf_view.zoom(1.1);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		sf_view.zoom(0.9);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		TestPing();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
	{
		/*Object n;
		n.Initialize(gid++);
		n.x += m_player.x + 10;
		n.y += m_player.y + 10;
		m_others.emplace_back(n);*/

	}
}

void GameFramework::MoveInput()
{
	char dir = dir_none;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		dir |= dir_left;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		dir |= dir_right;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		dir |= dir_up;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		dir |= dir_down;

	cs_packet_move packet;
	packet.dir = dir;
	Send(&packet);
}

void GameFramework::MoveProcess(sc_packet_move* _packet)
{
	sc_packet_move packet = *_packet;
	if (m_pid == packet.id) {
		m_player.x = packet.x;
		m_player.y = packet.y;
	}
	else {
		m_others[packet.id].id = packet.id;
		m_others[packet.id].x = packet.x;
		m_others[packet.id].y = packet.y;
	}
}

void GameFramework::EnterProcess(sc_packet_enter* _packet)
{
	sc_packet_enter packet = *_packet;
	Object oth;
	oth.id = packet.id;
	oth.Initialize(oth.id);
	oth.x = packet.x;
	oth.y = packet.y;
	cout << oth.id << " " << oth.x << " " << oth.y << "\n";
	m_others.insert(make_pair(oth.id, oth));
}

void GameFramework::TestPing()
{
	std::cout << "TestPing\n";
	cs_packet_none packet;

	Send(&packet);
}

size_t GameFramework::Send(void* _packet)
{
	char* packet = reinterpret_cast<char*>(_packet);
	size_t sent;
	sf_socket.send(packet, packet[0], sent);
	return sent;
}

void GameFramework::ProcessPacket(char* _packet)
{
	switch (_packet[1])
	{
	case ps2c_login:
	{
		sc_packet_login* packet = reinterpret_cast<sc_packet_login*>(_packet);
		LoginProcess(packet);
		break;
	}
	case ps2c_disconnect:
	{
		std::cout << "ps2c_disconnect\n";
		break;
	}
	case ps2c_enter:
	{
		sc_packet_enter* packet = reinterpret_cast<sc_packet_enter*>(_packet);
		EnterProcess(packet);
		std::cout << "ps2c_enter\n";
		break;
	}
	case ps2c_move:
	{
		sc_packet_move* packet = reinterpret_cast<sc_packet_move*>(_packet);
		MoveProcess(packet);
		break;
	}
	case ps2c_attack:
	{
		std::cout << "ps2c_attack\n";
		break;
	}
	default:
		std::cout << "default\n";
		break;
	}
}

void GameFramework::Packet_assembler(char* data, size_t io_byte)
{
	char* p = data;
	static char packet_buffer[MAX_BUF_SIZE];
	static size_t m_prev_size = 0;
	static size_t packet_size = 0;

	while (io_byte > 0)
	{
		if (packet_size == 0) packet_size = p[0];
		if (io_byte + m_prev_size >= packet_size)
		{
			size_t diff = packet_size - m_prev_size;
			memcpy(packet_buffer + m_prev_size, p, diff);
			ProcessPacket(packet_buffer);
			p += diff;
			io_byte -= diff;
			packet_size = 0;
			m_prev_size = 0;
		}
		else 
		{
			memcpy(packet_buffer + m_prev_size, p, io_byte);
			m_prev_size += io_byte;
			io_byte = 0;
		}
	}
}

void GameFramework::LoginProcess(sc_packet_login* _packet)
{
	sc_packet_login packet = *_packet;
	m_player.x = packet.x;
	m_player.y = packet.y;
	m_pid = packet.id;
	std::cout << "LoginProcess\n";
	std::cout << " -pos:(" << m_player.x << "," << m_player.y << ")\n";
}

