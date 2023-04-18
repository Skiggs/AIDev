#include"MessageDispatcher.h"
#include<cassert>
#include<iostream>

MessageDispatcher::~MessageDispatcher()
{

}

void MessageDispatcher::Init(HandlerManager &hM)
{
	handlerMngr = hM;
}

void MessageDispatcher::AddDispatchMessage(Message msg, float currentTime)
{
	if (msg.receiver != -1)
	{
		auto map = handlerMngr.containerForAllMessageType[msg.msg];
		auto it = map.find(msg.receiver);
		assert(it != map.end());
		if (it != map.end())
		{
			MessageQueue.insert({ currentTime + msg.time, msg });
		}
	}
	else
	{
		MessageQueue.insert({ currentTime + msg.time, msg });
	}
}

void MessageDispatcher::GetPotentialDispatchMsg(float elapsedTime, Message &msg)
{
	if (!MessageQueue.empty())
	{
		if (MessageQueue.begin()->first <= elapsedTime)
		{
			Message curMsg = MessageQueue.begin()->second;
			msg = curMsg;
			MessageQueue.erase(MessageQueue.begin());
		}
	}
}
