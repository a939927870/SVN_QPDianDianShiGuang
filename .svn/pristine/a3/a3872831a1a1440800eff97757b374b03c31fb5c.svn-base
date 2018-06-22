#include "Desk.h"
#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "UserManager.h"

//const unsigned int g_nAutoSpeakCounts = 20;
//unsigned int g_nSpeakTimes[g_nAutoSpeakCounts] = {0};
unsigned int g_nGoldGameNums = 0;

Desk::Desk():m_GameHandler(NULL), m_vip(NULL)
{
	m_id = 0;
	memset(m_user,0,sizeof(m_user));
	m_deskState = DESK_FREE;
	m_nUserCounts = 0;
	memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
	ResetClear();
	m_resetPos = INVAILD_VAL;
}

void Desk::_clearDesk()
{
	if (game_zone_gold ==  m_gameZone)
	{
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
				m_readyState[i] = 0;
				unSetTimeOfWaitUser(0, i);
		}
	}
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
	if (!CheckRoom(curr))
	{
		ResetEnd();
	}
}

//金币场退出桌子;
void Desk::HanderQuitGoldDesk(User* pUser,LMsgC2SResetDesk* msg)
{
	//进入机器人托管模式;
	pUser->setTuoguan();
	pUser->setUserState(LGU_STATE_CENTER);
	pUser->ModifyUserState(true);
	pUser->setLeaveGoldDesk();   //玩家离开金币场桌子，不会再接受出牌消息;
	//如果托管或者机器人数目超过3，直接销毁桌子;
}

void  Desk::HanderAskGoldDeskState(User* pUser,LMsgC2SAskGoldDeskState* msg)
{
	if (NULL == msg)
	{
		LLOG_DEBUG("HanderAskGoldDeskState msg is NULL!");
		return;
	}
	if (GetUserPos(pUser) != msg->m_pos)
	{
		LLOG_DEBUG("HanderAskGoldDeskState user pos error:user pos %d  ,but msg pos %d", GetUserPos(pUser), msg->m_pos);
		return;
	}
	switch (msg->m_eventType)    //m_eventType事件码;1:进入金币房;2:退出金币房; 3:进入托管; 4:退出托管;
	{
	case 1:
		pUser->unsetLeaveGoldDesk();
		break;
	case 2:
		//離開金幣房;
		if (DESK_WAIT == m_deskState)
		{
			m_readyState[msg->m_pos] = 0;
			for (Lint i = 0; i < m_nUserCounts; ++i)
			{
				unSetTimeOfWaitUser(0, i);//取消人满倒计时;
			}
			BoadCastGoldDeskEvent(msg->m_eventType , msg->m_pos);
			LLOG_DEBUG("user quit gold desk, user pos:%d  user state:%d",msg->m_pos, (Lint)pUser->m_bLeaveGoldDesk);
			OnUserOutRoom(pUser);		//该函数调用后离开玩家将不再收到该桌子消息，最好放在最后调用;
			//需要通知LogicManager回收一个桌子位置;
			recycleDeskPlace(m_id, m_gameStyle);

			//if (0 == GetUserCount())  //所有人走完，不销毁桌子，logicmanager会重新利用;
			//{
			//	ResetEnd();
			//}
		}
		else
		{
			pUser->setTuoguan();
			BoadCastGoldDeskEvent(msg->m_eventType, msg->m_pos);
			LLOG_DEBUG("user quit gold desk, user pos:%d  user state:%d",msg->m_pos, (Lint)pUser->m_bLeaveGoldDesk);
			pUser->setLeaveGoldDesk();   //玩家离开金币场桌子，不会再接受出牌消息;
			pUser->setUserState(LGU_STATE_CENTER);
			pUser->ModifyUserState(true);	
		}
		break;
	case 3:
		//进入机器人托管模式;
		pUser->setTuoguan();
		BoadCastGoldDeskEvent(msg->m_eventType, msg->m_pos);
		break;
	case 4:
		//取消机器人托管模式;
		pUser->Tick(); //自动tick会有时间差，导致上一个机器人操作被忽略;在取消托管之前先处理托管事件;
		pUser->unSetTuoguan();
		BoadCastGoldDeskEvent(msg->m_eventType, msg->m_pos);
		break;
	}
}

//申请解算房间;
void Desk::HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg)
{
	if (game_zone_gold == m_gameZone)
	{
		HanderQuitGoldDesk(pUser, msg);
		return;
	}
	//重复申请;
	if (m_resetTime != 0)
	{
		return;
	}

	Lint pos = GetUserPos(pUser);
	if(pos == INVAILD_POS)
	{
		return;
	}

	if(m_deskState == DESK_FREE)  // !m_vip  ||
	{
		LLOG_ERROR("Desk::HanderResutDesk error desk free");
		return;
	}

	if (m_deskState == DESK_PLAY || m_deskState == QIANG_DIZHU)
	{
		memset(m_reset, RESET_ROOM_TYPE_WAIT, sizeof(m_reset));
		m_reset[pos] = RESET_ROOM_TYPE_AGREE;
		m_resetTime = gWork.GetCurTime().Secs() + 2 * 60;
		m_resetUser = pUser->m_userData.m_nike;
		m_resetPos = pos;

		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			if (m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_nUserCounts; ++j)
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

			for (Lint i = 0; i < m_nUserCounts; ++i)
			{
				if (m_user[i] == NULL)
					continue;

				LMsgS2CResetDesk send;
				send.m_errorCode = 5;
				send.m_applay = m_resetUser;
				send.m_time = m_resetTime - gWork.GetCurTime().Secs();
				send.m_flag = m_reset[i] ? 1 : 0;
				for (Lint j = 0; j < m_nUserCounts; ++j)
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

//玩家选择解算操作;
void Desk::HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg)
{
	if(m_resetTime == 0)
		return;

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
	for (Lint i = 0; i < m_nUserCounts; ++i)
	{
		if (m_reset[i] == RESET_ROOM_TYPE_AGREE)
			agree++;
		else if (m_reset[i] == RESET_ROOM_TYPE_REFLUSE)
			refluse++;
	}

	if (refluse >= 1)
	{
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			if(m_user[i] == NULL)
				continue;

			LMsgS2CResetDesk send;
			send.m_errorCode = 4;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_nUserCounts; ++j)
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
		ResetClear();
	}
	else if (agree >= (m_nUserCounts-1))
	{
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 2;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_nUserCounts; ++j)
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
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			if(m_user[i] == NULL)
				continue;
			LMsgS2CResetDesk send;
			send.m_errorCode = 0;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_nUserCounts; ++j)
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

			for (Lint i = 0; i<m_nUserCounts; ++i)
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
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			if(m_user[i] == NULL)
				continue; 
			LMsgS2CResetDesk send;
			send.m_errorCode = 3;
			send.m_applay = m_resetUser;
			send.m_time = m_resetTime - gWork.GetCurTime().Secs();
			send.m_flag = m_reset[i] ? 1 : 0;
			for (Lint j = 0; j < m_nUserCounts; ++j)
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
		for (Lint i = 0; i<m_nUserCounts; ++i)
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

	//玩家准备,重置准备倒计时;
	unSetTimeOfWaitUser(0, pos);

	CheckGameStart();
}

void Desk::HanderUserOutDesk(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if(pos == INVAILD_POS)
	{
		return;
	}

	//这里游戏未开始，直接退出房间;
	//if (m_deskState != DESK_PLAY && !m_vip->IsBegin() && pos != 0)
	//{
		//OnUserOutRoom(pUser);
		//return;
	//}

	if (game_zone_gold == m_gameZone && DESK_WAIT == m_deskState)
	{
		m_readyState[pos] = 0;
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			unSetTimeOfWaitUser(0, i);//取消人满倒计时;
		}
		BoadCastGoldDeskEvent(2, pos);
		LLOG_DEBUG("user leave from game, user pos:%d  user state:%d", pos, (Lint)pUser->m_bLeaveGoldDesk);
		OnUserOutRoom(pUser);		//该函数调用后离开玩家将不再收到该桌子消息，最好放在最后调用;
		recycleDeskPlace(m_id, m_gameStyle);			//需要通知LogicManager回收一个桌子位置;
	}
	else
	{
		LMsgS2CUserOnlineInfo info;
		info.m_flag = 0;
		info.m_pos = pos;
		BoadCastWithOutUser(info, pUser);
	}
}

void Desk::HanderUserQiangDiZhu(User* pUser,LMsgC2SQiangDiZhu* msg)
{
	m_GameHandler->HanderUserQiangDiZhu(pUser, msg);
}

void Desk::HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg)
{
	m_GameHandler->HanderUserQiangDiZhuOper(pUser, msg);
}

void Desk::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
{
	m_GameHandler->HanderUserPlayCard(pUser, msg);
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

	//统计常用语词频屏蔽;
	//if (1 == msg->m_type && msg->m_id < g_nAutoSpeakCounts)
	//{
	//	++g_nSpeakTimes[msg->m_id];
	//}
}

bool Desk::OnUserReconnect(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserReconnect11 pos error %d", pUser->GetUserDataId());
		return false;
	}

	/*	m_pDesk->m_readyState[pos] = 1;*/

	//把自己加进来;
	LMsgS2CIntoDesk send1;
	send1.m_deskId = m_id;
	send1.m_pos = pos;
	send1.m_ready = m_readyState[pos];
	send1.m_score = m_vip->GetUserScore(pUser);
	send1.m_gameStyle = m_gameStyle;
	send1.m_maxCircle = m_vip->m_maxCircle;
	send1.m_curCircle = m_vip->m_curCircle;
	send1.m_nMaxFanshu = m_nMaxFanshu;
	send1.m_gameZone = m_gameZone;
	send1.m_extValue = m_extValue;
	send1.m_gold = pUser->m_userData.m_Gold;
	send1.m_nProportionate = GetProportionate();
	send1.m_nCircleGoldLimit = GetCircleGoldLimit();
	send1.m_nStake = GetStake();
	pUser->Send(send1);

	//把其他人加进来;
	for (Lint i = 0; i < m_nUserCounts; ++i)
	{
		if (m_user[i] != NULL && m_user[i] != pUser)
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
			addmsg2.m_gold = m_user[i]->m_userData.m_Gold;
			addmsg2.m_location = m_user[i]->m_userData.m_location;
			addmsg2.m_address = m_user[i]->m_userData.m_address;
			pUser->Send(addmsg2);
		}
	}

	//谁回来了;
	LMsgS2CUserOnlineInfo info;
	info.m_flag = 1;
	info.m_pos = pos;
	BoadCastWithOutUser(info, pUser);

	//这里有人申请解算房间;
	if (m_resetTime)
	{
		LMsgS2CResetDesk send;
		send.m_errorCode = 0;
		send.m_applay = m_resetUser;
		send.m_time = m_resetTime - gWork.GetCurTime().Secs();
		send.m_flag = m_reset[pos] ? 1 : 0;
		for (Lint j = 0; j < m_nUserCounts; ++j)
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
		pUser->Send(send);
	}

	//不在游戏中，不需要后面的数据;
	if (m_deskState == DESK_WAIT)
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
			for (Lint i = 0; i<m_nUserCounts; ++i)
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


	bool rel = m_GameHandler->OnUserReconnect(pUser);
	if (m_resetPos != INVAILD_VAL && ( m_deskState == DESK_PLAY || m_deskState == QIANG_DIZHU) )
	{
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			if (m_user[i] == NULL)
				continue;
			if (i != m_resetPos && RESET_ROOM_TYPE_WAIT == m_reset[i])
			{
				LMsgS2CResetDesk send;
				send.m_errorCode = 0;
				send.m_applay = m_resetUser;
				send.m_time = m_resetTime - gWork.GetCurTime().Secs();
				send.m_flag = m_reset[i] ? 1 : 0;
				for (Lint j = 0; j < m_nUserCounts; ++j)
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
	return rel;
}

void Desk::SetVip(VipLogItem* vip)
{
	m_vip = vip;
	if(m_vip)
	{
		m_state = vip->m_flag;
	}
}	  

VipLogItem* Desk::GetVip()
{
	return m_vip;
}

Lint Desk::GetFreePos()
{
	for (Lint i = 0; i < m_nUserCounts; ++i)
	{
		if (m_user[i] == NULL)
			return i;
	}

	return INVAILD_POS;
}

void Desk::OnUserInRoom(User* user)
{
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
	//金币场不会自动准备;
	if (game_zone_gold != m_gameZone)
	{
		m_readyState[pos] = 1;
	}
	else
	{
		m_readyState[pos] = 0;
		unSetTimeOfWaitUser(2, pos);
	}
	LMsgS2CIntoDesk send1;
	send1.m_gameZone = m_gameZone;
	send1.m_deskId = m_id;
	send1.m_pos = pos;
	send1.m_ready = m_readyState[pos];
	send1.m_score = m_vip->GetUserScore(user);
	send1.m_curCircle = m_vip->m_curCircle;
	send1.m_gameStyle = m_gameStyle;
	send1.m_maxCircle = m_vip->m_maxCircle;
	send1.m_nMaxFanshu = m_nMaxFanshu;
	send1.m_extValue = m_extValue;
	send1.m_gold = user->m_userData.m_Gold;
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
	addmsg1.m_gold = user->m_userData.m_Gold;
	addmsg1.m_location = user->m_userData.m_location;
	addmsg1.m_address = user->m_userData.m_address;
 	
 	for(Lint i = 0 ;i < m_nUserCounts; ++i)
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
			addmsg2.m_gold = m_user[i]->m_userData.m_Gold;
			addmsg2.m_location = m_user[i]->m_userData.m_location;
			addmsg2.m_address = m_user[i]->m_userData.m_address;
 			user->Send(addmsg2);
 			m_user[i]->Send(addmsg1);
 		}
 	}
	CheckGameStart();
}

void Desk::OnUserInGoldRoom(User* user)
{
	user->m_goldDesk = this;
	OnUserInRoom(user);
}

//该函数调用后离开玩家将不再收到该桌子消息，最好放在最后调用;	
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
 	for(Lint i = 0 ;i < m_nUserCounts; ++i)
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
	user->m_goldDesk = NULL;

	//清除状态;
	user->SetDesk(NULL);
 	m_user[pos] = NULL;

	if (DESK_WAIT == m_deskState)
	{
		m_readyState[pos] = 0;
	}
	//删除玩家 不删除机器人;
	if (!user->IsRobot())		//不是机器人删除掉
	{
		UserManager::Instance().DelUser(user);
		delete user;
	}

}

Lint Desk::GetNextPos(Lint prev)
{
	return prev==m_nUserCounts-1?0:prev+1;
}

Lint Desk::GetPrevPos(Lint next)
{
	return next==0?m_nUserCounts-1:next-1;
}

Lint Desk::GetUserPos(User* user)
{
	for(Lint i = 0;i<m_nUserCounts;++i)
	{
		if(m_user[i] == user)
			return i;
	}
	return INVAILD_POS;
}

User* Desk::GetPosUser(Lint pos)
{
	if(pos<0 || pos >= INVAILD_POS)
	{
		return NULL;
	}

	return m_user[pos];
}

Lint Desk::GetUserCount()
{
	Lint cnt = 0;
	for(Lint i = 0; i < m_nUserCounts;++i)
	{
		if(m_user[i])
		{
			cnt += 1;
		}
	}

	return cnt;
}

Lint Desk::getDeskState()
{
	return m_deskState;
}

void Desk::SetDeskWait()
{
	m_deskState = DESK_WAIT;
}

void Desk::OnDeskWait()
{
	
}

void Desk::CheckGameStart()
{
	if(!m_vip->IsBegin())
	{
		if (GetUserCount() == m_nUserCounts && game_zone_gold != m_gameZone)
		{
			m_GameHandler->CheckSameIP();
			m_GameHandler->SetDeskDeal();
		}
		if (GetUserCount() == m_nUserCounts && game_zone_gold == m_gameZone)
		{
			m_GameHandler->CheckSameIP();
			bool begin = true;
			for (Lint i = 0; i < m_nUserCounts; ++i)
			{
				if (m_user[i] == NULL || !m_readyState[i])
				{
					begin = false;
					//游戏开始,设置等待准备倒计时;
					setTimeOfWaitUser(0, i, 16);
				}
			}
			if (begin)
			{
				m_GameHandler->SetDeskDeal();
			}
		}
	}
	else
	{
		bool begin = true;
		for (Lint i = 0; i < m_nUserCounts; ++i)
		{
			if (m_user[i] == NULL || !m_readyState[i])
			{
				begin = false;
				break;
			}
		}

		if (begin)
		{
			m_GameHandler->SetDeskDeal();
		}
	}
}

void Desk::SetDeskFree()
{
	m_deskState = DESK_FREE;
	if (m_GameHandler)
	{
		GameHandlerFactory::getInstance()->destroy((PK_GAME_STYLE)m_state, m_GameHandler);
		LLOG_DEBUG("Desk::SetDeskFree set mHander NULL!");
		m_GameHandler = NULL;
	}
	gWork.RecycleDeskId(m_id);
}

void Desk::OnDeskFree()
{

}

void Desk::BoadCast(LMsg& msg)
{
	for(Lint i = 0 ; i < m_nUserCounts ;++i)
	{
		if (game_zone_gold == m_gameZone && m_user[i] && m_user[i]->m_bLeaveGoldDesk)    //离开金币场在大厅中的玩家不应在收到牌局信息;
		{
			continue;;
		}
		if(m_user[i])
		{
			LLOG_DEBUG("send to user(pos:%d) msg id:%d", i, msg.m_msgId);
			m_user[i]->Send(msg);			
		}
	}
}

void Desk::BoadCastWithOutUser(LMsg& msg, User* user)
{
	for (Lint i = 0; i < m_nUserCounts; ++i)
	{
		if (m_user[i] && m_user[i] != user)
			m_user[i]->Send(msg);
	}
}

void Desk::ClearUser()
{
	for (Lint i = 0; i < m_nUserCounts; ++i)
	{
		if (m_user[i])
			OnUserOutRoom(m_user[i]);
	}
}

void Desk::ResetClear()
{
	m_resetTime = 0;
	m_resetUser.clear();
	m_resetPos = INVAILD_VAL;
	memset(m_reset, 0, sizeof(m_reset));
}

void Desk::ResetEnd()
{
	//if(m_vip->IsBegin())
	if(GetUserCount() == m_nUserCounts)
	{
		m_vip->m_reset = 1;
		m_GameHandler->SetDeskEnd(INVAILD_POS);
	}
	else
	{
		SetVip(NULL);
		ClearUser();
		ResetClear();
		SetDeskFree();
	}
}

bool Desk::InitDesk(int iDeskId, PK_GAME_STYLE gameType)
{
	if(game_ddz_2players == gameType)
	{
		m_nUserCounts = 2;
	}
	else if(game_ddz_sichuan_pz == gameType)
	{
		m_nUserCounts = 4;
	}
	else if (game_ddz_sichuan == gameType || game_ddz_sichuan3_pz == gameType || game_wakeng_shanxi == gameType ||
		game_ddz_classical == gameType || game_ddz_3lai == gameType)
	{
		m_nUserCounts = 3;
	}
	else 
	{
		m_nUserCounts = 0;
		LLOG_ERROR("Desk::initDesk -- Game type is wrong. Type = %d", gameType);
		return false;
	}
	_clearDesk();

	if(!_createRegister(gameType))
	{
		return false;
	}

	m_id = iDeskId;
	m_video.m_flag = gameType;
	m_resetPos = -1;
	return true;
}

bool Desk::_createRegister(PK_GAME_STYLE gameType)
{
	m_GameHandler = GameHandlerFactory::getInstance()->create(gameType);
	if (!m_GameHandler)
	{
		LLOG_ERROR("No game handler for game type %d found.", gameType);
		return false;
	}

	m_GameHandler->startup(this);
	return true;
}

void Desk::FinalEnd()
{
	for (Lint i = 0; i < m_nUserCounts; ++i)
	{
		if (m_user[i])
		{
			m_user[i]->addBashu(1);	//增加把数;

			//logout 不管用直接写这;
			m_user[i]->SavePlayCount("system");
		}
	}
}

bool Desk::HasRobot()
{
	for(int i = 0; i < 4; i++)
	{
		if(m_user[i])
		{
			if(m_user[i]->IsRobot())
			{
				return true;
			}
		}
	}

	return false;
}

void Desk::BoadCastGoldDeskEvent(Lint eventType, Lint pos)
{
	LMsgS2CGoldDeskState msg;
	msg.m_eventType = eventType;
	msg.m_pos = pos;
	BoadCast(msg);
}

void  Desk::unSetTimeOfWaitUser(Lint waitReason, Lint userPos)
{
	if (m_user[userPos])
	{
		m_user[userPos]->m_nWaitTime = -1;
	}
}

void  Desk::setTimeOfWaitUser(Lint waitReason, Lint userPos, Lint WaitTime)
{
	if (game_zone_gold == m_gameZone && m_user[userPos])
	{
		m_user[userPos]->m_nWaitTime = WaitTime;
	}
}

void  Desk::recycleDeskPlace(Lint deskID, Lint gameType)
{
	LMsgL2LMGRecyleGoldDeskPlace msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_deskID = deskID;
	msg.m_gameType = gameType;
	gWork.SendToLogicManager(msg);
}


void Desk::BrocastUserNoGold(Lint exPos)
{
	if (!m_vip->IsBegin())
	{
		return;
	}

	if (!CanGameStart())
	{
		for (Lint j = 0; j < m_nUserCounts; ++j)
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
			for (Lint i = 0; i<m_nUserCounts; ++i)
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

			if (sendMsg.m_flag == 1 && m_nUserCounts != GetUserCount())
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
	for (Lint i = 0; i<m_nUserCounts; ++i)
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

	if (nownum == m_nUserCounts && !waitPlayer.size())
	{
		return;
	}

	for (Lint i = 0; i<m_nUserCounts; ++i)
	{
		LMsgS2CBrocastUserNoGold sendMsg;
		sendMsg.m_time = t_time;
		sendMsg.m_flag = 1;
		if (nownum != m_nUserCounts)
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
	for (Lint i = 0; i<m_nUserCounts; ++i)
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
	for (Lint i = 0; i < m_nUserCounts; ++i)
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

	for (Lint i = 0; i < m_nUserCounts; ++i)
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
		for (Lint i = 0; i < m_nUserCounts; ++i)
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
		for (Lint i = 0; i < m_nUserCounts; ++i)
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
		for (Lint i = 0; i < m_nUserCounts; ++i)
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
	for (Lint i = 0; i < m_nUserCounts; ++i)
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

Lint Desk::getGameType()
{
	return m_gameStyle;
}