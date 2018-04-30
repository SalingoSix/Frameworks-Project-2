#ifndef _Command_HG_
#define _Command_HG_

#include <string>

#include "iCommand.h"
#include "CommandGroup.h"

class Command : public iCommand
{
public:
	Command();
	~Command();

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

	void scriptComplete();

	std::string commandName;

	std::string fileName;

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