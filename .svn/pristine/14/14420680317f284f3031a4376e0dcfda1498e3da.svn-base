#include "PlayerTaskBase.h"
#include "IRewardStrategy.h"
#include "../Work.h"
#include "../UserManager.h"
#include "LMsgS2S.h"

PlayerTaskBase::PlayerTaskBase(const PlayerTaskAttribute & attribute, User* doTaskUser)
	: m_attribute(attribute), m_doTaskUser(doTaskUser), m_progressLoaded(false)
{
	Reset();
	m_rewardedUserId = m_doTaskUser->m_userData.m_id;
}

PlayerTaskBase::~PlayerTaskBase()
{
	SaveProgress();
}

bool PlayerTaskBase::LoadProgress(void)
{
	LMsgLMG2LDBReqTaskProgress msg;
	msg.m_userId = m_doTaskUser->m_userData.m_id;
	msg.m_taskId = m_attribute.id;

	stringstream ss;
	ss << "SELECT Id, Progress, RewardRecved FROM taskprogress WHERE ";
	ss << "TaskId = " << m_attribute.id << " AND ";
	ss << "UserId = " << m_doTaskUser->m_userData.m_id;
	if (m_attribute.IsFixedDuration())
	{
		ss << " AND StartTime = " << m_startTime << " AND ";
		ss << "ExpiredTime = " << m_expiredTime;
	}
	else if(m_attribute.repeated)
		ss << " AND RewardRecved = " << 0;

	ss << " limit 1";
	msg.m_sql = move(ss.str());
	LLOG_DEBUG("User %d's PlayerTaskBase::LoadProgress: sql = %s", m_doTaskUser->m_userData.m_id, msg.m_sql.c_str());
	gWork.SendMsgToDb(msg);

	return true;
}

void PlayerTaskBase::RecvReward(void)
{
	ErrorCode::ErrorCode code = ErrorCode::TaskNotFinished;
	bool statusChanged = false;
	if (m_progress >= m_attribute.total)
	{
		code = ErrorCode::Expired;
		if (!HasExpired())
		{
			if (!m_rewardRecved && m_rewardStrategy)
			{
				m_rewardStrategy->IssueReward();
				m_rewardRecved = true;
				//SaveProgress();
				DeleteExpiredProgressInDB();
				m_progressUpdated = true;
				statusChanged = true;
			}
			code = ErrorCode::ErrorNone;
		}
	}
	LLOG_DEBUG("User %d's PlayerTaskBase::RecvReward: error code is %d.", m_doTaskUser->m_userData.m_id, (Lint)code);

	if(IsExplicit())
	{
		LMsgS2CRecvTaskReward ret;
		ret.m_taskId = m_attribute.id;
		ret.m_error = code;
		if (code == ErrorCode::ErrorNone)
		{
			ret.m_reward.m_type = m_attribute.reward;
			ret.m_reward.m_count = m_attribute.rewardCount;
		}

		SendMsgToClient(ret);
	}


	if(code == ErrorCode::ErrorNone)
	{
		Update();
		statusChanged = true;
	}

	if (statusChanged)
		PushStatusToClient();
}

void PlayerTaskBase::Update(void)
{
	if (HasExpired())
		Reset();
}

void PlayerTaskBase::GetBasicInfo(TaskBasicInfo & info)
{
	info.m_id = m_attribute.id;
	info.m_description = m_attribute.desc;
	info.m_curProgress = m_progress;
	info.m_total = m_attribute.total;
	info.m_recvedReward = m_rewardRecved;
	info.m_remainingTime = static_cast<Lint>(CalcRemainingTimeToNext());
}

bool PlayerTaskBase::Process(LMsg * msg)
{
	bool processed = false;
	if (msg->m_msgId == MSG_C_2_S_RECV_TASK_REWARD)
		processed = Process((LMsgC2SRecvTaskReward*)msg);
	else if (msg->m_msgId == MSG_LDB_2_LMG_REQ_TASK_PROGRESS)
		processed = Process((LMsgLDB2LMGTaskProgress*)msg);
	else if (msg->m_msgId == MSG_LDB_2_LMG_INSERT_TASK_PROGRESS)
		processed = Process((LMsgLDB2LMGInsertTaskProgress*)msg);
	return processed;
}

bool PlayerTaskBase::Process(LMsgC2SRecvTaskReward* msg)
{
	bool processed = false;
	if (msg->m_taskId == m_attribute.id)
	{
		if(IsValid())
		{
			RecvReward();
		}
		else
		{
			LLOG_DEBUG("PlayerTaskBase::Process: task %d is invalid.", m_attribute.id);
			LMsgS2CRecvTaskReward ret;
			ret.m_taskId = m_attribute.id;
			ret.m_error = ErrorCode::Unknown;
			SendMsgToClient(ret);
		}
		processed = true;
	}
	return processed;
}

bool PlayerTaskBase::Process(LMsgLDB2LMGTaskProgress * msg)
{
	bool processed = false;
	if (msg->m_taskId == m_attribute.id)
	{
		if (msg->m_result)
		{
			m_progressId = msg->m_progressId;
			m_progress = msg->m_progress;
			m_rewardRecved = msg->m_rewardRecved != 0;
			m_existInDB = true;
			LLOG_DEBUG("User %d's task %d's progress was loded from database progressId=%d, progress=%d, rewardRecved=%d", m_doTaskUser->m_userData.m_id, m_attribute.id, msg->m_progressId, msg->m_progress, msg->m_rewardRecved);
		}
		m_progressLoaded = true;
		processed = true;

		if (m_progress == m_attribute.total && !m_rewardRecved)
			NotifyRewardNotReceived();
	}
	return processed;

}

bool PlayerTaskBase::Process(LMsgLDB2LMGInsertTaskProgress * msg)
{
	bool processed = false;
	if (msg->m_taskId == m_attribute.id)
	{
		m_progressId = msg->m_progressId;
		LLOG_DEBUG("Task progress was inserted into database, and then progress id %d returned.", m_progressId);
		processed = true;
	}
	return processed;
}

void PlayerTaskBase::Reset(void)
{
	m_progressId = -1;
	m_progress = 0;
	CalcDuration(m_startTime, m_expiredTime);
	m_rewardRecved = false;
	m_existInDB = false;
	m_progressUpdated = false;
}

void PlayerTaskBase::CalcDuration(time_t & start, time_t & expired)
{
	start = -1;
	expired = -1;
	if (m_attribute.duration & PlayerTaskAttribute::FixedDuration)
	{
		if (m_attribute.duration == PlayerTaskAttribute::Day)
		{
			time_t now;
			time(&now);
			struct tm when;
#if _LINUX
			localtime_r(&now, &when);
#else
			localtime_s(&when, &now);
#endif
			when.tm_sec = 0;
			when.tm_min = 0;
			when.tm_hour = 0;
			start = mktime(&when);
			when.tm_mday++;
			expired = mktime(&when);
		}
		else if (m_attribute.duration == PlayerTaskAttribute::Week)
		{
			time_t now;
			time(&now);
			struct tm when;
#if _LINUX
			localtime_r(&now, &when);
#else
			localtime_s(&when, &now);
#endif
			when.tm_sec = 0;
			when.tm_min = 0;
			when.tm_hour = 0;
			//adust to monday
			int wday = when.tm_wday == 0 ? when.tm_wday + 7 : when.tm_wday;
			when.tm_mday -= (wday - 1);
			when.tm_wday = 1;
			start = mktime(&when);
			when.tm_mday += 7;
			expired = mktime(&when);
		}
	}
}

bool PlayerTaskBase::HasExpired(void)
{
	bool expired = false;
	if (m_attribute.IsFixedDuration())
	{
		time_t currentTime = gWork.GetCurTime().Secs();
		expired = currentTime >= m_expiredTime;
	}
	else if(m_rewardRecved && m_attribute.repeated)
		expired = true;

	if(expired)
		LLOG_DEBUG("User %d's task has expired.", m_doTaskUser->m_userData.m_id);
	return expired;
}

void PlayerTaskBase::OnProgressIncreased(void)
{
	if(!IsValid())
	{
		LLOG_DEBUG("PlayerTaskBase::OnProgressIncreased:task %d is invalid.", m_attribute.id);
		return;
	}

	if(!m_progressLoaded)
	{
		LLOG_DEBUG("PlayerTaskBase::OnProgressIncreased:task %d 's progress has not been loaded from database, so do not handle.", m_attribute.id);
		return;
	}

	Update();

	if (m_progress < m_attribute.total)
	{
		m_progress++;
		if (m_progress >= m_attribute.total)
		{
			OnFinished();

			if (!m_attribute.recvManually)
				RecvReward();

			PushStatusToClient();
		}
		//SaveProgress();
		m_progressUpdated = true;
	}
	else
		LLOG_DEBUG("User %d's task %d has been already finished", m_doTaskUser->m_userData.m_id, m_attribute.id);
}

void PlayerTaskBase::SendMsgToClient(LMsg& msg)
{
	if (m_doTaskUser != NULL)
		m_doTaskUser->Send(msg);
}

void PlayerTaskBase::PushStatusToClient(void)
{
	if(IsExplicit())
	{
		LMsgS2CTaskList msg;
		TaskBasicInfo info;
		GetBasicInfo(info);
		msg.m_tasks.push_back(move(info));
		SendMsgToClient(msg);
	}
}

void PlayerTaskBase::SaveProgress(void)
{
	if(!IsValid())
	{
		LLOG_DEBUG("PlayerTaskBase::SaveProgress: task %d is invalid.", m_attribute.id);
		return;
	}

	if(!m_progressLoaded)
	{
		LLOG_DEBUG("PlayerTaskBase::SaveProgress: task %d 's has not been loaded, so do not handle.", m_attribute.id);
		return;
	}

	if(m_progressUpdated)
	{
		if (m_existInDB)
		{
			stringstream ss;
			ss << "UPDATE taskprogress SET ";
			ss << "Progress=" << m_progress << ",";
			ss << "RewardRecved=" << (m_rewardRecved ? 1 : 0);
			if (m_progressId == -1)
			{
				ss << " WHERE TaskId = " << m_attribute.id << " AND ";
				ss << "UserId = " << m_doTaskUser->m_userData.m_id << " AND ";
				if (m_attribute.IsFixedDuration())
				{
					ss << "StartTime = " << m_startTime << " AND ";
					ss << "ExpiredTime = " << m_expiredTime;
				}
				else
					ss << "RewardRecved = " << (m_rewardRecved ? 1 : 0);
			}	
			else
				ss << " WHERE Id=" << m_progressId;
			LMsgL2LDBExecSql sql;
			sql.m_sql = move(ss.str());
			LLOG_DEBUG("User %d's PlayerTaskBase::SaveProgress:sql %s is to execute.", m_doTaskUser->m_userData.m_id, sql.m_sql.c_str());
			gWork.SendMsgToDb(sql);
		}
		else
		{
			LMsgLMG2LDBInsertTaskProgress msg;
			msg.m_userId = m_doTaskUser->m_userData.m_id;
			msg.m_taskId = m_attribute.id;
			stringstream ss;
			ss << "INSERT INTO taskprogress (TaskId, StartTime, ExpiredTime, Progress, Total, UserId, RewardRecved) VALUES (";
			ss << m_attribute.id << ",";
			ss << m_startTime << ",";
			ss << m_expiredTime << ",";
			ss << m_progress << ",";
			ss << m_attribute.total << ",";
			ss << m_doTaskUser->m_userData.m_id << ",";
			ss << (m_rewardRecved ? 1 : 0) << ")";
			msg.m_sql = move(ss.str());
			LLOG_DEBUG("User %d's PlayerTaskBase::SaveProgress:sql %s is to execute.", m_doTaskUser->m_userData.m_id, msg.m_sql.c_str());
			gWork.SendMsgToDb(msg);
			m_existInDB = true;
		}
	}
}

time_t PlayerTaskBase::CalcRemainingTimeToNext(void)
{
	time_t remainingTime = -1;
	
	if (m_attribute.IsFixedDuration() && m_attribute.repeated && m_rewardRecved)
	{
		time_t start = 0, expired = -1;
		CalcDuration(start, expired);
		time_t now = gWork.GetCurTime().Secs();
		if (expired >= now)
			remainingTime = expired - now;
	}
	LLOG_DEBUG("User %d's task %d 's remaining time to next is %d.", m_doTaskUser->m_userData.m_id, m_attribute.id, remainingTime);
	return remainingTime;
}

bool PlayerTaskBase::IsValid(void)
{
	return m_attribute.IsValid(gWork.GetCurTime().GetDate());
}

void PlayerTaskBase::DeleteExpiredProgressInDB()
{
	if(m_existInDB && HasExpired())
	{
		stringstream ss;
		ss << "DELETE FROM taskprogress WHERE id = " << m_progressId;
		LMsgL2LDBExecSql sql;
		sql.m_sql = move(ss.str());
		LLOG_DEBUG("User %d's PlayerTaskBase::DeleteExpiredProgressInDB:sql %s is to execute.", m_doTaskUser->m_userData.m_id, sql.m_sql.c_str());
		gWork.SendMsgToDb(sql);
	}
}

void PlayerTaskBase::NotifyRewardNotReceived()
{
	if (IsExplicit())
	{
		LMsgS2CRewardNotReceived msg;
		SendMsgToClient(msg);
	}
}