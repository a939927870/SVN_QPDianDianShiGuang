#ifndef __PlayingGameTask_
#define __PlayingGameTask_

#include "PlayerTaskBase.h"

class PlayingGameTask : public PlayerTaskBase
{
public:
	PlayingGameTask(const PlayerTaskAttribute& attribute, User* doTaskUser);

public:
	virtual void			AttachEvents(PlayerTaskManager& mgr);
};

#endif	// __PlayingGameTask_