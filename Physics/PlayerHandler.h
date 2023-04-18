#pragma once

#include"Message.h"

class Player;

class PlayerHandler
{
public:
	PlayerHandler() {};
	~PlayerHandler() {};
	Player* parent;
	void GetHurt(const Message &msg);
};