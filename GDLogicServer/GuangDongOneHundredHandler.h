#ifndef GUANGDONG_ONEHUNDREDHANDLER
#define GUANGDONG_ONEHUNDREDHANDLER

#include "GameHandler.h"
#include "Desk.h"
#include "Card.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"


class GameHandler_GuangDongOneHundler : public GameHandler
{
public:
	virtual bool startup(Desk *desk);
	virtual void shutdown(void);
	virtual void SetDeskPlay();
	virtual void ProcessRobot(Lint pos, User * pUser);
	virtual void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	virtual void HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg);
	virtual void GenerateGhost();
	virtual void DeakCard();
	virtual void FillspecifichuType(OperateState& gameinfo);
	virtual bool CanFourGhostHu() const;
	virtual Lint GetSocore(const Lint pos);
	virtual Lint GetGhostType() const;
	virtual bool gametypeDianPao() const;
	virtual void SetDeskPlayNum();
	virtual void SetThinkIng();
	virtual void SetPlayIng(Lint pos, bool needGetCard, bool gang, bool needThink, bool canhu, bool bu = false);
	virtual void OnUserReconnect(User* pUser);
	virtual void OnGameOver(Lint result,Lint winpos[],Lint bombpos, Lint winner = 0);
	virtual void CheckStartPlayCard();
	
	virtual bool IsGangshangkaihuaScore() const;
	virtual bool IsPayQianggangHorseScore() const;
	virtual bool calcScore(Lint result, Lint winpos[],Lint bombpos, Ldouble gold[], Lint& bombCount, Lint& zhuangpos, Lint maNum[], LMsgS2CGameOver& over, Lint winner);
	virtual void calHorsrScore(Lint result,Lint winpos[],Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over, Lint winner);
	virtual void calCardTypeScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[]);
	virtual void calBuyHorseScore(Lint winpos[],Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over);
	virtual void calBombHorseScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over);
	virtual void calHorseGenGangScore(Lint winpos[], Lint bombpos, Ldouble gold[], const std::vector<CardValue>& horsecards, Lint winner);
	virtual void ThinkEnd();
	virtual void calHorseNum(LMsgS2CZhaBirdShow &maPaiStruct, Lint winpos[], Lint maNum[]);
	virtual void calRedDragonSceor(Lint winpos[], Lint bombpos, Ldouble gold[]);
	virtual bool CanQiangGanghu() const;
	virtual bool IsBompHorseAddScoreType() const;
	virtual bool IsBompHorseMultipleScoreType() const;
	virtual bool CanTianhu();
	virtual bool CanDihu();
	virtual bool CanQiangGangHuHorseJiaBei();
	virtual bool CanHuangZhuangHuangGang();

	virtual bool IsNeedWindCards() const;
	virtual bool IsHorseGenGang() const;
	virtual bool IsSteadilyHighType() const;
	virtual bool IsBombHorse() const;
	virtual bool IsGenZhangOnlyOnce() const;
	virtual Lint GetMaPaiBaseScore(const Lint pos);
	virtual void UpdateZhuangPos(Lint result, Lint winpos[], Lint bombpos, Lint& zhuangpos);
	virtual bool calcMaPai(Lint zhuangPos, Lint winPos, Lint maPaiNum);
	virtual void calGenZhuangScore(Ldouble gold[]);
	virtual bool IsLiuju();
	virtual void calGangScore(Lint result, Ldouble gold[]);

	bool IsNoGhostType();
	bool IsNoHorseType();
	void CheckThink();
	void VideoThink(Lint pos);
	void VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number);
	bool CheckIfBigHu(std::vector<Lint>& vec);
	bool IsXiaoQiDuiType();
	Lint GameBuyhorseNum();
	Lint GetGengzhuangBaseScore();
	void UpdateGenZhuangInfo(const Card* const outCard, const Lint pos, const std::vector<CardValue>& cards);
	
	bool IsZhuang(Lint pos);
	bool IsQiduiDoubleScore() const;
	bool IsGhostCardBaiban() const;
	bool CanGengZhuang() const;
	bool IsMaGenDifen() const;
	bool IsTiDihu10Time() const;
	//无鬼加倍
	bool IsNoGhost2TimesScore() const;
	//四鬼加倍
	bool IsFourGhost2TimesScore() const;
	//对对胡加倍
	bool IsDuiduihuType() const;
	//清一色四倍
	bool IsQingyiseType() const;
	//幺九六倍
	bool IsYaojiuType() const;
	//幺九牌含1或者9即可
	bool IsYaojiuHasOneOrNine() const;
	//字一色八倍
	bool IsAllWindType() const;
	//十三幺八倍
	bool IsShisanyaoType() const;
	//混一色双倍
	bool IsHunyise2TimesScore() const;

	//双豪华七对
	bool IsDLuxuryqiduiType() const;
	//三豪华
	bool IsTLluxuryqiduiType() const;
	//十八罗汉
	bool IsEighteenarhats() const;
	//小三元
	bool IsLittleSanyuanType() const;
	//大三元
	bool IsBigSanyuanType() const;
	//小四喜
	bool IsLittleSixiType() const;
	//大四喜
	bool IsBigSixiType() const;

	bool IsMingGangkeqiang() const;
	bool IsQiangGangHu(std::vector<Lint>& hu);
	bool IsGangshangKaihuaHu(std::vector<Lint>& hu);
	Lint GetGhostCardNum(Lint pos) const;
	bool IsValidPos(const Lint& pos) const;
	const std::vector<CardValue> GetGhostCard() const;
	void UpdateEndInfo(Lint winpos[], Lint maNum[], const Lint result);
	void UpdateSuccessiveInfo(Lint prezhuangPos, Lint curzhuangPos);

protected:
	const std::vector<CardValue> GetHorseFromBirdInfo(const std::vector<BirdInfo>& birdinfo);
	bool m_gengZhengOnceEnd;

protected:
	void calPlayerHitHorseNum(const std::vector<CardValue>& horsecards, Lint maNum[]);
	Lint GetHorseGenGangHuPos(Lint winpos[], Lint bombpos, Lint winner ) const;
	Lint GetQihuFanshu();

public:
	Lint			m_maNum[DEFAULT_DESK_USER_COUNT];
	Lint			m_successive[DEFAULT_DESK_USER_COUNT];		//连庄	
	Lint			m_ghostZimo[DEFAULT_DESK_USER_COUNT];		//玩家有鬼自摸次数
	Lint			m_noghostZimo[DEFAULT_DESK_USER_COUNT];		//玩家无鬼自摸次数
	std::vector<CardValue>	m_ghostCardReal;			//鬼牌
	Card			m_ghostCardFlop;					//翻出的鬼牌 发送给客户端
	bool			m_oper_gang;						//别人出牌然后杠
	GengZhuangInfo  m_gengzhuangInfo;
	Lint			m_redDragonNum[DEFAULT_DESK_USER_COUNT];	//红中马数量
	CardVector		m_flowerCard[DEFAULT_DESK_USER_COUNT]; //玩家已开花牌
protected:
	Lint			m_playCardNum;		//梅州红中宝的地胡和别的玩法不一样，第二家自摸才是地胡
	Lint			m_operateNum;		//所有玩家吃碰杠次数
	bool			m_canTianHu;		//天胡
	bool			m_canDiHu;			//地胡

	OLD_TICK_TUOGUAN_DECLARE
};

DECLARE_GAME_HANDLER_CREATOR(GuangDongOneHundred, GameHandler_GuangDongOneHundler)

#endif