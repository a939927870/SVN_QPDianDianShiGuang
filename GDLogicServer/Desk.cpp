#include "Desk.h"
#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "UserManager.h"
Desk::Desk():m_id(0),m_deskState(DESK_FREE),m_deskPlayState(DESK_PLAY_GET_CARD),m_vip(0),m_flag(0)
	,m_resetTime(0),mGameHandler(NULL), m_deskplayer_num(0), m_canInteraction(false), m_nAppId(0), m_aaRoom(0), m_aaRoomCostPlayerCardNum(0)
	,m_nDeskUserCount(DEFAULT_DESK_USER_COUNT),m_antiCheat(false),m_LastWaiteThinkRetTime(0),m_DeskStartPlayTime(0),m_DeskRemainPlayTime(0), m_nTeamID(0),m_freeFlag(0)
	,m_fastPlay(0)
{
	memset(m_user,0,sizeof(m_user));
	memset(m_readyState, 0, sizeof(m_readyState));
	m_playtype.clear();
	ResetClear();
	SpecialCardClear();
}

void Desk::Init()
{
	m_id = 0;
	memset(m_user,0,sizeof(m_user));
	m_deskState = DESK_FREE;
	m_vip = 0;
	memset(m_readyState,0,sizeof(m_readyState));
	m_flag = 0;
	m_playtype.clear();
	ResetClear();
	SpecialCardClear();
	m_resetUser = "";					//申请的玩家
	m_resetTime = 0;					//申请解算的时间
	mGameHandler = NULL;
	m_deskplayer_num = 0;
	LTime curTime;
	m_deskCreatTime = curTime;
	m_nAppId = 0;
	m_aaRoom = 0;
	m_aaRoomCostPlayerCardNum = 0;
	m_fastPlay = 0;
	m_freeFlag = 0;
	m_nTeamID = 0;
}

Desk::~Desk()
{

}

Lint Desk::GetDeskId()
{
	return m_id;
}

void Desk::Tick(LTime& curr)
{
	CheckReset();
	CheckExpired(curr);
	if (!CheckRoom(curr))
	{
		ResetEnd();
	}
	else
	{
		if (mGameHandler)mGameHandler->Tick(*this, curr.MSecs());
	}
}


//申请解算房间
void Desk::HanderResetDesk(User* pUser,LMsgC2SResetDesk* msg)
{
	//重复申请
	if (m_resetTime != 0)
	{
		return;
	}
	if (IsMatchDesk())
	{
		return;
	}

	Lint pos = GetUserPos(pUser);
	if(!IsValidPos(pos))
	{
		return;
	}

	if(!m_vip  || m_deskState == DESK_FREE)
	{
		LLOG_ERROR("Desk::HanderResetDesk error desk free");
		return;
	}

	if (m_deskState == DESK_PLAY)
	{
		memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
		m_reset[pos] = RESET_ROOM_TYPE_AGREE;
		m_resetTime = gWork.GetCurTime().Secs() + 2 * 60;
		m_resetUser = pUser->m_userData.m_nike;

		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				if (m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
	}
	else if (m_deskState == DESK_WAIT)
	{
		if (!m_vip->IsBegin())
		{
			if(pos == 0)
			{
				LMsgS2CResetDesk send;
				send.m_errorCode = 1;
				send.m_applay = pUser->m_userData.m_nike;
				send.m_flag = 1;
				BoadCast(send);
				ResetEnd();
			}
		}
		else
		{
			memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
			m_reset[pos] = RESET_ROOM_TYPE_AGREE;
			m_resetTime = gWork.GetCurTime().Secs() + 5;
			m_resetUser = pUser->m_userData.m_nike;

			for (Lint i = 0; i < m_deskplayer_num; ++i)
			{
				if (m_user[i] == NULL)
					continue;

				LMsgS2CResetDesk send;
				send.m_errorCode = 0;
				send.m_applay = m_resetUser;
				send.m_time = m_resetTime - gWork.GetCurTime().Secs();
				send.m_flag = m_reset[i] ? 1 : 0;
				for (Lint j = 0; j < m_deskplayer_num; ++j)
				{
					if (m_user[j] == NULL)
						continue;
					if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
					{
						send.m_agree.push_back(m_user[j]->m_userData.m_nike);
					}
					else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
					{
						send.m_wait.push_back(m_user[j]->m_userData.m_nike);
					}
					else
					{
						send.m_refluse = m_user[j]->m_userData.m_nike;
					}
				}
				m_user[i]->Send(send);
			}
		}
	}
}

//玩家选择解算操作
void Desk::HanderSelectResetDesk(User* pUser,LMsgC2SSelectResetDesk* msg)
{
	if (pUser == NULL || msg == NULL)
		return;

	if(m_resetTime == 0)
		return;

	if (IsMatchDesk())
	{
		return;
	}

	Lint pos = GetUserPos(pUser);
	if (!IsValidPos(pos))
	{
		return;
	}

	if (msg->m_flag < 1 || msg->m_flag >2)
	{
		msg->m_flag = 1;
	}

	m_reset[pos] = msg->m_flag;

	Lint agree = 0, refluse = 0;
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (m_reset[i] == RESET_ROOM_TYPE_AGREE)
			agree++;
		else if (m_reset[i] == RESET_ROOM_TYPE_REFLUSE)
			refluse++;
	}

	if (refluse >= 1)
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if(m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 4;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				if(m_user[j] == NULL)
					continue;

				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		m_resetTime = 0;
	}
	else if (agree >= GetUserCount() - 1)
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 2;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		ResetEnd();
	}
	else
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
	}
}

bool Desk::CheckRoom(LTime& curr)
{
	if (m_waitChargeTime)
	{  
		if (gWork.GetCurTime().Secs() > m_waitChargeTime && !CanGameStart())
		{
			////处理离开
			//for (Lint i = 0; i<m_deskplayer_num; ++i)
			//{
			//	if (m_user[i] && m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
			//	{
			//		LMsgC2SLeaveDesk msg;
			//		msg.m_pos = i;
			//		msg.m_force = 1;
			//		m_user[i]->HanderUserLeaveDesk(&msg);
			//	}
			//}
			//m_waitChargeTime = gWork.GetCurTime().Secs() + 1;
			//BrocastUserNoGold();
			//m_waitChargeTime = 0;
			for (Lint i = 0; i<m_deskplayer_num; ++i)
			{
				if (m_user[i])
				{
					LMsgS2CBrocastUserNoGold sendMsg;
					sendMsg.m_time = 3;
					sendMsg.m_flag = 2;
					m_user[i]->Send(sendMsg);
				}
			}

			//直接结束
			m_waitChargeTime = 0;
			return false;
		}
	}
	else
	{
		if (m_deskState == DESK_WAIT)
		{
			CheckGameStart();
		}
	}
	return true;
}

void Desk::CheckReset()
{
	if (m_resetTime && gWork.GetCurTime().Secs() > m_resetTime)
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if(m_user[i] == NULL)
				continue; 
			LMsgS2CResetDesk send;
			send.m_errorCode = 3;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				if(m_user[j] == NULL)
					continue;
				if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
				{
					send.m_agree.push_back(m_user[j]->m_userData.m_nike);
				}
				else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
				{
					send.m_wait.push_back(m_user[j]->m_userData.m_nike);
				}
				else
				{
					send.m_refluse = m_user[j]->m_userData.m_nike;
				}
			}
			m_user[i]->Send(send);
		}
		ResetEnd();
	}
}

void Desk::CheckExpired(LTime& curr)
{
	const Lint fourHourSecs = 4 * 3600;
	if (curr.Secs() - m_deskCreatTime.Secs() > fourHourSecs && m_deskState == DESK_WAIT)
	{
		if (m_vip != NULL /*&& !m_vip->IsBegin()*/)
		{
			LMsgS2CResetDesk send;
			send.m_errorCode = 1;
			send.m_flag = 1;
			BoadCast(send);
			ResetEnd();
		}
	}
}

void Desk::HanderUserReady(User* pUser,LMsgC2SUserReady* msg)
{
	LMsgS2CUserReady ready;
	Lint pos = GetUserPos(pUser);

	if(m_deskState != DESK_WAIT)
	{
		LLOG_DEBUG("Desk::HanderUserReady error DESK_PLAY %d",pUser->GetUserDataId());
		return;
	}

	if(!IsValidPos(pos))
	{
		LLOG_ERROR("Desk::HanderUserReady error pos %d", pUser->GetUserDataId());
		return;
	}

	ready.m_pos = pos;
	BoadCast(ready);
	m_readyState[pos] = 1;

	//如果有钱不够，就会被广播钱不够
	if (!CanGameStart())
	{
		LMsgS2CBrocastUserNoGold sendMsg;
		if (m_waitChargeTime <= gWork.GetCurTime().Secs())
		{
			//m_waitChargeTime = gWork.GetCurTime().Secs() + 5 * 60;
			m_waitChargeTime = gWork.GetCurTime().Secs() + 10;
		}
		sendMsg.m_time = m_waitChargeTime - gWork.GetCurTime().Secs();
		sendMsg.m_flag = 1;
		for (Lint i = 0; i<m_deskplayer_num; ++i)
		{
			if (m_user[i] && m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
			{
				if (i == pos)
				{
					sendMsg.m_flag = 0;
				}
				sendMsg.m_wait.push_back(m_user[i]->m_userData.m_nike);
			}
		}
		pUser->Send(sendMsg);
		return;
	}
	CheckGameStart();
}

void Desk::HanderUserOutDesk(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if(!IsValidPos(pos))
	{
		return;
	}

	//谁回来了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 0;
	info.m_pos = pos;
	BoadCastWithOutUser(info, pUser);
}

void Desk::HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg)
{
	if(m_deskPlayState != DESK_PLAY_END_CARD)
	{
		LLOG_DEBUG("Desk::HanderUserEndSelect state error %d:%d:%d",m_deskState,m_deskPlayState,msg->m_flag);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserEndSelect(pUser, msg);
	}
}

void Desk::HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg)
{
	LLOG_DEBUG("Desk::HanderUserStartHu %d:%d",msg->m_type,msg->m_card.size());

	if(m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_START_HU)
	{
		LLOG_DEBUG("Desk::HanderUserStartHu state error %d:%d:%d",m_deskState,m_deskPlayState,msg->m_type);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserStartHu(pUser, msg);
	}
}

void Desk::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
{
	if(m_deskState != DESK_PLAY || (m_deskPlayState != DESK_PLAY_GET_CARD && m_deskPlayState != DESK_PLAY_END_CARD && m_deskPlayState != DESK_PLAY_SEVEN_FLOWER))
	{
		//pUser->Send(sendMsg);
		LLOG_ERROR("Desk::HanderUserPlayCard state error %s,%d",pUser->m_userData.m_nike.c_str(),m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserPlayCard(pUser, msg);
	}
}

void Desk::HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg)
{
	if (m_deskState != DESK_PLAY || m_deskPlayState != DESK_PLAY_THINK_CARD)
	{
		LLOG_ERROR("Desk::HanderUserOperCard state error %s,%d", pUser->m_userData.m_nike.c_str(), m_deskState);
		return;
	}
	if (mGameHandler)
	{
		mGameHandler->HanderUserOperCard(pUser, msg);
	}
}

void Desk::HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg)
{
	Lint pos = GetUserPos(pUser);
	if (!IsValidPos(pos))
	{
		return;
	}

	LMsgS2CUserSpeak speak;
	speak.m_userId = m_user[pos]->GetUserDataId();
	speak.m_pos = pos;
	speak.m_id = msg->m_id;
	speak.m_type = msg->m_type;
	speak.m_musicUrl = msg->m_musicUrl;
	speak.m_msg = msg->m_msg;
	BoadCast(speak);
}

void Desk::HanderUserInteraction(User* pUser, LMsgC2SInteraction* msg)
{
	Lint pos = GetUserPos(pUser);
	if (!IsValidPos(pos))
	{
		return;
	}

	if (nullptr == msg)
	{
		return;
	}

	LMsgS2CInteraction interaction;
	interaction.m_sendPos = msg->m_sendPos;
	interaction.m_targetPos = msg->m_targetPos;
	interaction.m_interactionType = msg->m_interactionType;
	BoadCast(interaction);
}

void Desk::BrocastUserNoGold(Lint exPos)
{
	if (!m_vip->IsBegin())
	{
		return;
	}

	if (!CanGameStart())
	{
		for (Lint j = 0; j < m_deskplayer_num; ++j)
		{
			if (m_user[j] == NULL)
				continue;
			if (j == exPos) continue;
			
			LMsgS2CBrocastUserNoGold sendMsg;
			if (m_waitChargeTime <= gWork.GetCurTime().Secs())
			{
				//m_waitChargeTime = gWork.GetCurTime().Secs() + 5 * 60;
				m_waitChargeTime = gWork.GetCurTime().Secs() + 10;
			}
			sendMsg.m_time = m_waitChargeTime - gWork.GetCurTime().Secs();
			sendMsg.m_flag = 1;
			for (Lint i = 0; i<m_deskplayer_num; ++i)
			{
				if (m_user[i] && m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
				{
					if (i == j)
					{
						sendMsg.m_flag = 0;
					}
					sendMsg.m_wait.push_back(m_user[i]->m_userData.m_nike);
				}
			}

			if (sendMsg.m_flag == 1 && m_deskplayer_num != GetUserCount())
			{
				sendMsg.m_flag = 2;
			}
			m_user[j]->Send(sendMsg);
		}
	}
}

bool Desk::OnUserReconnect(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if (!IsValidPos(pos))
	{
		LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
		return false;
	}

	m_readyState[pos] = 1;

	//把自己加进来
	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	send1.m_pos = pos;
	send1.m_ready = m_readyState[pos];
	send1.m_score = m_vip->GetUserScore(pUser);
	send1.m_flag = m_flag;
	send1.m_maxCircle = m_vip->m_maxCircle;
	send1.m_playtype = m_playtype;
	send1.m_canInteraction = m_canInteraction;
	send1.m_antiCheat = m_antiCheat;
	send1.m_curMatchId = m_vip->m_curMatchId;
	send1.m_playerNum = m_deskplayer_num;
	send1.m_aaRoom = m_aaRoom;
	send1.m_nFastPlay = m_fastPlay;
	send1.m_nProportionate = GetProportionate();
	send1.m_nCircleGoldLimit = GetCircleGoldLimit();
	send1.m_nStake = GetStake();
	pUser->Send(send1);

 	//把其他人加进来
 	for(Lint i = 0 ;i < m_deskplayer_num; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != pUser)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId();
 			addmsg2.m_pos = i;
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_score = m_vip->GetUserScore(m_user[i]);
			addmsg2.m_location = m_user[i]->GetLocation();
			//addmsg2.m_curHeadIconId = m_user[i]->m_userData.m_curHeadIconId;
 			pUser->Send(addmsg2);
 		}
 	}

	//谁回来了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 1;
	info.m_pos = pos;
	BoadCastWithOutUser(info, pUser);

	//这里有人申请解算房间
	if (m_resetTime)
	{
		LMsgS2CResetDesk send;
		send.m_errorCode = 0;
		send.m_applay = m_resetUser;
		send.m_time = m_resetTime - gWork.GetCurTime().Secs();
		send.m_flag = m_reset[pos] ? 1 : 0;
		for (Lint j = 0; j < m_deskplayer_num; ++j)
		{
			if(m_user[j] == NULL)
					continue; 
			if (m_reset[j] == RESET_ROOM_TYPE_AGREE)
			{
				send.m_agree.push_back(m_user[j]->m_userData.m_nike);
			}
			else if (m_reset[j] == RESET_ROOM_TYPE_WAIT)
			{
				send.m_wait.push_back(m_user[j]->m_userData.m_nike);
			}
			else
			{
				send.m_refluse = m_user[j]->m_userData.m_nike;
			}
		}
		pUser->Send(send);
	}

	//不在游戏中，不需要后面的数据
	if(m_deskState == DESK_WAIT)
	{
		CheckGameStart();
		if (!CanGameStart())
		{
			LMsgS2CBrocastUserNoGold sendMsg;
			if (m_waitChargeTime <= gWork.GetCurTime().Secs())
			{
				m_waitChargeTime = gWork.GetCurTime().Secs() + 5 * 60;
			}
			sendMsg.m_time = m_waitChargeTime - gWork.GetCurTime().Secs();
			sendMsg.m_flag = 1;
			for (Lint i = 0; i<m_deskplayer_num; ++i)
			{
				if (m_user[i] && m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
				{
					if (i == pos)
					{
						sendMsg.m_flag = 0;
					}
					sendMsg.m_wait.push_back(m_user[i]->m_userData.m_nike);
				}
			}
			pUser->Send(sendMsg);
		}
		return true;
	}

	//发送当前圈数信息
	if(m_vip)
		m_vip->SendInfo();
	if (mGameHandler)
	{
		mGameHandler->OnUserReconnect(pUser);
	}

	//重新发送托管消息
	/*
	LMsgS2C_TuoGuanInfo tuoGuanInfo;
	tuoGuanInfo.m_nPos = GetUserPos(pUser);
	tuoGuanInfo.m_nType = pUser->GetTuoGuan();
	pUser->Send(tuoGuanInfo);
	*/
	SetMatchDeskPlay();
	return true;
}

void Desk::SetVip(VipLogItem* vip)
{
	m_vip = vip;
	if(m_vip)
	{
		m_flag = vip->m_flag;
		m_playtype = vip->m_playtype;
	}
}

VipLogItem* Desk::GetVip()
{
	return m_vip;
}

Lint Desk::GetFreePos()
{
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (m_user[i] == NULL)
			return i;
	}

	return m_deskplayer_num;

}

void Desk::OnUserInRoom(User* user)
{
	if (NULL == user)
	{
		LLOG_ERROR("Desk::OnUserInRoom User is null");
		return;
	}

	if (!IsMatchDesk() && user->m_userData.m_nAppId != m_nAppId)
	{
		LLOG_ERROR("Desk::OnUserInRoom AppId error, user=%d, appid=%d, desk appid=%d", user->GetUserDataId(), user->m_userData.m_nAppId, m_nAppId);
		return;
	}

	if (GetUserPos(user) != m_deskplayer_num)
	{
		LLOG_ERROR("Desk::OnUserInRoom is in desk %d", user->GetUserDataId());
		return;
	}

	Lint pos = GetFreePos();
	if(pos == m_deskplayer_num)
	{
		LLOG_ERROR("Desk::OnUserInRoom DEFAULT_INVAILD_POS pos:%d, m_deskplayer_num %d, UserDataId %d", 
			pos, m_deskplayer_num, user->GetUserDataId());
		return;
	}

	m_user[pos] = user;
	user->SetDesk(this);
	m_readyState[pos] = 1;

	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	send1.m_pos = pos;
	send1.m_ready = m_readyState[pos];
	send1.m_score = m_vip->GetUserScore(user);
	send1.m_flag = m_flag;
	send1.m_maxCircle = m_vip->m_maxCircle;
	send1.m_playtype = m_playtype;
	send1.m_canInteraction = m_canInteraction;
	send1.m_antiCheat = m_antiCheat;
	send1.m_curMatchId = m_vip->m_curMatchId;
	send1.m_playerNum = m_deskplayer_num;
	send1.m_aaRoom = m_aaRoom;
	send1.m_nFastPlay = m_fastPlay;
	send1.m_nProportionate = GetProportionate();
	send1.m_nCircleGoldLimit = GetCircleGoldLimit();
	send1.m_nStake = GetStake();
	user->Send(send1);

 	LMsgS2CDeskAddUser addmsg1;
 	addmsg1.m_userId = user->GetUserDataId();
 	addmsg1.m_score = m_vip->GetUserScore(user);
	addmsg1.m_pos = pos;
 	addmsg1.m_nike = user->m_userData.m_nike;
 	addmsg1.m_ready = m_readyState[pos];
 	addmsg1.m_sex = user->m_userData.m_sex;
 	addmsg1.m_ip = user->GetIp();
 	addmsg1.m_face = user->m_userData.m_headImageUrl;
	addmsg1.m_location = user->GetLocation();
	//addmsg1.m_curHeadIconId = user->m_userData.m_curHeadIconId;
 	
 	for(Lint i = 0 ;i < m_deskplayer_num; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != user)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId();
 			addmsg2.m_pos = i;
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_score = m_vip->GetUserScore(m_user[i]);
			addmsg2.m_location = m_user[i]->GetLocation();
			//addmsg2.m_curHeadIconId = m_user[i]->m_userData.m_curHeadIconId;
 			user->Send(addmsg2);
 			m_user[i]->Send(addmsg1);
 		}
 	}

	LMsgS2C_TuoGuanInfo tuoGuanInfo;
	tuoGuanInfo.m_nPos = GetUserPos(user);
	tuoGuanInfo.m_nType = user->GetTuoGuan();
	user->Send(tuoGuanInfo);

	CheckGameStart();
}

void Desk::OnUserOutRoom(User* user)
{
 	Lint pos = GetUserPos(user);
 	if(!IsValidPos(pos))
 	{
 		LLOG_ERROR("Desk::OnUserOutRoom DEFAULT_INVAILD_POS %d",user->GetUserDataId());
 		return;
 	}
 
 	LMsgS2CDeskDelUser del;
 	del.m_pos = pos;	
 	for(Lint i = 0 ;i < m_deskplayer_num; ++i)
 	{
 		if(m_user[i] != NULL)
 		{
 			m_user[i]->Send(del);
 		}
	}
	//修改状态
	user->setUserState(LGU_STATE_CENTER, m_fastPlay);
	user->ModifyUserState(true);

	//清除状态
	user->SetDesk(NULL);
 	m_user[pos] = NULL;
	m_readyState[pos] = 0;

	//删除玩家 不删除机器人
	if (!user->IsRobot())		//不是机器人删除掉
	{
		UserManager::Instance().DelUser(user);
		delete user;
	}

}

Lint Desk::GetNextPos(Lint prev)
{
	return prev == m_deskplayer_num -1 ? 0:prev+1;
}

Lint Desk::GetPrevPos(Lint next)
{
	return next ==0 ? m_deskplayer_num -1:next-1;
}

Lint Desk::GetUserPos(User* user)
{
	for(Lint i = 0;i<m_deskplayer_num;++i)
	{
		if(m_user[i] == user)
			return i;
	}

	return m_deskplayer_num;
}

User* Desk::GetPosUser(Lint pos)
{
	if(pos<0 || pos >= m_deskplayer_num)
	{
		return NULL;
	}

	return m_user[pos];
}

Lint Desk::GetUserCount()
{
	Lint cnt = 0;
	for(Lint i = 0; i < m_deskplayer_num;++i)
	{
		if(m_user[i])
		{
			cnt += 1;
		}
	}

	return cnt;
}

void Desk::SetDeskWait()
{
	memset(m_readyState, 0, sizeof(m_readyState));
	m_deskState = DESK_WAIT;
}

void Desk::OnDeskWait()
{
	
}

void Desk::CheckGameStart()
{
	if(!m_vip->IsBegin())
	{
		if (GetUserCount() == m_deskplayer_num)
		{
			if (mGameHandler)
			{
				m_DeskStartPlayTime = gWork.GetCurTime().MSecs();
				m_DeskRemainPlayTime = m_vip->m_nForceFinishTime;
				m_vip->SetCurrDifen();
				mGameHandler->SetDeskPlay();
				SetMatchDeskPlay();
			}
		}
	}
	else
	{
		bool begin = true;
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (m_user[i] == NULL || !m_readyState[i])
			{
				begin = false;
				break;
			}
		}

		if (begin)
		{
			if (mGameHandler && CanGameStart())
			{
				m_DeskStartPlayTime = gWork.GetCurTime().MSecs();
				m_DeskRemainPlayTime = m_vip->m_nForceFinishTime;
				m_vip->SetCurrDifen();
				mGameHandler->SetDeskPlay();
				ResetTuoGuan();
				SetMatchDeskPlay();
			}
		}
	}
}

bool Desk::CanGameStart()
{
	for (Lint i = 0; i<m_deskplayer_num; ++i)
	{
		if (m_user[i] && m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
		{
			return false;
		}
	}
	return true;
}

void  Desk::BrocastUserNoGold()
{
	if (m_waitChargeTime <= gWork.GetCurTime().Secs())
	{
		return;
	}
	Lint t_time = m_waitChargeTime - gWork.GetCurTime().Secs();
	std::vector<Lstring> waitPlayer;
	Lint nownum = 0;
	for (Lint i = 0; i<m_deskplayer_num; ++i)
	{
		if (m_user[i] && m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
		{
			waitPlayer.push_back(m_user[i]->m_userData.m_nike);
		}

		if (m_user[i])
		{
			++nownum;
		}
	}

	if (nownum == m_deskplayer_num && !waitPlayer.size())
	{
		return;
	}

	for (Lint i = 0; i<m_deskplayer_num; ++i)
	{
		LMsgS2CBrocastUserNoGold sendMsg;
		sendMsg.m_time = t_time;
		sendMsg.m_flag = 1;
		if (nownum != m_deskplayer_num)
		{
			sendMsg.m_flag = 2;
		}
		sendMsg.m_wait.assign(waitPlayer.begin(), waitPlayer.end());

		if (m_user[i])
		{
			if (m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
			{
				sendMsg.m_flag = 0;
			}
			m_user[i]->Send(sendMsg);
		}
	}
}

void Desk::SetDeskFree()
{
	m_deskState = DESK_FREE;
	if (mGameHandler)
	{
		GameHandlerFactory::getInstance()->destroy((GameType)m_flag,mGameHandler);
	}
	
	Lint userId = 0;
	if(m_user[0] != NULL)
	{
		userId = m_user[0]->m_userData.m_id;
	}
	gWork.RecycleDeskId(m_id, userId, GetMatchID());
}

void Desk::OnDeskFree()
{

}

void Desk::BoadCast(LMsg& msg)
{
	for(Lint i = 0 ; i < m_deskplayer_num;++i)
	{
		if(m_user[i])
			m_user[i]->Send(msg);
	}
}

void Desk::BoadCastWithOutUser(LMsg& msg, User* user)
{
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (m_user[i] && m_user[i] != user)
			m_user[i]->Send(msg);
	}
}

void Desk::ClearUser()
{
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (m_user[i])
			OnUserOutRoom(m_user[i]);
	}
}


void Desk::SetAllReady()
{
	memset(m_readyState, 1, sizeof(m_readyState));
}

void Desk::ResetClear()
{
	m_resetTime = 0;
	m_resetUser.clear();
	memset(m_reset, 0, sizeof(m_reset));
}

void Desk::ResetEnd()
{
	if (m_vip)
	{
		m_vip->m_reset = 1;
	}
	
	if (m_deskState == DESK_PLAY)
	{
		if (mGameHandler)
		{
			Lint m_playerHuInfo[DEFAULT_DESK_USER_COUNT] = {0};
			mGameHandler->OnGameOver(WIN_NONE, m_playerHuInfo, DEFAULT_INVAILD_POS, NULL);
		}
	}
	else
	{
		if(m_vip && m_vip->IsBegin())
		{
			m_vip->SendEnd();
			m_vip->m_desk = NULL;
		}
		SetDeskFree();
		ClearUser();
		SetVip(NULL);
		ResetClear();
	}
}

void Desk::SpecialCardClear()
{
	memset(m_specialCard,0,sizeof(m_specialCard));
}

bool Desk::IsMatchDesk()
{
	return m_vip && m_vip->m_curMatchId != 0;
}

int Desk::GetMatchID()
{
	if (m_vip)
	{
		return  m_vip->m_curMatchId;
	}
	return 0;
}

void Desk::SetDeskPlayerNum(Lint flag)
{
	Lint playerNum = 0;
	
	if (flag == HuNanMaJing ||
		flag == GuangDongJiPingHuMaJing ||
		flag == GuangDongTuiDaoHuMaJing ||
		flag == GuangDongOneHundred ||
		flag == HeYuanJokerChickenHu ||
		flag == GuangDongCustomTuiDaoHu ||
		flag == GuangDongChaoZhou ||
		flag == HuizhouzhuangGhost ||
		flag == RedDragon ||
		flag == Hongkong ||
		flag == HuizhouzhuangFlower ||
		flag == JiHuFlower || 
		flag == GuangDongHongZhongMajiang ||
		flag == GuangDongJihu ||
		flag == MeizhouHongzhonbao ||
		flag == GuangdongJieyang ||
		flag == Shaoguan ||
		flag == Nanxiaong || 
		flag == Qujiang ||
		flag == Maba||
		flag == GuangdongJieXiMajiang||
		flag == ChaoShanBiHu||
		flag == GuangDongRaoping||
		flag == LongMenZiyouzhuang||
		flag == HuiYangWanFa
		)
	{
		playerNum = 4;
	}
	else if (flag == ThreePlayerTuiDaoHu)
	{
		playerNum = 3;
	}
	else if (flag == TwoPlayerTuiDaoHu)
	{
		playerNum = 2;
	}
	
	m_deskplayer_num = playerNum;
}

void Desk::SetDeskPlayerNum(Lint flag, Lint playerNum)
{
	if (playerNum > 1 && playerNum < 5)
		m_deskplayer_num = playerNum;
	else Desk::SetDeskPlayerNum(flag);
}

void Desk::SetDeskAppId(Lint nAppId)
{
	m_nAppId = nAppId;
}

Lint Desk::GetDeskAppId() const
{
	return m_nAppId;
}

Lint Desk::GetDeskPlayerNum() const
{
	return m_deskplayer_num;
}

bool Desk::IsAaRoom() const
{
	return m_aaRoom == 1;
}

void Desk::SetAaRoomCostPlayerCardNum(Lint costCardNum)
{
	m_aaRoomCostPlayerCardNum = costCardNum;
}

Lint Desk::GetAaRoomCostPlayerCardNum() const
{
	return m_aaRoomCostPlayerCardNum;
}

bool Desk::isCheat(User* puser) {
	if (!puser) {
		return false;
	}
	for (Lint i = 0; i < this->m_deskplayer_num; i++) {
		if (m_user[i] && puser->GetIp() == m_user[i]->GetIp()) {
			return true;
		}
	}
	return false;
}
void Desk::SetDeskCanInteraction(Lint canInteraction)
{
	m_canInteraction = canInteraction;
}

Lint Desk::GetDeskCanInteraction() const
{
	return m_canInteraction;
}

bool Desk::IsValidPos(Lint pos)
{
	bool result= (pos >= 0 && pos < m_deskplayer_num); 
	if (!result)
	{
		LLOG_ERROR("InValid Desk Position %d", pos);
	}
	return result;
}


bool Desk::isNewPlay()
{
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (!m_user[i])
		{
			continue;
		}

		if (m_user[i]->IsRobot())
		{
			return true;
		}
	}

	return false;
}

//转换分数到玩家
void Desk::SwitchScoreToUser_(Ldouble gold[4])
{
	if (!m_vip) return;

	//先把输的钱给扣了
	Ldouble totolwinscore = 0;
	Ldouble totolcostgold = 0;
	Lint winCount = 0;

	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (!m_user[i])
		{
			continue;
		}

		Lint pos = GetUserPos(m_user[i]);
		Ldouble chargeToCount = m_vip->GetUserScore(m_user[i]);
		if (chargeToCount < 0) chargeToCount = 0;
		Ldouble nowCount = m_user[i]->m_userData.m_numOfCard2s;
		if (chargeToCount > nowCount) //抽成
		{
			Ldouble realCost = chargeToCount - nowCount;
			totolwinscore += realCost;
			++winCount;
		}
		else if (chargeToCount < nowCount)
		{
			Ldouble failcost = nowCount - chargeToCount;
			m_user[i]->ModifyRoomCardCount(-failcost, CARDS_OPER_TYPE_ROOM_FAIL_CARD, m_flag);
			gold[pos] -= failcost;
			m_vip->SetUserScore(m_user[i], chargeToCount);
			totolcostgold += failcost;
		}
	}

	//把赢家的数据回滚
	if (totolcostgold == 0 || totolwinscore == 0 || winCount == 0)
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (!m_user[i])
			{
				continue;
			}

			Ldouble nowCount = m_user[i]->m_userData.m_numOfCard2s;
			m_vip->SetUserScore(m_user[i], nowCount);
		}
		return;
	}

	//如果只是一个人赢，就直接给分
	if (winCount == 1)
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (!m_user[i])
			{
				continue;
			}

			Lint pos = GetUserPos(m_user[i]);
			Ldouble chargeToCount = m_vip->GetUserScore(m_user[i]);
			Ldouble nowCount = m_user[i]->m_userData.m_numOfCard2s;
			if (chargeToCount > nowCount) //抽成
			{
				Ldouble realCost = totolcostgold;
				m_user[i]->ModifyRoomCardCount(realCost, CARDS_OPER_TYPE_ROOM_WIN_CARD, m_flag);
				gold[pos] += realCost;
				Ldouble needProprt = realCost * GetProportionate() / 100;
				if (needProprt /*&& !GetFreeFlag()*/)
				{
					m_user[i]->ModifyRoomCardCount(-needProprt, CARDS_OPER_TYPE_ROOM_COST_CARD, m_flag);
					gold[pos] -= needProprt;
				}
				m_vip->SetUserScore(m_user[i], nowCount + (realCost - needProprt));
				break;
			}
		}
	}
	else
	{
		//按照分算，如果可以以分数区分，就直接按照分数给钱
		if (totolwinscore == 0) totolwinscore = 1;
		double perScoreCost = (double)totolcostgold / (double)totolwinscore;
		Ldouble totolCalScore = 0;

		Lint maxIdx = 0;
		Lint maxGold = 0;
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (!m_user[i])
			{
				continue;
			}

			Lint pos = GetUserPos(m_user[i]);
			Ldouble chargeToCount = m_vip->GetUserScore(m_user[i]);
			Ldouble nowCount = m_user[i]->m_userData.m_numOfCard2s;
			if (chargeToCount > nowCount) //抽成
			{
				Ldouble tmp_count = (chargeToCount - nowCount);
				if (tmp_count > maxGold)
				{
					maxIdx = i;
					maxGold = tmp_count;
				}

				Ldouble realCost = floor((double)tmp_count * perScoreCost);
				if (realCost)
				{
					totolCalScore += realCost;
					m_user[i]->ModifyRoomCardCount(realCost, CARDS_OPER_TYPE_ROOM_WIN_CARD, m_flag);
					gold[pos] += realCost;
					Ldouble needProprt = realCost * GetProportionate() / 100;
					if (needProprt /*&& !GetFreeFlag()*/)
					{
						m_user[i]->ModifyRoomCardCount(-needProprt, CARDS_OPER_TYPE_ROOM_COST_CARD, m_flag);
						gold[pos] -= needProprt;
					}
					m_vip->SetUserScore(m_user[i], nowCount + (realCost - needProprt));
				}
			}
		}

		if (totolcostgold > totolCalScore)
		{
			Lint pos = GetUserPos(m_user[maxIdx]);
			Ldouble nowCount = m_user[maxIdx]->m_userData.m_numOfCard2s;
			Ldouble realCost = totolcostgold - totolCalScore;
			gold[pos] += realCost;
			m_user[maxIdx]->ModifyRoomCardCount(realCost, CARDS_OPER_TYPE_ROOM_WIN_CARD_EXADD, m_flag);
			m_vip->SetUserScore(m_user[maxIdx], nowCount + (realCost - 0));
		}
	}
}

void Desk::DelCard(Ldouble gold[4])
{
	if (!m_vip)
	{
		return;
	}

	if (!isNewPlay())
	{
		SwitchScoreToUser_(gold);
		return;
	}

	//机器人恢复
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (!m_user[i])
		{
			continue;
		}

		if (m_user[i]->IsRobot())
		{
			m_vip->SetUserScore(m_user[i], 0.0f);
		}
		else
		{
			Lint pos = GetUserPos(m_user[i]);
			Ldouble nowCount = m_user[i]->m_userData.m_numOfCard2s;
			m_vip->SetUserScore(m_user[i], nowCount);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool Desk::CreateRegister(GameType gameType)
{
	mGameHandler = GameHandlerFactory::getInstance()->create(
		gameType);

	if (!mGameHandler)
	{
		LLOG_ERROR("No game handler for game type %d found.", gameType);
		return false;
	}
	mGameHandler->startup(this);
	return true;
}

Lint Desk::getGameType()
{
	return m_flag;
}

std::vector<Lint>& Desk::getPlayType()
{
	return m_playtype;
}

Lint Desk::getDeskState()
{
	return m_deskState;
}

void Desk::setDeskState(Lint nState)
{
	m_deskState = nState;
}

Lint Desk::getDeskPlayState()
{
	return m_deskPlayState;
}

void Desk::setDeskPlayState(Lint nDeskPlayState)
{
	m_deskPlayState = nDeskPlayState;
}

void Desk::HanderGameOver()
{
	if (m_vip == NULL)
	{
		return;
	}

	if(m_vip->isEnd())
	{
		m_vip->SendEnd();
		m_vip->m_desk = NULL;
		SetDeskFree();
		ClearUser();
		SetVip(NULL);
		ResetClear();
		LLOG_DEBUG("Game over, reset desk!");
	}

	////玩家中马送礼券
	//LMsgL2LMGUserCircleIncreased msg;
	//for (Lint i = 0; i < m_deskplayer_num; ++i)
	//{
	//}
	//gWork.SendToLogicManager(msg);
	
}

void Desk::HanderUserTuoGuan(User* pUser, LMsgC2S_TuoGuan* msg)
{
	LMsgS2C_TuoGuanInfo info;
	info.m_nPos = GetUserPos(pUser);
	info.m_nType = pUser->GetTuoGuan();
	BoadCast(info);
	//m_LastWaiteThinkRetTime = gWork.GetCurTime().MSecs();
}

void Desk::SetMatchDeskPlay()
{
	if (!m_vip) return;
	if (m_vip->m_curMatchId == 0) return;
	for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
	{
		LMsgS2C_TuoGuanInfo tuoGuanInfo;
		tuoGuanInfo.m_nPos = GetUserPos(m_user[i]);
		tuoGuanInfo.m_nType = m_user[i]->GetTuoGuan();
		m_user[i]->Send(tuoGuanInfo);
	}
}

void Desk::ResetTuoGuan()
{
	if (!m_vip) return;
	if (m_vip->m_curMatchId == 0) return;
	for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
	{
		if (m_user[i])
		{
			if (m_user[i]->GetTuoGuan())
			{
				m_user[i]->SetTuoGuan(false);
				LMsgS2C_TuoGuanInfo msg;
				msg.m_nPos = GetUserPos(m_user[i]);
				msg.m_nType = m_user[i]->GetTuoGuan();
				BoadCast(msg);
			}
		}
	}
}

void Desk::SetTeamID(Lint nTeamID)
{
	m_nTeamID = nTeamID;
}

void Desk::SetTeamName(Lstring strTeamName)
{
	m_strTeamName = strTeamName;
}
void Desk::FastPlayResetDesk()
{
	bool del = true;
	for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
	{
		if (m_vip->m_posUserId[i])
		{
			del = false;
			break;
		}
	}
	if (del)
	{
		ResetEnd();
	}
}
//////////////////////////////////////////////////////////////////////////
