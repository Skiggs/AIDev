#include"HandlerManager.h"
#include"Enemyhandler.h"
#include"Enemy.h"
#include"MathFunc.h"

HandlerManager::HandlerManager()
{

}

HandlerManager::~HandlerManager()
{

}

void HandlerManager::Register(Enemy *enemy)
{
	RegisteredEnemies.push_back(enemy);
}

void HandlerManager::GetWalls(std::vector<Wall*> mapWalls)
{
	MapWalls = mapWalls;
}

void HandlerManager::Register(Item *item)
{
	RegisteredItems.push_back(item);
}

void HandlerManager::Register(Player *player)
{
	RegisteredPlayer = player;
}

void HandlerManager::Unregister(Enemy *enemy)
{
	std::vector<Enemy*>::iterator it;
	int index = 0;
	for (it = RegisteredEnemies.begin(); it < RegisteredEnemies.end(); it++)
	{
		if (*it == enemy)
		{
			break;
		}
		index++;
	}

	RegisteredEnemies.erase(RegisteredEnemies.begin() + index);
}

void HandlerManager::Unregister(Item *item)
{
	std::vector<Item*>::iterator it;
	int index = 0;
	for (it = RegisteredItems.begin(); it < RegisteredItems.end(); it++)
	{
		if (*it == item)
		{
			break;
		}
		index++;
	}

	RegisteredItems.erase(RegisteredItems.begin() + index);
}

void HandlerManager::Unregister(Player *player)
{
	RegisteredPlayer = nullptr;
}

void HandlerManager::HandleMessage(const Message &msg)
{
	if (msg.receiver != -1)
	{
		containerForAllMessageType[msg.msg][msg.receiver].second(msg);
	}

	else
	{
		auto map = containerForAllMessageType[msg.msg];

		IDtoFunc::iterator mapIt;
		if (msg.range > 0.f)
		{
			if (msg.msg == MessageType::DIEFROMGRENADE || msg.msg == MessageType::RUNAWAYFROMGRENADE)
			{
				for (mapIt = map.begin(); mapIt != map.end(); ++mapIt)
				{
					sf::Vector2f receiverPos = *mapIt->second.first;
					float DistFromRec = Distance(receiverPos, msg.SenderPosition);
					if (DistFromRec < msg.range)
					{

						bool isColliding = false;
						for (auto itr : MapWalls)
						{
							if (areLinesColliding(receiverPos, msg.SenderPosition, itr->p1, itr->p2))
							{
								isColliding = true;
								break;
							}
						}
						if (!isColliding)
						{
							mapIt->second.second(msg);
						}

					}
				}
			}
			else if (msg.msg == MessageType::SAVEFRIENDSASS)
		  {
				float closestPos = -1.f;
				std::function<void(const Message&)> closestEnemyTocall;
				for (mapIt = map.begin(); mapIt != map.end(); ++mapIt)
				{
					sf::Vector2f receiverPos = *mapIt->second.first;
					float DistFromRec = Distance(receiverPos, msg.SenderPosition);
					if (DistFromRec < msg.range)
					{
						if (closestPos == -1.f && mapIt->first != msg.sender)
						{
							closestPos = DistFromRec;
							closestEnemyTocall = mapIt->second.second;
						}

						else if (DistFromRec < closestPos && mapIt->first != msg.sender)
						{
							closestPos = DistFromRec;
							closestEnemyTocall = mapIt->second.second;
						}
					}
				}
				if(closestEnemyTocall!=nullptr)closestEnemyTocall(msg);
			}
			else
			{
				for (mapIt = map.begin(); mapIt != map.end(); ++mapIt)
				{
					if (Distance(*mapIt->second.first, msg.SenderPosition) < msg.range)
					{
						mapIt->second.second(msg);
					}
				}
			}
		}

		else
		{
			for (mapIt = map.begin(); mapIt != map.end(); ++mapIt)
			{
				mapIt->second.second(msg);
			}
		}
	}
}


//else
	//{
	//	Item* itemSender = GetItemByID(msg.sender);
	//	Enemy* enemySender = GetEnemyByID(msg.sender);
	//	sf::Vector2f Senderpos = (enemySender == nullptr ? (itemSender == nullptr ? RegisteredPlayer->GetPosition() : itemSender->GetPosition()) : enemySender->GetPosition());

	//  IDtoFunc::iterator mapIt;
	//  for (mapIt = containerForAllMessageType[msg.msg].begin(); mapIt != containerForAllMessageType[msg.msg].end(); ++mapIt)
	//  {
	//	  Item* itemReceiver = GetItemByID(mapIt->first);
	//	  Enemy* enemyReceiver = GetEnemyByID(mapIt->first);
	//	  sf::Vector2f Receiverpos = (enemyReceiver == nullptr ? (itemReceiver == nullptr ? RegisteredPlayer->GetPosition() : itemReceiver->GetPosition()) : enemyReceiver->GetPosition());
	//	  
 //         if (Distance(Receiverpos, Senderpos) < msg.range)
	//	  {
	//		  mapIt->second(msg);
	//	  }
	//  }
	//}

Enemy* HandlerManager::GetEnemyByID(int ID)
{
	auto FindEnemyByID = [ID](Enemy* e1) -> bool
	{
		return e1->GetID() == ID;
	};

	auto result = std::find_if(RegisteredEnemies.begin(), RegisteredEnemies.end(), FindEnemyByID);

	return *result;
}

Item* HandlerManager::GetItemByID(int ID)
{
	Item *itemById = nullptr;
	auto FindItemByID = [ID, &itemById](Item* e1) -> bool
	{
		if (e1->GetID() == ID) itemById = e1;
		return e1->GetID() == ID;
	};

	std::find_if(RegisteredItems.begin(), RegisteredItems.end(), FindItemByID);

	return itemById;
}

Player* HandlerManager::GetPlayerByID(int ID)
{
	if (RegisteredPlayer->GetID() == ID)
	{
		return RegisteredPlayer;
	}

	return nullptr;
}


void HandlerManager::RegisterWithFunction(MessageType MT, std::pair<int, funcWithDis> cb)
{
	containerForAllMessageType[MT].insert(cb);
}