#include "Desk.h"
#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "UserManager.h"
Desk::Desk():m_desk_user_count(4)
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
	m_State = 0;
	m_lastOptTime = 0;
	m_bOnReset = false;
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
	m_State = 0;
	m_lastOptTime = 0;
	m_bOnReset = false;
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
	CheckMatch();

	if (!CheckRoom(curr))
	{
		ResetEnd();
	}
}


//申请解算房间
void Desk::HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg)
{
	//重复申请
	if (m_resetTime != 0)
	{
		return;
	}

	if (gIsMatchType(m_flag))
	{
		LLOG_ERROR("==Desk::HanderResutDesk userid %d type %d deskid %d", pUser->GetUserDataId(), m_flag, m_id);
		return;
	}

	Lint pos = GetUserPos(pUser);
	if(pos == INVAILD_POS)
	{
		return;
	}

	if(!m_vip  || m_deskState == DESK_FREE)
	{
		LLOG_ERROR("Desk::HanderResutDesk error desk free %d userid %d", m_id, pUser->GetUserDataId());
		return;
	}

	if (m_deskState == DESK_PLAY)
	{
		memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
		m_reset[pos] = RESET_ROOM_TYPE_AGREE;
		m_resetTime = gWork.GetCurTime().Secs() + 2 * 60;
		m_resetUser = pUser->m_userData.m_nike;

		
		for (Lint i = 0; i < m_desk_user_count; ++i)
		{
			if (m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_desk_user_count; ++j)
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
			LLOG_DEBUG("==LMsgS2CResetDesk8 %d %d userid %d deskid %d", m_user[i]->m_userData.m_id, m_flag, pUser->GetUserDataId(), m_id);
		}
	}
	else if (m_deskState == DESK_WAIT)
	{
		if (!m_vip->IsBegin())
		{
			if(pos == 0)
			{
				LLOG_DEBUG("==LMsgS2CResetDesk %d %d userid %d deskid %d", pUser->m_userData.m_id, m_flag, pUser->GetUserDataId(), m_id);
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

			for (Lint i = 0; i < m_desk_user_count; ++i)
			{
				if (m_user[i] == NULL)
					continue;

				LMsgS2CResetDesk send;
				send.m_errorCode = 5;
				send.m_applay = m_resetUser;
				send.m_time = m_resetTime - gWork.GetCurTime().Secs();
				send.m_flag = m_reset[i] ? 1 : 0;
				for (Lint j = 0; j < m_desk_user_count; ++j)
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
				LLOG_DEBUG("==LMsgS2CResetDesk2 %d %d deskid %d", m_user[i]->GetUserDataId(), m_flag, m_id);
			}
		}
	}
}

//玩家选择解算操作
void Desk::HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg)
{
	if(m_resetTime == 0)
		return;

	if (gIsMatchType(m_flag))//比赛场没有玩家结算
	{
		LLOG_DEBUG("==HanderSelectResutDesk desk id %d flag %d userid %d", m_id, m_flag, pUser->GetUserDataId());
		return;
	}

	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		return;
	}

	if (msg->m_flag < 1 || msg->m_flag >2)
	{
		msg->m_flag = 1;
	}

	m_reset[pos] = msg->m_flag;

	Lint agree = 0, refluse = 0;
	for (Lint i = 0; i < m_desk_user_count; ++i)
	{
		if (m_reset[i] == RESET_ROOM_TYPE_AGREE)
			agree++;
		else if (m_reset[i] == RESET_ROOM_TYPE_REFLUSE)
			refluse++;
	}

	if (refluse >= 1)
	{
		for (Lint i = 0; i < m_desk_user_count; ++i)
		{
			if(m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 4;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_desk_user_count; ++j)
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
			LLOG_DEBUG("==LMsgS2CResetDesk3 %d %d userid %d deskid %d", m_user[i]->m_userData.m_id, m_flag, pUser->GetUserDataId(), m_id);
		}
		ResetClear();
	}
	else if (agree >= (m_desk_user_count==2?2:3))
	{
		for (Lint i = 0; i < m_desk_user_count; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 2;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_desk_user_count; ++j)
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
			LLOG_DEBUG("==LMsgS2CResetDesk4 %d %d userid %d deskid ", m_user[i]->m_userData.m_id, m_flag, pUser->GetUserDataId(), m_id);
		}
		ResetEnd();
	}
	else
	{
		for (Lint i = 0; i < m_desk_user_count; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j <  m_desk_user_count; ++j)
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
			LLOG_DEBUG("==LMsgS2CResetDesk5 %d %d", m_user[i]->m_userData.m_id, m_flag);
		}
	}
}

void Desk::HanderUserSelectPiao(User * pUser, LMsgC2SPiaoSelect * msg)
{
	Lint pos = GetUserPos(pUser);
	if (pos == msg->m_position)
	{
		if (mGameHandler)
		{
			mGameHandler->HanderUserChoosePiao(pUser, msg);
		}
	}
	else {
		LLOG_ERROR("Desk::HanderRepFlowerInfo error position");
	}
}

void Desk::SetState(Lint state)
{
	m_State = state;
}

Lint Desk::GetState()
{
	return m_State;
}

void Desk::CheckReset()
{
	if (m_resetTime && gWork.GetCurTime().Secs() > m_resetTime)
	{
		for (Lint i = 0; i < m_desk_user_count; ++i)
		{
			if(m_user[i] == NULL)
				continue; 
			LMsgS2CResetDesk send;
			send.m_errorCode = 3;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_desk_user_count; ++j)
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
			if(!gIsMatchType(m_flag))
				m_user[i]->Send(send);
			LLOG_DEBUG("==LMsgS2CResetDesk6 %d %d", m_user[i]->m_userData.m_id, m_flag);
		}
		ResetEnd();
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

	if(pos == INVAILD_POS)
	{
		LLOG_DEBUG("Desk::HanderUserReady error pos %d", pUser->GetUserDataId());
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
		for (Lint i = 0; i<m_desk_user_count; ++i)
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
	if(pos == INVAILD_POS)
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
	LLOG_DEBUG("Desk::HanderUserStartHu %d",msg->m_type);

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
	if(m_deskState != DESK_PLAY || (m_deskPlayState != DESK_PLAY_GET_CARD && DESL_PLAY_XIA_BU != m_deskPlayState))
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

void Desk::HanderUserOperCard(User* pUser, LMsgC2SUserOper* msg)
{
	if (m_deskState != DESK_PLAY || (m_deskPlayState != DESK_PLAY_THINK_CARD && DESK_PLAY_BAO_TING != m_deskPlayState))
	{
		LLOG_DEBUG("Desk::HanderUserOperCard state error %s,%d", pUser->m_userData.m_nike.c_str(), m_deskState);
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
	if (pos == INVAILD_POS)
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

bool Desk::OnUserReconnect(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
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
	
	if (gIsMatchType(m_flag))
	{
		send1.m_flag = 0;
	}
	else
	{
		send1.m_flag = m_flag;
	}
	send1.m_maxCircle = m_vip->m_maxCircle;
	send1.m_playtype = m_playtype;

	send1.m_rank = pUser->m_rank;
	send1.m_overScore = pUser->m_overScore;
	send1.m_sportId = pUser->m_sportId;
	send1.m_process = pUser->m_process;

	LLOG_DEBUG("===User::OnUserReconnect userid %d deskid %d score %d", pUser->GetUserDataId(), m_id, send1.m_score);
	send1.m_nProportionate = GetProportionate();
	send1.m_nCircleGoldLimit = GetCircleGoldLimit();
	send1.m_nStake = GetStake();

	pUser->Send(send1);

 	//把其他人加进来

 	for(Lint i = 0 ;i <m_desk_user_count; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != pUser)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId(true);
 			addmsg2.m_pos = i;
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_score = m_vip->GetUserScore(m_user[i]);
 			pUser->Send(addmsg2);
			LLOG_DEBUG("OnUserReconnect add player to deskid %d userid %d",m_id, addmsg2.m_userId);
 		}
 	}

	//谁回来了
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 1;
	info.m_pos = pos;
	if(m_user[pos])
		info.m_ip = m_user[pos]->GetIp();
	BoadCastWithOutUser(info, pUser);

	//这里有人申请解算房间
	if (m_resetTime)
	{
		LMsgS2CResetDesk send;
		send.m_errorCode = 0;
		send.m_applay = m_resetUser;
		send.m_time = m_resetTime - gWork.GetCurTime().Secs();
		send.m_flag = m_reset[pos] ? 1 : 0;
		for (Lint j = 0; j < m_desk_user_count; ++j)
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

		if (!gIsMatchType(m_flag))
			pUser->Send(send);
		LLOG_DEBUG("==LMsgS2CResetDesk7 %d %d", pUser->m_userData.m_id, m_flag);
	}

	if (gIsMatchType(GetState()))
	{
		pUser->SendManaged();
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
			for (Lint i = 0; i<m_desk_user_count; ++i)
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

	
	if (mGameHandler)
	{
		mGameHandler->OnUserReconnect(pUser);
	}
	return true;
}

void Desk::SetVip(VipLogItem* vip)
{
	m_vip = vip;
	if(m_vip)
	{
		m_flag = vip->m_flag;
		setPlayType(vip->m_playtype);
	}
}

VipLogItem* Desk::GetVip()
{
	return m_vip;
}

Lint Desk::GetFreePos()
{
	for (Lint i = 0; i <m_desk_user_count; ++i)
	{
		if (m_user[i] == NULL)
			return i;
	}
	return INVAILD_POS;
}

void Desk::OnUserInRoom(User* user)
{
	LLOG_DEBUG("===OnUserInRoom deskid %d is in userid %d innum %d ", m_id, user->GetUserDataId(), GetUserCount());
	if (GetUserPos(user) != INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserInRoom is in desk %d", user->GetUserDataId());
		return;
	}

	Lint pos = GetFreePos();
	if(pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserInRoom INVAILD_POS %d",user->GetUserDataId());
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
	if (gIsMatchType(m_flag))
	{
		send1.m_flag = 0;
	}
	else
	{
		send1.m_flag = m_flag;
	}
	
	send1.m_maxCircle = m_vip->m_maxCircle;
	send1.m_playtype = m_playtype;
	send1.m_rank = user->m_rank;
	send1.m_overScore = user->m_overScore;
	send1.m_sportId = user->m_sportId;
	send1.m_process = user->m_process;

	send1.m_nProportionate = GetProportionate();
	send1.m_nCircleGoldLimit = GetCircleGoldLimit();
	send1.m_nStake = GetStake();

	LLOG_DEBUG("===User::OnUserInRoom userid %d deskid %d score %d", user->GetUserDataId(), m_id, send1.m_score);
	user->Send(send1);

 	LMsgS2CDeskAddUser addmsg1;
 	addmsg1.m_userId = user->GetUserDataId(true);
 	addmsg1.m_score = m_vip->GetUserScore(user);
	addmsg1.m_pos = pos;
 	addmsg1.m_nike = user->m_userData.m_nike;
 	addmsg1.m_ready = m_readyState[pos];
 	addmsg1.m_sex = user->m_userData.m_sex;
 	addmsg1.m_ip = user->GetIp();
 	addmsg1.m_face = user->m_userData.m_headImageUrl;
 	
 	for(Lint i = 0 ;i <m_desk_user_count; ++i)
 	{
 		if(m_user[i] != NULL && m_user[i] != user)
 		{
 			LMsgS2CDeskAddUser addmsg2;
 			addmsg2.m_userId = m_user[i]->GetUserDataId(true);
 			addmsg2.m_pos = i;
 			addmsg2.m_nike = m_user[i]->m_userData.m_nike;
			addmsg2.m_ready = m_readyState[i];
 			addmsg2.m_sex = m_user[i]->m_userData.m_sex;
 			addmsg2.m_face = m_user[i]->m_userData.m_headImageUrl;
 			addmsg2.m_ip = m_user[i]->GetIp();
			addmsg2.m_score = m_vip->GetUserScore(m_user[i]);
 			user->Send(addmsg2);
 			m_user[i]->Send(addmsg1);
			LLOG_DEBUG("OnUserInRoom add player to userid %d deskid %d", addmsg2.m_userId, m_id);
 		}
 	}

	if (gIsMatchType(GetState()))
	{
		user->SendManaged();
	}

	CheckGameStart();
}

void Desk::OnUserOutRoom(User* user)
{
 	Lint pos = GetUserPos(user);
 	if(pos == INVAILD_POS)
 	{
 		LLOG_ERROR("Desk::OnUserOutRoom INVAILD_POS %d",user->GetUserDataId());
 		return;
 	}
 
 	LMsgS2CDeskDelUser del;
 	del.m_pos = pos;	
 	for(Lint i = 0 ;i <m_desk_user_count; ++i)
 	{
 		if(m_user[i] != NULL)
 		{
 			m_user[i]->Send(del);
 		}
	}
	//修改状态;
	if (user->getUserState() != LGU_STATE_CENTER)
	{
		user->setUserState(LGU_STATE_CENTER, 1);
		user->ModifyUserState(true);
	}

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
	return prev== m_desk_user_count-1?0:prev+1;
}

Lint Desk::GetPrevPos(Lint next)
{

	return next==0?m_desk_user_count-1:next-1;
	
}

Lint Desk::GetUserPos(User* user)
{
	for(Lint i = 0;i<m_desk_user_count;++i)
	{
		if(m_user[i] == user)
			return i;
	}

	return INVAILD_POS;
}

User* Desk::GetPosUser(Lint pos)
{
	if(pos<0 || pos > m_desk_user_count-1)
	{
		LLOG_ERROR("find user over size %d pos %d", m_desk_user_count, pos);
			return NULL;
		}
	return m_user[pos];
}

Lint Desk::GetUserCount()
{
	Lint cnt = 0;
		for(Lint i = 0; i < m_desk_user_count;++i)
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
	if (!m_vip)
	{
		return;
	}

	if(!m_vip->IsBegin())
	{
		if (mGameHandler)
		{
			if (GetUserCount() == m_desk_user_count)
			{
				mGameHandler->SetDeskPlay();
			}
		}
	}
	else
	{
		bool begin = true;
		for (Lint i = 0; i <m_desk_user_count; ++i)
		{
			if (m_user[i] == NULL || !m_readyState[i])
			{
				begin = false;
				break;
			}
		}

		if (begin)
		{
			if (mGameHandler)
			{
				mGameHandler->SetDeskPlay();
			}
		}
	}
}

void Desk::SetDeskFree()
{
	m_deskState = DESK_FREE;
	if (mGameHandler)
	{
		GameHandlerFactory::getInstance()->destroy((GameType)m_flag,mGameHandler);
		LLOG_DEBUG("Desk::SetDeskFree    mHander");
	}
	gWork.RecycleDeskId(m_id);
}

void Desk::OnDeskFree()
{

}

void Desk::BoadCast(LMsg& msg)
{
	for(Lint i = 0 ; i < m_desk_user_count ;++i)
	{
		if(m_user[i])
			m_user[i]->Send(msg);
	}
}

void Desk::BoadCastWithOutUser(LMsg& msg, User* user)
{
	for (Lint i = 0; i < m_desk_user_count; ++i)
	{
		if (m_user[i] && m_user[i] != user)
			m_user[i]->Send(msg);
	}
}

void Desk::ClearUser()
{
	for (Lint i = 0; i < m_desk_user_count; ++i)
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

	if (m_deskPlayState == DESK_PLAY_SELECT_PIAO)
	{
		if (m_vip)
		{
			m_vip->SendEnd();
			m_vip->m_desk = NULL;
		}
		ClearUser();
		SetDeskFree();
		SetVip(NULL);
		ResetClear();
	}
	else if (m_deskState == DESK_PLAY)
	{
		if (mGameHandler)
		{
			Lint m_playerHuInfo[DESK_USER_COUNT] = {0};
			mGameHandler->OnGameOver(WIN_NONE, m_playerHuInfo, INVAILD_POS, NULL);
		}
	}
	else
	{
		if(m_vip && m_vip->IsBegin())
		{
			m_vip->SendEnd();
			m_vip->m_desk = NULL;
		}
		ClearUser();
		SetDeskFree();
		SetVip(NULL);
		ResetClear();
	}
}

void Desk::SpecialCardClear()
{
	memset(m_specialCard,0,sizeof(m_specialCard));
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
	switch(m_flag)
	{
	case PK_GAME_STYLE_MAX_DOUDIZHU + 0:
	case PK_GAME_STYLE_MAX_DOUDIZHU + 1:
	case PK_GAME_STYLE_MAX_DOUDIZHU + 2:
		return ZhuanZhuanMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 3:
	case PK_GAME_STYLE_MAX_DOUDIZHU + 4:
	case PK_GAME_STYLE_MAX_DOUDIZHU + 5:
		return ChangShaMaJiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 6:
		return HongZhongMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 9:
		return ShaoYangMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 12:
		return ChenZhouMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 15:
		return ChangDeMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 18:
		return YongZhouMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 21:
		return XiangTanMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 24:
		return YiYangMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 27:
		return ChaoGuMajiang;
	case PK_GAME_STYLE_MAX_DOUDIZHU + 30:
		return ZhuoXiaZiMajiang;
	default:
		{
			if (gIsMatchType(m_flag))
			{
				return ZhuanZhuanMajiang;
			}
		}
	}
	LLOG_ERROR("error flag:%d",m_flag);
	return ZhuanZhuanMajiang;
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
		if (gIsMatchType(m_vip->m_flag))//比赛场
		{
			SendSportGameResult();
		}
		m_vip->m_desk = NULL;
		ClearUser();
		SetDeskFree();
		SetVip(NULL);
		ResetClear();
	}
	else
	{
		//重置玩家的托管状态
		if (gIsMatchType(m_vip->m_flag))
		{
			for (Lint i = 0; i < m_desk_user_count; ++i)
			{
				if (m_user[i] == NULL)
					continue;

				m_user[i]->cancelManaged();
				m_user[i]->SendManaged();
			}
		}
	}

	SpecialCardClear();
}

void Desk::setPlayType(std::vector<Lint>& playtype)
{
	m_playtype = playtype;
	for(int x=0;x<playtype.size();x++)
	{
		LLOG_DEBUG("playtype%d",playtype[x]);
	}
	if (mGameHandler)
	{
		mGameHandler->SetPlayType(playtype);
	}
}

bool Desk::HasRobot()
{
	for (int i = 0; i < 4; i++)
	{
		if (m_user[i])
		{
			if (m_user[i]->IsRobot())
			{
				return true;
			}
		}
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////

bool Desk::CheckRoom(LTime& curr)
{
	if (m_waitChargeTime)
	{
		if (gWork.GetCurTime().Secs() > m_waitChargeTime && !CanGameStart())
		{
			////处理离开
			//for (Lint i = 0; i<m_nUserCounts; ++i)
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

			for (Lint i = 0; i<m_desk_user_count; ++i)
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



void Desk::BrocastUserNoGold(Lint exPos)
{
	if (!m_vip->IsBegin())
	{
		return;
	}

	if (!CanGameStart())
	{
		for (Lint j = 0; j < m_desk_user_count; ++j)
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
			for (Lint i = 0; i<m_desk_user_count; ++i)
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

			if (sendMsg.m_flag == 1 && m_desk_user_count != GetUserCount())
			{
				sendMsg.m_flag = 2;
			}
			m_user[j]->Send(sendMsg);
		}
	}
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
	for (Lint i = 0; i<m_desk_user_count; ++i)
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

	if (nownum == m_desk_user_count && !waitPlayer.size())
	{
		return;
	}

	for (Lint i = 0; i<m_desk_user_count; ++i)
	{
		LMsgS2CBrocastUserNoGold sendMsg;
		sendMsg.m_time = t_time;
		sendMsg.m_flag = 1;
		if (nownum != m_desk_user_count)
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


bool Desk::CanGameStart()
{
	for (Lint i = 0; i<m_desk_user_count; ++i)
	{
		if (m_user[i] && (m_user[i]->getUserGateID() != 65535) && m_user[i]->m_userData.m_numOfCard2s < GetCircleGoldLimit())
		{
			return false;
		}
	}
	return true;
}

bool Desk::isNewPlay()
{
	for (Lint i = 0; i < m_desk_user_count; ++i)
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

	for (Lint i = 0; i < m_desk_user_count; ++i)
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
			m_user[i]->DelCardCount(CARD_TYPE_8, failcost, CARDS_OPER_TYPE_ROOM_FAIL_CARD, m_vip->m_flag, "system");
			gold[pos] -= failcost;
			m_vip->SetUserScore(m_user[i], chargeToCount);
			totolcostgold += failcost;
		}
	}

	//把赢家的数据回滚
	if (totolcostgold == 0 || totolwinscore == 0 || winCount == 0)
	{
		for (Lint i = 0; i < m_desk_user_count; ++i)
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
		for (Lint i = 0; i < m_desk_user_count; ++i)
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
				m_user[i]->AddCardCount(CARD_TYPE_8, realCost, CARDS_OPER_TYPE_ROOM_WIN_CARD, "system");    //新手送卡，15张;
				gold[pos] += realCost;
				Ldouble needProprt = realCost * GetProportionate() / 100;
				if (needProprt /*&& !GetFreeFlag()*/)
				{
					m_user[i]->DelCardCount(CARD_TYPE_8, needProprt, CARDS_OPER_TYPE_ROOM_COST_CARD, m_vip->m_flag, "system");
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
		for (Lint i = 0; i < m_desk_user_count; ++i)
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
					m_user[i]->AddCardCount(CARD_TYPE_8, realCost, CARDS_OPER_TYPE_ROOM_WIN_CARD, "system");    //新手送卡，15张;
					gold[pos] += realCost;
					Ldouble needProprt = realCost * GetProportionate() / 100;
					if (needProprt /*&& !GetFreeFlag()*/)
					{
						m_user[i]->DelCardCount(CARD_TYPE_8, needProprt, CARDS_OPER_TYPE_ROOM_COST_CARD, m_vip->m_flag, "system");
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
			m_user[maxIdx]->AddCardCount(CARD_TYPE_8, realCost, CARDS_OPER_TYPE_ROOM_WIN_CARD_EXADD, "system");    //新手送卡，15张;
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
	for (Lint i = 0; i < m_desk_user_count; ++i)
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

void Desk::SetTeamID(Lint nTeamID)
{
	m_nTeamID = nTeamID;
}

void Desk::SetTeamName(Lstring strTeamName)
{
	m_strTeamName = strTeamName;
}

