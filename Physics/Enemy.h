#pragma once

#include<SFML/Graphics.hpp>
#include "Game.h"
#include<stack>
#include"Wall.h"
#include"Node.h"
#include"Item.h"
#include"EnemyState.h"
#include"Player.h"


struct Line
{
	sf::Vector2f p1, p2;
};


enum class EnemyState
{
	PATROL,
	CHASE,
	ATTACK,
	DIE,
	SHOOT,
	STUNNED,
	FOLLOWPATHTOTOILET,
	FINDWEAPONNEARBY,
	MAKEPATHTOPLAYERLASTPOS,
	GONNADIESOON,
	RUNAWAY,
	HOLDONTOGETREVIVED,
	REVIVEFRIENDNOW,
	TAKECOVER,
	GOANDSAVEFRIENDSASS,
	GETINRANGEOFPLAYER,
	CALLFORHELP,
	DECIDEHOWTOHELP,
	TRAVELTOPOSITION,
	LOOKFORPLAYER,
	MAKEPATHTOPLAYER
};

enum class SuspicionLevel
{
	RELAXED,
	DOUBTFUL,
	SEENSOMETHING,
	TAKECOVERANDSHOOT,
	FINDWEAPONTOKILLENEMY,
	FINDLOSTENEMY,
	DIRECTSHOOT,
	SAWPLAYER,
	GOTSHOTANDSUPERANGRY
};

class EnemyStateMachine;
//class EnemyStates;
class EnemyHandler;
class PathFinder;
class MessageDispatcher;
class MainMenuState;
class EnemyStatesData;
//class Patrol;
//class CallForHelp;
//class MakePathToEnemy;
//class DecideHowToHelp;
//class Runaway;
//class TakeCover;
//class ShootNow;
//class FindWeaponNearBy;
//class TravelToPosition;
//class LookForPlayer;
//class Chase;
//class Attack;
//class GonnaDieSoon;
//class GoSaveFriend;
//class ReviveFriendNow;
//class GetInRangeOfPlayer;

class EnemyStateObjects
{
public:
	Patrol patrolInstance;
	CallForHelp callforhelpInstance;
	MakePathToEnemy makePathToEnemyInstance;
	DecideHowToHelp decideHowToHelpInstance;
	Runaway runawayInstance;
	TakeCover takeCover;
	ShootNow shootInstance;
	FindWeaponNearBy findweaponnearByInstance;
	GetInRangeOfPlayer getInRangeOfPlayerInstance;
	TravelToPosition travelToPositionInstance;
	LookForPlayer lookForPlayerInstance;
	Chase chaseInstance;
	Attack attackInstance;
	GonnaDieSoon gonnaDieSoonInstance;
	GoSaveFriend goSaveFriendInstance;
	ReviveFriendNow reviveFriendNowInstance;
};

class Enemy
{
public:
	Enemy(GameDataRef data, Player&,MessageDispatcher* msgDisptch,MainMenuState* mainMenu);
	~Enemy() {};
	EnemyHandler *enemyHandler;
	int EnemyToReviveId = -1000;
	sf::Vector2f lastKnownPositionOfPlayer;
	Wall* coverWall = nullptr;
	float nextHitTime = 1.5f;
	float coverTime = 0.f;
	float shootTime = 0.f;
	EnemyStates* curState;
	EnemyStatesData* curEnemyData;
	Player* playerInstance;
	EnemyStateMachine* enemymachine;
	sf::Vector2f lastCoverPosition, travelToPosition;
	sf::Vector2f closestPosToShootNearCover;
	sf::Vector2f  closestPointToDoorFromEnemy, iniVelo, finalVelo, acceleration, curPos, newPos, suspiciousPos, _dynamicWaypt, _dynamicWaypt2;
	Door* closestDoor;
	bool runAwayFromGrenade = false;
	sf::RectangleShape healthrect;
	bool isRemovedfromMainList = false;
	float _coolDownTime = 30.f;
	float TimeToRevive = 10.f, timeToDie = 30.f;
	float _rotateRad = 0;
	int currentDynWayPt = 0;
	std::vector<sf::Vector2f> _shortestPath;
	int currentShrtstPathPt = 0;
	bool _hasReachedSuspPos = false;
	bool _hasReachedPlyrLastPos = false;
	Line _line;
	Item *nearByItem = nullptr;
	void SetPlayer(Player* plyr) { playerInstance = plyr; }
	std::vector<Item*> _items;
	sf::CircleShape GetShape() { return rect; }
	void SetPosition(sf::Vector2f pos) { 
		rect.setPosition(pos); _line.p1 = rect.getPosition(); 
		curPos = rect.getPosition(); 
		healthrect.setPosition(sf::Vector2f(pos.x, pos.y - 30.f));
	}
	void Rotate(float angle) { rect.setRotation(angle); }
	void SetState(EnemyState state) { _enemyState = state; }
	void Update(float dt);
	sf::Vector2f GetPosition() { return rect.getPosition(); }
	sf::FloatRect getSize() { return rect.getGlobalBounds(); }
	void ReduceStunTimer();
	void AddPath(std::vector<sf::Vector2f> routes);
	void EnemyActionUpdate();
	void SetID(int id) { m_id = id; }
	void SetHealth(int value) { health = value; SetHealthBarSize();	}
	void SetEnemyColor(sf::Color col) { rect.setFillColor(col); }
	int GetID() const { return m_id; }
	void TargetPosition(sf::Vector2f tarPos);
	void SetToDirection(sf::Vector2f dir) { acceleration = dir; }
	sf::Vector2f GetDirection() { return acceleration; }
	sf::Vector2f GetCurrentWayPt();
	float GetStunTimer() { return stunTimer; }
	void SetNextWayPoint();
	void SetNextDynamicWayPt();
	int GetHealth();
	void Reducehealth();
	void SetHealthBarSize();
	bool hasBeenAssignedPath = false;
	sf::Vector2f GetPoint(int i);
	void KillEnemy() { _isDead = true; rect.setFillColor(sf::Color::Magenta); }
	void SetSuspicionLevel(SuspicionLevel sL) { _enemySuspicionLevel = sL; }
	SuspicionLevel GetSuspicionLevel() { return _enemySuspicionLevel; }
	bool IsEnemyDead() { return _isDead; }
	void SetStunTimerOn() { stunTimer = 3.0; }
	EnemyState GetState() { return _enemyState; }
	void SetShortestPath(std::vector<sf::Vector2f>);
	void SetName(std::string str) { m_name = str; }
	std::string GetName() { return m_name;}
	void SetNextPointInShrtstPath();
	void UpdateGunTargetLine();
	sf::Vector2f GetCurrentPointInShrstPath();
	void Move();
	bool CheckLineToPlayer();
	void SetDirection(sf::Vector2f position);
	void Shoot(sf::Vector2f pos);
	bool sentMessageForHelp = false;
	void SetpathFinder(PathFinder* pthFinder) { pathFinderInstance = pthFinder; }
	void AddItem()
	{
		if (nearByItem->CheckTrait(ItemTrait::PICKABLE))
		{
			_items.push_back(nearByItem);
		}
	}
	void Draw() {
		_data->window.draw(rect);
		_data->window.draw(healthrect);
		sf::Vertex line[] =
		{
			sf::Vertex(_line.p1),
			sf::Vertex(_line.p2)
		};
		_data->window.draw(line, 2, sf::Lines);

		if (_items.size() > 0 && _items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
		{
			_items[0]->Draw();
		}
	}
	EnemyStateObjects stateObjects;

private:
	EnemyState _enemyState;
	
	SuspicionLevel _enemySuspicionLevel;
	GameDataRef _data;
	PathFinder* pathFinderInstance;
	
	int m_id;
	std::string m_name;
	sf::Vector2f _targetPos;
	int health = 3;
	float stunTimer;
	std::vector<sf::Vector2f> _path;
	int currentWaypt;
	sf::Clock clock_;
	float currentTime, newTime;
	sf::CircleShape rect;
	bool _isDead = false;
};