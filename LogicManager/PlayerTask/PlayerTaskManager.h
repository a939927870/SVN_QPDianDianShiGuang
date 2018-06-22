#ifndef __PlayerTaskManager_
#define __PlayerTaskManager_

#include <vector>
#include <functional>
#include <boost/shared_ptr.hpp>
#include "LBase.h"
#include "LMsg.h"
#include "IRewardStrategy.h"
#include "PlayerTaskAttributeCollection.h"

using namespace std;

class PlayerTaskBase;
class IPlayerTaskFactory;
class IRewardStrategyFactory;
class User;

class PlayerTaskManager
{
public:
	PlayerTaskManager(User* doTaskUser);
	~PlayerTaskManager(void);

public:
	void							LoadTasks(void);
	void							FreeTasks(void);
	void							UpdateTasks(void);
	bool							GetTask(Lint taskId, TaskBasicInfo& info);
	vector<TaskBasicInfo>			GetTaskList(void);

	// events notification
public:
	void							OnUserPlayingNumIncreased(void);			// 玩家玩的局数增加
	ErrorCode::ErrorCode			OnUserBindedByInvitee(Lint inviteeId);		// 玩家被被邀请者绑定
	void							OnInviteeFinishedTask(Lint inviteeId);		// 被邀请者完成了任务
	void							Process(LMsg* msg);

public:
	typedef function<void()>			PlayingNumIncreasedEventProcessor;
	void								RegisterPlayingNumIncreasedEventProcessor(PlayingNumIncreasedEventProcessor processor);

	typedef function<ErrorCode::ErrorCode(Lint)>	BindedByInviteeEventProcessor;
	void								RegisterBindedByInviteeEvnetProcessor(BindedByInviteeEventProcessor processor);

	typedef function<void(Lint)>		InviteeFinishedTaskEventProcessor;
	void								RegisterInviteeFinishedTaskEventProcessor(InviteeFinishedTaskEventProcessor processor);

private:
	IRewardStrategy::RewardReason		GetRewardReason(const PlayerTaskAttribute & attri);
	bool								IsTaskExist(int id);

private:
	User*												m_doTaskUser;
	vector<boost::shared_ptr<PlayerTaskBase>>			m_tasks;

	vector<PlayingNumIncreasedEventProcessor>			m_playingEventProcessors;

	vector<BindedByInviteeEventProcessor>				m_bindedEventProcessors;

	vector<InviteeFinishedTaskEventProcessor>			m_finishedTaskEventProcessors;

private:
	static boost::shared_ptr<IPlayerTaskFactory>		m_taskFactory;
	static boost::shared_ptr<IRewardStrategyFactory>	m_rewardStrategyFactory;
};

#endif	// __PlayerTaskManager_