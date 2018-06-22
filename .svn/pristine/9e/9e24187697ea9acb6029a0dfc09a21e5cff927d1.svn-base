#include "GateUserManager.h"
#include "LLog.h"


CGateUserManager::CGateUserManager()
{
}


CGateUserManager::~CGateUserManager()
{
}

bool CGateUserManager::Init()
{
	return true;
}

bool CGateUserManager::Final()
{
	return true;
}

void CGateUserManager::AddUser(Llong nUserUnique, Lint nGateID, Lint nUserID, Llong nGateUserID, Lint nOnUseTeamId)
{
	//LLOG_DEBUG("AddUser:  nUserUnique:%lld,nUserID:%d", nUserUnique, nUserID);
	/*auto iter_unique = m_tUserIdUniqueMap.find(nUserID);
	if (iter_unique != m_tUserIdUniqueMap.end())
	{
		auto iter_Gate = m_tGateUserInfoMap.find(iter_unique->second);
		if (iter_Gate != m_tGateUserInfoMap.end())
			m_tGateUserInfoMap.erase(iter_Gate);
		m_tUserIdUniqueMap.erase(iter_unique);
	}*/
	SGateUserInfo gateUserInfo;
	gateUserInfo.nGateID = nGateID;
	gateUserInfo.nUserID = nUserID;
	gateUserInfo.nGateUserID = nGateUserID;
	gateUserInfo.nOnUseTeamId = nOnUseTeamId;

	m_tGateUserInfoMap[nUserUnique] = gateUserInfo;
	m_tUserIdUniqueMap[nUserID] = nUserUnique;
}

void CGateUserManager::RmvUser(Llong nUserUnique)
{
	
	tGateUserInfoMap::iterator iter = m_tGateUserInfoMap.find(nUserUnique);
	if ( iter != m_tGateUserInfoMap.end() )
	{
		tUserIdUniqueMap::iterator iter_unique = m_tUserIdUniqueMap.find(iter->second.nUserID);
		if ( iter_unique != m_tUserIdUniqueMap.end() )
		{
			//LLOG_DEBUG("RmvUser:  nUserUnique:%lld,nUserID:%d", nUserUnique, iter_unique->first);
			m_tUserIdUniqueMap.erase(iter_unique);
		}
		
		m_tGateUserInfoMap.erase(iter);
	}
}

Lint CGateUserManager::GetUserID(Llong nUserUnique)
{
	tGateUserInfoMap::iterator iter = m_tGateUserInfoMap.find(nUserUnique);
	
	if (iter != m_tGateUserInfoMap.end())
	{
		//LLOG_DEBUG("CGateUserManager::GetUserID  nUserUnique:%ld ,nUserID:%d", nUserUnique, iter->second.nUserID);
		return iter->second.nUserID;
	}
	//LLOG_DEBUG(" CGateUserManager::GetUserID  nUserUnique:%ld ,nUserID:%d", nUserUnique,0);
	return 0;
}

Lint CGateUserManager::GetOnUseTeamIdByUserID(Llong nUserUnique)
{
	tGateUserInfoMap::iterator iter = m_tGateUserInfoMap.find(nUserUnique);
	if (iter != m_tGateUserInfoMap.end())
	{
		return iter->second.nOnUseTeamId;
	}
	return 0;
}

Lint CGateUserManager::GetUserGateID(Llong nUserUnique)
{
	tGateUserInfoMap::iterator iter = m_tGateUserInfoMap.find(nUserUnique);
	if (iter != m_tGateUserInfoMap.end())
	{
		return iter->second.nGateID;
	}
	return 0;
}

Llong CGateUserManager::GetGateUserID(Llong nUserUnique)
{
	tGateUserInfoMap::iterator iter = m_tGateUserInfoMap.find(nUserUnique);
	if (iter != m_tGateUserInfoMap.end())
	{
		return iter->second.nGateUserID;
	}
	return 0;
}

Llong CGateUserManager::GetUnique(Lint nUserID)
{
	Llong nUserUnique = 0;
	tUserIdUniqueMap::iterator iter = m_tUserIdUniqueMap.find(nUserID);
	if (iter != m_tUserIdUniqueMap.end())
	{
		nUserUnique = iter->second;
	}

	return nUserUnique;
}

const SGateUserInfo& CGateUserManager::FindGateUserInfo(Llong nUserUnique)
{
	tGateUserInfoMap::iterator iter = m_tGateUserInfoMap.find(nUserUnique);
	if (iter != m_tGateUserInfoMap.end())
	{
		return iter->second;
	}

	static SGateUserInfo g_gate_user_info;
	return g_gate_user_info;
}