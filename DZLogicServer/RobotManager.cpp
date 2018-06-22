#include "RobotManager.h"
#include "UserManager.h"
#include "RoomVip.h"
#include "LMsg.h"
#include "User.h"
RobotManager::RobotManager(void)
{
}


RobotManager::~RobotManager(void)
{
}

bool RobotManager::Init(char* filename)
{
	if (filename == NULL)
	{
		return false;
	}
	Lint nIndex = 0;
	LTabFile tabFile;
	std::vector< std::string > lineItem;
	if (tabFile.Load(filename))
	{
		lineItem  = tabFile.getTapFileLine();
		while (lineItem.size() >= 16 )
		{
			LUser* user = new LUser();
			user->m_id = atoi(lineItem[0].c_str());
			user->m_openid = lineItem[1];
			user->m_nike = lineItem[2];
			user->m_sex =  atoi(lineItem[3].c_str());
			user->m_provice = lineItem[4];
			user->m_city = lineItem[5];
			user->m_country = lineItem[6];
			user->m_headImageUrl = lineItem[7];
			user->m_unioid =  (lineItem[8].c_str());
			user->m_numOfCard1s =  atoi(lineItem[9].c_str());
			user->m_numOfCard2s =  atoi(lineItem[10].c_str());
			user->m_numOfCard3s =  atoi(lineItem[11].c_str());
			user->m_regTime =  atoi(lineItem[12].c_str());
			user->m_lastLoginTime =  atoi(lineItem[13].c_str());
			user->m_new =  atoi(lineItem[14].c_str());
			user->m_gm =  atoi(lineItem[15].c_str());
			user->m_totalbuynum =  0;
			user->m_totalplaynum =  0;
			Robot* robot = new Robot(*user, 0, 0);
			robot->m_gateUserId = nIndex;
			robot->SetRobotIndex(nIndex ++);
			robot->SetUserGateId(65535);			//»úÆ÷ÈËGATE±àºÅ
			m_RobotList.push_back(robot);
			lineItem.clear();
			lineItem  = tabFile.getTapFileLine();
		}		
	}
	return true;
}

bool RobotManager::Init()
{
	return true;
}

bool RobotManager::Final()
{
	return true;
}

bool RobotManager::AddRobotToDesk(Lint nDeskIndex, Lint nPlayerCount)
{
	for (int i = 0; i < nPlayerCount; i ++)
	{
		Lint index = getFree();
		if (index >= 0)
		{
			User* pUser = gUserManager.GetUserbyDataId(m_RobotList[index]->GetUserDataId());
			if (!pUser)
			{
				gUserManager.AddUser(m_RobotList[index]);
				pUser = gUserManager.GetUserbyDataId(m_RobotList[index]->GetUserDataId());
			}
			pUser->HanderUserAddDesk(nDeskIndex);
		}
		else
		{
			return false;
		}
	}
	return true;
}

Lint RobotManager::getFree()
{
	for (size_t i = 0; i < m_RobotList.size(); i ++)
	{
		if (m_RobotList[i]->m_desk == NULL)
		{
			return m_RobotList[i]->GetRobotIndex();
		}
	}
	return -1;
}

