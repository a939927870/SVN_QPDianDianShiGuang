#include "DeskManager.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"

bool DeskManager::Init()
{
	//初始所有的桌子号;
	Lint		m_curDeskId;
	std::vector<Lint> m_deskVector;
	for (m_curDeskId = 100000; m_curDeskId < 999999; ++m_curDeskId)
	{
		m_deskVector.push_back(m_curDeskId);
	}

	//生成随机队列;
	Lint nSize = m_deskVector.size();
	while(nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		m_FreeDeskList.push(m_deskVector[seed1]);
		m_sharevideoId.push(m_deskVector[seed1]);
		m_deskVector[seed1] = m_deskVector[nSize - 1];
		nSize --;
	}
	return true;
}

bool DeskManager::Final()
{
	return true;
}

Lint DeskManager::GetFreeDeskId(Lint nServerID)
{
	Lint id = 0;
	Lint nDeskSize = m_FreeDeskList.size();
	if (nDeskSize > 0)
	{
		id = m_FreeDeskList.front();
		m_FreeDeskList.pop();
		m_DeskLogicID[id] = nServerID;
	}
	LLOG_DEBUG("DeskManager::GetFreeDeskId:%d size=%d", id, nDeskSize);
	return id;
}

Lint DeskManager::GetFreeGoldDeskId(Lint nServerID, Lint gameStyle)
{
	Lint goldDeskID = 0;
	if(m_vecWaitGoldDesk[gameStyle].size())
	{
		auto it = m_vecWaitGoldDesk[gameStyle].begin();
		if (it->m_leftPlace > 0)
		{
			goldDeskID = it->m_deskID;
			it->m_leftPlace--;
		}
		if (0 == it->m_leftPlace)
		{
			m_vecWaitGoldDesk[gameStyle].erase(it);
		}
	}
	if(0 == goldDeskID)  //不用else，防止前边意外分配失败;
	{
		goldDeskID = GetFreeDeskId(nServerID);
		WaitGoldDesk addWaitDesk;
		addWaitDesk.m_deskID = goldDeskID;
		addWaitDesk.m_leftPlace = getGameUserCounts(gameStyle)-1;  //第一次分配玩家即用去一个位置;
		m_vecWaitGoldDesk[gameStyle].push_back(addWaitDesk);
	}
	return goldDeskID;
}

void DeskManager::RecycleGoldDeskPlace(LMsgL2LMGRecyleGoldDeskPlace* msg)
{
	if (msg->m_gameType < game_style_sum)
	{
		bool isInWaitList = false;
		for (size_t i = 0; i < m_vecWaitGoldDesk[msg->m_gameType].size(); ++i)
		{
			if (msg->m_deskID == m_vecWaitGoldDesk[msg->m_gameType][i].m_deskID)
			{
				if (m_vecWaitGoldDesk[msg->m_gameType][i].m_leftPlace < getGameUserCounts(msg->m_gameType))
				{
					m_vecWaitGoldDesk[msg->m_gameType][i].m_leftPlace++;
				}
				else
				{
					LLOG_ERROR("error,too much user leave desk!");
					return;
				}
				isInWaitList = true;
				break;
			}
		}
		if (false == isInWaitList)  //已满的桌子有第一个人离场时候执行,重新加入等待列表;
		{
			WaitGoldDesk addWaitDesk;
			addWaitDesk.m_deskID = msg->m_deskID;
			addWaitDesk.m_leftPlace = 1;  //注意,满员桌子回收第一个位置;
			m_vecWaitGoldDesk[msg->m_gameType].push_back(addWaitDesk);
		}
	}
}

void DeskManager::UseGoldDeskPlace(LMsgL2LMGUseGoldDeskPlace* msg)
{
	if (msg->m_gameType < game_style_sum)
	{
		bool isInWaitList = false;
		auto it = m_vecWaitGoldDesk[msg->m_gameType].begin();
		for (auto it = m_vecWaitGoldDesk[msg->m_gameType].begin(); it != m_vecWaitGoldDesk[msg->m_gameType].end(); ++it)
		{
			if (msg->m_deskID == it->m_deskID)
			{
				it->m_leftPlace -= msg->m_usedCounts;
				if (it->m_leftPlace < 0)
				{
					it->m_leftPlace = 0;
					LLOG_ERROR("error,too much robot add in desk!");
				}
				if (0 == it->m_leftPlace)
				{
					m_vecWaitGoldDesk[msg->m_gameType].erase(it);
				}
				return;
			}
		}
	}
	return;
}

void DeskManager::RecycleDeskId(LMsgL2LMGRecyleDeskID* msg)
{
	if (!msg)
	{
		return;
	}

	LLOG_DEBUG("DeskManager::RecycleDeskId deskid :%d serverid %d", msg->m_deskID, msg->m_serverID);
	auto desk = m_DeskLogicID.find(msg->m_deskID);
	if (desk != m_DeskLogicID.end())
	{
		//移除桌子；
		m_DeskLogicID.erase(desk);

		//ID放回队列
		m_FreeDeskList.push(msg->m_deskID);

		//逻辑桌子数减一；
		auto logic = gWork.GetLogicInfoById(msg->m_serverID);
		if (logic != NULL)
		{
			logic->m_deskCount--;
		}
	}

	//不管有没有都直接发送到team服务器
	auto it_team = gWork.m_TeamServerDeskSet.find(msg->m_deskID);
	if (it_team != gWork.m_TeamServerDeskSet.end())
	{
		gWork.m_TeamServerDeskSet.erase(it_team);
	}
	else
	{
		LLOG_DEBUG("DeskManager:: m_TeamServerDeskSet notfind deskid :%d serverid %d", msg->m_deskID, msg->m_serverID);
	}

	// 通知 工会服务器 桌子ID 被回收 [2017-8-7 willing]
	LMsgLMG2TS_RecycleDeskId RecycleDeskIdMsg;
	RecycleDeskIdMsg.nDeskId = msg->m_deskID;
	gWork.SendMessageToTeamServer(RecycleDeskIdMsg);
}

Lint DeskManager::getServerIDbyDeskID(Lint nDeskID)
{
	auto desk = m_DeskLogicID.find(nDeskID);
	if (desk != m_DeskLogicID.end())
	{
		return desk->second;
	}
	return 0;
}

void DeskManager::RecycleDumpServer(Lint nLogicID)
{
	auto desk = m_DeskLogicID.begin();
	for (; desk != m_DeskLogicID.end();)
	{
		if (desk->second == nLogicID)
		{
			m_FreeDeskList.push(desk->first);
			desk = m_DeskLogicID.erase(desk);
		}
		else
		{
			++ desk;
		}
	}
}

Lint DeskManager::GetShareVideoId()
{
	if (m_sharevideoId.empty())
	{
		LLOG_ERROR("get ShareVideoID error1!");
		return -1;
	}
	Lint id = m_sharevideoId.front();
	m_sharevideoId.pop();
	m_sharevideoId.push(id);
	return id;
}


Lint DeskManager::GetFreeGoldDeskCounts()
{
	return m_FreeDeskList.size();
}

void DeskManager::HanderTSReqDeskId(std::vector<int>& deskVec)
{
	deskVec.clear();
	Lint id = 0;
	for (Lint i = 0; i < 100; i++)
	{
		Lint nDeskSize = m_FreeDeskList.size();
		LLOG_ERROR("DeskManager::HanderTSReqDeskId size=%d", nDeskSize);
		if (nDeskSize > 0)
		{
			id = m_FreeDeskList.front();
			m_FreeDeskList.pop();
			deskVec.push_back(id);
		}
		else
		{
			break;
		}
	}
}

void DeskManager::HanderFastDeskID(Lint deskid, Lint nServerID)
{
	if (deskid > 0 && nServerID > 0)
	{
		m_DeskLogicID[deskid] = nServerID;
		/*m_FastDeskSet.insert(deskid);*/
	}
}