#pragma once
#include<SFML/Graphics.hpp>
#include "Game.h"
#include "Item.h"
#include "PlayerHandler.h"


enum class MoveDirection
{
	LEFTSIDE,
	RIGHTSIDE,
	UPSIDE,
	DOWNSIDE
};

struct ItemLauncherLine
{
	sf::Vector2f p1, p2, p3;
};




class Player
{
public:
	PlayerHandler *playerHandler;
	sf::Vector2f p1, p2, closestPointToDoor,direction;
	float targetLength = 0.f;
	float nextHitTime = 0.f;
	sf::RectangleShape healthrect;
	ItemLauncherLine itemLauncher;
	Player(GameDataRef data);
	void Init();
	void Move(MoveDirection mv);
	void SetPosition(sf::Vector2f pos) { 
		circle.setPosition(pos); 
		p1 = circle.getPosition(); 
		healthrect.setPosition(sf::Vector2f(pos.x, pos.y - 30.f));
	}
	void Rotate(float angle);
	void SetDirection(sf::Vector2f dir) { direction = dir; }
	sf::Vector2f GetDirection() { return direction; }
	sf::Vector2f getPostion() {
		return circle.getPosition();
	}
	sf::CircleShape GetShape() { return circle; }
	void Update(float dt);
	void AddItem() 
	{
		if (nearByItem->CheckTrait(ItemTrait::PICKABLE))
		{
			nearByItem->IsPicked = true;
			_items.push_back(nearByItem);

			if (nearByItem->CheckTrait(ItemTrait::EXPLODESONHIT))
			{
				itemLauncher.p1 = GetPosition();
				itemLauncher.p2 = GetPosition();
			}
		}
	}
	sf::Vector2f GetPosition() { return circle.getPosition(); }
	void Draw();
	void ReduceHealth() { 
		health--;
		SeetHealthBarSize();
	}
	void IncreaseHealthByKit() { 
		health = 10; 
		SeetHealthBarSize();
	}
	void SeetHealthBarSize()
	{
		healthrect.setSize(sf::Vector2f(health*9.f, 8.f));
	}
	int GetHealth() const { return health; }
	Item* nearByItem;
	bool isdead = false;
	void Kill();
	void SetID(int id) { m_id = id; }
	int GetID() const { return m_id; }
	std::vector<Item*> _items;
	void ReduceHealthOfPlayer();
	void SetHealthBarSize();

private:

	GameDataRef _data;
	int m_id;
	int health = 5;
	sf::Vector2f mousPos;
	sf::CircleShape circle;
};