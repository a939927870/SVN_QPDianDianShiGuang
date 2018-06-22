#ifndef __PlayerTaskAttributeCollection_
#define __PlayerTaskAttributeCollection_

#include <vector>
#include "LBase.h"

using namespace std;

typedef struct
{
	enum TaskType
	{
		TaskNone = 0,
		PlayingGame = 1,
		UserShared,
		InvitingFriend = 1000,
		CompletingGame = 1001,
	};

	enum TaskDuration
	{
		Invalid = 0,
		FixedDuration = 0x0100,
		Day = 0x0101,
		Week = 0x0102,

		NotFixedDuration = 0x0200,
		Unlimited = 0x0201,	// 没有固定期间，直到任务完成
	};

	enum RewardType
	{
		RewardNone = 0,
		Coupon = 1,			// 礼券
		LuckyDraw = 2,		// 抽奖机会
		Card = 3,			// 房卡
	};

	Lint			id;
	TaskType		type;
	Lstring			desc;
	TaskDuration	duration;
	Lint			total;
	RewardType		reward;
	Lint			rewardCount;
	bool			repeated;
	bool			recvManually;
	bool			valid;
	Lint			startDate;
	Lint			endDate;
	Lstring			extParams;

	bool			IsFixedDuration(void) const { return (duration & FixedDuration) != 0; }
	bool			IsValid(Lint curDate) const {return (valid && (curDate >= startDate && curDate <= endDate));}
} PlayerTaskAttribute;

class PlayerTaskAttributeCollection
{
private:
	PlayerTaskAttributeCollection(void);
	PlayerTaskAttributeCollection(const PlayerTaskAttributeCollection&);
	const PlayerTaskAttributeCollection& operator = (const PlayerTaskAttributeCollection&);

public:
	static PlayerTaskAttributeCollection*		Instance(void);

public:
	bool								CheckUpdates(void);
	Lsize								GetTaskNum(void) const;
	const PlayerTaskAttribute&			GetTask(Lsize index) const;

private:
	void								LoadTaskAttributes(void);
	PlayerTaskAttribute::TaskType		ParseType(const Lstring & str);
	PlayerTaskAttribute::TaskDuration	ParseDuration(const Lstring & str);
	PlayerTaskAttribute::RewardType		ParseRewardType(const Lstring & str);
	bool								Validate(const PlayerTaskAttribute & attribute);

private:
	vector<PlayerTaskAttribute>		m_tasks;
	std::time_t						m_lastModifiedTime;
};

#endif	// __PlayerTaskAttributeCollection_