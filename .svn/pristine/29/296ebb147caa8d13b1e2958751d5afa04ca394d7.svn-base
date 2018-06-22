#ifndef __CardReward_
#define __CardReward_

#include "IRewardStrategy.h"
#include "LBase.h"
#include "LCharge.h"

class CardReward : public IRewardStrategy
{
public:
	CardReward(Lint rewardedUserId, Lint cardNum, RewardReason reason);

public:
	virtual bool	IssueReward(void);
	virtual void	SetRewardedUser(Lint userId);

private:
	CARDS_OPER_TYPE	GetCardOperType(void);

private:
	Lint			m_rewardedUserId;
	Ldouble			m_cardNum;
	RewardReason	m_reason;
};

#endif	// __CardReward_