#pragma once

#include<SFML/Graphics.hpp>
#include "Game.h"

enum BallState
{
	UP,
	Down
};

class Ball
{
public:
	BallState curState;
	Ball(GameDataRef data);
	void Init();
	void SetPosition(sf::Vector2f pos);
	void Update(float dt);
	sf::Vector2f GetPosition() { return circle.getPosition(); }
	sf::FloatRect getSize() { return circle.getGlobalBounds(); }
	void ReverseGravity();
	void Draw();
	BallState Getstate() { return curState; }

private:

	GameDataRef _data;

	sf::CircleShape circle;
	
	float currentTime, newTime;

	sf::Vector2f iniVelo, finalVelo, gravity, curPos, newPos;
};