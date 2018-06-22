#ifndef __GenericRewardStrategyFactory_
#define __GenericRewardStrategyFactory_

#include "IRewardStrategyFactory.h"

class GenericRewardStrategyFactory : public IRewardStrategyFactory
{
public:
	virtual boost::shared_ptr<IRewardStrategy>		CreateRewardStrategy(PlayerTaskAttribute::RewardType rewardType, Lint rewardCount, Lint rewardedUser, IRewardStrategy::RewardReason reason);
};

#endif	// __GenericRewardStrategyFactory_
