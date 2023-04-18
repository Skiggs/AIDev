#pragma once

#include"Message.h"


class Item;

class ItemHandler
{
public:
	ItemHandler();
	~ItemHandler();
	Item* parent;
	void DieFromGrenade(const Message &msg);
};