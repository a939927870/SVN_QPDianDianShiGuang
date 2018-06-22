#include "ActiveManager.h"
#include "LTabFile.h"
#include "LTime.h"
#include "Work.h"

bool ActiveManager::Init()
{
	LoadActiveConfg();
	//LoadActiveData();

	return true;
}

bool ActiveManager::Final()
{
	return true;
}

void ActiveManager::LoadActiveConfg()
{
	char* filename = "settings\\ActiveConfig.txt";
	Lint nIndex = 0;
	LTabFile tabFile;
	std::vector< std::string > lineItem;
	if (tabFile.Load(filename))
	{
		lineItem  = tabFile.getTapFileLine();
		while (lineItem.size() >= 65 )
		{
			ActiveInfo* active = new ActiveInfo();
			active->mActiveID = atoi(lineItem[0].c_str());
			active->mActiveName = lineItem[1];
			active->mActiveIntroduction = lineItem[2];
			active->mBeginTime = lineItem[3];
			active->mEndTime = lineItem[4];
			active->mRewardNum = atoi(lineItem[5].c_str());
			active->mAllPercent = 0;
			for (Lint i = 0; i < MAX_GIFT_NUM; i ++)
			{
				if (!lineItem[i * 4 + 5].empty() && !lineItem[i * 4 + 6].empty() && !lineItem[i * 4 + 7].empty())
				{
					active->mGift[i].giftname = lineItem[i * 5 + 6];
					active->mGift[i].gifttype = atoi(lineItem[i * 5 + 7].c_str());
					active->mGift[i].giftcount = atoi(lineItem[i * 5 + 8].c_str());
					active->mGift[i].giftpercent = atoi(lineItem[i * 5 + 9].c_str());
					active->mGift[i].gifttotalcount = atoi(lineItem[i * 5 + 10].c_str());
					active->mGift[i].giftsurpluscount = active->mGift[i].gifttotalcount;		//重载配置默认是总数量
					active->mAllPercent += active->mGift[i].giftpercent;
				}
			}
			mActiveList[active->mActiveID] = active;
			lineItem.clear();
			lineItem  = tabFile.getTapFileLine();
		}		
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
			if (Gift->gifttype == GT_ROOMCARD && Gift->giftcount == 50)
			{
				Ret.giftID = iii + 1;					//50张房卡
				Ret.Gift_type = Gift->gifttype;			//房卡
				Ret.Gift_count =  Gift->giftcount;		//50张
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
