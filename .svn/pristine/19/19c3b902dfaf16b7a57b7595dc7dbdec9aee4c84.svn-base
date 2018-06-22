#include "UserManager.h"

bool UserManager::Init()
{
	return true;
}

bool UserManager::Final()
{
	return true;
}

void UserManager::Tick()
{
	std::map<Llong, User*>::iterator it = m_userMap.begin();
	m_UserMapState = 1;
	for(; it != m_userMap.end(); ++it)
	{
		if (it->second)
		{
			it->second->Tick();
		}
		if (2 == m_UserMapState)
		{
			break;
		}
	}
	m_UserMapState = 1;
}

User* UserManager::GetUserByGateIdGateUserId(Lint gateId, Lint gateUserId)
{
	Llong id = ((Llong)gateId << 32 | (Llong)gateUserId);

	if (m_userMap.count(id))
	{
		return m_userMap[id];
	}

	return NULL;
}

User* UserManager::GetUserbyDataId(Lint dataId)
{
	if (m_userDataIdMap.count(dataId))
	{
		return m_userDataIdMap[dataId];
	}

	return NULL;
}

void UserManager::AddUser(User* user)
{
	//if (!m_userMap.count(user->GetUserUnique()))
	//{
	//	m_userMap[user->GetUserUnique()] = user;
	//}
	//if (!m_userDataIdMap.count(user->GetUserDataId()))
	//{
	//	m_userDataIdMap[user->GetUserDataId()] = user;
	//}
	m_userMap[user->GetUserUnique()] = user;
	m_userDataIdMap[user->GetUserDataId()] = user;
}

void UserManager::DelUser(User* user)
{
	if (m_userMap.count(user->GetUserUnique()))
	{
		if (1 == m_UserMapState)
		{
			m_UserMapState = 2;
		}
		m_userMap.erase(m_userMap.find(user->GetUserUnique()));
	}

	if (m_userDataIdMap.count(user->GetUserDataId()))
	{
		m_userDataIdMap.erase(m_userDataIdMap.find(user->GetUserDataId()));
	}
}

void UserManager::AddUserIdName(UserIdInfo& u)
{
	m_userIdInfo[u.m_id] = u;
}

UserIdInfo* UserManager::GetUserIdInfo(Lint id)
{
	if (m_userIdInfo.count(id))
	{
		return &m_userIdInfo[id];
	}

	return NULL;
}

Lint UserManager::GetGatUserCount(Lint gateId)
{
	Lint i = 0;	
	std::map<Llong, User*>::iterator it = m_userMap.begin();
	for(; it != m_userMap.end(); ++it)
	{
		if(it->second->GetUserGateId() == gateId && it->second->GetOnline())
			i++;
	}

	return i;
}

void	UserManager::BoadCast(LMsg& msg)
{
	std::map<Llong, User*>::iterator it = m_userMap.begin();
	for(; it != m_userMap.end(); ++it)
	{
		if(it->second->GetOnline())
			it->second->Send(msg);
	}

}