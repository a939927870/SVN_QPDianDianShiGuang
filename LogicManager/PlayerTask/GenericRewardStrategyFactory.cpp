#include "GenericRewardStrategyFactory.h"
//#include "CouponReward.h"
//#include "LuckyDrawReward.h"
#include "CardReward.h"

boost::shared_ptr<IRewardStrategy> GenericRewardStrategyFactory::CreateRewardStrategy(PlayerTaskAttribute::RewardType rewardType, Lint rewardCount, Lint rewardedUser, IRewardStrategy::RewardReason reason)
{
	boost::shared_ptr<IRewardStrategy> strategy;
	switch (rewardType)
	{
	//case PlayerTaskAttribute::Coupon:
	//	strategy = boost::shared_ptr<IRewardStrategy>(new CouponReward(rewardedUser, rewardCount, reason));
	//	break;
	//case PlayerTaskAttribute::LuckyDraw:
	//	strategy = boost::shared_ptr<IRewardStrategy>(new LuckyDrawReward(rewardedUser, rewardCount, reason));
	//	break;
	case PlayerTaskAttribute::Card:
		strategy = boost::shared_ptr<IRewardStrategy>(new CardReward(rewardedUser, rewardCount, reason));
		break;
	default:
		break;
	}
	return strategy;
}