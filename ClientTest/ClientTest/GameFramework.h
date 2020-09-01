#include"globalHeader.h"

class Ground
{
public:
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	void Initialize()
	{
		m_texture.loadFromFile("testBackground.jpg");
		m_sprite.setTexture(m_texture);
	}
	void Draw(sf::RenderWindow& window, sf::Vector2f v)
	{
		m_sprite.setPosition(v);
		window.draw(m_sprite);
	}
};

class Object
{
public:
	int x, y;
	int sp_x, sp_y;
	const int size = 16;
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	void Initialize()
	{
		sp_x = 10;
		sp_y = 10;
		m_texture.loadFromFile("hero.png");
		m_sprite.setTexture(m_texture);
	}
	void Update()
	{
		sp_x += 1;
		if (sp_x > 2)
		{
			sp_x = 0;
			sp_y += 1;

			if (sp_y > 3)sp_y = 0;
		}
	}
	void Draw(sf::RenderWindow& window)
	{
		m_sprite.setTextureRect(sf::IntRect(
			sp_x * size, sp_y * size,  size, size));
		m_sprite.setPosition(400,200);
		window.draw(m_sprite);
	}
};

class GameFramework
{
	sf::RenderWindow sf_window;
	sf::View sf_view;

	sf::TcpSocket sf_socket;

	Ground m_ground;
	Object m_player;
public:
	void Initialize();
	void Destroy();

	int  Run();
	void Update();
	void Draw();
	void Network();
	void WindowEvent();
	void KeyboardInput();

public:
	void MoveInput();

	void TestPing();

public:
	size_t Send(void* _packet);
	void ProcessPacket(char* packet);
	void Packet_assembler(char* data,size_t io_byte);
	void LoginProcess(sc_packet_login* packet);
	void MoveProcess(sc_packet_move* packet);
};

