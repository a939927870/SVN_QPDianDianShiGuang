#include "TeamInfo.h"
#include "DbServer.h"
#include "LLog.h"
#include "BaseCommonFunc.h"

// 存储 [2017-7-17 willing]
void STeamInfo::SaveTeamInfo()
{
	std::stringstream strStream;

	strStream << "update team set ";
	strStream << "Cards=" << nCards << ",";
	strStream << "CardAlert=" << nCardAlert << ",";
	strStream << "PerDayMemLimit=" << nMemCardLimitPerDay << ",";
	strStream << "TeamName= '" << strTeamName << "',";
	strStream << "boardInfo= '" << m_boardInfo << "',";

	Lstring sPlayTypes;
	tokenize_vitoa(vPlayTypes, sPlayTypes, ";");
	strStream << "playTypes= '" << sPlayTypes << "',";
	strStream << "deskCntLimit=" << nDeskCntLimit << ",";
	strStream << "deskProportionate=" << nDeskProportionate << ",";

	strStream << "TeamState=" << nState << " ";
	strStream << "where TeamID=" << nTeamID << ";";
	LLOG_ERROR("STeamInfo::SaveTeamInfo:%s", strStream.str().c_str());
	gDbServer.Push(move(strStream.str()), nTeamID);
}

// 成员信息存库 [2017-7-20 willing]
void STeamInfo::SaveMemInfo(Lint nUserID)
{
	tTeamMemberMap::iterator iter = m_tTeamMemberMap.find(nUserID);
	if ( iter == m_tTeamMemberMap.end() )
	{
		LLOG_ERROR("STeamInfo::SaveMemInfo not found user:%d,teamid=%d",nUserID,nTeamID);
		return;
	}

	STeamMember& member = iter->second;

	std::stringstream strStream;

	strStream << "update team_member set ";
	strStream << "UseCardNum=" << member.nTodayUserCardNum << ",";
	strStream << "TeamState=" << member.nTeamState << ",";
	strStream << "LastUserCardTime=" << member.nLastUserCardTime << " ";
	strStream << "where TeamID=" << member.nTeamID << " and ";
	strStream << "UserID=" << member.nUserID << ";";

	gDbServer.Push(move(strStream.str()), member.nTeamID);
}

// 删除成员 [2017-7-20 willing]
void STeamInfo::RmvMemInfo(Lint nUserID)
{
	std::stringstream strStream;

	strStream << "delete from team_member ";
	strStream << "where TeamID=" << nTeamID << " and ";
	strStream << "UserID=" << nUserID << ";";

	gDbServer.Push(move(strStream.str()), nTeamID);
}

// 获取指定的玩家在 工会中的状态 [2017-7-20 willing]
Lint STeamInfo::GetUserTeamState(Lint nUserID)
{
	tTeamMemberMap::iterator iter = m_tTeamMemberMap.find(nUserID);
	if ( iter != m_tTeamMemberMap.end() )
	{
		STeamMember& teamMember = iter->second;
		if (teamMember.nUserID == nUserID )
		{
			return iter->second.nTeamState;
		}
	}

	return ETMTS_NOT_EXIST;
}



// 存储 
void	STeamPlayConfig::SaveTeamPlayConfigInfo(Lint time)
{
	std::stringstream strStream;

	strStream << "insert into team_play_config(roomType,playEnum,playtype,teamID,userId,time,circleGoldLimit) value(";
	strStream << m_roomType << ",";
	strStream << m_playEnum << ",";
	for (Lint i = 0; i < m_playtype.size(); ++i)
	{
		strStream << m_playtype[i] << ";";
	}
	strStream << ",";
	strStream << m_teamID << ",";
	strStream << m_userID << ",";
	strStream << time << ",";
	strStream << m_circleGoldLimit<< ";";

	gDbServer.Push(move(strStream.str()), m_teamID);
}

// 成员信息存库 
void	STeamPlayConfig::UpdateTeamPlayConfigInfo(Lint time)
{
	std::stringstream strStream;

	strStream << "update team_play_config set ";
	strStream << "roomType=" << m_roomType << ",";
	strStream << "playEnum=" << m_playEnum << ",";
	strStream << "playtype=" << "'";
	for (Lint i = 0; i < m_playtype.size(); ++i)
	{
		strStream << m_playtype[i] << ";";
	}
	strStream << "',";
	strStream << "teamID=" << m_teamID << ",";
	strStream << "time=" << time << ", ";
	strStream << "userId=" << m_userID << ", ";
	strStream << "circleGoldLimit=" << m_circleGoldLimit << " ";
	strStream << "where id=" << m_id << ";";
	gDbServer.Push(move(strStream.str()), m_teamID);
}

// 删除成员 
void	STeamPlayConfig::RmvTeamPlayConfigInfo(Lint id)
{
	std::stringstream strStream;

	strStream << "delete from team_play_config ";
	strStream << "where id=" << id << ";";

	gDbServer.Push(move(strStream.str()), m_teamID);
}