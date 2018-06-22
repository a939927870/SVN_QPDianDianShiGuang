#ifndef __FINANCIALSERVICE_H_
#define __FINANCIALSERVICE_H_

#include "LBase.h"

class Work;
class DUser;

class FinancialService
{
public:
	FinancialService(void);
	~FinancialService(void);

public:
	Lint					ChargeDiamondFor(DUser* user, Lint quantity, bool userChargeSelf, const Lstring& orderNo);
	Lint					PurchaseItem(DUser* user, Lint itemId, Lint diamondPaid, Lint cardAdded, Lint cardForVIP, Lint cardType, const Lstring& orderNo);

private:
	bool					UpdateDiamondQuantityToDB(Lint userId, Lint quantityChanged);
	bool					UpdateDataToDB(Lint userId, Lint diamondPaid, Lint cardAdded, Lint cardType);
	void					InsertCardLog(Lint time, Lint userId, Lint cardAdded, Lint cardType, Lint operType, Lint nTotalCardNum, Lint nTeamId);
	Lint					CalcCardNumByType(Lint cardNum, Lint cardType, Lint& cardNumType4, Lint& cardNumType8, Lint& cardNumType16);
	void					NotifyPurchasingResultToLogicServer(Lint logicId, bool succeed, Lint userId, Lint itemId, Lint diamondPaid, Lint cardAdded, Lint cardType, Lint operType);
	void					NotifyChargingResultToLogicServer(Lint logicId, bool succeed, Lint userId, Lint diamondCharged);
	void					InsertDiamondDetailLog(Lint time, Lint userId, Lint diamondNum, Lint reason, const Lstring& orderNo);
};

#endif		// __FINANCIALSERVICE_H_