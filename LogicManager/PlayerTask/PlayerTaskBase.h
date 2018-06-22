#ifndef __PlayerTaskBase_
#define __PlayerTaskBase_

#include <boost/shared_ptr.hpp>
#include "LBase.h"
#include "PlayerTaskAttributeCollection.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "defines.h"
#include "../User.h"

class IRewardStrategy;
class PlayerTaskManager;

class PlayerTaskBase
{
public:
	PlayerTaskBase(const PlayerTaskAttribute& attribute, User* doTaskUser);
	virtual ~PlayerTaskBase();

public:
	virtual bool			LoadProgress(void);
	virtual void			AttachEvents(PlayerTaskManager& mgr) {}
	virtual bool			IsExplicit(void) { return true; }
	virtual bool			Process(LMsg* msg);

public:
	void					SetRewardStrategy(boost::shared_ptr<IRewardStrategy> strategy) { m_rewardStrategy = strategy; }
	Lint					GetId(void) { return m_attribute.id; }
	Lint					GetDoTaskUserId(void) { return m_doTaskUser->m_userData.m_id; }
	Lint					GetRewardedUserId(void) { return m_rewardedUserId; }
	void					RecvReward(void);
	void					Update(void);
	bool					IsValid(void);
	void					GetBasicInfo(TaskBasicInfo& info);

protected:
	bool					Process(LMsgLDB2LMGTaskProgress* msg);
	bool					Process(LMsgLDB2LMGInsertTaskProgress* msg);
	bool					Process(LMsgC2SRecvTaskReward * msg);
	void					OnProgressIncreased(void);
	void					SendMsgToClient(LMsg& msg);

private:
	virtual void			OnFinished(void) {}

private:
	void					CalcDuration(time_t& start, time_t& expired);
	bool					HasExpired(void);
	void					PushStatusToClient(void);
	void					SaveProgress(void);
	void					DeleteExpiredProgressInDB(void);
	void					NotifyRewardNotReceived();
	time_t					CalcRemainingTimeToNext(void);

private:
	void					Reset(void);

protected:
	Lint									m_progressId;				// primary key of table taskprogress, only for operating database
	PlayerTaskAttribute						m_attribute;
	Lint									m_progress;
	time_t									m_startTime;
	time_t									m_expiredTime;
	User*									m_doTaskUser;
	Lint									m_rewardedUserId;
	bool									m_rewardRecved;
	boost::shared_ptr<IRewardStrategy>		m_rewardStrategy;
	bool									m_existInDB;
	bool									m_progressUpdated;
	bool									m_progressLoaded;
};

#endif	// __PlayerTaskBase_