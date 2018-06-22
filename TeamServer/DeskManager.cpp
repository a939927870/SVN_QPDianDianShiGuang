#include "DeskManager.h"
#include "LLog.h"
#include "TeamManager.h"
#include "LMsgCT.h"
#include "Work.h"
#include "GateUserManager.h"


CDeskManager::CDeskManager()
{
}


CDeskManager::~CDeskManager()
{
}

bool CDeskManager::Init()
{
	m_tTeamDeskInfoListMap.clear();
	m_tDeskIDTeamIDMap.clear();
	m_tFastFreeDesk.clear();
	return true;
}

bool CDeskManager::Final()
{
	return true;
}

void CDeskManager::AddDeskInfo(const STeamDeskInfo& teamDeskInfo, bool is_fastDesk)
{
	if (is_fastDesk)
	{
		auto it_team = m_tFastUseDeskMap.find(teamDeskInfo.nTeamID);
		if (it_team != m_tFastUseDeskMap.end())
		{
			auto it_desk = std::find(it_team->second.begin(), it_team->second.end(), teamDeskInfo.nDeskID);
			it_desk->m_userCount++;
		}
		std::pair<Lint, STeamDeskInfo> tmp_instr(teamDeskInfo.nTeamID, teamDeskInfo);
		m_tFastDeskInfoMap.insert(tmp_instr);
	}
	else
	{
		tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(teamDeskInfo.nTeamID);
		if (iter == m_tTeamDeskInfoListMap.end())
		{
			tTeamDeskInfoList newDeskInfoList;
			newDeskInfoList.push_back(teamDeskInfo);
			m_tTeamDeskInfoListMap.insert(std::make_pair(teamDeskInfo.nTeamID, newDeskInfoList));
		}
		else
		{
			tTeamDeskInfoList& deskInfoList = iter->second;
			deskInfoList.push_back(teamDeskInfo);
		}

	}
	

	//// 同步给所有工会成员 [2017-8-2 willing]
	//LMsgS2CNewTeamDesk newTeamDesk;
	//newTeamDesk.nTeamID = teamDeskInfo.nTeamID;
	//newTeamDesk.nDeskID = teamDeskInfo.nDeskID;
	//for ( int i=0; i<DEFAULT_DESK_USER_COUNT; i++ )
	//{
	//	newTeamDesk.nUserID.push_back(teamDeskInfo.nUserID[i]);
	//	newTeamDesk.strHeadImageUrl.push_back(teamDeskInfo.strHeadImageUrl[i]);
	//}

	//newTeamDesk.nMajongType = teamDeskInfo.nMajongType;
	//newTeamDesk.nMaxCircle = teamDeskInfo.nMaxCircle;
	//newTeamDesk.nMaxUserCount = teamDeskInfo.nMaxUserCount;
	///*newTeamDesk.nUpDownJi = teamDeskInfo.nUpDownJi;
	//newTeamDesk.nAllJi = teamDeskInfo.nAllJi;
	//newTeamDesk.nLianZhuang = teamDeskInfo.nLianZhuang;*/
	//newTeamDesk.nPlayTypeCount = teamDeskInfo.tPlayType.size();
	//for ( int i=0; i<newTeamDesk.nPlayTypeCount; i++ )
	//{
	//	newTeamDesk.tPlayType.push_back(teamDeskInfo.tPlayType[i]);
	//}
	//newTeamDesk.nCreateTime = teamDeskInfo.nCreateTime;

	// 添加 桌子和工会的对应关系 [2017-8-7 willing]
	m_tDeskIDTeamIDMap[teamDeskInfo.nDeskID] = teamDeskInfo.nTeamID;

	//gTeamManager.BroadcastTeamDeskMsg(teamDeskInfo.nTeamID, newTeamDesk);
}

void CDeskManager::DeskUserChg(Lint nTeamID, Lint nDesk, Lint nUserID, Lint nIsJoin)
{
	tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
	auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
	if ( iter == m_tTeamDeskInfoListMap.end() && it.first == it.second )
	{
		return;
	}
	const STeamMember& teamMember = gTeamManager.FindMember(nTeamID, nUserID);
	if (teamMember.nTeamID == 0)
	{
		return;
	}

	//bool bNeedBrocast = false;
	if (it.first != it.second)
	{
		//bool	can_erase = false;
		for (; it.first != it.second; it.first++)
		{
			STeamDeskInfo& deskInfo = it.first->second;
			if (deskInfo.nTeamID != nTeamID || deskInfo.nDeskID != nDesk)
				continue;
			// 加入桌子 
			if (1 == nIsJoin)
			{
				bool is_inDesk = false;
				for (Lint pos = 0; pos < DEFAULT_DESK_USER_COUNT; pos++)
				{
					if (deskInfo.nUserID[pos] == nUserID)
					{
						is_inDesk = true;
						break;
					}
				}
				if (!is_inDesk)
				{
					for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
					{
						if (deskInfo.nUserID[i] == 0 && deskInfo.nDeskID == nDesk)
						{
							deskInfo.nUserID[i] = nUserID;
							deskInfo.strHeadImageUrl[i] = teamMember.strHeadImageUrl;
							deskInfo.nCurUserCount++;
							deskInfo.m_nike[i] = teamMember.strUserName;
							auto it_team = m_tFastUseDeskMap.find(nTeamID);
							if (it_team != m_tFastUseDeskMap.end())
							{
								auto it_desk = std::find(it_team->second.begin(), it_team->second.end(), nDesk);
								if (it_desk != it_team->second.end())
								{
									it_desk->m_userCount++;
									if (it_desk->m_userCount == DEFAULT_DESK_USER_COUNT)
									{
										if (it_desk != it_team->second.end())
										{
											LLOG_DEBUG("changer del deskid:%d", it_desk->m_deskid);
											it_team->second.erase(it_desk);
										}

										if (it_team->second.empty())
											m_tFastUseDeskMap.erase(it_team);
										//can_erase = true;
									}
								}

							}
							//bNeedBrocast = true;

							break;
						}
					}
					/*if (can_erase)
					{
						m_tFastDeskInfoMap.erase(it.first);
						
					}*/
					break;
				}

			}
			// 离开桌子
			else
			{
				auto it = m_tFastUseDeskMap.find(nTeamID);
				if (it != m_tFastUseDeskMap.end())
				{
					auto it_desk = std::find(it->second.begin(), it->second.end(), nDesk);
					it_desk->m_userCount--;
				}
				for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
				{
					if (deskInfo.nUserID[i] == nUserID)
					{
						deskInfo.nUserID[i] = 0;
						deskInfo.strHeadImageUrl[i].clear();
						deskInfo.m_nike[i].clear();
						deskInfo.nCurUserCount--;
						//bNeedBrocast = true;
						break;
					}
				}
			}
		}

	}
	
	if(iter != m_tTeamDeskInfoListMap.end())
	{
		tTeamDeskInfoList& deskInfoList = iter->second;
		tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
		tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();
		for (; iter_desk != iend_desk; iter_desk++)
		{
			STeamDeskInfo& deskInfo = *iter_desk;
			if (deskInfo.nDeskID != nDesk)
			{
				continue;
			}

			// 加入桌子 [2017-8-4 willing]
			if (1 == nIsJoin)
			{
				bool is_inDesk = false;
				for (Lint pos = 0; pos < DEFAULT_DESK_USER_COUNT; pos++)
				{
					if (deskInfo.nUserID[pos] == nUserID)
					{
						is_inDesk = true;
						break;
					}
				}
				if (!is_inDesk)
				{
					for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
					{
						//if (deskInfo.nUserID[i] == nUserID)
						//{
						//	//bNeedBrocast = true;
						//	break;
						//}

						if (deskInfo.nUserID[i] == 0 && deskInfo.nDeskID == nDesk)
						{
							deskInfo.nUserID[i] = nUserID;
							deskInfo.strHeadImageUrl[i] = teamMember.strHeadImageUrl;
							deskInfo.nCurUserCount++;
							deskInfo.m_nike[i] = teamMember.strUserName;
							//bNeedBrocast = true;
							break;
						}
					}
				}
			}
			// 离开桌子 [2017-8-4 willing]
			else
			{
				for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
				{
					if (deskInfo.nUserID[i] == nUserID)
					{
						deskInfo.nUserID[i] = 0;
						deskInfo.strHeadImageUrl[i].clear();
						deskInfo.m_nike[i].clear();
						deskInfo.nCurUserCount--;
						//bNeedBrocast = true;
						break;
					}
				}
			}
		}
	}	

	/*LMsgS2CTeamDeskUserChg deskUserChg;
	deskUserChg.nTeamID = nTeamID;
	deskUserChg.nDeskID = nDesk;
	deskUserChg.nUserID = nUserID;
	deskUserChg.nIsJoin = nIsJoin;
	deskUserChg.strHeadImageUrl = teamMember.strHeadImageUrl;

	gTeamManager.BroadcastTeamDeskMsg(nTeamID, deskUserChg);*/
}

void CDeskManager::DelDesk(Lint nTeamID, Lint nDesk)
{
	//快速组局 待分配的桌子
	auto it_team = m_tFastUseDeskMap.find(nTeamID);
	if (it_team != m_tFastUseDeskMap.end())
	{
		auto it_desk = std::find(it_team->second.begin(), it_team->second.end(), nDesk);
		if (it_desk != it_team->second.end())
		{
			it_team->second.erase(it_desk);
			if (it_team->second.empty())
			{
				m_tFastUseDeskMap.erase(it_team);
			}
		}
	}


	tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
	auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
	if (iter == m_tTeamDeskInfoListMap.end() && it.first == it.second)
	{
		return;
	}
	Lint userid = 0;
	if (it.first != it.second)
	{
		for (; it.first != it.second; it.first++)
		{
			STeamDeskInfo& deskInfo = it.first->second;
			if (deskInfo.nDeskID == nDesk)
			{
				m_tFastDeskInfoMap.erase(it.first);
				break;
			}
		}
	}
	if(iter != m_tTeamDeskInfoListMap.end())
	{
		tTeamDeskInfoList& deskInfoList = iter->second;
		tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
		tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();
		for (; iter_desk != iend_desk;)
		{
			STeamDeskInfo& deskInfo = *iter_desk;
			if (deskInfo.nDeskID == nDesk)
			{
				/*if (deskInfo.nCurUserCount < DEFAULT_DESK_USER_COUNT)
				{
					gTeamManager.HanderRestDeskwithPerLimitUse(deskInfo.nTeamID, deskInfo.nUserID[0], deskInfo.nMaxCircle);
				}*/
				iter_desk = deskInfoList.erase(iter_desk);
			}
			else
			{
				iter_desk++;
			}
		}
		if (iter->second.empty())
			m_tTeamDeskInfoListMap.erase(iter);
	}
	/*LMsgS2CDelTeamDesk delTeamDesk;
	delTeamDesk.nTeamID = nTeamID;
	delTeamDesk.nDeskID = nDesk;
	
	gTeamManager.BroadcastTeamDeskMsg(nTeamID, delTeamDesk);*/

	// 添加 桌子和工会的对应关系 [2017-8-7 willing]
	tDeskIDTeamIDMap::iterator iter_deskidmap = m_tDeskIDTeamIDMap.find(nDesk);
	if (iter_deskidmap != m_tDeskIDTeamIDMap.end() )
	{
		m_tDeskIDTeamIDMap.erase(iter_deskidmap);
	}
	
}

bool CDeskManager::CheckDeskInfoLimit(Lint nTeamID, Lint nDesk, Ldouble card_num, Lint& limit_num)
{
	tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
	auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
	if (iter == m_tTeamDeskInfoListMap.end() && it.first == it.second)
	{
		LLOG_ERROR("CheckDeskInfoLimit not find teamid %d", nTeamID);
		return false;
	}
	Lint userid = 0;
	if (it.first != it.second)
	{
		for (; it.first != it.second; it.first++)
		{
			STeamDeskInfo& deskInfo = it.first->second;
			if (deskInfo.nDeskID == nDesk)
			{
				if (deskInfo.nCircleGoldLimit > card_num)
				{
					LLOG_ERROR("CheckDeskInfoLimit1 not nCircleGoldLimit %d card_num %f", deskInfo.nCircleGoldLimit, card_num);
					return false;
				}

				limit_num = deskInfo.nCircleGoldLimit;
				return true;
			}
		}
	}

	if (iter != m_tTeamDeskInfoListMap.end())
	{
		tTeamDeskInfoList& deskInfoList = iter->second;
		tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
		tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();
		for (; iter_desk != iend_desk;)
		{
			STeamDeskInfo& deskInfo = *iter_desk;
			if (deskInfo.nDeskID == nDesk)
			{
				if (deskInfo.nCircleGoldLimit > card_num)
				{
					LLOG_ERROR("CheckDeskInfoLimit not nCircleGoldLimit %d card_num %f", deskInfo.nCircleGoldLimit, card_num);
					return false;
				}

				limit_num = deskInfo.nCircleGoldLimit;
				return true;
			}
			else
			{
				iter_desk++;
			}
		}
	}

	LLOG_ERROR("CheckDeskInfoLimit not find nDesk %d", nDesk);
	return false;
}

void CDeskManager::SendAllDesk(Lint nTeamID, Llong nUserUnique)
{
	tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
	auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
	if (iter == m_tTeamDeskInfoListMap.end() && it.first == it.second)
	{
		LMsgS2CAllTeamDesk allTeamDeskMsg;
		allTeamDeskMsg.nTeamID = nTeamID;
		allTeamDeskMsg.tAllTeamDesk.clear();
		gWork.SendToGate(allTeamDeskMsg, nUserUnique);
		//gTeamManager.SetCareForDesk(nTeamID, gGateUserManager.GetUserID(nUserUnique), 1);
		return;
	}

	LMsgS2CAllTeamDesk allTeamDeskMsg;
	allTeamDeskMsg.nTeamID = nTeamID;
	//if (it.first != it.second)
	//{
	//	for (;it.first != it.second;it.first++)
	//	{
	//		/*if(it.first->second.nCurUserCount == DEFAULT_DESK_USER_COUNT)
	//			continue;*/
	//		STeamDeskInfoMsg deskInfoMsg;
	//		STeamDeskInfo& deskInfo = it.first->second;
	//		deskInfoMsg.nTeamID = deskInfo.nTeamID;
	//		deskInfoMsg.nDeskID = deskInfo.nDeskID;
	//		for (int i = 0; i<DEFAULT_DESK_USER_COUNT; i++)
	//		{
	//			if (deskInfo.nUserID[i])
	//			{
	//				deskInfoMsg.nUserID.push_back(deskInfo.nUserID[i]);
	//				deskInfoMsg.strHeadImageUrl.push_back(deskInfo.strHeadImageUrl[i]);
	//				deskInfoMsg.m_nike.push_back(deskInfo.m_nike[i]);
	//			}
	//		}
	//		deskInfoMsg.nMajongType = deskInfo.nMajongType;
	//		deskInfoMsg.nMaxCircle = deskInfo.nMaxCircle;
	//		deskInfoMsg.nMaxUserCount = deskInfo.nMaxUserCount;
	//		/*deskInfoMsg.nPlayTypeCount = deskInfo.tPlayType.size();
	//		for (int i = 0; i<deskInfoMsg.nPlayTypeCount; i++)
	//		{
	//		deskInfoMsg.tPlayType.push_back(deskInfo.tPlayType[i]);
	//		}*/
	//		deskInfoMsg.tPlayType = deskInfo.tPlayType;
	//		deskInfoMsg.nCreateTime = deskInfo.nCreateTime;
	//		deskInfoMsg.m_fastDesk = 1;
	//		deskInfoMsg.nCircleGoldLimit = deskInfo.nCircleGoldLimit;
	//		allTeamDeskMsg.tAllTeamDesk.push_back(deskInfoMsg);
	//	}
	//	
	//}
	if (iter != m_tTeamDeskInfoListMap.end())
	{
		tTeamDeskInfoList& deskInfoList = iter->second;
		tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
		tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();
		for (; iter_desk != iend_desk; iter_desk++)
		{
			if(iter_desk->nCurUserCount == iter_desk->nMaxUserCount)
				continue;
			STeamDeskInfoMsg deskInfoMsg;
			STeamDeskInfo& deskInfo = *iter_desk;
			deskInfoMsg.nTeamID = deskInfo.nTeamID;
			deskInfoMsg.nDeskID = deskInfo.nDeskID;
			for (int i = 0; i<DEFAULT_DESK_USER_COUNT; i++)
			{
				if (deskInfo.nUserID[i])
				{
					deskInfoMsg.nUserID.push_back(deskInfo.nUserID[i]);
					deskInfoMsg.strHeadImageUrl.push_back(deskInfo.strHeadImageUrl[i]);
					deskInfoMsg.m_nike.push_back(deskInfo.m_nike[i]);
				}
			}
			deskInfoMsg.nMajongType = deskInfo.nMajongType;
			deskInfoMsg.nMaxCircle = deskInfo.nMaxCircle;
			deskInfoMsg.nMaxUserCount = deskInfo.nMaxUserCount;
			/*deskInfoMsg.nPlayTypeCount = deskInfo.tPlayType.size();
			for ( int i=0; i<deskInfoMsg.nPlayTypeCount; i++ )
			{
			deskInfoMsg.tPlayType.push_back(deskInfo.tPlayType[i]);
			}*/
			deskInfoMsg.tPlayType = deskInfo.tPlayType;
			deskInfoMsg.nCreateTime = deskInfo.nCreateTime;
			deskInfoMsg.nCircleGoldLimit = deskInfo.nCircleGoldLimit;
			deskInfoMsg.nStake = deskInfo.nStake;
			deskInfoMsg.nMaxFanshu = deskInfo.nMaxFanshu;
			allTeamDeskMsg.tAllTeamDesk.push_back(deskInfoMsg);
		}
	}
	

	gWork.SendToGate(allTeamDeskMsg, nUserUnique);
	//gTeamManager.SetCareForDesk(nTeamID,gGateUserManager.GetUserID(nUserUnique),1);
}

Lint CDeskManager::GetDeskCount(Lint nTeamID)
{
	tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
	auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
	if (iter == m_tTeamDeskInfoListMap.end() && it.first == it.second)
	{
		return 0;
	}

	Lint deskNum = 0;
	if (it.first != it.second)
	{
		for (; it.first != it.second; it.first++)
		{
			++deskNum;
		}
	}
	if (iter != m_tTeamDeskInfoListMap.end())
	{
		tTeamDeskInfoList& deskInfoList = iter->second;
		tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
		tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();
		for (; iter_desk != iend_desk; iter_desk++)
		{
			++deskNum;
		}
	}

	return deskNum;
}

bool CDeskManager::IsInDesk(Lint nUserID)
{
	Lint tmp_teamid;
	return IsInDesk(nUserID, tmp_teamid);
}


bool CDeskManager::IsInDesk(Lint nUserID, Lint& nTeamId)
{
	if (0 == nUserID)
	{
		return false;
	}

	auto iter_team_id = gTeamManager.m_tUserJoinTeamMap.find(nUserID);

	if (iter_team_id == gTeamManager.m_tUserJoinTeamMap.end())
	{
		LLOG_ERROR("CDeskManager::IsInDesk not fount join team is list");
		return false;
	}
	bool	result = false;
	tIntList& joinTeamIdList = iter_team_id->second;
	tIntList::iterator iter_id = joinTeamIdList.begin();
	tIntList::iterator iend_id = joinTeamIdList.end();
	for (; iter_id != iend_id; iter_id++)
	{
		int nTeamID = *iter_id;

		auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
		tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
		if (iter == m_tTeamDeskInfoListMap.end() && it.first == it.second)
		{
			continue;
		}

		if (iter != m_tTeamDeskInfoListMap.end())
		{
			tTeamDeskInfoList& deskInfoList = iter->second;
			if (!deskInfoList.empty())
			{
				tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
				tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();

				for (; iter_desk != iend_desk; iter_desk++)
				{
					STeamDeskInfo& deskInfo = *iter_desk;
					for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
					{
						if (deskInfo.nUserID[i] == nUserID)
						{
							nTeamId = nTeamID;
							return true;
						}
					}
				}
			}
		}

		if (it.first != it.second)
		{
			for (; it.first != it.second; it.first++)
			{
				STeamDeskInfo& deskInfo = it.first->second;
				for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
				{
					if (deskInfo.nUserID[i] == nUserID)
					{
						nTeamId = nTeamID;
						return true;
					}
				}
			}

		}
	}

	return result;
}

bool  CDeskManager::GetDeskInfoByUserId(Lint nUserID, Lint& nTeamId, Lint& nDesk)
{
	if (0 == nUserID)
	{
		LLOG_ERROR("CDeskManager::GetDeskIdByUserId userid %d", nUserID);
		return false;
	}

	auto iter_team_id = gTeamManager.m_tUserJoinTeamMap.find(nUserID);

	if (iter_team_id == gTeamManager.m_tUserJoinTeamMap.end())
	{
		LLOG_ERROR("CDeskManager::GetDeskIdByUserId not fount join team is list");
		return false;
	}
	bool	result = false;
	tIntList& joinTeamIdList = iter_team_id->second;
	tIntList::iterator iter_id = joinTeamIdList.begin();
	tIntList::iterator iend_id = joinTeamIdList.end();
	for (; iter_id != iend_id; iter_id++)
	{
		int nTeamID = *iter_id;

		auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
		tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
		if (iter == m_tTeamDeskInfoListMap.end() && it.first == it.second)
		{
			continue;
		}

		if (iter != m_tTeamDeskInfoListMap.end())
		{
			tTeamDeskInfoList& deskInfoList = iter->second;
			if (!deskInfoList.empty())
			{
				tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
				tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();

				for (; iter_desk != iend_desk; iter_desk++)
				{
					STeamDeskInfo& deskInfo = *iter_desk;
					for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
					{
						if (deskInfo.nUserID[i] == nUserID)
						{
							nDesk = deskInfo.nDeskID;
							nTeamId = deskInfo.nTeamID;
							return true;
						}
					}
				}
			}
		}

		if (it.first != it.second)
		{
			for (; it.first != it.second; it.first++)
			{
				STeamDeskInfo& deskInfo = it.first->second;
				for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
				{
					if (deskInfo.nUserID[i] == nUserID)
					{
						nDesk = deskInfo.nDeskID;
						nTeamId = deskInfo.nTeamID;
						return true;
					}
				}
			}

		}
	}

	return false;
}

Lint  CDeskManager::CheckDeskFull(Lint nTeamID, Lint deskId)
{
	tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.find(nTeamID);
	auto it = m_tFastDeskInfoMap.equal_range(nTeamID);
	if (iter == m_tTeamDeskInfoListMap.end() && it.first == it.second)
	{
		//房间不存在 2:房间不存在 1：房间已满
		return 2;
	}
	if (it.first != it.second)
	{
		for (; it.first != it.second;it.first++)
		{
			STeamDeskInfo& deskInfo = it.first->second;
			if (deskInfo.nTeamID != nTeamID || deskInfo.nDeskID != deskId)
				continue;
			if (deskInfo.nCurUserCount == DEFAULT_DESK_USER_COUNT)
				return 1;
			else
				return 0;
		}
	}
	if (iter != m_tTeamDeskInfoListMap.end())
	{
		tTeamDeskInfoList& deskInfoList = iter->second;
		tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
		tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();
		for (; iter_desk != iend_desk; iter_desk++)
		{
			STeamDeskInfo& deskInfo = *iter_desk;
			if (deskInfo.nDeskID != deskId)
				continue;

			if (deskInfo.nCurUserCount == DEFAULT_DESK_USER_COUNT)
				return 1;
			else
				return 0;
		}
	}
	return 1;
}
Lint CDeskManager::GetTeamIDByDeskID(Lint nDeskId)
{
	tDeskIDTeamIDMap::iterator iter = m_tDeskIDTeamIDMap.find(nDeskId);
	if ( iter == m_tDeskIDTeamIDMap.end())
	{
		return 0;
	}

	return iter->second;
}

// 清理超过8小时的桌子 [2017-8-9 willing]
void CDeskManager::ClearTimeOutDesk()
{
	tTeamDeskInfoListMap::iterator iter = m_tTeamDeskInfoListMap.begin();
	tTeamDeskInfoListMap::iterator iend = m_tTeamDeskInfoListMap.end();
	Llong nCurTimestamp = LTime().Secs();
	for ( ; iter != iend; iter++ )
	{
		tTeamDeskInfoList& deskInfoList = iter->second;
		tTeamDeskInfoList::iterator iter_desk = deskInfoList.begin();
		tTeamDeskInfoList::iterator iend_desk = deskInfoList.end();
		for (; iter_desk != iend_desk;)
		{
			STeamDeskInfo& deskInfo = *iter_desk;
			if ( (nCurTimestamp - deskInfo.nCreateTime) >= (8*3600)) 
			{
				iter_desk = deskInfoList.erase(iter_desk);
			}
			else
			{
				iter_desk++;
			}
		}
	}
	
}


void CDeskManager::PushFastDesk(std::vector<Lint>& addVec)
{
	if(!addVec.empty())
		m_tFastFreeDesk.insert(m_tFastFreeDesk.end(), addVec.begin(), addVec.end());
}
Lint CDeskManager::PopFastDesk()
{
	if (m_tFastFreeDesk.empty())
		return 0;
	Lint	deskId = m_tFastFreeDesk.back();
	m_tFastFreeDesk.pop_back();
	if (m_tFastFreeDesk.size() < 100)
	{
		LMsgTS2LMG_ReqDeskId send;
		gWork.SendToLogicManager(send);
	}
	return deskId;
}
Lint	CDeskManager::GetFreeDeskId(bool& isCreate, Lint teamId, std::vector<Lint>& deskIds, std::vector<Lint>& limitGolds)
{
	isCreate = false;
	Lint deskid = 0;
	auto it = m_tFastUseDeskMap.find(teamId);
	if (it == m_tFastUseDeskMap.end())
	{
		//deskid = PopFastDesk();
		//if (deskid != 0)
		//{
		//	isCreate = true;
		//	STeamFastDesk desk;
		//	desk.m_deskid = deskid;
		//	//desk.m_userCount++;
		//	m_tFastUseDeskMap[teamId].push_back(desk);
		//	m_tDeskIDTeamIDMap[deskid] = teamId;
		//}
	}
	else
	{		
		auto it_desk = m_tFastUseDeskMap[teamId].rbegin();
		deskid = it_desk->m_deskid;
		//it_desk->m_userCount++;
	}

	auto it2 = m_tTeamDeskInfoListMap.find(teamId);
	if (it2 != m_tTeamDeskInfoListMap.end() && m_tTeamDeskInfoListMap[teamId].size())
	{
		auto it_desk = m_tTeamDeskInfoListMap[teamId].begin();
		while (it_desk != m_tTeamDeskInfoListMap[teamId].end())
		{
			if (it_desk->nCurUserCount != it_desk->nMaxUserCount)
			{
				deskid = it_desk->nDeskID;
				deskIds.push_back(it_desk->nDeskID);
				limitGolds.push_back(it_desk->nCircleGoldLimit);
			}
			++it_desk;
		}
	}
	return deskid;
}