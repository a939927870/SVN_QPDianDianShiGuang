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
	for(; it != m_userMap.end(); ++it)
	{
		it->second->Tick();
	}
}

User* UserManager::GetUserByGateIdGateUserId(Lint gateId, Llong gateUserId)
{
	Llong id = ((Llong)gateId << 48 | gateUserId);

	auto iter = m_userMap.find(id);
	if (iter!=m_userMap.end())
	{
		return iter->second;
	}
	return NULL;
}

User* UserManager::GetUserbyDataId(Lint dataId)
{
	auto iter = m_userDataIdMap.find(dataId);
	if (iter!=m_userDataIdMap.end())
	{
		return iter->second;
	}
	return NULL;
}

void UserManager::AddUser(User* user)
{
	if (!m_userMap.count(user->GetUserUnique()))
	{
		m_userMap[user->GetUserUnique()] = user;
	}
	if (!m_userDataIdMap.count(user->GetUserDataId()))
	{
		m_userDataIdMap[user->GetUserDataId()] = user;
	}
}

void UserManager::DelUser(User* user)
{
	auto iter1 = m_userMap.find(user->GetUserUnique());
	if (iter1!=m_userMap.end())
	{
		m_userMap.erase(iter1);
	}

	auto iter2 = m_userDataIdMap.find(user->GetUserDataId());
	if (iter2!=m_userDataIdMap.end())
	{
		m_userDataIdMap.erase(iter2);
	}
}

void UserManager::AddUserIdName(UserIdInfo& u)
{
	m_userIdInfo[u.m_id] = u;
}

UserIdInfo* UserManager::GetUserIdInfo(Lint id)
{
	auto iter = m_userIdInfo.find(id);
	if (iter!=m_userIdInfo.end())
	{
		return &iter->second;
	}

	return NULL;
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