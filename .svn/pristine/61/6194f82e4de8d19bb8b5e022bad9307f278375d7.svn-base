#ifndef _ARENA_MANAGER_H_
#define _ARENA_MANAGER_H_

#include "LBase.h"
#include "LSingleton.h"
#include "Arena.h"
#include "User.h"

class ArenaManager:public LSingleton<ArenaManager>
{
public:

	void ArenaConfigIni();
	//比赛场定时时间触发管理;
	void Tick();
	//玩家报名竞技场;分配玩家到具体的Arena;
	void DealUserSignUp4Arena(User* pUser);
	//玩家退出竞技场;
	bool DealUserExitFromArena(User* pUser);
	
	void SetOneArenaEnd();

private:
	//分配玩家到具体的Arena；
	void AddPlayer2Arena(User* pUser);
	//保存比赛场结果;
	void SaveArenaResult();
private:
	std::vector<JJC_Attribution>  m_vecJJC_Conig;
	std::vector<Arena*>  m_vecFreeArenas;
	std::vector<Arena*>  m_vecBusyArenas;
};
#define gArenaManager ArenaManager::Instance()

#endif