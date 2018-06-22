// 算胡法新数据结构
// 2016年9月18日
// 李胜才 


#ifndef _NEWHANDCARDS_H_
#define _NEWHANDCARDS_H_

#include "LSingleton.h"
#include "GameDefine.h"
#include "Card.h"
#include "LMsg.h"
#include <bitset>


enum NEW_THINK_DOING
{
	NEW_THING_DOING_NULL		= 0,	    //什么都不做 PASS 
	NEW_THING_DOING_CHUPAI		= 1 << 0,	//简单出牌
	NEW_THING_DOING_XIAO_CHI	= 1 << 1,	//小吃  用小牌吃目标牌,      34吃5
	NEW_THING_DOING_ZHONG_CHI	= 1 << 2,	//中吃  用两头吃中建目标  	46吃5
	NEW_THING_DOING_DA_CHI		= 1 << 3,	//大吃  用大牌吃目标牌       67吃5
	NEW_THING_DOING_PENG		= 1 << 4,	//碰
	NEW_THING_DOING_MING_GANG   = 1 << 5,	//明杠,3+1 //不存在抢杠胡
	NEW_THING_TRYing_MING_GANG1 = 1 << 6,   //想碰转杠, 看看别人能抢胡否ing  2+1   +1?   杠牌分为明杠、暗杠、加杠。
	NEW_THING_TRYing_MING_GANG2 = 1 << 7,   //手里碰转杠, 第二个看看别人能抢胡否ing  2+1   +1?   杠牌分为明杠、暗杠、加杠。
	NEW_THING_TRYing_MING_GANG3	= 1 << 8,   //第三个 看看别人能抢胡否ing  2+1   +1?   杠牌分为明杠、暗杠、加杠。
	NEW_THING_DOING_AN_GANG1	= 1 << 9,	//暗杠
	NEW_THING_DOING_AN_GANG2	= 1 <<10, 	//暗杠
	NEW_THING_DOING_AN_GANG3	= 1 <<11, 	//暗杠
	NEW_THING_DOING_HU_ZI_MO	= 1 <<12, 	//自摸
	NEW_THING_DOING_HU_SHOU_PAO = 1 <<13,	//收炮
	NEW_THING_GETTING_NEW_CARD  = 1 <<14,   //摸牌11111111
};

#define    SAME_COUNT       4
#define    SHI_SAN_YAO		0XE07A0280A02    //十三幺必备牌型
#define    HAVE_MASK(U,M)   (((U)&(M))!=0)
#define    MARK_MASK(U,M)   ((U)=((U)|(M)))
#define    UNSET_MASK(U,M)  ((U)=(U)&(~M)) //~按位取反
#define    HAVE_CARD_(U,i)  (((U)   &   (((U64)1)<<i))) 

#define    ALL_CARDMD		 0XE07BFEFFBFE		//所有有效牌型
#define    GET_ZIPAIs(U)	((U)&0XE0780000000)	//获取所有的字牌
#define    GET_WAN(U)		((U)&0X3FE)			//获取所有的万牌
#define    GET_TONG(U)		((U)&0XFF800)		//获取所有的筒牌
#define    GET_TIAO(U)		((U)&0X3FE00000)	//获取所有的条牌
#define   GET_FEIZI(U)	    ((U)&0X3FEFFBFE)	//获取所有的非字牌
#define	   ALL258JIANG		0X12449124			//258将的时候全部可选将

#define    YITIAOLONG_WAN     0X3FE				//万牌的一条龙
#define    YITIAOLONG_TONG    0XFF800			//筒牌的一条龙
#define    YITIAOLONG_TIAO    0X3FE00000		//条牌的一条龙



#define FEIZI_ID_MIN     0  //[)
#define FEIZI_ID_MAX	27  //[)
#define ZI_INDEX_MIN	27  //3*9 [)
#define ZI_INDEX_MAX	34  //3*9 [)

#define CARD_TYPE_COUNT 34

//从0开始的数值映射到麻将牌 循环中用
//1万-9万1筒-9筒1条-9条东南西北中发白
//extern const int I2Cards[CARD_TYPE_COUNT];
extern const int HaveButCanNotShunZi[32];
extern const U64 u64ColorMask[3];


inline int GetCardID(const Card & card)		{ return  card.m_color * 10 + card.m_number - 10;}
inline int GetCardID(const Card* pCard)		{ return (pCard != NULL) ? GetCardID(*pCard) : 0; }
inline U64 GetCardID64(const Card & card)	{ return  (U64)1 << GetCardID(card);}
inline U64 GetCardID64(const Card* pCard)	{ return (pCard != NULL) ? GetCardID64(*pCard) : 0;}

#ifdef _DEBUG
const char * GetCardName(const Card* pCard);
const char * GetCardName(int id);
void U64ToCards(U64 u64, std::vector<Card>& out);
#endif // _DEBUG


//对比现有算法和本算法的函数
#ifdef _DEBUG
void CompareMethods(int  time = 100000);
void CompareMethodsEx(int  time = 100000);
void CompareMethodsEx1(int  time = 100000);
//void CompareMethodsEx2(int  time = 100000);
void TestNewHandCards_HuPai(const char* pchar);
void CompareTingPai(int  time = 100000);
void TestNewHandCard_TingPai(const char* pchar);
#endif // _DEBUG
 
// (nLose+4-nWin)%4*4+nWin;
// int AryWinLosModifyRet[16][4]     
// {
// 	{ 3,-1,-1,-1}, //0  第0家自摸
// 	{-1, 3,-1,-1}, //1  第1家自摸
// 	{-1,-1, 3,-1}, //2  第2家自摸
// 	{-1,-1,-1, 3}, //3  第3家自摸
// 	{ 1,-1, 0, 0}, //4  1家给0家点炮
//  { 0, 1,-1, 0}, //5  2家给1家点炮
//  { 0, 0, 1,-1}, //6  3家给2家点炮
//  {-1, 0, 0, 1}, //7  0家给3家点炮
//  { 1, 0,-1, 0}, //8  2家给0家点炮
//  { 0, 1, 0,-1}, //9  3家给1家点炮
//  {-1, 0, 1, 0}, //10 0家给2家点炮
//  { 0,-1, 0, 1}, //11 1家给3家点炮
//  { 1, 0, 0,-1}, //12 3家给0家点炮
//  {-1, 1, 0, 0}, //13 0家给1家点炮
//  { 0,-1, 1, 0}, //14 1家给2家点炮
//  { 0, 0,-1, 1}, //15 2家给3家点炮
// };

// int n_FanWinLoseGold[DEFAULT_DESK_USER_COUNT];//胡牌番数影响分数
// int n_GangWinLoseGold[DEFAULT_DESK_USER_COUNT];//杠影响分数
// int n_MaWinLoseGold[DEFAULT_DESK_USER_COUNT]; //奖码,翻醒,钓鱼影响分数 
// int n_OtherWinLoseGold[DEFAULT_DESK_USER_COUNT]; //其他,例如跟庄,等影响分数 
// int n_Eat3TimesGold[DEFAULT_DESK_USER_COUNT]; //吃三比影响分数 


// TestNewHandCards("11,12,13,14,14,15,15,16,16,17,17,17,18,19");
// CompareMethods(1000000);

/////////////////////////////////////////////////////////////////////////
// struct ChangeCard_TingU64
// {
// 	int outCardId;
// 	U64 u64TingCards; 
// 	ChangeCard_TingU64(int out, U64 u64T) { outCardId = out; u64TingCards = u64T; }
// };
// typedef std::vector<ChangeCard_TingU64> Ting_ResultU64;

//跟局类型返回胡怕字符串 调试用用的
const char * GetHuStrByID(int id);
extern const int Index2Cards[CARD_TYPE_COUNT];
enum NEWHU_TPYE
{
	NEW_HU_TYPE_NULL = 0, // 不能胡
	NEW_HU_TYPE_SHISANYAO = 1,  //十三幺
	NEW_HU_TYPE_7DUI = 2,      //七对
	NEW_HU_TYPE_PENGPENG_HU = 4, //碰碰胡
	NEW_HU_TYPE_DA7DUI_HU = 8 | NEW_HU_TYPE_7DUI, //大七对
	NEW_HU_TYPE_HAO7DUI_HU = 16 | NEW_HU_TYPE_7DUI | NEW_HU_TYPE_DA7DUI_HU, //豪华七对
	NEW_HU_TYPE_13LAN_FU = 32, //十三烂副烂(河池)
	NEW_HU_TYPE_13LAN_ZHENG = 64 | NEW_HU_TYPE_13LAN_FU, //十三烂正烂(河池)
	NEW_HU_TYPE_3HAOHUA7DUI = 128 | NEW_HU_TYPE_HAO7DUI_HU | NEW_HU_TYPE_7DUI | NEW_HU_TYPE_DA7DUI_HU, //三豪华七对



	NEW_HU_TYPE_PIHU = 1024, //屁胡, 先这么低吧. 这个可以配 天胡地胡杠上花等
};




//杠上花, 和杠后炮的区分 的时候 一个m_beforeType 出牌设置要在算胡之后设置..
//区分有点绕, 需要回溯前两步的操作 如果在加入 明杠的杠上花算点炮,就更操蛋了.
//用数组实现,简单的四步回溯
struct OperatorRecoder  //回溯上次出牌类型, 目前只能记忆四次, 需要的时候在扩容吧
{
	OperatorRecoder()
	{
		clear();
	}
	void clear()
	{
		for (int i = 0; i < 4; i++)
		{
			m_beforeType[i] = THINK_OPERATOR_NULL;
			m_OperatorPos[i] = 0;
		}
		m_curIndex = 0;
	}

	void PushOperator(int nOpt,int Pos =0)
	{
		m_curIndex = (m_curIndex + 1) & 3;
		m_beforeType[m_curIndex] = nOpt;	
		m_OperatorPos[m_curIndex]= Pos & 3; //错误数据兼容, 不要超过数组上线
	}
	int GetLast1Operator()const //获取上次玩家的动作
	{
		return m_beforeType[m_curIndex];
	}
	int GetLast2Operator()const //上次的上次
	{
		return m_beforeType[(m_curIndex + 3) & 3];
	}
	int GetLast3Operator()const//上次的上次的上次
	{
		return m_beforeType[(m_curIndex + 2) & 3];
	}
	
	int GetLast1Pos()const //获取上次玩家的位置
	{
		return m_OperatorPos[m_curIndex];
	}
	int GetLast2Pos()const //上次的上次
	{
		return m_OperatorPos[(m_curIndex + 3) & 3];
	}
	int GetLast3Pos()const//上次的上次的上次
	{
		return m_OperatorPos[(m_curIndex + 2) & 3];
	}

private:
	int m_beforeType[4];
	int m_OperatorPos[4];
	int m_curIndex;
};







struct  NewHandCard
{
private:

	// 测试用结构体
	struct NewHandCardTester   //胡牌算法临时数据就. 只有m_BitsCard[0]中包含的数据的颜色同色的才是有效的数据.
	{
		friend struct NewHandCard;
	private:
		U64 m_BitsCard[SAME_COUNT]; //所有牌的第i次出现,
	public:
		NewHandCardTester(U64 a0, U64 a1, U64 a2, U64 a3)
		{
			m_BitsCard[0] = a0;
			m_BitsCard[1] = a1;
			m_BitsCard[2] = a2;
			m_BitsCard[3] = a3;
		}
	private:
		bool TryAddV64_1(U64 n64CardValue)
		{
			for (int i = 0; i < SAME_COUNT; ++i)
			{
				if (!HAVE_MASK(m_BitsCard[i], n64CardValue))
				{
					MARK_MASK(m_BitsCard[i], n64CardValue);
					return true;
				}
			}
			return false;
		}
		bool TryAddV64_2(U64 n64CardValue)
		{
			for (int i = 0; i < SAME_COUNT-1; ++i)
			{
				if (!HAVE_MASK(m_BitsCard[i], n64CardValue))
				{
					MARK_MASK(m_BitsCard[i], n64CardValue);
					MARK_MASK(m_BitsCard[i+1], n64CardValue);
					return true;
				}
			}
			return false;
		}

		bool TryRemoveV64_1(U64 n64CardValue)
		{
			for (int i = SAME_COUNT - 1; i >= 0; --i)
			{
				if (HAVE_MASK(m_BitsCard[i], n64CardValue))
				{
					UNSET_MASK(m_BitsCard[i], n64CardValue);
					return true;
				}
			}
			return false;
		}

		bool TryRemoveV64_2(U64 n64CardValue)
		{
			for (int i = SAME_COUNT - 1; i >= 1; --i)
			{
				if (HAVE_MASK(m_BitsCard[i], n64CardValue))
				{
					UNSET_MASK(m_BitsCard[i], n64CardValue);
					UNSET_MASK(m_BitsCard[i - 1], n64CardValue);
					return true;
				}
			}
			return false;
		}
//	public:
		U64 GetCanNotShun()  //获取所有不能组成顺子的牌
		{
			U64 out = 0;
			U64 Cur = GET_FEIZI(m_BitsCard[0]);
			while (Cur)
			{
				U64 tem = (Cur ^ (Cur - 1))&Cur; //只有最低的1
				if (HaveButCanNotShunZi[((m_BitsCard[0] & ((tem * 31) >> 2)) << 2) / tem])
				{
					out |= tem;
				}
				Cur = Cur&(Cur - 1);
			}
			return out;
		}

		bool CheckNoShun1(U64 u64Cards)//无顺的单牌是不是超过1张
		{
			U64 Cur = GET_FEIZI(u64Cards);
			while (Cur)
			{
				U64 tem = (Cur ^ (Cur - 1))&Cur; //只有最低的1
				if (HaveButCanNotShunZi[((m_BitsCard[0] & ((tem * 31) >> 2)) << 2) / tem])
				{
					return true;
				}
				Cur = Cur&(Cur - 1);
			}
			return false;
		}


		bool TryRemoveBy333_ALL_Slow()//执行这个后 数据失效了, 不能继续使用该对象了...   中发白 不会会认为是顺子
		{
			while (m_BitsCard[0])
			{
				U64 CurBit = (m_BitsCard[0] ^ (m_BitsCard[0] - 1))&m_BitsCard[0]; // 只有m_BitsCard[0]最低的1
				if (m_BitsCard[3] & CurBit)//四张
				{
					if (CurBit > 0X40000000) return false;				
					if (!TryRemoveV64_1(CurBit << 1) || !TryRemoveV64_1(CurBit << 2))
					{
						return false;
					}
					UNSET_MASK(m_BitsCard[0], CurBit);//	m_BitsCard[0]-= CurBit; 
					continue;
				}
				else if (m_BitsCard[2] & CurBit) //三章			
				{
					UNSET_MASK(m_BitsCard[0], CurBit);
					continue;
				}
				else if (m_BitsCard[1] & CurBit) //两张
				{
					if (CurBit > 0X40000000) return false;
					if (!TryRemoveV64_2(CurBit << 1) || !TryRemoveV64_2(CurBit << 2))
					{
						return false;
					}
					UNSET_MASK(m_BitsCard[0], CurBit);
					continue;
				}
				else  //单张
				{
					if (CurBit > 0X40000000) return false;
					if (!TryRemoveV64_1(CurBit << 1) || !TryRemoveV64_1(CurBit << 2))
					{
						return false;
					}
					UNSET_MASK(m_BitsCard[0], CurBit);
					continue;
				}
			}
			return true;
		}


		bool TryRemoveBy333()//执行这个后 数据失效了, 不能继续使用该对象了... 这个确定不会有字牌单三了. 中发白会认为是顺子
		{
			while (m_BitsCard[0])
			{
				U64 CurBit = (m_BitsCard[0] ^ (m_BitsCard[0] - 1))&m_BitsCard[0]; // 只有m_BitsCard[0]最低的1
				if (m_BitsCard[3] & CurBit)//四张
				{
					if (!TryRemoveV64_1(CurBit << 1) || !TryRemoveV64_1(CurBit << 2))
					{
						return false;
					}
					UNSET_MASK(m_BitsCard[0], CurBit);//	m_BitsCard[0]-= CurBit; 
					continue;
				}
				else if (m_BitsCard[2] & CurBit) //三章			
				{
					UNSET_MASK(m_BitsCard[0], CurBit);
					continue;
				}
				else if (m_BitsCard[1] & CurBit) //两张
				{
					if (!TryRemoveV64_2(CurBit << 1) || !TryRemoveV64_2(CurBit << 2))
					{
						return false;
					}
					UNSET_MASK(m_BitsCard[0], CurBit);
					continue;
				}
				else  //单张
				{
					if (!TryRemoveV64_1(CurBit << 1) || !TryRemoveV64_1(CurBit << 2))
					{
						return false;
					}
					UNSET_MASK(m_BitsCard[0], CurBit);
					continue;
				}
			}
			return true;
		}

	private:
		NewHandCardTester();
//		NewHandCardTester(const NewHandCardTester&);
	};

private:
	U64 m_BitsCard[SAME_COUNT]; //所有牌的第i次出现,
	int m_AllCounts;
	int m_TypeCounts[8];
	U64 m_DanZhang; //所有的单牌 只有执行完UpdateOneTwoThree 才可靠
	U64 m_DuiZi; //所有的对牌 只有执行完UpdateOneTwoThree 才可靠
	U64 m_SanZhang; //所有的三张 只有执行完UpdateOneTwoThree 才可靠  四个的直接在m_BitsCard[3]里面

	int m_nSameCount[5][9];
public:
	U64 GetBits_0() const { return m_BitsCard[0]; }
	U64 GetBits_1() const { return m_BitsCard[1]; }
	U64 GetBits_2() const { return m_BitsCard[2]; }
	U64 GetBits_3() const { return m_BitsCard[3]; }
	U64 GetDuiZi()  const { return m_DuiZi; }

	bool Can13Lan()const; // 11.十三烂：手中十四张牌中，序数牌间隔大于2，且字牌没有重复所组成的牌型。东南西北中发白，147、158、159、258、259、369、这种搭配选14张就可胡牌。
	bool Can7Xing13Lan()const;//十三烂：加5番，东南西北中发白都凑齐的话算10番。

	bool Can13Lan_zheng()const; //十三正烂(牌组中万桶条皆是147或258或369（万筒条要么都是147要么都是258，不可以有258+147形式的, 可以147+14+47+17+1+4+7）加至少5张不同字牌组成)
	bool Can13Lan_fu()const; //十三副烂(牌组中万桶条可是147或258或369加至少5张不同字牌组成,可以147+14+47+17+1+4+7, 万筒条可以是既有147又有258)

	bool IsEmpty()const { return m_AllCounts == 0; }

 //兼容原来的代码 单独做个处理 开始
	int size() const { return m_AllCounts; }
	void GetColor_number(int maxLen, CardValue* pCards)const;
	void Serializ2Vec(std::vector<CardValue>& vec)const;
	void Serializ2Vec(EntiryCardVector& vec)const;
	void GetFirstCard(CardValue& out)const; //机器人出牌规则 

	/*
	* 机器人出牌规则,优先打无法组成对子 顺子的牌，优先从风中发白打 
	*/
	void GetSmartOutCard(CardValue& out) const;

	/*
	* 获得尾牌
	*/
	void GetTail(CardValue& card);

	int  Serializ2Vec4YBSuanFa(Lint(&cards)[YINGBIN_SUANFA_VECLEN], int laiziID)const;
	//兼容原来的代码 单独做个处理 结束

	void ReSet()
	{
		m_AllCounts = 0;
		m_DanZhang = 0;
		m_DuiZi = 0;
		m_SanZhang = 0;
		for (int i = 0; i < SAME_COUNT; ++i)
		{
			m_BitsCard[i] = 0;
			m_TypeCounts[i] = 0;
			m_TypeCounts[4+i] = 0;
		}	
		memset(m_nSameCount, 0, sizeof(m_nSameCount));
	}
	NewHandCard() { ReSet(); }



	//摸牌,判胡 临时插入
	bool AddCard(int color,int number)
	{
		if (__AddCard(color * 10 + number - 10))
		{
			++m_AllCounts;
			++m_TypeCounts[(color - 1) & 7];
			++m_nSameCount[color - 1][number - 1];
			return true;
		}
		return false;		 
	}	
	bool AddCard(const Card* pCard)        {  return pCard && AddCard(pCard->m_color  ,pCard->m_number); }
	bool AddCard(const Card& card)         {  return AddCard(card.m_color, card.m_number); }
	

	//出牌,打出牌
	bool RemoveCard(int color, int number,int nCount = 1)
	{
		if (__RemoveCard(color * 10 + number - 10, nCount))
		{
			 m_AllCounts -=nCount;
			 m_TypeCounts[(color - 1) & 7]-=nCount;
			 m_nSameCount[color - 1][number - 1] -= nCount;
			return true;
		}
		return false;
	}
	bool RemoveCard(const CardValue * pCardValue,int nCount =1)	{ return pCardValue && RemoveCard(pCardValue->m_color, pCardValue->m_number,nCount); }
	bool RemoveCard(const CardValue& cardValue, int nCount = 1)	{ return   RemoveCard(cardValue.m_color, cardValue.m_number, nCount); }
	bool RemoveCard(const Card* pCard, int nCount = 1)			{  return pCard && RemoveCard(pCard->m_color, pCard->m_number, nCount);}
	bool RemoveCard(const Card& card, int nCount = 1)			{  return RemoveCard(card.m_color, card.m_number, nCount); }


	//判断有没有牌相关
	bool IsHaveCard(int id)const				{ return __IsHaveCard64((U64)1 << id); }
	bool IsHaveCard(int color, int number)const { return IsHaveCard(color * 10 + number - 10); }
	bool IsHaveCard(const Card* pCard)const		{ return pCard && IsHaveCard(pCard->m_color, pCard->m_number); }
	bool IsHaveCard(const Card& card)const		{ return IsHaveCard(card.m_color, card.m_number); }
	bool IsHaveCard(const std::vector<CardValue>& vCard);

	//判断牌的数量
	int CountCard(int id)const					{ return __CountCard64((U64)1 << id); }
	int CountCard(int color, int number)const	{ return CountCard(color * 10 + number - 10); }
	int CountCard(const Card* pCard)const		{ return pCard && CountCard(pCard->m_color, pCard->m_number); }
	int CountCard(const Card& card)const		{ return CountCard(card.m_color, card.m_number); }



	//吃牌相关
	int  CanEatCard(int id)const;		       // { return __CanEatCard((U64)1 << id); }
	int  CanEatCard(int color, int number)const		{ return CanEatCard(color * 10 + number - 10); }
	int  CanEatCard(const Card* pCard)const			{ return pCard && CanEatCard(pCard->m_color, pCard->m_number); }
	int  CanEatCard(const Card& card)const			{ return CanEatCard(card.m_color, card.m_number); }
	
	//碰牌相关
	bool CanPeng(int id)const						{ return __CanPeng64((U64)1 << id); }
	bool CanPeng(int color, int number)const		{ return CanPeng(color * 10 + number - 10); }
	bool CanPeng(const Card* pCard)const			{ return pCard && CanPeng(pCard->m_color, pCard->m_number);}
	bool CanPeng(const Card& card)const				{ return CanPeng(card.m_color, card.m_number); }

	//杠牌相关
	bool CanGang(int id)const					{ return __CanGang64((U64)1 << id); }
	bool CanGang(int color, int number)const	{ return CanGang(color * 10 + number - 10); }
	bool CanGang(const Card* pCard)const		{ return pCard && CanGang(pCard->m_color, pCard->m_number);}
	bool CanGang(const Card& card)const			{ return CanGang(card.m_color, card.m_number); }

	//暗杠牌相关
	bool GetCanAnGangs(std::vector<Card>& outcard); 
	void UpdateOneTwoThree()
	{
		m_DanZhang = m_BitsCard[0] ^ m_BitsCard[1]; //所有的单牌
		m_DuiZi = m_BitsCard[1] ^ m_BitsCard[2];    //所有的对子
		m_SanZhang = m_BitsCard[2] ^ m_BitsCard[3]; //所有的刻子   三张
	}

public: 
	bool TryRemoveYiTiaoLongWithGhost(int nColor,const Card& ghost,bool bHavePlainGhost=false)
	{
		for (int i =1; i <=9; ++i)
		{
			if (!RemoveCard(nColor,i))
			{
				if (bHavePlainGhost && CountCard(ghost)<=1)
				{
					return false;
				}
				if (!RemoveCard(ghost))
				{
					return false;
				}
			}
		}
		return true;
	}

	bool TryRemoveYiTiaoLong_BaiSe(int nColor)
	{
		for (int i = 1; i <= 9; ++i)
		{
			if (!RemoveCard(nColor, i))
			{
				return false;
			}
		}
		return true;
	}


private:
	bool __IsHaveCard64(U64 n64CardValue) const{ return (m_BitsCard[0] & n64CardValue) != 0; }
	int __CountCard64(U64 n64CardValue)const 
	{
		for (int i = SAME_COUNT - 1; i >= 0; --i)
		{
			if (HAVE_MASK(m_BitsCard[i], n64CardValue))
			{
				return i + 1;				
			}
		}
		return 0;
	}
	

	bool __AddCard(int id){		return __AddCard64((U64)1 << id);	}
	bool __AddCard64(U64 n64CardValue)//
	{
		if ((n64CardValue & ALL_CARDMD) == 0)
		{
			return false;
		}
		for (int i = 0; i < SAME_COUNT; ++i)
		{
			if (!HAVE_MASK(m_BitsCard[i], n64CardValue))
			{
				MARK_MASK(m_BitsCard[i], n64CardValue);
				return true;
			}
		}
		return false;
	}
	bool __RemoveCard(int id,int nCount = 1){		return __RemoveCard64((U64)1 << id,nCount);	}
	bool __RemoveCard64(U64 n64CardValue, int nCount = 1)
	{
		if ((n64CardValue& ALL_CARDMD) == 0)
		{
			return false;
		}
		for (int i = SAME_COUNT - 1; i >= 0; --i)
		{
			if (HAVE_MASK(m_BitsCard[i], n64CardValue))
			{
				UNSET_MASK(m_BitsCard[i], n64CardValue);
				if (--nCount==0)
				{
					return true;
				}				
			}
		}
		return false;
	}
	
	
	int __CanEatCard64(U64 n64CardValue)const
	{
		if ((n64CardValue& ALL_CARDMD) == 0)
		{
			return 0;
		}
		int result = 0;
		U64 n64CardValue1 = (n64CardValue << 2);
		U64 n64CardValue2 = (n64CardValue << 1);
		U64 n64CardValue3 = (n64CardValue >> 1);
		U64 n64CardValue4 = (n64CardValue << 2);
		if ((m_BitsCard[0] & (n64CardValue1 | n64CardValue2)) == (n64CardValue1 | n64CardValue2))
		{
			result = NEW_THING_DOING_XIAO_CHI;
		}
		if ((m_BitsCard[0] & (n64CardValue2 | n64CardValue3)) == (n64CardValue2 | n64CardValue3))
		{
			result |= NEW_THING_DOING_ZHONG_CHI;
		}
		if ((m_BitsCard[0] & (n64CardValue3 | n64CardValue4)) == (n64CardValue3 | n64CardValue4))
		{
			result |= NEW_THING_DOING_DA_CHI;
		}
		return 1;
	}
		
	
	bool __CanPeng64(U64 n64CardValue)const
	{
		if ((n64CardValue& ALL_CARDMD) == 0)
		{
			return false;
		}		  
		return (m_BitsCard[1] & (n64CardValue)) != 0;
	}
	
	bool __CanGang64(U64 n64CardValue)const
	{
		if ((n64CardValue& ALL_CARDMD) == 0)
		{
			return false;
		}
		return (m_BitsCard[2] & (n64CardValue)) != 0;
	}


	bool CheckNoShun1(U64 u64Cards)//无顺的单牌是不是超过1张
	{
		U64 Cur = GET_FEIZI(u64Cards);
		while (Cur)
		{
			U64 Cur_1 = Cur - 1;
			U64 tem = Cur&~Cur_1;//(Cur ^ (Cur - 1))&Cur; //只有最低的1   Cur&~(Cur-1)也可以获得最低位1
			if (HaveButCanNotShunZi[((m_BitsCard[0] & ((tem * 31) >> 2)) << 2) / tem])
			{
				return true;
			}
			Cur = Cur&Cur_1;
		}
		return false;
	}


public:
	bool __IsQingWan() const   { return (m_BitsCard[0] & 0XFFFFFFFFFFFFFC01) == 0; }
	bool __IsQingTiao() const  { return (m_BitsCard[0] & 0XFFFFFFFFC01FFFFF) == 0; }
	bool __IsQingTong()const   { return (m_BitsCard[0] & 0XFFFFFFFFFFF007FF) == 0; }
	bool __IsQingZi()  const   { return (m_BitsCard[0] & 0XFFFFF1F87FFFFFFF) == 0; }

	//单独调用,需要先UpdateOneTwoThree
	bool Can7Dui()const  {return (m_AllCounts == 14) && (m_DanZhang == 0) && (m_SanZhang == 0);}	

	//单独调用,需要先UpdateOneTwoThree
	bool CanPengPenghu() const
	{
		if (m_BitsCard[3])
		{
			return false;
		}
		
		if (m_DanZhang)
		{
			return false;
		}

		if ((m_DuiZi & (m_DuiZi-1))==0)
		{
			return true;
		}		
		return false;
	}


	bool Can13YaoByGhost(U64 u64GhostCard, bool bHave1PlainGhost= false)const; //通过u64GhostCard做鬼牌能组成13妖的牌型:
	bool Can7DuiByGhost(U64 u64GhostCard, bool bHave1PlainGhost = false)const;//通过u64GhostCard做鬼牌能组成7Dui的牌型:
	int	 Get7DuiByGhost(U64 u64GhostCard, bool bHave1PlainGhost, bool bSanHaoHuaQiDui = false) const;//获取七对的情况, 可以区分大七对,豪华七对
	bool CanPengPengByGhost(U64 u64GhostCard, bool bHave1PlainGhost = false)const;//通过u64GhostCard做鬼牌能组成7Dui的牌型:

	bool CheckEatGhostEnough(U64 u64GhostCard, bool bHave1PlainGhost = false)const;//检查特定(非全部)吃鬼牌需要的鬼牌是不是足够~!!

	int  CanHu(bool bMust258Jiang = false, bool b13LanZheng = false, bool b13LanFu = false, bool bShiSanYao = true, bool bQiDui = true, bool bSanHaoHuaQiDui = false)
	{   
		UpdateOneTwoThree();
		if (bShiSanYao && m_BitsCard[0] == SHI_SAN_YAO)
		{ 
			return NEW_HU_TYPE_SHISANYAO;
		}
		if (bQiDui && Can7Dui()) 
		{ 
			if (m_BitsCard[3])
			{
				if (bSanHaoHuaQiDui)
				{
					std::bitset<64> bs(m_BitsCard[3]);
					if (bs.count() == 3)
					{
						return NEW_HU_TYPE_3HAOHUA7DUI;
					}
				}

				if (m_BitsCard[3] & (m_BitsCard[3]-1))
				{
					return NEW_HU_TYPE_HAO7DUI_HU;
				}
				return NEW_HU_TYPE_DA7DUI_HU;
			}
			return NEW_HU_TYPE_7DUI;
		}
		if (CanPengPenghu())//这个需要跟外边的 eatvector.empyt() 配合验证, 只是手牌可以碰碰胡而已
		{
			return NEW_HU_TYPE_PENGPENG_HU;
		}

		//十三烂,先正烂,后副烂
		if (b13LanZheng && Can13Lan_zheng())
		{
			return NEW_HU_TYPE_13LAN_ZHENG;
		}
		if (b13LanFu && Can13Lan_fu())
		{	
			return NEW_HU_TYPE_13LAN_FU;
		}
		
		
		if (GET_ZIPAIs(m_DanZhang))
		{
			return NEW_HU_TYPE_NULL;  // 有单张的字牌,肯定不能胡
		}
		if (m_BitsCard[1] == 0)
		{
			return NEW_HU_TYPE_NULL;  //连对子都么有....
		}
		
		U64 duiZipai = GET_ZIPAIs(m_DuiZi);//字牌的对		
		if (duiZipai && (duiZipai&(duiZipai - 1)))
		{
			return NEW_HU_TYPE_NULL; //有两对字牌, 肯定不能胡
		}
		bool bMustFengJiang = (duiZipai != 0); //如果有成对的字牌, 要想胡, 必需要字牌做将牌, 不用轮询做将了
		if (GET_ZIPAIs(m_BitsCard[3])) 
		{ 
			return NEW_HU_TYPE_NULL; //如果有四张一样的字牌, 没法胡
		}
	
		U64 NeedShun = (m_DanZhang | m_BitsCard[3]); //单张的和四张的 需要用顺子吃掉一个.

		if (bMustFengJiang)  //必需风做为将牌
		{
			if (bMust258Jiang)
			{
				return NEW_HU_TYPE_NULL;
			}
			if (CheckNoShun1(NeedShun))
			{
				return NEW_HU_TYPE_NULL; //单张的和四张的 但是没法顺子. 
			}	
			if ((m_TypeCounts[0] % 3) || (m_TypeCounts[1] % 3) || (m_TypeCounts[2] % 3))
			{
				return NEW_HU_TYPE_NULL; //有花色不能组成 牌型
			}
			NewHandCardTester HandCardtester(GET_FEIZI(m_BitsCard[0]),m_BitsCard[1],m_BitsCard[2],m_BitsCard[3]);
			return HandCardtester.TryRemoveBy333()? NEW_HU_TYPE_PIHU : NEW_HU_TYPE_NULL;
		}
		
		//以下肯定不是字做将, 
		//以下字牌肯定是333的组合了.
		U64 uMustJiangColor = 0;
		for(int i =0; i < 3;++i)
		{
			if (m_TypeCounts[i]==0)
			{
				continue;
			}
			switch (m_TypeCounts[i] % 3)
			{
			case 0:
			{
				NewHandCardTester HandCardtester(m_BitsCard[0] & u64ColorMask[i], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
				if (!HandCardtester.TryRemoveBy333())
				{
					return NEW_HU_TYPE_NULL;
				}
				break;
			}				
			case 1:
			{
				return NEW_HU_TYPE_NULL;
				break;
			}
			case 2:
			{
				uMustJiangColor = u64ColorMask[i];
				break;
			}
			default:
				break;
			} 
		}
		//到这里说明 nMustJiangValue所在颜色的牌之外的牌都可以了. 
		//以下只要拍单nMustJiangValue所在颜色的牌是不是22 +x333 就行了.
		if ((m_BitsCard[1] & uMustJiangColor)==0)
		{
			return NEW_HU_TYPE_NULL; //没有将
		}
		
		NewHandCardTester HandCardtester(m_BitsCard[0] & uMustJiangColor, m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
		NeedShun &= uMustJiangColor; //单张的和四张的 需要用顺子吃掉一个
		if (HandCardtester.CheckNoShun1(NeedShun))
		{
			return NEW_HU_TYPE_NULL;//需要组成顺子, 但是不能成顺子
		}


		U64 CanNotShun = HandCardtester.GetCanNotShun();
// 		if (NeedShun & CanNotShun)
// 		{
// 			return false;   //这个 到这里必然不满足了
// 		}
		U64 FakeZiPai = CanNotShun&m_DuiZi;  //不能顺的对, 跟风一样 假字牌
		if (FakeZiPai) //不能顺的牌 沦为字牌
		{
			if (FakeZiPai&(FakeZiPai - 1)) //字对子超过2对字牌了. 
			{
				return NEW_HU_TYPE_NULL;
			}		 
			HandCardtester.TryRemoveV64_2(FakeZiPai);	 
			return HandCardtester.TryRemoveBy333() ? NEW_HU_TYPE_PIHU : NEW_HU_TYPE_NULL;
		}
		//没有不能顺的对了
		U64 CanJiang = m_BitsCard[1] & uMustJiangColor;
		if (bMust258Jiang)
		{
			CanJiang &= ALL258JIANG;
		}
		while (CanJiang)
		{
			U64 tem = CanJiang ^ (CanJiang - 1)&CanJiang;//只有最低位的1.其他的都是0 
			NewHandCardTester HandCardtester1(m_BitsCard[0] & uMustJiangColor, m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
			HandCardtester1.TryRemoveV64_2(tem);	
			if (HandCardtester1.TryRemoveBy333())
			{
				return NEW_HU_TYPE_PIHU;  //初13幺和七对之外的都在这里了...
			} 
			CanJiang = CanJiang&(CanJiang - 1);
		}
		return NEW_HU_TYPE_NULL;
	}


	U64 GetTingCards_Right_Slow(bool bDaiShiSanYao = true, bool bDaiQiDui = true)const;
	U64 GetTingCards()const;
	bool GetTingCardsByOutCard(Ting_Result&)const;
};


#define SIZE_ARRAY(a) (sizeof(a)/sizeof(a[0]))

#define _ADD_QUOTES(LEN) #LEN
#define ADD_QUOTES(LEN) _ADD_QUOTES(LEN)


#define PRINT_FROMATE_D_1()  %s : %d	
#define PRINT_FROMATE_D_2()  PRINT_FROMATE_D_1() %d
#define PRINT_FROMATE_D_3()  PRINT_FROMATE_D_2() %d
#define PRINT_FROMATE_D_4()  PRINT_FROMATE_D_3() %d
#define PRINT_FROMATE_D_5()  PRINT_FROMATE_D_4() %d
#define PRINT_FROMATE_D_6()	 PRINT_FROMATE_D_5() %d
#define PRINT_FROMATE_D_7()	 PRINT_FROMATE_D_6() %d
#define PRINT_FROMATE_D_8()	 PRINT_FROMATE_D_7() %d
#define PRINT_FROMATE_D_9()	 PRINT_FROMATE_D_8() %d
#define PRINT_FROMATE_D_10() PRINT_FROMATE_D_9() %d
#define PRINT_FROMATE_D_11() PRINT_FROMATE_D_10() %d
#define PRINT_FROMATE_D_12() PRINT_FROMATE_D_11() %d
#define PRINT_FROMATE_D_13() PRINT_FROMATE_D_12() %d
#define PRINT_FROMATE_D_14() PRINT_FROMATE_D_13() %d
#define PRINT_FROMATE_D_15() PRINT_FROMATE_D_14() %d
#define PRINT_FROMATE_D_16() PRINT_FROMATE_D_15() %d
#define PRINT_FROMATE_D_17() PRINT_FROMATE_D_16() %d
#define PRINT_FROMATE_D_18() PRINT_FROMATE_D_17() %d
#define PRINT_FROMATE_D_19() PRINT_FROMATE_D_18() %d
#define PRINT_FROMATE_D_20() PRINT_FROMATE_D_19() %d
#define PRINT_FROMATE_D_21() PRINT_FROMATE_D_20() %d
#define PRINT_FROMATE_D_22() PRINT_FROMATE_D_21() %d
#define PRINT_FROMATE_D_23() PRINT_FROMATE_D_22() %d
#define PRINT_FROMATE_D_24() PRINT_FROMATE_D_23() %d
#define PRINT_FROMATE_D_25() PRINT_FROMATE_D_24() %d
#define PRINT_FROMATE_D_26() PRINT_FROMATE_D_25() %d
#define PRINT_FROMATE_D_27() PRINT_FROMATE_D_26() %d
#define PRINT_FROMATE_D_28() PRINT_FROMATE_D_27() %d
#define PRINT_FROMATE_D_29() PRINT_FROMATE_D_28() %d
#define PRINT_FROMATE_D_30() PRINT_FROMATE_D_29() %d


#define PRINT_VALUE_1(X)   	X##[0]	 
#define PRINT_VALUE_2(X)   	PRINT_VALUE_1(X), X##[1]
#define PRINT_VALUE_3(X)   	PRINT_VALUE_2(X), X##[2]
#define PRINT_VALUE_4(X)   	PRINT_VALUE_3(X), X##[3]
#define PRINT_VALUE_5(X)   	PRINT_VALUE_4(X), X##[4]
#define PRINT_VALUE_6(X)   	PRINT_VALUE_5(X), X##[5]
#define PRINT_VALUE_7(X)   	PRINT_VALUE_6(X), X##[6]
#define PRINT_VALUE_8(X)   	PRINT_VALUE_7(X), X##[7]
#define PRINT_VALUE_9(X)   	PRINT_VALUE_8(X), X##[8]
#define PRINT_VALUE_10(X)  	PRINT_VALUE_9(X), X##[9]
#define PRINT_VALUE_11(X)  	PRINT_VALUE_10(X), X##[10]
#define PRINT_VALUE_12(X)  	PRINT_VALUE_11(X), X##[11]
#define PRINT_VALUE_13(X)  	PRINT_VALUE_12(X), X##[12]
#define PRINT_VALUE_14(X)  	PRINT_VALUE_13(X), X##[13]
#define PRINT_VALUE_15(X)  	PRINT_VALUE_14(X), X##[14]
#define PRINT_VALUE_16(X)  	PRINT_VALUE_15(X), X##[15]
#define PRINT_VALUE_17(X)  	PRINT_VALUE_16(X), X##[16]
#define PRINT_VALUE_18(X)  	PRINT_VALUE_17(X), X##[17]
#define PRINT_VALUE_19(X)  	PRINT_VALUE_18(X), X##[18]
#define PRINT_VALUE_20(X)  	PRINT_VALUE_19(X), X##[19]
#define PRINT_VALUE_21(X)  	PRINT_VALUE_20(X), X##[20]
#define PRINT_VALUE_22(X)  	PRINT_VALUE_21(X), X##[21]
#define PRINT_VALUE_23(X)  	PRINT_VALUE_22(X), X##[22]
#define PRINT_VALUE_24(X)  	PRINT_VALUE_23(X), X##[23]
#define PRINT_VALUE_25(X)  	PRINT_VALUE_24(X), X##[24]
#define PRINT_VALUE_26(X)  	PRINT_VALUE_25(X), X##[25]
#define PRINT_VALUE_27(X)  	PRINT_VALUE_26(X), X##[26]
#define PRINT_VALUE_28(X)  	PRINT_VALUE_27(X), X##[27]
#define PRINT_VALUE_29(X)  	PRINT_VALUE_28(X), X##[28]
#define PRINT_VALUE_30(X)  	PRINT_VALUE_29(X), X##[29]




#define  LSC_PRINT_VEC_1(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_1()),#X,PRINT_VALUE_1(X))
#define  LSC_PRINT_VEC_2(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_2()),#X,PRINT_VALUE_2(X))
#define  LSC_PRINT_VEC_3(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_3()),#X,PRINT_VALUE_3(X))
#define  LSC_PRINT_VEC_4(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_4()),#X,PRINT_VALUE_4(X))
#define  LSC_PRINT_VEC_5(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_5()),#X,PRINT_VALUE_5(X))
#define  LSC_PRINT_VEC_6(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_6()),#X,PRINT_VALUE_6(X))
#define  LSC_PRINT_VEC_7(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_7()),#X,PRINT_VALUE_7(X))
#define  LSC_PRINT_VEC_8(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_8()),#X,PRINT_VALUE_8(X))
#define  LSC_PRINT_VEC_9(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_9()),#X,PRINT_VALUE_9(X))
#define  LSC_PRINT_VEC_10(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_10()),#X,PRINT_VALUE_10(X))
#define  LSC_PRINT_VEC_11(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_11()),#X,PRINT_VALUE_11(X))
#define  LSC_PRINT_VEC_12(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_12()),#X,PRINT_VALUE_12(X))
#define  LSC_PRINT_VEC_13(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_13()),#X,PRINT_VALUE_13(X))
#define  LSC_PRINT_VEC_14(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_14()),#X,PRINT_VALUE_14(X))
#define  LSC_PRINT_VEC_15(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_15()),#X,PRINT_VALUE_15(X))
#define  LSC_PRINT_VEC_16(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_16()),#X,PRINT_VALUE_16(X))
#define  LSC_PRINT_VEC_17(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_17()),#X,PRINT_VALUE_17(X))
#define  LSC_PRINT_VEC_18(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_18()),#X,PRINT_VALUE_18(X))
#define  LSC_PRINT_VEC_19(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_19()),#X,PRINT_VALUE_19(X))
#define  LSC_PRINT_VEC_20(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_20()),#X,PRINT_VALUE_20(X))
#define  LSC_PRINT_VEC_21(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_21()),#X,PRINT_VALUE_21(X))
#define  LSC_PRINT_VEC_22(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_22()),#X,PRINT_VALUE_22(X))
#define  LSC_PRINT_VEC_23(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_23()),#X,PRINT_VALUE_23(X))
#define  LSC_PRINT_VEC_24(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_24()),#X,PRINT_VALUE_24(X))
#define  LSC_PRINT_VEC_25(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_25()),#X,PRINT_VALUE_25(X))
#define  LSC_PRINT_VEC_26(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_26()),#X,PRINT_VALUE_26(X))
#define  LSC_PRINT_VEC_27(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_27()),#X,PRINT_VALUE_27(X))
#define  LSC_PRINT_VEC_28(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_28()),#X,PRINT_VALUE_28(X))
#define  LSC_PRINT_VEC_29(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_29()),#X,PRINT_VALUE_29(X))
#define  LSC_PRINT_VEC_30(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_30()),#X,PRINT_VALUE_30(X))




#ifdef _DEBUG
#define  Debug_Log_Vec(X) \
switch (X.size())\
{					  	  	 \
	case 1:		LSC_PRINT_VEC_1(X); break; \
	case 2:		LSC_PRINT_VEC_2(X); break; \
	case 3:		LSC_PRINT_VEC_3(X); break; \
	case 4:		LSC_PRINT_VEC_4(X); break; \
	case 5:		LSC_PRINT_VEC_5(X); break; \
	case 6:		LSC_PRINT_VEC_6(X); break; \
	case 7:		LSC_PRINT_VEC_7(X); break; \
	case 8:		LSC_PRINT_VEC_8(X); break; \
	case 9:		LSC_PRINT_VEC_9(X); break; \
	case 10:	LSC_PRINT_VEC_10(X); break; \
	case 11: 	LSC_PRINT_VEC_11(X); break; \
	case 12: 	LSC_PRINT_VEC_12(X); break; \
	case 13: 	LSC_PRINT_VEC_13(X); break; \
	case 14:  	LSC_PRINT_VEC_14(X); break; \
	case 15:  	LSC_PRINT_VEC_15(X); break; \
	case 16:  	LSC_PRINT_VEC_16(X); break; \
	case 17:  	LSC_PRINT_VEC_17(X); break; \
	case 18:  	LSC_PRINT_VEC_18(X); break; \
	case 19:  	LSC_PRINT_VEC_19(X); break; \
	case 20:   	LSC_PRINT_VEC_20(X); break; \
	case 21:   	LSC_PRINT_VEC_21(X); break; \
	case 22:   	LSC_PRINT_VEC_22(X); break; \
	case 23:   	LSC_PRINT_VEC_23(X); break; \
	case 24:   	LSC_PRINT_VEC_24(X); break; \
	case 25:   	LSC_PRINT_VEC_25(X); break; \
	case 26:   	LSC_PRINT_VEC_26(X); break; \
	case 27:   	LSC_PRINT_VEC_27(X); break; \
	case 28:   	LSC_PRINT_VEC_28(X); break; \
	case 29:   	LSC_PRINT_VEC_29(X); break; \
	case 30:   	LSC_PRINT_VEC_30(X); break; \
		default:  			break;	 \
}

#else 	 
#define  Debug_Log_Vec(x) 
#endif // _DEBUG

#define PRINT_CARD_1(X)   	X##[0].m_color*10+X##[0].m_number
#define PRINT_CARD_2(X)   	PRINT_CARD_1(X), X##[1].m_color*10+X##[1].m_number
#define PRINT_CARD_3(X)   	PRINT_CARD_2(X), X##[2].m_color*10+X##[2].m_number
#define PRINT_CARD_4(X)   	PRINT_CARD_3(X), X##[3].m_color*10+X##[3].m_number
#define PRINT_CARD_5(X)   	PRINT_CARD_4(X), X##[4].m_color*10+X##[4].m_number
#define PRINT_CARD_6(X)   	PRINT_CARD_5(X), X##[5].m_color*10+X##[5].m_number
#define PRINT_CARD_7(X)   	PRINT_CARD_6(X), X##[6].m_color*10+X##[6].m_number
#define PRINT_CARD_8(X)   	PRINT_CARD_7(X), X##[7].m_color*10+X##[7].m_number
#define PRINT_CARD_9(X)   	PRINT_CARD_8(X), X##[8].m_color*10+X##[8].m_number
#define PRINT_CARD_10(X)  	PRINT_CARD_9(X), X##[9].m_color*10+X##[9].m_number
#define PRINT_CARD_11(X)  	PRINT_CARD_10(X), X##[10].m_color*10+X##[10].m_number
#define PRINT_CARD_12(X)  	PRINT_CARD_11(X), X##[11].m_color*10+X##[11].m_number
#define PRINT_CARD_13(X)  	PRINT_CARD_12(X), X##[12].m_color*10+X##[12].m_number
#define PRINT_CARD_14(X)  	PRINT_CARD_13(X), X##[13].m_color*10+X##[13].m_number
#define PRINT_CARD_15(X)  	PRINT_CARD_14(X), X##[14].m_color*10+X##[14].m_number
#define PRINT_CARD_16(X)  	PRINT_CARD_15(X), X##[15].m_color*10+X##[15].m_number
#define PRINT_CARD_17(X)  	PRINT_CARD_16(X), X##[16].m_color*10+X##[16].m_number
#define PRINT_CARD_18(X)  	PRINT_CARD_17(X), X##[17].m_color*10+X##[17].m_number
#define PRINT_CARD_19(X)  	PRINT_CARD_18(X), X##[18].m_color*10+X##[18].m_number
#define PRINT_CARD_20(X)  	PRINT_CARD_19(X), X##[19].m_color*10+X##[19].m_number
#define PRINT_CARD_21(X)  	PRINT_CARD_20(X), X##[20].m_color*10+X##[20].m_number
#define PRINT_CARD_22(X)  	PRINT_CARD_21(X), X##[21].m_color*10+X##[21].m_number
#define PRINT_CARD_23(X)  	PRINT_CARD_22(X), X##[22].m_color*10+X##[22].m_number
#define PRINT_CARD_24(X)  	PRINT_CARD_23(X), X##[23].m_color*10+X##[23].m_number
#define PRINT_CARD_25(X)  	PRINT_CARD_24(X), X##[24].m_color*10+X##[24].m_number
#define PRINT_CARD_26(X)  	PRINT_CARD_25(X), X##[25].m_color*10+X##[25].m_number
#define PRINT_CARD_27(X)  	PRINT_CARD_26(X), X##[26].m_color*10+X##[26].m_number
#define PRINT_CARD_28(X)  	PRINT_CARD_27(X), X##[27].m_color*10+X##[27].m_number
#define PRINT_CARD_29(X)  	PRINT_CARD_28(X), X##[28].m_color*10+X##[28].m_number
#define PRINT_CARD_30(X)  	PRINT_CARD_29(X), X##[29].m_color*10+X##[29].m_number

#define PRINT_pCARD_1(X)   	X##[0]->m_color*10+X##[0]->m_number
#define PRINT_pCARD_2(X)   	PRINT_pCARD_1(X), X##[1]->m_color*10+X##[1]->m_number
#define PRINT_pCARD_3(X)   	PRINT_pCARD_2(X), X##[2]->m_color*10+X##[2]->m_number
#define PRINT_pCARD_4(X)   	PRINT_pCARD_3(X), X##[3]->m_color*10+X##[3]->m_number
#define PRINT_pCARD_5(X)   	PRINT_pCARD_4(X), X##[4]->m_color*10+X##[4]->m_number
#define PRINT_pCARD_6(X)   	PRINT_pCARD_5(X), X##[5]->m_color*10+X##[5]->m_number
#define PRINT_pCARD_7(X)   	PRINT_pCARD_6(X), X##[6]->m_color*10+X##[6]->m_number
#define PRINT_pCARD_8(X)   	PRINT_pCARD_7(X), X##[7]->m_color*10+X##[7]->m_number
#define PRINT_pCARD_9(X)   	PRINT_pCARD_8(X), X##[8]->m_color*10+X##[8]->m_number
#define PRINT_pCARD_10(X)  	PRINT_pCARD_9(X), X##[9]->m_color*10+X##[9]->m_number
#define PRINT_pCARD_11(X)  	PRINT_pCARD_10(X), X##[10]->m_color*10+X##[10]->m_number
#define PRINT_pCARD_12(X)  	PRINT_pCARD_11(X), X##[11]->m_color*10+X##[11]->m_number
#define PRINT_pCARD_13(X)  	PRINT_pCARD_12(X), X##[12]->m_color*10+X##[12]->m_number
#define PRINT_pCARD_14(X)  	PRINT_pCARD_13(X), X##[13]->m_color*10+X##[13]->m_number
#define PRINT_pCARD_15(X)  	PRINT_pCARD_14(X), X##[14]->m_color*10+X##[14]->m_number
#define PRINT_pCARD_16(X)  	PRINT_pCARD_15(X), X##[15]->m_color*10+X##[15]->m_number
#define PRINT_pCARD_17(X)  	PRINT_pCARD_16(X), X##[16]->m_color*10+X##[16]->m_number
#define PRINT_pCARD_18(X)  	PRINT_pCARD_17(X), X##[17]->m_color*10+X##[17]->m_number
#define PRINT_pCARD_19(X)  	PRINT_pCARD_18(X), X##[18]->m_color*10+X##[18]->m_number
#define PRINT_pCARD_20(X)  	PRINT_pCARD_19(X), X##[19]->m_color*10+X##[19]->m_number
#define PRINT_pCARD_21(X)  	PRINT_pCARD_20(X), X##[20]->m_color*10+X##[20]->m_number
#define PRINT_pCARD_22(X)  	PRINT_pCARD_21(X), X##[21]->m_color*10+X##[21]->m_number
#define PRINT_pCARD_23(X)  	PRINT_pCARD_22(X), X##[22]->m_color*10+X##[22]->m_number
#define PRINT_pCARD_24(X)  	PRINT_pCARD_23(X), X##[23]->m_color*10+X##[23]->m_number
#define PRINT_pCARD_25(X)  	PRINT_pCARD_24(X), X##[24]->m_color*10+X##[24]->m_number
#define PRINT_pCARD_26(X)  	PRINT_pCARD_25(X), X##[25]->m_color*10+X##[25]->m_number
#define PRINT_pCARD_27(X)  	PRINT_pCARD_26(X), X##[26]->m_color*10+X##[26]->m_number
#define PRINT_pCARD_28(X)  	PRINT_pCARD_27(X), X##[27]->m_color*10+X##[27]->m_number
#define PRINT_pCARD_29(X)  	PRINT_pCARD_28(X), X##[28]->m_color*10+X##[28]->m_number
#define PRINT_pCARD_30(X)  	PRINT_pCARD_29(X), X##[29]->m_color*10+X##[29]->m_number


#define  LSC_PRINT_VECpCARD_1(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_1()),#X,PRINT_pCARD_1(X))
#define  LSC_PRINT_VECpCARD_2(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_2()),#X,PRINT_pCARD_2(X))
#define  LSC_PRINT_VECpCARD_3(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_3()),#X,PRINT_pCARD_3(X))
#define  LSC_PRINT_VECpCARD_4(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_4()),#X,PRINT_pCARD_4(X))
#define  LSC_PRINT_VECpCARD_5(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_5()),#X,PRINT_pCARD_5(X))
#define  LSC_PRINT_VECpCARD_6(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_6()),#X,PRINT_pCARD_6(X))
#define  LSC_PRINT_VECpCARD_7(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_7()),#X,PRINT_pCARD_7(X))
#define  LSC_PRINT_VECpCARD_8(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_8()),#X,PRINT_pCARD_8(X))
#define  LSC_PRINT_VECpCARD_9(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_9()),#X,PRINT_pCARD_9(X))
#define  LSC_PRINT_VECpCARD_10(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_10()),#X,PRINT_pCARD_10(X))
#define  LSC_PRINT_VECpCARD_11(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_11()),#X,PRINT_pCARD_11(X))
#define  LSC_PRINT_VECpCARD_12(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_12()),#X,PRINT_pCARD_12(X))
#define  LSC_PRINT_VECpCARD_13(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_13()),#X,PRINT_pCARD_13(X))
#define  LSC_PRINT_VECpCARD_14(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_14()),#X,PRINT_pCARD_14(X))
#define  LSC_PRINT_VECpCARD_15(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_15()),#X,PRINT_pCARD_15(X))
#define  LSC_PRINT_VECpCARD_16(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_16()),#X,PRINT_pCARD_16(X))
#define  LSC_PRINT_VECpCARD_17(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_17()),#X,PRINT_pCARD_17(X))
#define  LSC_PRINT_VECpCARD_18(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_18()),#X,PRINT_pCARD_18(X))
#define  LSC_PRINT_VECpCARD_19(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_19()),#X,PRINT_pCARD_19(X))
#define  LSC_PRINT_VECpCARD_20(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_20()),#X,PRINT_pCARD_20(X))
#define  LSC_PRINT_VECpCARD_21(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_21()),#X,PRINT_pCARD_21(X))
#define  LSC_PRINT_VECpCARD_22(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_22()),#X,PRINT_pCARD_22(X))
#define  LSC_PRINT_VECpCARD_23(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_23()),#X,PRINT_pCARD_23(X))
#define  LSC_PRINT_VECpCARD_24(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_24()),#X,PRINT_pCARD_24(X))
#define  LSC_PRINT_VECpCARD_25(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_25()),#X,PRINT_pCARD_25(X))
#define  LSC_PRINT_VECpCARD_26(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_26()),#X,PRINT_pCARD_26(X))
#define  LSC_PRINT_VECpCARD_27(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_27()),#X,PRINT_pCARD_27(X))
#define  LSC_PRINT_VECpCARD_28(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_28()),#X,PRINT_pCARD_28(X))
#define  LSC_PRINT_VECpCARD_29(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_29()),#X,PRINT_pCARD_29(X))
#define  LSC_PRINT_VECpCARD_30(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_30()),#X,PRINT_pCARD_30(X))

#define  LSC_PRINT_VECCARD_1(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_1()),#X,PRINT_CARD_1(X))
#define  LSC_PRINT_VECCARD_2(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_2()),#X,PRINT_CARD_2(X))
#define  LSC_PRINT_VECCARD_3(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_3()),#X,PRINT_CARD_3(X))
#define  LSC_PRINT_VECCARD_4(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_4()),#X,PRINT_CARD_4(X))
#define  LSC_PRINT_VECCARD_5(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_5()),#X,PRINT_CARD_5(X))
#define  LSC_PRINT_VECCARD_6(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_6()),#X,PRINT_CARD_6(X))
#define  LSC_PRINT_VECCARD_7(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_7()),#X,PRINT_CARD_7(X))
#define  LSC_PRINT_VECCARD_8(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_8()),#X,PRINT_CARD_8(X))
#define  LSC_PRINT_VECCARD_9(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_9()),#X,PRINT_CARD_9(X))
#define  LSC_PRINT_VECCARD_10(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_10()),#X,PRINT_CARD_10(X))
#define  LSC_PRINT_VECCARD_11(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_11()),#X,PRINT_CARD_11(X))
#define  LSC_PRINT_VECCARD_12(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_12()),#X,PRINT_CARD_12(X))
#define  LSC_PRINT_VECCARD_13(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_13()),#X,PRINT_CARD_13(X))
#define  LSC_PRINT_VECCARD_14(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_14()),#X,PRINT_CARD_14(X))
#define  LSC_PRINT_VECCARD_15(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_15()),#X,PRINT_CARD_15(X))
#define  LSC_PRINT_VECCARD_16(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_16()),#X,PRINT_CARD_16(X))
#define  LSC_PRINT_VECCARD_17(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_17()),#X,PRINT_CARD_17(X))
#define  LSC_PRINT_VECCARD_18(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_18()),#X,PRINT_CARD_18(X))
#define  LSC_PRINT_VECCARD_19(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_19()),#X,PRINT_CARD_19(X))
#define  LSC_PRINT_VECCARD_20(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_20()),#X,PRINT_CARD_20(X))
#define  LSC_PRINT_VECCARD_21(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_21()),#X,PRINT_CARD_21(X))
#define  LSC_PRINT_VECCARD_22(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_22()),#X,PRINT_CARD_22(X))
#define  LSC_PRINT_VECCARD_23(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_23()),#X,PRINT_CARD_23(X))
#define  LSC_PRINT_VECCARD_24(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_24()),#X,PRINT_CARD_24(X))
#define  LSC_PRINT_VECCARD_25(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_25()),#X,PRINT_CARD_25(X))
#define  LSC_PRINT_VECCARD_26(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_26()),#X,PRINT_CARD_26(X))
#define  LSC_PRINT_VECCARD_27(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_27()),#X,PRINT_CARD_27(X))
#define  LSC_PRINT_VECCARD_28(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_28()),#X,PRINT_CARD_28(X))
#define  LSC_PRINT_VECCARD_29(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_29()),#X,PRINT_CARD_29(X))
#define  LSC_PRINT_VECCARD_30(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_D_30()),#X,PRINT_CARD_30(X))



 



#ifdef _DEBUG
#define  Debug_Log_CardVec(X) \
switch (X.size())\
{					  	  	 \
	case 1:		LSC_PRINT_VECCARD_1(X); break; \
	case 2:		LSC_PRINT_VECCARD_2(X); break; \
	case 3:		LSC_PRINT_VECCARD_3(X); break; \
	case 4:		LSC_PRINT_VECCARD_4(X); break; \
	case 5:		LSC_PRINT_VECCARD_5(X); break; \
	case 6:		LSC_PRINT_VECCARD_6(X); break; \
	case 7:		LSC_PRINT_VECCARD_7(X); break; \
	case 8:		LSC_PRINT_VECCARD_8(X); break; \
	case 9:		LSC_PRINT_VECCARD_9(X); break; \
	case 10:	LSC_PRINT_VECCARD_10(X); break; \
	case 11: 	LSC_PRINT_VECCARD_11(X); break; \
	case 12: 	LSC_PRINT_VECCARD_12(X); break; \
	case 13: 	LSC_PRINT_VECCARD_13(X); break; \
	case 14:  	LSC_PRINT_VECCARD_14(X); break; \
	case 15:  	LSC_PRINT_VECCARD_15(X); break; \
	case 16:  	LSC_PRINT_VECCARD_16(X); break; \
	case 17:  	LSC_PRINT_VECCARD_17(X); break; \
	case 18:  	LSC_PRINT_VECCARD_18(X); break; \
	case 19:  	LSC_PRINT_VECCARD_19(X); break; \
	case 20:   	LSC_PRINT_VECCARD_20(X); break; \
	case 21:   	LSC_PRINT_VECCARD_21(X); break; \
	case 22:   	LSC_PRINT_VECCARD_22(X); break; \
	case 23:   	LSC_PRINT_VECCARD_23(X); break; \
	case 24:   	LSC_PRINT_VECCARD_24(X); break; \
	case 25:   	LSC_PRINT_VECCARD_25(X); break; \
	case 26:   	LSC_PRINT_VECCARD_26(X); break; \
	case 27:   	LSC_PRINT_VECCARD_27(X); break; \
	case 28:   	LSC_PRINT_VECCARD_28(X); break; \
	case 29:   	LSC_PRINT_VECCARD_29(X); break; \
	case 30:   	LSC_PRINT_VECCARD_30(X); break; \
		default:  			break;	 \
}

#else 	 
#define  Debug_Log_CardVec(x) 
#endif // _DEBUG









#ifdef _DEBUG
#define  Debug_Log_pCardVec(X) \
switch (X.size())\
{					  	  	 \
	case 1:		LSC_PRINT_VECpCARD_1(X); break; \
	case 2:		LSC_PRINT_VECpCARD_2(X); break; \
	case 3:		LSC_PRINT_VECpCARD_3(X); break; \
	case 4:		LSC_PRINT_VECpCARD_4(X); break; \
	case 5:		LSC_PRINT_VECpCARD_5(X); break; \
	case 6:		LSC_PRINT_VECpCARD_6(X); break; \
	case 7:		LSC_PRINT_VECpCARD_7(X); break; \
	case 8:		LSC_PRINT_VECpCARD_8(X); break; \
	case 9:		LSC_PRINT_VECpCARD_9(X); break; \
	case 10:	LSC_PRINT_VECpCARD_10(X); break; \
	case 11: 	LSC_PRINT_VECpCARD_11(X); break; \
	case 12: 	LSC_PRINT_VECpCARD_12(X); break; \
	case 13: 	LSC_PRINT_VECpCARD_13(X); break; \
	case 14:  	LSC_PRINT_VECpCARD_14(X); break; \
	case 15:  	LSC_PRINT_VECpCARD_15(X); break; \
	case 16:  	LSC_PRINT_VECpCARD_16(X); break; \
	case 17:  	LSC_PRINT_VECpCARD_17(X); break; \
	case 18:  	LSC_PRINT_VECpCARD_18(X); break; \
	case 19:  	LSC_PRINT_VECpCARD_19(X); break; \
	case 20:   	LSC_PRINT_VECpCARD_20(X); break; \
	case 21:   	LSC_PRINT_VECpCARD_21(X); break; \
	case 22:   	LSC_PRINT_VECpCARD_22(X); break; \
	case 23:   	LSC_PRINT_VECpCARD_23(X); break; \
	case 24:   	LSC_PRINT_VECpCARD_24(X); break; \
	case 25:   	LSC_PRINT_VECpCARD_25(X); break; \
	case 26:   	LSC_PRINT_VECpCARD_26(X); break; \
	case 27:   	LSC_PRINT_VECpCARD_27(X); break; \
	case 28:   	LSC_PRINT_VECpCARD_28(X); break; \
	case 29:   	LSC_PRINT_VECpCARD_29(X); break; \
	case 30:   	LSC_PRINT_VECpCARD_30(X); break; \
		default:  			break;	 \
}

#else 	 
#define  Debug_Log_pCardVec(x) 
#endif // _DEBUG







#define PRINT_FROMATE_S_1()  %s : %s	
#define PRINT_FROMATE_S_2()  PRINT_FROMATE_S_1() %s
#define PRINT_FROMATE_S_3()  PRINT_FROMATE_S_2() %s
#define PRINT_FROMATE_S_4()  PRINT_FROMATE_S_3() %s
#define PRINT_FROMATE_S_5()  PRINT_FROMATE_S_4() %s
#define PRINT_FROMATE_S_6()	 PRINT_FROMATE_S_5() %s
#define PRINT_FROMATE_S_7()	 PRINT_FROMATE_S_6() %s
#define PRINT_FROMATE_S_8()	 PRINT_FROMATE_S_7() %s
#define PRINT_FROMATE_S_9()	 PRINT_FROMATE_S_8() %s
#define PRINT_FROMATE_S_10() PRINT_FROMATE_S_9() %s
#define PRINT_FROMATE_S_11() PRINT_FROMATE_S_10() %s
#define PRINT_FROMATE_S_12() PRINT_FROMATE_S_11() %s
#define PRINT_FROMATE_S_13() PRINT_FROMATE_S_12() %s
#define PRINT_FROMATE_S_14() PRINT_FROMATE_S_13() %s
#define PRINT_FROMATE_S_15() PRINT_FROMATE_S_14() %s
#define PRINT_FROMATE_S_16() PRINT_FROMATE_S_15() %s
#define PRINT_FROMATE_S_17() PRINT_FROMATE_S_16() %s
#define PRINT_FROMATE_S_18() PRINT_FROMATE_S_17() %s
#define PRINT_FROMATE_S_19() PRINT_FROMATE_S_18() %s
#define PRINT_FROMATE_S_20() PRINT_FROMATE_S_19() %s
#define PRINT_FROMATE_S_21() PRINT_FROMATE_S_20() %s
#define PRINT_FROMATE_S_22() PRINT_FROMATE_S_21() %s
#define PRINT_FROMATE_S_23() PRINT_FROMATE_S_22() %s
#define PRINT_FROMATE_S_24() PRINT_FROMATE_S_23() %s
#define PRINT_FROMATE_S_25() PRINT_FROMATE_S_24() %s
#define PRINT_FROMATE_S_26() PRINT_FROMATE_S_25() %s
#define PRINT_FROMATE_S_27() PRINT_FROMATE_S_26() %s
#define PRINT_FROMATE_S_28() PRINT_FROMATE_S_27() %s
#define PRINT_FROMATE_S_29() PRINT_FROMATE_S_28() %s
#define PRINT_FROMATE_S_30() PRINT_FROMATE_S_29() %s
 

#define PRINT_HU_1(X)   	GetHuStrByID(X##[0])
#define PRINT_HU_2(X)   	PRINT_HU_1(X), GetHuStrByID(X##[1])
#define PRINT_HU_3(X)   	PRINT_HU_2(X), GetHuStrByID(X##[2])
#define PRINT_HU_4(X)   	PRINT_HU_3(X), GetHuStrByID(X##[3])
#define PRINT_HU_5(X)   	PRINT_HU_4(X), GetHuStrByID(X##[4])
#define PRINT_HU_6(X)   	PRINT_HU_5(X), GetHuStrByID(X##[5])
#define PRINT_HU_7(X)   	PRINT_HU_6(X), GetHuStrByID(X##[6])
#define PRINT_HU_8(X)   	PRINT_HU_7(X), GetHuStrByID(X##[7])
#define PRINT_HU_9(X)   	PRINT_HU_8(X), GetHuStrByID(X##[8])
#define PRINT_HU_10(X)  	PRINT_HU_9(X), GetHuStrByID(X##[9])
#define PRINT_HU_11(X)  	PRINT_HU_10(X), GetHuStrByID(X##[10])
#define PRINT_HU_12(X)  	PRINT_HU_11(X), GetHuStrByID(X##[11])
#define PRINT_HU_13(X)  	PRINT_HU_12(X), GetHuStrByID(X##[12])
#define PRINT_HU_14(X)  	PRINT_HU_13(X), GetHuStrByID(X##[13])
#define PRINT_HU_15(X)  	PRINT_HU_14(X), GetHuStrByID(X##[14])
#define PRINT_HU_16(X)  	PRINT_HU_15(X), GetHuStrByID(X##[15])
#define PRINT_HU_17(X)  	PRINT_HU_16(X), GetHuStrByID(X##[16])
#define PRINT_HU_18(X)  	PRINT_HU_17(X), GetHuStrByID(X##[17])
#define PRINT_HU_19(X)  	PRINT_HU_18(X), GetHuStrByID(X##[18])
#define PRINT_HU_20(X)  	PRINT_HU_19(X), GetHuStrByID(X##[19])
#define PRINT_HU_21(X)  	PRINT_HU_20(X), GetHuStrByID(X##[29])
#define PRINT_HU_22(X)  	PRINT_HU_21(X), GetHuStrByID(X##[21])
#define PRINT_HU_23(X)  	PRINT_HU_22(X), GetHuStrByID(X##[22])
#define PRINT_HU_24(X)  	PRINT_HU_23(X), GetHuStrByID(X##[23])
#define PRINT_HU_25(X)  	PRINT_HU_24(X), GetHuStrByID(X##[24])
#define PRINT_HU_26(X)  	PRINT_HU_25(X), GetHuStrByID(X##[25])
#define PRINT_HU_27(X)  	PRINT_HU_26(X), GetHuStrByID(X##[26])
#define PRINT_HU_28(X)  	PRINT_HU_27(X), GetHuStrByID(X##[27])
#define PRINT_HU_29(X)  	PRINT_HU_28(X), GetHuStrByID(X##[28])
#define PRINT_HU_30(X)  	PRINT_HU_29(X), GetHuStrByID(X##[29])
 


#define  LSC_PRINT_HUMSG_1(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_1()),#X,PRINT_HU_1(X))
#define  LSC_PRINT_HUMSG_2(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_2()),#X,PRINT_HU_2(X))
#define  LSC_PRINT_HUMSG_3(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_3()),#X,PRINT_HU_3(X))
#define  LSC_PRINT_HUMSG_4(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_4()),#X,PRINT_HU_4(X))
#define  LSC_PRINT_HUMSG_5(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_5()),#X,PRINT_HU_5(X))
#define  LSC_PRINT_HUMSG_6(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_6()),#X,PRINT_HU_6(X))
#define  LSC_PRINT_HUMSG_7(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_7()),#X,PRINT_HU_7(X))
#define  LSC_PRINT_HUMSG_8(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_8()),#X,PRINT_HU_8(X))
#define  LSC_PRINT_HUMSG_9(X)  LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_9()),#X,PRINT_HU_9(X))
#define  LSC_PRINT_HUMSG_10(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_10()),#X,PRINT_HU_10(X))
#define  LSC_PRINT_HUMSG_11(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_11()),#X,PRINT_HU_11(X))
#define  LSC_PRINT_HUMSG_12(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_12()),#X,PRINT_HU_12(X))
#define  LSC_PRINT_HUMSG_13(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_13()),#X,PRINT_HU_13(X))
#define  LSC_PRINT_HUMSG_14(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_14()),#X,PRINT_HU_14(X))
#define  LSC_PRINT_HUMSG_15(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_15()),#X,PRINT_HU_15(X))
#define  LSC_PRINT_HUMSG_16(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_16()),#X,PRINT_HU_16(X))
#define  LSC_PRINT_HUMSG_17(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_17()),#X,PRINT_HU_17(X))
#define  LSC_PRINT_HUMSG_18(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_18()),#X,PRINT_HU_18(X))
#define  LSC_PRINT_HUMSG_19(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_19()),#X,PRINT_HU_19(X))
#define  LSC_PRINT_HUMSG_20(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_20()),#X,PRINT_HU_20(X))
#define  LSC_PRINT_HUMSG_21(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_21()),#X,PRINT_HU_21(X))
#define  LSC_PRINT_HUMSG_22(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_22()),#X,PRINT_HU_22(X))
#define  LSC_PRINT_HUMSG_23(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_23()),#X,PRINT_HU_23(X))
#define  LSC_PRINT_HUMSG_24(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_24()),#X,PRINT_HU_24(X))
#define  LSC_PRINT_HUMSG_25(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_25()),#X,PRINT_HU_25(X))
#define  LSC_PRINT_HUMSG_26(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_26()),#X,PRINT_HU_26(X))
#define  LSC_PRINT_HUMSG_27(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_27()),#X,PRINT_HU_27(X))
#define  LSC_PRINT_HUMSG_28(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_28()),#X,PRINT_HU_28(X))
#define  LSC_PRINT_HUMSG_29(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_29()),#X,PRINT_HU_29(X))
#define  LSC_PRINT_HUMSG_30(X) LLOG_ERROR(ADD_QUOTES(PRINT_FROMATE_S_30()),#X,PRINT_HU_30(X))



#ifdef _DEBUG
#define  Debug_Log_HUVec(X) \
switch (X.size())\
{					  	  	 \
	case 1:		LSC_PRINT_HUMSG_1(X); break; \
	case 2:		LSC_PRINT_HUMSG_2(X); break; \
	case 3:		LSC_PRINT_HUMSG_3(X); break; \
	case 4:		LSC_PRINT_HUMSG_4(X); break; \
	case 5:		LSC_PRINT_HUMSG_5(X); break; \
	case 6:		LSC_PRINT_HUMSG_6(X); break; \
	case 7:		LSC_PRINT_HUMSG_7(X); break; \
	case 8:		LSC_PRINT_HUMSG_8(X); break; \
	case 9:		LSC_PRINT_HUMSG_9(X); break; \
	case 10:	LSC_PRINT_HUMSG_10(X); break; \
	case 11: 	LSC_PRINT_HUMSG_11(X); break; \
	case 12: 	LSC_PRINT_HUMSG_12(X); break; \
	case 13: 	LSC_PRINT_HUMSG_13(X); break; \
	case 14:  	LSC_PRINT_HUMSG_14(X); break; \
	case 15:  	LSC_PRINT_HUMSG_15(X); break; \
	case 16:  	LSC_PRINT_HUMSG_16(X); break; \
	case 17:  	LSC_PRINT_HUMSG_17(X); break; \
	case 18:  	LSC_PRINT_HUMSG_18(X); break; \
	case 19:  	LSC_PRINT_HUMSG_19(X); break; \
	case 20:   	LSC_PRINT_HUMSG_20(X); break; \
	case 21:   	LSC_PRINT_HUMSG_21(X); break; \
	case 22:   	LSC_PRINT_HUMSG_22(X); break; \
	case 23:   	LSC_PRINT_HUMSG_23(X); break; \
	case 24:   	LSC_PRINT_HUMSG_24(X); break; \
	case 25:   	LSC_PRINT_HUMSG_25(X); break; \
	case 26:   	LSC_PRINT_HUMSG_26(X); break; \
	case 27:   	LSC_PRINT_HUMSG_27(X); break; \
	case 28:   	LSC_PRINT_HUMSG_28(X); break; \
	case 29:   	LSC_PRINT_HUMSG_29(X); break; \
	case 30:   	LSC_PRINT_HUMSG_30(X); break; \
		default:  			break;	 \
}

#else 	 
#define  Debug_Log_HUVec(x) 
#endif // _DEBUG










#endif

