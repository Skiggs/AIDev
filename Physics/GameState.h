#pragma once

#include<SFML/Graphics.hpp>
#include"State.h"
#include"Game.h"
#include"Ball.h"
#include"Floor.h"
#include<memory>
#include"Collision.h"

class GameState :public State
{
public:
	GameState(GameDataRef data);
	void Init();

	void HandleInput();
	void Update(float dt);
	void Draw(float dt);

private:
	GameDataRef _data;
	Ball *ball;
	Floor *floor;
	Collision collision;
};
