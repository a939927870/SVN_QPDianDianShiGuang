#ifndef __GenericPlayerTaskFactory_
#define __GenericPlayerTaskFactory_

#include <boost/shared_ptr.hpp>
#include "IPlayerTaskFactory.h"

class GenericPlayerTaskFactory : public IPlayerTaskFactory
{
public:
	virtual boost::shared_ptr<PlayerTaskBase>		CreateTask(const PlayerTaskAttribute& attribute, User* doTaskUser);
};

#endif	// __GenericPlayerTaskFactory_