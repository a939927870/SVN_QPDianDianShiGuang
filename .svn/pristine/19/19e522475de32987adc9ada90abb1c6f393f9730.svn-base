#ifndef _GATE_MANAGER_H_
#define _GATE_MANAGER_H_

#include "LBase.h"
#include "User.h"
#include "LSingleton.h"

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

	Lint	GetGatUserCount(Lint gateId);

	void	BoadCast(LMsg& msg);
private:
	std::map<Llong, User*>	m_userMap;
	std::map<Lint, User*>	m_userDataIdMap;

	std::map<Lint,UserIdInfo>  m_userIdInfo;
	Lint                    m_UserMapState;
};

#define gUserManager UserManager::Instance()

#endif