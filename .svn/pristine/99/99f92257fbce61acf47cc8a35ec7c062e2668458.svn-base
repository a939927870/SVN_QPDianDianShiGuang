#include "TeamChargeLog.h"
#include "DbServer.h"
#include "LTime.h"

CTeamChargeLog::CTeamChargeLog()
{
}


CTeamChargeLog::~CTeamChargeLog()
{
}

bool CTeamChargeLog::Init()
{
	return true;
}

bool CTeamChargeLog::Final()
{
	return true;
}

void CTeamChargeLog::AddTeamCard(Lint nTeamID, Lint nTeamMemID, Lint nCardCount, Lint nCardReason, Lint nBalance, Lint nAppID)
{
	LTime curTime;
	char szTeam[20] = { 0 };
	sprintf(szTeam, "%02d%02d", curTime.GetYear(), curTime.GetMonth() + 1);

	std::stringstream strStream;

	strStream << "insert into team_charge (ServerId,TeamId,TeamMemId,Time,CardNum,Type,Balance,Operator,Remark) value(";
	strStream << nAppID << ",";
	strStream << nTeamID << ",";
	strStream << nTeamMemID << ",";
	strStream << curTime.Secs() << ",";
	/*strStream << nCardCount << ",";*/
	if (ETCR_MEM_CREATE_DESK == nCardReason)
	{
		strStream << 0 - nCardCount << ",";
	}
	else
	{
		strStream << nCardCount << ",";
	}
	strStream << nCardReason << ",";
	strStream << nBalance << ",";
	strStream << "''" << ",";
	strStream << "''" << ");";

	gDbServer.Push(move(strStream.str()), nTeamID);
}

void CTeamChargeLog::DelCardLog(Lint nTeamID, Lint nTeamMemID, Lint nCardCount, Lint nBalance, Lint nAppID)
{
	LTime curTime;
	char szTeam[20] = { 0 };
	sprintf(szTeam, "%02d%02d", curTime.GetYear(), curTime.GetMonth() + 1);

	std::stringstream strStream;

	strStream << "insert into team_charge (ServerId,TeamId,TeamMemId,Time,CardNum,Type,Balance,Operator,Remark) value(";
	strStream << nAppID << ",";
	strStream << nTeamID << ",";
	strStream << nTeamMemID << ",";
	strStream << curTime.Secs() << ",";
	//strStream << nCardCount << ",";
	strStream << 0 - nCardCount << ",";
	strStream << ETCR_MEM_CREATE_DESK << ",";
	strStream << nBalance << ",";
	strStream << "''" << ",";
	strStream << "''" << ");";

	gDbServer.Push(move(strStream.str()), nTeamID);
}
