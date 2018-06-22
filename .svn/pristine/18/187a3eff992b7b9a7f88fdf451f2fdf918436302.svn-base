#ifndef _CARD_H_
#define _CARD_H_

#include "LSingleton.h"
#include "GameDefine.h"
#include "LVideo.h"

#define  YINGBIN_SUANFA_VECLEN 44
#define  YINGBIN_SUANFL_CHARLEN 176
struct  NewHandCard;
struct Card
{
	Lint m_number;	//牌的号码1-9
	Lint m_color;   //牌的花色1-3 1-万，2-筒，3-条

	Card()
	{
		m_number = 0;
		m_color = 0;
	}
	bool operator == (const Card& c)	//用于std::find查找
	{
		return m_number == c.m_number && m_color == c.m_color;
	}
	bool operator<(const Card& c) const      //用于map,set
	{
		if (m_number < c.m_number)
		{
			return true;
		}
		else if (m_number > c.m_number)
		{
			return false;
		}
		else
		{
			if (m_color < c.m_color)
			{
				return true;
			}
			return false;
		}
	}
	void clear()
	{
		m_number = 0;
		m_color = 0;
	}
	operator bool()const
	{
		return m_number != 0 && m_color != 0;
	}

	Card operator =(const CardValue& other)
	{
		m_number = other.m_number;
		m_color = other.m_color;
		return *this;
	}
};

typedef std::vector<const Card*> CardVector;
typedef std::vector<Card>  EntiryCardVector;
typedef std::vector<Card>::const_iterator  EntiryCardVectorItor;

struct ThinkUnit
{
	ThinkUnit()
	{
		m_type = 0;
	}

	void Clear()
	{
		m_type = 0;
		m_card.clear();
		m_hu.clear();
	}
	Lint m_type;
	CardVector	m_card;
	std::vector<Lint> m_hu;//胡牌的类型
};

typedef std::vector<ThinkUnit> ThinkVec;

struct OperateState
{
	//ToDO 优化
	Lint m_GameType;	// 0 湖南， 3， 长沙
	bool b_canEat;		// 是否可以吃
	bool b_canHu;		// 是否可以胡
	bool b_onlyHu;		// 是否只能操作胡
	bool m_thinkGang;	// 单独处理是不是杠的牌
	Lint m_deskState;	// 当前局牌状态
	Lint m_playerPos;	// 当前一个出牌位置
	Lint m_cardState;	// 当前一个出牌状态
	bool m_hasDianGang;	// 当前是否发生点杠
	Lint m_endStartPos;	// 结束位置，即海底位置
	Lint m_MePos;		// 玩家的位置
	bool m_first_turn;	// 是否为玩家第一轮
	bool m_QiXiaoDui;	// 可以胡七小队
	Lint m_qihuFanshu;	// 起胡番数	
	Lint m_fengQuan;		// 风圈
	bool m_canTianhu;	//天胡
	bool m_canDihu;		//地胡
	bool m_canRenhu;	//人胡
	int  m_ghostType;	//鬼牌玩法类型
	std::vector<CardValue> m_ghostCard;	//鬼牌
	bool m_zimoType;	//自摸	
	bool m_canqiangganghu; //是否可以抢杠胡
	bool m_canfourghosthu;	//是否可以四鬼胡牌
	//做牌推到胡
	bool m_duiduihuType;	//对对胡
	bool m_qingyiseType;	//清一色
	bool m_qiduiType;		//七对
	bool m_yaojiuType;	//幺九
	bool m_yaojiuHas1or9;	//幺九有1或9
	bool m_allwindType;	//全风
	bool m_shisanyaoType;	//十三幺
	//潮汕麻将
	bool m_luxuryqiduiType; //豪华七对
	bool m_doubletimeqiduiType; //双倍七对
	bool m_hunyiseType;		//混一色
	bool m_dluxuryqidui;	//双豪华七对
	bool m_tluxuryqidui;	//三豪华
	bool m_eighteenarhats;	//十八罗汉
	bool m_canlittleHu;	//鸡胡
	bool m_haidilao2timeType;	//海底捞

	bool m_littleSixiType;	//小四喜
	bool m_bigSixiType;		//大四喜
	bool m_littleSanyuanType;	//小三元
	bool m_bigSanyuanType;		//大三元
	//Hongkong MJ
	Lint m_additionFanNum;		//附加番数
	bool m_bZhuang;				//是否庄家
	bool m_bHuSevenFlower;		//是否可以胡七花
	bool m_louPeng;				//漏碰
	bool m_tenfanjiepao;		//10番听牌可接炮
	Lint m_nTingFanshu;			//揭阳可听牌番数
	bool m_canJihuQianggang;	//鸡胡可抢杠胡
	//Jiexi
	bool m_isNoGhost2Times; //是否无鬼加倍
	Lint m_JiexiJIDAORJIHU;	//揭西玩法中的鸡大或者鸡胡

	//raoping
	bool m_cannotJihu = false;			//鸡胡不能胡
	bool m_cannotJihuDianpao = false;	//鸡胡不能点炮
	CardVector	m_louGangCard;	//漏杠的牌
	bool m_gangHouPao2TimeScore = false; //杠后炮双倍
	bool m_danDin2TimeScore = false;	//单吊双倍

	OperateState():m_GameType(0), m_deskState(0), m_playerPos(0), m_cardState(0), m_MePos(0), m_endStartPos(0), m_thinkGang(false)
		,b_canEat(false),b_canHu(true),b_onlyHu(false),m_QiXiaoDui(false),m_qihuFanshu(0),m_fengQuan(0),m_canTianhu(false)
		,m_canDihu(false),m_canRenhu(false),m_ghostType(PT_GHOSTCARDNO),m_first_turn(false),m_zimoType(true),m_canqiangganghu(false),m_canfourghosthu(false)
		,m_duiduihuType(false),m_qingyiseType(false),m_qiduiType(false),m_yaojiuType(false),m_allwindType(false),m_shisanyaoType(false),m_yaojiuHas1or9(false)
		,m_luxuryqiduiType(false),m_hunyiseType(false),m_doubletimeqiduiType(false), m_hasDianGang(false), m_dluxuryqidui(false),m_tluxuryqidui(false),m_eighteenarhats(false)
		,m_canlittleHu(false), m_haidilao2timeType(false),m_littleSixiType(false), m_bigSixiType(false), m_littleSanyuanType(false), m_bigSanyuanType(false), m_additionFanNum(0)
		,m_bZhuang(false), m_bHuSevenFlower(false), m_louPeng(false), m_tenfanjiepao(false), m_nTingFanshu(0), m_canJihuQianggang(false)
		,m_isNoGhost2Times(false), m_JiexiJIDAORJIHU(0)
	{
		m_ghostCard.clear();
		m_louGangCard.clear();
	}

};


struct StartHuCard
{
	StartHuCard()
	{
		m_type = START_HU_NULL;
	}

	void	Clear()
	{
		m_type = START_HU_NULL;
		m_card.clear();
	}
	Lint m_type;
	CardVector m_card;
};
typedef std::vector<StartHuCard> StartHuVector;
class CardManager:public LSingleton<CardManager>
{
public:
	virtual bool		Init();
	virtual bool		Final();
	
	//发牌
	void				DealCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind = true, bool needBaiBan = true, bool needHongzhong = false, bool needWan = true, bool needHua = false);
	void				DealCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[]
							,bool needWind = true, bool needBaiBan = true, bool needHongzhong = false, bool needWan = true, bool needHua = false);

	//特定牌局 发牌
	void				DealCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind = true, bool needBaiBan = true, bool needHongzhong = false, bool needWan = true, bool needHua = false);

	//发全牌
	void				DealAllCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5);
	void				DealAllCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT]);
	void				DealAllCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[CARD_COUNT]);
	void				DealGeneratedCards(CardVector& mCards, CardVector* playerHandCard, Lint playerCount, CardVector& pile, Card specialCard[]);
	//发香港麻将
	void				DealHongKongCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5);
	void				DealHongKongCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT]);

	//发惠州庄-花
	void				DealHuizhouZhuangFlowerCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5);
	void				DealHuizhouZhuangFlowerCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT]);
	void				DealHuizhouZhuangFlowerCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[CARD_COUNT]);
	//鸡胡补花
	void				DealJiHuFlowerCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5);
	void				DealJiHuFlowerCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT]);
	//发百搭鸡
	void				DealJokerChickenCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind = true);
	void				DealJokerChickenCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind = true);
	//发三人推到牌
	void				DealThreePlayerCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, bool needWind = true, bool needBaiBan = true, bool nowan = false);
	void				DealThreePlayerCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, const Card specialCard[CARD_COUNT], bool needWind = true, bool needBaiBan = true, bool nowan = false);
	//发二人推到牌
	void				DealTwoPlayerCard(CardVector& v1, CardVector& v2, CardVector& v3);
	void				DealTwoPlayerCard2(CardVector& v1, CardVector& v2, CardVector& v3, const Card specialCard[CARD_COUNT]);
	//发广东鸡胡
	void				DealGuangdongJiHuCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind = true);
	void				DealGuangdongJiHuCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind = true);
	//发红中王牌
	void				DealRedDragonCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind = true, bool needBaiBan = true);
	void				DealRedDragonCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind = true, bool needBaiBan = true);
	void				DealRedDragonCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[CARD_COUNT]
							, bool needWind = true, bool needBaiBan = true);
	//发惠州庄
	void				DealHuizhouZhuang(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, Lint flowerNum = 0);
	void				DealHuizhouZhuang2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], Lint flowerNum = 0);
	void				DealHuizhouZhuang(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[]
							,Lint flowerNum = 0);
	//发梅州红中宝牌
	void				DealMeizhouHongzhongbao(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5);
	void				DealMeizhouHongzhongbao2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT]);
	//发揭阳牌
	void				DealJieyang(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5)const;
	void				DealJieyang2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT])const;
	void                DealJieyang(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[]);
	//发韶关牌
	void				DealShaoguan(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needOneNineWan);
	void				DealShaoguan2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needOneNineWan);

	//对牌进行排序，万，筒，条，红中
	void				SortCard(CardVector& vec) const;
	//把des中的相，从src中删除
	void				EraseCard(CardVector& src, CardVector& des) const;
	//把pCard从src中删除
	void				EraseCard(CardVector& src, const Card* pCard) const;
	//把Card从src中删除
	Lint				EraseCard(CardVector& src, const Card& card) const;
	Lint                EraseCard(CardVector& src, const std::vector<CardValue>& ghostCard) const;
	bool				IsGhostCard(const std::vector<CardValue>& ghostCard, const Card * pCard) const;
	//删除src 与pCard 数据一样的牌 N张
	void				EraseCard(CardVector& src, const Card* pCard, Lint n) const;
	//删除一手牌中，相同的牌字，比如2个三万，删除其中一个
	void				EarseSameCard(CardVector& src) const;
	//别人出牌 检测操作类型
	ThinkVec			CheckOutCardOperator(CardVector& handcard, CardVector& pengCard, CardVector& mgangCard, CardVector& agangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo) const;
	//自己摸牌 检测操作类型
	ThinkVec			CheckGetCardOperator(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* getCard, OperateState& gameInfo) const;
	//检测是否胡牌
	bool				CheckCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
							CardVector& eatCard,  const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//推到胡检测是否胡牌
	bool				CheckTuiDaohuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//鸡平胡检测是否胡牌
	bool				CheckJipinghuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//转转检测是否胡牌
	bool				CheckZhuanzhuanCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//一百张检测是否胡牌
	bool				CheckOneHundredCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//百搭鸡胡检测是否胡牌
	bool				CheckJokerChickenHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//惠州庄是否胡牌
	bool				CheckHuizhouzhuangGhostHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//做牌推到胡检测是否胡牌
	bool				CheckCustomTuiDaoCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//饶平胡牌
	bool				CheckRaopingCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;


	//潮汕是否胡牌
	bool				CheckChaoShanCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//红中王是否胡牌
	bool				CheckRedDragonCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//香港麻将是否胡牌
	bool				CheckHongKongCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//惠州庄-花胡牌
	bool				CheckHuizhouzhuangFlowerHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;

	//惠阳胡牌
	bool				CheckHuiYangHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;

	//鸡胡补花胡牌
	bool				CheckJiHuFlowerHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//检查二人推到胡胡牌
	bool				CheckTwoPlayerTuidaoHuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//检查广东鸡胡胡牌
	bool				CheckGuangdongJihuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//检查梅州红中宝
	bool				CheckMeizhouHongzhongbaoCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//检查揭阳麻将
	bool				CheckGuangdongJieyangCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//检查揭西鸡胡牌
	bool				 CheckGuangdongJiexiJihuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType) const;
	//检查揭西麻将
	bool				CheckGuangdongJiexiCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	
	//检查韶关
	bool				CheckShaoguanCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, \
		CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType = 0) const;
	//起手4个红中赖子(转转玩法，没有发财白板)
	bool				CheckQishouLaizi(CardVector& handcard) const;
	//起手4个红中赖子
	bool				CheckQishouHongzhong(const CardVector& handcard, const Card* outCard) const;
	//检测是否杠 
	bool				CheckCanGang(CardVector& handcard, const Card* outCard, bool bzhuanzhuan = false) const;
	//检测是否碰 
	bool				CheckCanPeng(CardVector& handcard, const Card* outCard, bool bzhuanzhuan = false) const;
	bool				CheckHu(CardVector handcard, const Card* curCard, bool needJiang, bool isGetCard) const;
	//bool				CheckHu2(CardVector handcard, const Card* outCard, bool needJiang) const;
	//检查吃
	bool				CheckCanChi(CardVector& handcard,const Card* outCard, CardVector& vec) const;
	//检测暗杠 
	bool				CheckAnGang(CardVector& handcard, CardVector& vec, bool zhuanzhuan = false) const;
	//检测明杠 
	bool				CheckMGang(CardVector& handcard, CardVector& pengCard, CardVector& vec, bool zhuanzhuan = false) const;
	bool				IsContinue(CardVector& result) const;
	bool				IsSame(const CardVector& result) const;
	bool				IsSame(const Card* c1, const Card* c2) const;
	bool				IsNineOne(const Card* c) const;
	bool				IsZi(const Card* c) const;
	bool				IsHongzhong(const Card* c) const;
	bool				IsFlower(const Card* c) const;
	bool				IsSameColor(const CardVector& result) const;
	bool				HaveZi(const CardVector& cards) const;
	bool				GetSpecialOneTwoThreeFour(CardVector& src, CardVector& one, CardVector& two, CardVector& three, CardVector& four) const;
	const Card*			GetCard(Lint color,Lint number) const;
	bool				IsHaveGang( CardVector& src) const;
	bool				CanHongzhongPengGangType(const OperateState& gameInfo) const;
	bool				IsHaveRedDragon(const CardVector& src) const;
	bool				IsHaveFlower(const CardVector& src) const;

	Lint				GetJipingFanNumByHuType(const int hutype) const;
	Lint				GetHongkongFanFromHuType(const Lint huType) const;
	Lint				GetMeizhouHongzhongbaoFanFromHuType(const Lint huType) const;
public: //广东胡牌类型
	//检测小七对,是否是豪华，special = 1 
	bool				CheckXiaoqidui(const CardVector& handcard, const Card* outCard, bool isGetCard, Lint ghostType, const std::vector<CardValue>& ghostCard, bool zimoType, Lint& special, bool& isQingQidui) const;
	//检测乱将胡
	bool				CheckLuanjianghu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测 平胡
	bool				CheckPingHu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测碰碰胡
	bool				CheckPengpenghu(CardVector& handcard, CardVector& eatCard, const Card* outCard) const;
	//检测混一色
	bool				CheckHunYiSe(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测混碰
	bool				CheckHunPeng(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测清一色
	bool				CheckQingyise(const CardVector& handcard, const CardVector& eatCard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard)  const;
	//检测清碰
	bool				CheckQingPeng(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测混幺九
	bool				CheckHunyaojiu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测惠州花幺九
	bool				CheckHuizhouFloweryaojiu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, bool& qingyise) const;
	//检测小三元
	bool				CheckXiaosanyuan(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测小四喜
	bool				CheckXiaosixi(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测字一色
	bool				CheckZiyise(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测七小队和碰碰胡的字一色
	bool				CheckQixiaoduiOrPengPengHuZiyise(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const bool isQiXiaoDui, const bool isPengPenghu) const;
	//检测清幺九
	bool				CheckQingyaojiu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测大三元
	bool				CheckDasanyuan(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测大四喜
	bool				CheckDasixi(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//检测十三幺
	bool				CheckShisanyao(CardVector& handcard, const Card* outCard) const;
	//检测九莲宝灯
	bool				CheckJiulianbaodeng(CardVector& handcard, const Card* outCard) const;
	//检测全球人
	bool				CheckQuanqiuren(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//枪杠胡
	bool				CheckQiangganghu(OperateState& gameInfo) const;
	//明杠可抢
	bool                CheckMinggangkeqiang(OperateState& gameInfo) const;
	//检测杠上开花
	bool				CheckGangshangkaihua(const OperateState& gameInfo) const;
	//抢杠胡
	bool				CheckGangshangpao(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, OperateState& gameInfo) const;
	//海底捞
	bool				CheckHaiDiLao(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, OperateState& gameInfo) const;
	bool				CheckNoGhost2Times(const OperateState& gameInfo) const;
	//海底炮			
	//bool				CheckHaiDiPao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard, OperateState& gameInfo);
	//门清			
	bool				CheckMenQing(CardVector& handcard, CardVector& eatCard,  CardVector& pengCard,CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//四个鬼牌
	bool				CheckFourGhost(CardVector& handcard, const Card* outCard, bool is_get_card, const std::vector<CardValue>& ghostCard) const;

	//花吊花、花吊单
	bool				CheckFlowerMatch( CardVector& handcard, const Card* outCard, bool is_get_card, bool& bmatchKe, bool& bmatchNomal, bool& bmatchQidui, bool shisanyaohu = false, bool qixiaohuihu = false) const;
	//特殊类型花吊判断
	bool				CheckSpecialFlowerMatchType(const CardVector& handcard,  const Card* getCard, Lint ghostNum, bool shisanyaohu = false, bool qixiaohuihu = false) const;
	bool				CheckShisanyaoFlowerMatchType(const CardVector& handcard, const Card* getCard, Lint ghostNum) const;
	bool				CheckQixiaoduiFlowerMatchType(const CardVector& handcard, const Card* getCard, Lint ghostNum) const;

	bool				IsJipingFanShuCanQihu(const int qihufanshu, const std::vector<Lint>& vecHuType) const;
	bool				IsHongKongFanShuCanQihu(const int qihufanshu, const std::vector<Lint>& vecHuType, Lint additionFanNum) const;
	bool				IsMeizhouHongzhongbaoCanQihu(const int qihufanshu, const std::vector<Lint>& vecHuType) const;
	bool				Ghost_CheckHu(CardVector handcard, const Card* curCard,bool needJiang,bool is_out_card, const std::vector<CardValue>& ghostCard, int ghostType = 0) const;

	//一百张胡牌类型
	bool				CheckShisanyaoGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const; 
	bool				CheckAllWindGhostType(CardVector& handcard,  CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckYaoJiuGhostType(CardVector& handcard,  CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckYaoJiuHasOneOrNineHu(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckQingyiseGhostType(CardVector& handcard,  CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckDuiduihuGhostType(CardVector& handcard,  CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckHunYiseGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckQingDuiGhostType(CardVector& handcard,  CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckHunpengGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;

	//潮汕类型
	bool				CheckBigSanyuanGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckLittleSanyuanGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckBigSixiGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckLittleSixiGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;

	bool				CheckEighteenarhtsType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	bool				IsOneHundredSpecialHuType(std::vector<Lint> hu_vec) const;

	bool				CheckQingyaojiuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckHunyaojiuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	//梅州红中宝
	bool				CheckNoHongzhong(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	//揭阳麻将
	bool				CheckJieYangHunyaojiuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckJieYangXiaoyaoGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const;

	//听牌类型检查
	bool				CheckTingShisanyaoGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckTingXiaoqidui(const CardVector& handcard, Lint ghostType, const std::vector<CardValue>& ghostCard, bool zimoType, Lint& special, bool& isQingQidui) const;
	bool				CheckTingEighteenarhtsType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard) const;
	bool				CheckTingDuiduihuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckTingQingyaojiuGhostType(bool bTingQidui, bool bTingDuidui, CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckTingAllWindGhostType(bool bTingQidui, bool bTingDuidui, CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCard) const;
	bool				CheckJieyangCardTypeCanTingTenFan(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, Lint ghostType) const;
	bool				CheckDandiao(const CardVector& handcard) const;
	bool	            CheckHuiyangyaojiu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, bool& qingyise) const;

	//begin:龙门自由庄
	bool LongMenZiyouzhuang_GhostCheckHu(CardVector handcard, const Card* curCard, bool is_get_card, const std::vector<CardValue>& ghostCard, Lint ghostType, Lint handGhostNum) const;
	bool LongMenZiyouzhuang_CheckShisanyaoGhostType(CardVector handcard, CardVector pengCard, const Card* outCard, Lint handGhostNum) const;
	//isGetCard是否是摸到的牌,该函数必须配合LongMenZiyouzhuang_GhostCheckHu使用
	bool LongMenZiyouzhuang_CheckDaJiHu(CardVector handcard, const CardVector& pengCard, const CardVector& gangCard,const CardVector& mgangCard, const Card* currCard, Lint handGhostNum) const;
	//龙门自由庄的牛杂处理和对对胡一样
	bool LongMenZiyouzhuang_CheckDuiduihuGhostType(CardVector handcard,const CardVector& pengCard,const CardVector& gangcard,const CardVector& mgangcard, const Card* outCard,const Lint ghostNum) const;
	//使用ghostcheckcard检测不出来大哥大的胡法
	bool LongMenZiyouzhuang_CheckDaGeDaHu(CardVector handcard, const CardVector& pengCard,const CardVector& gangcard,const CardVector& mgangcard, const Card* outCard, Lint ghostNum) const;
	//配合LongMenZiyouzhuang_GhostCheckHu使用，有一有九有幡子
	bool LongMenZiyouzhuang_CheckFloweryaojiu(CardVector handcard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard) const;
	//配合LongMenZiyouzhuang_GhostCheckHu使用，有同色的一九只能有一对,有幡子
	bool LongMenZiyouzhuang_CheckChunyaojiu(CardVector handcard, const CardVector& pengCard,const CardVector& gangcard,const CardVector& mgangcard, const Card* outCard, Lint ghostNum) const;
	bool LongMenZiyouzhuang_CheckHunpengGhostType(const CardVector& handcard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard, const Lint ghostNum) const;
private:
	//普通十三幺
	bool LongMenZiyouzhuang_NormalShisanyaoType(CardVector handcard, const Card* outCard, const Lint handGhostNum) const;
	//特殊十三幺->参数:isYaojiuOrFanzi == true 表示幺九十三幺，否则表示幡子十三幺
	bool LongMenZiyouzhuang_SpecialShisanyaoType(CardVector handcard, const CardVector& pengCard,const Card* outCard, Lint handGhostNum,bool isYaojiuOrFanzi) const;
	bool LongMenZiyouzhuang_CheckHunYiSe(CardVector handcard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard) const;
	//end:龙门自由庄

public:
	int					CountCard(const CardVector& result, const Card& card) const;
	bool				IsSame(const Card& c1, const Card& c2) const;
	bool				IsQingZi(const std::vector<Card>& result) const;
	bool				IsQingTong(const std::vector<Card>& result) const;
	bool				IsQingTiao(const std::vector<Card>& result) const;
	bool				IsQingWan(const std::vector<Card>& result) const;
	bool				IsHave(CardVector& vec, Card* pCard); //判断数组中有没有这个牌(数字不是指针)
	bool				IsHave(const std::vector<Card>& vec, const Card& Card);

	//考贝自张颖斌的算法
	bool				xiaohu_CheckHu_ConstRef(const NewHandCard&, const Card& LaiCard, bool bHandCardHavePlainGhost, bool need258Jiang = false);
 	///NewhandCard 开始
	//发牌
	void				DealCard(NewHandCard& v1, NewHandCard& v2, NewHandCard& v3, NewHandCard& v4, CardVector& v5, bool needWind = true, bool needBaiBan = true, bool needHongzhong = false, bool needWan = true, bool needHua = false)const;
	//特定牌局 发牌
	void				DealCard2(NewHandCard& v1, NewHandCard& v2, NewHandCard& v3, NewHandCard& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind = true, bool needBaiBan = true, bool needHongzhong = false, bool needWan = true, bool needHua = false)const;
	///NewhandCard 结束

private:
	bool				explorer_zheng_function(std::vector<Lint> cards, Lint index, Lint Lai_num) const;
	bool				explorer_ke_function(std::vector<Lint> cards, Lint index, Lint Lai_num) const;
	Lint				judge_ke(std::vector<Lint>& targe_vector, Lint index) const;
	Lint				judge_shun(std::vector<Lint>& targe_vector, Lint index) const;
	void				SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT]) const;
	Lint				GetJianKeNum(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const;
	Lint				GetOutCardsSpecifyColorKeNum(const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Lint& colorType) const;
	void				CheckFengCardKe(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard,	int fengWei, int fengQuan, std::vector<int>& vetHu) const;
	Lint				GetJiulianbaodengNum(const Lint& pos) const;
	Lint				GetLuxuryqiduiType(Lint special, const OperateState& oper) const;
	void				GetSpecifyColorCards(const CardVector& totalCards, CardVector& specifyCards, CardVector& restCards, const Lint& colorType) const;
	bool				CheckJieyangCardTypeCanJiepao(const std::vector<Lint>& huVec, Lint nTingFanshu) const;
	
	//比赛场
	bool				explorer_zheng_function_ConstRef(const Lint(&cards)[YINGBIN_SUANFA_VECLEN], Lint index, Lint Lai_num);
	Lint				judge_ke_ModifyRef(Lint(&cards)[YINGBIN_SUANFA_VECLEN], Lint index);
	Lint				judge_shun_ModifyRef(Lint(&cards)[YINGBIN_SUANFA_VECLEN], Lint index);
private:
	Card		m_card[CARD_COUNT];
	CardVector	m_cardVec;

	//十三幺牌型， 为了比较方便
	Card		m_ShiSanYiao[13];
	CardVector	m_ShiSanYiaoVec;
};

#define gCardMgr CardManager::Instance()

#endif