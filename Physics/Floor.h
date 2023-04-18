#pragma once

#include"Game.h"
#include<SFML/Graphics.hpp>

class Floor
{
public:
	Floor(GameDataRef data);
	sf::FloatRect getSize() { return _ground.getGlobalBounds(); }
	void Draw();
	void Update(float dt);
private:
	GameDataRef _data;
	sf::RectangleShape _ground;
};