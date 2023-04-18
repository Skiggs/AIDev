#pragma once
#include"Game.h"

class MainMenuState;
class MessageDispatcher;
class Enemy;

struct EnemyStatesData
{
	Enemy* parentEnemy;
	GameDataRef _dataref;
	MessageDispatcher* messageDispatcher;
	MainMenuState* mainmenuptr;
};

class EnemyStates
{
public:
	virtual void Execute(EnemyStatesData* curData,int index) = 0;
};



class Patrol : public EnemyStates
{
public:
	Patrol() { };
	~Patrol() {};
	void Execute(EnemyStatesData* curData,int index);
};

class GoToToilet : public EnemyStates
{
public:
	GoToToilet() {};
	~GoToToilet() {};
	void Execute(EnemyStatesData* curData, int index);
};

class CallForHelp : public EnemyStates
{
public:
	CallForHelp() { };
	~CallForHelp() {};
	void Execute(EnemyStatesData* curData, int index);
};

class FindWeaponNearBy : public EnemyStates
{
public:
	FindWeaponNearBy() {};
	~FindWeaponNearBy() {};
	void Execute(EnemyStatesData* curData, int index);
};

class DecideHowToHelp : public EnemyStates
{
public:
	DecideHowToHelp() {};
	~DecideHowToHelp() {};
	void Execute(EnemyStatesData* curData, int index);
};

class TakeCover : public EnemyStates
{
public:
	TakeCover() {};
	~TakeCover() {};
	void Execute(EnemyStatesData* curData, int index);
};

class ShootNow : public EnemyStates
{
public:
	ShootNow() {};
	~ShootNow() {};
	void Execute(EnemyStatesData* curData, int index);
};

class GetInRangeOfPlayer : public EnemyStates
{
public:
	GetInRangeOfPlayer() {};
	~GetInRangeOfPlayer() {};
	void Execute(EnemyStatesData* curData, int index);
};


class MakePathToEnemy : public EnemyStates
{
public:
	MakePathToEnemy() {};
	~MakePathToEnemy() {};
	void Execute(EnemyStatesData* curData,int index);
};

class Runaway : public EnemyStates
{
public:
	Runaway() {};
	~Runaway() {};
	void Execute(EnemyStatesData* curData, int index);
};

class TravelToPosition : public EnemyStates
{
public:
	TravelToPosition() {};
	~TravelToPosition() {};
	void Execute(EnemyStatesData* curData, int index);
};

class Chase : public EnemyStates
{
public:
	Chase() {};
	~Chase() {};
	void Execute(EnemyStatesData* curData, int index);
};

class Attack : public EnemyStates
{
public:
	Attack() {};
	~Attack() {};
	void Execute(EnemyStatesData* curData, int index);
};


class LookForPlayer : public EnemyStates
{
public:
	LookForPlayer() {};
	~LookForPlayer() {};
	void Execute(EnemyStatesData* curData, int index);
};

class GonnaDieSoon : public EnemyStates
{
public:
	GonnaDieSoon() {};
	~GonnaDieSoon() {};
	void Execute(EnemyStatesData* curData, int index);
};

class GoSaveFriend : public EnemyStates
{
public:
	GoSaveFriend() {};
	~GoSaveFriend() {};
	void Execute(EnemyStatesData* curData, int index);
};

class ReviveFriendNow : public EnemyStates
{
public:
	ReviveFriendNow() {};
	~ReviveFriendNow() {};
	void Execute(EnemyStatesData* curData, int index);
};