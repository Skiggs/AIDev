#pragma once

#include"Ball.h"
#include"Floor.h"

class Collision
{
public:
	Collision();
	bool CheckCollision(sf::FloatRect tcir, sf::FloatRect tfloor);
};