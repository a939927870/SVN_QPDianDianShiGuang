#ifndef _ARENA_H_
#define _ARENA_H_

#include "LBase.h"
#include "User.h"

//竞技场状态;
enum JJC_Status
{
	JJC_WaitStart = 0,
	JJC_Playing = 1,
	JJC_WaitEnd = 2
};

//竞技场类型;
enum JJC_Type
{
	JJC_Type_error = 0,
	JJC_Play_With_PC = 1,    //人机对战;
	JJC_Play_With_PERSON = 2
};

enum JJC_PrizeType
{
	JJC_Prize_ERROR = 0,
	JJC_Prize_MONEY = 1,
	JJC_Prize_DIAMOND = 2
};

struct JJC_Prize_Detail
{
	JJC_PrizeType m_nPrizeType;
	Lint  m_nPrizeNums;
};

struct JJC_Attribution
{
	Lint m_JJCTypeID = 0;
	Lint m_nlevel = 0;
	Lint m_nWinPlayNums = 0;
	JJC_Type m_gameType = JJC_Play_With_PC;

	Lint m_nStartTime = 0;
	std::vector<JJC_Prize_Detail> m_Prize;

	Lint m_nMaxPlayerCounts = 20;
	Lint m_nMinPlayerCounts = 1;
};

class Arena
{
public:
	//比赛场定时时间触发管理;
	void Tick();
	bool UserIntoTheArena();
	bool UserLeaveTheArena();

	void OneDeskEnd();                 //desk调用,游戏结果保存;

	Lint GetMaxPlayerCounts() { return m_pJJCAttribution->m_nMaxPlayerCounts;}
	Lint GetMinPlayerCounts() { return m_pJJCAttribution->m_nMinPlayerCounts;}
private:
	void ArenaStart();
	void ArenaEnd();     //竞技场结束，结果保存：

	JJC_Status m_JJCstatus;
	JJC_Attribution* m_pJJCAttribution;

	Lint m_nDeskSum = 1;
	Lint m_nEndDeskNum = 0;

	Lint m_nPlayerCountsInArena = 0;          
	std::vector<User*>  m_usersInArena;
};

#endif