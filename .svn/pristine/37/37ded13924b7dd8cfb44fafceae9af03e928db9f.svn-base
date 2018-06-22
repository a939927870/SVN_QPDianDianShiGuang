#ifndef __IPlayerTaskFactory_
#define __IPlayerTaskFactory_

#include <boost/shared_ptr.hpp>
#include "PlayerTaskAttributeCollection.h"

class PlayerTaskBase;
class User;

class IPlayerTaskFactory
{
public:
	virtual ~IPlayerTaskFactory(){}

public:
	virtual boost::shared_ptr<PlayerTaskBase>		CreateTask(const PlayerTaskAttribute& attribute, User* doTaskUser) = 0;
};

#endif	// __IPlayerTaskFactory_