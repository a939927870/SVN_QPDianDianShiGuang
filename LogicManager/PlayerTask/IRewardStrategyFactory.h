#ifndef __IRewardStrategyFactory_
#define __IRewardStrategyFactory_

#include <boost/shared_ptr.hpp>
#include "PlayerTaskAttributeCollection.h"
#include "LBase.h"
#include "IRewardStrategy.h"

class IRewardStrategyFactory
{
public:
	virtual ~IRewardStrategyFactory(){}

public:
	virtual boost::shared_ptr<IRewardStrategy>		CreateRewardStrategy(PlayerTaskAttribute::RewardType rewardType, Lint rewardCount, Lint rewardedUser, IRewardStrategy::RewardReason reason) = 0;
};

#endif	// __IRewardStrategyFactory_