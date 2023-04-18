#pragma once
#include<map>
#include"Message.h"
#include"HandlerManager.h"



class MessageDispatcher
{
public:
	MessageDispatcher() {}
	void Init(HandlerManager &hm);
	~MessageDispatcher();
	std::multimap<float, Message> MessageQueue;
	void AddDispatchMessage(Message,float currentTime);
	void GetPotentialDispatchMsg(float elapsedtime, Message &msg);
private:
	HandlerManager handlerMngr;
};