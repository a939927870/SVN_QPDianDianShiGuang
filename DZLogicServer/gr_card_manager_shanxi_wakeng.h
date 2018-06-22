#ifndef _CARD_MANAGER_H_
#define _CARD_MANAGER_H_

#include "gr_card_wakeng.h"
#include "LVideo.h"

#define CARD_COUNT_WAKENG	(52)

//////////////////////////////////////////////////////////////////////////
class CardStyle_Shanxi_Wakeng
{
public:
	CardStyle_Shanxi_Wakeng();
	~CardStyle_Shanxi_Wakeng();

	void        SortCard(CardVec_Wakeng& vce);//对牌按大到小排序;
	void        SortCardLaiziFront(CardVec_Wakeng& vce);//癞子放前面;
	void        SortCardLaiziInsert(CardVec_Wakeng& vce);//癞子替换掉;

	void        EarseCard(CardVec_Wakeng& des,CardVec_Wakeng& src);//删除des中属于src的牌;
	void		CheckCardStyle(CardVec_Wakeng const& src,CardStyleNumberVec& style,bool last);//检查所有符合牌型;
	bool		CheckCardStyle(CardVec_Wakeng const& src,BYTE style,BYTE& cardNumber);//以某种牌型来检查当前的牌;
	int			CanBig(DWORD dsCount,BYTE dsStyle,BYTE dsNumber,CardStyleNumberVec& style, bool isUseBomb);
	bool        CheckHasBomb(CardVec_Wakeng const& Src, Lint* nBombCard = NULL);//查看手牌中是否有炸弹;

	void		GetSingleDoubleThreeFour(CardVec_Wakeng const& vec,CardVec_Wakeng& single,CardVec_Wakeng& duizi,CardVec_Wakeng& three,CardVec_Wakeng& four);
	void		GetJokeNumber2(CardVec_Wakeng const& vec,CardVec_Wakeng& joke,CardVec_Wakeng& number2,CardVec_Wakeng& other);
	void		GetLaizi(CardVec_Wakeng const& vec,CardVec_Wakeng& laizi,CardVec_Wakeng& other);
	void		GetBombLaiziJokeNumber2(CardVec_Wakeng const& vec,CardVec_Wakeng& bomb,CardVec_Wakeng& laizi,CardVec_Wakeng& joke,CardVec_Wakeng& number2,CardVec_Wakeng& other);
	bool		CheckContinuous(std::vector<BYTE>& vec);
	//bool        HasRocket(CardVec_Wakeng const& vec);

	bool        IsBiZhua(CardVec_Wakeng const& vec);
	bool        IsBiDao(CardVec_Wakeng const& vec);

	int			GetCardScore(CardVec_Wakeng& vec);
private:

	//bool        IsRocket(CardVec_Wakeng const& vec,BYTE& cardType);	//是否是双王;
	bool        IsDouble(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);		//对子;
	//bool        IsBomb1(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);		//纯软炸弹;
	bool        IsBomb2(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);		//纯硬炸弹;
	//bool        IsBomb3(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);		//软炸弹;
	//bool        IsBombToSingle(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);	//四带单;
	//bool        IsBombToDouble(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);	//四带对;
	bool        IsThree(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);		//三个;
	//bool        IsThreeToSingle(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);//三带单;
	//bool		IsThreeToDouble(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);//三带对;
	//bool        IsThreeList(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);		//飞机;
	//bool        IsThreeListToSingle(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);//飞机带单;
	//bool		IsThreeListToDouble(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);//飞机带对;
	bool        IsSingleList(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);//是否是单顺;
	bool        IsDoubleList(CardVec_Wakeng const& vec,CardVec_Wakeng const& laizi,BYTE& cardType);//是否是双顺;
public:
	static CardStyle_Shanxi_Wakeng* Instance();
};

///////////////////////////////////////////////////////////////////////////
//发牌的类;
class CardDeal_Shanxi_Wakeng
{
public:
	CardDeal_Shanxi_Wakeng();
	~CardDeal_Shanxi_Wakeng();

	void		ClearLaizi();
	void		SetLaizi(BYTE bCardNumber);
	void		DealCard(CardVec_Wakeng& vec1, CardVec_Wakeng& vec2, CardVec_Wakeng& vec3, Lint cardCount, CardValue* tiaopai = NULL, Lint nLenth = 0);
	void        GetLeftCard(CardVec_Wakeng& leftCard);
	void        SS(Lint nCardPos, Lint nColor, Lint nNumber);         // 调牌辅助函数;
	void        TiaoPai(CardValue tiaopai[], Lint nLenth);                                            // 调牌函数;

private:
	void		RandCard();

private:
	Card_Wakeng		m_oCard[CARD_COUNT_WAKENG];
	CardVec_Wakeng		m_pCard;
};


////////////////////////////////////////////////////////////////////////////////////////////
class CardHelp_Shanxi_Wakeng
{
public:
	CardHelp_Shanxi_Wakeng();
	~CardHelp_Shanxi_Wakeng();

	//提示;
	std::vector<std::vector<Lint>> HelpMe(Lint cardMode, Lint cardType, Lint cardCount, CardVec_Wakeng& handcard, std::vector<Lint>& m_curHelpUnusable, bool baojing, bool isUseBomb);


	void	CheckOne(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f,std::vector<std::vector<Lint>>& ret,bool baojing);
	void	CheckTwo(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f, std::vector<std::vector<Lint>>& ret);
	//void	CheckThreeToTwo(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f, std::vector<std::vector<Lint>>& ret);
	//void	CheckThreeFlyToTwo(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f,CardVec_Wakeng& handcard,Lint count, std::vector<std::vector<Lint>>& ret);
	void	CheckOneList(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f,Lint count, std::vector<std::vector<Lint>>& ret);
	void	CheckTwoList(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f,Lint count, std::vector<std::vector<Lint>>& ret);
	//void    CheckThreeToOne(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f, std::vector<std::vector<Lint>>& ret);
	//void    CheckFourToTwo(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f, std::vector<std::vector<Lint>>& ret);
	void    CheckThree(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f, std::vector<std::vector<Lint>>& ret);
	//void    CheckThreeList(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f,CardVec_Wakeng& handcard,Lint count, std::vector<std::vector<Lint>>& ret);
	//void    CheckThreeFlyToSingle(Lint cardType,CardVec_Wakeng& o,CardVec_Wakeng& t,CardVec_Wakeng& th, CardVec_Wakeng& f,CardVec_Wakeng& handcard,Lint count, std::vector<std::vector<Lint>>& ret);
};
#endif