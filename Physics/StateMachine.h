#pragma once

#include"State.h"
#include<memory>
#include<stack>
#include <stdlib.h>

typedef std::unique_ptr<State> StateRef;

class StateMachine
{
public:
	void AddState(StateRef newState, bool isReplacing = true);
	void RemoveState();
	void ProcessState();

	StateRef& GetActiveState();

private:
	std::stack<StateRef> _states;
	StateRef _newState;


	bool _isRemoving;
	bool _isAdding;
	bool _isReplacing;
};