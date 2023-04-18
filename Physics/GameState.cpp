#include "GameState.h"


GameState::GameState(GameDataRef data) : _data(data)
{
	printf("Loaded");
	ball = new Ball(_data);
	ball->Init();
	ball->SetPosition(sf::Vector2f(300.0, 100.0));
	floor = new Floor(_data);
}

void GameState::Init()
{
	
}

void GameState::HandleInput()
{
	sf::Event event;

	while (_data->window.pollEvent(event))
	{
		if (event.type == sf::Event::EventType::Closed)
		{
			_data->window.close();
		}

	}
}

void GameState::Update(float dt)
{
	if (ball->Getstate() == BallState::Down)
	{
		//if (collision.CheckCollision(ball->getSize(), floor->getSize()))
		{
			ball->ReverseGravity();
		}
	}

	ball->Update(dt);
}

void GameState::Draw(float dt)
{
	_data->window.clear();
	ball->Draw();
	floor->Draw();
	_data->window.display();
}