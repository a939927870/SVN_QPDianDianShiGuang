#ifndef _ACTIVE_MANAGER_H
#define _ACTIVE_MANAGER_H

#include "LSingleton.h"
#define MAX_GIFT_NUM	(12)

enum	GIFTTYPE
{
	GT_NONE = 0,
	GT_ITEM = 1,
	GT_PHONECARD = 2,//话费
	GT_ROOMCARD = 3,//房卡
};
struct ActiveGift
{
	Lstring giftname;
	Lint gifttype;
	Lint giftcount;
	Lint giftpercent;
	Lint gifttotalcount;
	Lint giftsurpluscount;
	ActiveGift()
	{
		gifttype = 0;
		giftcount = 0;
		giftpercent = 0;
		gifttotalcount = 0;
		giftsurpluscount = 0;
	}
};
struct ActiveInfo
{
	Lint	mActiveID;
	Lstring	mActiveName;
	Lstring	mActiveIntroduction;
	Lstring	mBeginTime;					// 时间格式为: 年月日  如 20160506;
	Lstring	mEndTime;
	Lint	mRewardNum;
	ActiveGift	mGift[MAX_GIFT_NUM];
	Lint	mAllPercent;
};

// 抽奖返回;
struct ActiveRun_RetGift
{
	Lint	giftID;
	Lint	Gift_type;	
	Lint	Gift_count;
	ActiveRun_RetGift()
	{
		giftID = 0;
		Gift_type = 0;
		Gift_count = 0;
	}
};
class ActiveManager:public LSingleton<ActiveManager>
{
	
public:
	virtual	bool		Init();
	virtual	bool		Final();
	void				ReloadConfig();
	Lint				GetLastActiveID();
	bool				IsActiveOpoen(Lint mDate);
	bool				IsNeedPhoneNum(Lint activeID, Lint giftID);
	// 玩家抽奖;
	// 参数：活动ID，抽到的奖品信息，是否剩余数量自动减一;
	// 返回：是否抽到;
	bool				Run(Lint ActiveId, ActiveRun_RetGift& Ret, bool IsAutoDecOne=true);
	bool				CheckTime(Lstring&  mBeginTime, Lstring&  mEndTime);
private:
	void LoadActiveConfg();	// 装入活动配置;
	void LoadActiveData();	// 装入活动数据;		未实现
	void SaveActiveData();	// 回写活动数据;		未实现


private:
	std::map<Lint, ActiveInfo*>	mActiveList;
};

#define gActiveManager ActiveManager::Instance()

#endif

