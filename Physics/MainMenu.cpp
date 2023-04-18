
#include "MainMenu.h"
#include "GameState.h"
#include<iostream>
#include<stdio.h>
#include<fstream>
#include"PathFinder.h"
#include<math.h>
#include<string>
#include"EnemyHandler.h"
#include"ItemHandler.h"
#include"MathFunc.h"
#include"EnemyStateMachine.h"
#include<random>

MainMenuState::MainMenuState(GameDataRef data) : _data(data), _doorCloseToPlayer(nullptr)
{

}

std::vector<Wall*> MainMenuState::_walls;
std::vector<Item*> MainMenuState::_items;
std::vector<Enemy*> MainMenuState::_enemies;
std::vector<Door*> MainMenuState::_doors;
std::vector<Window*> MainMenuState::_windows;
Player* MainMenuState::player;
sf::Vector2f MainMenuState::_lastSeenPositionOfPlayer;

int MainMenuState::GlobalIDCounter = 0;
std::map<std::string, ItemTrait> EnumStringMap;

void MapItemEnumWithStrings()
{
#define ItemTraitEnum(x) EnumStringMap.insert({#x,x});
#include "ItemTraitsDefinitions.h"
#undef ItemTraitEnum 
}

void MainMenuState::Init()
{
	MapItemEnumWithStrings();
	LoadLevel();
	DrawLines();
	msgDisptch.Init(handlerManager);
	handlerManager.GetWalls(_walls);
	PathFinder::PathFinder(_data);
	PathFinder::Init(_walls, _doors, _windows);
	PathFinder::ConnectLinesToNeighbours();
	buffer.loadFromFile("C:/Users/User/Desktop/Physics/door-knock-1.wav");

	knockSound.setBuffer(buffer);

	player = new Player(_data);
	player->SetPosition(sf::Vector2f(500, 720));
	player->SetID(++GlobalIDCounter);
	player->p2 = sf::Vector2f(490, 350);
	handlerManager.Register(player);

	for (int i = 0; i < _enemies.size(); i++)
	{
		_enemies[i]->SetPlayer(player);
		_enemies[i]->SetState(EnemyState::PATROL);
		_enemies[i]->_line.p2 = RotateLine(0.2, _enemies[i]->_line.p1, _enemies[i]->_line.p2);
		sf::Vector2f direction = _enemies[i]->_line.p2 - _enemies[i]->_line.p1;
		sf::Vector2f direction2 = _enemies[i]->GetCurrentWayPt() - _enemies[i]->_line.p1;
		double angle = AngleBWVectors(direction, direction2);
		//_enemies[i]->Rotate(-angle * (180 / 3.14));
		_enemies[i]->_line.p2 = RotateLine(-angle + 0.2, _enemies[i]->_line.p1, _enemies[i]->_line.p2);
		sf::Vector2f unitVectorDir = UnitVector(_enemies[i]->_line.p2 - _enemies[i]->_line.p1);
		_enemies[i]->SetDirection(unitVectorDir);
	}
}

void MainMenuState::HandleInput()
{
	sf::Event event;

	while (_data->window.pollEvent(event))
	{
		if (event.type == sf::Event::EventType::Closed)
		{
			_data->window.close();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::A && m_side_collided != COLLISIONSIDE::LEFT) {
				player->SetDirection(sf::Vector2f(-1, 0));
				MovePlayer();
			}
			else if (event.key.code == sf::Keyboard::D && m_side_collided != COLLISIONSIDE::RIGHT)
			{
				player->SetDirection(sf::Vector2f(1, 0));
				MovePlayer();
			}
			else if (event.key.code == sf::Keyboard::W && m_side_collided != COLLISIONSIDE::UPWARDS)
			{
				player->SetDirection(sf::Vector2f(0, -1));
				MovePlayer();;
			}
			else if (event.key.code == sf::Keyboard::S && m_side_collided != COLLISIONSIDE::DOWNWARDS)
			{
				player->SetDirection(sf::Vector2f(0, 1));
				MovePlayer();
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && player->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
			{
				PlayerAttack();
			}

			else if (event.key.code == sf::Keyboard::X && _doorCloseToPlayer != nullptr)
			{
				OpenDoorbyEnemy(0, player);
			}
			else if (event.key.code == sf::Keyboard::C && _doorCloseToPlayer != nullptr)
			{
				CheckEnemyCloseToDoor();
				knockSound.play();
			}

			else if (event.key.code == sf::Keyboard::V && player->nearByItem != nullptr)
			{
				PickItem();
			}

			else if (event.key.code == sf::Keyboard::V && player->_items.size()>0)
			{
				ThrowItem();
			}

			else if (event.key.code == sf::Keyboard::O && player->_items.size() > 0)
			{
				DecreaseTargetItemLine();
			}

			else if (event.key.code == sf::Keyboard::I && player->_items.size() > 0)
			{
				IncreaseTargetItemLine();
			}
			else if (event.key.code == sf::Keyboard::G)
			{
				Message msg(-1, -1, MessageType::GOTOTOILET, 5.f, 0.f);
				msgDisptch.AddDispatchMessage(msg, _data->_Globalclock.getElapsedTime().asSeconds());
			}
		}

		if (_data->input.IskeyPressed())
		{
			_data->machine.AddState(StateRef(new GameState(_data)), true);
		}
	}
}


void MainMenuState::Update(float dt)
{
	if (!player->isdead)
	{
		player->Update(dt);
		ReduceNextHitTime(dt);
		RunEnemiesState();
		//MoveEnemies(dt);
		CheckCollisionWithWalls();
		GetDispatchedMessages();
		CheckDistWithDoors();
		RemoveDeadEnemies();
		RemoveItems();
		CheckWhatIsNearToItems();
		MoveItems();
		UpdatePathofUpdatedMap(dt);
	}
	//updateGridInsideRectangle();
}

void MainMenuState::Draw(float dt)
{
	_data->window.clear();

	PathFinder::Draw();
	for (int i = 0; i < _items.size(); i++)
	{
		_items[i]->Draw();
	}
	player->Draw();
	for (int i = 0; i < _walls.size(); i++)
	{
		_walls[i]->Draw();
	}

	for (int i = 0; i < _windows.size(); i++)
	{
		_windows[i]->Draw();
	}

	//for (int i = 0; i < _RectWalls.size(); i++)
	//{
	//	_RectWalls[i]->Draw();
	//}

	for (int i = 0; i < _doors.size(); i++)
	{
		_doors[i]->Draw();
	}

	for (int i = 0; i < _enemies.size(); i++)
	{
		_enemies[i]->Draw();
	}

	for (int i = 0; i < _Deadenemies.size(); i++)
	{
		_Deadenemies[i]->Draw();
	}

	_data->window.display();
}

void MainMenuState::MovePlayer()
{
	sf::Vector2f nxtPos = player->GetPosition() + (player->GetDirection()*4.f);
	player->SetPosition(nxtPos);
}

void MainMenuState::ReduceNextHitTime(float dt)
{
	if (player->nextHitTime > 0)
	{
		player->nextHitTime -= dt;
	}

	for (int i = 0; i < _enemies.size(); i++)
	{
		if (_enemies[i]->nextHitTime > 0)
		{
			_enemies[i]->nextHitTime -= dt;
		}
	}
}

void MainMenuState::CheckCollisionWithWalls()
{
	for (int i = 0; i < _walls.size(); i++)
	{
		/*1.when circle's center is between two points of the line -> CheckDistance() returns correct distance, it finds a closest point which is in between of the two points
		of the line and finds the distance between the closest point and circle's centre. If the circle collides with the wall then it finds the penetration and pushes the
		  circle by the value of the penetration
		  2.when circle's center is not in the range of the line but some part of the circle falls between the line - CheckDistance() doesn't return the closest point within
		  two points of the line and later there is no collision resolve since the it assumes there is no collision happening with the wall
		  3. if the circle's centre is exactly on the same axis of the common axis of the wall. For eg. a wall with position 400.0 80.0 400.0 200.0 has a common x axis. If the
		  circle too has a common X axis value then it can walk through the wall with no collision detection
		*/
		float dist = CheckDistance(player->GetShape(), _walls[i]->p1, _walls[i]->p2);

		if (dist < player->GetShape().getRadius())
		{
			sf::Vector2f clostestPointToWall = GetClosesPointToDoor(player->GetShape(), _walls[i]->p1, _walls[i]->p2);

			float penetration = player->GetShape().getRadius() - Distance(player->GetPosition(), clostestPointToWall);
			sf::Vector2f ptToCenter = player->GetPosition() - clostestPointToWall;
			ptToCenter = UnitVector(ptToCenter);
			sf::Vector2f newpos = ptToCenter * penetration;

			player->SetPosition(sf::Vector2f(player->GetPosition() + newpos));
		}

		if (player->_items.size() > 0 && player->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
		{
			if (areLinesColliding(player->_items[0]->_targetLine.p1, player->_items[0]->_targetLine.p2, _walls[i]->p1, _walls[i]->p2))
			{
				sf::Vector2f ptOfint = PointOfContactOfLines(player->_items[0]->_targetLine.p1, player->_items[0]->_targetLine.p2, _walls[i]->p1, _walls[i]->p2);
			    player->_items[0]->_targetLine.p3 = ptOfint;
				player->_items[0]->_targetLine.p2 = ptOfint;
			}
		}

		for (int j = 0; j < _enemies.size(); j++)
		{
			float distToEnemy = CheckDistance(_enemies[j]->GetShape(), _walls[i]->p1, _walls[i]->p2);

			if (distToEnemy < _enemies[j]->GetShape().getRadius())
			{
				sf::Vector2f clostestPointToWall = GetClosesPointToDoor(_enemies[j]->GetShape(), _walls[i]->p1, _walls[i]->p2);
				float penetration = _enemies[j]->GetShape().getRadius() - Distance(_enemies[j]->GetPosition(), clostestPointToWall);
				sf::Vector2f ptToCenter = _enemies[j]->GetPosition() - clostestPointToWall;
				ptToCenter = UnitVector(ptToCenter);
				sf::Vector2f newpos = ptToCenter * penetration;
				_enemies[j]->SetPosition(sf::Vector2f(_enemies[j]->GetPosition() + newpos));
			}

			if (_enemies[j]->_items.size() > 0 && _enemies[j]->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
			{
				if (areLinesColliding(_enemies[j]->_items[0]->_targetLine.p1, _enemies[j]->_items[0]->_targetLine.p2, _walls[i]->p1, _walls[i]->p2))
				{
					sf::Vector2f ptOfint = PointOfContactOfLines(_enemies[j]->_items[0]->_targetLine.p1, _enemies[j]->_items[0]->_targetLine.p2, _walls[i]->p1, _walls[i]->p2);
					_enemies[j]->_items[0]->_targetLine.p3 = ptOfint;
					_enemies[j]->_items[0]->_targetLine.p2 = ptOfint;
				}
			}
		}


		for (int j = 0; j < _items.size(); j++)
		{
			if (areLinesColliding(_items[j]->_Itemline.p1, _items[j]->_Itemline.p2, _walls[i]->p1, _walls[i]->p2))
			{
				_items[j]->SetDirection(sf::Vector2f(NULL, NULL));
			}
		}

	}

	for (int i = 0; i < _doors.size(); i++)
	{
		float dist = Distance(player->GetPosition(), GetClosesPointToDoor(player->GetShape(), _doors[i]->p1, _doors[i]->p2));

		if (dist < player->GetShape().getRadius())
		{
			sf::Vector2f clostestPointToWall = GetClosesPointToDoor(player->GetShape(), _doors[i]->p1, _doors[i]->p2);

			float penetration = player->GetShape().getRadius() - Distance(player->GetPosition(), clostestPointToWall);
			sf::Vector2f ptToCenter = player->GetPosition() - clostestPointToWall;
			ptToCenter = UnitVector(ptToCenter);
			sf::Vector2f newpos = ptToCenter * penetration;

			player->SetPosition(sf::Vector2f(player->GetPosition() + newpos));
		}

		for (int j = 0; j < _items.size(); j++)
		{
			if (areLinesColliding(_items[j]->_Itemline.p1, _items[j]->_Itemline.p2, _doors[i]->p1, _doors[i]->p2))
			{
				_items[j]->SetDirection(sf::Vector2f(NULL, NULL));
			}
		}

		for (int j = 0; j < _enemies.size(); j++)
		{
			float distToEnemy = Distance(_enemies[j]->GetPosition(), GetClosesPointToDoor(_enemies[j]->GetShape(), _doors[i]->p1, _doors[i]->p2));

			if (distToEnemy < _enemies[j]->GetShape().getRadius())
			{
				sf::Vector2f clostestPointToWall = GetClosesPointToDoor(_enemies[j]->GetShape(), _doors[i]->p1, _doors[i]->p2);
				float penetration = _enemies[j]->GetShape().getRadius() - Distance(_enemies[j]->GetPosition(), clostestPointToWall);
				sf::Vector2f ptToCenter = _enemies[j]->GetPosition() - clostestPointToWall;
				ptToCenter = UnitVector(ptToCenter);
				sf::Vector2f newpos = ptToCenter * penetration;
				_enemies[j]->SetPosition(sf::Vector2f(_enemies[j]->GetPosition() + newpos));
			}
		}
	}


	for (int i = 0; i < _enemies.size(); i++)
	{
		for (int j = 0; j < _items.size(); j++)
		{
			float dist = CheckDistance(_enemies[i]->GetShape(), _items[j]->_Itemline.p1, _items[j]->_Itemline.p2);

			if (dist < _enemies[i]->GetShape().getRadius() && _items[j]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE) && _items[j]->GetDirection() != sf::Vector2f(NULL, NULL) && !_items[j]->isThrownByEnemy)
			{
				_items[j]->SetDirection(sf::Vector2f(NULL, NULL));
				_enemies[i]->KillEnemy();
			}

			else if (dist < _enemies[i]->GetShape().getRadius() && !_items[j]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE))
			{
				_items[j]->SetDirection(sf::Vector2f(NULL, NULL));
			}
		}
	}

	for (int j = 0; j < _items.size(); j++)
	{
		float dist = CheckDistance(player->GetShape(), _items[j]->_Itemline.p1, _items[j]->_Itemline.p2);


		if (dist < player->GetShape().getRadius() && _items[j]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE) && _items[j]->GetDirection() != sf::Vector2f(NULL, NULL) && _items[j]->isThrownByEnemy)
		{
			player->Kill();
		}
	}


	for (int i = 0; i < _windows.size(); i++)
	{
		/*	for (int j = 0; j < _enemies.size(); j++)
			{
				std::vector<sf::Vector2f> _rectPoints;
				int ptCount = _enemies[j]->GetShape().getPointCount();
				for (int k = 0; k < ptCount; k++)
				{
					_rectPoints.push_back(_enemies[j]->GetPoint(k));
				}
				sf::Vector2f ClosestPtToenemy = GetClosesPointToDoor(_enemies[j]->GetPosition(), _windows[i]->p1, _windows[i]->p2);
				if ((ClosestPtToenemy.x > _windows[i]->p2.x || ClosestPtToenemy.y > _windows[i]->p2.y || ClosestPtToenemy.x < _windows[i]->p1.x || ClosestPtToenemy.y < _windows[i]->p1.y)) continue;


				for (int k = 0; k < _rectPoints.size(); k++)
				{
					if (areLinesColliding(_rectPoints[k], _rectPoints[k == 3 ? 0 : k + 1], _windows[i]->p1, _windows[i]->p2))
					{
						if (ClosestPtToenemy.x != _enemies[j]->GetPosition().x)
						{
							if (ClosestPtToenemy.x < _enemies[j]->GetPosition().x)
							{
								_enemies[j]->SetPosition(sf::Vector2f(_enemies[j]->GetPosition().x + 5.0, _enemies[j]->GetPosition().y));
							}

							else
							{
								_enemies[j]->SetPosition(sf::Vector2f(_enemies[j]->GetPosition().x - 5.0, _enemies[j]->GetPosition().y));
							}
						}

						else if (ClosestPtToenemy.y != _enemies[j]->GetPosition().y)
						{
							if (ClosestPtToenemy.y < _enemies[j]->GetPosition().y)
							{
								_enemies[j]->SetPosition(sf::Vector2f(_enemies[j]->GetPosition().x, _enemies[j]->GetPosition().y + 5));

							}

							else
							{
								_enemies[j]->SetPosition(sf::Vector2f(_enemies[j]->GetPosition().x, _enemies[j]->GetPosition().y - 5));

							}
						}
					}
				}

			}*/
	}
}

void MainMenuState::UpdatePathofUpdatedMap(float dt)
{
	if (hasMapUpdated)
	{
		for (int i = 0; i < _enemies.size(); i++)
		{
			if (_enemies[i]->hasBeenAssignedPath)
			{
				_enemies[i]->SetShortestPath(PathFinder::CheckPath(_enemies[i]->GetPosition(), _enemies[i]->_shortestPath[0]));
			}
		}
		hasMapUpdated = false;
	}
}

void MainMenuState::GetDispatchedMessages()
{
	Message msg(0, 0, MessageType::GOTOTOILET, 6.f, 0.f);
	msgDisptch.GetPotentialDispatchMsg(_data->_Globalclock.getElapsedTime().asSeconds(), msg);
	if (msg.receiver != 0)handlerManager.HandleMessage(msg);
}

void MainMenuState::CheckWhatIsNearToItems()
{
	for (int i = 0; i < _items.size(); i++)
	{
		double dist = Distance(player->getPostion(), _items[i]->GetPosition());
		if (dist < 22.0)
		{
			player->nearByItem = _items[i];
			break;
		}

		else
		{
			player->nearByItem = nullptr;
		}
	}
	for (int i = 0; i < _items.size(); i++)
	{
		for (int j = 0; j < _enemies.size(); j++)
		{
			if (!_enemies[j]->runAwayFromGrenade)
			{
				double distToEnem = Distance(_enemies[j]->GetPosition(), _items[i]->GetPosition());
				if (distToEnem < 40.0 && !CheckEnemyToItems(i, j))
				{
					_enemies[j]->nearByItem = _items[i];
				}

				if (!CheckEnemyToItems(i, j) && _items[i]->CheckTrait(ItemTrait::EXPLODESONHIT) && distToEnem < 200.f && _items[i]->isThrownByEnemy && !_items[i]->needToMove)
				{
					_enemies[j]->runAwayFromGrenade = true;
					sf::Vector2f pos = PathFinder::FindUnaffectedPointByGrenade(_items[i]->GetPosition(),_enemies[j]->GetPosition());
					_enemies[j]->SetState(EnemyState::RUNAWAY);
					_enemies[j]->SetShortestPath(PathFinder::CheckPath(_enemies[j]->GetPosition(), pos));
				}
			}
		}
	}
}

void MainMenuState::PickItem(int index)
{
	if (index == -1)
	{
		if (player->nearByItem != nullptr)
		{
			if (player->nearByItem->CheckTrait(ItemTrait::HEALTHBOOST))
			{
				player->IncreaseHealthByKit();
				std::vector<Item*>::iterator it;
				int indexNo = 0;
				for (it = _items.begin(); it < _items.end(); it++)
				{
					if (*it == player->nearByItem)
					{
						break;
					}
					indexNo++;
				}
				_items.erase(_items.begin() + indexNo);
				player->nearByItem = nullptr;
			}
			else
			{
				player->AddItem();

				std::vector<Item*>::iterator it;
				int indexNo = 0;
				for (it = _items.begin(); it < _items.end(); it++)
				{
					if (*it == player->nearByItem)
					{
						break;
					}
					indexNo++;
				}
				_items.erase(_items.begin() + indexNo);
				player->nearByItem = nullptr;
			}
		}
	}

	else
	{
		if (_enemies[index]->nearByItem != nullptr)
		{
			_enemies[index]->AddItem();
			std::vector<Item*>::iterator it;
			int indexNo = 0;
			for (it = _items.begin(); it < _items.end(); it++)
			{
				if (*it == _enemies[index]->nearByItem)
				{
					break;
				}
				indexNo++;
			}
			_items.erase(_items.begin() + indexNo);
			_enemies[index]->nearByItem = nullptr;
		}
	}
}

void MainMenuState::ThrowItem(int index)
{
	if (index == -1)
	{
		player->_items[0]->SetPosition(player->getPostion());
		sf::Vector2f ItemDir = UnitVector(player->p2 - player->p1);
		player->_items[0]->SetDirection(ItemDir);
		player->_items[0]->IsPicked = false;
		player->_items[0]->isThrownByEnemy = true;
		player->_items[0]->needToMove = true;
		_items.push_back(player->_items[0]);
		player->_items.erase(player->_items.begin());
		player->targetLength = 0.f;
	}

	else
	{
		_enemies[index]->_items[0]->SetPosition(_enemies[index]->GetPosition());
		sf::Vector2f ItemDir = UnitVector(_enemies[index]->_line.p2 - _enemies[index]->_line.p1);
		_enemies[index]->_items[0]->SetDirection(ItemDir);
		_enemies[index]->_items[0]->isThrownByEnemy = true;
		if (_enemies[index]->_items[0]->CheckTrait(ItemTrait::EXPLODESONHIT))
		{
			_enemies[index]->_items[0]->targetPos = _lastSeenPositionOfPlayer;
		}
		_items.push_back(_enemies[index]->_items[0]);
		_enemies[index]->_items.erase(_enemies[index]->_items.begin());
		_enemies[index]->_items.shrink_to_fit();
	}
}

void MainMenuState::IncreaseTargetItemLine()
{
	if (player->_items.size() > 0 && player->_items[0]->CheckTrait(ItemTrait::EXPLODESONHIT) && player->targetLength < 300.f)
	{
		player->targetLength += 20.f;
	}
}

void MainMenuState::DecreaseTargetItemLine()
{
	if (player->_items.size() > 0 && player->_items[0]->CheckTrait(ItemTrait::EXPLODESONHIT) && player->targetLength > 30.f)
	{
		player->targetLength -= 20.f;
	}
}

void MainMenuState::CheckEnemyCloseToDoor()
{
	for (int i = 0; i < _enemies.size(); i++)
	{
		if (Distance(_doorCloseToPlayer->p2, _enemies[i]->GetPosition()) < 300.0f)
		{
			_enemies[i]->SetSuspicionLevel(SuspicionLevel::DOUBTFUL);
			_enemies[i]->SetShortestPath(PathFinder::CheckPath(_enemies[i]->GetPosition(), (sf::Vector2f(_doorCloseToPlayer->p1.x, _doorCloseToPlayer->p1.y + 40.f))));
			//_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _items[0]->GetPosition()));
			_enemies[i]->suspiciousPos = sf::Vector2f(_enemies[i]->_shortestPath[_enemies[i]->_shortestPath.size() - 1]);
		}

		else
		{
			_enemies[i]->suspiciousPos = sf::Vector2f(NULL, NULL);
		}
	}
}

void MainMenuState::CheckDistWithDoors(int index)
{
	for (int i = 0; i < _doors.size(); i++)
	{
		if (index != -1)
		{
			if (Distance(GetClosesPointToDoor(_enemies[index]->GetPosition(), _doors[i]->p1, _doors[i]->p2), _enemies[index]->GetPosition()) < 40.0)
			{
				_enemies[index]->closestPointToDoorFromEnemy = GetClosesPointToDoor(_enemies[index]->GetPosition(), _doors[i]->p1, _doors[i]->p2);
				_enemies[index]->closestDoor = _doors[i];
				CanDoorOpen = true;
				break;
			}

			else
			{
				_enemies[index]->closestPointToDoorFromEnemy = sf::Vector2f(NULL, NULL);
				CanDoorOpen = false;
				_enemies[index]->closestDoor = nullptr;
			}
		}

		else
		{
			if (Distance(GetClosesPointToDoor(player->GetPosition(), _doors[i]->p1, _doors[i]->p2), player->GetPosition()) < 40.f)
			{
				player->closestPointToDoor = GetClosesPointToDoor(player->GetShape(), _doors[i]->p1, _doors[i]->p2);
				_doorCloseToPlayer = _doors[i];
				CanDoorOpen = true;
				break;
			}
			else
			{
				player->closestPointToDoor = sf::Vector2f(NULL, NULL);
				CanDoorOpen = false;
				_doorCloseToPlayer = nullptr;
			}
		}
	}
}

void MainMenuState::RemoveItems()
{
	std::vector<Item*> nonexistingItems;

	for (int i = 0; i < _items.size(); i++)
	{
		if (_items[i]->doesntExist)
		{
			nonexistingItems.push_back(_items[i]);
		}
	}

	for (int i = 0; i < nonexistingItems.size(); i++)
	{
		ReadjustItemsList(nonexistingItems[i]);
	}
}

void MainMenuState::ReadjustItemsList(Item *item)
{
	std::vector<Item*>::iterator it;
	int index = 0;
	for (it = _items.begin(); it < _items.end(); it++)
	{
		if (*it == item)
		{
			break;
		}
		index++;
	}
	_items.erase(_items.begin() + index);
}

void MainMenuState::RemoveDeadEnemies()
{
	for (int i = 0; i < _enemies.size(); i++)
	{
		if (_enemies[i]->IsEnemyDead())
		{
			_Deadenemies.push_back(_enemies[i]);
		}
	}

	for (int i = 0; i < _Deadenemies.size(); i++)
	{
		if (!_Deadenemies[i]->isRemovedfromMainList)
		{
			_Deadenemies[i]->isRemovedfromMainList = true;
			ReadjustEnemyVector(_Deadenemies[i]);
		}
	}
}

void MainMenuState::ReadjustEnemyVector(Enemy* enemy)
{
	std::vector<Enemy*>::iterator it;
	int index = 0;
	for (it = _enemies.begin(); it < _enemies.end(); it++)
	{
		if (*it == enemy)
		{
			break;
		}
		index++;
	}
	_enemies.erase(_enemies.begin() + index);
}


bool MainMenuState::CheckEnemiesToPlayerLine(int index)
{
	bool enemy2player = checkEnemyToPlayer(index);
	if (!enemy2player)
	{
		sf::Vector2f vectorToPlayer = player->GetPosition() - _enemies[index]->GetPosition();
		sf::Vector2f enemyForward = _enemies[index]->_line.p2 - _enemies[index]->_line.p1;
		float ang = AngleBWVectors(vectorToPlayer, enemyForward);
		ang = ang * (180 / 3.14);
		float absAngle = std::abs(ang);
		if (absAngle < 100.0f)
		{
			return true;
		}

		else return false;
	}

	return false;
}

bool MainMenuState::checkEnemyToPlayer(int i)
{
	for (int wI = 0; wI < _walls.size(); wI++)
	{
		if (areLinesColliding(_enemies[i]->GetPosition(), player->getPostion(), _walls[wI]->p1, _walls[wI]->p2))
		{
			return true;
		}
	}

	for (int wI = 0; wI < _doors.size(); wI++)
	{
		if (areLinesColliding(_enemies[i]->GetPosition(), player->getPostion(), _doors[wI]->p1, _doors[wI]->p2))
		{
			return true;
		}
	}

	//for (int wI = 0; wI < _windows.size(); wI++)
	//{
	//	if (areLinesColliding(_enemies[i]->GetPosition(), player->getPostion(), _windows[wI]->p1, _windows[wI]->p2))
	//	{
	//		return false;
	//	}
	//}

	return false;
}

void MainMenuState::PlayerAttack(int i)
{
	if (i == -1)
	{
		if (player->nextHitTime <= 0)
		{
			player->nextHitTime = 1.5f;
			for (int i = 0; i < _enemies.size(); i++)
			{
				float distToEnemy = CheckDistance(_enemies[i]->GetShape(), player->_items[0]->_targetLine.p1, player->_items[0]->_targetLine.p2);
				if (distToEnemy < _enemies[i]->GetShape().getRadius())
				{
					_enemies[i]->Reducehealth();
				}
			}
		}
	}

	else
	{
		if (_enemies[i]->nextHitTime <= 0)
		{
			_enemies[i]->nextHitTime = 1.5f;
			
				float distToEnemy = CheckDistance(player->GetShape(), _enemies[i]->_items[0]->_targetLine.p1, _enemies[i]->_items[0]->_targetLine.p2);
				if (distToEnemy < player->GetShape().getRadius())
				{
					player->ReduceHealthOfPlayer();
				}
			
		}
	}
}

bool MainMenuState::CheckEnemyToPointIntersection(sf::Vector2f pointPos, sf::Vector2f enemyPos)
{
	for (auto wall : _walls)
	{
		if (areLinesColliding(pointPos, enemyPos, wall->p1, wall->p2))
		{
			return true;
		}
	}

	return false;
}

void MainMenuState::RunEnemiesState()
{
	int i = 0;
	for (auto pEnemy : _enemies)
	{
		pEnemy->enemymachine->ProcessState(i);
		i++;
	}
}

//void MainMenuState::MoveEnemies(float dt)
//{
//	for (int i = 0; i < _enemies.size(); i++)
//	{	
//		switch (_enemies[i]->GetState())
//		{
//		case EnemyState::PATROL:
//		{
//			if (_enemies[i]->GetSuspicionLevel() == SuspicionLevel::RELAXED)
//			{
//				if (Distance(_enemies[i]->GetCurrentWayPt(), _enemies[i]->GetPosition()) > 0.5f)
//				{
//					SetEnemyDirection(i, _enemies[i]->GetCurrentWayPt());
//					MoveEnemy(i);
//					CheckWhereIsEnemy(i);
//					continue;
//				}
//
//				else
//				{
//					_enemies[i]->SetNextWayPoint();
//					_enemies[i]->_line.p2 = RotateLine(0.2, _enemies[i]->_line.p1, _enemies[i]->_line.p2);
//					sf::Vector2f direction = _enemies[i]->_line.p2 - _enemies[i]->_line.p1;
//					sf::Vector2f direction2 = _enemies[i]->GetCurrentWayPt() - _enemies[i]->_line.p1;
//					double angle = AngleBWVectors(direction, direction2);
//					_enemies[i]->_line.p2 = RotateLine(-angle + 0.2, _enemies[i]->_line.p1, _enemies[i]->_line.p2);
//					sf::Vector2f unitVectorDir = UnitVector(_enemies[i]->_line.p2 - _enemies[i]->_line.p1);
//					_enemies[i]->SetDirection(unitVectorDir);
//					break;
//				}
//			}
//
//			else if (_enemies[i]->GetSuspicionLevel() == SuspicionLevel::DOUBTFUL)
//			{
//				bool isItemnear = false;
//				if (!_enemies[i]->_hasReachedSuspPos)
//				{
//					if (Distance(_enemies[i]->GetCurrentPointInShrstPath(), _enemies[i]->GetPosition()) > 0.5f)
//					{
//						SetEnemyDirection(i, _enemies[i]->GetCurrentPointInShrstPath());
//						MoveEnemy(i);
//					}
//
//					else
//					{
//						_enemies[i]->SetNextPointInShrtstPath();
//					}
//
//					CheckWhereIsEnemy(i);
//				}
//				else
//				{
//					if (Distance(_enemies[i]->GetPosition(), _enemies[i]->_dynamicWaypt) > 0.5f)
//					{
//						SetEnemyDirection(i, _enemies[i]->_dynamicWaypt);
//						MoveEnemy(i);
//						CheckWhereIsEnemy(i);
//					}
//					else
//					{
//						_enemies[i]->_rotateRad += dt;
//						_enemies[i]->_line.p2 = RotateLine(-_enemies[i]->_rotateRad, _enemies[i]->_line.p1, _enemies[i]->_line.p2);
//						if (_enemies[i]->nearByItem != nullptr && _enemies[i]->nearByItem->CheckTrait(ItemTrait::CONSUMABLE))
//						{
//							std::vector<Item*>::iterator it;
//							int index = 0;
//							for (it = _items.begin(); it < _items.end(); it++)
//							{
//								if (*it == _enemies[i]->nearByItem)
//								{
//									break;
//								}
//								index++;
//							}
//							Message sandWichMsg(_enemies[i]->GetID(), _enemies[i]->nearByItem->GetID(), MessageType::GOTOTOILET, 6.f, 0.f);
//							msgDisptch.AddDispatchMessage(sandWichMsg, _data->_Globalclock.getElapsedTime().asSeconds());
//							_items.erase(_items.begin() + index);
//							_enemies[i]->nearByItem = nullptr;
//						}
//
//						CheckWhereIsEnemy(i);
//					}
//					_enemies[i]->_coolDownTime -= 0.1;
//					if (_enemies[i]->_coolDownTime < 0.f)
//					{
//						_enemies[i]->SetSuspicionLevel(SuspicionLevel::RELAXED);
//						_enemies[i]->_dynamicWaypt = sf::Vector2f(NULL, NULL);
//						_enemies[i]->_coolDownTime = 30.f;
//						_enemies[i]->_hasReachedSuspPos = false;
//						break;
//					}
//
//					continue;
//				}
//
//				if (_enemies[i]->closestDoor == nullptr) CheckDistWithDoors(i);
//
//				else if (_enemies[i]->closestDoor != nullptr && _enemies[i]->closestDoor->isOpen == false)
//				{
//					OpenDoorbyEnemy(i);
//					if (_enemies[i]->closestDoor->p1.x > _enemies[i]->closestDoor->p2.x)
//					{
//						MakeDirtyRect(_enemies[i]->closestDoor->p2.x - 100, _enemies[i]->closestDoor->p1.x + 100, _enemies[i]->closestDoor->p2.y - 200, _enemies[i]->closestDoor->p2.y + 200);
//					}
//					else if (_enemies[i]->closestDoor->p1.x < _enemies[i]->closestDoor->p2.x)
//					{
//						MakeDirtyRect(_enemies[i]->closestDoor->p1.x, _enemies[i]->closestDoor->p2.x, _enemies[i]->closestDoor->p2.y - 200, _enemies[i]->closestDoor->p2.y + 200);
//					}
//					_enemies[i]->closestDoor->isOpen = true;
//					updateGridInsideRectangle();
//
//					FindDynamicWayPtForEnemy(i, &_enemies[i]->_dynamicWaypt);
//					_enemies[i]->_hasReachedSuspPos = true;
//					_enemies[i]->SetDirection(sf::Vector2f(NULL, NULL));
//					_enemies[i]->closestDoor = nullptr;
//
//					break;
//				}
//				else if (_enemies[i]->closestDoor != nullptr && _enemies[i]->closestDoor->isOpen == true)
//				{
//					FindDynamicWayPtForEnemy(i, &_enemies[i]->_dynamicWaypt);
//					_enemies[i]->_hasReachedSuspPos = true;
//					_enemies[i]->SetDirection(sf::Vector2f(NULL, NULL));
//					_enemies[i]->closestDoor = nullptr;
//					continue;
//				}
//
//				continue;
//			}
//
//			break;
//		}
//
//		case EnemyState::STUNNED:
//		{
//			if (_enemies[i]->GetStunTimer() > 0)
//			{
//				_enemies[i]->ReduceStunTimer();
//				continue;
//			}
//			else
//			{
//				_enemies[i]->SetState(EnemyState::FOLLOWPATHTOTOILET);
//				break;
//			}
//		}
//
//		case EnemyState::CALLFORHELP:
//		{
//		    Message helpmsg(-1, _enemies[i]->GetID(), MessageType::GOFORHELP, 0.0f, 800.f, _enemies[i]->GetPosition());
//			msgDisptch.AddDispatchMessage(helpmsg, _data->_Globalclock.getElapsedTime().asSeconds());
//			break;
//		}
//
//		case EnemyState::GONNADIESOON:
//		{
//			if (!_enemies[i]->sentMessageForHelp)
//			{
//				Message helpMessage(-1, _enemies[i]->GetID(), MessageType::SAVEFRIENDSASS, 0.f, 500.f,_enemies[i]->GetPosition());
//				msgDisptch.AddDispatchMessage(helpMessage, _data->_Globalclock.getElapsedTime().asSeconds());
//				_enemies[i]->SetEnemyColor(sf::Color::Blue);
//				_enemies[i]->sentMessageForHelp = true;
//			}
//
//			if (_enemies[i]->timeToDie > 0.f)
//			{
//				_enemies[i]->SetPosition(_enemies[i]->GetPosition());
//				_enemies[i]->timeToDie -= 0.03f;
//				break;
//			}
//
//			else
//			{
//				_enemies[i]->KillEnemy();
//				break;
//			}
//		}
//
//		case EnemyState::SHOOT:
//		{
//
//			if (_enemies[i]->GetSuspicionLevel() == SuspicionLevel::TAKECOVERANDSHOOT)
//			{
//				if (_enemies[i]->shootTime > 0.f)
//				{
//					_enemies[i]->shootTime -= dt;
//					if (Distance(_enemies[i]->closestPosToShootNearCover, _enemies[i]->GetPosition()) > 0.5f)
//					{
//						SetEnemyDirection(i, _enemies[i]->closestPosToShootNearCover);
//						MoveEnemy(i);
//					}
//
//					else
//					{
//						SetEnemyDirection(i, _lastSeenPositionOfPlayer);
//						if (_enemies[i]->_items.size() > 0 && _enemies[i]->_items[0]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE))
//						{
//							if (CheckEnemiesToPlayerLine(i))
//							{
//								_lastSeenPositionOfPlayer = player->GetPosition();
//
//								if (Distance(_enemies[i]->GetPosition(), player->GetPosition()) < 250.f)
//								{
//									if (_enemies[i]->_items.size() > 0 && _enemies[i]->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
//									{
//										PlayerAttack(i);
//									}
//
//									else
//									{
//										ThrowItem(i);
//										_enemies[i]->SetState(EnemyState::TAKECOVER);
//										_enemies[i]->coverTime = 30.f;
//									}
//								}
//
//								else
//								{
//									if (!CheckEnemyToPointIntersection(_lastSeenPositionOfPlayer, _enemies[i]->GetPosition()))
//									{
//										_enemies[i]->SetState(EnemyState::GETINRANGEOFPLAYER);
//										break;
//									}
//
//									else
//									{
//										_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _lastSeenPositionOfPlayer));
//										_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//										break;
//									}
//								}
//
//							}
//
//
//							else
//							{
//								if (!CheckEnemyToPointIntersection(_lastSeenPositionOfPlayer, _enemies[i]->GetPosition()))
//								{
//									_enemies[i]->travelToPosition = _lastSeenPositionOfPlayer;
//									_enemies[i]->SetState(EnemyState::TRAVELTOPOSITION);
//									break;
//								}
//
//								else
//								{
//									_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _lastSeenPositionOfPlayer));
//									_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//									break;
//								}
//							}
//						}
//
//						else
//						{
//							DecideToTakeCover(i);
//							break;
//						}
//					}
//					
//				}
//
//				else
//				{
//					_enemies[i]->SetState(EnemyState::TAKECOVER);
//					std::random_device rd;
//					std::mt19937 gen(rd());
//					std::uniform_int_distribution<> distr(5.f, 7.f);
//					_enemies[i]->coverTime = distr(gen);
//					break;
//				}
//
//				break;
//			}
//
//			else if (_enemies[i]->GetSuspicionLevel() == SuspicionLevel::DIRECTSHOOT)
//			{
//				if (CheckEnemiesToPlayerLine(i))
//				{
//					_lastSeenPositionOfPlayer = player->GetPosition();
//					SetEnemyDirection(i, _lastSeenPositionOfPlayer);
//					if (Distance(_enemies[i]->GetPosition(), player->GetPosition()) < 250.f)
//					{
//						if (_enemies[i]->_items.size() > 0 && _enemies[i]->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
//						{
//							PlayerAttack(i);
//						}
//					}
//
//					else
//					{
//						_enemies[i]->SetState(EnemyState::GETINRANGEOFPLAYER);
//					}
//				}
//
//				else
//				{
//					if (!CheckEnemyToPointIntersection(_lastSeenPositionOfPlayer, _enemies[i]->GetPosition()))
//					{
//						_enemies[i]->travelToPosition = _lastSeenPositionOfPlayer;
//						_enemies[i]->SetState(EnemyState::TRAVELTOPOSITION);
//						break;
//					}
//
//					else
//					{
//						_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _lastSeenPositionOfPlayer));
//						_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//						break;
//					}
//				}
//				break;
//			}
//		}
//
//		case EnemyState::TAKECOVER:
//		{
//			if (_enemies[i]->coverTime > 0.f)
//			{
//				_enemies[i]->hasBeenAssignedPath = false;
//				_enemies[i]->coverTime -= dt;
//				if (Distance(_enemies[i]->lastCoverPosition, _enemies[i]->GetPosition()) > 0.5f && !_enemies[i]->hasBeenAssignedPath)
//				{
//					SetEnemyDirection(i, _enemies[i]->lastCoverPosition);
//					MoveEnemy(i);
//				}
//
//				else 
//				{
//					SetEnemyDirection(i, _lastSeenPositionOfPlayer);
//					if (CheckEnemiesToPlayerLine(i))
//					{
//						DecideToTakeCover(i);
//					}
//				}
//				break;
//			}
//			
//			else if(_enemies[i]->_items.size() > 0 && (_enemies[i]->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT) || _enemies[i]->_items[0]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE)))
//			{
//				_enemies[i]->SetState(EnemyState::SHOOT);
//				std::random_device rd;
//				std::mt19937 gen(rd());
//				std::uniform_int_distribution<> distr(3.f, 5.f);
//				_enemies[i]->shootTime = distr(gen);
//				break;
//			}
//		}
//
//		case EnemyState::DECIDEHOWTOHELP:
//		{
//		    DecideToTakeCover(i);
//			break;
//		}
//
//		case EnemyState::GOANDSAVEFRIENDSASS:
//		{
//			if (!_enemies[i]->hasBeenAssignedPath)
//			{
//				for (auto enemy : _enemies)
//				{
//					if (enemy->GetID() == _enemies[i]->EnemyToReviveId)
//					{
//						if (CheckEnemyToPointIntersection(enemy->GetPosition(), _enemies[i]->GetPosition()))
//						{
//							_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), enemy->GetPosition()));
//							_enemies[i]->hasBeenAssignedPath = true;
//						}
//					     
//						else _enemies[i]->_shortestPath[0] = enemy->GetPosition();
//					}
//				}
//			}
//
//			if (Distance(_enemies[i]->_shortestPath[0], _enemies[i]->GetPosition()) < 1.f)
//			{
//				_enemies[i]->SetState(EnemyState::REVIVEFRIENDNOW);
//				_enemies[i]->hasBeenAssignedPath = false;
//				break;
//			}
//
//		if (Distance(_enemies[i]->GetCurrentPointInShrstPath(), _enemies[i]->GetPosition()) > 0.5f)
//			{
//				SetEnemyDirection(i, _enemies[i]->GetCurrentPointInShrstPath());
//				MoveEnemy(i);
//			}
//
//			else
//			{
//				_enemies[i]->SetNextPointInShrtstPath();
//			}
//
//			continue;
//		}
//			
//		case EnemyState::CHASE:
//		{
//			if (Distance(_enemies[i]->GetPosition(), player->getPostion()) > 100.f)
//			{
//				if (CheckEnemiesToPlayerLine(i))
//				{
//					SetEnemyDirection(i, player->getPostion());
//					MoveEnemy(i);
//					_lastSeenPositionOfPlayer = player->getPostion();
//					continue;
//				}
//				else
//				{
//					_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _lastSeenPositionOfPlayer));
//					_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//					break;
//				}
//			}
//			else
//			{
//				_enemies[i]->SetState(EnemyState::ATTACK);
//				break;
//			}
//		}
//
//		case EnemyState::REVIVEFRIENDNOW:
//		{
//			if (_enemies[i]->TimeToRevive > 0.f)
//			{
//				_enemies[i]->TimeToRevive -= 0.09f;
//			}
//
//			else
//			{
//				Message SavedFromDying(_enemies[i]->EnemyToReviveId, _enemies[i]->GetID(), MessageType::REVIVED, 0.f, 0.f);
//				msgDisptch.AddDispatchMessage(SavedFromDying, _data->_Globalclock.getElapsedTime().asSeconds());
//				_enemies[i]->EnemyToReviveId = -1000;
//				_enemies[i]->TimeToRevive = 10.f;
//				_enemies[i]->hasBeenAssignedPath = false;
//				DecideToTakeCover(i);
//				break;
//			}
//			continue;
//		}
//
//		case EnemyState::ATTACK:
//		{
//			if (Distance(_enemies[i]->GetPosition(), player->getPostion()) < 100.f)
//			{
//				if (CheckEnemiesToPlayerLine(i))
//				{
//					SetEnemyDirection(i, player->getPostion());
//					if (_enemies[i]->_items.size() > 0)ThrowItem(i);
//					MoveEnemy(i);
//					_lastSeenPositionOfPlayer = player->getPostion();
//					continue;
//				}
//				else if (Distance(_enemies[i]->GetPosition(), player->getPostion()) > 80.f && !CheckEnemiesToPlayerLine(i))
//				{
//					_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _lastSeenPositionOfPlayer));
//					_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//					break;
//				}
//
//				else
//				{
//					_enemies[i]->SetDirection(sf::Vector2f(NULL, NULL));
//				}
//			}
//
//			else
//			{
//				_enemies[i]->SetState(EnemyState::CHASE);
//				break;
//			}
//		}
//
//		case EnemyState::TRAVELTOPOSITION:
//		{
//			if (Distance(_enemies[i]->travelToPosition, _enemies[i]->GetPosition()) > 1.f)
//			{
//				SetEnemyDirection(i, _enemies[i]->travelToPosition);
//				MoveEnemy(i);
//				if (CheckEnemiesToPlayerLine(i))
//				{
//					DecideToTakeCover(i);
//				}
//				break;
//			}
//
//			else
//			{
//				if (_enemies[i]->GetSuspicionLevel() == SuspicionLevel::TAKECOVERANDSHOOT)
//				{
//					_enemies[i]->_dynamicWaypt = pathFinder.Findpointnearby(_enemies[i]->GetPosition());
//					_enemies[i]->SetState(EnemyState::LOOKFORPLAYER);
//				}
//
//				else if (_enemies[i]->GetSuspicionLevel() == SuspicionLevel::DIRECTSHOOT)
//				{
//					_enemies[i]->SetState(EnemyState::SHOOT);
//				}
//				break;
//			}
//			break;
//		}
//
//		case EnemyState::FOLLOWPATHTOTOILET:
//		{
//			if (!_enemies[i]->hasBeenAssignedPath)
//			{
//				_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), ItemsWithFlags(ItemTrait::TOILET)->GetPosition()));
//				_enemies[i]->hasBeenAssignedPath = true;
//			}
//
//			if (Distance(_enemies[i]->_shortestPath[0], _enemies[i]->GetPosition()) < 10.f)
//			{
//				_enemies[i]->SetDirection(sf::Vector2f(NULL, NULL));
//				break;
//			}
//
//			if (Distance(_enemies[i]->GetCurrentPointInShrstPath(), _enemies[i]->GetPosition()) > 1.f)
//			{
//				SetEnemyDirection(i, _enemies[i]->GetCurrentPointInShrstPath());
//				MoveEnemy(i);
//			}
//
//			else
//			{
//				_enemies[i]->SetNextPointInShrtstPath();
//			}
//
//			if (_enemies[i]->closestDoor == nullptr) CheckDistWithDoors(i);
//
//			else if (_enemies[i]->closestDoor != nullptr && _enemies[i]->closestDoor->isOpen == false)
//			{
//				OpenDoorbyEnemy(i);
//				_enemies[i]->closestDoor->isOpen = true;
//			}
//
//			continue;
//		}
//
//		case EnemyState::FINDWEAPONNEARBY:
//		{
//			if (!_enemies[i]->hasBeenAssignedPath)
//			{
//				std::pair<int, Item*> meleeItem = FindNearestWeapon(i);
//				if (meleeItem.first != -1)
//				{
//						_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), meleeItem.second->GetPosition()));
//						_enemies[i]->hasBeenAssignedPath = true;
//						break;
//				}
//
//				else
//				{
//					_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//					break;
//				}
//	
//			}
//
//			if (Distance(_enemies[i]->_shortestPath[0], _enemies[i]->GetPosition()) < 10.f)
//			{
//				{
//					PickItem(i);
//					_enemies[i]->hasBeenAssignedPath = false;
//					_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//					break;
//				}
//			}
//
//			if (Distance(_enemies[i]->GetCurrentPointInShrstPath(), _enemies[i]->GetPosition()) > 0.5f)
//			{
//				SetEnemyDirection(i, _enemies[i]->GetCurrentPointInShrstPath());
//				MoveEnemy(i);
//			}
//
//			else
//			{
//				_enemies[i]->SetNextPointInShrtstPath();
//			}
//
//			continue;
//		}
//
//		case EnemyState::MAKEPATHTOPLAYERLASTPOS:
//		{
//		if (!_enemies[i]->hasBeenAssignedPath)
//		{
//			_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _lastSeenPositionOfPlayer));
//			_enemies[i]->hasBeenAssignedPath = true;
//		}
//
//		if (_enemies[i]->hasBeenAssignedPath && hasMapUpdated)
//		{
//			_enemies[i]->SetShortestPath(pathFinder.CheckPath(_enemies[i]->GetPosition(), _lastSeenPositionOfPlayer));
//			continue;
//		}
//		
//		if (Distance(_enemies[i]->_shortestPath[0], _enemies[i]->GetPosition()) < 10.f)
//		{
//			_enemies[i]->_dynamicWaypt = pathFinder.Findpointnearby(_enemies[i]->GetPosition());
//			_enemies[i]->SetState(EnemyState::LOOKFORPLAYER);
//			_enemies[i]->hasBeenAssignedPath = false;
//			break;
//		}
//
//		if (Distance(_enemies[i]->GetCurrentPointInShrstPath(), _enemies[i]->GetPosition()) > 1.f)
//		{
//			SetEnemyDirection(i, _enemies[i]->GetCurrentPointInShrstPath());
//			MoveEnemy(i);
//			if (CheckEnemiesToPlayerLine(i))
//			{
//				if (player->_items.size() > 0 && (player->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT)))
//				{
//					_enemies[i]->SetState(EnemyState::GETINRANGEOFPLAYER);
//				}
//				else _enemies[i]->SetState(EnemyState::CHASE);
//			}
//		}
//
//		else
//		{
//			_enemies[i]->SetNextPointInShrtstPath();
//		}
//
//		if (_enemies[i]->closestDoor == nullptr) CheckDistWithDoors(i);
//
//		else if (_enemies[i]->closestDoor != nullptr && _enemies[i]->closestDoor->isOpen == false)
//		{
//			OpenDoorbyEnemy(i);
//			if (_enemies[i]->closestDoor->p1.x > _enemies[i]->closestDoor->p2.x)
//			{
//				MakeDirtyRect(_enemies[i]->closestDoor->p2.x - 100, _enemies[i]->closestDoor->p1.x + 100, _enemies[i]->closestDoor->p2.y - 200, _enemies[i]->closestDoor->p2.y + 200);
//			}
//			else if (_enemies[i]->closestDoor->p1.x < _enemies[i]->closestDoor->p2.x)
//			{
//				MakeDirtyRect(_enemies[i]->closestDoor->p1.x, _enemies[i]->closestDoor->p2.x, _enemies[i]->closestDoor->p2.y - 200, _enemies[i]->closestDoor->p2.y + 200);
//			}
//			_enemies[i]->closestDoor->isOpen = true;
//			_enemies[i]->closestDoor = nullptr;
//			updateGridInsideRectangle();
//			hasMapUpdated = true;
//		}
//		continue;
//		}
//
//		case EnemyState::LOOKFORPLAYER:
//		{
//			if (Distance(_enemies[i]->_dynamicWaypt, _enemies[i]->GetPosition()) > 0.5f)
//			{
//				SetEnemyDirection(i, _enemies[i]->_dynamicWaypt);
//				MoveEnemy(i);
//				if (CheckEnemiesToPlayerLine(i)) DecideToTakeCover(i);
//				continue;
//			}
//			else
//			{
//			/*	int r = rand();
//				if (r % 2 == 0)
//				{
//					int g = rand();
//					if (g % 2 == 0) _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x + 60, _enemies[i]->GetPosition().y);
//					else _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x - 60, _enemies[i]->GetPosition().y);
//				}
//				else
//				{
//					int g = rand();
//					if (g % 2 == 0) _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x, _enemies[i]->GetPosition().y + 40);
//					else _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x, _enemies[i]->GetPosition().y - 40);
//				}
//				FindDynamicWayPtForEnemy(i, &_enemies[i]->_dynamicWaypt);*/
//				_enemies[i]->_dynamicWaypt = pathFinder.Findpointnearby(_enemies[i]->GetPosition());
//				SetEnemyDirection(i, _enemies[i]->_dynamicWaypt);
//				break;
//			}
//		}
//
//		case EnemyState::GETINRANGEOFPLAYER:
//		{
//			if (Distance(_enemies[i]->GetPosition(), player->GetPosition()) > 250.f)
//			{
//				SetEnemyDirection(i, player->GetPosition());
//				MoveEnemy(i);
//			}
//
//			else if (Distance(_enemies[i]->GetPosition(), player->GetPosition()) < 250.f)
//			{
//				DecideToTakeCover(i);
//				break;
//			}
//			
//			break;
//		}
//
//		case EnemyState::RUNAWAY:
//		{
//			if (Distance(_enemies[i]->_shortestPath[0], _enemies[i]->GetPosition()) <= 1.f)
//			{
//				if (_enemies[i]->GetSuspicionLevel()==SuspicionLevel::TAKECOVERANDSHOOT)
//				{
//					_enemies[i]->SetState(EnemyState::TAKECOVER);
//					_enemies[i]->hasBeenAssignedPath = false;
//				}
//				break;
//			}
//
//			if (Distance(_enemies[i]->GetCurrentPointInShrstPath(), _enemies[i]->GetPosition()) > 1.f)
//			{
//				SetEnemyDirection(i, _enemies[i]->GetCurrentPointInShrstPath());
//				MoveEnemy(i);
//			}
//
//			else
//			{
//				_enemies[i]->SetNextPointInShrtstPath();
//			}
//		}
//
//		default:
//			break;
//
//		}
//	}
//}

void MainMenuState::EnemyToReviveCalc(int index)
{
	for (auto enemy : _enemies)
				{
					if (enemy->GetID() == _enemies[index]->EnemyToReviveId)
					{
						if (CheckEnemyToPointIntersection(enemy->GetPosition(), _enemies[index]->GetPosition()))
						{
							_enemies[index]->SetShortestPath(PathFinder::CheckPath(_enemies[index]->GetPosition(), enemy->GetPosition()));
							_enemies[index]->hasBeenAssignedPath = true;
						}
					     
						else _enemies[index]->_shortestPath[0] = enemy->GetPosition();
					}
				}
}

bool MainMenuState::isWeaponInDangerZone(int i,sf::Vector2f weaponPos)
{
	if (checkEnemyToPlayer(i))
	{
		_lastSeenPositionOfPlayer = player->getPostion();
	}

	if (Distance(weaponPos, _lastSeenPositionOfPlayer) < 250.f)
	{
		return false;
	}

	else return true;
}

void MainMenuState::DecideToTakeCover(int i)
{
	if (CheckEnemiesToPlayerLine(i))
	{
		_lastSeenPositionOfPlayer = player->GetPosition();
		if (Distance(_enemies[i]->GetPosition(), player->GetPosition()) < 250.f)
		{
			if (player->_items.size() > 0 && player->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
			{
				std::pair<sf::Vector2f, Wall*> coverWithWall = PathFinder::GetPositionBehindCover(player->GetPosition(), _enemies[i]->GetPosition());
				if (coverWithWall.first != sf::Vector2f(NULL, NULL))
				{
					_enemies[i]->SetSuspicionLevel(SuspicionLevel::TAKECOVERANDSHOOT);
					_enemies[i]->lastCoverPosition = coverWithWall.first;
					_enemies[i]->coverWall = coverWithWall.second;
					_enemies[i]->closestPosToShootNearCover = PathFinder::FindClosestOutOfCoverPosition(_enemies[i]->lastCoverPosition, _lastSeenPositionOfPlayer, _enemies[i]->coverWall);
					if (CheckEnemyToPointIntersection(_enemies[i]->lastCoverPosition, _enemies[i]->GetPosition()))
					{
						_enemies[i]->SetShortestPath(PathFinder::CheckPath(_enemies[i]->GetPosition(), _enemies[i]->lastCoverPosition));
						_enemies[i]->hasBeenAssignedPath = true;
 				    	_enemies[i]->SetState(EnemyState::RUNAWAY);
						_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.runawayInstance);
					}

					else
					{
						_enemies[i]->SetState(EnemyState::TAKECOVER);
						_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.takeCover);
					}
					std::random_device rd;
					std::mt19937 gen(rd());
					std::uniform_int_distribution<> distr(5.f, 7.f);
					_enemies[i]->coverTime = distr(gen);
				}
				else
				{
					_enemies[i]->SetState(EnemyState::SHOOT);
					_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.shootInstance);
					_enemies[i]->SetSuspicionLevel(SuspicionLevel::DIRECTSHOOT);
				}
			}

			else
			{
				std::pair<int, Item*> meleeItem = FindNearestWeapon(i);

				if (meleeItem.first != -1)
				{
					_enemies[i]->SetState(EnemyState::FINDWEAPONNEARBY);
					_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.findweaponnearByInstance);
				}
				else
				{
					std::pair<sf::Vector2f, Wall*> coverWithWall = PathFinder::GetPositionBehindCover(player->GetPosition(), _enemies[i]->GetPosition());
					if (coverWithWall.first != sf::Vector2f(NULL, NULL))
					{
						_enemies[i]->SetSuspicionLevel(SuspicionLevel::TAKECOVERANDSHOOT);
						_enemies[i]->lastCoverPosition = coverWithWall.first;
						_enemies[i]->coverWall = coverWithWall.second;
						_enemies[i]->closestPosToShootNearCover = PathFinder::FindClosestOutOfCoverPosition(_enemies[i]->lastCoverPosition, _lastSeenPositionOfPlayer, _enemies[i]->coverWall);
						if (CheckEnemyToPointIntersection(_enemies[i]->lastCoverPosition, _enemies[i]->GetPosition()))
						{
							_enemies[i]->SetShortestPath(PathFinder::CheckPath(_enemies[i]->GetPosition(), _enemies[i]->lastCoverPosition));
							_enemies[i]->hasBeenAssignedPath = true;
							_enemies[i]->SetState(EnemyState::RUNAWAY);
							_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.runawayInstance);
						}

						else
						{
							_enemies[i]->SetState(EnemyState::TAKECOVER);
							_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.takeCover);
						}
						std::random_device rd;
						std::mt19937 gen(rd());
						std::uniform_int_distribution<> distr(5.f, 7.f);
						_enemies[i]->coverTime = distr(gen);
					}
					else
					{
						_enemies[i]->SetState(EnemyState::SHOOT);
						_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.shootInstance);
						_enemies[i]->SetSuspicionLevel(SuspicionLevel::DIRECTSHOOT);
					}
				}
			}
		}

		else
		{
			_enemies[i]->SetState(EnemyState::GETINRANGEOFPLAYER);
			_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.getInRangeOfPlayerInstance);
		}
	}

	else
	{
		_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
		_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.makePathToEnemyInstance);
		_enemies[i]->hasBeenAssignedPath = false;
	}
}

void MainMenuState::MoveItems()
{
	for (int i = 0; i < _items.size(); i++)
	{
		if (_items[i]->CheckTrait(ItemTrait::EXPLODESONHIT) && _items[i]->needToMove)
		{
			if(Distance(_items[i]->targetPos,_items[i]->GetPosition()) > 10.f)
			{
				sf::Vector2f nextPos = _items[i]->GetPosition() + sf::Vector2f(_items[i]->GetDirection().x * 3.0, _items[i]->GetDirection().y * 3.0);
				_items[i]->_Itemline.p1 = _items[i]->GetPosition();
				_items[i]->_Itemline.p2 = _items[i]->GetPosition() + (_items[i]->GetDirection() * (float)20.0);
				_items[i]->SetPosition(nextPos);
			}
			else
			{
				_items[i]->SetDirection(sf::Vector2f(NULL, NULL));
				_items[i]->needToMove = false;
				Message grenadeMsg(-1, _items[i]->GetID(), MessageType::DIEFROMGRENADE, 10.f, 200.f, _items[i]->GetPosition());
				msgDisptch.AddDispatchMessage(grenadeMsg, _data->_Globalclock.getElapsedTime().asSeconds());
			}
		}

		else
		{
			sf::Vector2f nextPos = _items[i]->GetPosition() + sf::Vector2f(_items[i]->GetDirection().x * 3.0, _items[i]->GetDirection().y * 3.0);
			_items[i]->_Itemline.p1 = _items[i]->GetPosition();
			_items[i]->_Itemline.p2 = _items[i]->GetPosition() + (_items[i]->GetDirection() * (float)20.0);
			_items[i]->SetPosition(nextPos);
		}
	}
}


Item* MainMenuState::ItemsWithFlags(ItemTrait iT)
{
	for (int i = 0; i < _items.size(); i++)
	{
		if (_items[i]->CheckTrait(iT)) return _items[i];
	}

	return nullptr;
}

std::pair<int,Item*> MainMenuState::FindNearestWeapon(int index)
{
	std::pair<float, std::pair<int,Item*>> ClosestItem(-1, std::make_pair(-1, nullptr));
	for (int j = 0; j < _items.size(); j++)
	{
		if (_items[j]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE) && !_items[j]->GonnaCapturedByotherEnemy)
		{
			if (isWeaponInDangerZone(index, _items[j]->GetPosition()) && (Distance(_lastSeenPositionOfPlayer, _items[j]->GetPosition()) > Distance(_enemies[index]->GetPosition(), _items[j]->GetPosition())))
			{
				float dist = Distance(_enemies[index]->GetPosition(), _items[j]->GetPosition());
				if (ClosestItem.first == -1.f) ClosestItem = std::make_pair(dist, std::make_pair(j, _items[j]));
				if (dist < ClosestItem.first) ClosestItem = std::make_pair(dist, std::make_pair(j, _items[j]));
			}
		}
	}
	ClosestItem.second.second->GonnaCapturedByotherEnemy = true;
	return ClosestItem.second;
}

void MainMenuState::SetEnemyDirection(int i, sf::Vector2f pos)
{
	sf::Vector2f enemy2wayPt = UnitVector(pos - _enemies[i]->_line.p1);
	_enemies[i]->_line.p2 = Line2Mouse(_enemies[i]->_line.p1, enemy2wayPt);
	sf::Vector2f unitVectorDir = UnitVector(_enemies[i]->_line.p2 - _enemies[i]->_line.p1);
	_enemies[i]->SetToDirection(unitVectorDir);
	_enemies[i]->UpdateGunTargetLine();
}


void MainMenuState::CheckWhereIsEnemy(int i)
{
	if (CheckEnemiesToPlayerLine(i))
	{
		_lastSeenPositionOfPlayer = player->GetPosition();

		_enemies[i]->SetSuspicionLevel(SuspicionLevel::SAWPLAYER);

		if (_enemies[i]->GetState() != EnemyState::CALLFORHELP)
		{
		    _enemies[i]->SetState(EnemyState::CALLFORHELP);
			_enemies[i]->enemymachine->ChangeState(&_enemies[i]->stateObjects.callforhelpInstance);
		}

		else if (_enemies[i]->_items.size() < 1)
		{
			_enemies[i]->SetState(EnemyState::FINDWEAPONNEARBY);
		}

		else
		{
			if (seeingThroughWindow(i))
			{
				_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
			}
			else _enemies[i]->SetState(EnemyState::CHASE);
		}

	}

}

void MainMenuState::MakeDirtyRect(float minX, float maxX, float minY, float maxY)
{
	rectanglePts[0] = minX;
	rectanglePts[1] = maxX;
	rectanglePts[2] = minY;
	rectanglePts[3] = maxY;
}

void MainMenuState::updateGridInsideRectangle()
{
	if (rectanglePts[0] != oldrectanglePts[0] || rectanglePts[1] != oldrectanglePts[1] || rectanglePts[2] != oldrectanglePts[2] || rectanglePts[3] != oldrectanglePts[3])
	{
		PathFinder::UpdateNodesInRectangle(rectanglePts);
	}

	for (int i = 0; i < 4; i++)
	{
		oldrectanglePts[i] = rectanglePts[i];
	}
}

void MainMenuState::FindDynamicWayPtForEnemy(int i, sf::Vector2f* dynPt)
{
	for (int wI = 0; wI < _walls.size(); wI++)
	{
		if (areLinesColliding(_enemies[i]->GetPosition(), *dynPt, _walls[wI]->p1, _walls[wI]->p2))
		{
			sf::Vector2f ClosestPtToenemy = GetClosesPointToDoor(_enemies[i]->GetPosition(), _walls[wI]->p1, _walls[wI]->p2);
			if ((ClosestPtToenemy.x > _walls[wI]->p2.x || ClosestPtToenemy.y > _walls[wI]->p2.y || ClosestPtToenemy.x < _walls[wI]->p1.x || ClosestPtToenemy.y < _walls[wI]->p1.y))
			{
				continue;
			}
			if (ClosestPtToenemy.x != _enemies[i]->GetPosition().x)
			{
				float extraDist;
				if (dynPt->x > ClosestPtToenemy.x) extraDist = dynPt->x - ClosestPtToenemy.x;
				else extraDist = ClosestPtToenemy.x - dynPt->x;

				if (dynPt->x < ClosestPtToenemy.x) dynPt->x = dynPt->x + (extraDist + 30);
				else dynPt->x = dynPt->x - (extraDist + 30);
				FindDynamicWayPtForEnemy(i, dynPt);
			}

			else if (ClosestPtToenemy.y != _enemies[i]->GetPosition().y)
			{
				float extraDist;
				if (dynPt->y > ClosestPtToenemy.y) extraDist = dynPt->y - ClosestPtToenemy.y;
				else extraDist = ClosestPtToenemy.y - dynPt->y;
				if (dynPt->y < ClosestPtToenemy.y) dynPt->y = dynPt->y + (extraDist + 30);
				else dynPt->y = dynPt->y - (extraDist + 30);
				FindDynamicWayPtForEnemy(i, dynPt);
			}

		}

	}

	for (int wI = 0; wI < _doors.size(); wI++)
	{
		if (areLinesColliding(_enemies[i]->GetPosition(), *dynPt, _doors[wI]->p1, _doors[wI]->p2))
		{
			sf::Vector2f ClosestPtToenemy = GetClosesPointToDoor(_enemies[i]->GetPosition(), _doors[wI]->p1, _doors[wI]->p2);
			if ((ClosestPtToenemy.x > _doors[wI]->p2.x || ClosestPtToenemy.y > _doors[wI]->p2.y || ClosestPtToenemy.x < _doors[wI]->p1.x || ClosestPtToenemy.y < _doors[wI]->p1.y))
			{
				continue;
			}
			if (ClosestPtToenemy.x != _enemies[i]->GetPosition().x)
			{
				float extraDist;
				if (dynPt->x > ClosestPtToenemy.x) extraDist = dynPt->x - ClosestPtToenemy.x;
				else extraDist = ClosestPtToenemy.x - dynPt->x;

				if (dynPt->x < ClosestPtToenemy.x) dynPt->x = dynPt->x + (extraDist + 30);
				else dynPt->x = dynPt->x - (extraDist + 30);
				FindDynamicWayPtForEnemy(i, dynPt);
			}

			else if (ClosestPtToenemy.y != _enemies[i]->GetPosition().y)
			{
				float extraDist;
				if (dynPt->y > ClosestPtToenemy.y) extraDist = dynPt->y - ClosestPtToenemy.y;
				else extraDist = ClosestPtToenemy.y - dynPt->y;
				if (dynPt->y < ClosestPtToenemy.y) dynPt->y = dynPt->y + (extraDist + 30);
				else dynPt->y = dynPt->y - (extraDist + 30);
				FindDynamicWayPtForEnemy(i, dynPt);
			}

		}

	}

	for (int wI = 0; wI < _windows.size(); wI++)
	{
		if (areLinesColliding(_enemies[i]->GetPosition(), *dynPt, _windows[wI]->p1, _windows[wI]->p2))
		{
			sf::Vector2f ClosestPtToenemy = GetClosesPointToDoor(_enemies[i]->GetPosition(), _windows[wI]->p1, _windows[wI]->p2);
			if ((ClosestPtToenemy.x > _windows[wI]->p2.x || ClosestPtToenemy.y > _windows[wI]->p2.y || ClosestPtToenemy.x < _windows[wI]->p1.x || ClosestPtToenemy.y < _windows[wI]->p1.y))
			{
				continue;
			}
			if (ClosestPtToenemy.x != _enemies[i]->GetPosition().x)
			{
				float extraDist;
				if (dynPt->x > ClosestPtToenemy.x) extraDist = dynPt->x - ClosestPtToenemy.x;
				else extraDist = ClosestPtToenemy.x - dynPt->x;

				if (dynPt->x < ClosestPtToenemy.x) dynPt->x = dynPt->x + (extraDist + 30);
				else dynPt->x = dynPt->x - (extraDist + 30);
				FindDynamicWayPtForEnemy(i, dynPt);
			}

			else if (ClosestPtToenemy.y != _enemies[i]->GetPosition().y)
			{
				float extraDist;
				if (dynPt->y > ClosestPtToenemy.y) extraDist = dynPt->y - ClosestPtToenemy.y;
				else extraDist = ClosestPtToenemy.y - dynPt->y;
				if (dynPt->y < ClosestPtToenemy.y) dynPt->y = dynPt->y + (extraDist + 30);
				else dynPt->y = dynPt->y - (extraDist + 30);
				FindDynamicWayPtForEnemy(i, dynPt);
			}

		}

	}
}


bool MainMenuState::CheckEnemyToItems(int ItemIndex, int EnemyIndex)
{
	for (int wI = 0; wI < _walls.size(); wI++)
	{
		if (areLinesColliding(_enemies[EnemyIndex]->GetPosition(), _items[ItemIndex]->GetPosition(), _walls[wI]->p1, _walls[wI]->p2))
		{
			return true;
		}
	}


	for (int wI = 0; wI < _doors.size(); wI++)
	{
		if (areLinesColliding(_enemies[EnemyIndex]->GetPosition(), _items[ItemIndex]->GetPosition(), _doors[wI]->p1, _doors[wI]->p2))
		{
			return true;
		}
	}

	for (int wI = 0; wI < _windows.size(); wI++)
	{
		if (areLinesColliding(_enemies[EnemyIndex]->GetPosition(), _items[ItemIndex]->GetPosition(), _windows[wI]->p1, _windows[wI]->p2))
		{
			return true;
		}
	}

	return false;
}


void MainMenuState::MoveEnemy(int i)
{
	sf::Vector2f nxtPos = _enemies[i]->GetPosition() + _enemies[i]->GetDirection();
	_enemies[i]->SetPosition(nxtPos);
	_enemies[i]->_line.p2 = _enemies[i]->GetPosition() + (_enemies[i]->GetDirection() * (float)50.0);
	_enemies[i]->UpdateGunTargetLine();
}

bool MainMenuState::seeingThroughWindow(int index)
{
	for (int wI = 0; wI < _windows.size(); wI++)
	{
		if (areLinesColliding(_enemies[index]->GetPosition(), player->getPostion(), _windows[wI]->p1, _windows[wI]->p2))
		{
			return true;
		}
	}

	return false;
}

void MainMenuState::OpenDoorbyEnemy(int index, Player *plyr)
{
	if (plyr == nullptr)
	{
		if (_enemies[index]->closestPointToDoorFromEnemy.x != _enemies[index]->GetPosition().x)
		{
			if (_enemies[index]->closestPointToDoorFromEnemy.x < _enemies[index]->GetPosition().x)
			{
				float length = (_enemies[index]->closestDoor->p2.y - _enemies[index]->closestDoor->p1.y);
				_enemies[index]->closestDoor->p1.x -= length;
				_enemies[index]->closestDoor->p1.y = _enemies[index]->closestDoor->p2.y;
				_enemies[index]->closestDoor->minX = _enemies[index]->closestDoor->p1.x;
				_enemies[index]->closestDoor->maxX = _enemies[index]->closestDoor->p2.x;
				_enemies[index]->closestDoor->minY = _enemies[index]->closestDoor->p1.y;
				_enemies[index]->closestDoor->maxY = _enemies[index]->closestDoor->p2.y;
				_enemies[index]->_dynamicWaypt = sf::Vector2f(_enemies[index]->GetPosition().x - 60, _enemies[index]->GetPosition().y);

			}
			else
			{
				float length = (_enemies[index]->closestDoor->p2.y - _enemies[index]->closestDoor->p1.y);
				_enemies[index]->closestDoor->p1.x += length;
				_enemies[index]->closestDoor->p1.y = _enemies[index]->closestDoor->p2.y;
				_enemies[index]->closestDoor->minX = _enemies[index]->closestDoor->p2.x;
				_enemies[index]->closestDoor->maxX = _enemies[index]->closestDoor->p1.x;
				_enemies[index]->closestDoor->minY = _enemies[index]->closestDoor->p1.y;
				_enemies[index]->closestDoor->maxY = _enemies[index]->closestDoor->p2.y;
				_enemies[index]->_dynamicWaypt = sf::Vector2f(_enemies[index]->GetPosition().x + 60, _enemies[index]->GetPosition().y);
			}
			//_doorCloseToEnemy->isOpen = true;
		}

		else if (_enemies[index]->closestPointToDoorFromEnemy.y != _enemies[index]->GetPosition().y)
		{
			if (_enemies[index]->closestPointToDoorFromEnemy.y < _enemies[index]->GetPosition().y)
			{
				float length = (_enemies[index]->closestDoor->p2.x - _enemies[index]->closestDoor->p1.x);
				_enemies[index]->closestDoor->p1.x = _enemies[index]->closestDoor->p2.x;
				_enemies[index]->closestDoor->p1.y -= length;
				_enemies[index]->closestDoor->minX = _enemies[index]->closestDoor->p1.x;
				_enemies[index]->closestDoor->maxX = _enemies[index]->closestDoor->p2.x;
				_enemies[index]->closestDoor->minY = _enemies[index]->closestDoor->p1.y;
				_enemies[index]->closestDoor->maxY = _enemies[index]->closestDoor->p2.y;
				_enemies[index]->_dynamicWaypt = sf::Vector2f(_enemies[index]->GetPosition().x, _enemies[index]->GetPosition().y - 60);
			}
			else
			{
				float length = (_enemies[index]->closestDoor->p2.y - _enemies[index]->closestDoor->p1.y);
				_enemies[index]->closestDoor->p1.x = _enemies[index]->closestDoor->p2.x;
				_enemies[index]->closestDoor->p1.y += length;
				_enemies[index]->closestDoor->minX = _enemies[index]->closestDoor->p1.x;
				_enemies[index]->closestDoor->maxX = _enemies[index]->closestDoor->p2.x;
				_enemies[index]->closestDoor->minY = _enemies[index]->closestDoor->p2.y;
				_enemies[index]->closestDoor->maxY = _enemies[index]->closestDoor->p1.y;
				_enemies[index]->_dynamicWaypt = sf::Vector2f(_enemies[index]->GetPosition().x, _enemies[index]->GetPosition().y + 60);
			}

			//_doorCloseToEnemy->isOpen = true;
		}
	}

	else
	{
		if (plyr->closestPointToDoor.x != (plyr->GetPosition().x))
		{
			if ((plyr->closestPointToDoor.x < (plyr->GetPosition().x)))
			{
				float length = (_doorCloseToPlayer->p2.y - _doorCloseToPlayer->p1.y);
				_doorCloseToPlayer->p1.x -= length;
				_doorCloseToPlayer->p1.y = _doorCloseToPlayer->p2.y;
				_doorCloseToPlayer->minX = _doorCloseToPlayer->p1.x;
				_doorCloseToPlayer->maxX = _doorCloseToPlayer->p2.x;
				_doorCloseToPlayer->minY = _doorCloseToPlayer->p1.y;
				_doorCloseToPlayer->maxY = _doorCloseToPlayer->p2.y;
			}
			else
			{
				float length = (_doorCloseToPlayer->p2.y - _doorCloseToPlayer->p1.y);
				_doorCloseToPlayer->p1.x += length;
				_doorCloseToPlayer->p1.y = _doorCloseToPlayer->p2.y;
				_doorCloseToPlayer->minX = _doorCloseToPlayer->p2.x;
				_doorCloseToPlayer->maxX = _doorCloseToPlayer->p1.x;
				_doorCloseToPlayer->minY = _doorCloseToPlayer->p1.y;
				_doorCloseToPlayer->maxY = _doorCloseToPlayer->p2.y;
			}
			//_doorCloseToEnemy->isOpen = true;
		}

		else if (plyr->closestPointToDoor.y != plyr->GetPosition().y)
		{
			if (plyr->closestPointToDoor.y < plyr->GetPosition().y)
			{
				float length = (_doorCloseToPlayer->p2.x - _doorCloseToPlayer->p1.x);
				_doorCloseToPlayer->p1.x = _doorCloseToPlayer->p2.x;
				_doorCloseToPlayer->p1.y -= length;
				_doorCloseToPlayer->minX = _doorCloseToPlayer->p1.x;
				_doorCloseToPlayer->maxX = _doorCloseToPlayer->p2.x;
				_doorCloseToPlayer->minY = _doorCloseToPlayer->p1.y;
				_doorCloseToPlayer->maxY = _doorCloseToPlayer->p2.y;
			}
			else
			{
				float length = (_doorCloseToPlayer->p2.y - _doorCloseToPlayer->p1.y);
				_doorCloseToPlayer->p1.x = _doorCloseToPlayer->p2.x;
				_doorCloseToPlayer->p1.y += length;
				_doorCloseToPlayer->minX = _doorCloseToPlayer->p1.x;
				_doorCloseToPlayer->maxX = _doorCloseToPlayer->p2.x;
				_doorCloseToPlayer->minY = _doorCloseToPlayer->p2.y;
				_doorCloseToPlayer->maxY = _doorCloseToPlayer->p1.y;
			}

			//_doorCloseToEnemy->isOpen = true;
		}
	}
}

void MainMenuState::LoadLevel()
{
	std::string str;
	std::ifstream myfile;
	myfile.open("C:/Users/User/Desktop/Physics/Level.txt");
	while (!myfile.eof())
	{
		char typeName[20];
		sf::Vector2f pos, pos1;

		std::getline(myfile, str);
		int size = str.length();
		char cstr[50];
		strcpy_s(cstr, str.c_str());
		sscanf_s(cstr, "%20s", typeName, (unsigned)_countof(typeName));  /*, &pos.x, &pos.y, &pos1.x, &pos1.y);*/

		if (strcmp("Wall", typeName) == 0 || strcmp("Window", typeName) == 0 || strcmp("Door", typeName) == 0)
		{
			WallWinDoorData _walldata;
			_walldata.m_name = typeName;
			sscanf_s(cstr, "%20s %f %f %f %f", typeName, (unsigned)_countof(typeName), &_walldata.pos.x, &_walldata.pos.y, &_walldata.pos2.x, &_walldata.pos2.y);
			m_wallDataPool.push_back(_walldata);
		}

		else if (strcmp("Enemy", typeName) == 0)
		{
			EnemyTraits enemyData;
			sscanf_s(cstr, "Enemy %20s %f %f", typeName, (unsigned)_countof(typeName), &enemyData.position.x, &enemyData.position.y);
			enemyData.name = typeName;
			m_enemyDataPool.push_back(enemyData);
		}


		else if (strcmp("SpoiledSandwich", typeName) == 0 || strcmp("Toillete", typeName) == 0 || strcmp("Knife", typeName) == 0 || strcmp("Gun", typeName) == 0 || strcmp("grenade", typeName) == 0|| strcmp("Medkit", typeName) == 0 || strcmp("Alarm", typeName) == 0)
		{
			ItemData _itmData;
			char oname[30];
			_itmData.fileName = typeName;
			LoadItemsInLevel(typeName, &_itmData);
			sscanf_s(cstr, "%20s %30s %f %f", typeName, (unsigned)_countof(typeName), oname, (unsigned)_countof(typeName), &_itmData.position.x, &_itmData.position.y);
			_itmData.owner = oname;
			m_ItemDataPool.push_back(_itmData);
		}

		else
		{
			m_enemyDataPool[m_enemyDataPool.size() - 1].route.push_back(sf::Vector2f(0, 0));
			sscanf_s(cstr, "Waypoint %f %f", &m_enemyDataPool[m_enemyDataPool.size() - 1].route[m_enemyDataPool[m_enemyDataPool.size() - 1].route.size() - 1].x, &m_enemyDataPool[m_enemyDataPool.size() - 1].route[m_enemyDataPool[m_enemyDataPool.size() - 1].route.size() - 1].y);
		}
	}

}

void MainMenuState::LoadItemsInLevel(std::string fileName, ItemData *itemDetails)
{
	std::string str;
	std::ifstream myfile;
	myfile.open("C:/Users/User/Desktop/Physics/Items/" + itemDetails->fileName + ".txt");

	if (myfile.fail())
	{
		std::cout << "NoFile";
	}
	while (!myfile.eof())
	{
		char typeName[20];

		std::getline(myfile, str);
		int size = str.length();
		char cstr[50];
		strcpy_s(cstr, str.c_str());
		sscanf_s(cstr, "%20s", typeName, (unsigned)_countof(typeName));

		if (strcmp("Location", typeName) == 0)
		{
			char stringChars[20];
			sscanf_s(cstr, "Location %20s", stringChars, (unsigned)_countof(stringChars));
			itemDetails->spriteName = stringChars;
		}

		else if (strcmp("Name", typeName) == 0)
		{
			char stringChars[20];
			sscanf_s(cstr, "Name %20s", stringChars, (unsigned)_countof(stringChars));
			itemDetails->m_name = stringChars;
			_data->asset.LoadTexture(itemDetails->spriteName, "C:/Users/User/Desktop/Physics/Items/" + itemDetails->spriteName + ".png");
		}

		else if (strcmp("Scale", typeName) == 0)
		{
			sscanf_s(cstr, "Scale %f %f", &itemDetails->scale.x, &itemDetails->scale.y);
		}

		else if (strcmp("Trait", typeName) == 0)
		{
			char stringChars[25];
			sscanf_s(cstr, "Trait %25s", stringChars, (unsigned)_countof(stringChars));
			itemDetails->traitName = stringChars;
			if (EnumStringMap.find(itemDetails->traitName) != EnumStringMap.end())
			{
				itemDetails->_traits[EnumStringMap.find(itemDetails->traitName)->second] = true;
			}
		}

	}

}


void MainMenuState::DrawLines()
{
	for (int i = 0; i < m_wallDataPool.size(); i++)
	{
		if (m_wallDataPool[i].m_name == "Wall")
		{
			Wall* t_wall = new Wall(_data);
			t_wall->SetLinePoints(m_wallDataPool[i].pos, m_wallDataPool[i].pos2);
			_walls.push_back(t_wall);
		}

		else if (m_wallDataPool[i].m_name == "Door")
		{
			Door* t_door = new Door(_data);
			t_door->SetLinePoints(m_wallDataPool[i].pos, m_wallDataPool[i].pos2);
			t_door->minX = m_wallDataPool[i].pos.x;
			t_door->maxX = m_wallDataPool[i].pos2.x;
			t_door->minY = m_wallDataPool[i].pos.y;
			t_door->maxY = m_wallDataPool[i].pos2.y;
			_doors.push_back(t_door);
		}

		else if (m_wallDataPool[i].m_name == "Window")
		{
			Window* t_window = new Window(_data);
			t_window->SetLinePoints(m_wallDataPool[i].pos, m_wallDataPool[i].pos2);
			_windows.push_back(t_window);
		}

		//else if (m_wallDataPool[i].m_name == "RectWall")
		//{
		//	RectWall* t_rectWall = new RectWall(_data);
		//	t_rectWall->SetPosition(sf::Vector2f((m_wallDataPool[i].pos.x + m_wallDataPool[i].pos2.x) / 2, (m_wallDataPool[i].pos.y + m_wallDataPool[i].pos2.y) / 2));
		//	//_RectWalls.push_back(t_rectWall);
		//}
	}

	for (int i = 0; i < m_enemyDataPool.size(); i++)
	{
		Enemy* enemy = new Enemy(_data,*player,&msgDisptch,this);
		//enemy->SetpathFinder(&pathFinder);
		enemy->SetPosition(m_enemyDataPool[i].position);
		enemy->AddPath(m_enemyDataPool[i].route);
		enemy->SetName(m_enemyDataPool[i].name);
		enemy->SetState(EnemyState::PATROL);
		enemy->SetID(++GlobalIDCounter);
		_enemies.push_back(enemy);
		handlerManager.Register(enemy);
		sf::Vector2f* enemyForMapPos = &enemy->curPos;
		std::pair<sf::Vector2f*, std::function<void(const Message&)>> FunctionWithDist = std::make_pair(enemyForMapPos, std::bind(&EnemyHandler::GoToToilet, enemy->enemyHandler, std::placeholders::_1));
		std::pair<int, std::pair<sf::Vector2f*, std::function<void(const Message&)>>> FunctionWithId = std::make_pair(enemy->GetID(), FunctionWithDist);
		handlerManager.RegisterWithFunction(MessageType::GOTOTOILET, FunctionWithId);
		std::pair<sf::Vector2f*, std::function<void(const Message&)>> FunctionWithDist2 = std::make_pair(enemyForMapPos, std::bind(&EnemyHandler::GoForHelp, enemy->enemyHandler, std::placeholders::_1));
		std::pair<int, std::pair<sf::Vector2f*, std::function<void(const Message&)>>> FunctionWithId2 = std::make_pair(enemy->GetID(), FunctionWithDist2);
		handlerManager.RegisterWithFunction(MessageType::GOFORHELP, FunctionWithId2);
		std::pair<sf::Vector2f*, std::function<void(const Message&)>> FunctionWithDist3 = std::make_pair(enemyForMapPos, std::bind(&EnemyHandler::DieFromGrenade, enemy->enemyHandler, std::placeholders::_1));
		std::pair<int, std::pair<sf::Vector2f*, std::function<void(const Message&)>>> FunctionWithId3 = std::make_pair(enemy->GetID(), FunctionWithDist3);
		handlerManager.RegisterWithFunction(MessageType::DIEFROMGRENADE, FunctionWithId3);
		std::pair<sf::Vector2f*, std::function<void(const Message&)>> FunctionWithDist4 = std::make_pair(enemyForMapPos, std::bind(&EnemyHandler::Revived, enemy->enemyHandler, std::placeholders::_1));
		std::pair<int, std::pair<sf::Vector2f*, std::function<void(const Message&)>>> FunctionWithId4 = std::make_pair(enemy->GetID(), FunctionWithDist4);
		handlerManager.RegisterWithFunction(MessageType::REVIVED, FunctionWithId4);
		std::pair<sf::Vector2f*, std::function<void(const Message&)>> FunctionWithDist5 = std::make_pair(enemyForMapPos, std::bind(&EnemyHandler::SaveFriendsAss, enemy->enemyHandler, std::placeholders::_1));
		std::pair<int, std::pair<sf::Vector2f*, std::function<void(const Message&)>>> FunctionWithId5 = std::make_pair(enemy->GetID(), FunctionWithDist5);
		handlerManager.RegisterWithFunction(MessageType::SAVEFRIENDSASS, FunctionWithId5);
		std::pair<sf::Vector2f*, std::function<void(const Message&)>> FunctionWithDist6 = std::make_pair(enemyForMapPos, std::bind(&EnemyHandler::RunAwayFromGrenade, enemy->enemyHandler, std::placeholders::_1));
		std::pair<int, std::pair<sf::Vector2f*, std::function<void(const Message&)>>> FunctionWithId6 = std::make_pair(enemy->GetID(), FunctionWithDist6);
		handlerManager.RegisterWithFunction(MessageType::RUNAWAYFROMGRENADE, FunctionWithId6);
	}

	for (int i = 0; i < m_ItemDataPool.size(); i++)
	{
		Item* item = new Item(_data);
		item->SetName(m_ItemDataPool[i].m_name); 
		item->SetPosition(m_ItemDataPool[i].position);
		item->SetTraits(m_ItemDataPool[i]._traits);
		item->SetTexture(m_ItemDataPool[i].spriteName);
		item->_Itemline.p1 = sf::Vector2f(item->GetPosition().x, item->GetPosition().y);
		item->_Itemline.p2 = sf::Vector2f(item->GetPosition().x + 30, item->GetPosition().y);
		item->SetID(++GlobalIDCounter);
		item->SetSize(m_ItemDataPool[i].scale);
		FindEnemyByName(m_ItemDataPool[i].owner,item);
		handlerManager.Register(item);
		sf::Vector2f* itemForMap = &item->curPos;
		std::pair<sf::Vector2f*, std::function<void(const Message&)>> FunctionWithDist = std::make_pair(itemForMap, std::bind(&ItemHandler::DieFromGrenade, item->itemhandler, std::placeholders::_1));
		std::pair<int, std::pair<sf::Vector2f*, std::function<void(const Message&)>>> FunctionWithId = std::make_pair(item->GetID(), FunctionWithDist);
		handlerManager.RegisterWithFunction(MessageType::DIEFROMGRENADE, FunctionWithId);
	}
}
 
void MainMenuState::FindEnemyByName(std::string name, Item* itm)
{
	if (name != "None")
	{
		auto FindEnemyByName = [name, itm](Enemy* e1) 
		{
			if (e1->GetName() == name)
			{
				e1->_items.push_back(itm);
			}
			return e1->GetName() == name;
		};
		itm->IsPicked = true;
		std::find_if(_enemies.begin(), _enemies.end(), FindEnemyByName);
	}
	
	else _items.push_back(itm);
}