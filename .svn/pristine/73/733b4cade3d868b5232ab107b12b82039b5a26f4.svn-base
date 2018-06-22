#ifndef _GATE_USER_H_
#define _GATE_USER_H_

#include "LSingleton.h"
#include "LSocket.h"

struct GateUser
{
	Lint			m_id;
	LSocketPtr		m_sp;
	Lint			m_login;
	Lint			m_userState;				//玩家当前状态
	Lint			m_logicID;					//玩家在哪个logic上面
	GateUser()
	{
		m_id = 0;
		m_login = 0;
		m_userState = 0;
		m_logicID = 0;
	}
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserLogicID(){return m_logicID;}
	void	setUserLogicID(Lint nValue){m_logicID = nValue;}
};

//////////////////////////////////////////////////////////////////////////
class GateUserManager :public LSingleton<GateUserManager>
{
public:

	virtual	bool	Init();
	virtual bool	Final();


	GateUser* GetUserById(Lint id);
	GateUser* GetUserBySp(LSocketPtr sp);
	void	AddUser(GateUser* user);
	void	DelUser(GateUser* user);

	GateUser*	CreateUser(LSocketPtr sp);
private:
	std::map<Lint, GateUser*>	m_gateUserIdMap;
	std::map<LSocketPtr, GateUser*>	m_gateUserSpMap;
	Lint	m_genid;
};

#define gGateUserManager GateUserManager::Instance()

#endif