#include"EnemyStateMachine.h"
#include"Enemy.h"

void EnemyStateMachine::ProcessState(int i)
{
	enemyStateHolder->curState->Execute(enemyStateHolder->curEnemyData,i);
}

void EnemyStateMachine::ChangeState(EnemyStates* newState)
{
	enemyStateHolder->curState =  newState;
}