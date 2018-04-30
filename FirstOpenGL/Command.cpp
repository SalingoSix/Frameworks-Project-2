#include "Command.h"

Command::Command()
{
	isCompleted = 0;
	isParallel = 0;
	isIssued = 0;
	isDeleted = 0;
	isCommandGroup = 0;
	int commandID = -1;
	myParent = NULL;
}

Command::~Command()
{

}

bool Command::getComplete()
{
	return this->isCompleted;
}

bool Command::getParallel()
{
	return this->isParallel;
}

bool Command::getIssued()
{
	return this->isIssued;
}

bool Command::getDeleted()
{
	return this->isDeleted;
}

bool Command::getCommandGroup()
{
	return this->isCommandGroup;
}

int Command::getCommandID()
{
	return this->commandID;
}

iCommand* Command::getMyParent()
{
	return this->myParent;
}

void Command::setComplete(bool value)
{
	this->isCompleted = value;
}

void Command::setParallel(bool value)
{
	this->isParallel = value;
}

void Command::setIssued(bool value)
{
	this->isIssued = value;
}

void Command::setDeleted(bool value)
{
	this->isDeleted = value;
}

void Command::setCommandGroup(bool value)
{
	this->isCommandGroup = value;
}

void Command::setCommandID(int value)
{
	this->commandID = value;
}

void Command::setMyParent(iCommand* value)
{
	this->myParent = value;
}

void Command::scriptComplete()
{
	//this->isCompleted = true;

	CommandGroup* theParent = (CommandGroup*)myParent;

	theParent->commandComplete();
}