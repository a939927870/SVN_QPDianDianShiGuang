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

void VipLogItem::AddLog(User** user, Ldouble* gold, Ldouble cal_gold[4], Lint* winInfo, Lint *bomb, Lstring& videId)
{
	VipDeskLog* log = new VipDeskLog();
	log->m_time = gWork.GetCurTime().Secs();
	log->m_videoId = videId;
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];
		m_score[i] += gold[i];
		log->m_bomb[i] += bomb[i];
	}

	//记录初始积分
	if (m_curCircle == 0)
	{
		for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
		{
			if (m_desk->m_user[i])
			{
				m_iniScore[i] = m_desk->m_user[i]->m_userData.m_numOfCard2s;
			}
		}
	}

	//计算结果
	m_desk->DelCard(cal_gold);
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		log->m_cal_gold[i] = cal_gold[i];
		m_calScore[i] += cal_gold[i];
	}

	m_log.push_back(log);

	m_curCircle += 1;

	if(m_curCircle == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	if(!m_reset && m_desk->m_gameZone != game_zone_gold)
	{
		for (Lint i = 0; i < m_desk->m_nUserCounts; i++)
		{
			if (user[i] != NULL)
			{
				LMsgL2LMGUserCircleIncreased msg;
				msg.m_userid = user[i]->m_userData.m_id;
				gWork.SendToLogicManager(msg);
			}
		}
	}

	//第八局存上圈数;
	if (m_curCircle == m_maxCircle)
	{
		for (Lint i = 0; i < m_desk->m_nUserCounts; i ++)
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
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}

bool VipLogItem::ExiestUser(Lint id)
{
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		if (id == m_posUserId[i])
			return true;
	}
	return false;
}

void VipLogItem::InsertToDb()
{
	std::stringstream ss;
	ss << "INSERT INTO Log (Id,TeamId,Time,Pos1,Pos2,Pos3,Pos4,Flag,SubFlag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,CalScore1,CalScore2,CalScore3,CalScore4,IniScore1,IniScore2,IniScore3,IniScore4,Reset,Data, PlayType) VALUES (";
	ss << "'" << m_id << "',";
	ss << "'" << m_desk->GetTeamID() << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_posUserId[0]<< "',";
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
	ss << "'" << ToString()  << "',";
	ss << "'"<< PlayTypeToStrint() << "')";

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
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		if (id == m_posUserId[i])
			m_posUserId[i] = 0;
	}
}

void VipLogItem::UpdateToDb()
{
	std::stringstream ss;
	ss << "UPDATE Log SET CurCircle=";
	ss << "'" << m_curCircle << "',Pos1=";
	ss << "'" << m_posUserId[0] << "',Pos2=";
	ss << "'" << m_posUserId[1] << "',Pos3=";
	ss << "'" << m_posUserId[2] << "',Pos4=";
	ss << "'" << m_posUserId[3] << "',Score1=";
	ss << "'" << std::to_string(m_score[0]) << "',Score2=";
	ss << "'" << std::to_string(m_score[1]) << "',Score3=";
	ss << "'" << std::to_string(m_score[2]) << "',Score4=";
	ss << "'" << std::to_string(m_score[3]) << "',CalScore1=";
	ss << "'" << std::to_string(m_calScore[0]) << "',CalScore2=";
	ss << "'" << std::to_string(m_calScore[1]) << "',CalScore3=";
	ss << "'" << std::to_string(m_calScore[2]) << "',CalScore4=";
	ss << "'" << std::to_string(m_calScore[3]) << "',Reset=";
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
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		if (m_posUserId[i] == 0 || user->GetUserDataId() == m_posUserId[i])
			return false;
	}

	return true;
}

void VipLogItem::SendInfo()
{
	LMsgS2CVipInfo info;
	info.m_curCircle = m_curCircle ;
	info.m_curMaxCircle = m_maxCircle ;
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		User* user = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user)
			user->Send(info);
	}
}

void VipLogItem::SendEnd()
{
 	LMsgS2CVipEnd msg;
 	User* user[4] = { NULL };
	//Lint  best[4] = {0};
 	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
 	{
 		user[i] = gUserManager.GetUserbyDataId(m_posUserId[i]);
 		if (user[i])
 		{
 			msg.m_nike[i] = user[i]->m_userData.m_nike;
			msg.m_imageUrl[i] = user[i]->m_userData.m_headImageUrl;
 		}
		msg.m_id[i] = m_posUserId[i];
 	}
 
	memset(msg.m_gold,0,sizeof(msg.m_gold));
	memset(msg.m_score, 0, sizeof(msg.m_score));
	memset(msg.m_best,0,sizeof(msg.m_best));
	memset(msg.m_bomb,0,sizeof(msg.m_bomb));
	memset(msg.m_win,0,sizeof(msg.m_win));
	memset(msg.m_lose,0,sizeof(msg.m_lose));
	
 	for (Lsize i = 0; i < m_log.size(); ++i)
 	{
		for(Lint  j = 0; j < m_desk->m_nUserCounts; ++j)
		{
			msg.m_gold[j] += m_log[i]->m_gold[j];
			msg.m_score[j] += m_log[i]->m_cal_gold[j];
			msg.m_bomb[j] += m_log[i]->m_bomb[j];
			if(msg.m_best[j] < m_log[i]->m_cal_gold[j])
				msg.m_best[j] = m_log[i]->m_cal_gold[j];
			
			bool needRecord = false;
			for(int k = 0; k < m_desk->m_nUserCounts ; ++k)
			{
				if(m_log[i]->m_win[k])
				{
					needRecord = true;
					break;
				}
			}
			
			if (needRecord)
			{
				if (m_log[i]->m_win[j] > 0)
					msg.m_win[j]++;
				else
					msg.m_lose[j]++;
			}
		}
	}

	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
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

	LTime l_Time;
	l_Time.SetSecs(m_time);
	RLOG("bswf", LTimeTool::GetLocalTimeInString()
		<< "|" << 15088
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
		<< "|" << LTimeTool::GetLocalTimeInString(l_Time)
		<< "|" << m_deskId
		);
}
bool VipLogItem::isEnd()
{
	return m_maxCircle <= m_curCircle || m_reset == 1;
}

bool VipLogItem::isFinalEnd()
{
	return m_maxCircle <= m_curCircle;
}

Lint VipLogItem::GetUserPos(User* user)
{
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return i;
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
	for (Lint i = 0; i < m_desk->m_nUserCounts; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return true;
		
		if (m_posUserId[i] == 0)
		{
			m_posUserId[i] = user->GetUserDataId();
			return true;
		}
	}

	return false;
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
	m_id = (m_id + 1) % 1000;
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
	m_videoId = (m_videoId + 1) % 1000;
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
	if (m_item.count(logId))
	{
		return m_item[logId];
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
			LLOG_INFO("VipLogMgr::Tick vip end %s",it->first.c_str());
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
			m_deskMapFree.push(it->second);
			m_deskMapUseing.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

Desk* RoomVip::GetFreeDesk(Lint nDeskID, PK_GAME_STYLE gameType)
{
	Desk* ret = NULL;
	if (m_deskMapFree.size() > 0)
	{
		LLOG_DEBUG("GetFree Desk Exist >>>>>>>>>");
		ret = m_deskMapFree.front();
		m_deskMapFree.pop();
	}
	else
	{
		LLOG_DEBUG("GetFree Desk not Exist, new a one <<<<<<<<<<");
		ret = new Desk();
	}
	if (!ret->InitDesk(nDeskID, gameType))
	{
		delete ret;
		ret = NULL;
		LLOG_DEBUG("RoomVip::GetFreeDesk -- Fail to init desk");
	}
	else
	{
		ret->m_id = nDeskID;
		m_deskMapUseing[ret->m_id] = ret;
		ret->SetDeskWait();
	}
	return ret;
}

Desk* RoomVip::GetDeskById(Lint id)
{
	if (m_deskMapUseing.count(id))
	{
		return m_deskMapUseing[id];
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

	if (pMsg->m_flag < CARD_TYPE_4 || pMsg->m_flag > CARD_TYPE_16)
	{
		pMsg->m_flag = CARD_TYPE_8;
	}
	Lint itemId = 0;
	Lint circle = 0;
	switch (pMsg->m_flag)
	{
	case CARD_TYPE_4:
		circle = 6;
		break;
	case CARD_TYPE_8:
		circle = 12;
		break;
	case CARD_TYPE_16:
		circle = 20;
		break;
	}

	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk in desk");
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk deskID is in Use");
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	//创建桌子失败;
	Desk* desk = GetFreeDesk(pMsg->m_deskID, (PK_GAME_STYLE)pMsg->m_gameType);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed!");
		ret.m_errorCode = 5;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

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
		newTeamDesk.nMaxUserCount = desk->m_nUserCounts;
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
	log->m_flag = pMsg->m_gameType;
	log->m_desk = desk;
	log->m_deskId = log->m_desk->m_id;

	log->m_desk->m_tiaopaishu = pMsg->m_lenth;
	log->m_desk->m_gameStyle = (PK_GAME_STYLE)pMsg->m_gameType;
	log->m_desk->m_nMaxFanshu = pMsg->m_nMaxFanshu;
	log->m_desk->m_extValue.assign(pMsg->m_extValue.begin(), pMsg->m_extValue.end());

	log->m_playtype.push_back(game_para_maxfan);
	log->m_playtype.push_back(log->m_desk->m_nMaxFanshu);
	
	if (pMsg->m_lenth > 0 && pMsg->m_lenth <= 48)
	{
		for (Lint cc = 0; cc < pMsg->m_lenth; ++cc)
		{
			log->m_desk->m_specialCard[cc].m_color = pMsg->m_cardValue[cc].m_color;
			log->m_desk->m_specialCard[cc].m_number = pMsg->m_cardValue[cc].m_number;
		}
	}
	
	ret.m_deskId = log->m_desk->m_id;
	log->m_desk->SetVip(log);
	pUser->Send(ret);

	log->AddUser(pUser);
	log->SetUserScore(pUser, pUser->m_userData.m_numOfCard2s);
	log->m_desk->OnUserInRoom(pUser);

	//判断是否需要加电脑， 增加电脑;
	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		if (pMsg->m_robotNum > 0)
		{
			if ( pMsg->m_robotNum >= log->m_desk->m_nUserCounts)
			{
				gRobotManager.AddRobotToDesk(log->m_deskId, log->m_desk->m_nUserCounts - 1);
			}
			else
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
		LLOG_ERROR("RoomVip::AddToVipDesk has desk");
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk )
	{
		LLOG_DEBUG("RoomVip::AddToVipDesk desk null");
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk vip NULL");
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//已经开始，就是房间满了
	if (desk->GetVip()->IsBegin())
	{
		LLOG_DEBUG("RoomVip::AddToVipDesk desk IsBegin %d", nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->m_nUserCounts ||
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

Lint RoomVip::IntoVipGoldDesk(LMsgLMG2LIntoGoldDesk* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		return -1;
	}
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;

	Lint itemId = 0;
	Lint circle = 1;

	if (pUser->GetGoldDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipGoldDesk in desk");
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	Desk* pDesk = GetDeskById(pMsg->m_deskID);
	if (NULL != pDesk)
	{
		if (pDesk->GetVip())
		{
			pDesk->GetVip()->AddUser(pUser);
			pDesk->OnUserInGoldRoom(pUser);
			return ret.m_errorCode;
		}
		else     //金币场到此会导致无法再进入金币场,需要释放桌子;重新分配；
		{
			LLOG_ERROR("error! RoomVIP:Into GoldDesk");
			return -1;
		}
	}
	else
	{
		pDesk = GetFreeDesk(pMsg->m_deskID, (PK_GAME_STYLE)pMsg->m_gameType);
		pDesk->m_gameZone = game_zone_gold;
		pDesk->_clearDesk();
	}
	if (pDesk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed!");
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUser->GetUserDataId());
	log->m_curCircle = 0;
	log->m_maxCircle = circle;
	log->m_flag = pMsg->m_gameType;
	log->m_desk = pDesk;
	log->m_deskId = log->m_desk->m_id;

	log->m_desk->m_tiaopaishu = pMsg->m_lenth;
	log->m_desk->m_gameStyle = (PK_GAME_STYLE)pMsg->m_gameType;
	log->m_desk->m_extValue = pMsg->m_extValue;

	log->m_playtype.push_back(game_para_maxfan);

	if (pMsg->m_lenth > 0 && pMsg->m_lenth <= 48)
	{
		for (Lint cc = 0; cc < pMsg->m_lenth; ++cc)
		{
			log->m_desk->m_specialCard[cc].m_color = pMsg->m_cardValue[cc].m_color;
			log->m_desk->m_specialCard[cc].m_number = pMsg->m_cardValue[cc].m_number;
		}
	}

	ret.m_deskId = log->m_desk->m_id;
	log->m_desk->SetVip(log);
	pUser->Send(ret);

	log->AddUser(pUser);
	log->m_desk->OnUserInGoldRoom(pUser);

	//判断是否需要加电脑， 增加电脑;
	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		if (pMsg->m_robotNum > 0)
		{
			if ( pMsg->m_robotNum >= log->m_desk->m_nUserCounts)
			{
				if (gRobotManager.AddRobotToDesk(log->m_deskId, log->m_desk->m_nUserCounts - 1))
				{
					LMsgL2LMGUseGoldDeskPlace msg;
					msg.m_deskID = log->m_deskId;
					msg.m_gameType = pMsg->m_gameType;
					msg.m_usedCounts = log->m_desk->m_nUserCounts - 1;
					gWork.SendToLogicManager(msg);
				}
			}
			else
			{
				if (gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum))
				{
					LMsgL2LMGUseGoldDeskPlace msg;
					msg.m_deskID = log->m_deskId;
					msg.m_gameType = pMsg->m_gameType;
					msg.m_usedCounts = pMsg->m_robotNum;
					gWork.SendToLogicManager(msg);
				}
			}			
		}
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
	pUser->Send(send);

	//房主
	if(desk->GetVip()->GetOwerId() ==  pUser->GetUserDataId())
	{
		//房间保留，不做任何操作;
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
	}

	return true;
}


