#ifndef __IRewardStrategy_
#define __IRewardStrategy_

#include "LBase.h"

class IRewardStrategy
{
public:
	virtual ~IRewardStrategy(){}

public:
	virtual bool	IssueReward(void) = 0;
	virtual void	SetRewardedUser(Lint userId) = 0;

	enum RewardReason
	{
		None = 0,
		PlayingGame = 1,
		UserShared = 2,
		InvitingFriend = 3,
		BindingInviter = 4,
		RRSportReward = 5,
	};
};

#endif	// __IRewardStrategy_