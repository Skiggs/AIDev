#include"Floor.h"

Floor::Floor(GameDataRef data) : _data(data)
{
	_ground.setSize(sf::Vector2f(400.0, 20.0));
	//_ground.setPosition(sf::Vector2f(0.0 , 600.0));
	_ground.setFillColor(sf::Color::Red);
}

void Floor::Update(float dt)
{

}

void Floor::Draw()
{
	_data->window.draw(_ground);
}