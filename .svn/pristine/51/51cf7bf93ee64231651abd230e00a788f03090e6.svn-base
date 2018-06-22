#ifndef __UserSharedTask_
#define __UserSharedTask_

#include <functional>
#include "PlayerTaskBase.h"

using namespace std;

class UserSharedTask : public PlayerTaskBase
{
public:
	UserSharedTask(const PlayerTaskAttribute& attribute, User* doTaskUser);
	
public:
	virtual bool			Process(LMsg* msg);

private:
	bool					Process(LMsgC2SUserShared* msg);
};

#endif		// __UserSharedTask_