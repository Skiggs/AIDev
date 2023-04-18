#include"EnemyHandler.h"
#include"Enemy.h"
#include"EnemyStateMachine.h"


EnemyHandler::EnemyHandler()
{

}

EnemyHandler::~EnemyHandler()
{

}

void EnemyHandler::GoToToilet(const Message &msg)
{
	parent->SetState(EnemyState::FOLLOWPATHTOTOILET);
	parent->enemymachine->ChangeState(&parent->stateObjects.findweaponnearByInstance);
}


void EnemyHandler::GoForHelp(const Message &msg)
{
	if (parent->_items.size()<=0 ||(parent->_items.size() > 0 && !parent->_items[0]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE)))
	{
		parent->SetSuspicionLevel(SuspicionLevel::FINDWEAPONTOKILLENEMY);
		parent->SetState(EnemyState::FINDWEAPONNEARBY);
		parent->enemymachine->ChangeState(&parent->stateObjects.findweaponnearByInstance);
	}

	else if (parent->_items.size() > 0 && parent->_items[0]->CheckTrait(ItemTrait::PHYSICALLYDAMAGEABLE))
	{
		parent->SetState(EnemyState::DECIDEHOWTOHELP);
		parent->enemymachine->ChangeState(&parent->stateObjects.decideHowToHelpInstance);
	}
}

void EnemyHandler::DieFromGrenade(const Message &msg)
{
	parent->SetHealth(0);
	parent->SetState(EnemyState::GONNADIESOON);
	parent->enemymachine->ChangeState(&parent->stateObjects.gonnaDieSoonInstance);
}

void EnemyHandler::SaveFriendsAss(const Message &msg)
{
	//if (parent->GetHealth() > 1)
	{
		parent->EnemyToReviveId = msg.sender;
		parent->hasBeenAssignedPath = false;
		parent->SetState(EnemyState::GOANDSAVEFRIENDSASS);
		parent->enemymachine->ChangeState(&parent->stateObjects.goSaveFriendInstance);
	}
}

void EnemyHandler::Revived(const Message &msg)
{
	parent->SetHealth(2);
	parent->timeToDie = 30.f;
	parent->SetEnemyColor(sf::Color::Red);
	parent->sentMessageForHelp = false;
	parent->SetState(EnemyState::DECIDEHOWTOHELP);
	parent->enemymachine->ChangeState(&parent->stateObjects.decideHowToHelpInstance);
}

void EnemyHandler::RunAwayFromGrenade(const Message &msg)
{
	parent->SetState(EnemyState::RUNAWAY);
	parent->enemymachine->ChangeState(&parent->stateObjects.runawayInstance);
}