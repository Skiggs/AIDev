#include"Enemy.h"
#include"MathFunc.h"
#include"EnemyHandler.h"
#include"MessageDispatcher.h"
#include"PathFinder.h"
//#include"EnemyState.h"
#include"EnemyStateMachine.h"
#include<iostream>
#include"MainMenu.h"



Enemy::Enemy(GameDataRef data, Player& plyr,MessageDispatcher* msgdisptch,MainMenuState *mainMenu) :_data(data), _line(), iniVelo(sf::Vector2f(0.0, 0.0)),playerInstance(&plyr), finalVelo(sf::Vector2f(0.0, 0.0)), health(3), stunTimer(0), closestDoor(NULL) {
	_line.p1 = sf::Vector2f(0, 0);
	_enemySuspicionLevel = SuspicionLevel::RELAXED;
	_line.p2 = sf::Vector2f(10, 10);
	//stateObjects.patrolInstance = new Patrol();
	/*stateObjects.attackInstance = new Attack();
	stateObjects.callforhelpInstance = new CallForHelp();
	stateObjects.chaseInstance = new Chase();
	stateObjects.decideHowToHelpInstance = new DecideHowToHelp();
	stateObjects.findweaponnearByInstance = new FindWeaponNearBy();
	stateObjects.getInRangeOfPlayerInstance = new GetInRangeOfPlayer();
	stateObjects.gonnaDieSoonInstance = new GonnaDieSoon();
	stateObjects.goSaveFriendInstance = new GoSaveFriend();
	stateObjects.lookForPlayerInstance = new LookForPlayer();
	stateObjects.makePathToEnemyInstance = new MakePathToEnemy();
	stateObjects.reviveFriendNowInstance = new ReviveFriendNow();
	stateObjects.runawayInstance = new Runaway();
	stateObjects.shootInstance = new ShootNow();
	stateObjects.takeCover = new TakeCover();
	stateObjects.travelToPositionInstance = new TravelToPosition();*/


	SetHealthBarSize();
	healthrect.setOrigin(sf::Vector2f(healthrect.getSize().x / 2, 2.5f));
	healthrect.setFillColor(sf::Color::Red);
	rect.setOrigin(sf::Vector2f(20, 20));
	rect.setRadius(20);
	curEnemyData = new EnemyStatesData();
	curEnemyData->parentEnemy = this;
	curEnemyData->_dataref = data;
	curEnemyData->messageDispatcher = msgdisptch;
	curEnemyData->mainmenuptr = mainMenu;
	//curState = new Patrol();
	curState = &stateObjects.patrolInstance;
	rect.setFillColor(sf::Color::Red);
	currentTime = this->clock_.getElapsedTime().asSeconds();
	enemyHandler = new EnemyHandler();
	enemyHandler->parent = this;
	enemymachine = new EnemyStateMachine();
	enemymachine->SetParent(this);
};

void Enemy::Update(float dt)
{
	if (acceleration.x != 0 || acceleration.y != 0)
	{
		newTime = currentTime + dt;
		finalVelo = iniVelo + (acceleration * newTime);
		newPos = curPos + (finalVelo * (newTime/2));
		this->SetPosition(newPos);
		_line.p2 = newPos + (acceleration * (float)50.0);
		currentTime = newTime;
		iniVelo = finalVelo;
		curPos = this->GetPosition();
	}
}

//double Enemy::GetDirectionVectorAngle()
//{
//	sf::Vector2f direction = _line.p2 - _line.p1;
//	sf::Vector2f direction2 = _traversePos.top() - _line.p1;
//	double angle = AngleBWVectors(direction, direction2);
//	return angle;
//}

void Enemy::UpdateGunTargetLine()
{
	if (_items.size() > 0 && _items[0]->CheckTrait(ItemTrait::HOLDANDSHOOT))
	{
		_items[0]->SetPosition(GetPosition());
		_items[0]->_targetLine.p1 = _line.p1;
		sf::Vector2f playerToTargetPt = UnitVector(_line.p2 - _items[0]->_targetLine.p1);
		sf::Vector2f newPoint;
		newPoint = _line.p1 + playerToTargetPt * 300.f;
		_items[0]->_targetLine.p2 = newPoint;
		_items[0]->_targetLine.p3 = newPoint;
		_items[0]->_targetLine.p1 = _line.p1;
	}
}

void Enemy::Reducehealth()
{
	if(health>0) health--;
	else
	{
		SetState(EnemyState::GONNADIESOON);
		enemymachine->ChangeState(&stateObjects.gonnaDieSoonInstance);
	}
	SetHealthBarSize();
}


int Enemy::GetHealth()
{
  return health;
}

void Enemy::ReduceStunTimer()
{
	if (stunTimer > 0.f) stunTimer -= 0.2f;
}

void Enemy::SetHealthBarSize()
{
	healthrect.setSize(sf::Vector2f(health*30.f, 8.f));
}


void Enemy::AddPath(std::vector<sf::Vector2f> routes)
{
	_path = routes;
}

sf::Vector2f Enemy::GetCurrentWayPt()
{
	return _path[currentWaypt];
}

void Enemy::SetNextWayPoint()
{
	if (currentWaypt < _path.size()-1) currentWaypt++;
	else currentWaypt = 0;
}

void Enemy::Move()
{
	sf::Vector2f nxtPos = GetPosition() + GetDirection();
	SetPosition(nxtPos);
	_line.p2 = GetPosition() + (GetDirection() * (float)50.0);
	UpdateGunTargetLine();
}

bool Enemy::CheckLineToPlayer()
{
	bool enemy2player = PathFinder::CheckIfLineBetweenColliding(GetPosition(),playerInstance->GetPosition());
	if (!enemy2player)
	{
		sf::Vector2f vectorToPlayer = playerInstance->GetPosition() - GetPosition();
		sf::Vector2f enemyForward = _line.p2 - _line.p1;
		float ang = AngleBWVectors(vectorToPlayer, enemyForward);
		ang = ang * (180 / 3.14);
		float absAngle = std::abs(ang);
		if (absAngle < 100.0f)
		{
			lastKnownPositionOfPlayer = playerInstance->GetPosition();
			return true;
		}

		else return false;
	}

	return false;
}

void Enemy::Shoot(sf::Vector2f pos)
{
	if (nextHitTime <= 0)
	{
		nextHitTime = 1.5f;

		float distToEnemy = CheckDistance(playerInstance->GetShape(), _items[0]->_targetLine.p1, _items[0]->_targetLine.p2);
		if (distToEnemy < 20)
		{
			playerInstance->ReduceHealthOfPlayer();
		}

	}
}

void Enemy::SetDirection(sf::Vector2f position)
{
	sf::Vector2f enemy2wayPt = UnitVector(position - _line.p1);
	_line.p2 = Line2Mouse(_line.p1, enemy2wayPt);
	sf::Vector2f unitVectorDir = UnitVector(_line.p2 - _line.p1);
	SetToDirection(unitVectorDir);
	UpdateGunTargetLine();
}

void Enemy::SetNextDynamicWayPt()
{
	if (currentDynWayPt == 0) currentDynWayPt++;
	else currentDynWayPt = 0;
}

void Enemy::SetShortestPath(std::vector<sf::Vector2f> shrtstPath)
{
	_shortestPath = shrtstPath;
	currentShrtstPathPt = _shortestPath.size() - 1;
}

sf::Vector2f Enemy::GetCurrentPointInShrstPath()
{
	return _shortestPath[currentShrtstPathPt];
}

void Enemy::SetNextPointInShrtstPath()
{
	if (currentShrtstPathPt > 0) currentShrtstPathPt--;
}

sf::Vector2f Enemy::GetPoint(int i)
{
	return rect.getTransform().transformPoint(rect.getPoint(i));
}

