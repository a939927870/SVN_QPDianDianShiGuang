#ifndef __TEAM_CHARGE_H__
#define __TEAM_CHARGE_H__
#include "LSingleton.h"

enum E_TEAM_CHARGE_REASON
{
	ETCR_CREATE_ADD_CARD,		// 工会创建者充卡 [2017-7-17 willing]
	ETCR_MEM_CREATE_DESK,		// 成员创建房间扣卡 [2017-7-25 willing]
};

class CTeamChargeLog : public LSingleton<CTeamChargeLog>
{
public:
	CTeamChargeLog();
	virtual ~CTeamChargeLog();
	virtual bool Init();
	virtual bool Final();

	void	AddTeamCard(Lint nTeamID, Lint nTeamMemID, Lint nCardCount, Lint nCardReason, Lint nBalance, Lint nAppID);

	void	DelCardLog(Lint nTeamID, Lint nTeamMemID, Lint nCardCount, Lint nBalance, Lint nAppID);
};

#define gTeamChargeLog CTeamChargeLog::Instance()

#endif