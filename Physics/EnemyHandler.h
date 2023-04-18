#pragma once
#include"Message.h"

class Enemy;

class EnemyHandler
{
public:
	EnemyHandler();
	~EnemyHandler();
	Enemy* parent;
	void GoToToilet(const Message &msg);
	void GoForHelp(const Message &msg);
	void DieFromGrenade(const Message &msg);
	void SaveFriendsAss(const Message &msg);
	void Revived(const Message &msg);
	void RunAwayFromGrenade(const Message &msg);
};