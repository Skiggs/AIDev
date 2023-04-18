#include"EnemyState.h"
#include"MathFunc.h"
#include"PathFinder.h"
#include"EnemyStateMachine.h"
#include "MainMenu.h"
#include"MessageDispatcher.h"
#include"Enemy.h"
#include<random>

//void EnemyStates::CheckWhereIsPlayer()
//{
//	if (parentEnemy->CheckLineToPlayer())
//	{
//		parentEnemy->SetSuspicionLevel(SuspicionLevel::SAWPLAYER);
//
//		parentEnemy->enemymachine->ChangeState(new FindWeaponNearBy());
//
//		Message helpmsg(-1, parentEnemy->GetID(), MessageType::GOFORHELP, 0.0f, 800.f, parentEnemy->GetPosition());
//		messageDispatcher->AddDispatchMessage(helpmsg, _dataref->_Globalclock.getElapsedTime().asSeconds());
//		
//
//		/*else if (parentEnemy->_items.size() < 1)
//		{
//			parentEnemy->SetState(EnemyState::FINDWEAPONNEARBY);
//		}*/
//
//		/*else
//		{
//			if (seeingThroughWindow(i))
//			{
//				_enemies[i]->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//			}
//			else _enemies[i]->SetState(EnemyState::CHASE);
//		}*/
//
//	}
//}

void Patrol::Execute(EnemyStatesData* curData,int index)
{
	if (curData->parentEnemy->GetSuspicionLevel() == SuspicionLevel::RELAXED)
	{
		if (Distance(curData->parentEnemy->GetCurrentWayPt(), curData->parentEnemy->GetPosition()) > 0.5f)
		{
			curData->parentEnemy->SetDirection(curData->parentEnemy->GetCurrentWayPt());
			curData->parentEnemy->Move();
			curData->mainmenuptr->CheckWhereIsEnemy(index);
		}

		else
		{
			curData->parentEnemy->SetNextWayPoint();
			curData->parentEnemy->_line.p2 = RotateLine(0.2, curData->parentEnemy->_line.p1, curData->parentEnemy->_line.p2);
			sf::Vector2f direction = curData->parentEnemy->_line.p2 - curData->parentEnemy->_line.p1;
			sf::Vector2f direction2 = curData->parentEnemy->GetCurrentWayPt() - curData->parentEnemy->_line.p1;
			double angle = AngleBWVectors(direction, direction2);
			curData->parentEnemy->_line.p2 = RotateLine(-angle + 0.2, curData->parentEnemy->_line.p1, curData->parentEnemy->_line.p2);
			sf::Vector2f unitVectorDir = UnitVector(curData->parentEnemy->_line.p2 - curData->parentEnemy->_line.p1);
			curData->parentEnemy->SetDirection(unitVectorDir);
		}
	}

	/*else if (parentEnemy->GetSuspicionLevel() == SuspicionLevel::DOUBTFUL)
	{
		bool isItemnear = false;
		if (!parentEnemy->_hasReachedSuspPos)
		{
			if (Distance(parentEnemy->GetCurrentPointInShrstPath(), parentEnemy->GetPosition()) > 0.5f)
			{
				parentEnemy->SetToDirection(parentEnemy->GetCurrentPointInShrstPath());
				parentEnemy->Move();
			}

			else
			{
				parentEnemy->SetNextPointInShrtstPath();
			}

			CheckWhereIsPlayer();
		}
		else
		{
			if (Distance(parentEnemy->GetPosition(), parentEnemy->_dynamicWaypt) > 0.5f)
			{
				parentEnemy->SetToDirection(parentEnemy->_dynamicWaypt);
				parentEnemy->Move();
				CheckWhereIsPlayer();
			}
			else
			{
				parentEnemy->_rotateRad += dt;
				parentEnemy->_line.p2 = RotateLine(parentEnemy->_rotateRad, parentEnemy->_line.p1, parentEnemy->_line.p2);
				if (parentEnemy->nearByItem != nullptr && parentEnemy->nearByItem->CheckTrait(ItemTrait::CONSUMABLE))
				{
					std::vector<Item*>::iterator it;
					int index = 0;
					for (it = _items.begin(); it < _items.end(); it++)
					{
						if (*it == _enemies[i]->nearByItem)
						{
							break;
						}
						index++;
					}
					Message sandWichMsg(_enemies[i]->GetID(), _enemies[i]->nearByItem->GetID(), MessageType::GOTOTOILET, 6.f, 0.f);
					msgDisptch.AddDispatchMessage(sandWichMsg, _data->_Globalclock.getElapsedTime().asSeconds());
					_items.erase(_items.begin() + index);
					_enemies[i]->nearByItem = nullptr;
				}

				CheckWhereIsEnemy(i);
			}
			_enemies[i]->_coolDownTime -= 0.1;
			if (_enemies[i]->_coolDownTime < 0.f)
			{
				_enemies[i]->SetSuspicionLevel(SuspicionLevel::RELAXED);
				_enemies[i]->_dynamicWaypt = sf::Vector2f(NULL, NULL);
				_enemies[i]->_coolDownTime = 30.f;
				_enemies[i]->_hasReachedSuspPos = false;
				break;
			}

			continue;
		}

		if (_enemies[i]->closestDoor == nullptr) CheckDistWithDoors(i);

		else if (_enemies[i]->closestDoor != nullptr && _enemies[i]->closestDoor->isOpen == false)
		{
			OpenDoorbyEnemy(i);
			if (_enemies[i]->closestDoor->p1.x > _enemies[i]->closestDoor->p2.x)
			{
				MakeDirtyRect(_enemies[i]->closestDoor->p2.x - 100, _enemies[i]->closestDoor->p1.x + 100, _enemies[i]->closestDoor->p2.y - 200, _enemies[i]->closestDoor->p2.y + 200);
			}
			else if (_enemies[i]->closestDoor->p1.x < _enemies[i]->closestDoor->p2.x)
			{
				MakeDirtyRect(_enemies[i]->closestDoor->p1.x, _enemies[i]->closestDoor->p2.x, _enemies[i]->closestDoor->p2.y - 200, _enemies[i]->closestDoor->p2.y + 200);
			}
			_enemies[i]->closestDoor->isOpen = true;
			updateGridInsideRectangle();

			FindDynamicWayPtForEnemy(i, &_enemies[i]->_dynamicWaypt);
			_enemies[i]->_hasReachedSuspPos = true;
			_enemies[i]->SetDirection(sf::Vector2f(NULL, NULL));
			_enemies[i]->closestDoor = nullptr;

			break;
		}
		else if (_enemies[i]->closestDoor != nullptr && _enemies[i]->closestDoor->isOpen == true)
		{
			FindDynamicWayPtForEnemy(i, &_enemies[i]->_dynamicWaypt);
			_enemies[i]->_hasReachedSuspPos = true;
			_enemies[i]->SetDirection(sf::Vector2f(NULL, NULL));
			_enemies[i]->closestDoor = nullptr;
			continue;
		}

		continue;
	}*/
}

//void FindWeaponNearBy::Execute(int index)
//{
//	if (!parentEnemy->hasBeenAssignedPath)
//	{
//		std::pair<int, Item*> meleeItem = MainMenuState::FindNearestWeapon(index);
//		if (meleeItem.first != -1)
//		{
//			parentEnemy->SetShortestPath(PathFinder::CheckPath(parentEnemy->GetPosition(), meleeItem.second->GetPosition()));
//			parentEnemy->hasBeenAssignedPath = true;
//		}
//
//		else
//		{
//			parentEnemy->enemymachine->ChangeState(new MakePathToEnemy);
//			//parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//			
//		}
//
//	}
//
//	if (Distance(parentEnemy->_shortestPath[0], parentEnemy->GetPosition()) < 10.f)
//	{
//		{
//			MainMenuState::PickItem(index);
//			parentEnemy->hasBeenAssignedPath = false;
//			parentEnemy->enemymachine->ChangeState(new MakePathToEnemy);
//			//parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
//		}
//	}
//
//	if (Distance(parentEnemy->GetCurrentPointInShrstPath(), parentEnemy->GetPosition()) > 0.5f)
//	{
//		MainMenuState::SetEnemyDirection(index, parentEnemy->GetCurrentPointInShrstPath());
//		MainMenuState::MoveEnemy(index);
//	}
//
//	else
//	{
//		parentEnemy->SetNextPointInShrtstPath();
//	}
//}




void CallForHelp::Execute(EnemyStatesData* curData,int index)
{
	Message helpmsg(-1, curData->parentEnemy->GetID(), MessageType::GOFORHELP, 0.0f, 800.f, curData->parentEnemy->GetPosition());
	curData->messageDispatcher->AddDispatchMessage(helpmsg, curData->_dataref->_Globalclock.getElapsedTime().asSeconds());
	if (curData->parentEnemy->_items.size() > 0 && curData->parentEnemy->_items[0]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE))
		curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);

	else
		curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.findweaponnearByInstance);
}

void MakePathToEnemy::Execute(EnemyStatesData* curData, int index)
{
	if (!curData->parentEnemy->hasBeenAssignedPath)
				{
		curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), curData->mainmenuptr->_lastSeenPositionOfPlayer));
		curData->parentEnemy->hasBeenAssignedPath = true;
				}
		
				if (curData->parentEnemy->hasBeenAssignedPath && curData->mainmenuptr->hasMapUpdated)
				{
					curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), curData->mainmenuptr->_lastSeenPositionOfPlayer));
					return;
				}
				
				if (Distance(curData->parentEnemy->_shortestPath[0], curData->parentEnemy->GetPosition()) < 10.f)
				{
					curData->parentEnemy->_dynamicWaypt = PathFinder::Findpointnearby(curData->parentEnemy->GetPosition());
					curData->parentEnemy->SetState(EnemyState::LOOKFORPLAYER);
					curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.lookForPlayerInstance);
					curData->parentEnemy->hasBeenAssignedPath = false;
					return;
				}
		
				if (Distance(curData->parentEnemy->GetCurrentPointInShrstPath(), curData->parentEnemy->GetPosition()) > 1.f)
				{
					curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->GetCurrentPointInShrstPath());
					curData->mainmenuptr->MoveEnemy(index);
					if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
					{
						if (curData->parentEnemy->playerInstance->_items.size() > 0 && (curData->parentEnemy->playerInstance->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT)))
						{
							curData->parentEnemy->SetState(EnemyState::GETINRANGEOFPLAYER);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.getInRangeOfPlayerInstance);
						}
						else
						{
							curData->parentEnemy->SetState(EnemyState::CHASE);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.chaseInstance);
						}
					}
				}
		
				else
				{
					curData->parentEnemy->SetNextPointInShrtstPath();
				}
		
				if (curData->parentEnemy->closestDoor == nullptr) curData->mainmenuptr->CheckDistWithDoors(index);
		
				else if (curData->parentEnemy->closestDoor != nullptr && curData->parentEnemy->closestDoor->isOpen == false)
				{
					curData->mainmenuptr->OpenDoorbyEnemy(index);
					if (curData->parentEnemy->closestDoor->p1.x > curData->parentEnemy->closestDoor->p2.x)
					{
						curData->mainmenuptr->MakeDirtyRect(curData->parentEnemy->closestDoor->p2.x - 100, curData->parentEnemy->closestDoor->p1.x + 100, curData->parentEnemy->closestDoor->p2.y - 200, curData->parentEnemy->closestDoor->p2.y + 200);
					}
					else if (curData->parentEnemy->closestDoor->p1.x < curData->parentEnemy->closestDoor->p2.x)
					{
						curData->mainmenuptr->MakeDirtyRect(curData->parentEnemy->closestDoor->p1.x, curData->parentEnemy->closestDoor->p2.x, curData->parentEnemy->closestDoor->p2.y - 200, curData->parentEnemy->closestDoor->p2.y + 200);
					}
					curData->parentEnemy->closestDoor->isOpen = true;
					curData->parentEnemy->closestDoor = nullptr;
					curData->mainmenuptr->updateGridInsideRectangle();
					curData->mainmenuptr->hasMapUpdated = true;
				}
}

void DecideHowToHelp::Execute(EnemyStatesData* curData, int index)
{
	curData->mainmenuptr->DecideToTakeCover(index);
}

void Runaway::Execute(EnemyStatesData* curData, int index)
{
	if (Distance(curData->parentEnemy->_shortestPath[0], curData->parentEnemy->_shortestPath[0]) <= 1.f)
			{
				if (curData->parentEnemy->GetSuspicionLevel()==SuspicionLevel::TAKECOVERANDSHOOT)
				{
					curData->parentEnemy->SetState(EnemyState::TAKECOVER);
					curData->parentEnemy->hasBeenAssignedPath = false;
					curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.takeCover);
					
				}
				return;
			}

			if (Distance(curData->parentEnemy->GetCurrentPointInShrstPath(), curData->parentEnemy->GetPosition()) > 1.f)
			{
				curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->GetCurrentPointInShrstPath());
				curData->mainmenuptr->MoveEnemy(index);
			}

			else
			{
				curData->parentEnemy->SetNextPointInShrtstPath();
			}
}

void TakeCover::Execute(EnemyStatesData* curData, int index)
{
	if (curData->parentEnemy->coverTime > 0.f)
	{
		curData->parentEnemy->hasBeenAssignedPath = false;
		curData->parentEnemy->coverTime -= 1.0f / 60.0f;
		if (Distance(curData->parentEnemy->lastCoverPosition, curData->parentEnemy->GetPosition()) > 0.5f && curData->parentEnemy->hasBeenAssignedPath)
		{
			curData->mainmenuptr->SetEnemyDirection(index, curData->mainmenuptr->_lastSeenPositionOfPlayer);
			curData->mainmenuptr->MoveEnemy(index);
		}

		else
		{
			curData->mainmenuptr->SetEnemyDirection(index, curData->mainmenuptr->_lastSeenPositionOfPlayer);
			if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
			{
				curData->mainmenuptr->DecideToTakeCover(index);
			}
		}
		return;
	}

	else if (curData->parentEnemy->_items.size() > 0 && (curData->parentEnemy->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT) || curData->parentEnemy->_items[0]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE)))
	{
		curData->parentEnemy->SetState(EnemyState::SHOOT);
		curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.shootInstance);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(3.f, 5.f);
		curData->parentEnemy->shootTime = distr(gen);
		return;
	}
}


void ShootNow::Execute(EnemyStatesData* curData, int index)
{

				if (curData->parentEnemy->GetSuspicionLevel() == SuspicionLevel::TAKECOVERANDSHOOT)
				{
					if (curData->parentEnemy->shootTime > 0.f)
					{
						curData->parentEnemy->shootTime -= 1.0 / 60.0;
						if (Distance(curData->parentEnemy->closestPosToShootNearCover, curData->parentEnemy->GetPosition()) > 0.5f)
						{
							curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->closestPosToShootNearCover);
							curData->mainmenuptr->MoveEnemy(index);
						}
	
						else
						{
							curData->mainmenuptr->SetEnemyDirection(index, curData->mainmenuptr->_lastSeenPositionOfPlayer);
							if (curData->parentEnemy->_items.size() > 0 && curData->parentEnemy->_items[0]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE))
							{
								if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
								{
									curData->mainmenuptr->_lastSeenPositionOfPlayer = curData->parentEnemy->playerInstance->GetPosition();
	
									if (Distance(curData->parentEnemy->GetPosition(), curData->parentEnemy->playerInstance->GetPosition()) < 250.f)
									{
										if (curData->parentEnemy->_items.size() > 0 && curData->parentEnemy->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
										{
											curData->mainmenuptr->PlayerAttack(index);
										}
	
										else
										{
											curData->mainmenuptr->ThrowItem(index);
											curData->parentEnemy->SetState(EnemyState::TAKECOVER);
											curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.takeCover);
											curData->parentEnemy->coverTime = 30.f;
										}
									}
	
									else
									{
										if (!curData->mainmenuptr->CheckEnemyToPointIntersection(curData->mainmenuptr->_lastSeenPositionOfPlayer, curData->parentEnemy->GetPosition()))
										{
											curData->parentEnemy->SetState(EnemyState::GETINRANGEOFPLAYER);
											curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.getInRangeOfPlayerInstance);
											return;
										}
	
										else
										{
											curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), curData->mainmenuptr->_lastSeenPositionOfPlayer));
											curData->parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
											curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);
											return;
										}
									}
	
								}
	
	
								else
								{
									if (!curData->mainmenuptr->CheckEnemyToPointIntersection(curData->mainmenuptr->_lastSeenPositionOfPlayer, curData->parentEnemy->GetPosition()))
									{
										curData->parentEnemy->travelToPosition = curData->mainmenuptr->_lastSeenPositionOfPlayer;
										curData->parentEnemy->SetState(EnemyState::TRAVELTOPOSITION);
										curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.travelToPositionInstance);
										return;
									}
	
									else
									{
										curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), curData->mainmenuptr->_lastSeenPositionOfPlayer));
										curData->parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
										curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);
										return;
									}
								}
							}
	
							else
							{
								curData->mainmenuptr->DecideToTakeCover(index);
								return;
							}
						}
						
					}
	
					else
					{
						curData->parentEnemy->SetState(EnemyState::TAKECOVER);
						curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.takeCover);
						std::random_device rd;
						std::mt19937 gen(rd());
						std::uniform_int_distribution<> distr(5.f, 7.f);
						curData->parentEnemy->coverTime = distr(gen);
						return;
					}
	
					return;
				}
	
				else if (curData->parentEnemy->GetSuspicionLevel() == SuspicionLevel::DIRECTSHOOT)
				{
					if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
					{
						curData->mainmenuptr->_lastSeenPositionOfPlayer = curData->parentEnemy->playerInstance->GetPosition();
						curData->mainmenuptr->SetEnemyDirection(index, curData->mainmenuptr->_lastSeenPositionOfPlayer);
						if (Distance(curData->parentEnemy->GetPosition(), curData->parentEnemy->playerInstance->GetPosition()) < 250.f)
						{
							if (curData->parentEnemy->_items.size() > 0 && curData->parentEnemy->_items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
							{
								curData->mainmenuptr->PlayerAttack(index);
							}
						}
	
						else
						{
							curData->parentEnemy->SetState(EnemyState::GETINRANGEOFPLAYER);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.getInRangeOfPlayerInstance);
						}
					}
	
					else
					{
						if (!curData->mainmenuptr->CheckEnemyToPointIntersection(curData->mainmenuptr->_lastSeenPositionOfPlayer, curData->parentEnemy->GetPosition()))
						{
							curData->parentEnemy->travelToPosition = curData->mainmenuptr->_lastSeenPositionOfPlayer;
							curData->parentEnemy->SetState(EnemyState::TRAVELTOPOSITION);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.travelToPositionInstance);
							return;
						}
	
						else
						{
							curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), curData->mainmenuptr->_lastSeenPositionOfPlayer));
							curData->parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);
							return;
						}
					}
					return;
				}
}


void FindWeaponNearBy::Execute(EnemyStatesData* curData, int index)
{
	if (!curData->parentEnemy->hasBeenAssignedPath)
					{
						std::pair<int, Item*> meleeItem = curData->mainmenuptr->FindNearestWeapon(index);
						if (meleeItem.first != -1)
						{
							curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), meleeItem.second->GetPosition()));
							curData->parentEnemy->hasBeenAssignedPath = true;
							return;
						}
		
						else
						{
							curData->parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);
							return;
						}
			
					}
		
					if (Distance(curData->parentEnemy->_shortestPath[0], curData->parentEnemy->GetPosition()) < 10.f)
					{
						{
							curData->mainmenuptr->PickItem(index);
							curData->parentEnemy->hasBeenAssignedPath = false;
							curData->parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);
							return;
						}
					}
		
					if (Distance(curData->parentEnemy->GetCurrentPointInShrstPath(), curData->parentEnemy->GetPosition()) > 0.5f)
					{
						curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->GetCurrentPointInShrstPath());
						curData->mainmenuptr->MoveEnemy(index);
					}
		
					else
					{
						curData->parentEnemy->SetNextPointInShrtstPath();
					}
		
		
}



void GetInRangeOfPlayer::Execute(EnemyStatesData* curData, int index)
{
	if (Distance(curData->parentEnemy->GetPosition(), curData->parentEnemy->playerInstance->GetPosition()) > 250.f)
					{
		curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->playerInstance->GetPosition());
		curData->mainmenuptr->MoveEnemy(index);
					}
		
					else if (Distance(curData->parentEnemy->GetPosition(), curData->parentEnemy->playerInstance->GetPosition()) < 250.f)
					{
		curData->mainmenuptr->DecideToTakeCover(index);
		return;
					}
					
	return;
}

void TravelToPosition::Execute(EnemyStatesData* curData, int index)
{
	if (Distance(curData->parentEnemy->travelToPosition, curData->parentEnemy->GetPosition()) > 1.f)
				{
		curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->travelToPosition);
		curData->mainmenuptr->MoveEnemy(index);
					if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
					{
						curData->mainmenuptr->DecideToTakeCover(index);
					}
					return;
				}
		
				else
				{
					if (curData->parentEnemy->GetSuspicionLevel() == SuspicionLevel::TAKECOVERANDSHOOT)
					{
						curData->parentEnemy->_dynamicWaypt = PathFinder::Findpointnearby(curData->parentEnemy->GetPosition());
						curData->parentEnemy->SetState(EnemyState::LOOKFORPLAYER);
						curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.lookForPlayerInstance);
					}
		
					else if (curData->parentEnemy->GetSuspicionLevel() == SuspicionLevel::DIRECTSHOOT)
					{
						curData->parentEnemy->SetState(EnemyState::SHOOT);
						curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.shootInstance);
					}
					return;
				}
				return;
}

void LookForPlayer::Execute(EnemyStatesData* curData, int index)
{
	if (Distance(curData->parentEnemy->_dynamicWaypt, curData->parentEnemy->GetPosition()) > 0.5f)
				{
		curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->_dynamicWaypt);
		curData->mainmenuptr->MoveEnemy(index);
					if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index)) curData->mainmenuptr->DecideToTakeCover(index);
					return;
				}
				else
				{
				/*	int r = rand();
					if (r % 2 == 0)
					{
						int g = rand();
						if (g % 2 == 0) _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x + 60, _enemies[i]->GetPosition().y);
						else _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x - 60, _enemies[i]->GetPosition().y);
					}
					else
					{
						int g = rand();
						if (g % 2 == 0) _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x, _enemies[i]->GetPosition().y + 40);
						else _enemies[i]->_dynamicWaypt = sf::Vector2f(_enemies[i]->GetPosition().x, _enemies[i]->GetPosition().y - 40);
					}
					FindDynamicWayPtForEnemy(i, &_enemies[i]->_dynamicWaypt);*/
		curData->parentEnemy->_dynamicWaypt = PathFinder::Findpointnearby(curData->parentEnemy->GetPosition());
		curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->_dynamicWaypt);
					return;
				}
}

void Chase::Execute(EnemyStatesData* curData, int index)
{
	if (Distance(curData->parentEnemy->GetPosition(), curData->parentEnemy->playerInstance->getPostion()) > 100.f)
				{
					if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
					{
						curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->playerInstance->getPostion());
						curData->mainmenuptr->MoveEnemy(index);
						curData->mainmenuptr->_lastSeenPositionOfPlayer = curData->parentEnemy->playerInstance->getPostion();
						return;
					}
					else
					{
						curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), curData->mainmenuptr->_lastSeenPositionOfPlayer));
						curData->parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
						curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);
						return;
					}
				}
				else
				{
		curData->parentEnemy->SetState(EnemyState::ATTACK);
		curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.attackInstance);
					return;
				}
}

void Attack::Execute(EnemyStatesData* curData, int index)
{
	if (Distance(curData->parentEnemy->GetPosition(), curData->parentEnemy->playerInstance->getPostion()) < 100.f)
					{
						if (curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
						{
							curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->playerInstance->getPostion());
							if (curData->parentEnemy->_items.size() > 0)curData->mainmenuptr->ThrowItem(index);
							curData->mainmenuptr->MoveEnemy(index);
							curData->mainmenuptr->_lastSeenPositionOfPlayer = curData->parentEnemy->playerInstance->getPostion();
							return;
						}
						else if (Distance(curData->parentEnemy->GetPosition(), curData->parentEnemy->playerInstance->getPostion()) > 80.f && !curData->mainmenuptr->CheckEnemiesToPlayerLine(index))
						{
							curData->parentEnemy->SetShortestPath(PathFinder::CheckPath(curData->parentEnemy->GetPosition(), curData->mainmenuptr->_lastSeenPositionOfPlayer));
							curData->parentEnemy->SetState(EnemyState::MAKEPATHTOPLAYERLASTPOS);
							curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.makePathToEnemyInstance);
							return;
						}
		
						else
						{
							curData->parentEnemy->SetDirection(sf::Vector2f(NULL, NULL));
						}
					}
		
					else
					{
		curData->parentEnemy->SetState(EnemyState::CHASE);
		curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.chaseInstance);
						return;
					}
}

void GonnaDieSoon::Execute(EnemyStatesData* curData, int index)
{
	if (!curData->parentEnemy->sentMessageForHelp)
			{
				Message helpMessage(-1, curData->parentEnemy->GetID(), MessageType::SAVEFRIENDSASS, 0.f, 500.f, curData->parentEnemy->GetPosition());
				curData->messageDispatcher->AddDispatchMessage(helpMessage, curData->_dataref->_Globalclock.getElapsedTime().asSeconds());
				curData->parentEnemy->SetEnemyColor(sf::Color::Blue);
				curData->parentEnemy->sentMessageForHelp = true;
			}

			if (curData->parentEnemy->timeToDie > 0.f)
			{
				curData->parentEnemy->SetPosition(curData->parentEnemy->GetPosition());
				curData->parentEnemy->timeToDie -= 0.03f;
				return;
			}

			else
			{
				curData->parentEnemy->KillEnemy();
				return;
			}
}


void GoSaveFriend::Execute(EnemyStatesData* curData, int index)
{
	if (!curData->parentEnemy->hasBeenAssignedPath)
					{
		curData->mainmenuptr->EnemyToReviveCalc(index);
					}
		
					if (Distance(curData->parentEnemy->_shortestPath[0], curData->parentEnemy->GetPosition()) < 1.f)
					{
						curData->parentEnemy->SetState(EnemyState::REVIVEFRIENDNOW);
						curData->parentEnemy->enemymachine->ChangeState(&curData->parentEnemy->stateObjects.reviveFriendNowInstance);
						curData->parentEnemy->hasBeenAssignedPath = false;
						return;
					}
		
				if (Distance(curData->parentEnemy->GetCurrentPointInShrstPath(), curData->parentEnemy->GetPosition()) > 0.5f)
					{
					curData->mainmenuptr->SetEnemyDirection(index, curData->parentEnemy->GetCurrentPointInShrstPath());
					curData->mainmenuptr->MoveEnemy(index);
					}
		
					else
					{
					curData->parentEnemy->SetNextPointInShrtstPath();
					}
		
					return;
}


void ReviveFriendNow::Execute(EnemyStatesData* curData, int index)
{
	if (curData->parentEnemy->TimeToRevive > 0.f)
				{
		curData->parentEnemy->TimeToRevive -= 0.09f;
				}
		
				else
				{
					Message SavedFromDying(curData->parentEnemy->EnemyToReviveId, curData->parentEnemy->GetID(), MessageType::REVIVED, 0.f, 0.f);
					curData->messageDispatcher->AddDispatchMessage(SavedFromDying, curData->_dataref->_Globalclock.getElapsedTime().asSeconds());
					curData->parentEnemy->EnemyToReviveId = -1000;
					curData->parentEnemy->TimeToRevive = 10.f;
					curData->parentEnemy->hasBeenAssignedPath = false;
					curData->mainmenuptr->DecideToTakeCover(index);
					return;
				}
}