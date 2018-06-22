#ifndef _GATE_MANAGER_H_
#define _GATE_MANAGER_H_

#include "LBase.h"
#include "User.h"
#include "LSingleton.h"

struct UserExData
{
public:
	UserExData():m_GoldGameTimes(0), m_Date(0) {}
	UserExData(Lint nGoldGameTimes, Lint date):m_GoldGameTimes(nGoldGameTimes),m_Date(date){ }
	~UserExData() {}

	Lint    getLastGoldGameDate() { return m_Date; }
	Lint    getGoldGameTimesOfToday() { return m_GoldGameTimes; }
	void    setLastGoldGameDate(Lint date) { m_Date = date; }
	void    setGoldGameTimesOfToday(Lint nGoldGameTimes) { m_GoldGameTimes = nGoldGameTimes; }
	void    addGoldGameTimes(int addTimes) { m_GoldGameTimes += addTimes; }
private:
	Lint    m_GoldGameTimes;            //玩家当日金币场次数;
	Lint    m_Date;                     //最后一次金币场游戏日期;
};

class UserManager :public LSingleton<UserManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();

	void	Tick();

	User* GetUserByGateIdGateUserId(Lint gateId, Lint gateUserId);

	User* GetUserbyDataId(Lint dataId);

	void  AddUser(User* user);

	void  DelUser(User* user);

	void  AddUserIdName(UserIdInfo& u);

	UserIdInfo* GetUserIdInfo(Lint id);
	Lint		GetUserIdInfoSize() { return m_userIdInfo.size(); }

	Lint	GetGatUserCount(Lint gateId);
	
	void	BoadCast(LMsg& msg);

	//金币场相关;
	void    AddUser2Wait(User* user);
	void    delUserFromWait(User* user);
	void	UpdatePlayerTaskList();

	std::map<Lint, UserExData>	m_userExDataMap;
	void	SendOnlineUser2Team(LSocketPtr sp);
private:
	std::map<Llong, User*>	m_userMap;
	std::map<Lint, User*>	m_userDataIdMap;

	std::map<Lint,UserIdInfo>  m_userIdInfo;
	std::vector<Lint>       m_waitGoldGameUser;
};

#define gUserManager UserManager::Instance()

#endif