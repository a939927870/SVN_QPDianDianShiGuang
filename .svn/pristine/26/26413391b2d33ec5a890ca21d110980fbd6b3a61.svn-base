#include "RobotManager.h"
#include "UserManager.h"
#include "RoomVip.h"
#include "LMsg.h"
#include "User.h"
#include <boost/property_tree/xml_parser.hpp>  
#include <boost/regex.hpp>
using namespace boost::property_tree;

RobotManager::RobotManager(void)
{
}

RobotManager::~RobotManager(void)
{
}

bool RobotManager::Init(const char* filename)
{
	//if (filename == NULL)
	//{
	//	return false;
	//}
	//Lint nIndex = 0;
	//LTabFile tabFile;
	//std::vector< std::string > lineItem;
	//if (tabFile.Load(filename))
	//{
	//	lineItem  = tabFile.getTapFileLine();
	//	while (lineItem.size() >= 16 )
	//	{
	//		LUser* user = new LUser();
	//		user->m_id = atoi(lineItem[0].c_str());
	//		user->m_openid = lineItem[1];
	//		user->m_nike = lineItem[2];
	//		user->m_sex =  atoi(lineItem[3].c_str());
	//		user->m_provice = lineItem[4];
	//		user->m_city = lineItem[5];
	//		user->m_country = lineItem[6];
	//		user->m_headImageUrl = lineItem[7];
	//		user->m_unioid =  (lineItem[8].c_str());
	//		user->m_numOfCard1s =  atoi(lineItem[9].c_str());
	//		user->m_numOfCard2s =  atoi(lineItem[10].c_str());
	//		user->m_numOfCard3s =  atoi(lineItem[11].c_str());
	//		user->m_regTime =  atoi(lineItem[12].c_str());
	//		user->m_lastLoginTime =  atoi(lineItem[13].c_str());
	//		user->m_new =  atoi(lineItem[14].c_str());
	//		user->m_gm =  atoi(lineItem[15].c_str());
	//		user->m_totalbuynum =  0;
	//		user->m_totalplaynum =  0;
	//		Robot* robot = new Robot(*user, 0, 0);
	//		robot->m_gateUserId = nIndex;
	//		robot->SetRobotIndex(nIndex ++);
	//		robot->SetUserGateId(65535);			//机器人GATE编号
	//		m_RobotList.push_back(robot);
	//		lineItem.clear();
	//		lineItem  = tabFile.getTapFileLine();
	//	}	
	//	LLOG_DEBUG("Add Robot!");
	//}
	//LLOG_DEBUG("Can't Add Robot!");
	//return true;

	try
	{
		Lint nIndex = 0;

		ptree pt;
		read_xml(filename, pt);
		auto root = pt.get_child("RobotManager");
		for (auto i = root.begin(); i != root.end(); ++i)
		{
			LUser* user = new LUser();
			user->m_id = i->second.get<Lint>("<xmlattr>.ID", 10000001);
			user->m_openid = i->second.get<Lint>("<xmlattr>.ID", 10000001);
			user->m_nike = i->second.get<Lstring>("<xmlattr>.Name", "123456");
			user->m_sex = i->second.get<Lint>("<xmlattr>.Sex", 0);
			user->m_provice = i->second.get<Lstring>("<xmlattr>.Provice", "");
			user->m_city = i->second.get<Lstring>("<xmlattr>.City", "");
			user->m_country = i->second.get<Lstring>("<xmlattr>.Country", "");
			user->m_headImageUrl = i->second.get<Lstring>("<xmlattr>.HtmlUrl", "");
			user->m_unioid = i->second.get<Lint>("<xmlattr>.UnionID", 0);
			user->m_numOfCard1s = i->second.get<Lint>("<xmlattr>.NumsCard1", 0);
			user->m_numOfCard2s = i->second.get<Lint>("<xmlattr>.NumsCard2", 0);
			user->m_numOfCard3s = i->second.get<Lint>("<xmlattr>.NumsCard3", 0);
			user->m_regTime = i->second.get<Lint>("<xmlattr>.RegTime", 0);
			user->m_lastLoginTime = i->second.get<Lint>("<xmlattr>.LastLoginTime", 0);
			user->m_new = i->second.get<Lint>("<xmlattr>.New", 0);
			user->m_gm = i->second.get<Lint>("<xmlattr>.GM", 0);
			user->m_totalbuynum = 0;
			user->m_totalplaynum = 0;
			Robot* robot = new Robot(*user, 0, 0);
			robot->m_gateUserId = nIndex;
			robot->SetRobotIndex(nIndex++);
			robot->SetUserGateId(65535);			//机器人GATE编号
			m_RobotList.push_back(robot);
		}
		return true;
	}
	catch (...)
	{
		LLOG_ERROR("RobotManager Error!");
		return false;
	}
}

bool RobotManager::Init()
{
	return true;
}

bool RobotManager::Final()
{
	return true;
}

bool RobotManager::AddRobotToDesk(Lint nDeskIndex, Lint nPlayerCount, Lint appId)
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
			pUser->m_userData.m_nAppId = appId;
			pUser->HanderUserAddDesk(nDeskIndex);
		}
	}
	return true;
}

Lint RobotManager::getFree()
{
	for (int i = 0; i < m_RobotList.size(); i ++)
	{
		if (m_RobotList[i]->m_desk == NULL)
		{
			return m_RobotList[i]->GetRobotIndex();
		}
	}
	return -1;
}

