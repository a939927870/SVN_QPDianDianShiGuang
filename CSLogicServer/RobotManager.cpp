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
			robot->SetRobotIndex(nIndex++);
			robot->SetUserGateId(65535);			//机器人GATE编号
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
	//以下加延迟添加机器人
	for (int i = 0; i < nPlayerCount; i++)
	{
		Lint index = getFree();
		LLOG_DEBUG("==AddRobotToDesk indx %d", index);
		if (index >= 0)
		{
			User* pUser = gUserManager.GetUserbyDataId(m_RobotList[index]->GetUserDataId());
			if (!pUser)
			{
				gUserManager.AddUser(m_RobotList[index]);
				pUser = gUserManager.GetUserbyDataId(m_RobotList[index]->GetUserDataId());
			}

			if (pUser)
			{
				pUser->initPlayGroundData(0, 50, 0, 0, 0);
				pUser->HanderUserAddDesk(nDeskIndex);
			}
			else
			{
				LLOG_ERROR("AddRobotToDesk can not get user nDeskIndex %d, nPlayerCount %d", nDeskIndex, nPlayerCount);
				return false;
			}
		}
		else
		{
			LLOG_ERROR("AddRobotToDesk22 can not get user nDeskIndex %d, nPlayerCount %d index %d", nDeskIndex, nPlayerCount, index);
			return false;
		}
	}
	return true;
}

Lint RobotManager::getFree()
{
	LLOG_ERROR("m_RobotList size = %d", m_RobotList.size());
	for (int i = 0; i < m_RobotList.size(); i ++)
	{
		if (m_RobotList[i]->m_desk == NULL)
		{
			return i;
		}
	}

	static Lint nIndex = 0;

	//如果没有就添加新的机器人;拷贝一个机器人
	size_t sz = m_RobotList.size();
	if (sz == 0)
	{
		LLOG_ERROR("getFree() conf null sz %d", sz);
		return -1;
	}
	Lint randid = L_Rand(0, sz - 1);
	Robot* tmpRobot = m_RobotList[randid];
	if (!tmpRobot)
	{
		LLOG_ERROR("getFree() null  %d == %d", randid, sz);
		return -1;
	}
	LUser* tmpUser = &tmpRobot->m_userData;
	LUser* user = new LUser();

	user->m_id = tmpUser->m_id + 1000 + sz;
	user->m_openid = tmpUser->m_openid;
	ostringstream  strnike;
	strnike << user->m_nike;
	strnike << sz + 1;
	user->m_nike = strnike.str();
	user->m_sex = tmpUser->m_sex;
	user->m_provice = tmpUser->m_provice;
	user->m_city = tmpUser->m_city;
	user->m_country = tmpUser->m_country;
	user->m_headImageUrl = tmpUser->m_headImageUrl;
	user->m_unioid = tmpUser->m_unioid;
	user->m_numOfCard1s = tmpUser->m_numOfCard1s;
	user->m_numOfCard2s = tmpUser->m_numOfCard2s;
	user->m_numOfCard3s = tmpUser->m_numOfCard3s;
	user->m_regTime = tmpUser->m_regTime;
	user->m_lastLoginTime = tmpUser->m_lastLoginTime;
	user->m_new = tmpUser->m_new;
	user->m_gm = tmpUser->m_gm;
	user->m_totalbuynum = tmpUser->m_totalbuynum;
	user->m_totalplaynum = tmpUser->m_totalplaynum;

	Robot* robot = new Robot(*user, 0, 0);
	robot->m_gateUserId = nIndex;
	robot->SetRobotIndex(nIndex++);
	robot->SetUserGateId(65535);			//机器人GATE编号

	m_RobotList.push_back(robot);

	LLOG_ERROR("==getFree addrobot size %d  name ", sz, user->m_nike.c_str());
	return sz;
}

