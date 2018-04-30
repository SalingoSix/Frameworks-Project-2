#ifndef _iCommand_HG_
#define _iCommand_HG_

class iCommand
{
public:
	virtual ~iCommand() {};

	virtual bool getComplete() = 0;

	virtual bool getParallel() = 0;

	virtual bool getIssued() = 0;

	virtual bool getDeleted() = 0;

	virtual bool getCommandGroup() = 0;

	virtual int getCommandID() = 0;

	virtual iCommand* getMyParent() = 0;

	virtual void setComplete(bool value) = 0;

	virtual void setParallel(bool value) = 0;

	virtual void setIssued(bool value) = 0;

	virtual void setDeleted(bool value) = 0;

	virtual void setCommandGroup(bool value) = 0;

	virtual void setCommandID(int value) = 0;

	virtual void setMyParent(iCommand* value) = 0;
};


#endif