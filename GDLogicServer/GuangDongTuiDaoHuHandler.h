#ifndef GUANGDONG_TUIDAOHUHANDLER
#define GUANGDONG_TUIDAOHUHANDLER

#include "GuangDongOneHundredHandler.h"


class GameHandler_GuangDongTuiDaoHu : public GameHandler_GuangDongOneHundler
{
public:
	virtual void DeakCard();
	//ËÄ¹íºú
	virtual bool CanFourGhostHu() const;
	//ÎÞ¹í¼Ó±¶
	virtual bool IsNoGhost2TimesScore() const;
	virtual Lint GetSocore(const Lint pos);
	virtual void GenerateGhost();
	virtual void GenerateTwoGhost();
	virtual void GenerateOneGhost();
	virtual void FillspecifichuType(OperateState& gameinfo);
	virtual bool IsBombHorse() const;
	virtual void calBombHorseScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over);
	virtual Lint GetBombHorseBaseScore(Lint pos, const CardValue& card);
	Lint GetMultipleFromCard(const CardValue& card);
	virtual bool IsNoWanCards() const;
};

DECLARE_GAME_HANDLER_CREATOR(GuangDongTuiDaoHuMaJing, GameHandler_GuangDongTuiDaoHu)

#endif
