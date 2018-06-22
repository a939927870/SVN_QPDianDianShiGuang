#include "CardReward.h"
#include "../Work.h"
#include "../UserManager.h"

CardReward::CardReward(Lint rewardedUserId, Lint cardNum, RewardReason reason)
	: m_rewardedUserId(rewardedUserId), m_cardNum(cardNum), m_reason(reason)
{
}

bool CardReward::IssueReward(void)
{
	bool succeed = false;
	User* user = gUserManager.GetUserbyDataId(m_rewardedUserId);
	if (user != NULL)
	{
		user->AddCardCount(CARD_TYPE_8, m_cardNum, GetCardOperType(), "system");
		succeed = true;
	}
	else if (UserIdInfo* idInfo = gUserManager.GetUserIdInfo(m_rewardedUserId))
	{
		LLOG_DEBUG("CardReward::IssueReward, rewarded user %d is offline.", m_rewardedUserId);
		LMsgLMG2CeCardExchange msg;
		msg.m_unionId = idInfo->m_unionId;
		msg.m_cardNum = m_cardNum;
		msg.m_cardType = CARD_TYPE_8;
		msg.m_oper = GetCardOperType();
		msg.m_exchangeType = 1;
		gWork.SendToCenter(msg);
	}
	else
		LLOG_ERROR("Rewarded user %d does not exist.", m_rewardedUserId);
	return succeed;
}

void CardReward::SetRewardedUser(Lint userId)
{
	m_rewardedUserId = userId;
}

CARDS_OPER_TYPE CardReward::GetCardOperType(void)
{
	CARDS_OPER_TYPE type = CARDS_OPER_TYPE_ACTIVE;
	if (m_reason == IRewardStrategy::InvitingFriend)
		type = CARDS_OPER_TYPE_INVITING;
	else if (m_reason == IRewardStrategy::PlayingGame)
		type = CARDS_OPER_TYPE_PLAYING;

	return type;
}