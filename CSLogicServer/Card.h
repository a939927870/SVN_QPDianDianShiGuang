#ifndef _CARD_H_
#define _CARD_H_

#include "LSingleton.h"
#include "GameDefine.h"
#include "LMsg.h"


struct Card
{
	Lint m_number;	//牌的号码1-9
	Lint m_color;   //牌的花色1-3 1-万，2-筒，3-条

	Card()
	{
		m_number = 0;
		m_color = 0;
	}

	Lint GetNCIndex() const
	{
		return (m_color * 10 + m_number);
	}
};
typedef std::vector<Card*> CardVector;

struct ThinkUnit
{
	ThinkUnit():m_score(0),m_hu_way(1),m_type(0)
	{
	}

	void Clear()
	{
		m_score=0;
		m_type = 0;
		m_card.clear();
		m_hu.clear();
	}

	void count_score()
	{
		m_score=0;
		for(int x=0;x<m_hu.size();x++)
		{
			switch(m_hu[x])
			{
				case HU_XIAOHU:				//小胡
					m_score++;
					break;
				case HU_SPECIAL_XIAOQIDUI:		//豪华小七对
					m_score+=12;
					break;
				case HU_XIAOQIDUI:				//小七对
					m_score+=6;
					break;
				case HU_QINGYISE:				//清一色
				case HU_JIANGJIANGHU:			//将将胡
				case HU_PENGPENGHU:				//碰碰胡
				case HU_QUANQIUREN:				//全球人
				case HU_GANG1:					//杠上开花
				case HU_GANG2:					//杠上炮
				case HU_END1:					//海底捞
				case HU_END2:					//海底炮
				case HU_QIANGGANGHU:			//抢扛胡
				case HU_TianHu://天胡
				case HU_DiHu://地胡
					m_score+=6;
					break;
				case HU_DOUBLESPECIAL_XIAOQIDUI: //双豪华七小对
					m_score+=18;
					break;
				default:
					LLOG_ERROR("error hu type");
			}
		}
	}
	Lint m_type;
	std::vector<Card*>	m_card;
	std::vector<Lint>   m_hu;//胡牌的类型
	Lint m_hu_way;  //1自摸 2抢明杠胡 3抢暗杠胡 4杠上炮 5 杠上花
	Lint m_score;
};

typedef std::vector<ThinkUnit> ThinkVec;

struct OperateState
{
	//用于desk 和 card 之前的参数传递 参数太多了, 有扩展请扩展此处
	Lint m_GameType;	// 0 湖南， 3， 长沙
	bool b_canEat;		// 是否可以吃
	bool b_canHu;		// 是否可以胡
	Lint i_canHuScore;		// 是否可以胡
	bool b_onlyHu;		// 是否只能操作胡
	bool m_thinkGang;	// 单独处理是不是杠的牌
	bool b_guoGang;
	Lint m_deskState;	// 当前局牌状态
	Lint m_playerPos;	// 当前一个出牌位置
	Lint m_cardState;	// 当前一个出牌状态
	Lint m_endStartPos;	// 结束位置，即海底位置
	Lint m_MePos;		// 玩家的位置
	bool m_first_turn;	// 是否为玩家第一轮
	bool m_QiXiaoDui;	// 可以胡七小队
	bool m_pt_laizi;
	bool m_tianhu;
	bool m_dihu;
	CardVector m_gangcard;//过手杠
	CardVector m_louhuCard;

	//邵阳用
	bool m_qiangAnGang;
	bool m_qiangMingGang;
	bool m_gangShangPao;
	bool m_gangShangHua;
	bool m_qiangMing2Gang; //抢3+1明杠
	bool m_getCardThink;

	//永州用
	Card* m_wangBa;//王霸牌
	//湘潭用
	Card* m_wangBa2;//下王

	Lint  m_playerScore;//玩家的分

	bool b_menqing;
	bool b_menqingjiangjianghu;
	OperateState() :m_GameType(0), m_deskState(0), m_playerPos(0), m_cardState(0), m_MePos(0), m_endStartPos(0), m_thinkGang(false), b_canEat(false), b_canHu(true), i_canHuScore(0), b_onlyHu(false), m_QiXiaoDui(false), m_first_turn(false), b_guoGang(false), m_pt_laizi(false), m_tianhu(false), m_dihu(false), m_qiangAnGang(false), m_qiangMingGang(false), m_gangShangPao(false), m_gangShangHua(false), m_getCardThink(true), m_qiangMing2Gang(false), m_wangBa(NULL), m_wangBa2(NULL), b_menqing(false), b_menqingjiangjianghu(false)
	{
		m_louhuCard.clear();
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
	Lint	m_type;
	std::vector<Card*> m_card;
};
typedef std::vector<StartHuCard> StartHuVector;
class CardManager:public LSingleton<CardManager>
{
public:
	virtual bool		Init();
	virtual bool		Final();
	
	//DEBUG 发牌
	bool				GetSpecialOneTwoThreeFour(CardVector& src, CardVector& one, CardVector& two, CardVector& three, CardVector& four);
	//发牌
	void				DealCard(CardVector hand_card[],Lint user_count,CardVector& rest_card,Lint game_type, PlayTypeInfo& playtype);
	//特定牌局 发牌
	void				DealCard2(CardVector hand_card[],Lint user_count,CardVector& rest_card,const Card specialCard[CARD_COUNT],Lint game_type, PlayTypeInfo& playtype);
	//对牌进行排序，万，筒，条，红中
	void				SortCard(CardVector& vec);
	//把des中的相，从src中删除
	void				EraseCard(CardVector& src,CardVector& des);
	//把pCard从src中删除
	void				EraseCard(CardVector& src,Card* pCard);

	//删除src 与pCard 数据一样的牌 N张
	void				EraseCard(CardVector& src,Card* pCard,Lint n);

	//删除一手牌中，相同的牌字，比如2个三万，删除其中一个
	void				EarseSameCard(CardVector& src);

	bool				CheckStartHu(CardVector& handcard,std::vector<StartHuCard>& vec);

	//别人出牌 检测操作类型
	ThinkVec            CheckBaoTing(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo);
	ThinkVec			CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);
	ThinkVec			CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, CardVector& outCards,OperateState& gameInfo,LMsgS2CUserGangThinkCard& send_message);

	//自己摸牌 检测操作类型
	ThinkVec			CheckGetCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard, OperateState& gameInfo);
	ThinkVec			CheckGetCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,CardVector& getCards, OperateState& gameInfo);
	bool				CheckXiaoYiCanGang(CardVector& handcard1, CardVector& handcard2, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard);
	
	//检测是否胡牌
	bool				CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo,std::vector<Lint>& vec, bool isGetCard);
	bool				CheckCanHu_ShuangKai(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard);
	//检测是否杠 
	bool				CheckCanGang(CardVector& handcard,  Card* outCard, OperateState& gameInfo);
	//检测是否碰 																	  
	bool				CheckCanPeng(CardVector& handcard,  Card* outCard, OperateState& gameInfo);

//	bool				CheckHu(CardVector handcard,Card* curCard,bool needJiang, bool isGetCard);

	bool				xiaohu_CheckHu(CardVector handcard,Card* curCard,bool needJiang,bool is_out_card,bool hava_laizi=false);
	bool				xiaohu_CheckHu2(CardVector handcard, Card* curCard, bool needJiang, Card *Wangba = NULL);
	bool				xiaohu_CheckHu3(CardVector handcard, Card* curCard, bool needJiang, Card *Wangba = NULL); //不检查中发白为将牌
	bool                Yongzhou_CheckBaseHu(CardVector handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* curCard, bool is_out_card, Card * wangbaCard, std::vector<HU_PAI_LEIXING> &playtypes);
	bool                XiangTan_CheckBaseHu(CardVector handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* curCard, bool is_out_card, Card * wangbaCard, Card * wangbaCard2, std::vector<HU_PAI_LEIXING> &playtypes);
	bool                YiYang_CheckTing(CardVector handcard, CardVector pengCard, CardVector agangCard, CardVector mgangCard, CardVector eatCard);
	bool                XiaoYi_CheckTing(std::vector<Lint>& card_vector, std::vector<Lint> & huIndexs, std::vector<Lint> & huShunKes,CardVector handcard, CardVector pengCard, CardVector agangCard, CardVector mgangCard, CardVector eatCard);
	bool                XiaoYi_CheckTing_GetHuCards(std::vector<Card>& hu_cards, CardVector handcard, CardVector pengCard, CardVector agangCard, CardVector mgangCard, CardVector eatCard);
	bool                Jingmen_CheckBaseHu(CardVector handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* curCard, bool is_out_card, Card * wangbaCard, std::vector<HU_PAI_LEIXING> &playtypes, OperateState& gameInfo);
	bool				xiaohu_CheckTing(CardVector handcard,bool needJiang);
	bool				xiaohu_CheckTing2(std::vector<Lint> & card_vector, bool needJiang, std::vector<Lint> & huIndexs, std::vector<Lint> & huShunKes);//获取胡牌信息
	bool                XiangTanCheckTing(CardVector handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* curCard, bool isGetCard, OperateState& gameInfo);
	bool				CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,OperateState& gameInfo);
private:
	bool				checkJingmenOtherHu(std::vector<Lint> &card_vector, Card* curCard, bool is_out_card, Card * wangbaCard, Lint wangbaNum, std::vector<HU_PAI_LEIXING> &playtypes, OperateState& gameInfo);
	bool				explorer_zheng_function(std::vector<Lint> cards,Lint index,Lint Lai_num, Lint *wangbashunzi = NULL, Card *Wangba = NULL);
	bool				explorer_zheng_function2(Lint loopsize, std::vector<Lint> & huIndexs, std::vector<Lint> & huShunKes, std::vector<Lint> cards, Lint index, Lint Lai_num, Lint *wangbashunzi = NULL, Card *Wangba = NULL);
	Lint				judge_ke(std::vector<Lint>& targe_vector,Lint index);
	Lint				judge_shun(std::vector<Lint>& targe_vector,Lint index);
	Lint				judge_shun2(std::vector<Lint>& targe_vector, Lint index, std::vector<Lint>& need_vector);
	bool				have_play_type(Lint playtype,std::vector<Lint> playtypes);
public:
	//永州麻将七巧对，龙巧对，硬巧对
	bool  CheckqiaoduiWithWangba(CardVector& handcard, HU_PAI_LEIXING& special, Card* outCard, bool isGetCard, OperateState& gameInfo);
	//检测大胡
	//检测小七对,是否是豪华，special = 1 
	//检测清一色
	bool				CheckQingyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, std::vector<Lint>& vec);
	bool				CheckQingyise2(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, std::vector<Lint>& vec, Card* Wangba = nullptr);
	//检测乱将胡
	bool				CheckLuanjianghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	//检测碰碰胡
	bool				CheckPengpenghu(CardVector& handcard,CardVector& eatCard,Card* outCard, Card *WangBa1 = NULL, Card *WangBa2 = NULL);
	//检测全球人
	bool				CheckQuanqiuren(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard);
	bool                CheckQuanqiuren2(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	bool				CheckQuanqiuren3(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, Card* Wangba = nullptr); //代王霸的全球人

	//起手4个红中赖子
	bool				CheckQishouLaizi(CardVector& handcard);

	//起手4个王霸
	bool				CheckQishouWangBa(CardVector& handcard, OperateState& gameInfo);

	//枪杠胡
	bool				CheckQiangganghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//检测杠上开花
	bool				CheckGangshangkaihua(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//抢杠胡
	bool				CheckGangshangpao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底捞
	bool				CheckHaiDiLao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);
	//海底炮			
	bool				CheckHaiDiPao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,Card* outCard, OperateState& gameInfo);


	//邵阳玩法
public:
	//检测同一色
	bool                CheckXiangTongSe(CardVector& handcard, Card* outCard);

	bool                BaseQingyise(CardVector & handcard, CardVector & eatCard, CardVector & pengCard, CardVector & gangcard, CardVector & mgangcard, Card * outCard, Card* Wangba1, Card* Wangba2);
	//检测清一色
	bool				CheckQingyise(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	//检测清一色大碰对
	bool				CheckQingyiseDapengdui(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	//门清			
	bool				CheckMenQing(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	//风一色			
	bool				CheckFengYiSe(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard);
	bool				CheckFengYiSe2(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard);
	//十三幺			
	bool				CheckShiSanYiao(CardVector& handcard, Card* outCard);

	//永州麻将
public:
	bool				CheckShiSanLan(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, Card* outCard, bool& isqifeng);

public:
	//检查吃
	bool				CheckCanChi(CardVector& handcard,Card* outCard,  std::vector<Card*>& vec, OperateState& gameInfo);
	//检测暗杠 
	bool				CheckAnGang(CardVector& handcard,  std::vector<Card*>& vec, OperateState& gameInfo);

	//检测明杠 
	bool				CheckMGang(CardVector& handcard,CardVector& pengCard,  std::vector<Card*>& vec,OperateState& gameInfo);

	bool				IsContinue(CardVector& result);

	bool				IsSame(CardVector& result);

	bool				IsSame(Card* c1,Card* c2);

	bool				IsNineOne(Card* c);

	bool				HaveTheCard(CardVector& result,Card* thecard);

	Card*				GetCard(Lint m_color,Lint m_number);

	bool                CheckXiaoqidui2(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo);

	bool				CheckXiaoqidui(CardVector& handcard, Lint& special, Card* outCard, bool isGetCard, OperateState& gameInfo);
private:
	void				SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT]);
private:
	Card		wan_base_card[BASE_CARD_COUNT];
	Card        bing_base_card[BASE_CARD_COUNT];
	Card        tiao_base_card[BASE_CARD_COUNT];
	Card        wind_card[WIND_CARD_COUNT];
	Card        Lai_zi_card[LAI_ZI_COUNT];
	Card		yao_13[MAX_HANDCARD_COUNT];

	CardVector  m_3_feng;  //全部3副牌加风
	CardVector  m_3_laizi;	//全部3副牌加癞子
	CardVector  m_wan_feng; //万牌加风 二人转转用到
	CardVector  m_3;//全部3副牌
	CardVector  m_13yao; //13幺牌型
	CardVector  m_ZhuoXiaZi; //所有条、饼，加一万九万各四张
	//CardVector	m_laizi_Vec;
	//CardVector	m_windandzi_Vec;
	//CardVector	m_wancard_Vec;
	//CardVector	m_bingcard_Vec;
	//CardVector	m_tiaocard_Vec;
};

#define gCardMgr CardManager::Instance()

#endif