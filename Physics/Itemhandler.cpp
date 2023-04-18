#include"Item.h"
#include"ItemHandler.h"


ItemHandler::ItemHandler()
{

}

ItemHandler::~ItemHandler()
{

}

void ItemHandler::DieFromGrenade(const Message &msg)
{
	parent->doesntExist = true;
}