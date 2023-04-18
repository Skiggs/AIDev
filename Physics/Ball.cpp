#include"Ball.h"
#include"math.h"


Ball::Ball(GameDataRef data) : _data(data),iniVelo(sf::Vector2f(0.0,0.5)),finalVelo(sf::Vector2f(0.0,0.0))
{
	currentTime = _data->_Globalclock.getElapsedTime().asSeconds();
	gravity = sf::Vector2f(0.0, 5.0);
	this->curState = BallState::Down;
}

void Ball::Init()
{
	circle.setRadius(20.0);
	circle.setFillColor(sf::Color::Green);
}

void Ball::SetPosition(sf::Vector2f pos)
{
	circle.setPosition(pos);
	curPos = circle.getPosition();
}

void Ball::Update(float dt)
{
	newTime = currentTime + dt;
	finalVelo = iniVelo + (gravity * newTime);
	newPos = curPos + (finalVelo * newTime);

	this->SetPosition(newPos);
	currentTime = newTime;
	iniVelo = finalVelo;
	curPos = this->GetPosition();

	if (iniVelo.y > 0)
	{
		curState = BallState::Down;
	}

	if (iniVelo.y < 0.1 && iniVelo.y > 0)
	{
		printf("zero");
	}
}

void Ball::Draw()
{
	_data->window.draw(circle);
}

void Ball::ReverseGravity()
{
	curState = BallState::UP;
	curPos = circle.getPosition();
	currentTime = _data->_Globalclock.getElapsedTime().asSeconds();
	newTime = 0;
	iniVelo.y *= -1;
	finalVelo = sf::Vector2f(0.0, 0.0);
}