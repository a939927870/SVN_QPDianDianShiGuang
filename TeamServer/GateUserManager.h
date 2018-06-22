#ifndef __GATE_USER_MGR_H__
#define __GATE_USER_MGR_H__
// 在线玩家数据缓存 [2017-7-18 willing]
#include <map>
#include "LSingleton.h"

struct SGateUserInfo
{
	Lint nGateID = 0;
	Lint nUserID = 0;
	Llong nGateUserID = 0;
	Lint nOnUseTeamId = 0;
};

class CGateUserManager : public LSingleton<CGateUserManager>
{
	typedef std::map<Llong, SGateUserInfo> tGateUserInfoMap;
	typedef std::map<Lint, Llong> tUserIdUniqueMap;
public:
	CGateUserManager();
	virtual ~CGateUserManager();

	virtual	bool Init();
	virtual	bool Final();

	void	AddUser(Llong nUserUnique, Lint nGateID, Lint nUserID, Llong nGateUserID, Lint nOnUseTeamId);
	void	RmvUser(Llong nUserUnique);
	Lint	GetUserID(Llong nUserUnique);
	Lint	GetUserGateID(Llong nUserUnique);
	Llong	GetGateUserID(Llong nUserUnique);
	Lint	GetOnUseTeamIdByUserID(Llong nUserUnique);

	Llong	GetUnique(Lint nUserID);
	const SGateUserInfo& FindGateUserInfo(Llong nUserUnique);
private:
	tGateUserInfoMap	m_tGateUserInfoMap;
	tUserIdUniqueMap	m_tUserIdUniqueMap;
};

#define gGateUserManager CGateUserManager::Instance()

#endif // __GATE_USER_MGR_H__
