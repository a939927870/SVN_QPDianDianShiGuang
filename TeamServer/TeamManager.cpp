#include "TeamManager.h"
#include "Work.h"
#include "LLog.h"
#include "DbServer.h"
#include "TeamChargeLog.h"
#include "GateUserManager.h"
#include "Config.h"
#include <boost/regex.hpp>
#include "TeamErrorCode.h"
#include "AlertWork.h"
#include "DeskManager.h"
#include "BaseCommonFunc.h"
#include "HanderHttp.h"

CTeamManager::CTeamManager()
{
}


CTeamManager::~CTeamManager()
{
}

bool CTeamManager::Init()
{
	LoadAllTeamInfo();
	LoadAllTeamMember();
	LoadAllTeamPlayConfig();
	LoadAllTeamExchargeOpt();
	return true;
}

bool CTeamManager::Final()
{
	return true;
}

void CTeamManager::LoadAllTeamInfo()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::LoadAllTeamInfo mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while (true)
	{
		std::stringstream ss;
		ss << "select t.TeamID,t.CreaterID,t.adminId,t.CreaterName,u.HeadImageUrl,t.TeamName,t.Cards,t.PerDayMemLimit,t.CreateTime,t.TeamState,t.boardInfo,t.playTypes,t.manageId,t.countyId,t.deskCntLimit,t.deskProportionate ";
		ss << "from team t, user u where t.adminId = u.id ORDER BY t.Id DESC LIMIT ";
		ss << limitfrom << "," << count;

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CTeamManager::LoadAllTeamInfo sql error %s", mysql_error(m));
			break;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			break;
		}

		while (row)
		{
			++limitfrom;

			STeamInfo teamInfo;
			teamInfo.nTeamID = atoi(*row++);
			teamInfo.nCreaterID = atoi(*row++);
			teamInfo.nAdminID = atoi(*row++);
			teamInfo.strCreaterName = *row++;
			teamInfo.strCreaterImageUrl = *row++;
			teamInfo.strTeamName = *row++;
			teamInfo.nCards = atoi(*row++);
			teamInfo.nMemCardLimitPerDay = atoi(*row++);
			teamInfo.nCreateTime = atoi(*row++);
			teamInfo.nState = atoi(*row++);
			teamInfo.m_boardInfo = *row++;
			Lstring playTypes = *row++;
			tokenize_atovi(playTypes, teamInfo.vPlayTypes, ";");
			teamInfo.nManageId = atoi(*row++);
			teamInfo.nCountyId = atoi(*row++);
			teamInfo.nDeskCntLimit = atoi(*row++);
			teamInfo.nDeskProportionate = atoi(*row++);
			m_tTeamInfoMap.insert( std::make_pair(teamInfo.nTeamID, teamInfo) );

			// 记录该创建者 创建的所有 工会ID [2017-7-14 willing]
			if (m_tUserCreatTeamMap.find(teamInfo.nCreaterID) != m_tUserCreatTeamMap.end() )
			{
				m_tUserCreatTeamMap[teamInfo.nCreaterID].push_back(teamInfo.nTeamID);
			}
			else
			{
				tIntList intList;
				intList.push_back(teamInfo.nTeamID);
				m_tUserCreatTeamMap[teamInfo.nCreaterID] = intList;
			}

			m_tTeamNameIDMap.insert(std::make_pair(teamInfo.strTeamName, teamInfo.nTeamID));

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_DEBUG("CTeamManager::LoadAllTeamInfo team count %d", limitfrom);

	return;
}

void CTeamManager::GetAllTeamMember(Lint nTeamID, std::vector<STeamMember> & members)
{
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::LoadAllTeamMember mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while (true)
	{
		std::stringstream ss;
		ss << "select t.TeamID, t.UserID, u.Nike, u.HeadImageUrl, t.UseCardNum, ";
		ss << "t.TeamState,t.LastUserCardTime,t.JoinTime,t.ReqTime,t.NumsCard2 ";
		ss << "FROM team_member t, user u where t.UserID=u.Id ";
		ss << " AND t.TeamID = "<< nTeamID;
		ss << " ORDER BY t.Id DESC LIMIT ";
		ss << limitfrom << "," << count;

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CTeamManager::LoadAllTeamMember sql error %s", mysql_error(m));
			break;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			break;
		}

		while (row)
		{
			++limitfrom;

			STeamMember teamMember;
			teamMember.nTeamID = atoi(*row++);
			teamMember.nUserID = atoi(*row++);
			teamMember.strUserName = *row++;
			teamMember.strHeadImageUrl = *row++;
			teamMember.nTodayUserCardNum = atoi(*row++);
			teamMember.nTeamState = atoi(*row++);
			teamMember.nLastUserCardTime = atoi(*row++);
			teamMember.nJoinTime = atoi(*row++);
			teamMember.m_reqTime = atoi(*row++);
			teamMember.nNumOfCard2s = atof(*row++);
			members.push_back(teamMember);

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_DEBUG("CTeamManager::LoadAllTeamMember user count %d", limitfrom);

	return;
}

void CTeamManager::LoadAllTeamMember()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::LoadAllTeamMember mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while (true)
	{
		std::stringstream ss;
		ss << "select t.TeamID, t.UserID, u.Nike, u.HeadImageUrl, t.UseCardNum, ";
		ss << "t.TeamState,t.LastUserCardTime,t.JoinTime,t.ReqTime ";
		ss << "FROM team_member t, user u where t.UserID=u.Id ORDER BY t.Id DESC LIMIT ";
		ss << limitfrom << "," << count; 

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CTeamManager::LoadAllTeamMember sql error %s", mysql_error(m));
			break;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			break;
		}

		while (row)
		{
			++limitfrom;

			STeamMember teamMember;
			teamMember.nTeamID = atoi(*row++);
			teamMember.nUserID = atoi(*row++);
			teamMember.strUserName = *row++;
			teamMember.strHeadImageUrl = *row++;
			teamMember.nTodayUserCardNum = atoi(*row++);
			teamMember.nTeamState = atoi(*row++);
			teamMember.nLastUserCardTime = atoi(*row++);
			teamMember.nJoinTime = atoi(*row++);
			teamMember.m_reqTime = atoi(*row++);
			if ( !LTimeTool::IsInSameDay(teamMember.nLastUserCardTime, LTime().Secs()))
			{
				teamMember.nTodayUserCardNum = 0;
			}

			AddMemToTeam(teamMember);

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_DEBUG("CTeamManager::LoadAllTeamMember user count %d", limitfrom);

	return;
}

void CTeamManager::LoadAllTeamPlayConfig()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::LoadAllTeamPlayConfig mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while (true)
	{
		std::stringstream ss;
		ss << "select id,teamID,userId,roomType,playEnum,playType,time,circleGoldLimit ";
		ss << "FROM team_play_config ORDER BY Id DESC LIMIT ";
		ss << limitfrom << "," << count;

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CTeamManager::LoadAllTeamMember sql error %s", mysql_error(m));
			break;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			break;
		}

		while (row)
		{
			++limitfrom;

			STeamPlayConfig teamConfig;
			teamConfig.m_id = atoi(*row++);
			teamConfig.m_teamID = atoi(*row++);
			teamConfig.m_userID = atoi(*row++);
			teamConfig.m_roomType = atoi(*row++);
			teamConfig.m_playEnum = atoi(*row++);
			std::string tmp = *row++;
			if (!tmp.empty())
			{
				teamConfig.m_playtype = mySplit_int(tmp, ";");
			}
			
			teamConfig.m_time = atoi(*row++);
			teamConfig.m_circleGoldLimit = atoi(*row++);

			AddTeamPlayConfig(teamConfig);

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_DEBUG("CTeamManager::LoadAllTeamPlayConfig user count %d", limitfrom);

	return;
}

void CTeamManager::LoadAllTeamExchargeOpt()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::LoadAllTeamExchargeLog mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while (true)
	{
		std::stringstream ss;
		ss << "select Id,FromUserId,ToUserId,GoldNum,FromName,ToName,Opt,ReqTime,ProTime ";
		ss << "FROM team_exchageopt_log ORDER BY Id DESC LIMIT ";
		ss << limitfrom << "," << count;

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CTeamManager::LoadAllTeamMember sql error %s", mysql_error(m));
			break;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			break;
		}

		while (row)
		{
			++limitfrom;

			SExchageGoldOptLog * excharge = new SExchageGoldOptLog;
			SExchageGoldOptLog& teamExchage = *excharge;
			teamExchage.lLogId = atol(*row++);
			teamExchage.nFromUserId = atoi(*row++);
			teamExchage.nToUserId = atoi(*row++);
			teamExchage.nGoldNum = atoi(*row++);
			teamExchage.sFromName = *row++;
			teamExchage.sToName = *row++;
			teamExchage.nOpt = atoi(*row++);
			teamExchage.nReqTime = atoi(*row++);
			teamExchage.nProTime = atoi(*row++);

			AddTeamExchageOpt(excharge);

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
	}

	LLOG_DEBUG("CTeamManager::LoadAllTeamExchargeLog user count %d", limitfrom);
	return;
}


void CTeamManager::ModifyTeamData(LSocketPtr sp, Lint type, Lint createrId, Lint teamId, std::vector<Lint>& playTypes, Lstring& boardInfo, Lstring& strTeamName, Lint deskCntLimit, Lint deskProportionate)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(teamId);
	if (iter == m_tTeamInfoMap.end())
	{
		LLOG_ERROR("ModifyTeamData club, teamId not find! %d", teamId);
		gHanderHttp.SendHttpRet(EHEEC_TEAM_NOT_EXIST, sp);
		return;
	}

	STeamInfo& teamInfo = iter->second;
	if (teamInfo.nCreaterID != createrId)
	{
		LLOG_ERROR("ModifyTeamData club, createrId.length error!now %d  createid %d", createrId, teamInfo.nCreaterID);
		gHanderHttp.SendHttpRet(EHEEC_TEAM_CREATEID_ERR, sp);
		return;
	}

	switch (type)
	{
	case ETMOT_all:
	{
		teamInfo.vPlayTypes.clear();
		teamInfo.vPlayTypes.assign(playTypes.begin(), playTypes.end());
		teamInfo.nDeskCntLimit = deskCntLimit;
		teamInfo.nDeskProportionate = deskProportionate;
		teamInfo.m_boardInfo = boardInfo;
		//teamInfo.strTeamName = strTeamName;
		teamInfo.SaveTeamInfo();
		break;
	}
	case ETMOT_playTypes:
	{
		teamInfo.vPlayTypes.clear();
		teamInfo.vPlayTypes.assign(playTypes.begin(), playTypes.end());
		teamInfo.SaveTeamInfo();
		break;
	}
	case ETMOT_deskCntLimit:
	{
		teamInfo.nDeskCntLimit = deskCntLimit;
		teamInfo.SaveTeamInfo();
		break;
	}
	case ETMOT_deskProportionate:
	{
		teamInfo.nDeskProportionate = deskProportionate;
		teamInfo.SaveTeamInfo();
		break;
	}
	case ETMOT_boardInfo:
	{
		teamInfo.m_boardInfo = boardInfo;
		teamInfo.SaveTeamInfo();
		break;
	}
	/*case ETMOT_teamName:
	{
		teamInfo.strTeamName = strTeamName;
		teamInfo.SaveTeamInfo();
		break;
	}*/
	case ETMOT_playTypes_deskCntLimit:
	{
		teamInfo.vPlayTypes.clear();
		teamInfo.vPlayTypes.assign(playTypes.begin(), playTypes.end());
		teamInfo.nDeskCntLimit = deskCntLimit;
		teamInfo.SaveTeamInfo();
		break;
	}
	default:
		LLOG_ERROR("ModifyTeamData error type %d", type);
		gHanderHttp.SendHttpRet(EHEEC_PARAM_ERROR, sp);
		return;
	}

	LLOG_ERROR("ModifyTeamData send ok type %d", type);
	gHanderHttp.SendHttpRet(EHEEC_SUCCESS, sp);

	{
		LMsgS2CNoticeEvent noticeEvent;
		noticeEvent.nTeamID = teamId;
		noticeEvent.strCreaterName = teamInfo.strCreaterName;
		noticeEvent.strTeamName = teamInfo.strTeamName;
		noticeEvent.nOpt = 15;// 会长批准加入 [2017-7-20 willing] Lint nOpt = 0;				// 0 加入申请提交成功，1 会长批准加入，2 拒绝加入， 3 拒绝离开 4 会长同意离开，5 被会长移除 6 会长主动把玩家加入工会
		teamInfo.nMemCount++;

		STeamDescInfoMsg& teamDescInfo = noticeEvent.m_teamInfo;
		teamDescInfo.nTeamID = teamInfo.nTeamID;
		teamDescInfo.strTeamName = teamInfo.strTeamName;
		teamDescInfo.nCreaterID = teamInfo.nCreaterID;
		teamDescInfo.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
		teamDescInfo.strCreaterName = teamInfo.strCreaterName;
		teamDescInfo.nMemCount = teamInfo.nMemCount;
		teamDescInfo.nOnLienCount = teamInfo.nOnLineCount;
		teamDescInfo.m_isMaster = teamInfo.nAdminID == createrId ? 1 : 0;
		teamDescInfo.m_boardInfo = teamInfo.m_boardInfo;
		teamDescInfo.m_nCards = teamInfo.nCards;
		teamDescInfo.nReqJoinTime = teamInfo.nCreateTime;

		teamDescInfo.nManageId = teamInfo.nManageId;
		teamDescInfo.nCountyId = teamInfo.nCountyId;
		teamDescInfo.nDeskCntLimit = teamInfo.nDeskCntLimit;
		teamDescInfo.nDeskProportionate = teamInfo.nDeskProportionate;
		teamDescInfo.vPlayTypes.assign(teamInfo.vPlayTypes.begin(), teamInfo.vPlayTypes.end());

		/*auto it = m_tTeamPlayConfigMap.find(teamInfo.nTeamID);
		if (it != m_tTeamPlayConfigMap.end())
		{
			teamDescInfo.m_playConfig.m_roomType = it->second.m_roomType;
			teamDescInfo.m_playConfig.m_playEnum = it->second.m_playEnum;
			teamDescInfo.m_playConfig.m_teamID = it->second.m_teamID;
			teamDescInfo.m_playConfig.m_playtype = it->second.m_playtype;
			teamDescInfo.m_playConfig.m_nCreateTime = it->second.m_time;
		}*/

		gTeamManager.BroadcastTeamDeskMsg(noticeEvent.nTeamID, noticeEvent);
	}

}

Lint CTeamManager::CreateTeam(Lint nCreaterID, Lint nTeamId, const Lstring& strTeamName, const std::vector<Lint>& vPlayTypes, const Lstring& playTypes, Lint manageId, Lint countyId, Lint deskCntLimit, Lint deskProportionate)
{
	if ( 0 >= nCreaterID || 0 >=  nTeamId)
	{
		return -EHEEC_PARAM_ERROR;
	}

	if ( GetTeamCount(nCreaterID) >= gConfig.GetMaxCreateTeamCount())
	{
		LLOG_ERROR("Club------TeamCount > MaxCreateTeam!, create = %d", nCreaterID);
		return -EHEEC_CERATE_TEAM_MAX_COUNT;
	}

	if (m_tTeamInfoMap.find(nTeamId) != m_tTeamInfoMap.end())
	{
		LLOG_ERROR("Club------repeart teamid!, create = %d", nTeamId);
		return -EHEEC_PARAM_ERROR;
	}

	tStringIntMap::iterator iter_string_int = m_tTeamNameIDMap.find(strTeamName);
	if (iter_string_int != m_tTeamNameIDMap.end())
	{
		// 工会名称重复 [2017-7-25 willing]
		LLOG_ERROR("Club------TeamName was exist , create = %d", nCreaterID);
		return -EHEEC_TEAM_NAME;
	}

	//int nNewTeamID = GetRandInsertIDFromDB();
	int nNewTeamID = nTeamId;
	if ( nNewTeamID <= 0 )
	{
		LLOG_ERROR("CTeamManager::CreateTeam GetRandInsertIDFromDB return 0");
		return -EHEEC_PARAM_ERROR;
	}

	Lstring		nike;
	STeamInfo teamInfo;
	teamInfo.nTeamID = nNewTeamID;
	teamInfo.nCreaterID = nCreaterID;
	teamInfo.nAdminID = nCreaterID;
	teamInfo.strCreaterImageUrl = GetCreateHeadImageURL(nCreaterID, nike);
	teamInfo.strCreaterName = nike;
	teamInfo.strTeamName = strTeamName;
	teamInfo.nMemCardLimitPerDay = gConfig.GetMemPerDayLimit();
	teamInfo.nCards = 0;
	teamInfo.nCreateTime = LTime().Secs();

	//新加内容
	teamInfo.vPlayTypes.assign(vPlayTypes.begin(), vPlayTypes.end());   // 玩法内容
	teamInfo.nManageId = manageId;              //上级玩家id
	teamInfo.nCountyId = countyId;              //城市id
	teamInfo.nDeskCntLimit = deskCntLimit;          //开桌限制
	teamInfo.nDeskProportionate = deskProportionate;//抽成比例
	
	m_tTeamInfoMap.insert( std::make_pair(teamInfo.nTeamID, teamInfo) );

	// 记录该创建者 创建的所有 工会ID [2017-7-14 willing]
	if (m_tUserCreatTeamMap.find(teamInfo.nCreaterID) != m_tUserCreatTeamMap.end())
	{
		m_tUserCreatTeamMap[teamInfo.nCreaterID].push_back(teamInfo.nTeamID);
	}
	else
	{
		tIntList intList;
		intList.push_back(teamInfo.nTeamID);
		m_tUserCreatTeamMap[teamInfo.nCreaterID] = intList;
	}

	m_tTeamNameIDMap.insert(std::make_pair(teamInfo.strTeamName, teamInfo.nTeamID));

	// 存库 [2017-7-14 willing]
	std::stringstream ss;
	ss << "insert into team(TeamID,CreaterID,adminId,CreaterName,TeamName,Cards,PerDayMemLimit,CreateTime,TeamState,playTypes,manageId,countyId,deskCntLimit,deskProportionate) ";
	ss << "values(";
	ss << teamInfo.nTeamID << ",";
	ss << teamInfo.nCreaterID << ",";
	ss << teamInfo.nAdminID << ",";
	ss << "'" << teamInfo.strCreaterName << "',";
	//ss << "'" << teamInfo.strCreaterImageUrl << "',";
	ss << "'" << teamInfo.strTeamName << "',";
	ss << teamInfo.nCards << ",";
	ss << teamInfo.nMemCardLimitPerDay << ",";
	ss << teamInfo.nCreateTime << ",";
	ss << teamInfo.nState << ",'";
	ss << playTypes << "',";
	ss << teamInfo.nManageId << ",";
	ss << teamInfo.nCountyId << ",";
	ss << teamInfo.nDeskCntLimit << ",";
	ss << teamInfo.nDeskProportionate << ");";

	LLOG_ERROR("CTeamManager::CreateTeam sql:%s", ss.str().c_str());
	gDbServer.Push(move(ss.str()), teamInfo.nCreaterID);

	gTeamChargeLog.AddTeamCard(teamInfo.nTeamID,0, teamInfo.nCards, ETCR_CREATE_ADD_CARD, teamInfo.nCards, teamInfo.nServerID);

	LLOG_ERROR("CTeamManager::CreateTeam sucess teamid=%d,createrID=%d,adminID:%d,appid=%d", teamInfo.nTeamID, teamInfo.nCreaterID,teamInfo.nAdminID, teamInfo.nServerID);
	//将群主自己添加进自己的群里边
	STeamMember teamMember;
	teamMember.nTeamID = nNewTeamID;
	teamMember.nUserID = nCreaterID;
	teamMember.strUserName = nike;
	teamMember.strHeadImageUrl = teamInfo.strCreaterImageUrl;
	teamMember.nTodayUserCardNum = 0;
	teamMember.nGameState = ETMGS_OFFLINE;
	teamMember.nTeamState = ETMTS_NORMAL;
	teamMember.nLastUserCardTime = 0;
	teamMember.nJoinTime = LTime().Secs();
	gTeamManager.AddMemToTeam(teamMember, true);

	Llong nUnique = gGateUserManager.GetUnique(nCreaterID);
	if(nUnique) gTeamManager.SendTeamList(nUnique, ETMTS_NORMAL);

	return teamInfo.nTeamID;
}

const STeamInfo& CTeamManager::FindTeam(Lint nTeamID)
{
	static STeamInfo g_teamInfo;

	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if ( iter == m_tTeamInfoMap.end() )
	{
		return g_teamInfo;
	}

	return iter->second;
}

const STeamMember& CTeamManager::FindMember(Lint nTeamID, Lint nUserID)
{
	static STeamMember g_member;
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		return g_member;
	}

	STeamInfo& teamInfo = iter->second;

	tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
	if ( iter_mem == teamInfo.m_tTeamMemberMap.end() )
	{
		return g_member;
	}

	return iter_mem->second;
}

void CTeamManager::GetCreateTeamID(Lint nCreaterID, std::list<Lint>& tTeamIdVec)
{
	tTeamIdVec.clear();

	if ( m_tUserCreatTeamMap.empty() )
	{
		return;
	}

	tIntListMap::iterator iter = m_tUserCreatTeamMap.find(nCreaterID);
	
	if (iter == m_tUserCreatTeamMap.end())
		return;

	tIntList& createTeamList = iter->second;

	std::copy(createTeamList.begin(), createTeamList.end(), std::back_inserter(tTeamIdVec));

	return;
}

// 返回指定的玩家创建了多少个工会 [2017-7-14 willing]
Lint CTeamManager::GetTeamCount(Lint nCreateID)
{
	Lint nResult = 0;

	do 
	{
		if (m_tUserCreatTeamMap.find(nCreateID) == m_tUserCreatTeamMap.end() )
		{
			break;
		}

		nResult = m_tUserCreatTeamMap[nCreateID].size();
	} while (false);

	return nResult;
}

Lint CTeamManager::GetJoinCount(Lint nUserID)
{
	Lint nResult = 0;

	tIntListMap::iterator iter = m_tUserJoinTeamMap.find(nUserID);
	if ( iter != m_tUserJoinTeamMap.end() )
	{
		nResult = iter->second.size();
	}

	return nResult;
}

// 判断指定的玩家是否加入了指定的工会 [2017-7-14 willing]
bool CTeamManager::IsMember(Lint nTeamID, Lint nUserID)
{	
	
	if (m_tUserJoinTeamMap.find(nUserID) == m_tUserJoinTeamMap.end())
	{
		return false;
	}

	tIntList& memIdList = m_tUserJoinTeamMap[nUserID];

	tIntList::iterator iter = memIdList.begin();
	tIntList::iterator iend = memIdList.end();

	bool bResult = false;

	for (; iter != iend; iter++)
	{
		if (*iter == nTeamID)
		{
			bResult = true;
			break;
		}
	}
	
	return bResult;
}
bool CTeamManager::IsRealMember(Lint nTeamID, Lint nUserID)
{
	bool bResult = false;
	auto it = m_tTeamInfoMap.find(nTeamID);
	if (it != m_tTeamInfoMap.end())
	{
		STeamInfo& team = it->second;
		auto it_team = team.m_tTeamMemberMap.find(nUserID);
		if (it_team != team.m_tTeamMemberMap.end() && it_team->second.nTeamState == ETMTS_NORMAL)
			bResult = true;
	}
	return bResult;
}
// 获取指定的玩家在工会中的状态 [2017-7-19 willing]
Lint CTeamManager::GetMemStateInTeam(Lint nTeamID, Lint nUserID)
{
	Lint nState = ETMTS_NOT_EXIST;
	
	do 
	{
		tTeamInfoMap::iterator iter_team = m_tTeamInfoMap.find(nTeamID);
		if ( iter_team == m_tTeamInfoMap.end() )
		{
			break;
		}

		STeamInfo& teamInfo = iter_team->second;

		tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
		if ( iter_mem == teamInfo.m_tTeamMemberMap.end() )
		{
			break;
		}

		nState = iter_mem->second.nTeamState;

	} while (false);

	return nState;
}

Lint CTeamManager::AddMemToTeam(const STeamMember& teamMember, bool bNeedSave)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(teamMember.nTeamID);
	if (iter != m_tTeamInfoMap.end())
	{
		STeamInfo& teamInfo = iter->second;
		

		std::pair<tTeamMemberMap::iterator,bool> ret = teamInfo.m_tTeamMemberMap.insert(std::make_pair(teamMember.nUserID, teamMember));
		if ( ret.second )
		{
			if (teamMember.nTeamState == ETMTS_NORMAL)
			{
				teamInfo.nMemCount++;
			}
		}

		tIntListMap::iterator iter_list = m_tUserJoinTeamMap.find(teamMember.nUserID);
		if ( iter_list == m_tUserJoinTeamMap.end() )
		{
			tIntList joinList;
			joinList.push_back(teamMember.nTeamID);
			m_tUserJoinTeamMap.insert(std::make_pair(teamMember.nUserID, joinList));
		}
		else
		{
			iter_list->second.push_back(teamMember.nTeamID);
		}

		if (bNeedSave)
		{
			// 存库 [2017-7-19 willing]
			std::stringstream strStream;

			strStream << "insert into team_member(UserID,TeamID,UseCardNum,TeamState,LastUserCardTime,JoinTime,ReqTime) value(";
			strStream << teamMember.nUserID << ",";
			strStream << teamMember.nTeamID << ",";
			strStream << teamMember.nTodayUserCardNum << ",";
			strStream << teamMember.nTeamState << ",";
			strStream << teamMember.nLastUserCardTime << ",";
			strStream << teamMember.nJoinTime << ",";
			strStream << teamMember.m_reqTime << ");";
			LLOG_ERROR("CTeamManager::AddMemToTeam sql:%s", strStream.str().c_str());
			gDbServer.Push(move(strStream.str()), teamMember.nTeamID);
		}
	}
	else
	{
		LLOG_ERROR("CTeamManager::AddMemToTeam not found team:%d", teamMember.nTeamID);
		return EHEEC_TEAM_NOT_EXIST;
	}

	return EHEEC_SUCCESS;
}
Lstring CTeamManager::GetCreateHeadImageURL(Lint nCreaterID, Lstring& nike)
{
	MYSQL* m = gWork.GetDbSession().GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::GetCreateHeadImageURL mysql null");
		return "";
	}

	Lstring strCreateImagerURL;
	strCreateImagerURL = "";
	std::stringstream strStream;
	strStream << "select HeadImageUrl,nike from user where id=";
	strStream << nCreaterID << ";";
	if (mysql_real_query(m, strStream.str().c_str(), strStream.str().size()))
	{
		LLOG_ERROR("CTeamManager::GetCreateHeadImageURL  error: %s  Sql: %s", mysql_error(m), strStream.str().c_str());
		return "";
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("CTeamManager::GetCreateHeadImageURL  error: %s ", mysql_error(m));
		return "";
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("CTeamManager::GetCreateHeadImageURL  user Empty!");
		return "";
	}

	strCreateImagerURL = *row++;
	nike = *row;
	mysql_free_result(res);

	return strCreateImagerURL;
}
// 获取随机TeamID[2017-7-14 willing]
Lint CTeamManager::GetRandInsertIDFromDB()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::GetRandInsertIDFromDB mysql null");
		return 0;
	}

	Lint nid = 0;
	Lint nInsertID = 0;
	static Lstring  SQLSelect = "select Id, TeamId from team_id_table order by Id limit 1";
	if (mysql_real_query(m, SQLSelect.c_str(), SQLSelect.size()))
	{
		LLOG_ERROR("CTeamManager::GetRandInsertIDFromDB1  error: %s  Sql: %s", mysql_error(m), SQLSelect.c_str());
		return 0;
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("CTeamManager::GetRandInsertIDFromDB2  error: %s ", mysql_error(m));
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("CTeamManager::GetRandInsertIDFromDB3  user_id_table Empty!");
		return 0;
	}

	nid = atoi(*row++);
	nInsertID = atoi(*row);
	
	mysql_free_result(res);
	
	std::stringstream ss;
	ss << "delete from team_id_table where Id = " << nid;
	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB4 sql error: %s  Sql: %s", mysql_error(m), ss.str().c_str());
		return 0;
	}

	return nInsertID;
}

Lint CTeamManager::AddTeamCard(Lint nTeamID, Lint nCards, Lint nServerID)
{
	if ( 0 >= nCards )
	{
		return -EHEEC_PARAM_ERROR;
	}

	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if ( iter == m_tTeamInfoMap.end() )
	{
		return -EHEEC_TEAM_NOT_EXIST;
	}

	STeamInfo& teamInfo = iter->second;
	teamInfo.nCards += nCards;

	// 工会信息存库 [2017-7-17 willing]
	teamInfo.SaveTeamInfo();

	// 记录房卡变更日志 [2017-7-17 willing]
	gTeamChargeLog.AddTeamCard(nTeamID, 0, nCards, ETCR_CREATE_ADD_CARD,teamInfo.nCards, nServerID);

	// 返回修改成功后的房卡总数 [2017-7-17 willing]
	return teamInfo.nCards;

}

void CTeamManager::ModifyMemGameState(Lint nUserID, Lint nGameState)
{
	tIntListMap::iterator iter = m_tUserJoinTeamMap.find(nUserID);
	if ( iter != m_tUserJoinTeamMap.end() )
	{
		tIntList& joinTeamIdList = iter->second;
		tIntList::iterator iter_list = joinTeamIdList.begin();
		tIntList::iterator iend_list = joinTeamIdList.end();

		for (; iter_list != iend_list; iter_list++ )
		{
			tTeamInfoMap::iterator iter_map = m_tTeamInfoMap.find(*iter_list);
			if ( iter_map != m_tTeamInfoMap.end() )
			{
				STeamInfo& teamInfo = iter_map->second;
				tTeamMemberMap::iterator item_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
				if ( item_mem != teamInfo.m_tTeamMemberMap.end() )
				{
					STeamMember& member = item_mem->second;
					member.nGameState = nGameState;
					if ( ETMGS_OFFLINE == nGameState )
					{
						teamInfo.nOnLineCount--;
						member.nCareForDesk = 0;
					}
					else if ( ETMGS_ONLINE == nGameState)
					{
						teamInfo.nOnLineCount++;
					}
				}
			}
		}
	}
}

Lint CTeamManager::GetTeamMemCount(Lint nTeamID)
{
	const STeamInfo& teamInfo = FindTeam(nTeamID);
	if ( teamInfo.nTeamID == 0 )
	{
		return 0;
	}

	return teamInfo.nMemCount;
}

// 会长对成员的操作 [2017-7-20 willing]
Lint CTeamManager::TeamManagerOptMem(Lint nCreaterID, Lint nTeamID, Lstring strUserIDs, Lint nOpt, std::vector<std::string>& errorUserList)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if ( iter == m_tTeamInfoMap.end() )
	{
		return EHEEC_TEAM_NOT_EXIST;
	}
	
	STeamInfo& teamInfo = iter->second;

	if ( teamInfo.nCreaterID != nCreaterID )
	{
		return EHEEC_TEAM_CREATEID_ERR;
	}

	errorUserList.clear();

	std::vector<std::string> userIDs = split(strUserIDs,",");
	Lint nUserIDCount = userIDs.size();
	for ( int i=0; i<nUserIDCount; i++ )
	{
		Lint nUserID = atoi(userIDs[i].c_str());

		tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
		if (iter_mem == teamInfo.m_tTeamMemberMap.end())
		{
			LLOG_ERROR("CTeamManager::TeamManagerOptMem opt:%d user:%d not in team:%d",nOpt,nUserID,nTeamID);
			errorUserList.push_back(userIDs[i]);
			continue;
		}

		// 批准加入 [2017-7-20 willing]
		if ( nOpt == 1)
		{
			if ( (iter_mem->second.nTeamState == ETMTS_JOIN) && (teamInfo.nMemCount < gConfig.GetMaxTeamMemCount()) )
			{
				iter_mem->second.nTeamState = ETMTS_NORMAL;

				// 通知玩家 [2017-7-20 willing]
				LMsgS2CNoticeEvent noticeEvent;
				noticeEvent.nTeamID = nTeamID;
				noticeEvent.strCreaterName = teamInfo.strCreaterName;
				noticeEvent.strTeamName = teamInfo.strTeamName;
				noticeEvent.nOpt = 1;// 会长批准加入 [2017-7-20 willing]
				teamInfo.nMemCount++;

				STeamDescInfoMsg& teamDescInfo = noticeEvent.m_teamInfo;
				teamDescInfo.nTeamID = teamInfo.nTeamID;
				teamDescInfo.strTeamName = teamInfo.strTeamName;
				teamDescInfo.nCreaterID = teamInfo.nCreaterID;
				teamDescInfo.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
				teamDescInfo.strCreaterName = teamInfo.strCreaterName;
				teamDescInfo.nMemCount = teamInfo.nMemCount;
				teamDescInfo.nOnLienCount = teamInfo.nOnLineCount;
				teamDescInfo.m_isMaster = teamInfo.nAdminID == nUserID ? 1 : 0;
				teamDescInfo.m_boardInfo = teamInfo.m_boardInfo;
				teamDescInfo.m_nCards = teamInfo.nCards;
				teamDescInfo.nReqJoinTime = teamInfo.nCreateTime;

				teamDescInfo.nManageId = teamInfo.nManageId;
				teamDescInfo.nCountyId = teamInfo.nCountyId;
				teamDescInfo.nDeskCntLimit = teamInfo.nDeskCntLimit;
				teamDescInfo.nDeskProportionate = teamInfo.nDeskProportionate;
				teamDescInfo.vPlayTypes.assign(teamInfo.vPlayTypes.begin(), teamInfo.vPlayTypes.end());
				
				/*auto it = m_tTeamPlayConfigMap.find(teamInfo.nTeamID);
				if (it != m_tTeamPlayConfigMap.end())
				{
					teamDescInfo.m_playConfig.m_roomType = it->second.m_roomType;
					teamDescInfo.m_playConfig.m_playEnum = it->second.m_playEnum;
					teamDescInfo.m_playConfig.m_teamID = it->second.m_teamID;
					teamDescInfo.m_playConfig.m_playtype = it->second.m_playtype;
					teamDescInfo.m_playConfig.m_nCreateTime = it->second.m_time;
				}*/
				gWork.SendToGateByUserID(noticeEvent, nUserID);

				// 存库 [2017-7-20 willing]
				teamInfo.SaveMemInfo(nUserID);

				LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=1,teamid=%d, userid=%d", nTeamID, nUserID);

				// 记录操作日志 [2017-8-9 willing]
				SaveTameMemOptLog(nTeamID, nUserID, ETMOT_ALLOW_JOIN_TEAM);
			}
			else
			{
				errorUserList.push_back(userIDs[i]);
			}
		}
		// 拒绝加入 [2017-7-22 willing]
		else if ( nOpt == 2 )
		{
			// 通知玩家 [2017-7-20 willing]
			LMsgS2CNoticeEvent noticeEvent;
			noticeEvent.nTeamID = nTeamID;
			noticeEvent.strCreaterName = teamInfo.strCreaterName;
			noticeEvent.strTeamName = teamInfo.strTeamName;
			noticeEvent.nOpt = 2;// 会长删除 [2017-7-20 willing]

			gWork.SendToGateByUserID(noticeEvent, nUserID);

			teamInfo.m_tTeamMemberMap.erase(iter_mem);
			// 从数据库中删除 [2017-7-20 willing]
			teamInfo.RmvMemInfo(nUserID);

			// 从已加入的列表中移除 [2017-7-29 willing]
			tIntListMap::iterator iter_join = m_tUserJoinTeamMap.find(nUserID);
			if (iter_join != m_tUserJoinTeamMap.end())
			{
				tIntList& joinList = iter_join->second;
				for (tIntList::iterator iter_list = joinList.begin(); iter_list != joinList.end(); iter_list++)
				{
					if (*iter_list == nTeamID)
					{
						joinList.erase(iter_list);
						break;
					}
				}
			}

			LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=2,teamid=%d, userid=%d", nTeamID, nUserID);
			// 记录操作日志 [2017-8-9 willing]
			SaveTameMemOptLog(nTeamID, nUserID, ETMOT_REJECT_JOIN_TEAM);
		}
		// 拒绝离开 [2017-7-22 willing]
		else if (nOpt == 3)
		{
			if (teamInfo.nMemCount < gConfig.GetMaxTeamMemCount() )
			{
				iter_mem->second.nTeamState = ETMTS_NORMAL;
				teamInfo.SaveMemInfo(nUserID);

				teamInfo.nOnLineCount++;
				teamInfo.nMemCount++;

				// 通知玩家 [2017-7-20 willing]
				LMsgS2CNoticeEvent noticeEvent;
				noticeEvent.nTeamID = nTeamID;
				noticeEvent.strCreaterName = teamInfo.strCreaterName;
				noticeEvent.strTeamName = teamInfo.strTeamName;
				noticeEvent.nOpt = nOpt;
				STeamDescInfoMsg& teamDescInfo = noticeEvent.m_teamInfo;
				teamDescInfo.nTeamID = teamInfo.nTeamID;
				teamDescInfo.strTeamName = teamInfo.strTeamName;
				teamDescInfo.nCreaterID = teamInfo.nCreaterID;
				teamDescInfo.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
				teamDescInfo.strCreaterName = teamInfo.strCreaterName;
				teamDescInfo.nMemCount = teamInfo.nMemCount;
				teamDescInfo.nOnLienCount = teamInfo.nOnLineCount;
				teamDescInfo.m_isMaster = teamInfo.nAdminID == nUserID ? 1 : 0;
				teamDescInfo.m_boardInfo = teamInfo.m_boardInfo;
				teamDescInfo.m_nCards = teamInfo.nCards;
				teamDescInfo.nReqJoinTime = teamInfo.nCreateTime;

				teamDescInfo.nManageId = teamInfo.nManageId;
				teamDescInfo.nCountyId = teamInfo.nCountyId;
				teamDescInfo.nDeskCntLimit = teamInfo.nDeskCntLimit;
				teamDescInfo.nDeskProportionate = teamInfo.nDeskProportionate;
				teamDescInfo.vPlayTypes.assign(teamInfo.vPlayTypes.begin(), teamInfo.vPlayTypes.end());
				
				/*auto it = m_tTeamPlayConfigMap.find(teamInfo.nTeamID);
				if (it != m_tTeamPlayConfigMap.end())
				{
					teamDescInfo.m_playConfig.m_roomType = it->second.m_roomType;
					teamDescInfo.m_playConfig.m_playEnum = it->second.m_playEnum;
					teamDescInfo.m_playConfig.m_teamID = it->second.m_teamID;
					teamDescInfo.m_playConfig.m_playtype = it->second.m_playtype;
					teamDescInfo.m_playConfig.m_nCreateTime = it->second.m_time;
				}*/
				gWork.SendToGateByUserID(noticeEvent, nUserID);

				LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=3,teamid=%d, userid=%d", nTeamID, nUserID);
				// 记录操作日志 [2017-8-9 willing]
				SaveTameMemOptLog(nTeamID, nUserID, ETMOT_REJECT_LEAVE_TEAM);
			}
			else
			{
				errorUserList.push_back(userIDs[i]);
			}
			
		}
		// 同意离开 [2017-7-22 willing]
		else if (nOpt == 4 || nOpt == 5)
		{
			if ((iter_mem->second.nGameState == ETMGS_ONLINE) || (iter_mem->second.nGameState == ETMGS_GAME))
			{
				teamInfo.nOnLineCount--;
			}

			// 玩家申请离开的时候 成员数量已经修改过了 [2017-7-31 willing]
			if ( nOpt == 5 )
			{
				// 人数减少 [2017-7-29 willing]
				teamInfo.nMemCount--;
				if (teamInfo.nMemCount < 0)
				{
					teamInfo.nMemCount = 0;
				}
			}

			// 通知玩家 [2017-7-20 willing]
			LMsgS2CNoticeEvent noticeEvent;
			noticeEvent.nTeamID = nTeamID;
			noticeEvent.strCreaterName = teamInfo.strCreaterName;
			noticeEvent.strTeamName = teamInfo.strTeamName;
			noticeEvent.nOpt = nOpt;

			gWork.SendToGateByUserID(noticeEvent, nUserID);

			teamInfo.m_tTeamMemberMap.erase(iter_mem);
			// 从数据库中删除 [2017-7-20 willing]
			teamInfo.RmvMemInfo(nUserID);

			// 从已加入的列表中移除 [2017-7-29 willing]
			tIntListMap::iterator iter_join = m_tUserJoinTeamMap.find(nUserID);
			if ( iter_join != m_tUserJoinTeamMap.end() )
			{
				tIntList& joinList = iter_join->second;
				for (tIntList::iterator iter_list = joinList.begin(); iter_list != joinList.end(); iter_list++ )
				{
					if ( *iter_list == nTeamID )
					{
						joinList.erase(iter_list);
						break;
					}
				}
			}

			LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=%d,teamid=%d, userid=%d", nOpt, nTeamID, nUserID);

			// 记录操作日志 [2017-8-9 willing]
			if ( nOpt == 4 )
			{
				SaveTameMemOptLog(nTeamID, nUserID, ETMOT_ALLOW_LEAVE_TEAM);
			}
			else
			{
				SaveTameMemOptLog(nTeamID, nUserID, ETMOT_RMV_FROM_TEAM);
			}
			
		}
		else
		{
			return EHEEC_MEM_OPT_ERR;
		}
	}

	if (errorUserList.size() > 0 )
	{
		return EHEEC_MEM_OPT_NOTALL_SUC;
	}

	return EHEEC_SUCCESS;
}

// 会长对成员的操作 [2017-7-20 willing]
void CTeamManager::TeamManagerOptMem(LMsgC2SOptMember* msg, Llong nUserUnique)
{
	if (msg == NULL || nUserUnique == 0)
		return;
	Lint error = ETEC_SUCCESS;
	LMsgS2COptMemRet	send;
	send.m_teamId = msg->m_teamId;
	send.m_userId = msg->m_userId;
	send.m_opt = msg->m_opt;

	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(msg->m_teamId);
	if (iter == m_tTeamInfoMap.end())
	{
		send.m_error = ETEC_TEAM_NOT_EXIST;
		gWork.SendToGate(send, nUserUnique);
		return;
	}

	STeamInfo& teamInfo = iter->second;
	Lint	adminID = gGateUserManager.GetUserID(nUserUnique);
	if (teamInfo.nAdminID != adminID)
	{
		send.m_error = ETEC_TEAM_ADMIN_ERR;
		gWork.SendToGate(send, nUserUnique);
		return;
	}

	Lint nUserID = msg->m_userId;

	tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
	if (iter_mem == teamInfo.m_tTeamMemberMap.end())
	{
		LLOG_ERROR("CTeamManager::TeamManagerOptMem opt:%d user:%d not in team:%d", msg->m_opt, nUserID, msg->m_teamId);
		send.m_error = ETEC_TEAM_MEM_NOT_EXIST;
		gWork.SendToGate(send, nUserUnique);
		return;
	}
	Lint nOpt = msg->m_opt;
	// 批准加入 [2017-7-20 willing]
	if (nOpt == 1)
	{
		if ((iter_mem->second.nTeamState == ETMTS_JOIN) && (teamInfo.nMemCount < gConfig.GetMaxTeamMemCount()))
		{
			iter_mem->second.nTeamState = ETMTS_NORMAL;
			teamInfo.nMemCount++;
			// 通知玩家 [2017-7-20 willing]
			LMsgS2CNoticeEvent noticeEvent;
			noticeEvent.nTeamID = msg->m_teamId;
			noticeEvent.strCreaterName = teamInfo.strCreaterName;
			noticeEvent.strTeamName = teamInfo.strTeamName;
			noticeEvent.nOpt = 1;// 会长批准加入 [2017-7-20 willing]
			STeamDescInfoMsg& teamDescInfo = noticeEvent.m_teamInfo;
			teamDescInfo.nTeamID = teamInfo.nTeamID;
			teamDescInfo.strTeamName = teamInfo.strTeamName;
			teamDescInfo.nCreaterID = teamInfo.nCreaterID;
			teamDescInfo.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
			teamDescInfo.strCreaterName = teamInfo.strCreaterName;
			teamDescInfo.nMemCount = teamInfo.nMemCount;
			teamDescInfo.nOnLienCount = teamInfo.nOnLineCount;
			teamDescInfo.m_isMaster = teamInfo.nAdminID == nUserID ? 1 : 0;
			teamDescInfo.m_boardInfo = teamInfo.m_boardInfo;
			teamDescInfo.m_nCards = teamInfo.nCards;
			teamDescInfo.nReqJoinTime = teamInfo.nCreateTime;

			teamDescInfo.nManageId = teamInfo.nManageId;
			teamDescInfo.nCountyId = teamInfo.nCountyId;
			teamDescInfo.nDeskCntLimit = teamInfo.nDeskCntLimit;
			teamDescInfo.nDeskProportionate = teamInfo.nDeskProportionate;
			teamDescInfo.vPlayTypes.assign(teamInfo.vPlayTypes.begin(), teamInfo.vPlayTypes.end());
			
			/*auto it = m_tTeamPlayConfigMap.find(teamInfo.nTeamID);
			if (it != m_tTeamPlayConfigMap.end())
			{
				teamDescInfo.m_playConfig.m_roomType = it->second.m_roomType;
				teamDescInfo.m_playConfig.m_playEnum = it->second.m_playEnum;
				teamDescInfo.m_playConfig.m_teamID = it->second.m_teamID;
				teamDescInfo.m_playConfig.m_playtype = it->second.m_playtype;
				teamDescInfo.m_playConfig.m_nCreateTime = it->second.m_time;
			}*/
			gWork.SendToGateByUserID(noticeEvent, nUserID);

			// 存库 [2017-7-20 willing]
			teamInfo.SaveMemInfo(nUserID);

			

			LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=1,teamid=%d, userid=%d", msg->m_teamId, nUserID);

			// 记录操作日志 [2017-8-9 willing]
			SaveTameMemOptLog(msg->m_teamId, nUserID, ETMOT_ALLOW_JOIN_TEAM);
		}
	}
	// 拒绝加入 [2017-7-22 willing]
	else if (nOpt == 2 && iter_mem->second.nTeamState == ETMTS_JOIN)
	{
		// 通知玩家 [2017-7-20 willing]
		LMsgS2CNoticeEvent noticeEvent;
		noticeEvent.nTeamID = msg->m_teamId;
		noticeEvent.strCreaterName = teamInfo.strCreaterName;
		noticeEvent.strTeamName = teamInfo.strTeamName;
		noticeEvent.nOpt = 2;// 会长拒绝加入

		gWork.SendToGateByUserID(noticeEvent, nUserID);

		teamInfo.m_tTeamMemberMap.erase(iter_mem);
		// 从数据库中删除 [2017-7-20 willing]
		teamInfo.RmvMemInfo(nUserID);

		// 从已加入的列表中移除 [2017-7-29 willing]
		tIntListMap::iterator iter_join = m_tUserJoinTeamMap.find(nUserID);
		if (iter_join != m_tUserJoinTeamMap.end())
		{
			tIntList& joinList = iter_join->second;
			for (tIntList::iterator iter_list = joinList.begin(); iter_list != joinList.end(); iter_list++)
			{
				if (*iter_list == msg->m_teamId)
				{
					joinList.erase(iter_list);
					break;
				}
			}
		}

		LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=2,teamid=%d, userid=%d", msg->m_teamId, nUserID);
		// 记录操作日志 [2017-8-9 willing]
		SaveTameMemOptLog(msg->m_teamId, nUserID, ETMOT_REJECT_JOIN_TEAM);
	}
	// 拒绝离开 [2017-7-22 willing]
	else if (nOpt == 3)
	{
		if (teamInfo.nMemCount < gConfig.GetMaxTeamMemCount())
		{
			iter_mem->second.nTeamState = ETMTS_NORMAL;
			iter_mem->second.m_reqTime = 0;
			teamInfo.SaveMemInfo(nUserID);

			teamInfo.nOnLineCount++;
			teamInfo.nMemCount++;

			// 通知玩家 [2017-7-20 willing]
			LMsgS2CNoticeEvent noticeEvent;
			noticeEvent.nTeamID = msg->m_teamId;
			noticeEvent.strCreaterName = teamInfo.strCreaterName;
			noticeEvent.strTeamName = teamInfo.strTeamName;
			noticeEvent.nOpt = nOpt;
			STeamDescInfoMsg& teamDescInfo = noticeEvent.m_teamInfo;
			teamDescInfo.nTeamID = teamInfo.nTeamID;
			teamDescInfo.strTeamName = teamInfo.strTeamName;
			teamDescInfo.nCreaterID = teamInfo.nCreaterID;
			teamDescInfo.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
			teamDescInfo.strCreaterName = teamInfo.strCreaterName;
			teamDescInfo.nMemCount = teamInfo.nMemCount;
			teamDescInfo.nOnLienCount = teamInfo.nOnLineCount;
			teamDescInfo.m_isMaster = teamInfo.nAdminID == nUserID ? 1 : 0;
			teamDescInfo.m_boardInfo = teamInfo.m_boardInfo;
			teamDescInfo.m_nCards = teamInfo.nCards;
			teamDescInfo.nReqJoinTime = teamInfo.nCreateTime;

			teamDescInfo.nManageId = teamInfo.nManageId;
			teamDescInfo.nCountyId = teamInfo.nCountyId;
			teamDescInfo.nDeskCntLimit = teamInfo.nDeskCntLimit;
			teamDescInfo.nDeskProportionate = teamInfo.nDeskProportionate;
			teamDescInfo.vPlayTypes.assign(teamInfo.vPlayTypes.begin(), teamInfo.vPlayTypes.end());
		
			/*auto it = m_tTeamPlayConfigMap.find(teamInfo.nTeamID);
			if (it != m_tTeamPlayConfigMap.end())
			{
				teamDescInfo.m_playConfig.m_roomType = it->second.m_roomType;
				teamDescInfo.m_playConfig.m_playEnum = it->second.m_playEnum;
				teamDescInfo.m_playConfig.m_teamID = it->second.m_teamID;
				teamDescInfo.m_playConfig.m_playtype = it->second.m_playtype;
				teamDescInfo.m_playConfig.m_nCreateTime = it->second.m_time;
			}*/
			gWork.SendToGateByUserID(noticeEvent, nUserID);

			LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=3,teamid=%d, userid=%d", msg->m_teamId, nUserID);
			// 记录操作日志 [2017-8-9 willing]
			SaveTameMemOptLog(msg->m_teamId, nUserID, ETMOT_REJECT_LEAVE_TEAM);
		}

	}
	// 同意离开 [2017-7-22 willing]
	else if (nOpt == 4 || nOpt == 5)
	{
		if ((iter_mem->second.nGameState == ETMGS_ONLINE) || (iter_mem->second.nGameState == ETMGS_GAME))
		{
			teamInfo.nOnLineCount--;
		}

		// 玩家申请离开的时候 成员数量已经修改过了 [2017-7-31 willing]
		if (nOpt == 5)
		{
			// 人数减少 [2017-7-29 willing]
			teamInfo.nMemCount--;
			if (teamInfo.nMemCount < 0)
			{
				teamInfo.nMemCount = 0;
			}
		}

		// 通知玩家 [2017-7-20 willing]
		LMsgS2CNoticeEvent noticeEvent;
		noticeEvent.nTeamID = msg->m_teamId;
		noticeEvent.strCreaterName = teamInfo.strCreaterName;
		noticeEvent.strTeamName = teamInfo.strTeamName;
		noticeEvent.nOpt = nOpt;

		gWork.SendToGateByUserID(noticeEvent, nUserID);

		teamInfo.m_tTeamMemberMap.erase(iter_mem);
		// 从数据库中删除 [2017-7-20 willing]
		teamInfo.RmvMemInfo(nUserID);

		// 从已加入的列表中移除 [2017-7-29 willing]
		tIntListMap::iterator iter_join = m_tUserJoinTeamMap.find(nUserID);
		if (iter_join != m_tUserJoinTeamMap.end())
		{
			tIntList& joinList = iter_join->second;
			for (tIntList::iterator iter_list = joinList.begin(); iter_list != joinList.end(); iter_list++)
			{
				if (*iter_list == msg->m_teamId)
				{
					joinList.erase(iter_list);
					break;
				}
			}
		}

		LLOG_ERROR("CTeamManager::TeamManagerOptMem opt=%d,teamid=%d, userid=%d", nOpt, msg->m_teamId, nUserID);

		// 记录操作日志 [2017-8-9 willing]
		if (nOpt == 4)
		{
			SaveTameMemOptLog(msg->m_teamId, nUserID, ETMOT_ALLOW_LEAVE_TEAM);
		}
		else
		{
			SaveTameMemOptLog(msg->m_teamId, nUserID, ETMOT_RMV_FROM_TEAM);
		}

	}
	else
	{
		error = ETEC_TEAM_ADMIN_OPT_ERR;
	}
	
	send.m_error = error;
	gWork.SendToGate(send, nUserUnique);
	
}

// 玩家请求 工会列表 [2017-7-20 willing]
void CTeamManager::SendTeamList(Llong nUserUnique, Lint nState)
{
	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);
	Lint nOnUseTeamId = gGateUserManager.GetOnUseTeamIdByUserID(nUserUnique);
	tIntListMap::iterator iter = m_tUserJoinTeamMap.find(nUserID);

	LMsgS2CTeamList teamList;
	teamList.nReqState = nState;
	
	if ( iter == m_tUserJoinTeamMap.end() )
	{
		gWork.SendToGate(teamList, nUserUnique);
		return;
	}

	tIntList& TeamIdList = iter->second;
	tIntList::iterator iter_id = TeamIdList.begin();
	tIntList::iterator iend_id = TeamIdList.end();
	teamList.m_isInDesk = gDeskManager.IsInDesk(nUserID) ? 1 : 0;

	STeamDescInfoMsg teamDescInfo;
	for ( ; iter_id != iend_id; iter_id++ )
	{
		tTeamInfoMap::iterator iter_team = m_tTeamInfoMap.find(*iter_id);
		if ( iter_team != m_tTeamInfoMap.end() )
		{
			STeamInfo& teamInfo = iter_team->second;
			if ( teamInfo.GetUserTeamState(nUserID) == nState)
			{
				// 找到了条件的工会信息，放入到消息列表中 [2017-7-20 willing]
				teamDescInfo.nTeamID = teamInfo.nTeamID;
				teamDescInfo.strTeamName = teamInfo.strTeamName;
				teamDescInfo.nCreaterID = teamInfo.nAdminID;
				teamDescInfo.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
				teamDescInfo.strCreaterName = teamInfo.strCreaterName;
				teamDescInfo.nMemCount = teamInfo.nMemCount;
				teamDescInfo.nOnLienCount = teamInfo.nOnLineCount;
				teamDescInfo.m_isMaster = teamInfo.nAdminID == nUserID ? 1 : 0;
				teamDescInfo.m_boardInfo = teamInfo.m_boardInfo;
				teamDescInfo.m_nCards = teamInfo.nCards;
				teamDescInfo.nReqJoinTime = teamInfo.nCreateTime;
				
				teamDescInfo.nManageId = teamInfo.nManageId;
				teamDescInfo.nCountyId = teamInfo.nCountyId;
				teamDescInfo.nDeskCntLimit = teamInfo.nDeskCntLimit;
				teamDescInfo.nDeskProportionate = teamInfo.nDeskProportionate;
				teamDescInfo.vPlayTypes.assign(teamInfo.vPlayTypes.begin(), teamInfo.vPlayTypes.end());
				
				/*auto it = m_tTeamPlayConfigMap.find(teamInfo.nTeamID);
				if (it != m_tTeamPlayConfigMap.end())
				{
					teamDescInfo.m_playConfig.m_roomType = it->second.m_roomType;
					teamDescInfo.m_playConfig.m_playEnum = it->second.m_playEnum;
					teamDescInfo.m_playConfig.m_teamID = it->second.m_teamID;
					teamDescInfo.m_playConfig.m_playtype = it->second.m_playtype;
					teamDescInfo.m_playConfig.m_nCreateTime = it->second.m_time;
				}*/
				
				if (teamInfo.nTeamID == nOnUseTeamId)
				{
					teamList.teamInfoList.push_back(teamDescInfo);
					break;
				}
			}
		}
	}

	if (teamList.teamInfoList.size() == 0 && teamDescInfo.nTeamID)
	{
		teamList.teamInfoList.push_back(teamDescInfo);
	}
	
	LLOG_DEBUG("SendTeamList sendto client %d", nUserID);
	// 给玩家返回消息 [2017-7-20 willing]
	gWork.SendToGate(teamList, nUserUnique);
}

// 返回成员列表 [2017-7-21 willing]
void CTeamManager::SendMemberList(Lint nTeamID, Llong nUserUnique)
{
	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);

	if (!IsMember(nTeamID, nUserID))
	{
		return;
	}

	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		LMsgS2CMemberList memberlist;
		memberlist.nPackageIndex = 0;
		gWork.SendToGate(memberlist, nUserUnique);

		// 发送结束消息 [2017-8-11 willing]
		LMsgS2CMemberList memberlist_end;
		memberlist_end.nPackageIndex = -1;
		gWork.SendToGate(memberlist_end, nUserUnique);
		return;
	}

	// 每个包最大发送的成员个数 [2017-8-11 willing]
	const int nMaxSendMemCount = 4;

	STeamInfo& teamInfo = iter->second;

	//Lint nPackageCount = (teamInfo.nMemCount + nMaxSendMemCount -1 ) / nMaxSendMemCount;
	Lint nPackageCount = (teamInfo.m_tTeamMemberMap.size() + nMaxSendMemCount - 1) / nMaxSendMemCount;
	Lint nCurPackageIndex = 0;

	tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.begin();
	tTeamMemberMap::iterator iend_mem = teamInfo.m_tTeamMemberMap.end();

	std::vector<STeamMemberMsg> memberlistMsgList;
	int nPackageIndex = 0;
	for (; (iter_mem != iend_mem)&&(nPackageIndex < nPackageCount); iter_mem++)
	{
		const STeamMember& teamMember = iter_mem->second;
		/*if (teamMember.nTeamState != ETMTS_NORMAL)
		{
			continue;
		}*/

		STeamMemberMsg member;
		member.nUserID = teamMember.nUserID;
		member.strUserName = teamMember.strUserName;
		member.strHeadImageUrl = teamMember.strHeadImageUrl;
		member.nGameState = teamMember.nGameState;
		member.nTeamState = teamMember.nTeamState;
		member.m_joinTime = teamMember.nJoinTime;
		member.m_reqTime = teamMember.m_reqTime;
		memberlistMsgList.push_back(member);

		if (memberlistMsgList.size() == nMaxSendMemCount)
		{
			LMsgS2CMemberList memberlist;
			memberlist.nPackageIndex = nPackageIndex;
			memberlist.memberList.insert(memberlist.memberList.end(), memberlistMsgList.begin(), memberlistMsgList.end());
			gWork.SendToGate(memberlist, nUserUnique);
			memberlistMsgList.clear();
			nPackageIndex++;
		}
	}

	if (memberlistMsgList.empty())
	{
		LMsgS2CMemberList memberlist;
		memberlist.nPackageIndex = -1;
		gWork.SendToGate(memberlist, nUserUnique);
	}
	else
	{
		if (nPackageCount == 1 )
		{
			LMsgS2CMemberList memberlist;
			memberlist.nPackageIndex = 0;
			memberlist.memberList.insert(memberlist.memberList.end(), memberlistMsgList.begin(), memberlistMsgList.end());
			gWork.SendToGate(memberlist, nUserUnique);
			memberlistMsgList.clear();
		}
		
		LMsgS2CMemberList memberlist;
		memberlist.nPackageIndex = -1;
		memberlist.memberList.insert(memberlist.memberList.end(), memberlistMsgList.begin(), memberlistMsgList.end());
		gWork.SendToGate(memberlist, nUserUnique);
	}
}

// 取消加入工会 [2017-7-21 willing]
void CTeamManager::CancelJoin(Lint nTeamID, Llong nUserUnique)
{
	Lint nResult = ETEC_SUCCESS;
	
	Lstring strTeamName = "";
	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);

	do 
	{
		tTeamInfoMap::iterator iter_team = m_tTeamInfoMap.find(nTeamID);
		if (iter_team == m_tTeamInfoMap.end())
		{
			nResult = ETEC_TEAM_NOT_EXIST;
			break;
		}

		STeamInfo& teamInfo = iter_team->second;

		strTeamName = teamInfo.strTeamName;

		tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
		if (iter_mem == teamInfo.m_tTeamMemberMap.end())
		{
			nResult = ETEC_TEAM_MEM_NOT_EXIST;
			break;
		}

		if (iter_mem->second.nTeamState != ETMTS_JOIN )
		{
			nResult = ETEC_CANOT_CANCEL_JOIN;
			break;
		}

		// 从内存中删除 [2017-7-21 willing]
		teamInfo.m_tTeamMemberMap.erase(iter_mem);

		// 修改数据库 [2017-7-21 willing]
		teamInfo.RmvMemInfo(nUserID);

		RmvJoinTeamID(nUserID, nTeamID);

	} while (false);

	LMsgS2CCancelJoin retMsg;
	retMsg.nTeamID = nTeamID;
	retMsg.nResult = nResult;
	retMsg.strTeamName = strTeamName;

	gWork.SendToGate(retMsg, nUserUnique);

	if ( nResult == ETEC_SUCCESS )
	{
		SaveTameMemOptLog(nTeamID, nUserID, ETMOT_CANCEL_JOIN_TEAM);
	}
}

std::vector<std::string> CTeamManager::split(std::string str, std::string s)
{
	boost::regex reg(s.c_str());
	std::vector<std::string> vec;

	boost::sregex_token_iterator it(str.begin(), str.end(), reg, -1);
	boost::sregex_token_iterator end;

	while (it != end)
	{
		vec.push_back(*it++);
	}

	return vec;
}

std::vector<int> CTeamManager::mySplit_int(std::string str, std::string s)
{
	boost::regex reg(s.c_str());
	std::vector<int> vec;

	boost::sregex_token_iterator it(str.begin(), str.end(), reg, -1);
	boost::sregex_token_iterator end;

	while (it != end)
	{
		std::string tmp = *it++;
		if (!tmp.empty())
			vec.push_back(atoi(tmp.c_str()));
	}

	return vec;
}

// 修改工会的最低房卡警报值 [2017-7-22 willing]
Lint CTeamManager::SetMinCardAlert(Lint nCreaterID, Lint nTeamID, Lint nAlert)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if ( iter == m_tTeamInfoMap.end() )
	{
		return EHEEC_TEAM_NOT_EXIST;
	}

	STeamInfo& teamInfo = iter->second;

	if ( teamInfo.nCreaterID != nCreaterID )
	{
		return EHEEC_TEAM_CREATEID_ERR;
	}

	teamInfo.nCardAlert = nAlert;

	LLOG_ERROR("CTeamManager::SetMinCardAlert set card alert=%d",nAlert);

	teamInfo.SaveTeamInfo();

	return EHEEC_SUCCESS;
}

// 设置工会成员每日房卡最大消耗数量 [2017-7-22 willing]
Lint CTeamManager::SetPerDayLimit(Lint nCreaterID, Lint nTeamID, Lint nLimit)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		return EHEEC_TEAM_NOT_EXIST;
	}

	STeamInfo& teamInfo = iter->second;

	if (teamInfo.nCreaterID != nCreaterID)
	{
		return EHEEC_TEAM_CREATEID_ERR;
	}

	teamInfo.nMemCardLimitPerDay = nLimit;

	teamInfo.SaveTeamInfo();

	return EHEEC_SUCCESS;
}

// 删除工会 [2017-7-22 willing]
Lint CTeamManager::DelTeam(Lint nCreaterID, Lint nTeamID)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		return EHEEC_TEAM_NOT_EXIST;
	}

	STeamInfo& teamInfo = iter->second;

	if (teamInfo.nCreaterID != nCreaterID )
	{
		return EHEEC_TEAM_CREATEID_ERR;
	}

	tIntListMap::iterator iter_map = m_tUserJoinTeamMap.begin();
	tIntListMap::iterator iend_map = m_tUserJoinTeamMap.end();

	for ( ; iter_map != iend_map; iter_map++ )
	{
		Lint nUserID = iter_map->first;

		tIntList& teamIdList = iter_map->second;
		tIntList::iterator iter_list = teamIdList.begin();
		tIntList::iterator iend_list = teamIdList.end();
		for ( ;iter_list != iend_list; )
		{
			if ( *iter_list == nTeamID )
			{
				iter_list = teamIdList.erase(iter_list);

				// 通知该玩家工会被解散 [2017-7-22 willing]
				LMsgS2CNoticeTeamDismiss teamDismiss;
				teamDismiss.strCreaterName = teamInfo.strCreaterName;
				teamDismiss.strTeamName = teamInfo.strTeamName;
				teamDismiss.nTeamId = teamInfo.nTeamID;

				gWork.SendToGate(teamDismiss,gGateUserManager.GetUnique(nUserID));
			}
			else
			{
				iter_list++;
			}
		}
	}

	tStringIntMap::iterator iter_string_int = m_tTeamNameIDMap.find(teamInfo.strTeamName);
	if (iter_string_int != m_tTeamNameIDMap.end() )
	{
		m_tTeamNameIDMap.erase(iter_string_int);
	}

	// 内存中删除工会 [2017-7-22 willing]
	m_tTeamInfoMap.erase(iter);

	std::stringstream strStream;
	// 数据库中 删除工会所有成员 [2017-7-22 willing]
	strStream << "delete from team_member where TeamID=" << nTeamID << ";";
	gDbServer.Push(move(strStream.str()), nTeamID);

	// 数据库中 删除工会 [2017-7-22 willing]
	std::stringstream strStream2;
	strStream2 << "delete from team where TeamID=" << nTeamID << ";";
	gDbServer.Push(move(strStream2.str()), nTeamID);

	LLOG_ERROR("CTeamManager::DelTeam sucess teamid=%d",nTeamID);

	tIntListMap::iterator iter_createteam = m_tUserCreatTeamMap.find(nCreaterID);
	if ( iter_createteam != m_tUserCreatTeamMap.end() )
	{
		tIntList& createTeamIdList = iter_createteam->second;

		for ( auto iter_team_id = createTeamIdList.begin(); iter_team_id != createTeamIdList.end() ;)
		{
			if ( *iter_team_id == nTeamID )
			{
				iter_team_id = createTeamIdList.erase(iter_team_id);
			}
			else
			{
				iter_team_id++;
			}
		}
	}

	return EHEEC_SUCCESS;
}

// 删除一个 玩家已经加入的工会ID [2017-7-22 willing]
void CTeamManager::RmvJoinTeamID(Lint nUserID, Lint nTeamID)
{
	tIntListMap::iterator iter_map = m_tUserJoinTeamMap.find(nUserID);
	if ( iter_map == m_tUserJoinTeamMap.end() )
	{
		return;
	}

	tIntList& joinTeamIdList = iter_map->second;

	tIntList::iterator iter_list = joinTeamIdList.begin();
	tIntList::iterator iend_list = joinTeamIdList.end();

	for ( ; iter_list != iend_list; iter_list++ )
	{
		if ( *iter_list == nTeamID )
		{
			joinTeamIdList.erase(iter_list);
			break;
		}
	}
}

// 申请离开工会 [2017-7-22 willing]
void CTeamManager::RequestLeaveTeam(Lint nTeamID, Llong nUserUnique)
{
	Lint nResult = ETEC_SUCCESS;
	Lstring strTeamName = "";

	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);

	do 
	{
		if ( gDeskManager.IsInDesk(nUserID) )
		{
			nResult = ETEC_IN_DESK_LEAVE;
			break;
		}

		tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
		if (iter == m_tTeamInfoMap.end())
		{
			nResult = ETEC_TEAM_NOT_EXIST;
			break;
		}

		STeamInfo& teamInfo = iter->second;
		strTeamName = teamInfo.strTeamName;

		tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
		if (iter_mem == teamInfo.m_tTeamMemberMap.end())
		{
			nResult = ETEC_TEAM_MEM_NOT_EXIST;
			break;
		}

		STeamMember& teamMember = iter_mem->second;

		if (teamMember.nTeamState != ETMTS_NORMAL)
		{
			nResult = ETEC_CANOT_LEAVE_TEAM;
			break;
		}

		teamMember.nTeamState = ETMTS_LEAVE;
		teamMember.m_reqTime = LTime().Secs();
		if ((teamMember.nGameState == ETMGS_ONLINE) || (teamMember.nGameState == ETMGS_GAME))
		{
			teamInfo.nOnLineCount--;
		}

		teamInfo.SaveMemInfo(nUserID);

		teamInfo.nMemCount--;
		if ( teamInfo.nMemCount < 0 )
		{
			teamInfo.nMemCount = 0;
		}

	} while (false);

	LMsgS2CLeaveTeamRst leaveTeamRst;
	leaveTeamRst.nTeamID = nTeamID;
	leaveTeamRst.strTeamName = strTeamName;
	leaveTeamRst.nResult = nResult;

	gWork.SendToGate(leaveTeamRst, nUserUnique);

	LLOG_ERROR("Work::RequestLeaveTeam TeamID=%d UserID=%d", nTeamID, nUserID);

	if ( nResult == ETEC_SUCCESS )
	{
		// 记录操作日志 [2017-8-9 willing]
		gTeamManager.SaveTameMemOptLog(nTeamID, nUserID, ETMOT_REQ_LEAVE_TEAM);
	}
}
Lint	CTeamManager::CheckCanAddRoom(Lint userId,Lint deskId)
{
	Lint result = 0;
	if (userId <= 0)
		return 4;
	if (gWork.IsInMatchDesk(userId))
	{
		//已参加比赛场中
		return 100;
	}
	auto it = m_tUserJoinTeamMap.find(userId);
	if (it != m_tUserJoinTeamMap.end())
	{
		Lint teamId = gDeskManager.GetTeamIDByDeskID(deskId);
		Lint full_error = gDeskManager.CheckDeskFull(teamId, deskId);
		if (full_error != 0)
			result = full_error;
		Lint mem_state = GetMemStateInTeam(teamId, userId);
		if (result == 0 && mem_state != ETMTS_NORMAL)
		{
			if (mem_state == ETMTS_NOT_EXIST)
				result = 200;
			else
				result = ETEC_MEM_STATE_ERROR;
		}
		
		if (result == 0 && gDeskManager.IsInDesk(userId))
			result = ETEC_IN_DESK;
		if (result == 0)
		{
			result = 200;
			tIntList& joinTeamIdList = it->second;
			tIntList::iterator iter_id = joinTeamIdList.begin();
			tIntList::iterator iend_id = joinTeamIdList.end();
			for (; iter_id != iend_id; iter_id++)
			{
				if (*iter_id == teamId)
				{
					result = 0;
					break;
				}
			}
		}
		
	}
	else
	{
		result = 200;
	}
	return result;
}
// 检测是否可以创建房间 [2017-7-23 willing]
Lint CTeamManager::CheckCanCreateRoom(Lint nTeamID, Lint nUserID, Lint nFlag)
{
	Lint nResult = ETEC_SUCCESS;

	do 
	{
		tTeamInfoMap::iterator iter_map = m_tTeamInfoMap.find(nTeamID);
		if ( iter_map == m_tTeamInfoMap.end() )
		{
			nResult = ETEC_TEAM_NOT_EXIST;
			break;
		}

		STeamInfo& teamInfo = iter_map->second;

		tTeamMemberMap::iterator iter_team_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
		if ( iter_team_mem == teamInfo.m_tTeamMemberMap.end() )
		{
			nResult = ETEC_TEAM_MEM_NOT_EXIST;
			break;
		}
		if (iter_team_mem->second.nTeamState != ETMTS_NORMAL)
		{
			nResult = ETEC_MEM_STATE_ERROR;
			break;
		}
		if (gDeskManager.IsInDesk(nUserID))
		{
			nResult = ETEC_IN_DESK;
			break;
		}
		//// 获取需要消耗的房卡数量 [2017-7-23 willing]
		//Lint nNeedCardCount = GetCardCount(nFlag);
		//if ( nNeedCardCount == 0 )
		//{
		//	nResult = ETEC_JUSHU_ERROR;
		//	break;
		//}

		//// 判断房卡是否足够 [2017-7-23 willing]
		//if ( (teamInfo.nCards - teamInfo.nYuKouCard) < nNeedCardCount)
		//{
		//	nResult = ETEC_CARD_NOT_ENOUGH;
		//	LLOG_ERROR("CheckCanCreateRoom card not enough team realCards:%d,yuKouCards:%d,needCards:%d", teamInfo.nCards, teamInfo.nYuKouCard, nNeedCardCount);
		//	break;
		//}

		STeamMember& member = iter_team_mem->second;

		if ( member.nTeamState != ETMTS_NORMAL)
		{
			nResult = ETEC_LEAVE_CREATE_DESK;
			break;
		}

		// 判断使用的房卡数，是否超过上限 [2017-7-24 willing]
		if (!(LTimeTool::IsInSameDay(LTime().Secs(),member.nLastUserCardTime)))
		{
			member.nTodayUserCardNum = 0;

			LMsgS2CMemberInfoChange memInfoChage;
			memInfoChage.nTeamID = nTeamID;
			memInfoChage.nTodayUseCardCount = member.nTodayUserCardNum;
			gWork.SendToGateByUserID(memInfoChage, member.nUserID);
		}

		/*if ((teamInfo.nMemCardLimitPerDay > 0) && (teamInfo.nMemCardLimitPerDay - member.nTodayUserCardNum - nNeedCardCount) < 0 )
		{
			LLOG_ERROR("CTeamManager::CheckCanCreateRoom teamInfo.nMemCardLimitPerDay:%d,teamInfo.nMemCardLimitPerDay:%d,member.nTodayUserCardNum:%d,nNeedCardCount:%d", \
				teamInfo.nMemCardLimitPerDay, teamInfo.nMemCardLimitPerDay, member.nTodayUserCardNum, nNeedCardCount);
			nResult = ETEC_PERDAY_CARD_LIMIT;
			break;
		}*/

		if (gDeskManager.GetDeskCount(nTeamID) >= teamInfo.nDeskCntLimit)
		{
			LLOG_ERROR("CTeamManager::CheckCanCreateRoom teamInfo.nDeskCntLimit:%d, now nDeskCntLimit:%d", teamInfo.nDeskCntLimit, gDeskManager.GetDeskCount(nTeamID));
			nResult = ETEC_DESK_OVER_LIMIT;
			break;
		}
	} while (false);

	return nResult;
}

Lint CTeamManager::CheckCanLogin(Lint nUserID, Lint nOnUseTeamId, Lint& nTeamId)
{
	Lint nResult = ETEC_SUCCESS;

	do
	{
		auto iter_map = m_tTeamInfoMap.find(nOnUseTeamId);
		if (iter_map == m_tTeamInfoMap.end())
		{
			nResult = ETEC_TEAM_NOT_EXIST;
			break;
		}

		STeamInfo& teamInfo = iter_map->second;

		auto iter_team_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
		if (iter_team_mem == teamInfo.m_tTeamMemberMap.end())
		{
			nResult = ETEC_TEAM_MEM_NOT_EXIST;
			break;
		}

		if (iter_team_mem->second.nTeamState != ETMTS_NORMAL)
		{
			nResult = ETEC_MEM_STATE_ERROR;
			break;
		}

		if (gDeskManager.IsInDesk(nUserID, nTeamId))
		{
			if (nOnUseTeamId == nTeamId)
			{
				break;
			}
			nResult = ETEC_IN_DESK;
			break;
		}

	} while (0);

	return nResult;
}

//// 根据桌子最大人数，返回需要扣除的房卡数 [2017-7-23 willing]
//Lint CTeamManager::GetCardCount(int nMaxDeskUserCount)
//{
//	Lint nRet = 0;
//
//	switch (nMaxDeskUserCount)
//	{
//	case FRIEND_ROOM_TYPE_4:
//		nRet = 2;
//		break;
//	case FRIEND_ROOM_TYPE_8:
//		nRet = 4;
//		break;
//	case FRIEND_ROOM_TYPE_16:
//		nRet = 8;
//		break;
//	default:
//		break;
//	}
//
//	return nRet;
//}

// 预扣房卡 [2017-7-23 willing]
void CTeamManager::YuKouCard(Lint nTeamID, Lint nUserID, Lint nFlag, bool isFast)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		LLOG_ERROR("CTeamManager::YuKouCard not found team id=%d", nTeamID);
		return;
	}

	STeamInfo& teamInfo = iter->second;

	/*Lint nCardCount = GetCardCount(nFlag);
	if ( nCardCount <= 0 )
	{
		LLOG_ERROR("CTeamManager::YuKouCard GetCardCount=%d flag=%d", nCardCount, nFlag);
		return;
	}

	if ( (teamInfo.nCards - teamInfo.nYuKouCard) < nCardCount )
	{
		LLOG_ERROR("CTeamManager::YuKouCard teamInfo.nCards=%d teamInfo.nYuKouCard=%d nCardCount=%d", teamInfo.nCards, teamInfo.nYuKouCard, nCardCount);
		return;
	}*/

	tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
	if (iter_mem == teamInfo.m_tTeamMemberMap.end())
	{
		LLOG_ERROR("CTeamManager::YuKouCard not fount user ,teamid=%d, userId=%d", nTeamID, nUserID);
		return;
	}

	//teamInfo.nYuKouCard += nCardCount;
	/*if (!isFast)
	{
		STeamMember& member = iter_mem->second;
		member.nTodayUserCardNum += nCardCount;
		member.nLastUserCardTime = LTime().Secs();

		teamInfo.SaveMemInfo(nUserID);

		LMsgS2CMemberInfoChange memInfoChage;
		memInfoChage.nTeamID = nTeamID;
		memInfoChage.nTodayUseCardCount = member.nTodayUserCardNum;
		gWork.SendToGateByUserID(memInfoChage, member.nUserID);
	}*/
	
	//LLOG_ERROR("CTeamManager::YuKouCard TeamID=%d Card=%d", nTeamID, nCardCount);

	// 检测是否需要发送警报 [2017-7-25 willing]
	if ((teamInfo.nCards - teamInfo.nYuKouCard) < teamInfo.nCardAlert )
	{
		Lint nCurTimestamp = LTime().Secs();

		if ((nCurTimestamp - teamInfo.nSendAlertTime ) > 3600*12)
		{
			LMsgTeamCardAlert* pAlertMsg = new LMsgTeamCardAlert();
			pAlertMsg->nCreaterID = teamInfo.nCreaterID;
			pAlertMsg->nTeamID = teamInfo.nTeamID;
			pAlertMsg->strTeamName = teamInfo.strTeamName;

			gAlertWork.Push(pAlertMsg);
			teamInfo.nSendAlertTime = nCurTimestamp;
		}
	}
}

// 广播房卡改变 [2017-7-23 willing]
void CTeamManager::BroadcastTeamInfoChange(Lint nTeamID)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		return;
	}

	STeamInfo& teamInfo = iter->second;

	LMsgS2CTeamInfoChange teamInfoChgMsg;
	teamInfoChgMsg.nTeamID = nTeamID;
	teamInfoChgMsg.nCards = teamInfo.nCards - teamInfo.nYuKouCard;

	tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.begin();
	tTeamMemberMap::iterator iend_mem = teamInfo.m_tTeamMemberMap.end();

	for ( ; iter_mem != iend_mem; iter_mem++ )
	{
		STeamMember& member = iter_mem->second;
		if(member.nGameState == ETMGS_ONLINE)
			gWork.SendToGate(teamInfoChgMsg,gGateUserManager.GetUnique(member.nUserID));
	}
}

// 通知 退还预扣的卡 [2017-7-23 willing]
void CTeamManager::GivebackCard(Lint nTeamID, Lint nUserID, Lint nFlag,Lint nFast)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		LLOG_DEBUG("CTeamManager::GivebackCard not find team id=%d,", nTeamID);
		return;
	}

	/*Lint nCardCount = GetCardCount(nFlag);
	if (nCardCount <= 0)
	{
		LLOG_DEBUG("CTeamManager::GivebackCard GetCardCount ret %d,", nCardCount);
		return;
	}*/

	//STeamInfo& teamInfo = iter->second;

	//teamInfo.nYuKouCard = teamInfo.nYuKouCard - nCardCount;

	//// 广播房卡改变 [2017-7-23 willing]
	//BroadcastTeamInfoChange(nTeamID);

	if (nUserID == 0 || nFast == 1)
		return;
	/*tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
	if ( iter_mem != teamInfo.m_tTeamMemberMap.end() )
	{
		iter_mem->second.nTodayUserCardNum -= nCardCount;
		if (!(LTimeTool::IsInSameDay(LTime().Secs(), iter_mem->second.nLastUserCardTime)))
		{
			iter_mem->second.nTodayUserCardNum = 0;
			iter_mem->second.nLastUserCardTime = LTime().Secs();
		}

		teamInfo.SaveMemInfo(nUserID);

		LMsgS2CMemberInfoChange memInfoChage;
		memInfoChage.nTeamID = nTeamID;
		memInfoChage.nTodayUseCardCount = iter_mem->second.nTodayUserCardNum;
		gWork.SendToGateByUserID(memInfoChage, nUserID);
		
	}*/

	//LLOG_ERROR("CTeamManager::GivebackCard nTeamID=%d Card=%d", nTeamID,nCardCount);

	return;
}

// 实现真正的扣卡 [2017-7-23 willing]
void CTeamManager::RealDelCard(Lint nTeamID, Lint nUserID, Lint nFlag)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(nTeamID);
	if (iter == m_tTeamInfoMap.end())
	{
		return;
	}

	/*Lint nCardCount = GetCardCount(nFlag);
	if (nCardCount <= 0)
	{
		return;
	}*/

	/*STeamInfo& teamInfo = iter->second;
	teamInfo.nCards = teamInfo.nCards - nCardCount;
	teamInfo.nYuKouCard = teamInfo.nYuKouCard - nCardCount;

	teamInfo.SaveTeamInfo();*/

	/*gTeamChargeLog.DelCardLog(nTeamID, nUserID, nCardCount, teamInfo.nCards, teamInfo.nServerID);

	LLOG_ERROR("CTeamManager::RealDelCard nTeamID=%d Card=%d", nTeamID,nCardCount);*/

	return;
}

// 发送战绩信息 [2017-7-24 willing]
void CTeamManager::SendVideoLog(bool admin, Lint nTeamID, Lint nUserID, time_t nBeginTimestamp, time_t nEndTimestamp, Llong nUserUnique)
{
	// 获取查询者的ID [2017-7-24 willing]
	Lint nQueryUserID = gGateUserManager.GetUserID(nUserUnique);

	if ( !(IsMember(nTeamID,nUserID) && IsMember(nTeamID, nQueryUserID)) )
	{
		LMsgS2CVideoLog videoLog;
		videoLog.nPackageIndex = -2;
		gWork.SendToGate(videoLog, nUserUnique);
		return;
	}

	// 以下开始查询数据库 [2017-7-24 willing]

	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::SendVideoLog mysql null");
		LMsgS2CVideoLog videoLog;
		videoLog.nPackageIndex = -2;
		gWork.SendToGate(videoLog, nUserUnique);
		return;
	}

	while (true)
	{
		std::stringstream ss;
		ss << "select DeskID,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,Nike1,Nike2,Nike3,Nike4,OptState,PlayFlag,Time ";
		ss << "FROM team_log where Time>=" << nBeginTimestamp << " ";
		ss << "and Time < " << nEndTimestamp << " ";
		if (!admin)
		{
			ss << "and (UserId1=" << nUserID << " or ";
			ss << "UserId2=" << nUserID << " or ";
			ss << "UserId3=" << nUserID << " or ";
			ss << "UserId4=" << nUserID << ") ";
		}
		
		ss << "and TeamID=" << nTeamID << " ";
		ss << "ORDER BY Id DESC;";
		LLOG_DEBUG("video querry :%s", ss.str().c_str());
		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CTeamManager::SendVideoLog sql error %s", mysql_error(m));
			LMsgS2CVideoLog videoLog;
			videoLog.nPackageIndex = -2;
			gWork.SendToGate(videoLog, nUserUnique);
			return;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			break;
		}

		bool can_send = false;
		Lint paketIndex = 0;
		/*do 
		{
			can_send = false;
			LMsgTS2LMG_TeamVideoLog TeamVideoLog;
			TeamVideoLog.nPackageIndex = paketIndex++;
			TeamVideoLog.nRequestUserID = nQueryUserID;
			TeamVideoLog.nTeamID = nTeamID;
			TeamVideoLog.nLogCount = 0;

			while (row)
			{
				STeamVideoLogS2S videoLog;
				videoLog.nDeskID = atoi(*row++);
				videoLog.nUserID[0] = atoi(*row++);
				videoLog.nUserID[1] = atoi(*row++);
				videoLog.nUserID[2] = atoi(*row++);
				videoLog.nUserID[3] = atoi(*row++);
				videoLog.nScore[0] = atoi(*row++);
				videoLog.nScore[1] = atoi(*row++);
				videoLog.nScore[2] = atoi(*row++);
				videoLog.nScore[3] = atoi(*row++);
				videoLog.nOptState = atoi(*row++);
				videoLog.m_flag = atoi(*row++);
				videoLog.nTime = atoi(*row++);
				TeamVideoLog.nLogCount++;
				TeamVideoLog.teamVideoLog.push_back(videoLog);
				row = mysql_fetch_row(res);
				if (TeamVideoLog.nLogCount >= 20)
				{
					gWork.SendToLogicManager(TeamVideoLog);
					can_send = true;
					break;
				}
			}
			if (!can_send)
			{
				TeamVideoLog.nPackageIndex = -1;
				gWork.SendToLogicManager(TeamVideoLog);
			}
		} while (can_send);*/
		do
		{
			can_send = false;
			LMsgS2CVideoLog videoLog;
			videoLog.nPackageIndex = paketIndex++;
			Lint indexCount = 0;
			while (row)
			{
				STeamVideoLog videoLogItem;
				indexCount++;
				videoLogItem.nDeskID = atoi(*row++);
				videoLogItem.nUserID.push_back(atoi(*row++));
				videoLogItem.nUserID.push_back(atoi(*row++));
				videoLogItem.nUserID.push_back(atoi(*row++));
				videoLogItem.nUserID.push_back(atoi(*row++));
				videoLogItem.nScore.push_back(atoi(*row++));
				videoLogItem.nScore.push_back(atoi(*row++));
				videoLogItem.nScore.push_back(atoi(*row++));
				videoLogItem.nScore.push_back(atoi(*row++));
				videoLogItem.strUserName.push_back(*row++);
				videoLogItem.strUserName.push_back(*row++);
				videoLogItem.strUserName.push_back(*row++);
				videoLogItem.strUserName.push_back(*row++);
				videoLogItem.nOptState = atoi(*row++);
				videoLogItem.m_flag = atoi(*row++);
				videoLogItem.nTime = atoi(*row++);
				videoLog.teamVideoLog.push_back(videoLogItem);
				row = mysql_fetch_row(res);
				if (indexCount >= 20)
				{
					gWork.SendToGate(videoLog, nUserUnique);
					can_send = true;
					break;
				}
			}
			if (!can_send)
			{
				gWork.SendToGate(videoLog, nUserUnique);
			}
		} while (can_send);
		LMsgS2CVideoLog videoLog;
		videoLog.nPackageIndex = -1;
		gWork.SendToGate(videoLog, nUserUnique);
		mysql_free_result(res);
		break;
	}
}

// 获取 工会成员，当天已经使用了多少工会房卡 [2017-7-26 willing]
Lint CTeamManager::GetMemTodayUserCard(Lint nTeamID, Lint nUserID)
{
	Lint nCount = 0;
	tTeamInfoMap::iterator iter_team = m_tTeamInfoMap.find(nTeamID);
	if ( iter_team != m_tTeamInfoMap.end() )
	{
		STeamInfo& teamInfo = iter_team->second;

		tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
		if ( iter_mem != teamInfo.m_tTeamMemberMap.end() )
		{
			STeamMember& teamMember = iter_mem->second;

			if (!(LTimeTool::IsInSameDay(LTime().Secs(), teamMember.nLastUserCardTime)))
			{
				teamMember.nTodayUserCardNum = 0;
				teamMember.nLastUserCardTime = LTime().Secs();
				teamInfo.SaveMemInfo(nUserID);

				LMsgS2CMemberInfoChange memInfoChage;
				memInfoChage.nTeamID = nTeamID;
				memInfoChage.nTodayUseCardCount = teamMember.nTodayUserCardNum;
				gWork.SendToGateByUserID(memInfoChage, nUserID);
			}

			nCount = teamMember.nTodayUserCardNum;
		}
	}

	return nCount;
}

void CTeamManager::SetUserInfo(Lint nUserID, Lstring strNick, Lstring strHeadImageUrl)
{
	tIntListMap::iterator iter_team_id =  m_tUserJoinTeamMap.find(nUserID);
	if ( iter_team_id == m_tUserJoinTeamMap.end() )
	{
		LLOG_ERROR("CTeamManager::SetUserInfo not fount join team is list");
		return;
	}

	tIntList& joinTeamIdList = iter_team_id->second;
	tIntList::iterator iter_id = joinTeamIdList.begin();
	tIntList::iterator iend_id = joinTeamIdList.end();
	for ( ; iter_id != iend_id; iter_id++ )
	{
		int nTeamID = *iter_id;

		tTeamInfoMap::iterator iter_team = m_tTeamInfoMap.find(nTeamID);
		if ( iter_team == m_tTeamInfoMap.end() )
		{
			LLOG_ERROR("CTeamManager::SetUserInfo not fount team, id=%d", nTeamID);
			continue;
		}

		tTeamMemberMap::iterator iter_mem = iter_team->second.m_tTeamMemberMap.find(nUserID);
		if (iter_mem == iter_team->second.m_tTeamMemberMap.end() )
		{
			LLOG_ERROR("CTeamManager::SetUserInfo not fount user:%d in team:%d",nUserID, nTeamID);
			continue;
		}

		STeamMember& teamMember = iter_mem->second;
		teamMember.strUserName = strNick;
		teamMember.strHeadImageUrl = strHeadImageUrl;
	}
}

// 在工会中广播指定的消息 [2017-8-2 willing]
void CTeamManager::BroadcastTeamDeskMsg(Lint nTeamID, LMsg& msg)
{
	LLOG_ERROR("BroadcastTeamDeskMsg teamid %d ", nTeamID);
	tTeamInfoMap::iterator iter_team = m_tTeamInfoMap.find(nTeamID);
	if ( iter_team == m_tTeamInfoMap.end() )
	{
		LLOG_ERROR("CTeamManager::BroadcastMsg not found team,id=%d",nTeamID);
		return;
	}

	STeamInfo& teamInfo = iter_team->second;
	tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.begin();
	tTeamMemberMap::iterator iend_mem = teamInfo.m_tTeamMemberMap.end();

	for ( ; iter_mem != iend_mem; iter_mem++ )
	{
		STeamMember& member = iter_mem->second;

		if ( member.nTeamState != ETMTS_NORMAL)
		{
			member.nCareForDesk = 0;
			continue;
		}

		/*if ( member.nCareForDesk != 1 )
		{
			continue;
		}*/

		LLOG_ERROR("member userid %d gamestate %d", member.nUserID, member.nGameState);
		if ( (ETMGS_ONLINE == member.nGameState) || (ETMGS_GAME == member.nGameState) )
		{
			Llong nUnique = gGateUserManager.GetUnique(member.nUserID);
			if (0 == nUnique)
			{
				LLOG_ERROR("CTeamManager::BroadcastMsg not found user unique,userid=%d", member.nUserID);
				continue;
			}
			LLOG_ERROR("==BroadcastTeamDeskMsg userid %d", member.nUserID);
			gWork.SendToGate(msg, nUnique);
		}
	}

}

// 订阅/取消订阅桌子信息 [2017-8-3 willing]
void CTeamManager::SetCareForDesk(Lint nTeamID, Lint nUserID, Lint nCareFor)
{
	tTeamInfoMap::iterator iter_team = m_tTeamInfoMap.find(nTeamID);
	if (iter_team == m_tTeamInfoMap.end() )
	{
		LLOG_ERROR("CTeamManager::SetCareForDesk not found team,nTeamID=%d,nUserID=%d,nCareFor=%d", nTeamID, nUserID, nCareFor);
		return;
	}

	STeamInfo& teamInfo = iter_team->second;

	tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(nUserID);
	if ( iter_mem == teamInfo.m_tTeamMemberMap.end() )
	{
		LLOG_ERROR("CTeamManager::SetCareForDesk not found user,nTeamID=%d,nUserID=%d,nCareFor=%d", nTeamID, nUserID, nCareFor);
		return;
	}

	STeamMember& teamMember = iter_mem->second;

	teamMember.nCareForDesk = nCareFor;
}

// 玩家查询对工会的日志 [2017-8-8 willing]
void CTeamManager::SendTeamOptLog(Llong nUserUnique)
{
	LMsgS2CMemTeamOptLog teamOptLog;

	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);
	if (0 == nUserID)
	{
		LLOG_ERROR("CTeamManager::SendTeamOptLog UserID=0");
		gWork.SendToGate(teamOptLog, nUserUnique);
		return;
	}

	// 以下开始查询数据库 [2017-7-24 willing]

	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("CTeamManager::SendTeamOptLog mysql null");
		gWork.SendToGate(teamOptLog, nUserUnique);
		return;
	}
	
	while (true)
	{
		std::stringstream ss;
		ss << "select TeamID,TeamName,Opt,OptTime ";
		ss << "FROM team_mem_opt where UserID="<<nUserID<<" ORDER BY Id DESC;";

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("CTeamManager::SendTeamOptLog sql error %s", mysql_error(m));
			gWork.SendToGate(teamOptLog, nUserUnique);
			return;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if (!row)
		{
			break;
		}

		while (row)
		{
			STeamMemOptLog optLog;

			optLog.nTeamID = atoi(*row++);
			optLog.strTeamName = *row++;
			optLog.nOpt = atoi(*row++);
			optLog.nTimestamp = atoi(*row++);		// 时间戳 [2017-8-8 willing]

			teamOptLog.tAllTeamMemOptLog.push_back(optLog);

			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
		break;
	}

	gWork.SendToGate(teamOptLog, nUserUnique);
}

// 记录玩家对工会的操作日志 [2017-8-9 willing]
void CTeamManager::SaveTameMemOptLog(Lint nTeamID, Lint nUserID, Lint nOptType)
{
	const STeamInfo& teamInfo = FindTeam(nTeamID);
	if ( teamInfo.nTeamID != nTeamID )
	{
		LLOG_ERROR("CTeamManager::SaveTameMemOptLog not fount team,id=%d",nTeamID);
		return;
	}

	std::stringstream strSqlStream;
	strSqlStream << "insert into team_mem_opt(TeamID,TeamName,UserID,Opt,OptTime) value(";
	strSqlStream << nTeamID << ",";
	strSqlStream << "'"<< teamInfo.strTeamName << "',";
	strSqlStream << nUserID << ",";
	strSqlStream << nOptType << ",";
	strSqlStream << LTime().Secs() << ")";

	gDbServer.Push(move(strSqlStream.str()), nTeamID);
}


Lint CTeamManager::AddTeamPlayConfig(STeamPlayConfig& teamConfig, bool bNeedSave)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(teamConfig.m_teamID);
	//if (iter != m_tTeamInfoMap.end() && iter->second.nAdminID == teamConfig.m_userID)
	//{
	//	if (bNeedSave)
	//	{
	//		// 存库 [2017-7-19 willing]
	//		auto it = m_tTeamPlayConfigMap.find(teamConfig.m_teamID);
	//		if (it == m_tTeamPlayConfigMap.end())
	//		{
	//			MYSQL* m = gWork.GetDbSession().GetMysql();

	//			if (m == NULL)
	//			{
	//				LLOG_ERROR("CTeamManager::LoadAllTeamInfo mysql null");
	//				return 0;
	//			}
	//			std::stringstream ss;

	//			ss << "insert into team_play_config(roomType,playEnum,playtype,teamID,userId,time,circleGoldLimit) value(";
	//			ss << teamConfig.m_roomType << ",";
	//			ss << teamConfig.m_playEnum << ",";
	//			ss << "'";
	//			for (Lint i = 0; i < teamConfig.m_playtype.size(); ++i)
	//			{
	//				ss<<teamConfig.m_playtype[i] << ";";
	//			}
	//			ss << "',";
	//			ss << teamConfig.m_teamID << ",";
	//			ss << teamConfig.m_userID << ",";
	//			
	//			ss << teamConfig.m_time << ",";
	//			ss << teamConfig.m_circleGoldLimit << ");";
	//			if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	//			{
	//				LLOG_ERROR("CTeamManager::AddTeamPlayConfig sql error: %s  Sql: %s", mysql_error(m), ss.str().c_str());
	//				return 0;
	//			}
	//			teamConfig.m_id = (Lint)mysql_insert_id(m);
	//		}
	//		else
	//		{
	//			std::stringstream strStream;

	//			strStream << "update team_play_config set ";
	//			strStream << "roomType=" << teamConfig.m_roomType << ",";
	//			strStream << "playEnum=" << teamConfig.m_playEnum << ",";
	//			strStream << "playtype=" << "'";
	//			for (Lint i = 0; i < teamConfig.m_playtype.size(); ++i)
	//			{
	//				strStream << teamConfig.m_playtype[i] << ";";
	//			}
	//			strStream << "',";
	//			strStream << "teamID=" << teamConfig.m_teamID << ",";
	//			LTime time;
	//			strStream << "time = "<< time.Secs() << ",";
	//			strStream << "userId=" << teamConfig.m_userID << ",";
	//			strStream << "circleGoldLimit=" << teamConfig.m_circleGoldLimit << " ";
	//			
	//			strStream << "where teamID=" << teamConfig.m_teamID << ";";
	//			LLOG_DEBUG("sql:%s", strStream.str().c_str());
	//			gDbServer.Push(move(strStream.str()), teamConfig.m_teamID);
	//		}
	//		
	//		
	//	}
	//	//STeamInfo& teamInfo = iter->second;
	//	m_tTeamPlayConfigMap[teamConfig.m_teamID] = teamConfig;
	//	
	//}
	//else
	{
		LLOG_ERROR("CTeamManager::AddTeamPlayConfig not found team:%d", teamConfig.m_teamID);
		return 0;
	}

	return 1;
}

Lint CTeamManager::AddTeamExchageOpt(SExchageGoldOptLog* teamExcharge, bool bNeedSave)
{
	if (!teamExcharge) return 0;

	SExchageGoldOptLog &log = *teamExcharge;
	auto iter = m_tIdSExchageGoldOptLog.find(log.lLogId);
	if (iter == m_tIdSExchageGoldOptLog.end())
	{
		if (bNeedSave)
		{
			// 存库 [2017-7-19 willing]
			MYSQL* m = gWork.GetDbSession().GetMysql();

			if (m == NULL)
			{
				LLOG_ERROR("CTeamManager::LoadAllTeamInfo mysql null");
				return 0;
			}
			std::stringstream ss;

			ss << "insert into team_exchageopt_log(FromUserId,ToUserId,GoldNum,FromName,ToName,Opt,ReqTime,ProTime) value(";
			ss << log.nFromUserId << ",";
			ss << log.nToUserId << ",";
			ss << log.nGoldNum << ",'";
			ss << log.sFromName << "','";
			ss << log.sToName << "',";
			ss << log.nOpt << ",";
			ss << log.nReqTime << ",";
			ss << log.nProTime << ");";
			if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
			{
				LLOG_ERROR("CTeamManager::AddTeamPlayConfig sql error: %s  Sql: %s", mysql_error(m), ss.str().c_str());
				delete teamExcharge;
				teamExcharge = nullptr;
				return 0;
			}
			log.lLogId = (Lint)mysql_insert_id(m);
		}

		m_tIdSExchageGoldOptLog[log.lLogId] = teamExcharge;
		if (m_tFromSExchageGoldOptLog.find(log.nFromUserId) == m_tFromSExchageGoldOptLog.end())
		{
			m_tFromSExchageGoldOptLog[log.nFromUserId];
		}
		if (m_tToSExchageGoldOptLog.find(log.nToUserId) == m_tToSExchageGoldOptLog.end())
		{
			m_tToSExchageGoldOptLog[log.nToUserId];
		}
		m_tFromSExchageGoldOptLog[log.nFromUserId].push_back(teamExcharge);
		m_tToSExchageGoldOptLog[log.nToUserId].push_back(teamExcharge);
	}
	else
	{
		std::stringstream strStream;
		strStream << "update team_exchageopt_log set ";
		strStream << "Opt=" << log.nOpt << ",";
		strStream << "ProTime=" << gWork.GetCurTime().Secs() << " ";
		strStream << "where Id=" << log.lLogId << ";";
		LLOG_DEBUG("sql:%s", strStream.str().c_str());
		gDbServer.Push(move(strStream.str()), log.nFromUserId); //这里要是玩家本身的话
	}

	return 1;
}

SExchageGoldOptLog * CTeamManager::GetTeamExchageOpt(Llong Id)
{
	auto iter = m_tIdSExchageGoldOptLog.find(Id);
	if (iter != m_tIdSExchageGoldOptLog.end())
	{
		return iter->second;
	}
	return nullptr;
}

tC_SExchageGoldOptLogVec& CTeamManager::GetTeamExchageOptByToUserId(Lint userId)
{
	return m_tToSExchageGoldOptLog[userId];
}

Lint CTeamManager::HanderUserFastPlay(Lint teamId, Lint userId, Llong nUserUnique)
{
	LMsgS2CFastPlayRet send;
	Lint	error = ETEC_SUCCESS;
	//auto it = m_tTeamPlayConfigMap.find(teamId);
	//if (it != m_tTeamPlayConfigMap.end())
	{
		//STeamPlayConfig& config = it->second;
		error = CheckCanCreateRoom(teamId, userId,/* it->second.m_roomType*/0);
		if (error == 0 && gWork.IsInMatchDesk(userId))
		{
			error = ETEC_MEM_IN_MATCH;
			/*LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = 100;
			gWork.SendToGate(ret, nUserUnique);
			return 0;*/
		}
		if (error == 0)
		{
			bool isCreate = false;
			std::vector<Lint> deskIds;
			std::vector<Lint> limitGolds;
			Lint deskid = gDeskManager.GetFreeDeskId(isCreate,teamId, deskIds, limitGolds);
			if (isCreate)
			{
				//YuKouCard(teamId, userId, it->second.m_roomType, true);
			}
			LLOG_DEBUG("CTeamManager::HanderUserFastPlay userid:%d,deskid:%d,isCreate:%d", userId, deskid, isCreate ? 1 : 0);
			if (deskid != 0)
			{
				if (isCreate)
				{
					//LMsgTS2LMG_CreateDesk createDesk;
					//createDesk.m_circleGoldLimit = config.m_circleGoldLimit;
					//createDesk.m_userID = userId;
					//createDesk.m_playtypeCount = config.m_playtype.size();
					//createDesk.m_free = 0;

					//createDesk.m_flag = config.m_roomType;						//房间类型，4圈，8圈，12圈
					//createDesk.m_state = config.m_playEnum;						//玩法规则 0 转转   1 长沙	//贵州废弃
					//createDesk.m_robotNum = 0;									// 0,不加机器人，1，2，3加机器人数量

					//for (int i = 0; i < createDesk.m_playtypeCount; i++)
					//{
					//	createDesk.m_playtype.push_back(config.m_playtype[i]);
					//}

					//createDesk.m_useTeamCard = 1;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
					//createDesk.m_teamID = teamId;					// 工会ID [2017-7-23 willing]

					//const STeamInfo& teamInfo = gTeamManager.FindTeam(teamId);

					//createDesk.m_teamName = teamInfo.strTeamName;
					//createDesk.m_deskId = deskid;
					//createDesk.m_maYouDesk = 1;
					//gWork.SendToLogicManager(createDesk);
					//// 广播房卡改变 [2017-7-23 willing]
					//BroadcastTeamInfoChange(teamId);

					send.m_error = ETEC_NO_DESK;
				}
				else
				{
					LMsgTS2LMG_AddDesk send;
					//send.m_location = ;
					send.m_userID = userId;
					send.m_deskIds.assign(deskIds.begin(), deskIds.end());
					send.m_limitGolds.assign(limitGolds.begin(), limitGolds.end());
					gWork.SendToLogicManager(send);
				}
			}
			else
			{
				send.m_error = ETEC_NO_DESK;
			}
		}
		else
		{
			send.m_error = error;
		}
	}
	/*else
	{

		send.m_error = ETEC_TEAM_PLAY_CONFIG;

	}*/
	
	if(send.m_error)
		gWork.SendToGate(send, nUserUnique);
	return 0;
}

void CTeamManager::HanderOptTeamInfo(LMsgC2SOptTeamInfo* msg,Lint nUserID,Llong nUserUnique)
{
	LMsgS2CTeamInfo teamInfoMsg;
	
	auto it = m_tTeamInfoMap.find(msg->m_teamId);
	if (it != m_tTeamInfoMap.end() &&it->second.nAdminID == nUserID)
	{
		STeamInfo& teamInfo = it->second;
		if (!msg->m_teamName.empty())
		{
			teamInfo.strTeamName = msg->m_teamName;
		}
		if (!msg->m_boardInfo.empty())
		{
			teamInfo.m_boardInfo = msg->m_boardInfo;
		}
		teamInfoMsg.m_error = 0;
		teamInfoMsg.nTeamID = teamInfo.nTeamID;
		teamInfoMsg.strCreaterName = teamInfo.strCreaterName;
		teamInfoMsg.strTeamName = teamInfo.strTeamName;
		teamInfoMsg.nCards = teamInfo.nCards - teamInfo.nYuKouCard;
		teamInfoMsg.nCreateTime = teamInfo.nCreateTime;
		teamInfoMsg.nMemCardLimitPerDay = teamInfo.nMemCardLimitPerDay;
		teamInfoMsg.nState = teamInfo.nState;
		teamInfoMsg.nOnLineCount = teamInfo.nOnLineCount;
		teamInfoMsg.nMemCount = teamInfo.nMemCount;
		teamInfoMsg.m_boardInfo = teamInfo.m_boardInfo;
		teamInfo.SaveTeamInfo();
	}
	else
	{
		teamInfoMsg.m_error = ETEC_TEAM_NOT_EXIST;
	}
	gWork.SendToGate(teamInfoMsg, nUserUnique);
}
Lint CTeamManager::GetAdminIdbyTeamID(Lint nTeamID)
{
	auto it = m_tTeamInfoMap.find(nTeamID);
	if (it != m_tTeamInfoMap.end() )
	{
		return it->second.nAdminID;
	}
	return 0;
}

void CTeamManager::HanderRestDeskwithPerLimitUse(Lint teamID, Lint userID, Lint flag, bool is_fast)
{
	tTeamInfoMap::iterator iter = m_tTeamInfoMap.find(teamID);
	if (iter == m_tTeamInfoMap.end())
	{
		return;
	}

	STeamInfo& teamInfo = iter->second;

	
	/*Lint nCardCount = GetCardCount(flag);
	if (nCardCount <= 0)
	{
		LLOG_ERROR("CTeamManager::HanderRestDeskwithPerLimitUse  GetCardCount=%d flag=%d", nCardCount, flag);
		return;
	}
*/
	//teamInfo.nYuKouCard -= nCardCount;

	if (is_fast)
		return;

	/*tTeamMemberMap::iterator iter_mem = teamInfo.m_tTeamMemberMap.find(userID);
	if (iter_mem == teamInfo.m_tTeamMemberMap.end())
	{
		return;
	}
	STeamMember& member = iter_mem->second;
	member.nTodayUserCardNum -= nCardCount;
	member.nLastUserCardTime = LTime().Secs();
	teamInfo.SaveMemInfo(userID);*/
}

void CTeamManager::HanderTeamAdminInviteOther(LMsgC2SAdminInviteOther* msg, Llong nUserUnique)
{
	LMsgS2CAdminInviteOtherRet send;
	send.m_error = ETEC_SUCCESS;
	do
	{
		// 判断加入的工会是否超过了上限 [2017-7-21 willing]
		if (gTeamManager.GetJoinCount(msg->m_userid) >= gConfig.GetMemJoinTeamMaxCount())
		{
			send.m_error = ETEC_JOIN_MAX_TEAM;
			break;
		}

		// 判断指定的工会是否存在 [2017-7-19 willing]
		const STeamInfo& teamInfo = gTeamManager.FindTeam(msg->m_teamId);
		if (teamInfo.nTeamID == 0)
		{
			send.m_error = ETEC_TEAM_NOT_EXIST;
			break;
		}

		// 判断工会人数是否达到了上限 [2017-7-22 willing]
		if (teamInfo.nMemCount >= gConfig.GetMaxTeamMemCount())
		{
			send.m_error = ETEC_TEAM_MAX_COUNT;
			break;
		}

		// 判断是否已经加入了该工会 [2017-7-19 willing]
		Lint nState = gTeamManager.GetMemStateInTeam(msg->m_teamId, msg->m_userid);
		if (nState == ETMTS_JOIN)
		{
			send.m_error = ETEC_ALREADY_JOIN;
		}
		else if (nState == ETMTS_NORMAL)
		{
			send.m_error = ETEC_TEAM_NORMAL;
		}
		else if (nState == ETMTS_LEAVE)
		{
			send.m_error = ETEC_TEAM_LEAVE;
		}

	} while (false);

	if (send.m_error == ETEC_SUCCESS)
	{
		STeamMember member;
		member.nTeamID = msg->m_teamId;
		member.nUserID = msg->m_userid;
		member.nTodayUserCardNum = 0;
		member.nGameState = ETMGS_ONLINE;
		member.nTeamState = ETMTS_NORMAL;
		member.nLastUserCardTime = 0;
		member.nJoinTime = LTime().Secs();
		member.m_reqTime = 0;
		gTeamManager.AddMemToTeam(member, true);

		LLOG_ERROR("Work::HanderTeamAdminInviteOther TeamID=%d UserID=%d", msg->m_teamId, msg->m_userid);

		// 向LogicManager请求玩家的头像和昵称
		LMsgTS2LMG_GetUserInfo getUserInfo;
		getUserInfo.nUserID = msg->m_userid;
		gWork.SendToLogicManager(getUserInfo);

		// 记录操作日志 [2017-8-9 willing]
		SaveTameMemOptLog(msg->m_teamId, msg->m_userid, ETMOT_ALLOW_JOIN_TEAM);
	}

	gWork.SendToGate(send, nUserUnique);
}

bool CTeamManager::SendAllExchageGoldOptLog(Lint userID)
{
	LMsgS2CExchageGoldOptLog send;
	Lint sz = m_tFromSExchageGoldOptLog[userID].size();
	auto iter = m_tFromSExchageGoldOptLog[userID];
	for (Lint i = sz - 1; i > sz - 11 && i >= 0; --i)
	{
		send.tAllExchageGoldOptLog.push_back(*iter[i]);
	}
	gWork.SendToGateByUserID(send, userID);
	return true;
}

bool CTeamManager::SendInviteInRoom(Lint userId, Lint memberId)
{
	Lint nteamid = 0;
	Lint ndeskid = 0;
	if (!gDeskManager.GetDeskInfoByUserId(userId, nteamid, ndeskid))
	{
		LLOG_ERROR("SendInviteInRoom not find deskid userId %d", ndeskid);
		return false;
	}

	const STeamMember& meinfo = gTeamManager.FindMember(nteamid, userId);
	if (meinfo.nUserID == 0)
	{
		LLOG_ERROR("CTeamManager::SendInviteInRoom not found meinfo %d", userId);
		return false;
	}

	if (memberId > 0)
	{
		const STeamMember& memberinfo = gTeamManager.FindMember(nteamid, memberId);
		if (memberinfo.nUserID == 0)
		{
			LLOG_ERROR("CTeamManager::SendInviteInRoom not found memberid %d", memberId);
			return false;
		}

		Llong nUnique = gGateUserManager.GetUnique(memberinfo.nUserID);
		if (0 == nUnique)
		{
			LLOG_ERROR("CTeamManager::SendInviteInRoom not found user unique,userid=%d", memberinfo.nUserID);
			return false;
		}

		if (gDeskManager.IsInDesk(memberinfo.nUserID))
		{
			LLOG_ERROR("CTeamManager::SendInviteInRoom IsInDesk,userid=%d", memberinfo.nUserID);
			return false;
		}

		LMsgS2CInviteInRoom msg;
		msg.fromId = userId;
		msg.sFromName = meinfo.strUserName;
		msg.deskId = ndeskid;
		gWork.SendToGate(msg, nUnique);
		return true;
	}

	//所有成员邀请
	const STeamInfo& teamInfo = gTeamManager.FindTeam(nteamid);
	if (teamInfo.nTeamID != nteamid)
	{
		LLOG_ERROR("CTeamManager::SendInviteInRoom not find teamid %d", nteamid);
		return false;
	}

	Lint nTotalMemCount = 0;

	tTeamMemberMap::const_iterator iter_mem = teamInfo.m_tTeamMemberMap.begin();
	tTeamMemberMap::const_iterator iend_mem = teamInfo.m_tTeamMemberMap.end();

	for (; iter_mem != iend_mem; iter_mem++)
	{
		const STeamMember& teamMember = iter_mem->second;
		if (teamMember.nTeamState != ETMTS_NORMAL)
		{
			continue;
		}

		if (gDeskManager.IsInDesk(teamMember.nUserID))
		{
			continue;
		}

		Llong nUnique = gGateUserManager.GetUnique(teamMember.nUserID);
		if (0 == nUnique)
		{
			continue;
		}

		LMsgS2CInviteInRoom msg;
		msg.fromId = userId;
		msg.sFromName = meinfo.strUserName;
		msg.deskId = ndeskid;
		gWork.SendToGate(msg, nUnique);
	}
	return true;
}
