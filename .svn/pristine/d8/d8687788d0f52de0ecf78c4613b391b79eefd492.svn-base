#include "GateUser.h"
#include "LLog.h"

bool GateUserManager::Init()
{
	m_genid = 0;
	return true;
}

bool GateUserManager::Final()
{
	return true;
}


GateUser* GateUserManager::GetUserById(Lint id)
{
	if (m_gateUserIdMap.count(id))
	{
		return m_gateUserIdMap[id];
	}

	return  NULL;
}

GateUser* GateUserManager::GetUserBySp(LSocketPtr sp)
{
	if (m_gateUserSpMap.count(sp))
	{
		return m_gateUserSpMap[sp];
	}

	return NULL;
}

void GateUserManager::AddUser(GateUser* user)
{
	m_gateUserIdMap[user->m_id] = user;
	m_gateUserSpMap[user->m_sp] = user;
}

void GateUserManager::DelUser(GateUser* user)
{
	LLOG_DEBUG("deluser, gateid:%d  logicid:%d   ip:%s", user->m_id, user->m_logicID, user->m_sp->GetRemoteIp().c_str());
	if (m_gateUserIdMap.count(user->m_id))
	{
		m_gateUserIdMap.erase(m_gateUserIdMap.find(user->m_id));
	}

	if (m_gateUserSpMap.count(user->m_sp))
	{
		m_gateUserSpMap.erase(m_gateUserSpMap.find(user->m_sp));
	}

	delete user;
}

GateUser* GateUserManager::CreateUser(LSocketPtr sp)
{
	GateUser* user = new GateUser();
	user->m_id = ++m_genid;
	user->m_sp = sp;
	user->m_login = 0;

	AddUser(user);

	return user;
}