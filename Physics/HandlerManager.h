#pragma once

#include"Player.h"
#include"Item.h"
#include<functional>
#include<map>
#include<array>


class Enemy;
class Wall;

class HandlerManager
{
public:
	HandlerManager();
	~HandlerManager();
	void Register(Enemy *enemy);
	void Register(Item *item);
	void Register(Player *player);
	void Unregister(Enemy *enemy);
	void Unregister(Item *item);
	void Unregister(Player *player);
	void GetWalls(std::vector<Wall*> mapWalls);
	Enemy* GetEnemyByID(int ID);
	Item* GetItemByID(int ID);
	Player* GetPlayerByID(int ID);
	void HandleMessage(const Message &msg);
	typedef std::pair<sf::Vector2f*, std::function<void(const Message&)>> funcWithDis;
	typedef std::map<int, funcWithDis> IDtoFunc;
	typedef std::array<IDtoFunc, MT_COUNT> MTcontainer;
	void RegisterWithFunction(MessageType, std::pair<int,funcWithDis> cb);
	MTcontainer containerForAllMessageType;
private:
	std::vector<Item*> RegisteredItems;
	std::vector<Enemy*> RegisteredEnemies;
	std::vector<Wall*> MapWalls;
	Player *RegisteredPlayer;
};