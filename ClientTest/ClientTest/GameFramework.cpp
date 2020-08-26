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

	m_ground.Initialize();
	m_object.Initialize();


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
		m_object.Update();
		cnt = 0;
	}
}

void GameFramework::Draw()
{
	sf_window.clear();
	sf_window.setView(sf_view);
	// Draw Here
	m_ground.Draw(sf_window);
	m_object.Draw(sf_window);

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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		std::cout << "KeyLeft\n";
		sf_view.move(sf::Vector2f(-10.0f, 0.f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		std::cout << "KeyRight\n";
		sf_view.move(sf::Vector2f(10.0f, 0.f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		std::cout << "KeyUp\n";
		sf_view.move(sf::Vector2f(0.f, -10.0f));
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		std::cout << "KeyDown\n";
		sf_view.move(sf::Vector2f(0.f, 10.0f));
	}
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
}

void GameFramework::TestPing()
{
	std::cout << "TestPing\n";
	cs_packet_none p;

	char* packet = reinterpret_cast<char*>(&p);
	size_t sent;
	sf_socket.send(packet, packet[0], sent);
}

void GameFramework::ProcessPacket(char* packet)
{
	switch (packet[1])
	{
	case ps2c_login:
	{
		std::cout << "ps2c_login\n";
		break;
	}
	case ps2c_disconnect:
	{
		std::cout << "ps2c_disconnect\n";
		break;
	}
	case ps2c_enter:
	{
		std::cout << "ps2c_enter\n";
		break;
	}
	case ps2c_move:
	{
		std::cout << "ps2c_move\n";
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

	std::cout << "io_byte:" << io_byte << "\n";

	while (io_byte > 0)
	{
		if (packet_size == 0) packet_size = p[0];
		std::cout << "size:" << packet_size << "\n";
		if (io_byte + m_prev_size >= packet_size)
		{
			size_t diff = packet_size - m_prev_size;
			std::cout << "diff:" << diff << "\n";
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

