#include "GenericPlayerTaskFactory.h"
#include "IRewardStrategyFactory.h"
#include "PlayingGameTask.h"
#include "UserSharedTask.h"
#include "InvitingFriendTask.h"
//#include "CompletingGameTask.h"

boost::shared_ptr<PlayerTaskBase> GenericPlayerTaskFactory::CreateTask(const PlayerTaskAttribute & attribute, User* doTaskUser)
{
	boost::shared_ptr<PlayerTaskBase> task;
	switch (attribute.type)
	{
	case PlayerTaskAttribute::PlayingGame:
		task = boost::shared_ptr<PlayerTaskBase>(new PlayingGameTask(attribute, doTaskUser));
		break;
	case PlayerTaskAttribute::UserShared:
		task = boost::shared_ptr<PlayerTaskBase>(new UserSharedTask(attribute, doTaskUser));
		break;
	case PlayerTaskAttribute::InvitingFriend:
		task = boost::shared_ptr<PlayerTaskBase>(new InvitingFriendTask(attribute, doTaskUser));
		break;
	//case PlayerTaskAttribute::TaskType::CompletingGame:
	//	task = boost::shared_ptr<PlayerTaskBase>(new CompletingGameTask(attribute, doTaskUser));
	//	break;
	default:
		break;
	}
	return task;
}