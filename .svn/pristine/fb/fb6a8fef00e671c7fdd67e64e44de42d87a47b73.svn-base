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

void VipLogItem::AddTuiDaoLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos,Lint* agang,Lint* mgang,Lint *bomb, Lstring& videId, Lint* maNum, Lint* ghostZimo, Lint *noghostZimo)
{
	if (NULL == user || NULL == gold || NULL == winInfo || NULL == agang || NULL == mgang || NULL == bomb || NULL == ghostZimo || NULL == noghostZimo)
	{
		LLOG_ERROR("VipLogItem find Null point!");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	log->m_videoId = videId;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];

		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];

		log->m_maNum[i] = maNum[i];
		log->m_ghostZimo[i] = ghostZimo[i];
		log->m_noghostZimo[i] = noghostZimo[i];
	}


	//记录初始积分
	if (m_curCircle == 0)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_desk->m_user[i])
			{
				m_iniScore[i] = m_desk->m_user[i]->m_userData.m_numOfCard2s;
			}
		}
	}

	//计算结果
	m_desk->DelCard(cal_gold);
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_cal_gold[i] = cal_gold[i];
		m_calScore[i] += cal_gold[i];
	}

	m_log.push_back(log);

	m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if(m_curCircle == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		onCircleEnd(user);
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);

	ReportHorseInfo2LogicManager(user, maNum);
}
void VipLogItem::AddJiPingLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos,Lint* agang,Lint* mgang,Lint *bomb, Lint *fanshu, Lint *baohu, Lstring& videId)
{
	if (NULL == user || NULL == gold || NULL == winInfo || NULL == agang || NULL == mgang || NULL == bomb || NULL == fanshu || NULL == baohu)
	{
		LLOG_ERROR("VipLogItem find Null point!");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	log->m_videoId = videId;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];
		log->m_fanshu[i] = fanshu[i];
		log->m_baohu[i] = baohu[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];	
	}

	//记录初始积分
	if (m_curCircle == 0)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_desk->m_user[i])
			{
				m_iniScore[i] = m_desk->m_user[i]->m_userData.m_numOfCard2s;
			}
		}
	}

	//计算结果
	m_desk->DelCard(cal_gold);
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_cal_gold[i] = cal_gold[i];
		m_calScore[i] += cal_gold[i];
	}

	m_log.push_back(log);

	m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if(m_curCircle == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}
	
	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		onCircleEnd(user);
	}

	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
}
void VipLogItem::AddHongZhongLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lint *fanshu, Lint *zimo, Lint maNum[], Lstring& videId)
{
	if (NULL == user || NULL == gold || NULL == winInfo || NULL == agang || NULL == mgang || NULL == bomb || NULL == fanshu || NULL == zimo)
	{
		LLOG_ERROR("VipLogItem find Null point!");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	log->m_videoId = videId;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];
		log->m_noghostZimo[i] = zimo[i];
		m_score[i] += gold[i];
		log->m_maNum[i] = maNum[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];
	}

	//记录初始积分
	if (m_curCircle == 0)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_desk->m_user[i])
			{
				m_iniScore[i] = m_desk->m_user[i]->m_userData.m_numOfCard2s;
			}
		}
	}

	//计算结果
	m_desk->DelCard(cal_gold);
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_cal_gold[i] = cal_gold[i];
		m_calScore[i] += cal_gold[i];
	}

	m_log.push_back(log);

	m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if (m_curCircle == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		onCircleEnd(user);
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);

	ReportHorseInfo2LogicManager(user, maNum);
}
void VipLogItem::AddJieyangLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lstring& videoId, Lint* maNum)
{
	AddZhuanZhuanLog(user, gold, cal_gold, winInfo, zhuangPos, agang, mgang, bomb, videoId, maNum);
}
void VipLogItem::AddZhuanZhuanLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos,Lint* agang,Lint* mgang,Lint *bomb, Lstring& videId, Lint* maNum)
{
	if (NULL == user || NULL == gold || NULL == winInfo || NULL == agang || NULL == mgang || NULL == bomb)
	{
		LLOG_ERROR("VipLogItem find Null point!");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	log->m_videoId = videId;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];
		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];
	}

	//记录初始积分
	if (m_curCircle == 0)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_desk->m_user[i])
			{
				m_iniScore[i] = m_desk->m_user[i]->m_userData.m_numOfCard2s;
			}
		}
	}

	//计算结果
	m_desk->DelCard(cal_gold);
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_cal_gold[i] = cal_gold[i];
		m_calScore[i] += cal_gold[i];
	}

	m_log.push_back(log);

	m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if(m_curCircle == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		onCircleEnd(user);
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);

	ReportHorseInfo2LogicManager(user, maNum);
}
void VipLogItem::AddJokerChickenLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lstring& videoId, Lint* maNum, Lint *normal, Lint* singleflower)
{
	if (NULL == user || NULL == gold || NULL == winInfo || NULL == agang || NULL == mgang || NULL == bomb || NULL == normal || NULL == singleflower)
	{
		LLOG_ERROR("VipLogItem find Null point!");
		return;
	}
	VipDeskLog* log = new VipDeskLog();
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	log->m_videoId = videoId;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_gold[i] = gold[i];
		log->m_win[i] = winInfo[i];

		m_score[i] += gold[i];
		log->m_angang[i] += agang[i];
		log->m_mgang[i] += mgang[i];
		log->m_bomb[i] += bomb[i];

		log->m_maNum[i] = maNum[i];
		log->m_normalhu[i] = normal[i];
		log->m_singleflow[i] = singleflower[i];
	}

	//记录初始积分
	if (m_curCircle == 0)
	{
		for (Lint i = 0; i < m_iPlayerCapacity; ++i)
		{
			if (m_desk->m_user[i])
			{
				m_iniScore[i] = m_desk->m_user[i]->m_userData.m_numOfCard2s;
			}
		}
	}

	//计算结果
	m_desk->DelCard(cal_gold);
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		log->m_cal_gold[i] = cal_gold[i];
		m_calScore[i] += cal_gold[i];
	}

	m_log.push_back(log);

	m_curCircle += 1;
	m_curZhuangPos = zhuangPos;

	if (m_curCircle == 1)
	{
		InsertToDb();
	}
	else
	{
		UpdateToDb();
	}

	//第八局存上圈数
	if (m_curCircle == m_maxCircle)
	{
		onCircleEnd(user);
	}
	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);

	ReportHorseInfo2LogicManager(user, maNum);
}
void VipLogItem::AddChaoZhouLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lstring& videoId, Lint* maNum)
{
	AddZhuanZhuanLog(user, gold, cal_gold, winInfo, zhuangPos, agang, mgang, bomb, videoId, maNum);
}
void VipLogItem::AddHongkongLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lstring& videoId, Lint* maNum)
{
	AddZhuanZhuanLog(user, gold, cal_gold, winInfo, zhuangPos, agang, mgang, bomb, videoId, maNum);
}

bool VipLogItem::ExiestUser(Lint id)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (id == m_posUserId[i])
			return true;
	}
	return false;
}

void VipLogItem::InsertToDb()
{
	if (!m_desk) return;
	std::stringstream ss;
	ss << "INSERT INTO Log (Id,serverId,TeamId,Time,Pos1,Pos2,Pos3,Pos4,Flag,DeskId,MaxCircle,CurCircle,Pass,Score1,Score2,Score3,Score4,CalScore1,CalScore2,CalScore3,CalScore4,IniScore1,IniScore2,IniScore3,IniScore4,Reset,Data,PlayType) VALUES (";
	ss << "'" << m_id << "',";
	ss << "'" << m_nAppId << "',";
	ss << "'" << m_desk->GetTeamID() << "',";
	ss << "'" << m_time << "',";
	ss << "'" << m_posUserId[0]<< "',";
	ss << "'" << m_posUserId[1] << "',";
	ss << "'" << m_posUserId[2] << "',";
	ss << "'" << m_posUserId[3] << "',";
	ss << "'" << m_flag << "',";
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
	if (m_curMatchId != 0 && m_CreaterID == 0) return;//比赛场不存储viplog
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
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
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
	ss << "'" << m_score[0] << "',Score2=";
	ss << "'" << m_score[1] << "',Score3=";
	ss << "'" << m_score[2] << "',Score4=";
	ss << "'" << m_score[3] << "',CalScore1=";
	ss << "'" << m_calScore[0] << "',CalScore2=";
	ss << "'" << m_calScore[1] << "',CalScore3=";
	ss << "'" << m_calScore[2] << "',CalScore4=";
	ss << "'" << m_calScore[3] << "',Reset=";
	ss << "'" << m_reset << "',Pass=";
	ss << "'" << m_secret << "',Data=";
	ss << "'" << ToString() << "' WHERE Id=";
	ss << "'" << m_id << "'";
	if (m_curMatchId != 0 && m_CreaterID == 0) return;//比赛场不存储viplog
	LMsgL2LDBSaveLogItem send;
	send.m_type = 1;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsFull(User* user)
{
	if (NULL == user || NULL == m_desk)
	{
		return true;
	}

	Lint count = 0;
	Lint deskplayernum = m_desk->GetDeskPlayerNum();
	for (Lint i = 0; i < deskplayernum; ++i)
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
	for (Lint i = 0; i < 4; ++i)
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
	msg.m_playerNum = m_desk->m_deskplayer_num;
	for (Lint i = 0; i < 4; ++i)
	{
		user[i] = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user[i])
		{
			msg.m_nike[i] = user[i]->m_userData.m_nike;
		}
		msg.m_id[i] = m_posUserId[i];
	}
	memset(msg.m_gold, 0, sizeof(msg.m_gold));
	memset(msg.m_score, 0, sizeof(msg.m_score));
	memset(msg.m_zimo, 0, sizeof(msg.m_zimo));
	memset(msg.m_win, 0, sizeof(msg.m_win));
	memset(msg.m_bomb, 0, sizeof(msg.m_bomb));
	memset(msg.m_dzimo, 0, sizeof(msg.m_dzimo));
	memset(msg.m_dwin, 0, sizeof(msg.m_dwin));
	memset(msg.m_dbomb, 0, sizeof(msg.m_dbomb));
	memset(msg.m_agang, 0, sizeof(msg.m_agang));
	memset(msg.m_mgang, 0, sizeof(msg.m_mgang));
	memset(msg.m_fannum, 0, sizeof(msg.m_fannum));
	memset(msg.m_baohu, 0, sizeof(msg.m_baohu));

	memset(msg.m_maNum, 0, sizeof(msg.m_maNum));
	memset(msg.m_ghostZimo, 0, sizeof(msg.m_ghostZimo));
	memset(msg.m_noghostZimo, 0, sizeof(msg.m_noghostZimo));
	memset(msg.m_normalhu, 0, sizeof(msg.m_normalhu));
	memset(msg.m_singleflow, 0, sizeof(msg.m_singleflow));
	Lint gameType = m_desk->getGameType();
	for (Lsize i = 0; i < m_log.size(); ++i)
	{
		for (Lint j = 0; j < 4; ++j)
		{
			if (gameType == HeYuanJokerChickenHu || gameType == HuizhouzhuangGhost)
			{
				if (m_log[i]->m_win[j] == WIN_SUB_ZIMO || m_log[i]->m_win[j] == WIN_SUB_DZIMO)
					msg.m_win[j] += 1;

				msg.m_agang[j] += m_log[i]->m_angang[j];
				msg.m_mgang[j] += m_log[i]->m_mgang[j];
				msg.m_gold[j] += m_log[i]->m_gold[j];
				msg.m_score[j] += m_log[i]->m_cal_gold[j];
				msg.m_maNum[j] += m_log[i]->m_maNum[j];
				msg.m_normalhu[j] += m_log[i]->m_normalhu[j];
				msg.m_singleflow[j] += m_log[i]->m_singleflow[j];
			}
			else if (gameType == GuangDongTuiDaoHuMaJing || gameType == GuangDongOneHundred
				|| gameType == GuangDongCustomTuiDaoHu || gameType == ThreePlayerTuiDaoHu
				|| gameType == RedDragon || gameType == HuizhouzhuangFlower
				|| gameType == GuangDongJihu
				|| gameType == TwoPlayerTuiDaoHu
				|| gameType == MeizhouHongzhonbao
				|| gameType == Shaoguan
				|| gameType == Nanxiaong
				|| gameType == Qujiang
				|| gameType == Maba
				|| gameType == GuangdongJieXiMajiang
				|| gameType == HuiYangWanFa
				)
			{
				if (m_log[i]->m_win[j] == WIN_SUB_ZIMO || m_log[i]->m_win[j] == WIN_SUB_DZIMO)
					msg.m_win[j] += 1;

				msg.m_agang[j] += m_log[i]->m_angang[j];
				msg.m_mgang[j] += m_log[i]->m_mgang[j];
				msg.m_gold[j] += m_log[i]->m_gold[j];
				msg.m_score[j] += m_log[i]->m_cal_gold[j];
				msg.m_maNum[j] += m_log[i]->m_maNum[j];
				msg.m_ghostZimo[j] += m_log[i]->m_ghostZimo[j];
				msg.m_noghostZimo[j] += m_log[i]->m_noghostZimo[j];
			}
			else if (gameType == GuangDongJiPingHuMaJing)
			{
				msg.m_gold[j] += m_log[i]->m_gold[j];
				msg.m_score[j] += m_log[i]->m_cal_gold[j];
				//番数
				msg.m_fannum[j] += m_log[i]->m_fanshu[j];
				//爆胡
				msg.m_baohu[j] += m_log[i]->m_baohu[j];
				if (m_log[i]->m_win[j] == WIN_SUB_ZIMO || m_log[i]->m_win[j] == WIN_SUB_DZIMO)
					msg.m_zimo[j] += 1;
			}
			else if (gameType == HuNanMaJing)
			{
				msg.m_gold[j] += m_log[i]->m_gold[j];
				msg.m_score[j] += m_log[i]->m_cal_gold[j];

				if (m_log[i]->m_win[j] == WIN_SUB_ZIMO)
					msg.m_zimo[j] += 1;
				else if (m_log[i]->m_win[j] == WIN_SUB_BOMB)
					msg.m_win[j] += 1;
				else if (m_log[i]->m_win[j] == WIN_SUB_ABOMB)
				{
					for (int k = 0; k < m_iPlayerCapacity; k++)
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
			else if (gameType == GuangDongChaoZhou || gameType == Hongkong 
					|| gameType == ChaoShanBiHu || gameType == GuangDongRaoping || gameType == LongMenZiyouzhuang)
			{
				msg.m_gold[j] += m_log[i]->m_gold[j];
				msg.m_score[j] += m_log[i]->m_cal_gold[j];

				if (m_log[i]->m_win[j] == WIN_SUB_ZIMO || m_log[i]->m_win[j] == WIN_SUB_DZIMO)
					msg.m_zimo[j] += 1;
				else if (m_log[i]->m_win[j] == WIN_SUB_BOMB || m_log[i]->m_win[j] == WIN_SUB_DBOMB)
					msg.m_win[j] += 1;
				else if (m_log[i]->m_win[j] == WIN_SUB_ABOMB)
					//msg.m_bomb[j] += 1;		m_win只记点炮位置， 炮信息由bomb记录
				{
					for (int k = 0; k < m_iPlayerCapacity; k++)
					{
						if (m_log[i]->m_bomb[k] == WIN_SUB_ABOMB || m_log[i]->m_bomb[k] == WIN_SUB_DABOMB)
							msg.m_bomb[j] += 1;
					}
				}

				msg.m_mgang[j] += m_log[i]->m_mgang[j];
				msg.m_agang[j] += m_log[i]->m_angang[j];
			}
			else if (gameType == JiHuFlower || gameType == GuangdongJieyang)
			{
				msg.m_gold[j] += m_log[i]->m_gold[j];
				msg.m_score[j] += m_log[i]->m_cal_gold[j];

				if (m_log[i]->m_win[j] == WIN_SUB_ZIMO || m_log[i]->m_win[j] == WIN_SUB_DZIMO)
					msg.m_zimo[j] += 1;
				else if (m_log[i]->m_win[j] == WIN_SUB_BOMB || m_log[i]->m_win[j] == WIN_SUB_DBOMB)
					msg.m_win[j] += 1;
				else if (m_log[i]->m_win[j] == WIN_SUB_ABOMB)
				{
					for (int k = 0; k < m_iPlayerCapacity; k++)
					{
						if (m_log[i]->m_bomb[k] == WIN_SUB_ABOMB || m_log[i]->m_bomb[k] == WIN_SUB_DABOMB)
							msg.m_bomb[j] += 1;
					}
				}
				msg.m_maNum[j] += m_log[i]->m_maNum[j];
				msg.m_mgang[j] += m_log[i]->m_mgang[j];
				msg.m_agang[j] += m_log[i]->m_angang[j];
			}
			if (gameType == GuangDongHongZhongMajiang)
			{
				if (m_log[i]->m_win[j] == WIN_SUB_ZIMO || m_log[i]->m_win[j] == WIN_SUB_DZIMO)
					msg.m_win[j] += 1;

				msg.m_agang[j] += m_log[i]->m_angang[j];
				msg.m_mgang[j] += m_log[i]->m_mgang[j];
				msg.m_gold[j] += m_log[i]->m_gold[j];
				msg.m_score[j] += m_log[i]->m_cal_gold[j];
				msg.m_maNum[j] += m_log[i]->m_maNum[j];
				msg.m_zimo[j] += m_log[i]->m_noghostZimo[j];
			}
		}
	}

	for (Lint i = 0; i < 4; ++i)
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

	LTime StrartTime;
	StrartTime.SetSecs(m_time);

	std::basic_ostringstream<char> bo;
	bo << "bswf" << "|" <<
		LTimeTool::GetLocalTimeInString()
		<< "|" << m_nAppId
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
		<< "|" << LTimeTool::GetLocalTimeInString(StrartTime)
		<< "|" << m_desk->GetDeskId()
		<< "|" << GetPaiJuType()
		<< "|" << GetBaShuUnit()
		<< "|" << GetCardCountToDel()
		<< "|" << GetCardCountRealDel()
		<< "|" << RLOG_VERSION
		;
	if (!m_curMatchId)  //非比赛场才记bswf
	{
		RLOG("bswf", LTimeTool::GetLocalTimeInString()
			<< "|" << m_nAppId
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
			<< "|" << LTimeTool::GetLocalTimeInString(StrartTime)
			<< "|" << m_desk->GetDeskId()
			<< "|" << GetPaiJuType()
			<< "|" << GetBaShuUnit()
			<< "|" << GetCardCountToDel()
			<< "|" << GetCardCountRealDel()
			<< "|" << RLOG_VERSION
			);
	}

}

void VipLogItem::Report2MatchServer(const LMsgS2CGameOver & endmsg)
{
	LMsgL2Match_DeskCircleOverGoldInfo msg;
	msg.m_nDeskID = m_desk->GetDeskId();
	msg.m_serverID = gConfig.GetServerID();
	msg.m_nMatchID = m_curMatchId;
	msg.m_nGroupID = m_nGroupID;
	msg.m_bFinalEnd = endmsg.m_end != 0;
	for (int i = 0; i < 4; ++i)
	{
		msg.m_UserID[i] = m_posUserId[i];
		msg.m_Gold[i] = endmsg.m_score[i];
	}
	gWork.SendMsgToMatchServer(msg, m_curMatchId);
}


//这是只在桌子结束的时候同步, 跟上边的冲突,用上边不用下边的,使用需谨慎
void VipLogItem::Report2MatchServer(const LMsgS2CVipEnd & endmsg)
{
	LMsgL2Match_DeskCircleOverGoldInfo msg;
	msg.m_nDeskID = m_desk->GetDeskId();
	msg.m_serverID = gConfig.GetServerID();
	msg.m_nMatchID = m_curMatchId;
	msg.m_nGroupID = m_nGroupID;
	for (int i = 0; i < 4; ++i)
	{
		msg.m_UserID[i] = endmsg.m_id[i];
		msg.m_Gold[i] = endmsg.m_gold[i];
	}
	gWork.SendMsgToMatchServer(msg, msg.m_nMatchID);
}


bool VipLogItem::isEnd()
{
	return m_maxCircle <= m_curCircle || m_reset == 1;
}

void VipLogItem::onCircleEnd(User** user)
{
	if (m_curMatchId)//比赛不统计playCount
	{
		return;
	}
	if (NULL == user)
	{
		return;
	}

	if (NULL == *user)
	{
		return;
	}
	for (Lint i = 0; i < m_iPlayerCapacity; i++)
	{
		if (user[i] != NULL)
		{
			user[i]->m_userData.m_totalplaynum++;		
			LMsgL2LMGModifyPlayCount msg;
			msg.m_userid = user[i]->m_userData.m_id;
			msg.m_curPlayCount = user[i]->m_userData.m_totalplaynum;
			gWork.SendToLogicManager(msg);	
		}
	}

	//牌局结束不给玩家加礼券了
	for (Lint i = 0; i < m_iPlayerCapacity; i++)
	{
		if (user[i] != NULL)
		{
			LMsgL2LMGUserCircleIncreased msg;
			msg.m_userid = user[i]->m_userData.m_id;
			gWork.SendToLogicManager(msg);
		}
	}
}

void VipLogItem::ReportHorseInfo2LogicManager(User** user, Lint* maNum)
{
	if (m_curMatchId)//比赛不统计playCount
	{
		return;
	}
	if (NULL == user || NULL == *user || NULL == maNum)
	{
		return;
	}

	LMsgL2LMGReportHorseInfo msg;
	msg.m_playerNum = m_iPlayerCapacity;
	for (Lint i = 0; i < m_iPlayerCapacity; i++)
	{
		if (user[i] != NULL)
		{
			msg.m_userid[i] = user[i]->m_userData.m_id;
			msg.m_horseNum[i] = maNum[i];
		}
	}
	gWork.SendToLogicManager(msg);
}

Lint VipLogItem::GetUserPos(User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return i;
	}

	return DEFAULT_INVAILD_POS;
}

Ldouble VipLogItem::GetUserScore(User* user)
{
	Lint pos = GetUserPos(user);
	if (pos != DEFAULT_INVAILD_POS)
	{
		return m_score[pos];
	}
	return 0;
}

void VipLogItem::SetUserScore(User* user, Ldouble nScore)
{
	Lint pos = GetUserPos(user);
	if (pos != DEFAULT_INVAILD_POS)
	{
		m_score[pos] = nScore;
	}
}

bool VipLogItem::AddUser(User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
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

void VipLogItem::SetCurrDifen()
{
	if (m_curMatchId == 0) return;
	int nCurrentTime = gWork.GetCurTime().GetHour() * 100 + gWork.GetCurTime().GetMin();
	LTime tStart;
	tStart.SetSecs(m_MatchStartTime);
	int nStartMin = tStart.GetHour() * 100 + tStart.GetMin();
	int costMin = (nCurrentTime / 100 - nStartMin / 100) * 60 + (nCurrentTime % 100 - nStartMin % 100) + 1;
	if (m_nCurStage == 500)
	{
		m_nCurDifen = (m_nInitualDifen + costMin*(costMin - 1) * m_nDifenPerMinIncStep / 2);
		return;
	}
	m_nCurDifen = m_nDingJuInitualDifen;
}

//牌局类型 1:好友桌 2:金币场 3:比赛场
EPaiJuType VipLogItem::GetPaiJuType()
{
	return m_curMatchId ? PJT_BI_SAI_CHANG : PJT_HAO_YOU_ZHUO; //目前只有好友桌和比赛场
}

//把数单位 1:局 2:圈 3:锅
EBaShuUnit VipLogItem::GetBaShuUnit()
{
	return BSU_JU; //目前只有局
}

//应扣房卡数
Lint VipLogItem::GetCardCountToDel()
{
	if (m_curMatchId)
	{
		return 0;
	}

	if (m_curCircle <= 1)
		return 0; //第一局未完解散
	if (m_maxCircle == 8)
	{
		return 4;
	}
	else if (m_maxCircle == 16)
	{
		return 8;
	}
	else
	{
		LLOG_ERROR("[ERROR] invalid max circle: %d", m_maxCircle);
		return 0;
	}
}

//实扣房卡数
Lint VipLogItem::GetCardCountRealDel()
{
	if (m_curMatchId)
	{
		return 0;
	}

	return m_nCardDel;
}

//设置实际扣卡数
void VipLogItem::AddCardCountRealDel(Ldouble nCardDel)
{
	m_nCardDel += nCardDel;
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
	Lint id = ++m_id;
	m_id = id % 5000;
	char mVipLogID[64] = { 0 };
	sprintf_s(mVipLogID, 64, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_id);
	return mVipLogID;
}

void VipLogMgr::SetVideoId(Lint id)
{
	m_videoId = id;
}

Lstring VipLogMgr::GetVideoId()
{
	Lint id = ++m_videoId;
	m_videoId = id % 5000;
	char mVipVideoID[64] = { 0 };
	sprintf_s(mVipVideoID, 64, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_videoId);
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

Desk* RoomVip::GetFreeDesk(Lint nDeskID, Lint nAppId)
{
	Desk* ret = NULL;
	if (m_deskMapFree.size() > 0)
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
	ret->SetDeskAppId(nAppId);
	m_deskMapUseing[ret->m_id] = ret;
	ret->SetDeskWait();

	LLOG_ERROR("RoomVip::GetFreeDesk size=%d", m_deskMapUseing.size());
	return ret;
}

Desk* RoomVip::GetDeskById(Lint id)
{
	if(m_deskMapUseing.count(id))
	{
		return m_deskMapUseing[id];
	}

	return NULL;
}

bool RoomVip::CreateVipDesk(LMsgLMG2LCreateDesk* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		return false;
	}

	Lint nAppId = pUser->m_userData.m_nAppId;
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;
	//Lint needCostCardNum = pMsg->m_aaRoomCostPlayerCardNum;
	Lint maxCircle = 4;  
	if (pMsg->m_flag == FRIEND_ROOM_TYPE_8)
	{
		maxCircle = 8;	 	
		//needCostCardNum = pMsg->m_aaRoomCostPlayerCardNum * 2;
	}else if (pMsg->m_flag == FRIEND_ROOM_TYPE_16)
	{
		maxCircle = 16;
	}

	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk in desk");
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return false;
	}
	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk deskID is in Use");
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return false;
	}
	//获得桌子失败
	Desk* desk = GetFreeDesk(pMsg->m_deskID, nAppId);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed!");
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return false;
	}

	desk->m_antiCheat = pMsg->m_antiCheat;
	desk->m_aaRoom = pMsg->m_aaRoom;
	desk->SetAaRoomCostPlayerCardNum(0);
	//设置房间、viplog人数
	desk->SetDeskPlayerNum(pMsg->m_state, pMsg->m_playerNum);
	//
	desk->SetDeskAppId(nAppId);
	//设置房间是否可以交互
	desk->SetDeskCanInteraction(pMsg->m_canInteraction);

	desk->SetFreeFlag(pMsg->m_free);

	if (pMsg->m_stake <= 0)
	{
		pMsg->m_stake = 1;
	}

	desk->SetStake(pMsg->m_stake);
	desk->SetProportionate(pMsg->m_proportionate);
	desk->SetCircleGoldLimit(pMsg->m_circleGoldLimit);
	

	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log = gVipLogMgr.GetNewLogItem(maxCircle, pUser->GetUserDataId());
	log->m_curCircle = 0;
	log->m_maxCircle = maxCircle;
	log->m_flag = pMsg->m_state;
	log->m_desk = desk;
	log->m_deskId = log->m_desk->m_id;
	log->SetAppId(nAppId);
	auto iPlayType = pMsg->m_playtype.begin();
	for (; iPlayType != pMsg->m_playtype.end(); ++iPlayType)
	{
		log->m_playtype.push_back( *iPlayType );
	}
	ret.m_deskId = log->m_desk->m_id;
	log->m_desk->SetVip(log);
	log->m_iPlayerCapacity = desk->GetDeskPlayerNum();
	pUser->Send(ret);

	log->AddUser(pUser);
	pUser->SetLocation(pMsg->m_location);

	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		for (Lint i = 0; i < CARD_COUNT; i++)
		{
			log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}
	}

	//创建桌子
	if (!desk->CreateRegister((GameType)pMsg->m_state))
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Processor Failed!");
		ret.m_errorCode = 5;		//创建桌子失败
		pUser->Send(ret);
		return false;
	}

	if (pMsg->m_stake <= 0)
	{
		pMsg->m_stake = 1;
	}

	if (pMsg->m_useTeamCard)
	{
		desk->SetTeamID(pMsg->m_teamID);
		desk->SetTeamName(pMsg->m_teamName);

		desk->SetFastPlay(pMsg->m_maYouDesk);
		// 工会桌子，同步给工会服务器 [2017-8-3 willing]
		LMsgL2TS_NewTeamDesk newTeamDesk;
		newTeamDesk.nTeamID = pMsg->m_teamID;
		newTeamDesk.nDeskID = pMsg->m_deskID;
		newTeamDesk.nCreaterUserID = pUser->GetUserDataId();
		newTeamDesk.nMajongType = pMsg->m_state;
		newTeamDesk.nMaxCircle = pMsg->m_flag;
		newTeamDesk.nMaxUserCount = DEFAULT_DESK_USER_COUNT;
		newTeamDesk.nPlayTypeCount = pMsg->m_playTypeCount;
		//newTeamDesk.m_fastDesk = pMsg->m_maYouDesk == 1 ? 1 : 0;
		newTeamDesk.m_fastDesk = pMsg->m_robotNum >= 2 ? 1 : 0;
		for (int i = 0; i < newTeamDesk.nPlayTypeCount; i++)
		{
			newTeamDesk.tPlayType.push_back(pMsg->m_playtype[i]);
		}
		newTeamDesk.nCircleGoldLimit = pMsg->m_circleGoldLimit;
		newTeamDesk.nStake = pMsg->m_stake;
		gWork.SendMsgToTeamServer(newTeamDesk);
	}

	desk->SetStake(pMsg->m_stake);
	log->SetUserScore(pUser, pUser->m_userData.m_numOfCard2s);
	log->m_desk->OnUserInRoom(pUser);

	//判断是否需要加电脑， 增加电脑
	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
		{
			gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum, nAppId);
		}
	}

	return true;
}

VipLogItem* RoomVip::CreateMatchDesk(LMsg_Match2L_CreateMatchDesk* pMsg)
{
	if (pMsg->m_MaxCricle <= 0)
	{
		pMsg->m_MaxCricle = 1;
	}
	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateMatchDesk deskID is in Use");
		return NULL;
	}

	Desk* DeskNew = GetFreeDesk(pMsg->m_deskID, 20001);
	if (DeskNew == NULL)
	{//创建桌子失败
		LLOG_ERROR("RoomVip::CreateMatchDesk create Desk Failed!");
		return NULL;
	}

	DeskNew->SetDeskPlayerNum(pMsg->m_BigPlayType, 4);
	
	if (!DeskNew->CreateRegister((GameType)pMsg->m_BigPlayType))
	{//创建桌子失败
		LLOG_ERROR("RoomVip::CreateMatchDesk create Processor Failed!");
		return NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	VipLogItem* LogItemOut = gVipLogMgr.GetNewLogItem(pMsg->m_MaxCricle, -1);
	LogItemOut->m_curCircle = 0;
	LogItemOut->m_maxCircle = pMsg->m_MaxCricle;
	LogItemOut->m_flag = pMsg->m_BigPlayType;
	LogItemOut->m_deskId = pMsg->m_deskID;
	LogItemOut->m_playtype = pMsg->m_playtype;
	LogItemOut->m_MatchName = pMsg->m_MatchName;
	LogItemOut->m_curMatchId = pMsg->m_nMatchID;
	LogItemOut->m_nCurStage = pMsg->m_nCurStage;
	LogItemOut->m_nForceFinishTime = pMsg->m_nForceFinishTime;
	LogItemOut->m_IntoTuoGuanRemainTime = pMsg->m_IntoTuoGuanRemainTime;
	LogItemOut->m_InTuoGuanCanChuPaiTime = pMsg->m_InTuoGuanCanChuPaiTime;
	LogItemOut->m_MatchStartTime = pMsg->m_MatchStartTime;
	LogItemOut->m_nGroupID = pMsg->m_nGroupID;
	LogItemOut->m_CreaterID = pMsg->m_CreaterID;
	LogItemOut->m_MatchType = pMsg->m_MatchType;
	LogItemOut->m_nInitualDifen = pMsg->m_nInitualDifen;
	LogItemOut->m_nDifenPerMinIncStep = pMsg->m_nDifenPerMinIncStep;
	LogItemOut->m_nDingJuInitualDifen = pMsg->m_nDingJuInitualDifen;
	LogItemOut->SetAppId(20001);
	DeskNew->SetVip(LogItemOut);
	LogItemOut->m_desk = DeskNew;
	DeskNew->SetDeskPlayerNum(LogItemOut->m_flag, pMsg->m_UserCount);
	DeskNew->SetDeskCanInteraction(pMsg->m_canInteraction);

	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		for (Lint i = 0; i < CARD_COUNT; i++)
		{
			LogItemOut->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			LogItemOut->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}
	}
	// 	CardValue *p;
	// 	p = pMsg->m_cardValue;
	// 	LLOG_ERROR("%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d", p[0].m_color, p[0].m_number, p[1].m_color, p[1].m_number, p[2].m_color, p[2].m_number, p[3].m_color, p[3].m_number, p[4].m_color, p[4].m_number, p[5].m_color, p[5].m_number, p[6].m_color, p[6].m_number, p[7].m_color, p[7].m_number, p[8].m_color, p[8].m_number, p[9].m_color, p[9].m_number, p[10].m_color, p[10].m_number, p[11].m_color, p[11].m_number, p[12].m_color, p[12].m_number);
	// 	p+=13;
	// 	LLOG_ERROR("%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d", p[0].m_color, p[0].m_number, p[1].m_color, p[1].m_number, p[2].m_color, p[2].m_number, p[3].m_color, p[3].m_number, p[4].m_color, p[4].m_number, p[5].m_color, p[5].m_number, p[6].m_color, p[6].m_number, p[7].m_color, p[7].m_number, p[8].m_color, p[8].m_number, p[9].m_color, p[9].m_number, p[10].m_color, p[10].m_number, p[11].m_color, p[11].m_number, p[12].m_color, p[12].m_number);
	// 	p += 13;
	// 	LLOG_ERROR("%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d", p[0].m_color, p[0].m_number, p[1].m_color, p[1].m_number, p[2].m_color, p[2].m_number, p[3].m_color, p[3].m_number, p[4].m_color, p[4].m_number, p[5].m_color, p[5].m_number, p[6].m_color, p[6].m_number, p[7].m_color, p[7].m_number, p[8].m_color, p[8].m_number, p[9].m_color, p[9].m_number, p[10].m_color, p[10].m_number, p[11].m_color, p[11].m_number, p[12].m_color, p[12].m_number);
	// 	p += 13;
	// 	LLOG_ERROR("%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d", p[0].m_color, p[0].m_number, p[1].m_color, p[1].m_number, p[2].m_color, p[2].m_number, p[3].m_color, p[3].m_number, p[4].m_color, p[4].m_number, p[5].m_color, p[5].m_number, p[6].m_color, p[6].m_number, p[7].m_color, p[7].m_number, p[8].m_color, p[8].m_number, p[9].m_color, p[9].m_number, p[10].m_color, p[10].m_number, p[11].m_color, p[11].m_number, p[12].m_color, p[12].m_number);
	// 	p += 13;
	// 	LLOG_ERROR("%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d,%d%d", p[0].m_color, p[0].m_number, p[1].m_color, p[1].m_number, p[2].m_color, p[2].m_number, p[3].m_color, p[3].m_number, p[4].m_color, p[4].m_number, p[5].m_color, p[5].m_number, p[6].m_color, p[6].m_number, p[7].m_color, p[7].m_number, p[8].m_color, p[8].m_number, p[9].m_color, p[9].m_number, p[10].m_color, p[10].m_number, p[11].m_color, p[11].m_number, p[12].m_color, p[12].m_number);

	return LogItemOut;
}

bool RoomVip::AddToVipDesk(User* pUser, Lint nDeskID)
{
	if (!pUser)
	{
		return false;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = nDeskID;
	ret.m_errorCode = 0;

	if(pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk has desk");
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return false;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk )
	{
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return false;
	}

	if (!desk->GetVip() /*|| desk->GetVip()->IsBegin()*/)
	{
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return false;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetDeskPlayerNum() ||
		 desk->getDeskState() !=  DESK_WAIT)
	{
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return false;
	}
	if (desk->m_antiCheat && desk->isCheat(pUser)) {
		ret.m_errorCode = 4; /* suspicion of cheat */ 
		pUser->Send(ret);
		return false;
	}

	if (desk->GetDeskAppId() != pUser->m_userData.m_nAppId)
	{
		ret.m_errorCode = 5;
		pUser->Send(ret);
		return false;
	}

	if (desk->IsAaRoom() && pUser->m_userData.m_numOfCard2s < desk->GetAaRoomCostPlayerCardNum() )
	{
		ret.m_errorCode = 6;
		pUser->Send(ret);
		return false;
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
	return true;
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

	if(!desk->GetVip() /*|| desk->GetVip()->IsBegin()*/)
	{
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}
	pUser->Send(send);

	//房主
	if(desk->GetVip()->GetOwerId() ==  pUser->GetUserDataId() && !desk->IsFastPlay() && !pMsg->m_force && !desk->GetVip()->IsBegin())
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

		if (desk->IsFastPlay())
		{
			desk->FastPlayResetDesk();
		}
		//gWork.HanderUserLogout(pUser->getUserGateID() , pUser->GetUserGateUserId());
	}

	if (desk->GetVip() && desk->GetVip()->IsBegin())
	{
		//需要通知其他人其他情况
		desk->BrocastUserNoGold();
	}

	return true;
}
