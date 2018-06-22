#include "ActiveManager.h"
#include "LTabFile.h"
#include "LTime.h"
#include "Work.h"

bool ActiveManager::Init()
{
	LoadActiveConfg();
	//LoadActiveData();

	m_isLoaded = false;
	m_isWait = false;

	return true;
}

bool ActiveManager::Final()
{
	return true;
}

void ActiveManager::LoadActiveConfg()
{
	char* filename = "settings/ActiveConfig.txt";
	Lint nIndex = 0;
	LTabFile tabFile;
	std::vector< std::string > lineItem;
	if (tabFile.Load(filename))
	{
		lineItem  = tabFile.getTapFileLine();
		while (lineItem.size() >= 46 )   //这里的数字限制了读入活动配置文件的配置长度;
		{
			ActiveInfo* active = new ActiveInfo();
			active->mActiveID = atoi(lineItem[0].c_str());
			active->mActiveName = lineItem[1];
			active->mActiveIntroduction = lineItem[2];
			active->mTimesPerday = atoi(lineItem[3].c_str());   //新增，每日最大抽奖次数;
			active->mAlltimes = atoi(lineItem[4].c_str());   //新增，总的最大抽奖次数;
			active->mBeginTime = lineItem[5];
			active->mEndTime = lineItem[6];
			active->mRewardNum = atoi(lineItem[7].c_str());
			active->mAllPercent = 0;
			for (Lint i = 0; i < MAX_GIFT_NUM && (i * 5 + 12) < lineItem.size(); i ++)
			{
				if (!lineItem[i * 5 + 8].empty() 
					&& !lineItem[i * 5 + 9].empty() 
					&& !lineItem[i * 5 + 10].empty()
					&& !lineItem[i * 5 + 11].empty()
					&& !lineItem[i * 5 + 12].empty())
				{
					active->mGift[i].giftname = lineItem[i * 5 + 8];
					active->mGift[i].gifttype = atoi(lineItem[i * 5 + 9].c_str());
					active->mGift[i].giftcount = atoi(lineItem[i * 5 + 10].c_str());
					active->mGift[i].giftpercent = atoi(lineItem[i * 5 + 11].c_str());
					active->mGift[i].gifttotalcount = atoi(lineItem[i * 5 + 12].c_str());
					active->mGift[i].giftsurpluscount = active->mGift[i].gifttotalcount;		//重载配置默认是总数量
					active->mAllPercent += active->mGift[i].giftpercent;
				}
			}
			mActiveList[active->mActiveID] = active;
			lineItem.clear();
			lineItem  = tabFile.getTapFileLine();
			LLOG_ERROR("ActiveConfig id:%d , mTimesPerday:%d!", active->mActiveID, active->mTimesPerday);
		}		

	}
	else
	{
		LLOG_ERROR("Load file of ActiveConfig error!");
	}

}

bool ActiveManager::Run(Lint ActiveId, ActiveRun_RetGift& Ret,bool IsAutoDecOne)
{
	auto ActiveItr = mActiveList.find(ActiveId);
	if (ActiveItr != mActiveList.end())
	{
		Lint Temp = 0;
		Lint Rand = L_Rand(0,ActiveItr->second->mAllPercent);
		for (int iii=0; iii<MAX_GIFT_NUM ; iii++)
		{
			ActiveGift* Gift = &ActiveItr->second->mGift[iii];
			Temp += Gift->giftpercent;
			if (Gift->giftpercent>0 && Temp>=Rand)
			{
				//抽到奖了
				if (Gift->giftsurpluscount>0)
				{
					Ret.giftID	= iii + 1;
					Ret.Gift_type	= Gift->gifttype;	
					Ret.Gift_count	= Gift->giftcount;	
					if (IsAutoDecOne)
						Gift->giftsurpluscount --;
					return true;
				}
				else
				{
					break;
				}
			}
		}

		//防止出错 如果没找到或者奖励发完了，找到一个一张电话卡的ID，发过去 ，奖励3类型3号奖品
		for (int iii=0; iii<MAX_GIFT_NUM ; iii++)
		{
			ActiveGift* Gift = &ActiveItr->second->mGift[iii];
			if (Gift->gifttype == GT_ROOMCARD && Gift->giftcount == 1)
			{
				Ret.giftID = iii + 1;					//一张房卡
				Ret.Gift_type = Gift->gifttype;			//房卡
				Ret.Gift_count =  Gift->giftcount;		//一张
				return true;
			}
		}
	}
	return false;
}

void ActiveManager::ReloadConfig()
{
	LoadActiveConfg();
	//LoadActiveData();
}

void ActiveManager::LoadActiveData()
{
	
}

void ActiveManager::SaveActiveData()
{

}

bool ActiveManager::CheckTime(Lstring&  mBeginTime, Lstring&  mEndTime)
{
	Lint Begin = atoi(mBeginTime.c_str());
	Lint End   = atoi(mEndTime.c_str());
	Lint today = gWork.GetCurTime().GetDate();
	return (today >= Begin && today <= End);
}

Lint ActiveManager::GetLastActiveID()
{
	std::map<Lint, ActiveInfo*>::iterator itor = mActiveList.end();
	if (itor == mActiveList.begin())
	{
		return -1;		//无活动
	}
	--itor;
	return itor->first;
}

bool ActiveManager::IsActiveOpoen(Lint mDate)
{
	std::map<Lint, ActiveInfo*>::iterator itor = mActiveList.end();
	if (itor == mActiveList.begin())
	{
		return false;		//无活动
	}
	--itor;
	Lint begin = atoi(itor->second->mBeginTime.c_str());
	Lint end = atoi(itor->second->mEndTime.c_str());
	if (begin > 0 && end > 0)
	{
		if (mDate >= begin && mDate <= end)
		{
			return true;
		}
	}
	return false;
}

Lint ActiveManager::getTimesPerday()
{
	std::map<Lint, ActiveInfo*>::iterator itor = mActiveList.end();
	if (itor == mActiveList.begin())
	{
		return -1;		//无活动
	}
	--itor;
	if (itor->second)
	{
		return itor->second->mTimesPerday <= 5 ? itor->second->mTimesPerday : 5;  //强制限制每天抽奖最多五次，防止配置文件出错;
	}
	return -1;
}

Lint ActiveManager::getAlltimes()
{
	std::map<Lint, ActiveInfo*>::iterator itor = mActiveList.end();
	if (itor == mActiveList.begin())
	{
		return -1;		//无活动
	}
	--itor;
	if (itor->second)
	{
		return itor->second->mAlltimes;
	}
	return -1;
}

bool ActiveManager::IsNeedPhoneNum(Lint activeID, Lint giftID)
{
	auto ActiveItr = mActiveList.find(activeID);
	if (ActiveItr!=mActiveList.end())
	{
		if (giftID >= 0 && giftID < MAX_GIFT_NUM)
		{
			return (ActiveItr->second->mGift[giftID].gifttype == GT_ITEM) || (ActiveItr->second->mGift[giftID].gifttype == GT_PHONECARD);
		}
	}
	return false;
}

Lint ActiveManager::GetWinSize(ActiveType type)
{
	if(type != EAWin) return EWinMaxSize;

	auto& active = mAllWinList;
	return active.size();
}

std::vector<LActiveSignCollectSS> ActiveManager::GetActive(ActiveType type)
{
	if(!(type == EAWin)) return std::vector<LActiveSignCollectSS>();

	return mAllWinList;
}

void ActiveManager::LoadAllWinData(ActiveType type, std::vector<LActiveSignCollectSS> &actives)
{
	assert(type == EAWin);
	if(!(type == EAWin)) return;
	auto itor = actives.begin();
	for(; itor != actives.end(); ++itor)
	{
		mAllWinList.push_back(*itor);
	}

	m_isLoaded = true;
}

bool ActiveManager::AddWin(LActiveSignCollectSS& info)
{
	assert(mAllWinList.size() < EWinMaxSize);

	mAllWinList.push_back(info);
	
	return true;
}

void ActiveManager::SetLoaded(ActiveManager::ActiveType type, bool isLoaded)
{
	if(!(type == EAWin)) return ;

	m_isLoaded = isLoaded;
}

bool ActiveManager::GetLoaded(ActiveType type)
{
	assert(type == EAWin);

	if(!(type == EAWin)) return false;
	
	return m_isLoaded;
}

void ActiveManager::SetWait(bool isWait)
{
	m_isWait = isWait;
}

bool ActiveManager::GetWait()
{
	return m_isWait;
}

void  ActiveManager::CheckGiftSafe(Lint activeID, ActiveRun_RetGift& drawInfo)
{
	if (1088 == activeID)
	{
		static Lint nsCardNums = 0;
		static Lint oldDate = 0;
		Lint newDate = gWork.GetCurTime().GetDate();
		if (newDate > oldDate)
		{
			LLOG_ERROR("The date of %d has offered Cards is %d", oldDate, nsCardNums);
			oldDate = newDate;
			nsCardNums = 0;
		}
		if (GT_ROOMCARD == drawInfo.Gift_type && nsCardNums >= 30000)  //每日房卡上线3W;
		{
			drawInfo.giftID = 3;
			drawInfo.Gift_type = GT_GOLD;
			drawInfo.Gift_count = 1000;
			return;
		}
		if (GT_ROOMCARD == drawInfo.Gift_type)
		{
			nsCardNums += drawInfo.Gift_count;
		}
	}
}
