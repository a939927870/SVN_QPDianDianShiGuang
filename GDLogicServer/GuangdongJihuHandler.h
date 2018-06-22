#ifndef GUANGDONGJIHUTUIDAOHUHANDLER
#define GUANGDONGJIHUTUIDAOHUHANDLER

#include "GuangDongTuiDaoHuHandler.h"


class GuangdongJihuPlayerHu : public GameHandler_GuangDongTuiDaoHu
{
	virtual void DeakCard();
	virtual void calGenZhuangScore(Ldouble gold[]);
	virtual bool IsLiuju();
	virtual bool calcScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[], Lint& bombCount, Lint& zhuangpos, Lint maNum[], LMsgS2CGameOver& over, Lint winner);
	virtual Lint GetSocore(const Lint pos);
	virtual bool IsGenZhangOnlyOnce() const;
	virtual void calBuyHorseScore(Lint winpos[], Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over);
private:
	Lint GetTotalHorseNum();
	void calGuangdongJihuHorseNum(LMsgS2CZhaBirdShow &maPaiStruct, Lint winpos[], Lint maNum[], bool& bHorseHitAll);
};

DECLARE_GAME_HANDLER_CREATOR(GuangDongJihu, GuangdongJihuPlayerHu)

#endif
