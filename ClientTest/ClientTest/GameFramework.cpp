#include"globalHeader.h"
#include "GameFramework.h"

void GameFramework::Initialize()
{
	sf::err().rdbuf(NULL);
	sf_window.create(sf::VideoMode(CLIENTWIDHT, CLIENTHEIGHT), "Aster");
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
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
	{
		NetTest();
	}
}

void GameFramework::NetTest()
{
	std::cout << "NetTest\n";
	socket.connect("127.0.0.1", 9000);

}

