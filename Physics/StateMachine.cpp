#include"StateMachine.h"

void StateMachine::AddState(StateRef newState, bool isReplacing)
{
	this->_isAdding = true;
	this->_isReplacing = isReplacing;
	this->_newState = std::move(newState);

}

void StateMachine::RemoveState()
{
	this->_isRemoving = true;
}


void StateMachine::ProcessState()
{
	if (this->_isRemoving && !this->_states.empty())
	{
		this->_states.pop();

		if (!this->_states.empty())
		{
			delete this->_states.top();
			this->_states.top()->Resume();
		}
	}

	if (this->_isAdding)
	{
		if (!this->_states.empty())
		{
			if (this->_isReplacing)
			{
				this->_states.pop();
			}

			else
			{
				this->_states.top()->Pause();
			}

		}

		this->_states.push(std::move(this->_newState));
		this->_states.top()->Init();
		this->_isAdding = false;
	}
}

StateRef &StateMachine::GetActiveState()
{
	return this->_states.top();
}