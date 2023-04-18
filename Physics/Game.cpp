#include"Game.h"
#include"MainMenu.h"

Game::Game(int width, int height, std::string title)
{
	srand(time(NULL));
	_data->window.create(sf::VideoMode(width, height), title, sf::Style::Close | sf::Style::Titlebar);
	_data->machine.AddState(StateRef(new MainMenuState(this->_data)));

	this->Run();
}

void Game::Run()
{
	float newTime, frameTime, interpolation;

	float currenTime = _data->_Globalclock.getElapsedTime().asSeconds();

	float accumulator = 0;

	while (_data->window.isOpen())
	{
		this->_data->machine.ProcessState();

		newTime = _data->_Globalclock.getElapsedTime().asSeconds();
		frameTime = newTime - currenTime;

		if (frameTime > 0.25f)
		{
			frameTime = 0.25f;
		}

		currenTime = newTime;
		accumulator += frameTime;

		while (accumulator >= dt)
		{
			this->_data->machine.GetActiveState()->HandleInput();
			this->_data->machine.GetActiveState()->Update(dt);

			accumulator -= dt;
		}

		interpolation = accumulator / dt;
		this->_data->window.setFramerateLimit(60);
		this->_data->machine.GetActiveState()->Draw(interpolation);
	}
}