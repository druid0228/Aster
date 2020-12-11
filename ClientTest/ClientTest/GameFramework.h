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
	int sp_x, sp_y;
	const int size = 16;
	sf::Texture m_texture;
	sf::Sprite m_sprite;
	string m_texture_name;
	bool initialized;
public:
	int id;
	int x, y;

	int midWidth = 400;
	int midHeight = 200;

	Object() : initialized(false)
	{}

	void Initialize(int _id,string texture = "hero.png")
	{
		id = _id;
		sp_x = 10;
		sp_y = 10;
		m_texture_name = texture;
		bool success = m_texture.loadFromFile(texture);
		if (false == success) {
			cout << "Image loading failed!!\n";
		}
		m_sprite.setTexture(m_texture);
		initialized = true;
		cout << "object Initialized id:" << _id << "\n";
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
		if (false == initialized) {
			Initialize(id);
		}
		m_sprite.setTextureRect(sf::IntRect(
			sp_x * size, sp_y * size,  size, size));
		m_sprite.setPosition(midWidth + x, midHeight + y);
		window.draw(m_sprite);
	}

	bool operator==(const Object& rhs) const{ return id == rhs.id; }
	Object operator=(const Object& rhs)
	{
		sp_x = rhs.sp_x;
		sp_y = rhs.sp_y;
		initialized = rhs.initialized;

		id = rhs.id;
		x = rhs.x;
		y = rhs.y;
		midWidth = rhs.midWidth;
		midHeight = rhs.midHeight;
		m_texture = rhs.m_texture;
		m_texture_name = rhs.m_texture_name;
		m_sprite = rhs.m_sprite;
		bool success = m_texture.loadFromFile(m_texture_name);
		if (success == false) {
			cout << "texture_name: " << m_texture_name << "\n";
			cout << "failed loadFromFile in Operator=\n";
		}
		else {
			cout << "SUCCESS\n";
		}

		m_sprite.setTexture(m_texture);
		return *this;
	}
	
};

class GameFramework
{
	sf::RenderWindow sf_window;
	sf::View sf_view;

	sf::TcpSocket sf_socket;

	Ground m_ground;
	Object m_player;
	int m_pid;
	map<int,Object> m_others;

	int gid;
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

private:
	size_t Send(void* _packet);
	void ProcessPacket(char* packet);
	void Packet_assembler(char* data,size_t io_byte);
	void LoginProcess(sc_packet_login* packet);
	void MoveProcess(sc_packet_move* packet);
	void EnterProcess(sc_packet_enter* packet);
};


