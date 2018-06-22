#include "UserSharedTask.h"
#include "PlayerTaskManager.h"
#include "IRewardStrategy.h"

UserSharedTask::UserSharedTask(const PlayerTaskAttribute & attribute, User* doTaskUser)
	: PlayerTaskBase(attribute, doTaskUser)
{
}

bool UserSharedTask::Process(LMsg * msg)
{
	bool processed = false;
	if (msg->m_msgId == MSG_C_2_S_USER_SHARED)
		processed = Process((LMsgC2SUserShared*)msg);
	else
		processed = PlayerTaskBase::Process(msg);

	return processed;
}

bool UserSharedTask::Process(LMsgC2SUserShared * msg)
{
	OnProgressIncreased();
	return true;
}
