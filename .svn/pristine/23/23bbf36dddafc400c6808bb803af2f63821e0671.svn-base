#include "PlayingGameTask.h"
#include "PlayerTaskManager.h"
#include "IRewardStrategy.h"

PlayingGameTask::PlayingGameTask(const PlayerTaskAttribute & attribute, User* doTaskUser)
	: PlayerTaskBase(attribute, doTaskUser)
{
}

void PlayingGameTask::AttachEvents(PlayerTaskManager & mgr)
{
	PlayerTaskManager::PlayingNumIncreasedEventProcessor processor = [this]() {this->OnProgressIncreased();};
	mgr.RegisterPlayingNumIncreasedEventProcessor(processor);
}