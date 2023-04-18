#pragma once
#include"Game.h"
#include"Item.h"
#include<array>

struct WallWinDoorData
{ 
	std::string m_name;
	sf::Vector2f pos,pos2;
};

struct ItemData
{
	ItemData()
	{
		for (int i = THROWABLE; i < COUNT; i++)
		{
			_traits[i] = false;
		}
	}
	std::string spriteName;
	std::string owner;
	std::string m_name, fileName;
	sf::Vector2f position,scale;
	std::array<bool, COUNT> _traits;
	std::string traitName;
};

struct EnemyTraits
{
	std::string name;
	sf::Vector2f position;
	std::vector<sf::Vector2f> route;
	int routeSize;
};