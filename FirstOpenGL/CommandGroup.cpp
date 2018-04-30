#include "CommandGroup.h"

CommandGroup::CommandGroup()
{
	isCompleted = 0;
	isParallel = 0;
	isIssued = 0;
	isDeleted = 0;
	completedCommands = 0;
	isCommandGroup = 1;
	int commandID = -1;
	myParent = NULL;
}

CommandGroup::~CommandGroup()
{

}

bool CommandGroup::getComplete()
{
	return this->isCompleted;
}

bool CommandGroup::getParallel()
{
	return this->isParallel;
}

bool CommandGroup::getIssued()
{
	return this->isIssued;
}

bool CommandGroup::getDeleted()
{
	return this->isDeleted;
}

bool CommandGroup::getCommandGroup()
{
	return this->isCommandGroup;
}

int CommandGroup::getCommandID()
{
	return this->commandID;
}

iCommand* CommandGroup::getMyParent()
{
	return this->myParent;
}

void CommandGroup::setComplete(bool value)
{
	this->isCompleted = value;
}

void CommandGroup::setParallel(bool value)
{
	this->isParallel = value;
}

void CommandGroup::setIssued(bool value)
{
	this->isIssued = value;
}

void CommandGroup::setDeleted(bool value)
{
	this->isDeleted = value;
}

void CommandGroup::setCommandGroup(bool value)
{
	this->isCommandGroup = value;
}

void CommandGroup::setCommandID(int value)
{
	this->commandID = value;
}

void CommandGroup::setMyParent(iCommand* value)
{
	this->myParent = value;
}

void CommandGroup::commandComplete()
{
	this->completedCommands++;

	if (completedCommands == v_Commands.size())
	{
		this->isCompleted = true;
		if (myParent != NULL)
		{
			CommandGroup* theParent = (CommandGroup*)myParent;
			theParent->commandComplete();
		}
	}
}