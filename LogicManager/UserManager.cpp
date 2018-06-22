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

void    UserManager::AddUser2Wait(User* user)
{
	m_waitGoldGameUser.push_back(user->m_userData.m_id);
}

void    UserManager::delUserFromWait(User* user)
{
	std::vector<int>::iterator itr = m_waitGoldGameUser.begin();
	while (itr!= m_waitGoldGameUser.end())
	{
		if ( user->m_userData.m_id == *itr )
		{
			itr = m_waitGoldGameUser.erase(itr);
			break;
		}
		else
			++itr;
	}
}

void UserManager::UpdatePlayerTaskList()
{
	LLOG_DEBUG("Start to update player's task list.");
	for (auto iter = m_userDataIdMap.begin(); iter != m_userDataIdMap.end(); iter++)
	{
		User* user = iter->second;
		if (user != NULL)
			user->GetTaskManager()->UpdateTasks();
	}
}

void UserManager::SendOnlineUser2Team(LSocketPtr sp)
{
	for (auto it = m_userDataIdMap.begin(); it != m_userDataIdMap.end(); ++it)
	{
		User* user = it->second;
		if (user && user->GetOnline() /*&& user->GetUserLogicID() <= 0*/)
		{

			LMsgLMG2TS_UserLogin noticeUserLogin;
			noticeUserLogin.nUserUnique = user->GetUserUnique();
			noticeUserLogin.nGateID = user->GetUserGateId();
			noticeUserLogin.nUserID = user->GetUserDataId();
			noticeUserLogin.nGateUserID = user->GetUserGateUserId();
			sp->Send(noticeUserLogin.GetSendBuff());
		}
	}
}