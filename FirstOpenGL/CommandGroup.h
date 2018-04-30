#ifndef _CommandGroup_HG_
#define _CommandGroup_HG_

#include <string>
#include <vector>

#include "iCommand.h"

class CommandGroup : public iCommand
{
public:
	CommandGroup();
	~CommandGroup();

	virtual bool getComplete();

	virtual bool getParallel();

	virtual bool getIssued();

	virtual bool getDeleted();

	virtual bool getCommandGroup();

	virtual int getCommandID();

	virtual iCommand* getMyParent();

	virtual void setComplete(bool value);

	virtual void setParallel(bool value);

	virtual void setIssued(bool value);

	virtual void setDeleted(bool value);

	virtual void setCommandGroup(bool value);

	virtual void setCommandID(int value);

	virtual void setMyParent(iCommand* value);

	void commandComplete();

	std::vector< iCommand* > v_Commands;

	int completedCommands;

private:
	bool isCompleted;
	bool isParallel;
	bool isIssued;
	bool isDeleted;
	bool isCommandGroup;
	int commandID;
	iCommand* myParent;
};

#endif