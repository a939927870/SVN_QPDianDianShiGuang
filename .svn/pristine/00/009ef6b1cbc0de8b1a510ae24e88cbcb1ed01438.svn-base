#include <algorithm>
#include "InvitingFriendTask.h"
#include "PlayerTaskManager.h"
#include "../UserManager.h"
#include "../Work.h"
#include "IRewardStrategy.h"
#include "CardReward.h"

class BaseTimeInviting
{
public:
	BaseTimeInviting(){
		mBaseTimeInviting = -1;
		struct tm _tm = {0};
		_tm.tm_year = 2016 -1900;
		_tm.tm_mon = 10;
		_tm.tm_mday = 10;
		//_tm.tm_wday = 4;
		//_tm.tm_yday = 314;
		time_t baseTime = mktime(&_tm);
		mBaseTimeInviting = (Lint)baseTime;

		//test
		//const time_t _gtt = time(0);
		//struct tm* _gtm = gmtime(&_gtt);
		//time_t baseTime2 = mktime(_gtm);
	}
	Lint mBaseTimeInviting;
};

BaseTimeInviting gBaseTime;

InvitingFriendTask::InvitingFriendTask(const PlayerTaskAttribute & attribute, User* doTaskUser)
	: PlayerTaskBase(attribute, doTaskUser), m_inviterId(-1), m_taskFinished(false), m_invitingInfoLoaded(false), m_inBinding(false)
{
	LoadBindingRelationships();
}

void InvitingFriendTask::AttachEvents(PlayerTaskManager & mgr)
{
	PlayerTaskManager::BindedByInviteeEventProcessor processor1 = bind(&InvitingFriendTask::OnBindedByInvitee, this, placeholders::_1);
	mgr.RegisterBindedByInviteeEvnetProcessor(processor1);

	PlayerTaskManager::PlayingNumIncreasedEventProcessor processor2 = bind(&InvitingFriendTask::OnPlayingNumIncreased, this);
	mgr.RegisterPlayingNumIncreasedEventProcessor(processor2);

	PlayerTaskManager::InviteeFinishedTaskEventProcessor processor3 = bind(&InvitingFriendTask::OnInviteeFinishedTask, this, placeholders::_1);
	mgr.RegisterInviteeFinishedTaskEventProcessor(processor3);
}

bool InvitingFriendTask::Process(LMsg * msg)
{
	bool processed = false;
	if (msg->m_msgId == MSG_C_2_S_GET_INVITING_INFO)
		processed = Process((LMsgC2SGetInvitingInfo*)msg);
	if (msg->m_msgId == MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS)
		processed = Process((LMsgLDB2LMGBindingRelationships*)msg);
	else if (msg->m_msgId == MSG_C_2_S_BIND_INVITER)
		processed = Process((LMsgC2SBindInviter*)msg);
	else if (msg->m_msgId == MSG_LDB_2_LMG_INSERT_BINDING_RELATIONSHIP)
		processed = Process((LMsgLDB2LMGInsertBindingRelationship*)msg);
	else
		processed = PlayerTaskBase::Process(msg);
	return processed;
}

ErrorCode::ErrorCode InvitingFriendTask::OnBindedByInvitee(Lint inviteeId)
{
	if(!m_invitingInfoLoaded)
	{
		LLOG_DEBUG("InvitingFriendTask::OnBindedByInvitee:user %d's inviting info has not been loaded, so do not handle.", m_doTaskUser->m_userData.m_id);
		return ErrorCode::Unknown;
	}

	ErrorCode::ErrorCode code = ErrorCode::ExceedMaximum;
	if (m_invitingContext.size() < NUM_MAX_INVITEE)
	{
		code = ErrorCode::UserNotExist;
		UserIdInfo* idInfo = gUserManager.GetUserIdInfo(inviteeId);
		if (idInfo != NULL)
		{
			auto iter = std::find_if(m_invitingContext.begin(), m_invitingContext.end(), [inviteeId](const InvitingContext& context){return inviteeId == context.inviteeId;});
			if(iter == m_invitingContext.end())
			{
				InvitingContext context = { inviteeId, false };
				m_invitingContext.push_back(context);
			}
			code = ErrorCode::ErrorNone;
		}
		else
			LLOG_ERROR("User %d can't be binded by user %d, because invitee does not exist", m_doTaskUser->m_userData.m_id, inviteeId);
	}
	else
	{
		LLOG_ERROR("User %d can't be binded by user %d, because exceeds max", m_doTaskUser->m_userData.m_id, inviteeId);
	}
	return code;
}

void InvitingFriendTask::OnPlayingNumIncreased(void)
{
	if(!m_invitingInfoLoaded)
	{
		LLOG_DEBUG("InvitingFriendTask::OnPlayingNumIncreased: user %d 's inviting info has not been loaded, so do not handle", m_doTaskUser->m_userData.m_id);
		return;
	}

	if (m_inviterId != -1)
	{
		if(!m_taskFinished)
			OnProgressIncreased();
		else
			LLOG_DEBUG("InvitingFriendTask::OnPlayingNumIncreased: user %d has aleady finish his task.", m_doTaskUser->m_userData.m_id);
	}
	else
		LLOG_DEBUG("InvitingFriendTask::OnPlayingNumIncreased: user %d does not bind anyone.", m_doTaskUser->m_userData.m_id);
}

void InvitingFriendTask::OnInviteeFinishedTask(Lint inviteeId)
{
	LLOG_DEBUG("Invitee %d finished his task.", inviteeId);
	auto iter = find_if(m_invitingContext.begin(), m_invitingContext.end(),
		[inviteeId](const InvitingContext& context) {return context.inviteeId == inviteeId; });
	if (iter != m_invitingContext.end())
		iter->taskFinished = true;
}

void InvitingFriendTask::LoadBindingRelationships(void)
{
	LLOG_DEBUG("Start to load user %d's bingding relationships", m_doTaskUser->m_userData.m_id);
	LMsgLMG2LDBReqBindingRelationships msg;
	msg.m_userId = m_doTaskUser->m_userData.m_id;
	gWork.SendMsgToDb(msg);
}

void InvitingFriendTask::OnFinished(void)
{
	LLOG_DEBUG("User %d finished his task, and notify his inviter %d.", m_doTaskUser->m_userData.m_id, m_inviterId);
	User* user = gUserManager.GetUserbyDataId(m_inviterId);
	if (user != NULL)
	{
		PlayerTaskManager* mgr = user->GetTaskManager();
		if (mgr != NULL)
			mgr->OnInviteeFinishedTask(m_doTaskUser->m_userData.m_id);
	}

	stringstream ss;
	ss << "UPDATE bindingrelationships SET TaskFinished = 1";
	ss << " WHERE Invitee=" << m_doTaskUser->m_userData.m_id;

	LMsgL2LDBExecSql msg;
	msg.m_sql = move(ss.str());
	gWork.SendMsgToDb(msg);
}

bool InvitingFriendTask::GetUserBasicInfo(Lint userId, UserBasicInfo& info)
{
	bool succeed = false;
	UserIdInfo* idInfo = gUserManager.GetUserIdInfo(userId);
	if (idInfo != NULL)
	{
		info.m_id = idInfo->m_id;;
		info.m_unionId = idInfo->m_unionId;
		info.m_nickName = idInfo->m_nike;
		info.m_headImgUrl = idInfo->m_headImageUrl;
		succeed = true;
	}
	return succeed;
}

bool InvitingFriendTask::Process(LMsgLDB2LMGBindingRelationships * msg)
{
	if(m_inviterId == -1 && m_invitingContext.empty())
	{
		m_inviterId = msg->m_inviterId;
		m_taskFinished = msg->m_finished != 0;
		SetRewardedUser(m_inviterId);
		if (msg->m_inviteeCount > 0)
		{
			m_invitingContext.reserve(msg->m_inviteeCount);
			for (Lint i = 0; i < msg->m_inviteeCount; i++)
			{
				InvitingContext context = { msg->m_invitees[i], msg->m_taskFinished[i] != 0 };
				m_invitingContext.push_back(context);
			}
		}
		LLOG_DEBUG("User %d's bingding relationships was loaded. inviter id = %d, invitee num = %d.", m_doTaskUser->m_userData.m_id, msg->m_inviterId, msg->m_inviteeCount);
	}
	m_invitingInfoLoaded = true;
	return true;
}

bool InvitingFriendTask::Process(LMsgC2SGetInvitingInfo * msg)
{
	LMsgS2CInvitingInfo ret;
	ret.m_reward.m_type = m_attribute.reward;
	ret.m_reward.m_count = m_attribute.rewardCount;

	if (!GetUserBasicInfo(m_inviterId, ret.m_inviter))
		ret.m_inviter.m_id = -1;

	for (auto iter = m_invitingContext.begin(); iter != m_invitingContext.end(); ++iter)
	{
		auto& context = *iter;
		InviteeInfo info;
		if (GetUserBasicInfo(context.inviteeId, info.m_info))
		{
			info.m_taskFinished = context.taskFinished;
			ret.m_invitees.push_back(info);
		}
	}
	SendMsgToClient(ret);
	return true;
}

bool InvitingFriendTask::Process(LMsgC2SBindInviter * msg)
{
	if(!m_invitingInfoLoaded || m_inBinding)
	{
		LMsgS2CBindInviter ret;
		ret.m_error = ErrorCode::Unknown;
		SendMsgToClient(ret);
		return true;
	}

	ErrorCode::ErrorCode code = ErrorCode::BindingSelf;
	if (msg->m_inviterId != m_doTaskUser->m_userData.m_id)
	{
		code = ErrorCode::NotQualified;
		if (m_doTaskUser->m_userData.m_regTime >= gBaseTime.mBaseTimeInviting)
		{
			code = ErrorCode::AlreadyBinded;
			if (m_inviterId == -1)
			{
				code = ErrorCode::UserNotExist;
				UserIdInfo* idInfo = gUserManager.GetUserIdInfo(msg->m_inviterId);
				if (idInfo != NULL)
				{
					//code = ErrorCode::ErrorNone;
					User* user = gUserManager.GetUserbyDataId(msg->m_inviterId);
					if (user != NULL)
					{
						PlayerTaskManager* mgr = user->GetTaskManager();
						code = mgr->OnUserBindedByInvitee(m_doTaskUser->m_userData.m_id);

						if(code == ErrorCode::ErrorNone)
						{
							m_inviterId = msg->m_inviterId;
							InsertBindingRelationshipIntoDB();

							CardReward reward(m_doTaskUser->m_userData.m_id, 20, IRewardStrategy::BindingInviter);
							reward.IssueReward();

							SetRewardedUser(m_inviterId);
						}
					}
					else
					{
						LLOG_DEBUG("Inviter %d is offline, send msg to logicdb to check if reached max or not.", msg->m_inviterId);
						LMsgLMG2LDBInsertBindingRelationship insertMsg;
						insertMsg.m_userId = m_doTaskUser->m_userData.m_id;
						insertMsg.m_inviter = msg->m_inviterId;
						insertMsg.m_maxInviteeNum = NUM_MAX_INVITEE;
						gWork.SendMsgToDb(insertMsg);
						m_inBinding = true;
						return true;
					}
				}
			}
			else if (m_inviterId == msg->m_inviterId)
				code = ErrorCode::AlreadyBinded;
		}
	}

	LMsgS2CBindInviter ret;
	ret.m_error = code;
	if (code == ErrorCode::ErrorNone)
		GetUserBasicInfo(m_inviterId, ret.m_inviter);
	SendMsgToClient(ret);
	LLOG_DEBUG("InvitingFriendTask::Process: error = %d, user id = %d, inviter id = %d", (Lint)code, m_doTaskUser->m_userData.m_id, msg->m_inviterId);
	return true;
}

bool InvitingFriendTask::Process(LMsgLDB2LMGInsertBindingRelationship * msg)
{
	LLOG_DEBUG("InvitingFriendTask::Process(LMsgLDB2LMGInsertBindingRelationship * msg), error = %d.", msg->m_errorCode);
	LMsgS2CBindInviter ret;
	ret.m_error = msg->m_errorCode;
	if(msg->m_errorCode == ErrorCode::ErrorNone)
	{
		if(m_inviterId == -1)
		{
			User* user = gUserManager.GetUserbyDataId(msg->m_inviter);
			if (user != NULL)
			{
				PlayerTaskManager* mgr = user->GetTaskManager();
				mgr->OnUserBindedByInvitee(m_doTaskUser->m_userData.m_id);
			}

			m_inviterId = msg->m_inviter;

			CardReward reward(m_doTaskUser->m_userData.m_id, 20, IRewardStrategy::BindingInviter);
			reward.IssueReward();

			SetRewardedUser(m_inviterId);

		}
		else
			ret.m_error = ErrorCode::AlreadyBinded;
	}

	if(ret.m_error == ErrorCode::ErrorNone)
		GetUserBasicInfo(m_inviterId, ret.m_inviter);
	SendMsgToClient(ret);
	m_inBinding = false;
	return true;
}

void InvitingFriendTask::InsertBindingRelationshipIntoDB(void)
{
	LMsgL2LDBExecSql msg;
	stringstream ss;
	ss << "INSERT INTO bindingrelationships (Invitee, Inviter, BindingTime, TaskFinished) VALUES (";
	ss << m_doTaskUser->m_userData.m_id << ",";
	ss << m_inviterId << ",";
	ss << gWork.GetCurTime().Secs() << ",";
	ss << (m_rewardRecved ? 1 : 0) << ")";
	msg.m_sql = move(ss.str());
	LLOG_DEBUG("InvitingFriendTask::InsertBindingRelationshipIntoDB: sql = %s", msg.m_sql.c_str());
	gWork.SendMsgToDb(msg);
}

void InvitingFriendTask::SetRewardedUser(Lint userId)
{
	if(userId!= -1 && m_rewardStrategy)
		m_rewardStrategy->SetRewardedUser(userId);
}
