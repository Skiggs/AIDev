#pragma once

#include"EnemyState.h"


class Enemy;

class EnemyStateMachine
{
public:
	EnemyStateMachine() {};
	~EnemyStateMachine() {};
    void ChangeState(EnemyStates* newState);
	void ProcessState(int i);
	void SetParent(Enemy* parent) { enemyStateHolder = parent; }
private:
	Enemy* enemyStateHolder;
};