#pragma once
#include<SFML/Graphics.hpp>

enum MessageType
{
	DODGE,
	DEFEND,
	COUNTER,
	FOLLOW,
	WATCHOUT,
	GOTOTOILET,
	GOFORHELP,
	GETTINGREVIVED,
	RUNAWAYFROMGRENADE,
	REVIVED,
	DIEFROMGRENADE,
	SAVEMYASS,
	SAVEFRIENDSASS,
	MT_COUNT
};

 struct Message
{
	Message(int rec, int send, MessageType messg, float time, float range,sf::Vector2f senderPos = sf::Vector2f(NULL,NULL)) : receiver(rec),sender(send),msg(messg),time(time),range(range),SenderPosition(senderPos){}
	int receiver;
	int sender;
	static const MessageType a = MessageType::DEFEND;
	MessageType msg;
	float time;
	float range;
	sf::Vector2f SenderPosition;
};


