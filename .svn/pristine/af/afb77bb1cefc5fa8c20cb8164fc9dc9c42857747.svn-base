#include "GameHandler.h"
#include "Desk.h"
#include "Card.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "NewHandCards.h"

#ifdef _DEBUG
//#include "C:\\Users\\lishengcai\\Desktop\\Hufa\\NewHandCardsEx.h"
#endif // _DEBUG

struct __SRAND__ { __SRAND__() { srand((unsigned)time(NULL)); } }  s_Srand;

#define MOVE_WAN(X)  (((X)>>1)&511)
#define MOVE_TONG(X)  (((X)>>11)&511)
#define MOVE_TIAO(X)  (((X)>>21)&511)
#define MOVE_FENG(X)  (((X)>>31)&511)
#define MOVE_JIAN(X)  (((X)>>41)&511)

struct ThinkOperateData
{
	union
	{
		U64 u64ThinkDate;
		struct
		{
			Luint u32Opers;
			Luint u32Cards;
		};
	};
	ThinkOperateData() :u64ThinkDate(0) {}
	bool operator ==(U64 ot)const { return u64ThinkDate == ot; }
	bool operator ==(const ThinkOperateData& ot)const { return u64ThinkDate == ot.u64ThinkDate; }
	operator U64()const { return u64ThinkDate; }
	Luint High()const { return u32Cards; }
	Luint Low()const { return u32Opers; }


	Lint GetCard1()const { return u32Cards & 63; }
	void SetCard1(int i) { u32Cards = (u32Opers>>8<<8) | i; }



	/*Lint GetPos();*/

};











const U64 u64ColorMask[3] = { 0X3FE,0XFF800,0X3FE00000 };

//从0开始的数值映射到麻将牌 循环中用
//1万-9万1筒-9筒1条-9条东南西北中发白
const int Index2Cards[CARD_TYPE_COUNT] = { 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,21,22,23,24,25,26,27,28,29,31,32,33,34,41,42,43 };

const int HaveButCanNotShunZi[32] =
{
	0,0,0,0,1,1,1,0,//[0-7]
	0,0,0,0,1,1,0,0,//[8-15]
	0,0,0,0,1,1,1,0,//[16-23]
	0,0,0,0,0,0,0,0 //[24-31]
};

//能不能小吃, A1 & xiaochimask = xiaochimask 不能小吃的 就用特殊数据1替换只. 因为1==0万肯定没有的
//小吃 AB吃C , 12万吃3万
U64 u64XiaoChiMask[44] = { 1,1,1,6,12,24,48,96,192,384,1,1,1,6144,12288,24576,49152,98304,196608,393216,1,1,1,6291456,12582912,25165824,50331648,100663296,201326592,402653184,1,1,1,1,1,1,1,1,1,1,1,1,1 ,1 };
U64 u64ZhongChiMask[44] = { 1,1,10,20,40,80,160,320,640,1,1,1,10240,20480,40960,81920,163840,327680,655360,1,1,1,10485760,20971520,41943040,83886080,167772160,335544320,671088640,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
U64 u64DaChiMask[44] = { 1,12,24,48,96,192,384,768,1,1,1,12288,24576,49152,98304,196608,393216,786432,1,1,1,12582912,25165824,50331648,100663296,201326592,402653184,805306368,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };



//从麻将数值映射到0开始的索引.从麻将牌到上班索引 GetCardID(card)==上边的索引
//				                 0  1 2 3 4 5 6 7 8 9 10 11 1213 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 
static const int Card2I[44] = { 34,0,1,2,3,4,5,6,7,8,34,9,10,11,12,13,14,15,16,17,34,18,19,20,21,22,23,24,25,26,34,27,28,29,30,34,34,34,34,34,34,31,32,33 };

const char* const _pnames[] = { "一万","二万","三万","四万","五万","六万","七万","八万","九万",
"一筒","二筒","三筒","四筒","五筒","六筒","七筒","八筒","九筒",
"一条","二条","三条","四条","五条","六条","七条","八条","九条",
"东","南","西","北","中","发","白","错误" };


//%llu  64位无符号
//%llx	64位16进制数
void printBin(U64 Value, int len = 0, bool endline = true, bool Start = true);


const char * GetMiniCardName(U64 x)  // 
{
	int i = 0;
	while ((x & 1) == 0)
	{
		++i;
		x = x / 2;
	}
	return _pnames[Card2I[i]]; //0无效
}
const char * GetCardName(const Card* pCard)
{
	return _pnames[Card2I[pCard->m_color * 10 + pCard->m_number - 10]];
}
const char * GetCardName(int id)
{
	if (id < 0 || id >43)
	{
		id = 0;
	}
	return _pnames[Card2I[id]];
}




//%llu  64位无符号
//%llx	64位16进制数
inline void printBin(U64  Value, int len, bool endline, bool Start)
{
#ifdef _DEBUG
	if (Start) printf("%20llu= ", Value);
	if (Value < 2)
	{
		for (int i = 0; i < len; ++i)	printf("0");
		printf("%llu", Value & 1);
		if (endline)	printf("\n");
		return;
	}
	printBin(Value / 2, len - 1, false, false);
	printf("%llu", Value & 1);
	if (endline)	printf("\n");
#endif // _DEBUG
}


void U64ToCards(U64 u64, std::vector<Card>& out)
{
	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		int i = Index2Cards[k];
		if (HAVE_MASK(u64, (U64)1 << i))
		{
			Card v;
			v.m_color = i / 10 + 1;
			v.m_number = i % 10;
			out.push_back(v);
		}
	}
}




const static bool bCan13Lan[512] = { 1,1,1,0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1 };

bool NewHandCard::Can13Lan() const
{	
	return m_AllCounts ==14 && (!m_BitsCard[1]) && bCan13Lan[MOVE_WAN(m_BitsCard[0])] && bCan13Lan[MOVE_TIAO(m_BitsCard[0])] && bCan13Lan[MOVE_TONG(m_BitsCard[0])];
}

bool NewHandCard::Can7Xing13Lan() const
{
	return  m_AllCounts == 14 && (!m_BitsCard[1]) && ((0XE0780000000 & m_BitsCard[0]) == 0XE0780000000) && bCan13Lan[MOVE_WAN(m_BitsCard[0])] && bCan13Lan[MOVE_TIAO(m_BitsCard[0])] && bCan13Lan[MOVE_TONG(m_BitsCard[0])];
}

bool NewHandCard::Can13Lan_zheng() const
{
	if (m_AllCounts != 14 || m_BitsCard[1]) //手牌14张,无对
	{
		return false;
	}

	Lint windCount = 0;
	Lint nModVal = -1;
	EntiryCardVector tmpHandCard;
	Serializ2Vec(tmpHandCard);

	for (const auto& card : tmpHandCard)
	{
		if (card.m_color >= 4)
		{
			++windCount;
			continue;
		}

		if (nModVal == -1)
		{
			nModVal = card.m_number % 3;
			continue;
		}
		else if (nModVal != card.m_number % 3)
		{
			return false;
		}
	}

	if (windCount < 5)
	{
		return false;
	}

	return true;
}

bool NewHandCard::Can13Lan_fu() const
{
	if (m_AllCounts != 14 || m_BitsCard[1]) //手牌14张,无对
	{
		return false;
	}

	Lint windCount = 0;
	map<Lint, Lint> mapVal;
	mapVal.insert(make_pair(1, -1));
	mapVal.insert(make_pair(2, -1));
	mapVal.insert(make_pair(3, -1));
	
	EntiryCardVector tmpHandCard;
	Serializ2Vec(tmpHandCard);
	for (const auto& card : tmpHandCard)
	{
		if (card.m_color >= 4)
		{
			++windCount;
			continue;
		}

		if (mapVal.at(card.m_color) == -1)
		{
			mapVal.at(card.m_color) = card.m_number % 3;
		}
		else if (card.m_number % 3 != mapVal.at(card.m_color))
		{
			return false; //同一花色,只能是147或258或369的全部或部分
		}
	}

	if (windCount < 5) //风牌至少5张
	{
		return false;
	}

	return true;
}

//兼容原来的代码 单独做个处理 开始
void NewHandCard::GetColor_number(int maxLen, CardValue* pCards)const //14 包含MAX
{
	if (maxLen <= 0 || pCards == NULL)
	{
		return;
	}
	int loop = 0;

	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		int i = Index2Cards[k];
		for (int j = 0; j < 4; j++)
		{
			if (HAVE_MASK(m_BitsCard[j], (U64)1 << i))
			{
				pCards[loop].m_color = i / 10 + 1;
				pCards[loop].m_number = i % 10;
				if (++loop >= maxLen)
				{
					return;
				}
			}
			else
			{
				break;
			}
		}
	}
}
void NewHandCard::Serializ2Vec(std::vector<CardValue>& vec)const
{
	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		int i = Index2Cards[k];
		for (int j = 0; j < 4; j++)
		{
			if (HAVE_MASK(m_BitsCard[j], (U64)1 << i))
			{
				CardValue v;
				v.m_color = i / 10 + 1;
				v.m_number = i % 10;
				vec.push_back(v);
			}
			else
			{
				break;
			}
		}
	}
}

void NewHandCard::Serializ2Vec(EntiryCardVector& vec) const
{
	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		int i = Index2Cards[k];
		for (int j = 0; j < 4; j++)
		{
			if (HAVE_MASK(m_BitsCard[j], (U64)1 << i))
			{
				Card v;
				v.m_color = i / 10 + 1;
				v.m_number = i % 10;
				vec.push_back(v);
			}
			else
			{
				break;
			}
		}
	}
}

int NewHandCard::Serializ2Vec4YBSuanFa(Lint(&cards)[YINGBIN_SUANFA_VECLEN], int laiziID)const
{
	int LaiziNumber = 0;
	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		int i = Index2Cards[k];
		for (int j = 0; j < SAME_COUNT; j++)
		{
			if (HAVE_MASK(m_BitsCard[j], (U64)1 << i))
			{
				if (laiziID == i)
				{
					LaiziNumber++;
				}
				else
				{
					cards[i]++;
				}
			}
			else
			{
				break;
			}
		}
	}
	return LaiziNumber;
}



void NewHandCard::GetFirstCard(CardValue& out)const
{
	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		int i = Index2Cards[k];
		if (HAVE_MASK(m_BitsCard[0], (U64)1 << i))
		{
			out.m_color = i / 10 + 1;
			out.m_number = i % 10;
			return;
		}
	}
}

void NewHandCard::GetSmartOutCard(CardValue& out) const
{
	U64 notShun = 0;
	U64 Cur = GET_FEIZI(m_BitsCard[0]);
	while (Cur)
	{
		U64 tem = (Cur ^ (Cur - 1))&Cur;
		if (HaveButCanNotShunZi[((m_BitsCard[0] & ((tem * 31) >> 2)) << 2) / tem])
		{
			notShun |= tem;
		}
		Cur = Cur&(Cur - 1);
	}
	notShun |= (0xE0780000000 & m_BitsCard[0]);//获取牌中的风牌和中发白
	U64 duizi = m_BitsCard[1] ^ m_BitsCard[2];
	U64 sanZhang = m_BitsCard[2] ^ m_BitsCard[3];
	U64 canDachu = notShun & (~duizi) & (~sanZhang);
	if (!canDachu) return GetFirstCard(out);
	for (int k = CARD_TYPE_COUNT - 1; k >= 0; --k)
	{
		int i = Index2Cards[k];
		if (HAVE_MASK(canDachu, (U64)1 << i))
		{
			out.m_color = i / 10 + 1;
			out.m_number = i % 10;
			return ;
		}
	}
}

void NewHandCard::GetTail(CardValue& card)
{
	for (int k = CARD_TYPE_COUNT - 1; k >= 0; --k)
	{
		int i = Index2Cards[k];
		if (HAVE_MASK(m_BitsCard[0], (U64)1 << i))
		{
			card.m_color = i / 10 + 1;
			card.m_number = i % 10;
			return;
		}
	}
}

bool NewHandCard::GetCanAnGangs(std::vector<Card>& outcard)
{
	U64 Cur = m_BitsCard[3];
	while (Cur)
	{
		U64 Cur_1 = Cur - 1;
		U64 tem = Cur&~Cur_1;
		U64ToCards(tem, outcard);
		Cur = Cur&Cur_1;
	}
	return m_BitsCard[3] != 0;
}

bool NewHandCard::IsHaveCard(const std::vector<CardValue>& vCard)
{
	for (auto cardVal : vCard)
	{
		Card tmpCard;
		tmpCard.m_color = cardVal.m_color;
		tmpCard.m_number = cardVal.m_number;
		if (IsHaveCard(tmpCard))
		{
			return true;
		}
	}
	return false;
}

//兼容原来的代码 单独做个处理 结束
int NewHandCard::CanEatCard(int id)const
{
	int result = 0;
	if (id < 0 || id >29)
	{
		return  result;
	}
	if ((m_BitsCard[0] & u64XiaoChiMask[id]) == u64XiaoChiMask[id])
	{
		result |= NEW_THING_DOING_XIAO_CHI;
	}
	if ((m_BitsCard[0] & u64ZhongChiMask[id]) == u64ZhongChiMask[id])
	{
		result |= NEW_THING_DOING_ZHONG_CHI;
	}
	if ((m_BitsCard[0] & u64DaChiMask[id]) == u64DaChiMask[id])
	{
		result |= NEW_THING_DOING_DA_CHI;
	}
	return result;
}





void GenreateCardVec(Lint& index, Card	(&card)[CARD_COUNT], CardVector& temp, bool bHaveTong, bool bHaveTiao, bool bHaveFeng, bool bHaveJian)
{	

	//加入万字	//一定有万
	for (Lint k = 0; k < 9; k++)  //9张牌
	{
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
	}


	if (bHaveTong)
	{
		for (Lint k = 0; k < 9; k++)  //9张牌
		{
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
		}
	}


	if (bHaveTiao)
	{
		for (Lint k = 0; k < 9; k++)  //9张牌
		{
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
		}
	}


	if (bHaveFeng)
	{
		for (Lint x = 0; x < 4; x++)  //4张牌 东南西北
		{
			for (int i = 0; i < 4; i++) // 循环4次
			{
				card[index].m_color = CARD_COLOR_FENG;
				card[index].m_number = x + 1;
				index++;
			}
		}
	}

	if (bHaveJian)
	{
		for (Lint x = 0; x < 3; x++)  //3张牌 中 发 白
		{
			for (int i = 0; i < 4; i++)//循环4次
			{
				card[index].m_color = CARD_COLOR_JIAN;
				card[index].m_number = x + 1;
				index++;
			}
		}
	}

	for (Lint i = 0; i < index; ++i)
	{
		temp.push_back(&card[i]);
	}
}

void CompareMethodsEx(int  time)
{

#ifdef _DEBUG
	LLOG_ERROR("Start %8d Times Compare.", time);

	Lint index = 0;
	Card	card[CARD_COUNT];
	CardVector temp;
	GenreateCardVec(index, card, temp, true, true, true, false);


	LTime cur;
	Llong time1 = cur.MSecs();
	NewHandCard NewHandCards;
//	NewHandCardEx  NewHandCardExs;
	Llong total_time1 = 0;
	Llong total_time2 = 0;
	for (int i = 0; i < time; i++)
	{
		int done[136] = { 0 };
		NewHandCards.ReSet();
	//	NewHandCardExs.ReSet();
		CardVector recoder;
		for (int j = 0; j < 14; j++)
		{
			int ll = L_Rand(0, index - 1);
			while (done[ll])
			{
				ll = L_Rand(0, index - 1);
			}
			done[ll] = 1;
	//		NewHandCards.AddCard(temp[ll]);
	//		NewHandCardExs.AddCardByColorNum(temp[ll]->m_color-1, temp[ll]->m_number - 1);
			recoder.push_back(temp[ll]);
		}

		std::vector<CardValue> tem;
		tem.clear();
		cur.Now();
		time1 = cur.MSecs();
		bool bhu1 = false;// NewHandCardExs.CanPihu();
// 		bhu1 = bhu1 | gCardMgr.CheckShisanyao(NewHandCardExs, NULL);
// 		bhu1 = bhu1 | gCardMgr.CheckXiaoqidui(NewHandCardExs, NULL, false, PT_GHOSTCARDNO, tem, false);
// 		bhu1 = bhu1 | gCardMgr.CheckHu(NewHandCardExs, NULL, false, false);
		cur.Now();
		total_time1 += (cur.MSecs() - time1);



		cur.Now();
		time1 = cur.MSecs();
		bool bhu2 = NewHandCards.CanHu() == NEW_HU_TYPE_PIHU;
		cur.Now();
		total_time2 += (cur.MSecs() - time1);


		if (bhu1 != bhu2)
		{
			LLOG_ERROR("\n\nERRor  bhu1 = %d bhu2= %d", bhu1, bhu2);
			gCardMgr.SortCard(recoder);
			LLOG_ERROR("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				recoder[0]->m_color * 10 + recoder[0]->m_number,
				recoder[1]->m_color * 10 + recoder[1]->m_number,
				recoder[2]->m_color * 10 + recoder[2]->m_number,
				recoder[3]->m_color * 10 + recoder[3]->m_number,
				recoder[4]->m_color * 10 + recoder[4]->m_number,
				recoder[5]->m_color * 10 + recoder[5]->m_number,
				recoder[6]->m_color * 10 + recoder[6]->m_number,
				recoder[7]->m_color * 10 + recoder[7]->m_number,
				recoder[8]->m_color * 10 + recoder[8]->m_number,
				recoder[9]->m_color * 10 + recoder[9]->m_number,
				recoder[10]->m_color * 10 + recoder[10]->m_number,
				recoder[11]->m_color * 10 + recoder[11]->m_number,
				recoder[12]->m_color * 10 + recoder[12]->m_number,
				recoder[13]->m_color * 10 + recoder[13]->m_number
			);

			for (int i = 0; i < 14; ++i)
			{
				LLOG_ERROR("%s", GetCardName(recoder[i]));
			}
		}
	}
	LLOG_ERROR("%7d Times calculate  time1 = %d", time, total_time1);
	LLOG_ERROR("%7d Times calculate  time2 = %d", time, total_time2);
	LLOG_ERROR("END   %8d Times Compare.", time);
	//	Sleep(20000);
#endif // _DEBUG
}


void CompareMethodsEx1(int  time)
{

#ifdef _DEBUG
	LLOG_ERROR("Start %8d Times Compare.", time);

	Lint index = 0;
	Card	card[CARD_COUNT];
	CardVector temp;
	GenreateCardVec(index, card, temp, true, true, true, false);


	LTime cur;

	NewHandCard NewHandCards;
//	NewHandCardEx  NewHandCardExs;
	Llong total_time1 = 0;
	Llong total_time2 = 0;


	Llong time1 = cur.MSecs();
	for (int i = 0; i < time; i++)
	{
		int done[136] = { 0 };
		NewHandCards.ReSet();
//		NewHandCardExs.ReSet();
		CardVector recoder;
		for (int j = 0; j < 14; j++)
		{
			int ll = L_Rand(0, index - 1);
			while (done[ll])
			{
				ll = L_Rand(0, index - 1);
			}
			done[ll] = 1;
			NewHandCards.AddCard(temp[ll]);
	//		NewHandCardExs.AddCardByColorNum(temp[ll]->m_color - 1, temp[ll]->m_number - 1);
			recoder.push_back(temp[ll]);
		} 
		bool bhu1 = false;// NewHandCardExs.CanPihu();
	}
	cur.Now();
	total_time1 += (cur.MSecs() - time1);



	time1 = cur.MSecs();
	for (int i = 0; i < time; i++)
	{
		int done[136] = { 0 };
		NewHandCards.ReSet();
	//	NewHandCardExs.ReSet();
		CardVector recoder;
		for (int j = 0; j < 14; j++)
		{
			int ll = L_Rand(0, index - 1);
			while (done[ll])
			{
				ll = L_Rand(0, index - 1);
			}
			done[ll] = 1;
			NewHandCards.AddCard(temp[ll]);
	///		NewHandCardExs.AddCardByColorNum(temp[ll]->m_color - 1, temp[ll]->m_number - 1);
			recoder.push_back(temp[ll]);
		}

		NewHandCards.CanHu();
	}
	cur.Now();
	total_time2 += (cur.MSecs() - time1);	 
	 
	LLOG_ERROR("%7d Times calculate  time1 = %d", time, total_time1);
	LLOG_ERROR("%7d Times calculate  time2 = %d", time, total_time2);
	LLOG_ERROR("END   %8d Times Compare.", time);
	//	Sleep(20000);
#endif // _DEBUG
}


void CompareMethods(int  time)
{

#ifdef _DEBUG
	LLOG_ERROR("Start %8d Times Compare.", time);

	Lint index = 0;
	Card	card[CARD_COUNT];
	CardVector temp;	
	GenreateCardVec(index, card, temp,true,true,true,true);


	LTime cur;
	Llong time1 = cur.MSecs();
	NewHandCard NewHandCards;
	CardVector  OldHandCards;
	Llong total_time1 = 0;
	Llong total_time2 = 0;
	Lint special = 0;
	bool isQingQidui = false;
	for (int i = 0; i < time; i++)
	{
		int done[136] = { 0 };
		NewHandCards.ReSet();
		OldHandCards.clear();
		for (int j = 0; j < 14; j++)
		{
			int ll = L_Rand(0, index - 1);
			while (done[ll])
			{
				ll = L_Rand(0, index - 1);
			}
			done[ll] = 1;
			NewHandCards.AddCard(temp[ll]);
			OldHandCards.push_back(temp[ll]);
		}

		std::vector<CardValue> tem;
		cur.Now();
		time1 = cur.MSecs();
		bool bhu1 = false;
		for (int www=0; www<1000; www++)
		{
			bhu1 = false;
    		bhu1 = bhu1 | gCardMgr.CheckShisanyao(OldHandCards, NULL);
			bhu1 = bhu1 | gCardMgr.CheckXiaoqidui(OldHandCards, NULL, false, PT_GHOSTCARDNO, tem, false, special, isQingQidui);
			bhu1 = bhu1 | gCardMgr.CheckHu(OldHandCards, NULL, false, false);
		}
	
		cur.Now();
		total_time1 += (cur.MSecs() - time1);


		bool bhu2;
		cur.Now();
		time1 = cur.MSecs();

		for (int www = 0; www < 1000; www++)
		{
			bhu2 = NewHandCards.CanHu() != 0;
		}
		
		cur.Now();
		total_time2 += (cur.MSecs() - time1);


		if (bhu1 != bhu2)
		{
			LLOG_ERROR("\n\nERRor  bhu1 = %d bhu2= %d", bhu1, bhu2);
			gCardMgr.SortCard(OldHandCards);
			LLOG_ERROR("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				OldHandCards[0]->m_color * 10 + OldHandCards[0]->m_number,
				OldHandCards[1]->m_color * 10 + OldHandCards[1]->m_number,
				OldHandCards[2]->m_color * 10 + OldHandCards[2]->m_number,
				OldHandCards[3]->m_color * 10 + OldHandCards[3]->m_number,
				OldHandCards[4]->m_color * 10 + OldHandCards[4]->m_number,
				OldHandCards[5]->m_color * 10 + OldHandCards[5]->m_number,
				OldHandCards[6]->m_color * 10 + OldHandCards[6]->m_number,
				OldHandCards[7]->m_color * 10 + OldHandCards[7]->m_number,
				OldHandCards[8]->m_color * 10 + OldHandCards[8]->m_number,
				OldHandCards[9]->m_color * 10 + OldHandCards[9]->m_number,
				OldHandCards[10]->m_color * 10 + OldHandCards[10]->m_number,
				OldHandCards[11]->m_color * 10 + OldHandCards[11]->m_number,
				OldHandCards[12]->m_color * 10 + OldHandCards[12]->m_number,
				OldHandCards[13]->m_color * 10 + OldHandCards[13]->m_number
			);

			for (int i = 0; i < 14; ++i)
			{
				LLOG_ERROR("%s", GetCardName(OldHandCards[i]));
			}
		}
	}
	LLOG_ERROR("%7d Times calculate  time1 = %d", time, total_time1);
	LLOG_ERROR("%7d Times calculate  time2 = %d", time, total_time2);
	LLOG_ERROR("END   %8d Times Compare.", time);
	//	Sleep(20000);
#endif // _DEBUG
}

void TestNewHandCards_HuPai(const char* pchar)
{
#ifdef _DEBUG
	if (pchar == NULL)
	{
		return;
	}
	NewHandCard nhc;
	string inCards(pchar);
	size_t last = 0;
	size_t index = inCards.find_first_of(',');
	while (index != std::string::npos)
	{
		string tem = inCards.substr(last, index - last);
		int val = atoi(tem.c_str());
		nhc.AddCard(val / 10, val % 10);
		last = index + 1;
		index = inCards.find_first_of(',', last);
	}
	if (index != last)
	{
		string tem = inCards.substr(last, index - last);
		int val = atoi(tem.c_str());
		nhc.AddCard(val / 10, val % 10);
	}
	if (nhc.CanHu())
	{
		LLOG_ERROR("%s 能胡牌", pchar);
	}
	else
	{
		LLOG_ERROR("%s 不能胡牌", pchar);
	}

#endif
}

void TestNewHandCard_TingPai(const char* pchar)
{
#ifdef _DEBUG
	if (pchar == NULL)
	{
		return;
	}
	NewHandCard nhc;
	string inCards(pchar);
	size_t last = 0;
	size_t index = inCards.find_first_of(',');
	while (index != std::string::npos)
	{
		string tem = inCards.substr(last, index - last);
		int val = atoi(tem.c_str());
		nhc.AddCard(val / 10, val % 10);
		last = index + 1;
		index = inCards.find_first_of(',', last);
	}
	if (index != last)
	{
		string tem = inCards.substr(last, index - last);
		int val = atoi(tem.c_str());
		nhc.AddCard(val / 10, val % 10);
	}

	U64 tingCards = nhc.GetTingCards();
	if (tingCards)
	{
		LLOG_ERROR("--------------------\n\n\n\n");
		LLOG_ERROR("%s", pchar);
		do
		{
			for (int i = 0; i < 45; ++i)
			{
				if (tingCards & (U64)1 << i)
				{
					LLOG_ERROR("tingCard: %s", GetCardName(i));
				}
			}
		} while (0);

	}
	else
	{
		LLOG_ERROR("不能听牌: %s", pchar);
	}

#endif
}

enum TINGPAI_TYPE
{
	TINGPAI_TYPE_NONE = 0, //
	TINGPAI_TYPE_DANDIAOJIANG = 1, //单钓将, 111 222 444 555 999 东?
	TINGPAI_TYPE_BIANZHANG = 2, //听边张,    ABC DEF GHI XX 12?
	TINGPAI_TYPE_QIANZHANG = 3, //听嵌张,    ABC DEF GHI XX  4?6

	TINGPAI_TYPE_LIANGMIAN = 4, //听两面.    ABC DEF GHI XX   ?56?
	TINGPAI_TYPE_SHUANGPENG = 5, //双碰听     ABC DEF GHI   XX?  YY?
	TINGPAI_TYPE_SHUANGDIAOJIANG = 6, //双吊将,    ABC DEF GHI            4567  听47
	TINGPAI_TYPE_DANDIAOandBIANZHANG = 7,//单吊带边张                       1112  听23


};

inline int __Count1(U64 x)//计算1的个数
{
	int sum = 0;
	while (x)
	{
		++sum;
		x = x&(x - 1);
	}
	return sum;
}




inline int __Count1_cmp2(U64 x)//计算1的个数是不是超过2个. 超过2个的就不算了.....
{
	int sum = 0;
	while (x) {
		if (++sum > 2)
		{
			return sum;
		}
		x = x&(x - 1);
	}
	return sum;
}

bool NewHandCard::Can13YaoByGhost(U64 u64GhostCard, bool bHave1PlainGhost) const
{
	if (m_AllCounts != 14)
	{
		return false;
	}
	int nGhostNum = 0;
	int leftcount = bHave1PlainGhost ? 1 : 0;
	for (int i = SAME_COUNT - 1; i >= leftcount; --i)
	{
		if (HAVE_MASK(m_BitsCard[i], u64GhostCard))
		{
			nGhostNum = i + 1 - leftcount;
			break;
		}
	}
	U64 U64Cards = bHave1PlainGhost ? m_BitsCard[0] : (m_BitsCard[0] & ~u64GhostCard);

	if ((U64Cards | SHI_SAN_YAO) != SHI_SAN_YAO)
	{
		return false;
	}
	if (__Count1(U64Cards) < 13 - nGhostNum)
	{
		return false;
	}

	return true;
}

bool NewHandCard::Can7DuiByGhost(U64 u64GhostCard, bool bHave1PlainGhost) const
{
	if (m_AllCounts != 14)
	{
		return false;
	}
	int nGhostNum = 0;
	int leftcount = bHave1PlainGhost ? 1 : 0;
	for (int i = SAME_COUNT - 1; i >= leftcount; --i)
	{
		if (HAVE_MASK(m_BitsCard[i], u64GhostCard))
		{
			nGhostNum = i + 1 - leftcount;
			break;
		}
	}

	U64 u64Bits0 = bHave1PlainGhost ? m_BitsCard[0] : (m_BitsCard[0] & ~u64GhostCard);
	U64 u64Bits1 = m_BitsCard[1] & ~u64GhostCard;
	U64 u64Bits2 = m_BitsCard[2] & ~u64GhostCard;
	U64 u64Bits3 = m_BitsCard[3] & ~u64GhostCard;

	U64 DanZhang = u64Bits0 ^ u64Bits1; //所有的单牌 
	U64 SanZhang = u64Bits2 ^ u64Bits3; //所有的刻子   三张
	if (__Count1(DanZhang) + __Count1(SanZhang) > nGhostNum)
	{
		return false;
	}
	return true;
}

int NewHandCard::Get7DuiByGhost(U64 u64GhostCard, bool bHave1PlainGhost, bool bSanHaoHuaQiDui/* = false*/) const
{
	int nResult = 0;
	if (m_AllCounts != 14)
	{
		return nResult;
	}
	int nGhostCount = 0;
	
	int leftcount = bHave1PlainGhost ? 1 : 0;
	for (int i = SAME_COUNT - 1; i >= leftcount; --i)
	{
		if (HAVE_MASK(m_BitsCard[i], u64GhostCard))
		{
			nGhostCount = i + 1 - leftcount;
			break;
		}
	}

	U64 u64Bits0 = bHave1PlainGhost ? m_BitsCard[0] : (m_BitsCard[0] & ~u64GhostCard);
	U64 u64Bits1 = m_BitsCard[1] & ~u64GhostCard;
	U64 u64Bits2 = m_BitsCard[2] & ~u64GhostCard;
	U64 u64Bits3 = m_BitsCard[3] & ~u64GhostCard;

	U64 DanZhang = u64Bits0 ^ u64Bits1; //所有的单牌 
	U64 SanZhang = u64Bits2 ^ u64Bits3; //所有的刻子   三张

	int nDanCount = __Count1(DanZhang);
	int nSanCount = __Count1(SanZhang);
	int nFreeGhost = nGhostCount - nDanCount - nSanCount;
	if (nFreeGhost <0)
	{
		return nResult;
	}
	int nSiCount= __Count1(u64Bits3);  //所有4个一样的牌不能用鬼充数，

	if (nFreeGhost == 4)
	{
		nSiCount += 1;	//如果还有4张鬼牌,说明鬼牌没有当做鬼牌用,相当于两个相同的对子
	}

	if (bSanHaoHuaQiDui && nSiCount == 3)
	{
		return NEW_HU_TYPE_3HAOHUA7DUI; //三豪华七对
	}

	if (nSiCount >=2)
	{
// #ifdef _DEBUG
// 		LLOG_ERROR("豪华大七对!");
// #endif // _DEBUG
		return NEW_HU_TYPE_HAO7DUI_HU;
	}
	else if (nSiCount >= 1)
	{
// #ifdef _DEBUG
// 		LLOG_ERROR("大七对!");
// #endif // _DEBUG
		return NEW_HU_TYPE_DA7DUI_HU;
	}
// #ifdef _DEBUG
// 	LLOG_ERROR("七对!");
// #endif // _DEBUG
	return NEW_HU_TYPE_7DUI;


	
// 	int nGangCount = nSiCount + nSanCount + nFreeGhost / 2;
// 	if (nGangCount >=2)
// 	{
// #ifdef _DEBUG
// 		LLOG_ERROR("豪华大七对!");
// #endif // _DEBUG
// 		return NEW_HU_TYPE_HAO7DUI_HU;
// 	}
// 	else if (nGangCount >= 1)
// 	{
// #ifdef _DEBUG
// 		LLOG_ERROR("大七对!");
// #endif // _DEBUG
// 		return NEW_HU_TYPE_DA7DUI_HU;
// 	}
// #ifdef _DEBUG
// 	LLOG_ERROR("七对!");
// #endif // _DEBUG
// 	return NEW_HU_TYPE_7DUI;
}


bool NewHandCard::CanPengPengByGhost(U64 u64GhostCard, bool bHave1PlainGhost /*= false*/) const
{
	int nGhostNum = 0;
	int leftcount = bHave1PlainGhost ? 1 : 0;
	for (int i = SAME_COUNT - 1; i >= leftcount; --i)
	{
		if (HAVE_MASK(m_BitsCard[i], u64GhostCard))
		{
			nGhostNum = i + 1 - leftcount;
			break;
		}
	}

	U64 u64Bits0 = bHave1PlainGhost ? m_BitsCard[0] : (m_BitsCard[0] & ~u64GhostCard);
	U64 u64Bits1 = m_BitsCard[1] & ~u64GhostCard;
	U64 u64Bits2 = m_BitsCard[2] & ~u64GhostCard;
	U64 u64Bits3 = m_BitsCard[3] & ~u64GhostCard;

// 	if (u64Bits3)   //四个一万, 加个鬼, 是可以碰碰胡的.日狗
// 	{
// 		return false;
// 	}

	U64 DanZhang = u64Bits0 ^ u64Bits1; //所有的单牌 
	U64 DuiZi = u64Bits1 ^ u64Bits2; //所有的对子 

	if (__Count1(DanZhang) * 2 + __Count1(u64Bits3) * 2 + __Count1(DuiZi) - 1 > nGhostNum)
	{
		return false;
	}
	return true;
}



#ifdef _DEBUG

const  int g_nJuDuiGuPai[512] = //绝对孤牌  没有朋友的, 对a0做索引.
{
	0,1,2,0,4,0,0,0,8,9,0,0,0,0,0,0,16,17,18,16,0,0,0,0,0,1,0,0,0,0,0,0,
	32,33,34,32,36,32,32,32,0,1,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	64,65,66,64,68,64,64,64,72,73,64,64,64,64,64,64,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,0,1,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	128,129,130,128,132,128,128,128,136,137,128,128,128,128,128,128,144,145,146,144,128,128,128,128,128,129,128,128,128,128,128,128,
	0,1,2,0,4,0,0,0,0,1,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,8,9,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,0,1,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	256,257,258,256,260,256,256,256,264,265,256,256,256,256,256,256,272,273,274,272,256,256,256,256,256,257,256,256,256,256,256,256,
	288,289,290,288,292,288,288,288,256,257,256,256,256,256,256,256,256,257,258,256,256,256,256,256,256,257,256,256,256,256,256,256,
	0,1,2,0,4,0,0,0,8,9,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,0,1,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,8,9,0,0,0,0,0,0,16,17,18,16,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,0,1,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,8,9,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
	0,1,2,0,4,0,0,0,0,1,0,0,0,0,0,0,0,1,2,0,0,0,0,0,0,1,0,0,0,0,0,0,
};




const int CountBitsArray[512] = {
	0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
	1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
	2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
	3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
	4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,5,6,6,7,6,7,7,8,6,7,7,8,7,8,8,9,
};
const int ArryNotShunEqual[512] = {
	0,1,2,3,4,5,6,0,8,9,10,11,12,13,0,15,16,17,18,19,20,21,22,16,24,25,26,27,0,1,30,31,
	32,33,34,35,36,37,38,32,40,41,42,43,44,45,32,47,48,49,50,51,52,53,54,48,0,1,2,3,60,61,62,63,
	64,65,66,67,68,69,70,64,72,73,74,75,76,77,64,79,80,81,82,83,84,85,86,80,88,89,90,91,64,65,94,95,
	96,97,98,99,100,101,102,96,104,105,106,107,108,109,96,111,0,1,2,3,4,5,6,0,120,121,122,123,124,125,126,127,
	128,129,130,131,132,133,134,128,136,137,138,139,140,141,128,143,144,145,146,147,148,149,150,144,152,153,154,155,128,129,158,159,
	160,161,162,163,164,165,166,160,168,169,170,171,172,173,160,175,176,177,178,179,180,181,182,176,128,129,130,131,188,189,190,191,
	192,193,194,195,196,197,198,192,200,201,202,203,204,205,192,207,208,209,210,211,212,213,214,208,216,217,218,219,192,193,222,223,
	0,1,2,3,4,5,6,0,8,9,10,11,12,13,0,15,240,241,242,243,244,245,246,240,248,249,250,251,252,253,254,255,
	256,257,258,259,260,261,262,256,264,265,266,267,268,269,256,271,272,273,274,275,276,277,278,272,280,281,282,283,256,257,286,287,
	288,289,290,291,292,293,294,288,296,297,298,299,300,301,288,303,304,305,306,307,308,309,310,304,256,257,258,259,316,317,318,319,
	320,321,322,323,324,325,326,320,328,329,330,331,332,333,320,335,336,337,338,339,340,341,342,336,344,345,346,347,320,321,350,351,
	352,353,354,355,356,357,358,352,360,361,362,363,364,365,352,367,256,257,258,259,260,261,262,256,376,377,378,379,380,381,382,383,
	384,385,386,387,388,389,390,384,392,393,394,395,396,397,384,399,400,401,402,403,404,405,406,400,408,409,410,411,384,385,414,415,
	416,417,418,419,420,421,422,416,424,425,426,427,428,429,416,431,432,433,434,435,436,437,438,432,384,385,386,387,444,445,446,447,
	0,1,2,3,4,5,6,0,8,9,10,11,12,13,0,15,16,17,18,19,20,21,22,16,24,25,26,27,0,1,30,31,
	480,481,482,483,484,485,486,480,488,489,490,491,492,493,480,495,496,497,498,499,500,501,502,496,504,505,506,507,508,509,510,511,
};
const int  MinGhostNeedByAllDans[512] =
{
	0,2,2,1,2,1,1,0,2,4,1,3,1,3,0,2,2,4,4,3,1,3,3,2,1,3,3,2,0,2,2,1,
	2,4,4,3,4,3,3,2,1,3,3,2,3,2,2,1,1,3,3,2,3,2,2,1,0,2,2,1,2,1,1,0,
	2,4,4,3,4,3,3,2,4,6,3,5,3,5,2,4,1,3,3,2,3,2,2,1,3,5,2,4,2,4,1,3,
	1,3,3,2,3,2,2,1,3,5,2,4,2,4,1,3,0,2,2,1,2,1,1,0,2,4,1,3,1,3,0,2,
	2,4,4,3,4,3,3,2,4,6,3,5,3,5,2,4,4,6,6,5,3,5,5,4,3,5,5,4,2,4,4,3,
	1,3,3,2,3,2,2,1,3,5,2,4,2,4,1,3,3,5,5,4,2,4,4,3,2,4,4,3,1,3,3,2,
	1,3,3,2,3,2,2,1,3,5,2,4,2,4,1,3,3,5,5,4,2,4,4,3,2,4,4,3,1,3,3,2,
	0,2,2,1,2,1,1,0,2,4,1,3,1,3,0,2,2,4,4,3,1,3,3,2,1,3,3,2,0,2,2,1,
	2,4,4,3,4,3,3,2,4,6,3,5,3,5,2,4,4,6,6,5,3,5,5,4,3,5,5,4,2,4,4,3,
	4,6,6,5,6,5,5,4,3,5,5,4,5,4,4,3,3,5,5,4,5,4,4,3,2,4,4,3,4,3,3,2,
	1,3,3,2,3,2,2,1,3,5,2,4,2,4,1,3,3,5,5,4,2,4,4,3,2,4,4,3,1,3,3,2,
	3,5,5,4,5,4,4,3,2,4,4,3,4,3,3,2,2,4,4,3,4,3,3,2,1,3,3,2,3,2,2,1,
	1,3,3,2,3,2,2,1,3,5,2,4,2,4,1,3,3,5,5,4,2,4,4,3,2,4,4,3,1,3,3,2,
	3,5,5,4,5,4,4,3,2,4,4,3,4,3,3,2,2,4,4,3,4,3,3,2,1,3,3,2,3,2,2,1,
	0,2,2,1,2,1,1,0,2,4,1,3,1,3,0,2,2,4,4,3,1,3,3,2,1,3,3,2,0,2,2,1,
	2,4,4,3,4,3,3,2,1,3,3,2,3,2,2,1,1,3,3,2,3,2,2,1,0,2,2,1,2,1,1,0,
};


int g_minN[512][8] = {
	{},{ 6 },{ 5,12 },{ 4 },{ 3,10,24 },{ 2 },{ 1,8 },{},
	{ 6,20,48 },{ 54 },{ 4 },{ 52 },{ 2,16 },{ 50 },{},{ 48 },
	{ 12,40,96 },{ 46,102 },{ 45,101,108 },{ 44,100 },{ 8 },{ 42,98 },{ 41,97,104 },{ 40,96 },
	{ 4,32 },{ 38 },{ 37,100 },{ 36 },{},{ 34 },{ 33,96 },{ 32 },
	{ 24,80,192 },{ 30,86,198 },{ 29,85,197,92,204 },{ 28,84,196 },{ 27,83,195,90,202,216 },{ 26,82,194 },{ 25,81,193,88,200 },{ 24,80,192 },
	{ 16 },{ 22 },{ 21,84,196 },{ 20 },{ 19,82,194,208 },{ 18 },{ 17,80,192 },{ 16 },
	{ 8,64 },{ 14,70 },{ 13,69,76 },{ 12,68 },{ 11,67,74,200 },{ 10,66 },{ 9,65,72 },{ 8,64 },
	{},{ 6 },{ 5,68 },{ 4 },{ 3,66,192 },{ 2 },{ 1,64 },{},
	{ 48,160,384 },{ 54,166,390 },{ 53,165,389,60,172,396 },{ 52,164,388 },{ 51,163,387,58,170,394,184,408 },{ 50,162,386 },{ 49,161,385,56,168,392 },{ 48,160,384 },
	{ 54,166,390,180,404,432 },{ 438 },{ 52,164,388 },{ 436 },{ 50,162,386,176,400 },{ 434 },{ 48,160,384 },{ 432 },
	{ 32 },{ 38 },{ 37,44 },{ 36 },{ 35,42,168,392 },{ 34 },{ 33,40 },{ 32 },
	{ 38,164,388,416 },{ 422 },{ 36 },{ 420 },{ 34,160,384 },{ 418 },{ 32 },{ 416 },
	{ 16,128 },{ 22,134 },{ 21,133,28,140 },{ 20,132 },{ 19,131,26,138,152 },{ 18,130 },{ 17,129,24,136 },{ 16,128 },
	{ 22,134,148,400 },{ 406 },{ 20,132 },{ 404 },{ 18,130,144 },{ 402 },{ 16,128 },{ 400 },
	{},{ 6 },{ 5,12 },{ 4 },{ 3,10,136 },{ 2 },{ 1,8 },{},
	{ 6,132,384 },{ 390 },{ 4 },{ 388 },{ 2,128 },{ 386 },{},{ 384 },
	{ 96,320 },{ 102,326 },{ 101,325,108,332 },{ 100,324 },{ 99,323,106,330,120,344 },{ 98,322 },{ 97,321,104,328 },{ 96,320 },
	{ 102,326,116,340,368 },{ 374 },{ 100,324 },{ 372 },{ 98,322,112,336 },{ 370 },{ 96,320 },{ 368 },
	{ 108,332,360 },{ 366 },{ 365 },{ 364 },{ 104,328 },{ 362 },{ 361 },{ 360 },
	{ 100,324,352 },{ 358 },{ 357 },{ 356 },{ 96,320 },{ 354 },{ 353 },{ 352 },
	{ 64 },{ 70 },{ 69,76 },{ 68 },{ 67,74,88 },{ 66 },{ 65,72 },{ 64 },
	{ 70,84,336 },{ 342 },{ 68 },{ 340 },{ 66,80 },{ 338 },{ 64 },{ 336 },
	{ 76,328 },{ 334 },{ 333 },{ 332 },{ 72 },{ 330 },{ 329 },{ 328 },
	{ 68,320 },{ 326 },{ 325 },{ 324 },{ 64 },{ 322 },{ 321 },{ 320 },
	{ 32,256 },{ 38,262 },{ 37,261,44,268 },{ 36,260 },{ 35,259,42,266,56,280 },{ 34,258 },{ 33,257,40,264 },{ 32,256 },
	{ 38,262,52,276,304 },{ 310 },{ 36,260 },{ 308 },{ 34,258,48,272 },{ 306 },{ 32,256 },{ 304 },
	{ 44,268,296 },{ 302 },{ 301 },{ 300 },{ 40,264 },{ 298 },{ 297 },{ 296 },
	{ 36,260,288 },{ 294 },{ 293 },{ 292 },{ 32,256 },{ 290 },{ 289 },{ 288 },
	{},{ 6 },{ 5,12 },{ 4 },{ 3,10,24 },{ 2 },{ 1,8 },{},
	{ 6,20,272 },{ 278 },{ 4 },{ 276 },{ 2,16 },{ 274 },{},{ 272 },
	{ 12,264 },{ 270 },{ 269 },{ 268 },{ 8 },{ 266 },{ 265 },{ 264 },
	{ 4,256 },{ 262 },{ 261 },{ 260 },{},{ 258 },{ 257 },{ 256 },
	{ 192 },{ 198 },{ 197,204 },{ 196 },{ 195,202,216 },{ 194 },{ 193,200 },{ 192 },
	{ 198,212,240 },{ 246 },{ 196 },{ 244 },{ 194,208 },{ 242 },{ 192 },{ 240 },
	{ 204,232 },{ 238 },{ 237 },{ 236 },{ 200 },{ 234 },{ 233 },{ 232 },
	{ 196,224 },{ 230 },{ 229 },{ 228 },{ 192 },{ 226 },{ 225 },{ 224 },
	{ 216 },{ 222 },{ 221 },{ 220 },{ 219 },{ 218 },{ 217 },{ 216 },
	{ 208 },{ 214 },{ 213 },{ 212 },{ 211 },{ 210 },{ 209 },{ 208 },
	{ 200 },{ 206 },{ 205 },{ 204 },{ 203 },{ 202 },{ 201 },{ 200 },
	{ 192 },{ 198 },{ 197 },{ 196 },{ 195 },{ 194 },{ 193 },{ 192 },
	{ 128 },{ 134 },{ 133,140 },{ 132 },{ 131,138,152 },{ 130 },{ 129,136 },{ 128 },
	{ 134,148,176 },{ 182 },{ 132 },{ 180 },{ 130,144 },{ 178 },{ 128 },{ 176 },
	{ 140,168 },{ 174 },{ 173 },{ 172 },{ 136 },{ 170 },{ 169 },{ 168 },
	{ 132,160 },{ 166 },{ 165 },{ 164 },{ 128 },{ 162 },{ 161 },{ 160 },
	{ 152 },{ 158 },{ 157 },{ 156 },{ 155 },{ 154 },{ 153 },{ 152 },
	{ 144 },{ 150 },{ 149 },{ 148 },{ 147 },{ 146 },{ 145 },{ 144 },
	{ 136 },{ 142 },{ 141 },{ 140 },{ 139 },{ 138 },{ 137 },{ 136 },
	{ 128 },{ 134 },{ 133 },{ 132 },{ 131 },{ 130 },{ 129 },{ 128 },
	{ 64 },{ 70 },{ 69,76 },{ 68 },{ 67,74,88 },{ 66 },{ 65,72 },{ 64 },
	{ 70,84,112 },{ 118 },{ 68 },{ 116 },{ 66,80 },{ 114 },{ 64 },{ 112 },
	{ 76,104 },{ 110 },{ 109 },{ 108 },{ 72 },{ 106 },{ 105 },{ 104 },
	{ 68,96 },{ 102 },{ 101 },{ 100 },{ 64 },{ 98 },{ 97 },{ 96 },
	{ 88 },{ 94 },{ 93 },{ 92 },{ 91 },{ 90 },{ 89 },{ 88 },
	{ 80 },{ 86 },{ 85 },{ 84 },{ 83 },{ 82 },{ 81 },{ 80 },
	{ 72 },{ 78 },{ 77 },{ 76 },{ 75 },{ 74 },{ 73 },{ 72 },
	{ 64 },{ 70 },{ 69 },{ 68 },{ 67 },{ 66 },{ 65 },{ 64 },
	{},{ 6 },{ 5,12 },{ 4 },{ 3,10,24 },{ 2 },{ 1,8 },{},
	{ 6,20,48 },{ 54 },{ 4 },{ 52 },{ 2,16 },{ 50 },{},{ 48 },
	{ 12,40 },{ 46 },{ 45 },{ 44 },{ 8 },{ 42 },{ 41 },{ 40 },
	{ 4,32 },{ 38 },{ 37 },{ 36 },{},{ 34 },{ 33 },{ 32 },
	{ 24 },{ 30 },{ 29 },{ 28 },{ 27 },{ 26 },{ 25 },{ 24 },
	{ 16 },{ 22 },{ 21 },{ 20 },{ 19 },{ 18 },{ 17 },{ 16 },
	{ 8 },{ 14 },{ 13 },{ 12 },{ 11 },{ 10 },{ 9 },{ 8 },
	{},{ 6 },{ 5 },{ 4 },{ 3 },{ 2 },{ 1 },{},
};


#endif // _DEBUG


bool NewHandCard::CheckEatGhostEnough(U64 u64GhostCard, bool bHave1PlainGhost /*= false*/) const
{
	if (1)
	{
		return true;
	}
#ifdef _DEBUG
	int nGhostNum = 0;
	int leftcount = bHave1PlainGhost ? 1 : 0;
	for (int i = SAME_COUNT - 1; i >= leftcount; --i)
	{
		if (HAVE_MASK(m_BitsCard[i], u64GhostCard))
		{
			nGhostNum = i + 1 - leftcount;
			break;
		}
	}

	int nJiangCount = 0;//是不是有将 了

	U64 u64Bits0 = bHave1PlainGhost ? m_BitsCard[0] : (m_BitsCard[0] & ~u64GhostCard);
	U64 u64Bits1 = m_BitsCard[1] & ~u64GhostCard;
	U64 u64Bits2 = m_BitsCard[2] & ~u64GhostCard;
	U64 u64Bits3 = m_BitsCard[3] & ~u64GhostCard;


	U64 DanZhang = u64Bits0 ^ u64Bits1; //所有的单牌 
	U64 DuiZi = u64Bits1 ^ u64Bits2;	//所有的对子 
	U64 SanZhang = u64Bits2 ^ u64Bits3; //所有的刻子   三张



	if (GET_ZIPAIs(u64Bits3))
	{
		LLOG_ERROR("四张的字牌 什么鬼都不能解了");
		return false;
	}



	int nNeedGhost = 0;

	//单的字牌
	int nDanzi = CountBitsArray[MOVE_FENG(DanZhang) | MOVE_FENG(u64Bits3)] + CountBitsArray[MOVE_JIAN(DanZhang) | MOVE_JIAN(u64Bits3)];
	if (nDanzi)
	{
		nJiangCount = 1;
		nNeedGhost += nDanzi * 2 - 1;
		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("单张字牌吃到没有鬼 %d %d", nNeedGhost, nGhostNum);
			return false;
		}
	}

	//对的字牌
	int nDuiZi = (CountBitsArray[MOVE_FENG(DuiZi)] + CountBitsArray[MOVE_JIAN(DuiZi)]);
	if (nDuiZi)
	{
		if (nJiangCount)
		{
			nNeedGhost += nDuiZi;
		}
		else
		{
			nNeedGhost += nDuiZi - 1;
			nJiangCount = 1;
		}


		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("对子字牌吃到没有鬼 %d %d", nNeedGhost, nGhostNum);
			return false;
		}
	}


	//万
	int nJuduiDanZhang = g_nJuDuiGuPai[MOVE_WAN(u64Bits0)]; //绝对没有朋友的 单张,
	nDanzi = CountBitsArray[nJuduiDanZhang  &(MOVE_WAN(DanZhang) | MOVE_WAN(u64Bits3))];
	if (nDanzi)
	{
		if (nJiangCount)
		{
			nNeedGhost += nDanzi * 2;
		}
		else
		{
			nNeedGhost += nDanzi * 2 - 1;
			nJiangCount = 1;
		}
		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("单万字吃到没有鬼");
			return false;
		}
	}

	nDuiZi = CountBitsArray[nJuduiDanZhang  & MOVE_WAN(DuiZi)];
	if (nDuiZi)
	{
		if (nJiangCount)
		{
			nNeedGhost += nDuiZi;
		}
		else
		{
			nNeedGhost += nDuiZi - 1;
			nJiangCount = 1;
		}


		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("对子万牌吃到没有鬼 %d %d", nNeedGhost, nGhostNum);
			return false;
		}
	}





	//筒
	nJuduiDanZhang = g_nJuDuiGuPai[MOVE_TONG(u64Bits0)]; //绝对没有朋友的 单张,
	nDanzi = CountBitsArray[nJuduiDanZhang  &(MOVE_TONG(DanZhang) | MOVE_TONG(u64Bits3))];
	if (nDanzi)
	{
		if (nJiangCount)
		{
			nNeedGhost += nDanzi * 2;
		}
		else
		{
			nNeedGhost += nDanzi * 2 - 1;
			nJiangCount = 1;
		}
		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("单筒字吃到没有鬼");
			return false;
		}
	}

	nDuiZi = CountBitsArray[nJuduiDanZhang  & MOVE_TONG(DuiZi)];
	if (nDuiZi)
	{
		if (nJiangCount)
		{
			nNeedGhost += nDuiZi;
		}
		else
		{
			nNeedGhost += nDuiZi - 1;
			nJiangCount = 1;
		}


		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("对子筒牌吃到没有鬼 %d %d", nNeedGhost, nGhostNum);
			return false;
		}
	}

	//条
	nJuduiDanZhang = g_nJuDuiGuPai[MOVE_TIAO(u64Bits0)]; //绝对没有朋友的 单张,
	nDanzi = CountBitsArray[nJuduiDanZhang  &(MOVE_TIAO(DanZhang) | MOVE_TIAO(u64Bits3))];
	if (nDanzi)
	{
		if (nJiangCount)
		{
			nNeedGhost += nDanzi * 2;
		}
		else
		{
			nNeedGhost += nDanzi * 2 - 1;
			nJiangCount = 1;
		}
		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("单条字吃到没有鬼");
			return false;
		}
	}

	nDuiZi = CountBitsArray[nJuduiDanZhang  & MOVE_TIAO(DuiZi)];
	if (nDuiZi)
	{
		if (nJiangCount)
		{
			nNeedGhost += nDuiZi;
		}
		else
		{
			nNeedGhost += nDuiZi - 1;
			nJiangCount = 1;
		}


		if (nNeedGhost > nGhostNum)
		{
			LLOG_ERROR("对子条牌吃到没有鬼 %d %d", nNeedGhost, nGhostNum);
			return false;
		}
	}



	// 	if (MOVE_WAN(DuiZi)==0)
	// 	{
	// 		if (MinGhostNeedByAllDans[MOVE_WAN(DanZhang)]>nGhostNum)
	// 		{
	// 			LLOG_ERROR("单万字吃到没有鬼");
	// 			return false;
	// 		}
	// 	}
	// 	if (MOVE_TIAO(DuiZi) == 0)
	// 	{
	// 		if (MinGhostNeedByAllDans[MOVE_TIAO(DanZhang)] > nGhostNum)
	// 		{
	// 			LLOG_ERROR("单万字吃到没有鬼");
	// 			return false;
	// 		}
	// 	}
	// 	if (MOVE_JIAN(DuiZi) == 0)
	// 	{
	// 		if (MinGhostNeedByAllDans[MOVE_JIAN(DanZhang)] > nGhostNum)
	// 		{
	// 			LLOG_ERROR("单万字吃到没有鬼");
	// 			return false;
	// 		}
	// 	}



	LLOG_ERROR("可能胡牌, 不知道.");

#endif // _DEBUG

	return true;
}

U64 NewHandCard::GetTingCards_Right_Slow(bool bDaiShiSanYao/* = true*/, bool bDaiQiDui/* = true*/)const
{
	U64 out = 0;

	//十三幺
	if (bDaiShiSanYao && m_AllCounts == 13 && (m_BitsCard[0] | SHI_SAN_YAO) == SHI_SAN_YAO)
	{
		if (m_BitsCard[1] == 0)
		{
			return SHI_SAN_YAO;
		}
		else if (!(m_BitsCard[1] & (m_BitsCard[1] - 1)) && !m_BitsCard[2])
		{
			return SHI_SAN_YAO &~m_BitsCard[0];  //听十三幺
		}
	}
	
	//七对
	if (bDaiQiDui)
	{
		U64 DanorSanZhang = m_DanZhang | m_SanZhang;
		if (m_AllCounts == 13 && ((DanorSanZhang & (DanorSanZhang - 1)) == 0))
		{
			out |= DanorSanZhang;  //听七对...
		}
	}
	
	////////////////////////////////////////////////////////////////////
	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		U64 tem = (U64)1 << Index2Cards[k];
		NewHandCardTester HandCardtester1(m_BitsCard[0], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
		if (HandCardtester1.TryAddV64_1(tem))
		{
			U64 CanJiang = HandCardtester1.m_BitsCard[1];
			while (CanJiang)
			{
				U64 CanJiang_1 = CanJiang - 1;
				U64 CurJiangPai = CanJiang&~CanJiang_1;
				NewHandCardTester nhter(HandCardtester1);
				nhter.TryRemoveV64_2(CurJiangPai);
				if (nhter.TryRemoveBy333_ALL_Slow())
				{
					out |= tem;
					break;
				}
				CanJiang = CanJiang&CanJiang_1;
			}
		}
	}
	return out;
}

U64 NewHandCard::GetTingCards() const
{
	U64 out = 0;
	if (m_AllCounts == 13 && (m_BitsCard[0] | SHI_SAN_YAO) == SHI_SAN_YAO)//只有13妖的牌型
	{
		if (m_BitsCard[1] == 0)
		{
			return SHI_SAN_YAO;
		}
		else if (!(m_BitsCard[1] & (m_BitsCard[1] - 1)) && !m_BitsCard[2])
		{
			return SHI_SAN_YAO &~m_BitsCard[0];  //听十三幺
		}
	}
	//UpdateOneTwoThree();
	U64 DanorSanZhang = m_DanZhang | m_SanZhang;
	if (m_AllCounts == 13 && ((DanorSanZhang & (DanorSanZhang - 1)) == 0))
	{
		out |= DanorSanZhang;  //听七对...
	}
	//先看字牌, 四张的情况
	if (GET_ZIPAIs(m_BitsCard[3]))
	{
		return out; //如果有四张一样的字牌,  
	}
	//字牌单张的情况
	U64 DanZipai = GET_ZIPAIs(m_DanZhang);
	if (DanZipai)//如果有单的字牌
	{
		if ((DanZipai & (DanZipai - 1)) == 0)//>=两个单的字牌 不能胡
		{
			NewHandCardTester TempTester(m_BitsCard[0] & ~DanZipai, m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
			if (TempTester.TryRemoveBy333_ALL_Slow())
			{
				out |= DanZipai;
			}
		}
		return out;
	}

	//字牌,对子的情况.
	bool bMustFengJiang = false;
	U64 duiZipai = GET_ZIPAIs(m_DuiZi);//字牌的对		
	if (duiZipai)//有成对的字牌
	{
		switch (__Count1_cmp2(duiZipai))
		{
		case 1:
		{
			bMustFengJiang = true; //如果有成对的字牌, 要想胡, 必需要字牌做将牌, 不用轮询做将了
		}
		break;
		case 2:
		{
			NewHandCardTester newhandcardtester(m_BitsCard[0], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
			newhandcardtester.TryRemoveV64_2(duiZipai);
			if (newhandcardtester.TryRemoveBy333())
			{
				out |= duiZipai;
			}
			return out;
		}
		break;
		default:
			return out;
			break;
		}
	}

	U64 uCanTingCards[13] = { 0 };//如果能听牌只能听这里的牌
	int canTingIndex = 0;
	int nSinglColor = -1;
	U64 AlReadyJiang = 0;
	NewHandCardTester OptTester(m_BitsCard[0], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);// 操作对象

	//先过滤两个单张的  或者单张配风将的情况.
	//

	for (int i = 0; i < 3; ++i)
	{
		switch (m_TypeCounts[i])
		{
		case 1:
		case 4:
		case 7:
		case 10:
		case 13:
		{
			if (bMustFengJiang || nSinglColor != -1)
			{
				return out;
			}
			nSinglColor = i;
		}
		break;
		case 2:
		{
			U64 CurDanzhang = m_DanZhang & u64ColorMask[i];
			if (CurDanzhang)
			{
				U64 SmallDan = CurDanzhang&~(CurDanzhang - 1);
				U64 BigDan = CurDanzhang - SmallDan;
				U64 length = BigDan / SmallDan;
				switch (length)
				{
				case 2://听两面, 或者边张
				{
					uCanTingCards[canTingIndex++] = ((SmallDan / 2) | (BigDan * 2)) & ALL_CARDMD;
				}
				break;
				case 4:  //听嵌张
				{
					uCanTingCards[canTingIndex++] = SmallDan * 2;
				}
				break;

				default:
					return out;
					break;
				}
			}
			else
			{
				//有一对, 成将, 更风将一样的 
				AlReadyJiang = OptTester.m_BitsCard[0];
			}
			break;
			OptTester.m_BitsCard[0] &= ~u64ColorMask[i];
		}
		default:
			break;
		}
	}


	//过滤3个的, 这种牌听牌必不能听这个颜色的
	//必需能组成333的才能听牌
	for (int i = 0; i < 3; ++i)
	{
		switch (m_TypeCounts[i])
		{
		case 3:
		case 6:
		case 9:
		case 12:
		{
			U64 m1 = OptTester.m_BitsCard[0];
			OptTester.m_BitsCard[0] &= u64ColorMask[i];
			if (!OptTester.TryRemoveBy333())
			{
				return out;
			}
			OptTester.m_BitsCard[0] = m1;
			// 			NewHandCardTester HandCardtester(m_BitsCard[0] & u64ColorMask[i], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
			// 			if (!HandCardtester.TryRemoveBy333())
			// 			{
			// 				return out;
			// 			}
		}
		break;
		default:
			break;
		}
	}
	//字牌 三章的情况不用考虑了.... 肯定可以的了
	////////////////////////////////////////////////////////////////////////////////////////////////
	//三个的都能成牌了
	//在过滤双张的//如果有双张了

	for (int i = 0; i < 3; ++i)
	{
		switch (m_TypeCounts[i])
		{
		case 2:
		case 5:
		case 8:
		case 11:
		{
			if (bMustFengJiang)
			{
				//现在一定是2风, 2该色 , 0 , 0 ,0 的情况了..
				//2 5 8 11  这种前提下只有2中可能
				// 1. +1张同色. 组成333  就能听该张
				// 2. 不加张, 能组成2 333 就能听2和风
				// 先看 熟悉的不加张2333
				U64 CanJiang = m_BitsCard[1] & u64ColorMask[i];// 不加加章
				while (CanJiang)
				{
					U64 CanJiang_1 = CanJiang - 1;
					U64 CurJiang = CanJiang&~CanJiang_1;
					NewHandCardTester HandCardtester1(m_BitsCard[0] & u64ColorMask[i], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
					HandCardtester1.TryRemoveV64_2(CurJiang);
					if (HandCardtester1.TryRemoveBy333())
					{
						if (!HAVE_MASK(m_BitsCard[3], CurJiang)) //不是四张都在自己手上的
						{
							out |= CurJiang;
						}

						out |= duiZipai;
						break;
					}
					CanJiang = CanJiang&CanJiang_1;
				}

				//情况1 , 加一张能成3333
				for (int k = 1; k <= 9; ++k)
				{
					U64 tem = (U64)1 << (k + 10 * i);
					NewHandCardTester temTester(m_BitsCard[0] & u64ColorMask[i], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
					if (temTester.TryAddV64_1(tem))
					{
						if (temTester.TryRemoveBy333())
						{
							out |= tem;
						}
					}
				}
			}
			else
			{
				for (int j = i + 1; j < 3; ++j)
				{
					switch (m_TypeCounts[j])
					{
					case 2:
					case 5:
					case 8:
					case 11:
					{
						//现在一定是无风将, 2 +2的组合
						//情况1 , 加一张能成3333
						for (int k = 1; k <= 9; ++k)
						{
							U64 tem = (U64)1 << (k + 10 * i);
							NewHandCardTester TempTester(m_BitsCard[0] & (u64ColorMask[j] | u64ColorMask[i]), m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
							if (TempTester.TryAddV64_1(tem))
							{
								U64 CanJiang = TempTester.m_BitsCard[1] & (u64ColorMask[j] | u64ColorMask[i]);
								while (CanJiang)
								{
									U64 CanJiang_1 = CanJiang - 1;
									U64 CurJiangPai = CanJiang&~CanJiang_1;
									NewHandCardTester nhter(TempTester);
									nhter.TryRemoveV64_2(CurJiangPai);
									if (nhter.TryRemoveBy333())
									{
										out |= tem;
										break;
									}
									CanJiang = CanJiang&CanJiang_1;
								}
							}
						}

						for (int k = 1; k <= 9; ++k)
						{
							U64 tem = (U64)1 << (k + 10 * j);
							NewHandCardTester TempTester(m_BitsCard[0] & (u64ColorMask[j] | u64ColorMask[i]), m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
							if (TempTester.TryAddV64_1(tem))
							{
								U64 CanJiang = TempTester.m_BitsCard[1] & (u64ColorMask[j] | u64ColorMask[i]);
								while (CanJiang)
								{
									U64 CanJiang_1 = CanJiang - 1;
									U64 CurJiangPai = CanJiang&~CanJiang_1;
									NewHandCardTester nhter(TempTester);
									nhter.TryRemoveV64_2(CurJiangPai);
									if (nhter.TryRemoveBy333())
									{
										out |= tem;
										break;
									}
									CanJiang = CanJiang&CanJiang_1;
								}
							}
						}
					}
					break;
					default:
						break;
					}
				}
			}
		}
		break;
		default:
			break;
		}
	}

	if (nSinglColor != -1)
	{
		for (int k = 1; k <= 9; ++k)
		{
			U64 TestCard = (U64)1 << (k + 10 * nSinglColor);
			NewHandCardTester TempTester(m_BitsCard[0] & u64ColorMask[nSinglColor], m_BitsCard[1], m_BitsCard[2], m_BitsCard[3]);
			if (TempTester.TryAddV64_1(TestCard))
			{
				U64 CanJiang = TempTester.m_BitsCard[1] & u64ColorMask[nSinglColor];
				while (CanJiang)
				{
					U64 CanJiang_1 = CanJiang - 1;
					U64 CurJiangPai = CanJiang&~CanJiang_1;
					NewHandCardTester nhter(TempTester);
					nhter.TryRemoveV64_2(CurJiangPai);
					if (nhter.TryRemoveBy333())
					{
						out |= TestCard;
						break;
					}
					CanJiang = CanJiang&CanJiang_1;
				}
			}
			else
			{
				int deddd = 3;
			}
		}
	}
	return  out;
}


bool NewHandCard::GetTingCardsByOutCard(Ting_Result& inOuts)const
{ 
	NewHandCard temTest(*this);
	bool bFound = false;
	U64 Cur =   m_BitsCard[0];
	for (int k = 0; k < CARD_TYPE_COUNT; ++k)
	{
		int i = Index2Cards[k];		 
		if (temTest.RemoveCard(i/10+1, i%10))
		{
			temTest.UpdateOneTwoThree();
			U64 T = temTest.GetTingCards_Right_Slow();
			if (T)
			{
#ifdef _DEBUG1 
			   LLOG_ERROR("                  ");
					LLOG_ERROR("打出; %s ", GetCardName(i));
					for (int i = 0; i < 45; ++i)					
						if (T & (U64)1 << i)						
							LLOG_ERROR("                听 %s", GetCardName(i));
					
#endif // _DEBUG 
				inOuts.push_back(ChangeCard_Ting(i, Luint(T >> 32), Luint(T & 0XFFFFFFFF)));
				bFound = true;
			}
			temTest.AddCard(i / 10 + 1, i % 10);
		}
	}
	return bFound;
}

void CompareTingPai(int  time)
{

#ifdef _DEBUG
	LLOG_ERROR("Start %8d Times  TingPai.", time);
	//一定有万
	bool bHaveTong = true;   //是否有筒子
	bool bHaveTiao = true;   //是否有条子
	bool bHaveFeng = true;   //是否有风牌 东南西北
	bool bHaveJian = true;   //是否有箭牌 中发白


	LTime cur;
	Llong time1 = cur.MSecs();
	NewHandCard nhc;
	CardVector  ohc;
	Card	card[CARD_COUNT];
	Lint index = 0;
	CardVector AllCards;

	Llong total_time1 = 0;
	Llong total_time2 = 0;


	//加入万字
	for (Lint k = 0; k < 9; k++)  //9张牌
	{
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
		card[index].m_color = CARD_COLOR_WAN;
		card[index].m_number = k + 1;
		index++;
	}


	if (bHaveTong)	//-V547
	{
		for (Lint k = 0; k < 9; k++)  //9张牌
		{
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_TUO;
			card[index].m_number = k + 1;
			index++;
		}
	}


	if (bHaveTiao) //-V547
	{
		for (Lint k = 0; k < 9; k++)  //9张牌
		{
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
			card[index].m_color = CARD_COLOR_SUO;
			card[index].m_number = k + 1;
			index++;
		}
	}


	if (bHaveFeng) //-V547
	{
		for (Lint x = 0; x < 4; x++)  //4张牌 东南西北
		{
			for (int i = 0; i < 4; i++) // 循环4次
			{
				card[index].m_color = CARD_COLOR_FENG;
				card[index].m_number = x + 1;
				index++;
			}
		}
	}

	if (bHaveJian) //-V547
	{
		for (Lint x = 0; x < 3; x++)  //3张牌 中 发 白
		{
			for (int i = 0; i < 4; i++)//循环4次
			{
				card[index].m_color = CARD_COLOR_JIAN;
				card[index].m_number = x + 1;
				index++;
			}
		}
	}

	for (Lint i = 0; i < index; ++i)
	{
		AllCards.push_back(&card[i]);
	}

	std::vector<CardValue> tem;
	tem.clear();

	for (int i = 0; i < time; i++)
	{
		int done[136] = { 0 };
		nhc.ReSet();
		ohc.clear();
		for (int j = 0; j < 13; j++)
		{
			int ll = L_Rand(0, index - 1);
			while (done[ll])
			{
				ll = L_Rand(0, index - 1);
			}
			done[ll] = 1;
			nhc.AddCard(AllCards[ll]);
			ohc.push_back(AllCards[ll]);
		}

		nhc.UpdateOneTwoThree();
		cur.Now();
		time1 = cur.MSecs();
		U64 tingCards1 = nhc.GetTingCards_Right_Slow();
		cur.Now();
		total_time1 += (cur.MSecs() - time1);


		nhc.UpdateOneTwoThree();
		cur.Now();
		time1 = cur.MSecs();
		U64 tingCards2 = nhc.GetTingCards();
		cur.Now();
		total_time2 += (cur.MSecs() - time1);

		if (tingCards1 != tingCards2)
		{
			LLOG_ERROR("--------------------\n\n\n\n");
			gCardMgr.SortCard(ohc);
			LLOG_ERROR("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
				ohc[0]->m_color * 10 + ohc[0]->m_number,
				ohc[1]->m_color * 10 + ohc[1]->m_number,
				ohc[2]->m_color * 10 + ohc[2]->m_number,
				ohc[3]->m_color * 10 + ohc[3]->m_number,
				ohc[4]->m_color * 10 + ohc[4]->m_number,
				ohc[5]->m_color * 10 + ohc[5]->m_number,
				ohc[6]->m_color * 10 + ohc[6]->m_number,
				ohc[7]->m_color * 10 + ohc[7]->m_number,
				ohc[8]->m_color * 10 + ohc[8]->m_number,
				ohc[9]->m_color * 10 + ohc[9]->m_number,
				ohc[10]->m_color * 10 + ohc[10]->m_number,
				ohc[11]->m_color * 10 + ohc[11]->m_number,
				ohc[12]->m_color * 10 + ohc[12]->m_number
			);

			LLOG_ERROR("%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
				GetCardName(ohc[0]),
				GetCardName(ohc[1]),
				GetCardName(ohc[2]),
				GetCardName(ohc[3]),
				GetCardName(ohc[4]),
				GetCardName(ohc[5]),
				GetCardName(ohc[6]),
				GetCardName(ohc[7]),
				GetCardName(ohc[8]),
				GetCardName(ohc[9]),
				GetCardName(ohc[10]),
				GetCardName(ohc[11]),
				GetCardName(ohc[12])
			);

			for (int i = 0; i < 45; ++i)
			{
				if (tingCards1 & (U64)1 << i)
				{
					LLOG_ERROR("tingCard1: %s", GetCardName(i));
				}
			}


			for (int i = 0; i < 45; ++i)
			{
				if (tingCards2 & (U64)1 << i)
				{
					LLOG_ERROR("tingCard2: %s", GetCardName(i));
				}
			}


		}
	}
	LLOG_ERROR("%7d Times calculate  time1 = %d", time, total_time1);
	LLOG_ERROR("%7d Times calculate  time2 = %d", time, total_time2);
	LLOG_ERROR("END   %8d Times  .", time);
	//	Sleep(20000);
#endif // _DEBUG
}



const char * GetHuStrByID(int id)
{
#ifdef _DEBUG
	switch (id)
	{
	case 1:				return "小胡";
	case 7:				return "自摸";
	case 8:				return "杠上开花";
	case 9:				return "杠上炮";
	case 10:			return "海底捞";
	case 11:			return "海底炮";
	case 12:			return "抢扛胡";
	case 13:			return "起手四个赖子";
	case 14:			return "门清";
	case 15:			return "一副中发白刻";
	case 16:			return "二副中发白刻";
	case 17:			return "风牌刻与圈相同";
	case 18:			return "风牌刻与位相同";
	case 50:			return "乱将胡:全是258";
	case 51:			return "清7对：8番";
	case 52:			return "全求人";
	case 53:			return "大七对";
	case 54:			return "吃三比包三家";
	case 101:			return "鸡胡";
	case 102:			return "平胡";
	case 103:			return "碰碰胡";
	case 104:			return "混一色";
	case 105:			return "混碰";
	case 106:			return "清一色";
	case 107:			return "清碰";
	case 108:			return "混幺九";
	case 109:			return "小三元";
	case 110:			return "小四喜";
	case 111:			return "字一色";
	case 112:			return "清幺九";
	case 113:			return "大三元";
	case 114:			return "大四喜";
	case 115:			return "十三幺";
	case 116:			return "豪华七对";
	case 117:			return "小七对";
	case 118:			return "人胡";
	case 119:			return "地胡";
	case 120:			return "天胡";
	case 121:			return "九莲宝灯";
	case 122:			return "四鬼胡";
	case 123:			return "幺九";
	case 124:			return "无鬼加番";
	case 125:			return "花吊单";
	case 126:			return "花吊花";

	case 70:			return "抢杠包三家";
	case  71:			return "一条龙";
	case  80:			return "边牌";
	case  81:			return "单调";
	case  82:			return "胡两队";
	case  83:			return "十三烂";
	case  84:			return "无花果";
	case  85:			return "大吊车";
	case  86:			return "7星十三烂";
	case  90:			return "全求炮";
	default:

#ifdef _DEBUG//内存泄漏, DEBUG版无所谓了
		static char p[16];
		sprintf(p, "未知类型 %d", id);
		return p; //内存泄漏, DEBUG版无所谓了
#else
		return "未知类型";
#endif // _DEBUG

		
		break;
	}

#else
	return "未知类型";
#endif // _DEBUG
}



template<int nBitCounts>
struct BitSetAry
{ 
	enum  
	{
		BIT_SIZE = nBitCounts,				//bit总数
		INT64_SIZE = (BIT_SIZE + 63) / 64,	//u64总数
	};
//	static int GetBitSize(void)	{ return BIT_SIZE; }
//	static int GetInt64Size(void) { return INT64_SIZE; }

public:
	BitSetAry<nBitCounts>(void)	{ClearAllBits();}
	~BitSetAry<nBitCounts>(void){}  
	void MarkAllBits(void)	{ memset(m_U64BitBuffer, 0xff, sizeof(m_U64BitBuffer));	}
	void ClearAllBits(void)	{ memset(m_U64BitBuffer, 0x00, sizeof(m_U64BitBuffer));	}
	bool GetBit(unsigned int nIdx) const {	return (BIT_SIZE > nIdx) && ((m_U64BitBuffer[nIdx >> 6] & ((unsigned long long)1 << (nIdx & 63))) ? true : false);	}
	void MarkBit(unsigned int nIdx)
	{ 
		if (BIT_SIZE > nIdx)		
			m_U64BitBuffer[nIdx >> 6] |= (unsigned long long)1 << (nIdx & 63);
	}
	void ClearBit(unsigned int nIdx)
	{
		if (BIT_SIZE > nIdx)				 
			m_U64BitBuffer[nIdx >> 6] &= ~((unsigned long long)1 << (nIdx & 63));
	} 
private:
	 unsigned long long 	m_U64BitBuffer[INT64_SIZE];
};



BitSetAry <128> d;