#include "PlayerTaskManager.h"
#include "PlayerTaskAttributeCollection.h"
#include "GenericRewardStrategyFactory.h"
#include "GenericPlayerTaskFactory.h"
#include "PlayerTaskBase.h"
#include "../Work.h"

boost::shared_ptr<IPlayerTaskFactory> PlayerTaskManager::m_taskFactory = boost::make_shared<GenericPlayerTaskFactory>();
boost::shared_ptr<IRewardStrategyFactory> PlayerTaskManager::m_rewardStrategyFactory = boost::make_shared<GenericRewardStrategyFactory>();

PlayerTaskManager::PlayerTaskManager(User* doTaskUser)
	: m_doTaskUser(doTaskUser)
{
}

PlayerTaskManager::~PlayerTaskManager(void)
{
	FreeTasks();
}

void PlayerTaskManager::LoadTasks(void)
{
	if (m_tasks.empty())
	{
		Lint currentDate = gWork.GetCurTime().GetDate();
		Lsize taskNum = PlayerTaskAttributeCollection::Instance()->GetTaskNum();
		for (Lsize i = 0; i < taskNum; i++)
		{
			auto& attri = PlayerTaskAttributeCollection::Instance()->GetTask(i);
			if (attri.IsValid(currentDate))
			{
				auto task = m_taskFactory->CreateTask(attri, m_doTaskUser);
				if (task && task->LoadProgress())
				{
					task->AttachEvents(*this);
					auto rewardStrategy = m_rewardStrategyFactory->CreateRewardStrategy(attri.reward, attri.rewardCount, task->GetRewardedUserId(), GetRewardReason(attri));
					task->SetRewardStrategy(rewardStrategy);
					m_tasks.push_back(task);
				}
				else
					task.reset();
			}
		}
	}
}

void PlayerTaskManager::FreeTasks(void)
{
	m_tasks.clear();
}

void PlayerTaskManager::UpdateTasks(void)
{
	Lint currentDate = gWork.GetCurTime().GetDate();
	Lsize taskNum = PlayerTaskAttributeCollection::Instance()->GetTaskNum();
	for (Lsize i = 0; i < taskNum; i++)
	{
		auto& attri = PlayerTaskAttributeCollection::Instance()->GetTask(i);
		if (attri.IsValid(currentDate) && !IsTaskExist(attri.id))
		{
			auto task = m_taskFactory->CreateTask(attri, m_doTaskUser);
			if (task && task->LoadProgress())
			{
				task->AttachEvents(*this);
				auto rewardStrategy = m_rewardStrategyFactory->CreateRewardStrategy(attri.reward, attri.rewardCount, task->GetRewardedUserId(), GetRewardReason(attri));
				task->SetRewardStrategy(rewardStrategy);
				m_tasks.push_back(task);
			}
			else
				task.reset();
		}
	}
}

bool PlayerTaskManager::GetTask(Lint taskId, TaskBasicInfo & info)
{
	bool succeed = false;
	for (auto iter = m_tasks.begin();iter != m_tasks.end(); ++iter)
	{
		auto& t = *iter;
		if (t->GetId() == taskId)
		{
			if(!t->IsValid())
				break;

			t->Update();
			if (t->IsExplicit())
			{
				t->GetBasicInfo(info);
				succeed = true;
			}
			else
				break;
		}
	}
	return succeed;
}

vector<TaskBasicInfo> PlayerTaskManager::GetTaskList(void)
{
	vector<TaskBasicInfo> infos;
	for (auto iter = m_tasks.begin(); iter != m_tasks.end(); ++iter)
	{
		auto& t = *iter;
		if(!t->IsValid())
			continue;
		t->Update();
		if (t->IsExplicit())
		{
			TaskBasicInfo info;
			t->GetBasicInfo(info);
			infos.push_back(move(info));
		}
	}
	return move(infos);
}

void PlayerTaskManager::OnUserPlayingNumIncreased(void)
{
	for(auto iter = m_playingEventProcessors.begin(); iter != m_playingEventProcessors.end(); ++iter)
		(*iter)();
}

ErrorCode::ErrorCode PlayerTaskManager::OnUserBindedByInvitee(Lint inviteeId)
{
	ErrorCode::ErrorCode code = ErrorCode::ErrorNone;
	for(auto iter = m_bindedEventProcessors.begin(); iter != m_bindedEventProcessors.end(); ++iter)
		code = (*iter)(inviteeId);
	return code;
}

void PlayerTaskManager::OnInviteeFinishedTask(Lint inviteeId)
{
	for(auto iter = m_finishedTaskEventProcessors.begin(); iter != m_finishedTaskEventProcessors.end(); ++iter)
		(*iter)(inviteeId);
}

void PlayerTaskManager::Process(LMsg * msg)
{
	for(auto iter = m_tasks.begin(); iter != m_tasks.end(); ++iter)
	{
		if((*iter)->Process(msg))
			break;
	}
}

void PlayerTaskManager::RegisterPlayingNumIncreasedEventProcessor(PlayingNumIncreasedEventProcessor processor)
{
	m_playingEventProcessors.push_back(processor);
}

void PlayerTaskManager::RegisterBindedByInviteeEvnetProcessor(BindedByInviteeEventProcessor processor)
{
	m_bindedEventProcessors.push_back(processor);
}

void PlayerTaskManager::RegisterInviteeFinishedTaskEventProcessor(InviteeFinishedTaskEventProcessor processor)
{
	m_finishedTaskEventProcessors.push_back(processor);
}

IRewardStrategy::RewardReason PlayerTaskManager::GetRewardReason(const PlayerTaskAttribute& attri)
{
	IRewardStrategy::RewardReason reason = IRewardStrategy::None;
	if (attri.type == PlayerTaskAttribute::PlayingGame)
		reason = IRewardStrategy::PlayingGame;
	else if (attri.type == PlayerTaskAttribute::UserShared)
		reason = IRewardStrategy::UserShared;
	else if (attri.type == PlayerTaskAttribute::InvitingFriend)
		reason = IRewardStrategy::InvitingFriend;
	else if (attri.type == PlayerTaskAttribute::CompletingGame)
		reason = IRewardStrategy::PlayingGame;
	return reason;
}

bool PlayerTaskManager::IsTaskExist(int id)
{
	bool exist = false;
	for (auto iter = m_tasks.begin(); iter != m_tasks.end(); ++iter)
	{
		if ((*iter)->GetId() == id)
		{
			exist = true;
			break;
		}
	}
	return exist;
}