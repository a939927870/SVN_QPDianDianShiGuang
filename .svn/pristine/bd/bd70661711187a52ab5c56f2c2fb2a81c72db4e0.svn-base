#include <sstream>
#include "FinancialService.h"
#include "Work.h"
#include "LLog.h"
#include "DUser.h"

using namespace std;

enum DIAMOND_CHANGED_REASON
{
	DIAMOND_CHANGED_REASON_USER_CHARGE = 0,
	DIAMOND_CHANGED_REASON_PURCHASE = 1,
	DIAMOND_CHANGED_REASON_GM_GRANT = 2,
	DIAMOND_CHANGED_REASON_GM_DEDUCT = 3,
};

FinancialService::FinancialService(void)
{
}

FinancialService::~FinancialService(void)
{
}

Lint FinancialService::ChargeDiamondFor(DUser* user, Lint quantity, bool userChargeSelf, const Lstring& orderNo)
{
	LLOG_ERROR("start to charge %d diamond for user %d by %s", quantity, user->m_usert.m_id, (userChargeSelf ? "himself" : "GM"));
	Lint errCode = 6;
	if(user->m_usert.m_diamondNum + quantity >= 0)
	{
		errCode = 8;
		if(UpdateDiamondQuantityToDB(user->m_usert.m_id, quantity))
		{
			user->m_usert.m_diamondNum += quantity;
			LLOG_ERROR("charging diamond succeed.");
			Lint reason = DIAMOND_CHANGED_REASON_USER_CHARGE;
			if(!userChargeSelf)
				reason = quantity > 0 ? DIAMOND_CHANGED_REASON_GM_GRANT : DIAMOND_CHANGED_REASON_GM_DEDUCT;
			InsertDiamondDetailLog(gWork.GetCurTime().Secs(), user->m_usert.m_id, quantity, reason, orderNo);
			errCode = 0;
		}
		else
			LLOG_ERROR("Failed to update diamond quantity to database.");
	}

	return errCode;
}

Lint FinancialService::PurchaseItem(DUser* user, Lint itemId, Lint diamondPaid, Lint cardAdded, Lint cardForVIP, Lint cardType, const Lstring& orderNo)
{
	LLOG_ERROR("user %d purchased item %d, %d diamond paid, %d card given, %d card for vip.",user->m_usert.m_id, itemId, diamondPaid, cardAdded, cardForVIP);
	Lint errCode = 6;
	if(user->m_usert.m_diamondNum >= diamondPaid)
	{
		errCode = 8;
		Lint totalCardNum = (cardAdded + cardForVIP);
		if(UpdateDataToDB(user->m_usert.m_id, diamondPaid, totalCardNum, cardType))
		{
			Lint time = gWork.GetCurTime().Secs();
			InsertDiamondDetailLog(time, user->m_usert.m_id, -diamondPaid, DIAMOND_CHANGED_REASON_PURCHASE, orderNo);
			Lint cardNumType4 = 0, cardNumType8 = 0, cardNumType16 = 0;
			Lint total = CalcCardNumByType(totalCardNum, cardType, cardNumType4, cardNumType8, cardNumType16);
			user->m_usert.m_diamondNum -= diamondPaid;
			user->m_usert.m_numOfCard1s += cardNumType4;
			user->m_usert.m_numOfCard2s += cardNumType8;
			user->m_usert.m_numOfCard3s += cardNumType16;
			user->m_usert.m_totalbuynum += total;
			InsertCardLog(time, user->m_usert.m_id, cardAdded, cardType, CARDS_OPER_TYPE_PURCHASE_GIVE, user->m_usert.m_numOfCard2s, user->m_usert.m_useTeamId);
			if(cardForVIP > 0)
				InsertCardLog(time, user->m_usert.m_id, cardForVIP, cardType, CARDS_OPER_TYPE_VIP_GIVE, user->m_usert.m_numOfCard2s, user->m_usert.m_useTeamId);
			LLOG_ERROR("purchasing item succeed.");
			errCode = 0;
		}
	}
	return errCode;
}

bool FinancialService::UpdateDiamondQuantityToDB(Lint userId, Lint quantityChanged)
{
	bool succeed = false;
	MYSQL* mysql = gWork.GetDbSession().GetMysql();
	if(mysql != NULL)
	{
		stringstream ss;
		ss << "UPDATE User SET DiamondNum = DiamondNum +  " << quantityChanged;
		ss << " WHERE Id='" << userId << "'";
		if(mysql_real_query(mysql, ss.str().c_str(), ss.str().size()) == 0)
			succeed = true;
		else
			LLOG_ERROR("Failed to update user's DiamondNum.");
	}
	else
		LLOG_ERROR("Can not connect to database.");

	return succeed;
}

bool FinancialService::UpdateDataToDB(Lint userId, Lint diamondPaid, Lint cardAdded, Lint cardType)
{
	bool succeed = false;
	MYSQL* mysql = gWork.GetDbSession().GetMysql();
	if(mysql != NULL)
	{
		Lint cardNumType4 = 0, cardNumType8 = 0, cardNumType16 = 0;
		Lint total = CalcCardNumByType(cardAdded, cardType, cardNumType4, cardNumType8, cardNumType16);

		stringstream ss;
		ss << "UPDATE User SET DiamondNum = DiamondNum -  " << diamondPaid << ", ";
		ss << "NumsCard1 = NumsCard1 + " << cardNumType4 << ", ";
		ss << "NumsCard2 = NumsCard2 + " << cardNumType8 << ", ";
		ss << "NumsCard3 = NumsCard3 + " << cardNumType16 << ", ";
		ss << "TotalCardNum = TotalCardNum + " << total << " ";
		ss << "WHERE Id='" << userId << "'";
		if(mysql_real_query(mysql, ss.str().c_str(), ss.str().size()) == 0)
			succeed = true;
		else
			LLOG_ERROR("Failed to update user's DiamondNum, NumsCard1, NumsCard2, NumsCard3.");
	}
	else
		LLOG_ERROR("Can not connect to database.");
	return succeed;
}

void FinancialService::InsertDiamondDetailLog(Lint time, Lint userId, Lint diamondNum, Lint reason, const Lstring& orderNo)
{
	std::stringstream ss;
	ss << "INSERT INTO DiamondDetails (Time,DiamondNum,Reason,UserId,OrderNo) VALUES (";
	ss << "'" << time << "',";
	ss << "'" << diamondNum << "',";
	ss << "'" << reason << "',";
	ss << "'" << userId << "',";
	ss << "'" << orderNo << "')";

	LLOG_DEBUG("Work::InsertDiamondDetailLog:%s", ss.str().c_str());

	gDbServer.Push(move(ss.str()), userId);
}

void FinancialService::InsertCardLog(Lint time, Lint userId, Lint cardAdded, Lint cardType, Lint operType, Lint nTotalCardNum, Lint nTeamId)
{
	LMsgL2CeSaveCardInfo msg;
	LCharge& charge = msg.m_charge;
	charge.m_admin = "system";
	charge.m_userId = userId;
	charge.m_cardType = cardType;
	charge.m_cardNum = cardAdded;
	charge.m_type = operType;
	charge.m_time = time;
	charge.m_iTotalCardNum = nTotalCardNum;
	charge.m_teamId = nTeamId;
	gWork.HanderLogicSaveCardInfo(&msg);
}

Lint FinancialService::CalcCardNumByType(Lint cardNum, Lint cardType, Lint& cardNumType4, Lint& cardNumType8, Lint& cardNumType16)
{
	cardNumType4 = 0;
	cardNumType8 = 0;
	cardNumType16 = 0;
	switch (cardType)
	{
	case CARD_TYPE_4:
		cardNumType4 = cardNum;
		break;
	case CARD_TYPE_8:
		cardNumType8 = cardNum;
		break;
	case CARD_TYPE_16:
		cardNumType16 = cardNum;
		break;
	}
	return cardNumType4 / 2 + cardNumType8 + 2 * cardNumType16;
}