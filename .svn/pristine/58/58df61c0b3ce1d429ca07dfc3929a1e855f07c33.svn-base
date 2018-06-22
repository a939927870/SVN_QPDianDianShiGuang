#include "Desk.h"
#include "Work.h"
#include "Config.h"
#include "RoomVip.h"
#include "UserManager.h"
#include "BaseCommonFunc.h"
#include "InsideNet.h"

///////////////////////////////////////////////////work 部分//////////////////////////////////////////////////////////////
void  Work::SendToMatchServer(LMsg& msg)
{
	if (m_matchInfo.m_client)
		m_matchInfo.m_client->Send(msg.GetSendBuff());
}

void  Work::CheckSportResult(LTime& cur)
{
	std::vector<Lint>::iterator iter_time = m_delaySportTime.begin();
	VecLMsgL2LMGSportResult::iterator iter = m_delaySportResult.begin();
	std::vector<Lint>::iterator iter_time_end = m_delaySportTime.end();
	Lint nowSecs = cur.Secs();
	while (iter_time != iter_time_end)
	{
		Lint iter_sec = *iter_time;
		if ((iter_sec + 3) <= nowSecs) //给7秒时间
		{
			LLOG_DEBUG("=========Work::Tick LMsgL2LMGSportResult==deskid==%d========%d , %d  , %d = %d", (*iter).m_deskid, *iter_time, nowSecs, m_delaySportTime.size(), m_delaySportResult.size());
			gWork.SendToMatchServer(*iter);

			iter_time = m_delaySportTime.erase(iter_time);
			iter = m_delaySportResult.erase(iter);

			iter_time_end = m_delaySportTime.end();
			continue;
		}
		++iter_time;
		++iter;
	}
}

void   Work::handerLMGMatchAddDesk(LMsgLMG2LMatchAddDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	//前置判断，如果有异常，相应处理
	LMsgLMG2LCreateDesk * create_msg = &msg->m_create;
	bool isSame = false;
	if (!CheckMatchRoomAddClear_(msg, isSame))
	{
		//把原来的桌子数据返回，让其重新分配。即可
		LMsgL2LMGSportResult sendmsg;
		sendmsg.m_logicId = gConfig.GetServerID();
		sendmsg.m_deskid = create_msg->m_deskID;
		sendmsg.m_rescode = 1;
		sendmsg.m_desktype = create_msg->m_state;
		sendmsg.m_uid1 = create_msg->m_userid;
		sendmsg.m_score1 = create_msg->m_score;

		Lint addSz = msg->m_adds.size();
		for (size_t i = 0; i < addSz; ++i)
		{
			LMsgLMG2LAddToDesk* add_msg = &msg->m_adds[i];
			switch (i)
			{
			case 0:
				sendmsg.m_uid2 = add_msg->m_userid;
				sendmsg.m_score2 = add_msg->m_score;
				break;
			case 1:
				sendmsg.m_uid3 = add_msg->m_userid;
				sendmsg.m_score3 = add_msg->m_score;
				break;
			case 2:
				sendmsg.m_uid4 = add_msg->m_userid;
				sendmsg.m_score4 = add_msg->m_score;
				break;
			default:
				break;
			}
		}

		gWork.SendToMatchServer(sendmsg);

		if (!isSame)
		{
			RecycleDeskId(create_msg->m_deskID);	//通知LOGIC，回收桌子ID
		}
		return;
	}

	handerLMGCreateDesk(create_msg);
	Lint addSz = msg->m_adds.size();
	for (size_t i = 0; i < addSz; ++i)
	{
		LMsgLMG2LAddToDesk* add_msg = &msg->m_adds[i];
		handerLMGAddToDesk(add_msg);
	}
}

void Work::handerLMGSportResetDesk(LMsgLMG2CESportResetDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	Desk* desk = gRoomVip.GetDeskById(msg->m_uDeskid);
	if (!desk)
	{
		LLOG_ERROR("handerLMGSportResetDesk not find deskid %d", msg->m_uDeskid);
		return;
	}

	desk->HanderResetDesk();
}

void Work::HanderMatchInfo(LMsgLMG2LogicMatchInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderMatchInfo %d", msg->m_ID);
		return;
	}

	if (!msg->m_match.m_port)
	{
		return;
	}

	KeepAliveSetting setting = { true, 15000, 5000 };
	MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
	{
		LMsg* msg = ::ParseInsideNetPacket(recv, s);
		if (msg)
			this->Push(msg);
	};

	m_matchInfo = msg->m_match;
	m_matchInfo.m_client.reset(new TCPClient(gInsideNet, m_matchInfo.m_ip, m_matchInfo.m_port, setting, receiver));
	bool succeed = m_matchInfo.m_client->Connect();
	if (succeed)
	{
		LLOG_ERROR("Work::HanderMatchInfo end %s:%d id %d", m_matchInfo.m_ip.c_str(), m_matchInfo.m_port, m_matchInfo.m_id);
	}
	else
	{
		LLOG_ERROR("Since failed to connect Match on %s:%d, stopping... id %d", m_matchInfo.m_ip.c_str(), m_matchInfo.m_port, m_matchInfo.m_id);
	}
}

static void sGResetDesk(Desk* desk)
{
	if (!desk) return;

	if (!gIsMatchType(desk->GetState()))
	{
		desk->ResetEnd();
	}
	else
	{
		desk->HanderResetDesk();
	}
}

bool  Work::CheckMatchRoomAddClear_(LMsgLMG2LMatchAddDesk* msg, bool& isSame)
{
	if (!msg) return false;

	bool ret = true;
	LMsgLMG2LCreateDesk * create_msg = &msg->m_create;
	{
		Desk* desk = gRoomVip.GetDeskById(create_msg->m_deskID);
		if (desk)
		{
			if (desk->m_id == create_msg->m_deskID) isSame = true;
			LLOG_ERROR("match create room repeat create deskid %d state %d now deskid %d", create_msg->m_deskID, desk->GetState(), desk->m_id);
			sGResetDesk(desk); //房间有问题了，也得重置
			ret = false;
		}

		User* pUser = gUserManager.GetUserbyDataId(create_msg->m_userid);
		if (pUser)
		{
			Desk* tmp_desk = pUser->GetDesk();
			if (tmp_desk)
			{
				if (tmp_desk->m_id == create_msg->m_deskID) isSame = true;
				LLOG_ERROR("match create room user have in create deskid %d nowdeskid %d now useid %d userip %s old userid %d  userip %s",
					create_msg->m_deskID, pUser->GetDesk()->m_id, create_msg->m_userid, create_msg->m_ip.c_str(), pUser->GetUserDataId(), pUser->GetIp().c_str());
				sGResetDesk(tmp_desk);
				ret = false;
			}
		}
	}

	Lint addSz = msg->m_adds.size();
	for (size_t i = 0; i < addSz; ++i)
	{
		LMsgLMG2LAddToDesk* add_msg = &msg->m_adds[i];
		User* pUser = gUserManager.GetUserbyDataId(add_msg->m_userid);
		if (pUser)
		{
			Desk* tmp_desk = pUser->GetDesk();
			if (tmp_desk)
			{
				if (tmp_desk->m_id == create_msg->m_deskID) isSame = true;
				LLOG_ERROR("match add room user have in create deskid %d nowdeskid %d now useid %d userip %s old userid %d  userip %s",
					add_msg->m_deskID, pUser->GetDesk()->m_id, add_msg->m_userid, add_msg->m_ip.c_str(), pUser->GetUserDataId(), pUser->GetIp().c_str());
				sGResetDesk(tmp_desk);
				ret = false;
			}
		}
	}

	return ret;
}

///////////////////////////////////////////////////work 部分end//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////desk 部分//////////////////////////////////////////////////////////////
void Desk::HanderResetDesk()
{
	if (onMatchReset_()) //保证不重复解散房间
	{
		LLOG_DEBUG("On HanderResetDesk m_sportForceReset %d", m_id);
		return;
	}
	openOnMatchReset_();
	for (Lint i = 0; i < m_desk_user_count; ++i)
	{
		if (m_user[i] == NULL)
			continue;

		LMsgS2CSportTips sendMsg;
		sendMsg.m_id = EM_STT_SPORT_RESET;
		sendMsg.m_type = EM_STT_SPORT_RESET;
		sendMsg.m_sportId = m_State;
		sendMsg.m_args1 = 0;
		sendMsg.m_args2 = 0;
		sendMsg.m_args3 = 0;
		sendMsg.m_args4 = 0;
		sendMsg.m_args5 = 0;
		m_user[i]->Send(sendMsg);
	}
	LLOG_DEBUG("HanderResetDesk m_sportForceReset %d", m_id);
	ResetEnd();
}

void Desk::SendSportGameResult()
{
	if (m_vip)
	{
		LMsgL2LMGSportResult msg;
		msg.m_logicId = gConfig.GetServerID();
		msg.m_deskid = m_id;
		msg.m_rescode = 0;
		msg.m_desktype = m_vip->m_flag;
		msg.m_uid1 = m_vip->m_posUserId[0];
		msg.m_uid2 = m_vip->m_posUserId[1];
		msg.m_uid3 = m_vip->m_posUserId[2];
		msg.m_uid4 = m_vip->m_posUserId[3];
		msg.m_score1 = m_vip->m_score[0];
		msg.m_score2 = m_vip->m_score[1];
		msg.m_score3 = m_vip->m_score[2];
		msg.m_score4 = m_vip->m_score[3];

		gWork.PushAsynLMsgL2LMGSportResult(msg);
		LLOG_DEBUG("=========LMsgL2LMGSportResult============ %d", m_id);
	}
}

Lint Desk::GetLastOptTime()
{
	if (m_lastOptTime == 0)
	{
		m_lastOptTime = gWork.GetCurTime().Secs();
	}

	return m_lastOptTime;
}

void Desk::UpdateLastOptTimeAndPos(Lint optpos)
{
	m_lastOptTime = gWork.GetCurTime().Secs();
	m_curOptPos = optpos;
}

void Desk::UpdateLastOptTimeAndPos(User* user)
{
	Lint optpos = GetUserPos(user);
	UpdateLastOptTimeAndPos(optpos);
}

void Desk::ClearLastOptTime()
{
	m_lastOptTime = 0;
	m_curOptPos = 0;
}

void Desk::CloseOnMatchReset()
{
	m_bOnReset = false;
}

void Desk::openOnMatchReset_()
{
	m_bOnReset = true;
}

bool Desk::onMatchReset_()
{
	return m_bOnReset;
}

void Desk::SendErrSportGameResult(Lint err)
{
	if (m_vip)
	{
		LMsgL2LMGSportResult msg;
		msg.m_logicId = gConfig.GetServerID();
		msg.m_deskid = m_id;
		msg.m_rescode = err;
		msg.m_desktype = m_vip->m_flag;
		msg.m_uid1 = m_vip->m_posUserId[0];
		msg.m_uid2 = m_vip->m_posUserId[1];
		msg.m_uid3 = m_vip->m_posUserId[2];
		msg.m_uid4 = m_vip->m_posUserId[3];
		msg.m_score1 = m_vip->m_score[0];
		msg.m_score2 = m_vip->m_score[1];
		msg.m_score3 = m_vip->m_score[2];
		msg.m_score4 = m_vip->m_score[3];
		gWork.SendToMatchServer(msg);
	}
}

void Desk::checkMatchWait_(Lint curTime, Lint lastoptTime, Lint inNum)
{
	if (inNum >= m_desk_user_count) //一句玩了好几局，3分钟等待，加机器人
	{
		if (curTime < (lastoptTime + 30))
		{
			return;
		}

		LLOG_DEBUG("CheckMatch DESK_WAIT deskid %d out time %d = %d", m_id, curTime, lastoptTime);
		bool begin = true;
		for (Lint i = 0; i <m_desk_user_count; ++i)
		{
			if (m_user[i] == NULL)
			{
				begin = false;
				break;
			}
		}

		if (begin)
		{
			if (mGameHandler)
			{
				LLOG_DEBUG("CheckMatch DESK_WAIT deskid %d force open", m_id);
				mGameHandler->SetDeskPlay();
			}
		}
		else
		{
			if (curTime >= (lastoptTime + 60 * 3))
			{
				//先发送异常，后续的消息将找不到桌子就不处理了。基本不可能到这里
				LLOG_ERROR("=========SendErrSportGameResult111 =====desk %d == curTime %d = lastoptTime %d ", m_id, curTime, lastoptTime);
				SendErrSportGameResult(1);
				HanderResetDesk();
			}
		}
	}
	else if (inNum < m_desk_user_count)
	{
		LLOG_ERROR("==CheckMatch errr =====");
		for (Lint i = 0; i< m_desk_user_count; ++i)
		{
			if (m_user[i] == NULL)
			{
				LLOG_ERROR("==CheckMatch inNum %d =====", i);
			}
		}
	}
}

void Desk::checkMatchPlayer_(Lint curTime, Lint lastoptTime, Lint inNum)
{
	Lint pos = GetCurOptPos(); //如果是托管，就直接操作
	User* user = GetPosUser(pos);
	if (!user)
	{
		return;
	}

	if (user && user->isManaged())
	{
		mGameHandler->ProcessRobot(pos, user);
		return;
	}

	if (user && (!user->isManaged()))
	{
		if (curTime >= (lastoptTime + 20)) //超时添加托管
		{
			if (!user->isManaged())
			{
				user->attarchManaged();
				user->SendManaged();
				UpdateLastOptTimeAndPos(pos); //一个一个托管
				return;
			}
		}

		if (curTime >= (lastoptTime + 35)) //35秒尚未进比赛场，那么直接结算重新分配
		{
			//先发送异常，后续的消息将找不到桌子就不处理了。
			LLOG_ERROR("=========SendErrSportGameResult=====desk %d ====innum %d == curTime %d = lastoptTime %d ", m_id, inNum, curTime, lastoptTime);
			SendErrSportGameResult(1);
			HanderResetDesk();
		}
	}
}

void  Desk::CheckMatch()
{
	if (!gIsMatchType(GetState())) //不是比赛场不处理
	{
		return;
	}

	if (!mGameHandler)
	{
		return;
	}

	Lint curTime = gWork.GetCurTime().Secs();
	Lint lastoptTime = GetLastOptTime();
	Lint inNum = GetUserCount();

	if (inNum < m_desk_user_count)
	{
		//15秒尚未进比赛场，那么直接结算重新分配
		if (curTime >= (lastoptTime + 15))
		{
			LLOG_ERROR("=========SendErrSportGameResult11=====desk %d ====innum %d == curTime %d = lastoptTime %d ", m_id, inNum, curTime, lastoptTime);
			SendErrSportGameResult(1);
			HanderResetDesk();
			return;
		}
	}

	if (m_deskState == DESK_WAIT)
	{
		checkMatchWait_(curTime, lastoptTime, inNum);
	}
	else
	{
		checkMatchPlayer_(curTime, lastoptTime, inNum);
	}

	//任何情况只要超过5分钟没有操作就直接结算
	if (curTime >= (lastoptTime + 300))
	{
		LLOG_ERROR("=========SendErrSportGameResult22=====desk %d ====innum %d == curTime %d = lastoptTime %d ", m_id, inNum, curTime, lastoptTime);
		SendErrSportGameResult(1);
		HanderResetDesk();
		return;
	}
}

///////////////////////////////////////////////////desk 部分end//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////user 部分//////////////////////////////////////////////////////////////

void User::HanderAttackManaged(LMsgC2SCancelManaged * msg)
{
	cancelManaged();
	SendManaged();
	LLOG_DEBUG("Handler_ATTACK_MANAGED %s == %s %d", m_userData.m_nike.c_str(), m_userData.m_unioid.c_str(), isManaged());
}

void User::CheckSportStatus()
{
	if (GetDesk())
	{
		if (gIsMatchType(GetDesk()->GetState()))
		{
			GetDesk()->OnUserReconnect(this);
		}
	}
}

void User::initPlayGroundData(Lint rank, Lint score, Lint overScore, Lint sportId, Lint process)
{
	m_rank = rank;
	m_score = score;
	m_overScore = overScore;
	m_sportId = sportId;
	m_process = process;
}

void User::clearPlayGroundData()
{
	m_rank = 0;
	m_score = 0;
	m_overScore = 0;
}

void User::SendManaged()
{
	//告诉玩家为托管状态
	LMsgS2CAttachManaged sendmsg;
	sendmsg.m_managed = isManaged();
	Send(sendmsg);
}

///////////////////////////////////////////////////user 部分end//////////////////////////////////////////////////////////////
