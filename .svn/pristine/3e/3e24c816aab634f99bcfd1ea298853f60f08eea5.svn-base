#include "RoomVip.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"
#include "RobotManager.h"
#include "RemoteLogThread.h"

VipLogItem::VipLogItem()
{
	m_desk = NULL;
}

VipLogItem::~VipLogItem()
{
	//for(Lsize i = 0 ; i < m_log.size(); ++i)
	//{
	//	delete m_log[i];
	//}
}

Lint VipLogItem::GetOwerId()
{
	return m_posUserId[0];
}

void VipLogItem::AddLog(User** user, Lint* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos,Lint* agang,Lint* mgang,Lint *bomb, Lstring& videId,Lint video_time)
{
	if(!m_desk)
	{
		LLOG_ERROR("VipLogItem::AddLog have no desk");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = video_time;
	log->m_videoId = videId;
	for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];
	}

	//计算结果
	m_desk->DelCard(cal_gold);
	for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
	{
		log->m_cal_gold[i] = cal_gold[i];
		m_calScore[i] += cal_gold[i];
	}

	m_log.push_back(log);

	m_curCircle += 1;
	m_curZhuangPos = zhuangPos;
	
	//比赛场的比赛，将不记录日志和做任务相关内容
	if (gIsMatchType(m_desk->m_flag))
	{
		return;
	}

	if(m_curCircle == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	if(!m_reset)
	{
		for (Lint i = 0; i < m_desk->m_desk_user_count; i++)
		{
			if (user[i] != NULL)
			{
				LMsgL2LMGUserCircleIncreased msg;
				msg.m_userid = user[i]->m_userData.m_id;
				gWork.SendToLogicManager(msg);
			}
		}
	}

	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i <  m_desk->m_desk_user_count; i ++)
		{
			if (user[i] != NULL)
			{
				user[i]->m_userData.m_totalplaynum ++;
				LMsgL2LMGModifyPlayCount msg;
				msg.m_userid = user[i]->m_userData.m_id;
				msg.m_curPlayCount = user[i]->m_userData.m_totalplaynum;
				gWork.SendToLogicManager(msg);
			}
		}

		for (Lint i = 0; i < m_desk->m_desk_user_count; i++)
		{
			if (user[i] != NULL)
			{
				LMsgL2LMGUserCompleteGame msg;
				msg.m_userId = user[i]->m_userData.m_id;
				msg.m_startTime = m_time;
				msg.m_totalCircleNum = m_maxCircle;
				msg.m_playerNum = m_desk->m_desk_user_count;
				for (int j = 0; j < m_desk->m_desk_user_count; j++)
				{
					if (user[j] != NULL)
						msg.m_players[j] = user[j]->m_userData.m_id;
					else
						msg.m_players[j] = -1;
				}
				gWork.SendToLogicManager(msg);
			}
		}
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}

bool VipLogItem::ExiestUser(Lint id)
{
	if(!m_desk)
	{
		LLOG_ERROR("VipLogItem::ExiestUser have no desk");
		return false;
	}
	for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
	{
		if (id == m_posUserId[i])
			return true;
	}
	return false;
}

void VipLogItem::InsertToDb()
{
	if(m_desk) //比赛场取消回放
	{
		if (gIsMatchType(m_desk->m_flag))
		{
			return;
		}
	}

	std::stringstream ss;
	ss << "INSERT INTO Log (Id,TeamId,Time,Pos1,Pos2,Pos3,Pos4,Flag,SubFlag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,CalScore1,CalScore2,CalScore3,CalScore4,IniScore1,IniScore2,IniScore3,IniScore4,Reset,Data, PlayType) VALUES (";
	ss << "'" << m_id << "',";
	ss << "'" << m_desk->GetTeamID() << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_posUserId[0] << "',";
	ss << "'" << m_posUserId[1] << "',";
	ss << "'" << m_posUserId[2] << "',";
	ss << "'" << m_posUserId[3] << "',";
	ss << "'" << m_flag << "',";
	ss << "'" << m_subFlag << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_secret << "',";
	ss << "'" << std::to_string(m_score[0]) << "',";
	ss << "'" << std::to_string(m_score[1]) << "',";
	ss << "'" << std::to_string(m_score[2]) << "',";
	ss << "'" << std::to_string(m_score[3]) << "',";
	ss << "'" << std::to_string(m_calScore[0]) << "',";
	ss << "'" << std::to_string(m_calScore[1]) << "',";
	ss << "'" << std::to_string(m_calScore[2]) << "',";
	ss << "'" << std::to_string(m_calScore[3]) << "',";
	ss << "'" << std::to_string(m_iniScore[0]) << "',";
	ss << "'" << std::to_string(m_iniScore[1]) << "',";
	ss << "'" << std::to_string(m_iniScore[2]) << "',";
	ss << "'" << std::to_string(m_iniScore[3]) << "',";
	ss << "'" << m_reset << "',";
	ss << "'" << ToString() << "',";
	ss << "'" << PlayTypeToStrint() << "')";


	LMsgL2LDBSaveLogItem send;
	send.m_type = 0;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsBegin()
{
	return m_curCircle != 0;
}

void VipLogItem::RemoveUser(Lint id)
{
	if(!m_desk)
	{
		LLOG_ERROR("VipLogItem::RemoveUser have no desk");
		return;
	}
	for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
	{
		if (id == m_posUserId[i])
			m_posUserId[i] = 0;
	}
}

void VipLogItem::UpdateToDb()
{
	if (m_desk) //比赛场取消回放
	{
		if (gIsMatchType(m_desk->m_flag))
		{
			return;
		}
	}

	std::stringstream ss;
	ss << "UPDATE Log SET CurCircle=";
	ss << "'" << m_curCircle << "',Pos1=";
	ss << "'" << m_posUserId[0] << "',Pos2=";
	ss << "'" << m_posUserId[1] << "',Pos3=";
	ss << "'" << m_posUserId[2] << "',Pos4=";
	ss << "'" << m_posUserId[3] << "',Score1=";
	ss << "'" << m_score[0] << "',Score2=";
	ss << "'" << m_score[1] << "',Score3=";
	ss << "'" << m_score[2] << "',Score4=";
	ss << "'" << m_score[3] << "',Reset=";
	ss << "'" << m_reset << "',Pass=";
	ss << "'" << m_secret << "',Data=";
	ss << "'" << ToString() << "' WHERE Id=";
	ss << "'" << m_id << "'";

	LMsgL2LDBSaveLogItem send;
	send.m_type = 1;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsFull(User* user)
{
	Lint count = 0;
	if(m_desk)
	{
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			if (m_posUserId[i] == 0 || user->GetUserDataId() == m_posUserId[i])
				return false;
		}
	}
	else
		return false;
	return true;
}

void VipLogItem::SendInfo()
{
	if(!m_desk)
		return;
	LMsgS2CVipInfo info;
	info.m_curCircle = m_curCircle ;
	info.m_curMaxCircle = m_maxCircle ;
	for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
	{
		User* user = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user)
			user->Send(info);
	}
}

void VipLogItem::SendEnd()
{
	if (!m_desk)
		return;
	LMsgS2CVipEnd msg;
	User* user[4] = { NULL };
	for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
	{
		user[i] = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user[i])
		{
			msg.m_nike[i] = user[i]->m_userData.m_nike;
		}
		msg.m_id[i] = m_posUserId[i];
	}

	memset(msg.m_gold, 0, sizeof(msg.m_gold));
	memset(msg.m_zimo, 0, sizeof(msg.m_zimo));
	memset(msg.m_win, 0, sizeof(msg.m_win));
	memset(msg.m_bomb, 0, sizeof(msg.m_bomb));
	memset(msg.m_dzimo, 0, sizeof(msg.m_dzimo));
	memset(msg.m_dwin, 0, sizeof(msg.m_dwin));
	memset(msg.m_dbomb, 0, sizeof(msg.m_dbomb));
	memset(msg.m_agang, 0, sizeof(msg.m_agang));
	memset(msg.m_mgang, 0, sizeof(msg.m_mgang));

	for (Lsize i = 0; i < m_log.size(); ++i)
	{
		for (Lint j = 0; j < m_desk->m_desk_user_count; ++j)
		{
			msg.m_gold[j] += m_log[i]->m_gold[j];

			if (m_log[i]->m_win[j] == WIN_SUB_ZIMO)
				msg.m_zimo[j] += 1;
			else if (m_log[i]->m_win[j] == WIN_SUB_BOMB)
				msg.m_win[j] += 1;
			else if (m_log[i]->m_win[j] == WIN_SUB_ABOMB)
				//msg.m_bomb[j] += 1;		m_win只记点炮位置， 炮信息由bomb记录
			{
				for (int k = 0; k < m_desk->m_desk_user_count; k++)
				{
					if (m_log[i]->m_bomb[k] == WIN_SUB_ABOMB)
						msg.m_bomb[j] += 1;
					else if (m_log[i]->m_bomb[k] == WIN_SUB_DABOMB)
						msg.m_dbomb[j] += 1;
				}
			}
			else if (m_log[i]->m_win[j] == WIN_SUB_DZIMO)
				msg.m_dzimo[j] += 1;
			else if (m_log[i]->m_win[j] == WIN_SUB_DBOMB)
				msg.m_dwin[j] += 1;

			msg.m_mgang[j] += m_log[i]->m_mgang[j];
			msg.m_agang[j] += m_log[i]->m_angang[j];
		}
	}
	for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
	{
		if (user[i])
			user[i]->Send(msg);
	}

	// 工会创建的桌子，需要通知 工会服务器 记录日志 [2017-7-23 willing]
	if (m_desk->GetTeamID())
	{
		LMsgL2TS_VideoLog videoLog;
		videoLog.nTeamID = m_desk->GetTeamID();
		videoLog.nDeskID = m_desk->GetDeskId();
		videoLog.nFlag = m_desk->getGameType();
		videoLog.m_maxCircle = m_maxCircle;
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			if (user[i] != NULL)
			{
				videoLog.nUserID[i] = user[i]->m_userData.m_id;
			}

			videoLog.nScore[i] = msg.m_gold[i];
		}

		gWork.SendMsgToTeamServer(videoLog);
	}

	//远程统计把数
	const Lint appId = 10001;
	std::basic_ostringstream<char> bo;
	bo << "bswf" << "|" <<
		LTimeTool::GetLocalTimeInString()
		<< "|" << appId
		<< "|" << m_flag
		<< "|" << PlayTypeToString4RLog()
		<< "|" << m_maxCircle
		<< "|" << m_curCircle
		<< "|" << m_reset
		<< "|" << m_posUserId[0]
		<< ";" << m_posUserId[1]
		<< ";" << m_posUserId[2]
		<< ";" << m_posUserId[3]
		<< "|" << gConfig.GetInsideIp()
		<< "|" << gConfig.GetInsidePort()
		<< "|" << gConfig.GetServerID()
		;

	LLOG_ERROR("%s", move(bo.str().c_str()));


	RLOG("bswf", LTimeTool::GetLocalTimeInString()
		<< "|" << appId
		<< "|" << m_flag
		<< "|" << PlayTypeToString4RLog()
		<< "|" << m_maxCircle
		<< "|" << m_curCircle
		<< "|" << m_reset
		<< "|" << m_posUserId[0]
		<< ";" << m_posUserId[1]
		<< ";" << m_posUserId[2]
		<< ";" << m_posUserId[3]
		<< "|" << gConfig.GetInsideIp()
		<< "|" << gConfig.GetInsidePort()
		<< "|" << gConfig.GetServerID()
	);
}
bool VipLogItem::isEnd()
{
	return m_maxCircle <= m_curCircle || m_reset == 1;
}

Lint VipLogItem::GetUserPos(User* user)
{
	if(m_desk)
	{
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			if (user->GetUserDataId() == m_posUserId[i])
				return i;
		}
	}
	return INVAILD_POS;
}

Ldouble VipLogItem::GetUserScore(User* user)
{
	Lint pos = GetUserPos(user);
	if (pos != INVAILD_POS)
	{
		return m_score[pos];
	}
	return 0;
}

void VipLogItem::SetUserScore(User* user, Ldouble nScore)
{
	Lint pos = GetUserPos(user);
	if (pos != INVAILD_POS)
	{
		m_score[pos] = nScore;
	}
}

bool VipLogItem::AddUser(User* user)
{
	if(m_desk)
	{
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			if (user->GetUserDataId() == m_posUserId[i])
				return true;

			if (m_posUserId[i] == 0)
			{
				if(gIsMatchType(m_flag))//比赛场
				{
					set_initScore(i,user->m_score);
				}
				m_posUserId[i] = user->GetUserDataId();
				return true;
			}
		}

	}
	return false;
}

void VipLogItem::set_initScore(Lint position,Lint score)
{
	m_score[position] = score;
}

//////////////////////////////////////////////////////////////////////////
bool VipLogMgr::Init()
{
	m_id = 1;
	m_videoId = 1;
	return true;
}

bool VipLogMgr::Final()
{
	return true;
}

void VipLogMgr::SetVipId(Lint id)
{
	m_id = id;
}

Lstring VipLogMgr::GetVipId()
{
	m_id = (++m_id) % 1000;
	char mVipLogID[64] = { 0 };
	sprintf_s(mVipLogID, sizeof(mVipLogID), "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_id);
	return mVipLogID;
}

void VipLogMgr::SetVideoId(Lint id)
{
	m_videoId = id;
}

Lstring VipLogMgr::GetVideoId()
{
	m_videoId = (++m_videoId) % 1000;
	char mVipVideoID[64] = { 0 };
	sprintf_s(mVipVideoID, sizeof(mVipVideoID), "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_videoId);
	return mVipVideoID;
}

VipLogItem*	 VipLogMgr::GetNewLogItem(Lint card,Lint usrId)
{
	VipLogItem* item = new VipLogItem();
	item->m_id = GetVipId();
	item->m_maxCircle = card;
	item->m_time = gWork.GetCurTime().Secs();
	m_item[item->m_id]= item;
	return item;
}

VipLogItem*	VipLogMgr::GetLogItem(Lstring& logId)
{
	auto iter = m_item.find(logId);
	if (iter != m_item.end())
	{
		return iter->second;
	}

	return NULL;
}

VipLogItem*	VipLogMgr::FindUserPlayingItem(Lint id)
{
	auto it = m_item.begin();
	for(; it != m_item.end(); ++it)
	{
		VipLogItem* item = it->second;
		if(!item->isEnd() && item->ExiestUser(id))
		{
			return item;
		}
	}
	return NULL;
}

void VipLogMgr::AddPlayingItem(VipLogItem* item)
{
	m_item[item->m_id] = item;
}

void VipLogMgr::Tick()
{
	auto it = m_item.begin();
	for (; it != m_item.end();)
	{
		if (it->second->isEnd())
		{
			LLOG_DEBUG("VipLogMgr::Tick vip end %s",it->first.c_str());
			delete it->second;
			m_item.erase(it++);
		}
		else
		{
			it++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool RoomVip::Init()
{

	return true;
}

bool RoomVip::Final()
{
	return true;
}

void RoomVip::Tick(LTime& cur)
{
	DeskMap::iterator it = m_deskMapUseing.begin();
	for (; it != m_deskMapUseing.end();)
	{
		it->second->Tick(cur);
		if (it->second->getDeskState() == DESK_FREE)
		{
			//回收桌子
			LLOG_DEBUG("Recyle Desk %d --------", it->first);
			it->second->CloseOnMatchReset();
			m_deskMapFree.push(it->second);
			m_deskMapUseing.erase(it++);
		}
		else
		{
			 ++it;
		}
	}
}

Desk* RoomVip::GetFreeDesk(Lint nDeskID)
{
	Desk* ret = NULL;
	if (!m_deskMapFree.empty())
	{
		LLOG_DEBUG("GetFree Desk Exist >>>>>>>>>");
		ret = m_deskMapFree.front();
		m_deskMapFree.pop();
		ret->Init();
	}
	else
	{
		LLOG_DEBUG("GetFree Desk not Exist <<<<<<<<<<");
		ret = new Desk();
	}
	ret->m_id = nDeskID;
	m_deskMapUseing[ret->m_id] = ret;
	ret->SetDeskWait();

	LLOG_ERROR("RoomVip::GetFreeDesk size=%d", m_deskMapUseing.size());
	return ret;
}

Desk* RoomVip::GetDeskById(Lint id)
{
	auto iter = m_deskMapUseing.find(id);
	if(iter != m_deskMapUseing.end())
	{
		return iter->second;
	}

	return NULL;
}

Lint RoomVip::CreateVipDesk(LMsgLMG2LCreateDesk* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		return -1;
	}
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;

	if (pMsg->m_flag < CARD_TYPE_4 || pMsg->m_flag > CARD_TYPE_64)
	{
		pMsg->m_flag = CARD_TYPE_8;
	}

	Lint itemId = 0;
	Lint circle = 0;
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle = 4;
		break;
	case CARD_TYPE_8:
		circle = 8;
		break;
	case CARD_TYPE_16:
		circle = 16;
		break;
	case CARD_TYPE_32:
		circle = 1;
		break;
	case CARD_TYPE_64:
		circle = 2;
		break;
	}

	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk user is on desk %d:%d", pUser->m_userData.m_id, pUser->GetDesk()->GetDeskId());
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk deskID is in Use %d:%d", pUser->m_userData.m_id, pMsg->m_deskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//转换playtype
	pMsg->m_playtype.assign(pMsg->m_extValue.begin(), pMsg->m_extValue.end());

	//创建桌子失败
	Desk* desk = GetFreeDesk(pMsg->m_deskID);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed! %d:%d", pUser->m_userData.m_id, pMsg->m_deskID);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	GameType game_type=ZhuanZhuanMajiang;
	switch(pMsg->m_state)
	{
	case ZhuanZhuanMajiang:
	case TwoPeopleZhuanZhuan:
	case ThreePeopleZhuanZhuan:
		game_type=ZhuanZhuanMajiang;
		break;
	case ChangShaMaJiang:
	case TwoPeopleChangSha:
	case ThreePeopleChangSha:
		game_type=ChangShaMaJiang;
		break;
	case HongZhongMajiang:
		game_type=HongZhongMajiang;
		break;
	case ShaoYangMajiang:
		game_type=ShaoYangMajiang;
		break;
	case ChenZhouMajiang:
		game_type=ChenZhouMajiang;
		break;
	case ChangDeMajiang:
		game_type=ChangDeMajiang;
		break;
	case YongZhouMajiang:
		game_type=YongZhouMajiang;
		break;
	case XiangTanMajiang:
		game_type = XiangTanMajiang;
		break;
	case YiYangMajiang:
		game_type = YiYangMajiang;
		break;
	case ChaoGuMajiang:
		game_type = ChaoGuMajiang;
		break;
 	case ZhuoXiaZiMajiang:                //屏蔽捉虾子麻将
 		game_type = ZhuoXiaZiMajiang;
 		break;
	case EM_SIYJ_IPHONE_ZHUANZHUAN:
	case EM_SIYJ_FREE_MATCH:
	case EM_SIYJ_FEE_MATCH:
	case EM_SIYJ_CHRISTMAS_EVE:
	case EM_SIYJ_CHRISTMAS_DAY:
		game_type = ZhuanZhuanMajiang;
		pUser->initPlayGroundData(pMsg->m_rank, pMsg->m_score, pMsg->m_overScore, pMsg->m_state, pMsg->m_process);
		break;
	default:
		{
			{
				if (gIsMatchType(pMsg->m_state))
				{
					game_type = ZhuanZhuanMajiang;
					pUser->initPlayGroundData(pMsg->m_rank, pMsg->m_score, pMsg->m_overScore, pMsg->m_state, pMsg->m_process);
					break;
				}
			}
			LLOG_ERROR("RoomVip::CreateVipDesk create Processor Failed! %d:%d", pUser->m_userData.m_id, pMsg->m_deskID);
			ret.m_errorCode = 5;		//创建桌子失败
			pUser->Send(ret);
			return ret.m_errorCode;
		}
		break;
	}
	//创建桌子失败
	if (!desk->CreateRegister(game_type))
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Processor Failed! %d:%d", pUser->m_userData.m_id, pMsg->m_deskID);
		ret.m_errorCode = 6;		//创建桌子失败
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	desk->SetState(pMsg->m_state);

	if (pMsg->m_stake <= 0)
	{
		pMsg->m_stake = 1;
	}

	if (pMsg->m_useTeamCard)
	{
		desk->SetTeamID(pMsg->m_teamID);
		desk->SetTeamName(pMsg->m_teamName);

		//desk->SetFastPlay(pMsg->m_maYouDesk);
		// 工会桌子，同步给工会服务器 [2017-8-3 willing]
		LMsgL2TS_NewTeamDesk newTeamDesk;
		newTeamDesk.nTeamID = pMsg->m_teamID;
		newTeamDesk.nDeskID = pMsg->m_deskID;
		newTeamDesk.nCreaterUserID = pUser->GetUserDataId();
		newTeamDesk.nMajongType = pMsg->m_gameType;
		newTeamDesk.nMaxCircle = pMsg->m_flag;
		newTeamDesk.nMaxUserCount = desk->m_desk_user_count;
		newTeamDesk.nPlayTypeCount = pMsg->m_extValue.size();
		newTeamDesk.m_fastDesk = pMsg->m_robotNum >= 2 ? 1 : 0;
		for (int i = 0; i < newTeamDesk.nPlayTypeCount; i++)
		{
			newTeamDesk.tPlayType.push_back(pMsg->m_extValue[i]);
		}
		newTeamDesk.nCircleGoldLimit = pMsg->m_circleGoldLimit;
		newTeamDesk.nStake = pMsg->m_stake;
		newTeamDesk.nMaxFanshu = pMsg->m_nMaxFanshu;
		gWork.SendMsgToTeamServer(newTeamDesk);
	}

	desk->SetStake(pMsg->m_stake);
	desk->SetProportionate(pMsg->m_proportionate);
	desk->SetCircleGoldLimit(pMsg->m_circleGoldLimit);

	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUser->GetUserDataId());
	log->m_curCircle = 0;
	log->m_maxCircle = circle;
	log->m_flag = pMsg->m_state;
	log->m_desk = desk;
	log->m_deskId = log->m_desk->m_id;
	log->m_free = pMsg->m_free;
	auto iPlayType = pMsg->m_playtype.begin();
	for (; iPlayType != pMsg->m_playtype.end(); iPlayType ++)
	{
		log->m_playtype.push_back( *iPlayType );
	}
	ret.m_deskId = log->m_desk->m_id;
	log->m_desk->SetVip(log);
	pUser->Send(ret);

	log->AddUser(pUser);
	log->SetUserScore(pUser, pUser->m_userData.m_numOfCard2s);
	log->m_desk->OnUserInRoom(pUser);

	if (pMsg->m_robotNum < 0)
	{
		return ret.m_errorCode;
	}

	if (gIsMatchType(pMsg->m_state))
	{
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}
		if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
		{
			gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
			LLOG_DEBUG("===OnUserInRoom deskid robot %d is in userid %d innum %d robotnum %d ", desk->m_id, pUser->GetUserDataId(), desk->GetUserCount(), pMsg->m_robotNum);
		}
	}
	else
	{
		//判断是否需要加电脑， 增加电脑
		if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
		{
			for (Lint i = 0; i < CARD_COUNT; i ++)
			{
				log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
				log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
			}
			if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
			{
				gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
			}
		}
	}

	return ret.m_errorCode;
}

Lint RoomVip::AddToVipDesk(User* pUser, Lint nDeskID)
{
	if (!pUser)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = nDeskID;
	ret.m_errorCode = 0;

	if(pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk user is already on desk %d:%d userid %d", pUser->m_userData.m_id, pUser->GetDesk()->GetDeskId(), pUser->GetUserDataId());
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk )
	{
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->m_desk_user_count ||
		 desk->getDeskState() !=  DESK_WAIT)
	{
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	desk->GetVip()->AddUser(pUser);
	desk->GetVip()->SetUserScore(pUser, pUser->m_userData.m_numOfCard2s);
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);

	if (desk->GetTeamID() > 0)
	{
		LMsgL2TS_TeamDeskUserChg teamDeskUserChg;
		teamDeskUserChg.nTeamID = desk->GetTeamID();
		teamDeskUserChg.nDeskID = nDeskID;
		teamDeskUserChg.nUserID = pUser->GetUserDataId();
		teamDeskUserChg.nJoinOrLeave = 1; // 加入桌子
		LLOG_DEBUG("****************logicServer send add usdr to teamServer  userid:%d,deskid:%d", teamDeskUserChg.nUserID, nDeskID);
		gWork.SendMsgToTeamServer(teamDeskUserChg);
	}

	return ret.m_errorCode;
}


bool RoomVip::LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser)
{
	if (!pMsg || !pUser)
	{
		return false;
	}
	LMsgS2CLeaveDesk send;
	send.m_errorCode = 0;

	Desk* desk = pUser->GetDesk();
	if(desk == NULL || desk->getDeskState() != DESK_WAIT)
	{
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}

	if(!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}

	//如果是比赛场，是不可以离开桌子的。
	if (gIsMatchType(desk->m_flag))
	{
		LLOG_ERROR("==RoomVip::LeaveToVipDesk OnUserOutRoom  %d:%d", pUser->m_userData.m_id, desk->GetDeskId());
		return false;
	}

	pUser->Send(send);

	//房主
	if(desk->GetVip()->GetOwerId() ==  pUser->GetUserDataId())
	{
		//房间保留，不做任何操作
		gWork.HanderUserLogout(pUser->getUserGateID() , pUser->GetUserGateUserId());
	}
	else
	{
		LMsgL2TS_TeamDeskUserChg teamDeskUserChg;
		teamDeskUserChg.nTeamID = desk->GetTeamID();
		teamDeskUserChg.nDeskID = desk->GetDeskId();
		teamDeskUserChg.nUserID = pUser->GetUserDataId();
		teamDeskUserChg.nJoinOrLeave = 0; // 离开桌子
		gWork.SendMsgToTeamServer(teamDeskUserChg);

		desk->GetVip()->RemoveUser(pUser->GetUserDataId());
		desk->OnUserOutRoom(pUser);
		//gWork.HanderUserLogout(pUser->getUserGateID() , pUser->GetUserGateUserId());
	}
	return true;
}
