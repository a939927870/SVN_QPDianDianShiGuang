#include "Card.h"
#include "LTool.h"
#include "LLog.h"
#include "Config.h"
#include "LTime.h"
#include "NewHandCards.h"
#include <vector>
#include <random>
static bool CardSortFun(const Card* c1, const Card* c2)
{
	if (NULL == c1 || NULL == c2)
	{
		return false;
	}
	if(c2->m_color > c1->m_color)
	{
		return true;
	}
	else if(c2->m_color == c1->m_color)
	{
		return c2->m_number > c1->m_number;
	}
	
	return false;
}

bool CardManager::Init()
{
	Lint index = 0;

	for (Lint j = 0; j < 3; j ++)	//三种色
	{
		for (Lint k = 0; k < 9; k ++)  //9张牌
		{
			for(Lint i = 0; i < 4 ;++i)		//循环加四次
			{
				m_card[index].m_color = j + 1;
				m_card[index].m_number = k + 1;
				index ++ ;
			}
		}
	}

	index = 0;
	for(Lint x = 0;x < 4; x++)  //4张牌 东南西北
	{
		for(int i = 0;i < 4; i++) // 循环4次
		{
			m_card[108 + index].m_color = CARD_COLOR_FENG;
			m_card[108 + index].m_number = x + 1;
			index++;
		}
	}

	index = 0;
	for(Lint x = 0;x < 3;x++)  //3张牌 中 发 白
	{
		for(int i = 0;i < 4;i++)//循环4次
		{
			m_card[124 + index].m_color= CARD_COLOR_JIAN;
			m_card[124 + index].m_number = x + 1; 
			index++;
		}
	}

	index = 0;
	for(Lint x = 0; x < 8; x++)  //8张花牌 春夏秋冬梅兰竹菊
	{
		m_card[136 + index].m_color = CARD_COLOR_FLOWER;
		m_card[136 + index].m_number = x + 1; 
		index++;
	}
	for(Lint i = 0 ; i < CARD_COUNT; ++i)
	{
		m_cardVec.push_back(&m_card[i]);
	}
	SortCard(m_cardVec);

	//初始化十三幺
	Lint temp = 0;
	for (Lint i = 1; i < 4; i ++)
	{
		m_ShiSanYiao[temp].m_color = i;
		m_ShiSanYiao[temp].m_number = 1;
		temp ++;
		m_ShiSanYiao[temp].m_color = i;
		m_ShiSanYiao[temp].m_number = 9;
		temp ++;
	}
	for (Lint i = 1; i < 5; i ++)
	{
		m_ShiSanYiao[temp].m_color = CARD_COLOR_FENG;
		m_ShiSanYiao[temp].m_number = i;
		temp ++;
	}
	for (Lint i = 1; i < 4; i ++)
	{
		m_ShiSanYiao[temp].m_color = CARD_COLOR_JIAN;
		m_ShiSanYiao[temp].m_number = i;
		temp ++;
	}
	for (Lint i = 0; i < 13; i ++)
	{
		m_ShiSanYiaoVec.push_back(&m_ShiSanYiao[i]);
	}
	SortCard(m_ShiSanYiaoVec);

	return true;
}

bool CardManager::Final()
{
	return true;
}

void CardManager::SwapCardBySpecial(CardVector& cvIn, const Card specialCard[CARD_COUNT]) const
{
	Lint nCardCount = cvIn.size();
	for (Lint i = 0; i < nCardCount; i ++)
	{
		if (specialCard[i].m_color == 0 || specialCard[i].m_number == 0)
		{
			break;
		}
		for (Lint j = i + 1; j < nCardCount; j ++)
		{
			if (cvIn[j]->m_color == specialCard[i].m_color && cvIn[j]->m_number == specialCard[i].m_number)
			{
				const Card* tmp = cvIn[j];
				cvIn[j] = cvIn[i];
				cvIn[i] = tmp;	
				break;
			}
		}
	}
}

void CardManager::DealCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind, bool needBaiban, bool needHongzhong, bool needWan, bool needflower)
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	if (!needflower)
	{
		mCards.erase(mCards.begin() + 136, mCards.end());	//保留白板及花牌（删除风、中发）
	}

	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiban)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 132);	//保留白板及花牌（删除风、中发）
		}
		else if (needHongzhong)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);	//先删除风
			mCards.erase(mCards.begin() + 112, mCards.end());			//保留红中
		}
		else
		{
			mCards.erase(mCards.begin() + 108, mCards.end());
		}
	}

	//判断要不要万
	if (!needWan)
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);
	
	SwapCardBySpecial(mCards, specialCard);
	v1.insert(v1.begin(),mCards.begin(),mCards.begin()+13);
	v2.insert(v2.begin(),mCards.begin()+13,mCards.begin()+26);
	v3.insert(v3.begin(),mCards.begin()+26,mCards.begin()+39);
	v4.insert(v4.begin(),mCards.begin()+39,mCards.begin()+52);
	v5.insert(v5.begin(),mCards.begin()+52,mCards.end());
	
	std::reverse(v5.begin(),v5.end());		//逆序桌上牌
	
	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealGeneratedCards(CardVector& mCards, CardVector* playerHandCard, Lint playerCount, CardVector& deskCard, Card specialCard[])
{
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	if (gConfig.GetDebugModel() && specialCard && specialCard[0].m_color > 0)
	{
		SwapCardBySpecial(mCards, specialCard);
	}

	CardVector *playerCards = playerHandCard;
	int i = 0;
	for (; i < playerCount; i++, playerCards++)
	{
		playerCards->insert(playerCards->begin(), mCards.begin() + 13 * i, mCards.begin() + 13 * (i + 1));
		SortCard(*playerCards);
	}

	deskCard.insert(deskCard.begin(), mCards.begin() + 13 * i, mCards.end());
	return;
}

void CardManager::DealAllCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5)
{
	//随机算法修改
	CardVector mCards = m_cardVec;
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealAllCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT])
{
	CardVector mCards = m_cardVec;
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealAllCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[CARD_COUNT])
{
	CardVector mCards = m_cardVec;
	DealGeneratedCards(mCards, playerHandCard, playerCount, pile, specialCard);
	if (gConfig.GetDebugModel() && specialCard && specialCard[0].m_color > 0)
	{
		std::reverse(pile.begin(), pile.end());
	}
}

void CardManager::DealCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind, bool needBaiban, bool needHongzhong, bool needWan, bool needflower)
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	//随机算法修改
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	if (!needflower)
	{
		mCards.erase(mCards.begin() + 136, mCards.end());	//保留白板及花牌（删除风、中发）
	}

	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiban)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 132);		//保留白板
		}
		else if (needHongzhong)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);	//保留中发白
			mCards.erase(mCards.begin() + 112, mCards.end());			//保留红中
		}
		else
		{
			mCards.erase(mCards.begin() + 108, mCards.end());
		}
	}
	//判断要不要万
	if (!needWan)
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}
	////设置随机因子
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(),mCards.begin(),mCards.begin()+13);
	v2.insert(v2.begin(),mCards.begin()+13,mCards.begin()+26);
	v3.insert(v3.begin(),mCards.begin()+26,mCards.begin()+39);
	v4.insert(v4.begin(),mCards.begin()+39,mCards.begin()+52);
	v5.insert(v5.begin(),mCards.begin()+52,mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[]
		, bool needWind, bool needBaiban, bool needHongzhong, bool needWan, bool needflower)
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	//随机算法修改
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	if (!needflower)
	{
		mCards.erase(mCards.begin() + 136, mCards.end());	//保留白板及花牌（删除风、中发）
	}

	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiban)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 132);		//保留白板
		}
		else if (needHongzhong)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);	//保留中发白
			mCards.erase(mCards.begin() + 112, mCards.end());			//保留红中
		}
		else
		{
			mCards.erase(mCards.begin() + 108, mCards.end());
		}
	}
	//判断要不要万
	if (!needWan)
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}

	DealGeneratedCards(mCards, playerHandCard, playerCount, pile, specialCard);
	if (gConfig.GetDebugModel() && specialCard && specialCard[0].m_color > 0)
	{
		std::reverse(pile.begin(), pile.end());
	}
	return;
}

void CardManager::DealJokerChickenCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind)
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	//随机算法修改
	CardVector mCards = m_cardVec;
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		mCards.erase(mCards.begin() + 108, mCards.begin() + 136);
	}
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin()+13);
	v2.insert(v2.begin(), mCards.begin()+13, mCards.begin()+26);
	v3.insert(v3.begin(), mCards.begin()+26, mCards.begin()+39);
	v4.insert(v4.begin(), mCards.begin()+39, mCards.begin()+52);
	v5.insert(v5.begin(), mCards.begin()+52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealJokerChickenCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind)
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	CardVector mCards = m_cardVec;
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		mCards.erase(mCards.begin() + 108, mCards.begin() + 136);
	}

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin()+13);
	v2.insert(v2.begin(), mCards.begin()+13, mCards.begin()+26);
	v3.insert(v3.begin(), mCards.begin()+26, mCards.begin()+39);
	v4.insert(v4.begin(), mCards.begin()+39, mCards.begin()+52);
	v5.insert(v5.begin(), mCards.begin()+52, mCards.end());

	std::reverse(v5.begin(),v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealThreePlayerCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, bool needWind /*= true*/, bool needBaiBan /*= true*/, bool nowan /*= false*/)
{
	//随机算法修改
	CardVector mCards = m_cardVec;

	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiBan)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 132);		//保留白板
		}
		else
		{
			mCards.erase(mCards.begin() + 108, mCards.end());
		}
	}

	//判断要不要万
	if (nowan)
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}
	
	////设置随机因子
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
}

void CardManager::DealThreePlayerCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, const Card specialCard[CARD_COUNT], bool needWind /*= true*/, bool needBaiBan /*= true*/, bool nowan /*= false*/)
{
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiBan)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 132);	//保留白板及花牌（删除风、中发）
		}
		else
		{
			mCards.erase(mCards.begin() + 108, mCards.end());
		}
	}
	//判断要不要万
	if (nowan)
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);
	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.end());
	

	std::reverse(v4.begin(), v4.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
}

void CardManager::DealTwoPlayerCard(CardVector& v1, CardVector& v2, CardVector& v3)
{
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	mCards.erase(mCards.begin() + 36, mCards.begin() + 108);
	
	////设置随机因子
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.end());

	SortCard(v1);
	SortCard(v2);
}

void CardManager::DealTwoPlayerCard2(CardVector& v1, CardVector& v2, CardVector& v3, const Card specialCard[CARD_COUNT])
{
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	mCards.erase(mCards.begin() + 36, mCards.begin() + 108);

	////设置随机因子
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);
	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.end());

	std::reverse(v3.begin(), v3.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
}

void CardManager::DealGuangdongJiHuCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind /*= true*/)
{
	//随机算法修改
	CardVector mCards = m_cardVec;
	mCards.erase(mCards.begin() + 136, mCards.end());
	

	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		mCards.erase(mCards.begin() + 108, mCards.end());
	}
	
	////设置随机因子
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealGuangdongJiHuCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind /*= true*/)
{
	
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		mCards.erase(mCards.begin() + 108, mCards.end());
	}
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);
	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealRedDragonCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needWind /*= true*/, bool needBaiBan /*= true*/)
{

	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiBan)
		{
			//保留红中白板
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);   //删除风
			mCards.erase(mCards.begin() + 112, mCards.begin() + 116);   //删除发财
		}
		else
		{
			//保留红中
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);
			mCards.erase(mCards.begin() + 112, mCards.end());
		}
	}
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealRedDragonCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[CARD_COUNT]
	, bool needWind, bool needBaiBan)
{
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiBan)
		{
			//保留红中白板
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);   //删除风
			mCards.erase(mCards.begin() + 112, mCards.begin() + 116);   //删除发财
		}
		else
		{
			//保留红中
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);
			mCards.erase(mCards.begin() + 112, mCards.end());
		}
	}

	DealGeneratedCards(mCards, playerHandCard, playerCount, pile, specialCard);
	if (gConfig.GetDebugModel() && specialCard && specialCard[0].m_color > 0)
	{
		std::reverse(pile.begin(), pile.end());
	}
}

void CardManager::DealRedDragonCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind /*= true*/, bool needBaiBan /*= true*/)
{
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	mCards.erase(mCards.begin() + 136, mCards.end());
	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiBan)
		{
			//保留红中白板
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);   //删除风
			mCards.erase(mCards.begin() + 112, mCards.begin() + 116);   //删除发财
		}
		else
		{
			//保留红中
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);
			mCards.erase(mCards.begin() + 112, mCards.end());
		}
	}

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealHuizhouZhuang(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5,  Lint flowerNum /*= 0*/)
{
	//随机算法修改
	CardVector mCards = m_cardVec;

	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136 + flowerNum, mCards.end());
	
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealHuizhouZhuang2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT],  Lint flowerNum /*= 0*/)
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	CardVector mCards = m_cardVec;
	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136 + flowerNum, mCards.end());
	

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealHuizhouZhuang(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[]
	,Lint flowerNum)
{
	//随机算法修改
	CardVector mCards = m_cardVec;

	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136 + flowerNum, mCards.end());

	DealGeneratedCards(mCards, playerHandCard, playerCount, pile, specialCard);

	if (gConfig.GetDebugModel() && specialCard && specialCard[0].m_color > 0)
	{
		std::reverse(pile.begin(), pile.end());
	}
	return;
}

void CardManager::DealMeizhouHongzhongbao(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5)
{
	//随机算法修改
	CardVector mCards = m_cardVec;

	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136, mCards.end());

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealMeizhouHongzhongbao2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT])
{
	CardVector mCards = m_cardVec;
	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136, mCards.end());


	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealHongKongCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5)
{
	DealAllCard(v1, v2, v3, v4, v5);
}

void CardManager::DealHongKongCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT])
{
	DealAllCard2(v1, v2, v3, v4, v5, specialCard);
}

void CardManager::DealHuizhouZhuangFlowerCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5)
{
	DealAllCard(v1, v2, v3, v4, v5);
}

void CardManager::DealHuizhouZhuangFlowerCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT])
{
	DealAllCard2(v1, v2, v3, v4, v5, specialCard);
}

void CardManager::DealHuizhouZhuangFlowerCard(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[CARD_COUNT])
{
	DealAllCard(playerHandCard, playerCount, pile, specialCard);
}

void CardManager::DealJiHuFlowerCard(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5)
{
	DealAllCard(v1, v2, v3, v4, v5);
}

void CardManager::DealJiHuFlowerCard2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT])
{
	DealAllCard2(v1, v2, v3, v4, v5, specialCard);
}

void CardManager::DealJieyang(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5)const
{
	//随机算法修改
	CardVector mCards = m_cardVec;

	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136, mCards.end());

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealJieyang2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT])const
{
	CardVector mCards = m_cardVec;
	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136, mCards.end());


	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealJieyang(CardVector playerHandCard[], int playerCount, CardVector& pile, Card specialCard[])
{
	//随机算法修改
	CardVector mCards = m_cardVec;
	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136, mCards.end());

	DealGeneratedCards(mCards, playerHandCard, playerCount, pile, specialCard);
	if (gConfig.GetDebugModel() && specialCard && specialCard[0].m_color > 0)
	{
		std::reverse(pile.begin(), pile.end());
	}
	return;
}

void CardManager::DealShaoguan(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, bool needOneNineWan)
{
	//随机算法修改
	CardVector mCards = m_cardVec;

	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136, mCards.end());

	if (needOneNineWan)
	{
		mCards.erase(mCards.begin() + 4, mCards.begin() + 32);
	}
	else
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::DealShaoguan2(CardVector& v1, CardVector& v2, CardVector& v3, CardVector& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needOneNineWan)
{
	//随机算法修改
	CardVector mCards = m_cardVec;

	//先判断删除花牌数量
	mCards.erase(mCards.begin() + 136, mCards.end());

	if (needOneNineWan)
	{
		mCards.erase(mCards.begin() + 4, mCards.begin() + 32);
	}
	else
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}

	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(mCards.begin(), mCards.end(), engine);

	SwapCardBySpecial(mCards, specialCard);

	v1.insert(v1.begin(), mCards.begin(), mCards.begin() + 13);
	v2.insert(v2.begin(), mCards.begin() + 13, mCards.begin() + 26);
	v3.insert(v3.begin(), mCards.begin() + 26, mCards.begin() + 39);
	v4.insert(v4.begin(), mCards.begin() + 39, mCards.begin() + 52);
	v5.insert(v5.begin(), mCards.begin() + 52, mCards.end());

	std::reverse(v5.begin(), v5.end());		//逆序桌上牌

	SortCard(v1);
	SortCard(v2);
	SortCard(v3);
	SortCard(v4);
}

void CardManager::SortCard(CardVector& vec) const
{
	std::sort(vec.begin(),vec.end(),CardSortFun);
}

void CardManager::EraseCard(CardVector& src,  CardVector& des) const
{
	//这里直接比较地址是否相等
	CardVector::iterator it2 = des.begin();
	for(; it2 != des.end(); ++it2)
	{
		CardVector::iterator it1 = src.begin();
		for(; it1 != src.end();)
		{
			if((*it1) == (*it2))
			{
				it1 = src.erase(it1);
			}
			else
			{
				++it1;
			}
		}
	}
}

Lint CardManager::EraseCard(CardVector& src, const Card& card) const
{
	Lint num = 0;
	CardVector::iterator it = src.begin();
	for( ; it != src.end(); )
	{
		if(IsSame(*it, &card))
		{
			it = src.erase(it);
			++num;
		}
		else
		{
			++it;
		}
	}
	return num;
}

Lint CardManager::EraseCard(CardVector& src, const std::vector<CardValue>& ghostCard) const
{
	int ghostNum = 0;
	CardVector::iterator it_src = src.begin();
	for ( ; it_src != src.end(); )
	{
		CardValue card;
		card.m_color = (*it_src)->m_color;
		card.m_number = (*it_src)->m_number;
		std::vector<CardValue>::const_iterator cit_ghost = std::find(ghostCard.begin(), ghostCard.end(), card);
		if (cit_ghost == ghostCard.end())
		{
			++it_src;
		}
		else
		{
			ghostNum++;
			it_src = src.erase(it_src);
		}
	}

	return ghostNum;
}

bool CardManager::IsGhostCard(const std::vector<CardValue>& ghostCard, const Card * pCard) const
{
		std::vector<CardValue>::const_iterator cit_ghost = ghostCard.begin();
		for(; cit_ghost != ghostCard.end();)
		{
			if(cit_ghost->m_color == pCard->m_color && cit_ghost->m_number == pCard->m_number)
			{
				return true;
			}
			cit_ghost++;
		}

		return false;
}

void CardManager::EraseCard(CardVector& src,const Card* pCard) const
{
	//这里直接比较地址是否相等
	for(Lsize i = 0 ; i  < src.size(); ++i)
	{
		if(src[i] == pCard)
		{
			src.erase(src.begin()+i);
			break;
		}
	}
}

//删除src 与pCard 数据一样的牌 N张
void CardManager::EraseCard(CardVector& src,const Card* pCard, Lint n) const
{
	Lint cnt = 0;
	CardVector::iterator it2 = src.begin();
	for(; it2 != src.end();)
	{
		if(cnt >= n)
			break;

		if(IsSame(pCard,*it2))
		{
			cnt ++;
			it2 = src.erase(it2);
		}
		else
		{
			++it2;
		}
	}
}

void CardManager::EarseSameCard(CardVector& src) const
{
	if(src.empty())
	{
		return;
	}

	Lint color = src.front()->m_color;
	Lint number = src.front()->m_number;
	CardVector::iterator it = src.begin()+1;
	for(; it != src.end();)
	{
		if(color == (*it)->m_color && number == (*it)->m_number)
		{
			it = src.erase(it);
		}
		else
		{
			color = (*it)->m_color;
			number= (*it)->m_number;
			++it;
		}
	}
}

ThinkVec CardManager::CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& mgangCard,CardVector& agangCard,CardVector& eatCard, const Card* outCard,OperateState& gameInfo) const
{
	ThinkVec result;
	ThinkUnit unit;
	if (gameInfo.b_canHu && gameInfo.m_cardState != THINK_OPERATOR_MGANG)	//别人打牌
	{
		if (CheckCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, unit.m_hu, false, gameInfo.m_ghostType))
		{
			unit.m_type = THINK_OPERATOR_BOMB;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}
	}
	else if (gameInfo.b_onlyHu && gameInfo.m_cardState == THINK_OPERATOR_MGANG)	//是杠的状态，并且可以抢杠胡
	{
		if (CheckCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, unit.m_hu, false, gameInfo.m_ghostType))
		{
			unit.m_type = THINK_OPERATOR_BOMB;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}
	}
	
	if (!gameInfo.b_onlyHu)
	{
		bool bzhuanzhuan = CanHongzhongPengGangType(gameInfo);
		if(CheckCanGang(handcard,outCard, bzhuanzhuan))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_MGANG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}
		if(!gameInfo.m_louPeng && CheckCanPeng(handcard,outCard, bzhuanzhuan))
		{
			unit.Clear();
			unit.m_type = THINK_OPERATOR_PENG;
			unit.m_card.push_back(outCard);
			result.push_back(unit);
		}

		CardVector vec;
		if(gameInfo.b_canEat && CheckCanChi(handcard,outCard,vec) )
		{
			for(Lsize i = 0 ; i < vec.size(); i += 2)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_CHI;
				unit.m_card.push_back(vec[i]);
				unit.m_card.push_back(vec[i+1]);
				result.push_back(unit);
			}
		}
	}

	return result;
}

ThinkVec CardManager::CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, const Card* getCard,OperateState& gameInfo) const
{
	ThinkVec result;
	ThinkUnit unit;
	if(gameInfo.b_canHu && CheckCanHu(handcard, pengCard,agangCard,mgangCard,eatCard,getCard,gameInfo,unit.m_hu,true, gameInfo.m_ghostType))
	{
		unit.m_type = THINK_OPERATOR_BOMB;
		if (getCard)
		{
			unit.m_card.push_back(getCard);
		}
		result.push_back(unit);
	}

	if(!gameInfo.b_onlyHu)
	{
		CardVector tmp = handcard;
		if (getCard)
		{
			tmp.push_back(getCard);
		}
		SortCard(tmp);

		CardVector vec;
		bool bzhuanzhuan = CanHongzhongPengGangType(gameInfo);
		if(CheckAnGang(tmp, vec, bzhuanzhuan))					//请注意这个函数的使用，有些玩法，红中不能杠
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_AGANG;
				unit.m_card.push_back(vec[i]);
				result.push_back(unit);
			}
		}

		vec.clear();
		if(CheckMGang(tmp, pengCard, vec, bzhuanzhuan))
		{
			for(Lsize i = 0 ; i < vec.size(); ++i)
			{
				unit.Clear();
				unit.m_type = THINK_OPERATOR_MGANG;
				unit.m_card.push_back(vec[i]);
				result.push_back(unit);
			} 
		}
	}
	return result;
}


bool CardManager::CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, const Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec, bool isGetCard, int ghostType) const
{
	bool result = false;
	switch(gameInfo.m_GameType)
	{
	case GuangDongTuiDaoHuMaJing:
	case ThreePlayerTuiDaoHu:
		result = CheckTuiDaohuCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangDongJiPingHuMaJing:
		result = CheckJipinghuCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case HuNanMaJing:
		result = CheckZhuanzhuanCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangDongOneHundred:
		result = CheckOneHundredCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case HeYuanJokerChickenHu:
		result = CheckJokerChickenHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangDongCustomTuiDaoHu:
		result = CheckCustomTuiDaoCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangDongChaoZhou:
		result = CheckChaoShanCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case HuizhouzhuangGhost:
		result = CheckHuizhouzhuangGhostHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case RedDragon:
		result = CheckRedDragonCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case Hongkong:
		result = CheckHongKongCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case HuizhouzhuangFlower:
		result = CheckHuizhouzhuangFlowerHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case JiHuFlower:
		result = CheckJiHuFlowerHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case TwoPlayerTuiDaoHu:
		result = CheckTwoPlayerTuidaoHuCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangDongJihu:
		result = CheckGuangdongJihuCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case MeizhouHongzhonbao:
		result = CheckMeizhouHongzhongbaoCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangdongJieyang:
		result = CheckGuangdongJieyangCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case Shaoguan:
	case Nanxiaong:
	case Qujiang:
	case Maba:
		result = CheckShaoguanCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangdongJieXiMajiang:
		result = CheckGuangdongJiexiCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType); //检查胡牌算法
		break;
	case ChaoShanBiHu:
		result = CheckChaoShanCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case GuangDongRaoping:
		result = CheckRaopingCanHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	case HuiYangWanFa:
		result = CheckHuiYangHu(handcard, pengCard, agangCard, mgangCard, eatCard, outCard, gameInfo, vec, isGetCard, ghostType);
		break;
	default:
		LLOG_ERROR("Error:Unknown Game Type!");
		break;
	}
	return result;
}

bool CardManager::IsJipingFanShuCanQihu(const int qihufanshu, const std::vector<Lint>& vecHuType) const
{
	bool result = false;
	int fan = 0;
	for (Lint i = 0; i < vecHuType.size(); ++i)
	{
		fan += GetJipingFanNumByHuType(vecHuType[i]);
	}
	result = fan < qihufanshu ? false : true;
	return result;
}

bool CardManager::IsHongKongFanShuCanQihu(const int qihufanshu, const std::vector<Lint>& vecHuType, Lint additionFanNum) const
{
	const Lint nMaxFan = 13;		//13番不再叠加牌型番数
	Lint totalFan = 0;
	for (Lint i = 0; i < vecHuType.size(); ++i)
	{
		Lint fan = GetHongkongFanFromHuType(vecHuType[i]);

		if (nMaxFan == fan)
		{
			totalFan = nMaxFan;
			break;
		}
		totalFan += fan;
	}

	totalFan += additionFanNum;

	bool result =  totalFan >= qihufanshu ? true : false;
	return result;
}


bool CardManager::IsMeizhouHongzhongbaoCanQihu(const int qihufanshu, const std::vector<Lint>& vecHuType) const
{
	Lint totalFan = 0;
	for (Lint i = 0; i < vecHuType.size(); ++i)
	{
		Lint fan = GetMeizhouHongzhongbaoFanFromHuType(vecHuType[i]);

		totalFan += fan;
	}

	bool result = totalFan >= qihufanshu ? true : false;
	return result;
}

//胡牌算法
bool  CardManager::CheckHu(CardVector handcard, const Card* curCard,bool needJiang, bool isGetCard) const
{
	CardVector all = handcard;
	
	if(curCard)
	{
		all.push_back(curCard);
	}
	gCardMgr.SortCard(all);
	//提出所有成对的牌
	CardVector ttow;
	for(Lsize i = 0 ; i+1 < all.size(); )
	{
		if(all[i]->m_color == all[i+1]->m_color &&
			all[i]->m_number == all[i+1]->m_number )
		{
			if(needJiang)
			{
				if(all[i]->m_number==2 || all[i]->m_number == 5 || all[i]->m_number == 8 )
				{
					ttow.push_back(all[i]);
					ttow.push_back(all[i+1]);
				}	
			}
			else
			{
				ttow.push_back(all[i]);
				ttow.push_back(all[i+1]);
			}
			i += 2;
		}
		else
		{
			i += 1;
		}
	}

	//移除某个对， 剩下都能组成三个的，就是胡了
	for(Lsize i = 0 ; i < ttow.size(); i += 2)
	{
		CardVector des = all;
		gCardMgr.EraseCard(des,ttow[i]);
		gCardMgr.EraseCard(des,ttow[i+1]);

		CardVector continueh, checkh;
		Lint nCurrent = 0;
		while(nCurrent < des.size() && des.size() != 0)
		{
			if (nCurrent == 0)
			{
				continueh.push_back(des.front());
				checkh.push_back(des.front());
				nCurrent ++;
				continue;
			}
			if (!IsZi(des[nCurrent]) &&
				des[nCurrent]->m_color == checkh.back()->m_color &&
				des[nCurrent]->m_number == checkh.back()->m_number + 1)
			{
				checkh.push_back(des[nCurrent]);
			}
			else if (des[nCurrent]->m_color == continueh.back()->m_color &&
				des[nCurrent]->m_number == continueh.back()->m_number )
			{
				continueh.push_back(des[nCurrent]);
			}

			//检测是否满足三个
			if (checkh.size() == 3)
			{
				for (Lint j = 0; j < checkh.size(); j ++)
				{
					gCardMgr.EraseCard(des,checkh[j], 1);
				}
				checkh.clear();
				continueh.clear();
				nCurrent = 0;
			}
			else if (continueh.size() == 3)
			{
				for (Lint j = 0; j < continueh.size(); j ++)
				{
					gCardMgr.EraseCard(des,continueh[j], 1);
				}
				checkh.clear();
				continueh.clear();
				nCurrent = 0;
			}
			else
			{
				nCurrent ++;
			}
		}
		//查看结果
		if (des.size() == 0)
		{
			return true;
		}
	}
	return false;
}

bool CardManager::CheckCanGang(CardVector& handcard, const Card* outCard, bool bzhuanzhuan) const
{
	if (NULL == outCard)
	{
		return false;
	}

	if (bzhuanzhuan)	// 红中癞子
	{
		if (outCard->m_color == CARD_COLOR_JIAN && outCard->m_number == 1)
		{ 
			return false;
		}
	}
	CardVector tmpVec;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		const Card* pcard = handcard[i];
		if(pcard->m_color == outCard->m_color && 
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}
	return tmpVec.size() >= 3;
}

bool CardManager::CheckCanPeng(CardVector& handcard, const Card* outCard, bool bzhuanzhuan) const
{
	if (NULL == outCard)
	{
		return false;
	}

	if (bzhuanzhuan)	// 红中癞子
	{
		if (outCard->m_color == CARD_COLOR_JIAN && outCard->m_number == 1)
		{ 
			return false;
		}
	}

	CardVector tmpVec;
	for(Lsize i = 0 ;i < handcard.size(); ++i)
	{
		const Card* pcard = handcard[i];
		if(pcard->m_color == outCard->m_color && 
			pcard->m_number == outCard->m_number)
		{
			tmpVec.push_back(pcard);
		}
	}

	return (tmpVec.size() >= 2);
}

bool CardManager::CheckXiaoqidui(const CardVector& handcard, const Card* outCard, bool isGetCard, Lint ghostType, const std::vector<CardValue>& ghostCard, bool zimoType, Lint& special, bool& isQingQidui) const
{

	if (outCard && !isGetCard)
	{
		CardValue card;
		card.m_color = outCard->m_color;
		card.m_number = outCard->m_number;
		std::vector<CardValue>::const_iterator cit_ghost = std::find(ghostCard.begin(), ghostCard.end(), card);
		if (cit_ghost != ghostCard.end())
		{
			return false;	//不能胡别人打的鬼牌
		}
	}

	CardVector all = handcard;
	if (isGetCard )
	{
		if(outCard)
		{
			all.push_back(outCard);
		}
	}
	//计算癞子数，并从手牌里剔除
	int ghostNum = 0;
	if (ghostType != PT_GHOSTCARDNO)
	{
		CardVector::iterator it_all = all.begin();
		for ( ; it_all != all.end(); )
		{
			CardValue card;
			card.m_color = (*it_all)->m_color;
			card.m_number = (*it_all)->m_number;
			std::vector<CardValue>::const_iterator cit_ghost = std::find(ghostCard.begin(), ghostCard.end(), card);
			if (cit_ghost == ghostCard.end())
			{
				++it_all;
			}
			else
			{
				ghostNum++;
				it_all = all.erase(it_all);
			}
		}
	}

	if (!isGetCard && !zimoType && NULL != outCard) //别人打牌且非自摸模式
	{
		all.push_back(outCard);
	}

	int cardsNum = all.size();
	gCardMgr.SortCard(all);

	CardVector one,tow,three,four;
	GetSpecialOneTwoThreeFour(all,one,tow,three,four);
	int singleNum = one.size() + three.size() / 3;
	int handcardNum = cardsNum + ghostNum;

	Lint remainGhostNum = ghostNum - singleNum;
	if (remainGhostNum >= 0 && handcardNum == 14)
	{
		//查找豪华数量
		special = four.size() / 4 + three.size() / 3;
		Lint twoNum = tow.size() / 2 + one.size();
		Lint havePairGhostNum = remainGhostNum / 2;
		
		Lint makepairNum = twoNum > havePairGhostNum ? havePairGhostNum : twoNum;

		special += makepairNum;
		remainGhostNum = remainGhostNum - makepairNum * 2;
		if (remainGhostNum >= 4)
		{
			special += remainGhostNum / 4;
		}

		//检查是否是清七对
		bool bsame = IsSameColor(all);
		bool havezi = HaveZi(all);
		if (bsame && !havezi)
		{
			isQingQidui = true;
		}
		return true;
	}
	return false;
}

//检测清一色
bool CardManager::CheckQingyise(const CardVector& handcard, const CardVector& eatCard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard,const Card* outCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
	}
	SortCard(tmp);

	if(tmp.empty())
	{
		return false;
	}

	Lint color = tmp.front()->m_color;
	if (color == CARD_COLOR_FENG || color == CARD_COLOR_JIAN || color == CARD_COLOR_FLOWER)
	{
		return false;
	}
	for(Lsize i = 1 ; i < tmp.size(); ++i)
	{
		if(tmp[i]->m_color != color)
			return false;
	}
	return true;
}

bool CardManager::CheckQingPeng(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{	
	bool result = true;
	result &= CheckPengpenghu(handcard, eatCard, outCard);
	result &= CheckQingyise(handcard, eatCard, pengCard, gangcard, mgangcard, outCard);
	return result;
}

bool CardManager::CheckFourGhost(CardVector& handcard, const Card* outCard, bool is_get_card, const std::vector<CardValue>& ghostCard) const
{
	int ghostNum = 0;
	CardVector tempCard = handcard;
	if (outCard && is_get_card)
	{
		tempCard.push_back(outCard);
	}
	for (Lint i = 0; i < tempCard.size(); ++i)
	{
		std::vector<CardValue>::const_iterator cit = ghostCard.begin();
		for ( ; cit != ghostCard.end(); ++cit)
		{
			if (tempCard[i]->m_color == cit->m_color && tempCard[i]->m_number == cit->m_number)
			{
				ghostNum++;
			}
		}
	}
	return (ghostNum >= 4 ? true:false);
}

bool CardManager::CheckPingHu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	if (pengCard.size() > 0 || gangcard.size() > 0 || mgangcard.size() > 0)
	{
		return false;
	}
	CardVector all = handcard;

	if(outCard)
	{
		all.push_back(outCard);
	}
	gCardMgr.SortCard(all);
	//提出所有成对的牌
	CardVector ttow;
	for(Lsize i = 0 ; i+1 < all.size(); )
	{
		if(all[i]->m_color == all[i+1]->m_color &&
			all[i]->m_number == all[i+1]->m_number )
		{
			ttow.push_back(all[i]);
			ttow.push_back(all[i+1]);
			i += 2;
		}
		else
		{
			i += 1;
		}
	}

	//移除某个对， 剩下都能组成顺子就是平胡了
	for(Lsize i = 0 ; i < ttow.size(); i +=2)
	{
		CardVector des = all;
		gCardMgr.EraseCard(des,ttow[i]);
		gCardMgr.EraseCard(des,ttow[i+1]);

		CardVector checkh;
		Lint nCurrent = 0;
		while(nCurrent < des.size() && des.size() != 0)
		{
			if (nCurrent == 0)
			{
				checkh.push_back(des.front());
				nCurrent ++;
				continue;
			}
			if (!IsZi(des[nCurrent]) &&
				des[nCurrent]->m_color == checkh.back()->m_color &&
				des[nCurrent]->m_number == checkh.back()->m_number + 1)
			{
				checkh.push_back(des[nCurrent]);
			}
			
			//检测是否满足三个
			if (checkh.size() == 3)
			{
				for (Lint j = 0; j < checkh.size(); j ++)
				{
					gCardMgr.EraseCard(des,checkh[j], 1);
				}
				checkh.clear();
				nCurrent = 0;
			}
			else
			{
				nCurrent ++;
			}
		}
		//查看结果
		if (des.size() == 0)
		{
			return true;
		}
	}
	return false;
}

//检查大四喜
bool CardManager::CheckDasixi(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	bool result = false;
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return false;
	}

	
	CardVector one,two,three,four;
	GetSpecialOneTwoThreeFour(tmp,one,two,three,four);
	
	int colorNum = 0;
	for (int i = 0; i < three.size(); )
	{
		if (three[i]->m_color == CARD_COLOR_FENG)
		{
			colorNum++;
		} 
		i += 3;
	}
	for (int i = 0; i < four.size(); )
	{
		if (four[i]->m_color == CARD_COLOR_FENG)
		{
			colorNum++;
		} 
		i += 4;
	}

	result = colorNum == 4 ? true:false;
	return result ;
}

bool CardManager::CheckDasanyuan(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	bool result = false;
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return false;
	}

	CardVector one,two,three,four;
	GetSpecialOneTwoThreeFour(tmp,one,two,three,four);

	int colorNum = 0;
	for (int i = 0; i < three.size(); )
	{
		if (three[i]->m_color == CARD_COLOR_JIAN)
		{
			colorNum++;
		} 
		i += 3;
	}
	for (int i = 0; i < four.size(); )
	{
		if (four[i]->m_color == CARD_COLOR_JIAN)
		{
			colorNum++;
		} 
		i += 4;
	}
	result = colorNum == 3 ? true:false;
	return result;
}

//九莲宝灯
bool CardManager::CheckJiulianbaodeng(CardVector& handcard, const Card* outCard) const
{
	if (NULL == outCard || handcard.size() != 13)
	{
		return false;
	}

	Lint color = handcard[0]->m_color;
	for (Lint i = 0; i < handcard.size(); ++i)
	{
		Lint num = GetJiulianbaodengNum(i);
		if (handcard[i]->m_color != color || handcard[i]->m_number != num)
		{
			return false;
		}
	}
	return true;
}

int CardManager::GetJiulianbaodengNum(const Lint& pos) const
{
	int num = 0;
	if (pos > 13 || pos < 0)
	{
		return 0;
	}
	switch(pos)
	{
		case 0:
		case 1:
		case 2:
			num = 1;
			break;
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
			num = pos - 1;
			break;
		case 10:
		case 11:
		case 12:
			num = 9;
			break;
		default:
			break;
	}
	return num;
}

Lint CardManager::GetLuxuryqiduiType(Lint special, const OperateState& oper) const
{
	Lint luxuryqiduiType = HU_QIDUI;
	if (special == 3 && oper.m_tluxuryqidui)
	{
		luxuryqiduiType = HU_TLUXURYQIDUI;
	}
	else if(special >= 2 && oper.m_dluxuryqidui)
	{
		luxuryqiduiType = HU_DLUXURYQIDUI;
	}
	else if (special >=1 && oper.m_luxuryqiduiType)
	{
		luxuryqiduiType = HU_LUXURYQIDUI;
	}
	else
	{
		luxuryqiduiType = HU_QIDUI;
	}
	return luxuryqiduiType;
}

void CardManager::GetSpecifyColorCards(const CardVector& totalCards, CardVector& specifyCards, CardVector& restCards, const Lint& colorType) const
{
	CardVector::const_iterator cit = totalCards.begin();
	for ( ; cit != totalCards.end(); ++cit)
	{
		if ((*cit)->m_color == colorType)
		{
			specifyCards.push_back(*cit);
		}
		else
		{
			restCards.push_back(*cit);
		}
	}
}

bool CardManager::CheckJieyangCardTypeCanJiepao(const std::vector<Lint>& huVec, Lint nTingFanshu) const
{
	std::vector<Lint>::const_iterator cit = huVec.begin();
	for ( ; cit != huVec.end(); ++cit)
	{
		Lint huType = *cit;
		if (HU_QINGEIGHTEENARHATS == huType || HU_QINGDLUXURYQIDUI == huType || HU_EIGHTEENARHATS == huType
				|| HU_QINGYAOJIU == huType || HU_SHISANYAO == huType || HU_DLUXURYQIDUI == huType || HU_ZIYISE == huType)
		return true;
	}
	return false;
}

bool CardManager::CheckJieyangCardTypeCanTingTenFan(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, OperateState& gameInfo, Lint ghostType) const
{
	//检查是否可以胡十三幺
	if (CheckTingShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, gameInfo.m_ghostCard))
	{
		return true;
	}
	//检查是否可以听双豪华七对
	Lint nSpecial = 0;
	bool isQingQidui = false;
	bool bQidui = CheckTingXiaoqidui(handcard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, nSpecial, isQingQidui);
	if (bQidui && nSpecial >= 2)
	{
		return true;
	}
	//听十八罗汉
	if (CheckTingEighteenarhtsType(handcard, pengCard, agangCard, mgangCard))
	{
		return true;
	}

	bool bTingDuidui = CheckTingDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, gameInfo.m_ghostCard);
	//听纯幺九
	if (CheckTingQingyaojiuGhostType(bTingDuidui, bQidui, handcard, pengCard, agangCard, mgangCard, gameInfo.m_ghostCard))	
	{
		return true;
	}
	//听国字
	if (CheckTingAllWindGhostType(bTingDuidui, bQidui, handcard, pengCard, agangCard, mgangCard, gameInfo.m_ghostCard))	//国字
	{
		return true;
	}

	return false;
}

bool CardManager::CheckShisanyao(CardVector& handcard, const Card* outCard) const
{
	CardVector tmp = handcard;
	if (outCard)
	{
		tmp.push_back(outCard);
	}
	SortCard(tmp);
	EarseSameCard(tmp);
	if (tmp.size() < 13)
	{
		return false;		//手牌不够十三张，肯定不是十三
	}
	for (Lint i = 0; i < 13; i ++)
	{
		if (tmp[i]->m_color != m_ShiSanYiaoVec[i]->m_color || tmp[i]->m_number != m_ShiSanYiaoVec[i]->m_number)
		{
			return false;
		}
	}
	return true;
}

bool CardManager::CheckShisanyaoGhostType( CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCards) const
{
	CardVector handcard_tmp = handcard;
	if (outCard)
	{
		handcard_tmp.push_back(outCard);
	}

	if (handcard_tmp.size() != 14)
	{
		return false;
	}
	
	CardVector::const_iterator it_shisanyao = m_ShiSanYiaoVec.begin();
	for ( ; it_shisanyao != m_ShiSanYiaoVec.end(); ++it_shisanyao)
	{
		CardVector::const_iterator it_hand = handcard_tmp.begin();
		for ( ; it_hand != handcard_tmp.end(); )
		{
			if (IsSame(*it_hand, *it_shisanyao))
			{
				it_hand = handcard_tmp.erase(it_hand);
				break;
			}
			else
			{
				++it_hand;
			}
		}
	}

	for (Lint i = 0; i < ghostCards.size(); ++i)
	{
		Card card;
		card.m_color = ghostCards[i].m_color;
		card.m_number = ghostCards[i].m_number;
		EraseCard(handcard_tmp, card); 
	}
	
	if (handcard_tmp.size() > 1)
	{
		return false;
	}
	if (handcard_tmp.size() == 1)
	{
		CardVector::const_iterator it_remind_begin = handcard_tmp.begin();
		CardVector::const_iterator it_shisanyao = m_ShiSanYiaoVec.begin();
		for ( ; it_shisanyao != m_ShiSanYiaoVec.end(); ++it_shisanyao)
		{
			if (IsSame(*it_remind_begin, *it_shisanyao))
			{
				return true;
			}
		}
		if (it_shisanyao == m_ShiSanYiaoVec.end())
		{
			return false;
		}
	}
	return true;
}

bool CardManager::CheckQingyaojiu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return false;
	}

	for(Lsize i = 0 ; i < tmp.size(); ++i)
	{
		if(!IsNineOne(tmp[i]) || IsZi(tmp[i]))
			return false;
	}

	return true;
}

bool CardManager::CheckXiaosixi(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return false;
	}
	CardVector one,two,three,four;
	GetSpecialOneTwoThreeFour(tmp,one,two,three,four);

	int num = 0;
	for (int i = 0; i < three.size(); ++i)
	{
		if (three[i]->m_color == CARD_COLOR_FENG)
		{
			num++;
		}
	}
	for (int i = 0; i < four.size(); ++i)
	{
		if (four[i]->m_color == CARD_COLOR_FENG)
		{
			num++;
		}
	}
	if (num < 9)			//刻里至少有9个风牌
	{
		return false;
	}

	for (int i = 0; i < two.size(); ++i)
	{
		if (two[i]->m_color == CARD_COLOR_FENG)
		{
			return true;
		}
	}
	return false;
}

//检查字一色
bool CardManager::CheckZiyise(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	if (CheckPengpenghu( handcard, eatCard, outCard))
	{
		CardVector tmp = handcard;
		tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
		tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
		tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
		for(Lsize i = 0 ; i < tmp.size(); ++i)
		{
			if(!IsZi(tmp[i]))
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

//检查七小队和碰碰胡 字一色
bool CardManager::CheckQixiaoduiOrPengPengHuZiyise(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const bool isQiXiaoDui, const bool isPengPenghu) const
{
	if (isQiXiaoDui || isPengPenghu)
	{
		CardVector tmp = handcard;
		tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
		tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
		tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
		for (Lsize i = 0; i < tmp.size(); ++i)
		{
			if (!IsZi(tmp[i]))
			{
				return false;
			}
		}
		return true;
	}

	return false;
}




bool CardManager::CheckXiaosanyuan(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return false;
	}

	int num = 0;
	for (int i = 0; i < tmp.size(); ++i)
	{
		if (tmp[i]->m_color == CARD_COLOR_JIAN)
		{
			num++;
		}
	}

	if (num < 8)			//箭牌必须大于等于8个
	{
		return false;
	}

	CardVector one, two, three, four;
	GetSpecialOneTwoThreeFour(tmp, one, two, three, four);
	for (int i = 0; i < two.size(); ++i)
	{
		if (two[i]->m_color == CARD_COLOR_JIAN)
		{
			return true;
		}
	}
	return false;
}

bool CardManager::CheckHunyaojiu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	if (CheckPengpenghu( handcard, eatCard, outCard))
	{
		CardVector tmp = handcard;
		tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
		tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
		tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
		for(Lsize i = 0 ; i < tmp.size(); ++i)
		{
			if(!IsNineOne(tmp[i]) && !IsZi(tmp[i]))
			{
				return false;
			}
		}
		return true;
	}

	return false;
}

bool CardManager::CheckHuizhouFloweryaojiu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, bool& qingyise) const
{
	if (CheckPengpenghu(handcard, eatCard, outCard))
	{
		CardVector tmp = handcard;
		tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
		tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
		tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());

		Lint color = 0;
		qingyise = true;
		for (Lsize i = 0; i < tmp.size(); ++i)
		{
			if (!IsNineOne(tmp[i]) && !IsZi(tmp[i]))
			{
				return false;
			}

			if (color == 0 && IsNineOne(tmp[i]))
			{
				color = tmp[i]->m_color;
			}
			else if(color != tmp[i]->m_color && IsNineOne(tmp[i]) && !IsZi(tmp[i]))
			{
				qingyise = false;
			}
		}
		return true;
	}

	return false;
}

bool CardManager::CheckMenQing(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	if(eatCard.size() || pengCard.size() || mgangcard.size())
	{
		return false;
	}
	return true;
}

//检测乱将胡
bool CardManager::CheckLuanjianghu(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	CardVector tmp = handcard;
	if(outCard)
	{
		tmp.push_back(outCard);
	}
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(tmp.empty())
	{
		return false;
	}

	for(Lsize i = 0 ; i < tmp.size(); ++i)
	{
		if(!(tmp[i]->m_number == 2 || tmp[i]->m_number == 5 ||
			tmp[i]->m_number == 8))
		{
			return false;
		}
	}
	return true;
}

//检测碰碰胡
bool CardManager::CheckPengpenghu(CardVector& handcard,CardVector& eatCard,const Card* outCard) const
{
	if(eatCard.size())
	{
		return false;
	}

	CardVector check,one,tow,three,four;
	check = handcard;
	if(outCard)
	{
		check.push_back(outCard);
		SortCard(check);
	}

	GetSpecialOneTwoThreeFour(check,one,tow,three,four);

	if(one.size())
		return false;

	if(tow.size() != 2)
		return false;

	return true;
}

bool CardManager::CheckHunYiSe(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	CardVector tmp = handcard;
	if(outCard)
	{
		tmp.push_back(outCard);
	}
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(tmp.empty())
	{
		return false;
	}

	//CardVector::const_iterator cit = tmp.begin();
	CardVector::iterator cit = tmp.begin();
	bool haveZi = false;
	for ( ; cit != tmp.end();)
	{
		if ((*cit)->m_color > CARD_COLOR_SUO)
		{
			cit = tmp.erase(cit);
			haveZi = true;
		}
		else
		{
			++cit;
		}
	}
	if (haveZi)
	{
		return IsSameColor(tmp);
	}
	return false;
}

bool CardManager::CheckHunPeng(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	bool result = true;
	result &= CheckPengpenghu(handcard, eatCard, outCard);
	result &= CheckHunYiSe(handcard, eatCard, pengCard, gangcard, mgangcard, outCard);
	return result;
}

//检测抢杠胡
bool CardManager::CheckQiangganghu(OperateState& gameInfo) const
{
	if (!gameInfo.m_thinkGang && (gameInfo.m_cardState == THINK_OPERATOR_MGANG))
	{
		return true;
	}
	return false;
}

//检测抢杠胡
bool CardManager::CheckMinggangkeqiang(OperateState& gameInfo) const
{
	if (!gameInfo.m_thinkGang && (gameInfo.m_cardState == THINK_OPERATOR_MGANG))
	{
		return true;
	}
	return false;
}
//检测全球人
bool CardManager::CheckQuanqiuren(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	CardVector check;
	check = handcard;
	if(outCard)
	{
		check.push_back(outCard);
	}
	if (check.size() != 2)
	{
		return false;
	}
	return ((CheckLuanjianghu(handcard,eatCard,pengCard,gangcard,mgangcard,outCard) || 
		(check[0]->m_color == check[1]->m_color && check[0]->m_number == check[1]->m_number)));
}

bool CardManager::CheckFlowerMatch(CardVector& handcard, const Card* getCard, bool is_get_card, bool& bmatchKe, bool& bmatchNomal, bool& bmatchQidui, bool shisanyaohu, bool qixiaohuihu) const
{
	bool result = false;
	if (NULL == getCard)
	{
		return result;
	}

	std::vector<Lint> card_vector(60, 0);
	Lint Lai_num = 0;
	CardVector noghostCards;
	//癞子,分数量统计
	for(int x = 0; x < handcard.size(); x++)
	{
		if (handcard[x]->m_color == CARD_COLOR_FLOWER)
		{
			Lai_num++;
		}
		else
		{
			card_vector[(handcard[x]->m_color-1)*10+(handcard[x]->m_number)*1]++;
			noghostCards.push_back(handcard[x]);
		}	
	}

	//先判断十三幺与七小队
	if (shisanyaohu || qixiaohuihu)
	{
		bool match = CheckSpecialFlowerMatchType(noghostCards, getCard, Lai_num, shisanyaohu, qixiaohuihu);
		if (match)
		{
			if (qixiaohuihu)
			{
				bmatchQidui = true;
			}
			result = true;
		}
	}
	
	//普通胡
	if (Lai_num < 1)
	{
		return false;
	}
	std::vector<Lint> tmp_cards(card_vector);
	bmatchKe = explorer_ke_function(tmp_cards, 0, Lai_num - 1);
	if (bmatchKe)
	{
		bmatchNomal = true;
		result = true;
	}
	else
	{
		bmatchNomal = explorer_zheng_function(tmp_cards, 0, Lai_num - 1);
		if (bmatchNomal)
		{
			result = true;
		}
	}
	
	return result;
}

//杠上开花
bool CardManager::CheckGangshangkaihua(const OperateState& gameInfo) const
{
	//判断是不是自已杠的
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}

//杠上炮
bool CardManager::CheckGangshangpao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard, OperateState& gameInfo) const
{

	//杠上炮，判断是不是别人的杠
	if (gameInfo.m_MePos != gameInfo.m_playerPos && gameInfo.m_thinkGang)
	{
		return true;
	}

	return false;
}
//海底捞
bool CardManager::CheckHaiDiLao(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard, OperateState& gameInfo) const
{
	if (gameInfo.m_MePos == gameInfo.m_playerPos && gameInfo.m_deskState == DESK_PLAY_END_CARD)
	{
		return true;
	}
	return false;
}
bool CardManager::CheckNoGhost2Times(const OperateState& gameInfo) const
{
	  return  gameInfo.m_isNoGhost2Times && gameInfo.m_ghostType != PT_GHOSTCARDNO;
}

bool CardManager::CheckCanChi(CardVector& handcard, const Card* outCard, CardVector& vec) const
{
	if(handcard.size() < 2)
		return false;
	if (outCard->m_color == CARD_COLOR_FENG || outCard->m_color == CARD_COLOR_JIAN || outCard->m_color == CARD_COLOR_FLOWER)
	{
		return false;
	}
	CardVector one = handcard;
	SortCard(one);
	EarseSameCard(one);
	EraseCard(one, outCard, CARD_COLOR_FENG);

	for(Lint i = 0 ; i+1 < one.size(); ++i)
	{
		CardVector tmp;
		tmp.push_back(one[i]);
		tmp.push_back(one[i+1]);
		tmp.push_back(outCard);
		SortCard(tmp);
		if(IsContinue(tmp))
		{
			vec.push_back(one[i]);
			vec.push_back(one[i+1]);
		}
	}

	return true;
}
bool CardManager::CheckAnGang(CardVector& handcard,  CardVector& vec, bool zhuanzhuan) const
{
	bool result = false;
	if(handcard.size() < 4)
		return false;
	CardVector tmp = handcard;
	SortCard(tmp);
	for(Lsize i = 0 ;i+3 < tmp.size(); ++i)
	{
		const Card* pcarda = tmp[i];
		const Card* pcardb = tmp[i+3];

		if (zhuanzhuan)
		{
			if (pcarda->m_color == CARD_COLOR_JIAN && pcarda->m_number == 1)
			{
				continue;
			}
			if (pcarda->m_color == pcardb->m_color && pcarda->m_number == pcardb->m_number)
			{
				vec.push_back(pcarda);
				result = true;
			}
		}
		else if(pcarda->m_color == pcardb->m_color && pcarda->m_number == pcardb->m_number)
		{
			vec.push_back(pcarda);
			result = true;
		}
	}
	return result;
}

bool CardManager::CheckMGang(CardVector& handcard,CardVector& pengCard,  CardVector& vec, bool zhuanzhuan ) const
{
	bool result = false;
	for(Lsize i = 0 ; i < pengCard.size(); i+=3)
	{
		const Card* pcard = pengCard[i];
		for(Lsize j = 0 ; j < handcard.size(); ++j)
		{
			if (zhuanzhuan)
			{
				if (pcard->m_color == CARD_COLOR_JIAN && pcard->m_number == 1)
				{
					continue;
				}
				if (pcard->m_color ==  handcard[j]->m_color && pcard->m_number ==  handcard[j]->m_number)
				{
					vec.push_back(pcard);
					result = true;
				}
			}
			else if(pcard->m_color == handcard[j]->m_color && pcard->m_number ==  handcard[j]->m_number)
			{
				vec.push_back(pcard);
				result = true;
			}
			/*if(pcard->m_number == handcard[j]->m_number &&
				pcard->m_color == handcard[j]->m_color)
			{
				vec.push_back(pcard);
				result = true;
			}*/
		}
	}
	return result;
}


bool CardManager::IsContinue(CardVector& result) const
{
	if(result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for(Lsize i = 1; i < result.size(); ++i)
	{
		if(result[i]->m_number != number+i || color != result[i]->m_color)
			return false;
	}

	return true;
}

bool CardManager::IsSame(const CardVector& result) const
{
	if(result.empty())
	{
		return false;
	}

	Lint number = result.front()->m_number;
	Lint color = result.front()->m_color;
	for(Lsize i = 1; i < result.size(); ++i)
	{
		if(result[i]->m_number != number || color != result[i]->m_color)
			return false;
	}

	return true;
}
bool CardManager::IsSame(const Card* c1, const Card* c2) const
{
	if (c1 == NULL || c2 == NULL)
	{
		return false;
	}
	return c1->m_color==c2->m_color&&c1->m_number==c2->m_number;
}

bool CardManager::IsZi(const Card* c) const
{
	if (NULL == c)
	{
		return false;
	}
	return (c->m_color == CARD_COLOR_FENG || c->m_color == CARD_COLOR_JIAN);
}

bool CardManager::IsHongzhong(const Card* c) const
{
	if (NULL == c)
	{
		return false;
	}
	return ( c->m_color == CARD_COLOR_JIAN && c->m_number == 1);
}

bool CardManager::IsFlower(const Card* c) const
{
	if (NULL == c)
	{
		return false;
	}
	return c->m_color == CARD_COLOR_FLOWER;
}

bool CardManager::IsNineOne(const Card* c) const
{
	if (NULL == c)
	{
		return false;
	}
	return c->m_number == 1 || c->m_number == 9;
}

bool CardManager::GetSpecialOneTwoThreeFour(CardVector& src,CardVector& one,CardVector& two,CardVector& three,CardVector& four) const
{
	if(src.empty())
	{
		return false;
	}

	Lsize i = 0 ;
	Lint number = 0,color = 0 ,length = 0 ;
	for(; i < src.size(); ++i)
	{
		const Card* pCard = src[i];
		if(number == pCard->m_number && color == pCard->m_color)
		{
			length += 1;
		}
		else
		{
			if(length == 1)
			{
				one.push_back(src[i-1]);
			}
			else if(length == 2)
			{
				two.push_back(src[i-2]);
				two.push_back(src[i-1]);
			}
			else if(length == 3)
			{
				three.push_back(src[i-3]);
				three.push_back(src[i-2]);
				three.push_back(src[i-1]);
			}
			else if(length == 4)
			{
				four.push_back(src[i-4]);
				four.push_back(src[i-3]);
				four.push_back(src[i-2]);
				four.push_back(src[i-1]);
			}
			length = 1;
			number = pCard->m_number;
			color = pCard->m_color;
		}
	}

	if(length == 1)
	{
		one.push_back(src[i-1]);
	}
	else if(length == 2)
	{
		two.push_back(src[i-2]);
		two.push_back(src[i-1]);
	}
	else if(length == 3)
	{
		three.push_back(src[i-3]);
		three.push_back(src[i-2]);
		three.push_back(src[i-1]);
	}
	else if(length == 4)
	{
		four.push_back(src[i-4]);
		four.push_back(src[i-3]);
		four.push_back(src[i-2]);
		four.push_back(src[i-1]);
	}

	return true;
}

const Card* CardManager::GetCard(Lint color,Lint number) const
{
	return &m_card[(color-1)*4+(number-1)*12];
}

bool CardManager::IsSameColor(const CardVector& result) const
{
	CardVector::const_iterator cit = result.begin();
	if (cit == result.end())
	{
		return false;
	}

	Lint color = (*cit)->m_color;
	cit++;
	for ( ; cit != result.end(); ++cit)
	{
		if (color != (*cit)->m_color)
		{
			return false;
		}
	}
	return true;
}

bool CardManager::HaveZi(const CardVector& cards) const
{
	CardVector::const_iterator cit = cards.begin();
	 for(  ;cit != cards.end(); ++cit)
	{
		 if ((*cit)->m_color >= CARD_COLOR_FENG)
		 {
			 return true;
		 }
	}
	return false;
}

bool CardManager::IsHaveGang(CardVector& src) const
{
	CardVector one, two, three, four;
	GetSpecialOneTwoThreeFour(src, one, two, three, four);
	if (four.size() > 0)
	{
		return true;
	}
	return false;
}

Lint CardManager::GetJianKeNum(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard) const
{
	int jianNum = 0;
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return jianNum;
	}

	CardVector one,two,three,four;
	GetSpecialOneTwoThreeFour(tmp,one,two,three,four);
	
	for (int i = 0; i < three.size();)
	{
		if (three[i]->m_color == CARD_COLOR_JIAN)
		{
			jianNum++;
		}
		i += 3;
	}
	for (int i = 0; i < four.size();)
	{
		if (four[i]->m_color == CARD_COLOR_JIAN)
		{
			jianNum++;
		}
		i += 4;
	}
	return jianNum;
}

Lint CardManager::GetOutCardsSpecifyColorKeNum(const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Lint& colorType) const
{
	int jianNum = 0;
	CardVector tmp;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	
	CardVector one, two, three, four;
	GetSpecialOneTwoThreeFour(tmp, one, two, three, four);

	for (int i = 0; i < three.size();)
	{
		if (three[i]->m_color == colorType)
		{
			jianNum++;
		}
		i += 3;
	}
	for (int i = 0; i < four.size();)
	{
		if (four[i]->m_color == colorType)
		{
			jianNum++;
		}
		i += 4;
	}
	return jianNum;
}

void CardManager::CheckFengCardKe(CardVector& handcard,CardVector& eatCard,CardVector& pengCard,CardVector& gangcard,CardVector& mgangcard,const Card* outCard, int fengWei, int fengQuan, std::vector<int>& vetHu) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),eatCard.begin(),eatCard.end());
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	if(tmp.empty())
	{
		return;
	}

	CardVector one,two,three,four;
	GetSpecialOneTwoThreeFour(tmp,one,two,three,four);
	bool haveFengWei = false;
	bool haveFengQuan = false;
	for (int i = 0; i < three.size(); ++i)
	{
		if ((three[i]->m_number == fengWei + 1) && three[i]->m_color == CARD_COLOR_FENG)
		{
			haveFengWei = true;
		}
		if ((three[i]->m_number == fengQuan + 1) && three[i]->m_color == CARD_COLOR_FENG)
		{
			haveFengQuan = true;
		}
	}

	if (haveFengWei)
	{
		vetHu.push_back(HU_FENGPAIKEWEI);
	}

	if (haveFengQuan)
	{
		vetHu.push_back(HU_FENGPAIKEQUAN);
	}
	return ;
}

bool CardManager::CheckQishouLaizi(CardVector& handcard) const
{
	int lanzi_num=0;
	for(int x=0;x<handcard.size();x++)
	{
		if(handcard[x]!=NULL)
		{
			if(handcard[x]->m_color==5)
				lanzi_num++;
		}
	}
	if(lanzi_num==4)
	{
		return true;
	}
	else{
		return false;
	}
}

bool CardManager::CheckQishouHongzhong(const CardVector& handcard, const Card* outCard) const
{
	CardVector tmp = handcard;
	if (outCard)
	{
		tmp.push_back(outCard);
	}

	int lanzi_num = 0;
	for (int x = 0; x < tmp.size(); x++)
	{
		if (tmp[x] != NULL)
		{
			if (tmp[x]->m_color == CARD_COLOR_JIAN && tmp[x]->m_number == 1)
				lanzi_num++;
		}
	}
	if (lanzi_num == 4)
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool CardManager::Ghost_CheckHu(CardVector handcard,const Card* curCard,bool needJiang,bool is_get_card, const std::vector<CardValue>& ghostCard, Lint ghostType) const
{
	if (ghostType == PT_GHOSTCARDNO)
	{
		return CheckHu(handcard,curCard,false,is_get_card);
	}

	if(curCard && !is_get_card)
	{
		CardValue card;
		card.m_color = curCard->m_color;
		card.m_number = curCard->m_number;
		std::vector<CardValue>::const_iterator cit_ghost = std::find(ghostCard.begin(), ghostCard.end(), card);
		if (cit_ghost != ghostCard.end())
		{
			return false;	//不能胡别人打的鬼牌
		}
	}

	if(curCard)
	{
		handcard.push_back(curCard);
	}

	std::vector<Lint> card_vector(60,0);
	Lint Lai_num = 0;

	//第一步获取癞子,分数量统计
	for(int x = 0; x < handcard.size(); x++)
	{
		CardValue card;
		card.m_color = handcard[x]->m_color;
		card.m_number = handcard[x]->m_number;
		std::vector<CardValue>::const_iterator cit_ghost = std::find(ghostCard.begin(), ghostCard.end(), card);
		if (cit_ghost != ghostCard.end())
		{
			Lai_num++;
		}
		else
		{
			card_vector[(handcard[x]->m_color-1)*10+(handcard[x]->m_number)*1]++;
		}	
	}

	bool juge_lai_jiang = true;
	for(int x=0; x < card_vector.size(); x++)
	{
		if(x % 10 == 0)
		{
			x++;
		}
		if(card_vector[x] > 1)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x] -= 2;
			if(explorer_zheng_function(tmp_cards, 0, Lai_num))
			{
				return true;
			}
		}
		else if(card_vector[x] == 1 && Lai_num > 0)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x] -= 1;
			if(explorer_zheng_function(tmp_cards, 0, Lai_num - 1))
			{
				return true;
			}
		}
		else
		{
			if(Lai_num > 1 && juge_lai_jiang)
			{
				juge_lai_jiang = false;
				if(explorer_zheng_function(card_vector, 0, Lai_num - 2))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CardManager::explorer_zheng_function(std::vector<Lint> cards, Lint index, Lint Lai_num) const
{
	if(index < 50)
	{
		if(index % 10 == 0)
		{
			index++;
		}

		if(cards[index] > 0)
		{
			std::vector<Lint> tmp_cards(cards);
			Lint ke_dif = Lai_num - judge_ke(tmp_cards,index);
			if(ke_dif>=0)
			{
				if(explorer_zheng_function(tmp_cards,index,ke_dif))
					return true;
			}

			if(index % 10 < 8 && index < 30)		//万筒条才可以组成顺
			{
				std::vector<Lint> tmp_cards2(cards);
				Lint shun_dif = Lai_num-judge_shun(tmp_cards2,index);
				if(shun_dif>=0)
				{
					if(explorer_zheng_function(tmp_cards2,index,shun_dif))
						return true;
				}
			}
			return false;
		}
		else
		{
			if(Lai_num > 0 && index % 10 < 8 && index < 30)
			{
				std::vector<Lint> tmp_cards(cards);
				Lint shun_dif = Lai_num-judge_shun(tmp_cards,index);
				if(shun_dif>=0)
				{
					if(explorer_zheng_function(tmp_cards,index,shun_dif))
					{
						return true;
					}
				}
			}
			index++;
			if(explorer_zheng_function(cards,index,Lai_num))
			{
				return true;
			}
			return false;
		}
	}
	else
	{
		return true;
	}
}

bool CardManager::explorer_ke_function(std::vector<Lint> cards, Lint index, Lint Lai_num) const
{
	if (index < 50)
	{
		if (index % 10 == 0)
		{
			index++;
		}

		if (cards[index] > 0)
		{
			std::vector<Lint> tmp_cards(cards);
			Lint ke_dif = Lai_num - judge_ke(tmp_cards, index);
			if (ke_dif >= 0)
			{
				if (explorer_ke_function(tmp_cards, index, ke_dif))
					return true;
			}
			return false;
		}
		else
		{
			index++;
			if (explorer_ke_function(cards, index, Lai_num))
			{
				return true;
			}
			return false;
		}
	}
	else
	{
		return true;
	}
}

Lint CardManager::judge_ke(std::vector<Lint>& targe_vector,Lint index) const
{
	Lint Lai_num = 0;
	switch(targe_vector[index])
	{
	case 4:
	case 3:
		{
			targe_vector[index]-=3;
			Lai_num=0;
			break;
		}
	case 2:
		{
			targe_vector[index]-=2;
			Lai_num=1;
			break;
		}
	case 1:
		{
			targe_vector[index]-=1;
			Lai_num=2;
			break;
		}
	case 0:
		{
			Lai_num=3;
			break;
		}
	default:
		Lai_num = 100;
		break;
	}

	return Lai_num;
}

Lint CardManager::judge_shun(std::vector<Lint>& targe_vector,Lint index) const
{
	Lint Lai_num =3;
	if(targe_vector[index]>0)
	{
		targe_vector[index]--;
		Lai_num--;
	}
	if(targe_vector[index+1]>0)
	{
		targe_vector[index+1]--;
		Lai_num--;
	}
	if(targe_vector[index+2]>0)
	{
		targe_vector[index+2]--;
		Lai_num--;
	}
	return Lai_num;
}

bool CardManager::CanHongzhongPengGangType(const OperateState& gameinfo) const
{
	if (gameinfo.m_GameType == HuNanMaJing || gameinfo.m_GameType == MeizhouHongzhonbao)
	{
		return true;
	}
	return false;
}

bool CardManager::IsHaveRedDragon(const CardVector& src) const
{
	CardVector::const_iterator cit = src.begin();
	for (; cit != src.end(); ++cit )
	{
		if ((*cit)->m_color == CARD_COLOR_JIAN && (*cit)->m_number == 1 )	//红中
		{
			return true;
		}
	}

	return false;
}

bool CardManager::IsHaveFlower(const CardVector& src) const
{
	CardVector::const_iterator cit = src.begin();
	for (; cit != src.end(); ++cit)
	{
		if ((*cit)->m_color == CARD_COLOR_FLOWER)	//红中
		{
			return true;
		}
	}
	return false;
}

bool CardManager::CheckTuiDaohuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;
	Lint special = 0;
	bool isQingQidui = false;
	if (gameInfo.m_QiXiaoDui && CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, special, isQingQidui))		//检查七小队
	{
		vec.push_back(HU_QIDUI);
		result = true;
	}

	if (ghostType != PT_GHOSTCARDNO && ghostType != PT_GHOSTCARDFLOPTWO)		//检查四个鬼牌
	{
		if (CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard))
		{
			result = true;
		}
	}

	if(!result)
	{
		if (!Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
		{
			return false;
		}
	}
	//杠上开花
	if (CheckGangshangkaihua(gameInfo))
	{
		vec.push_back(HU_GANGSHANGKAIHUA);
	}
	//抢杠胡
	if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
	{
		auto huType = gameInfo.m_hasDianGang ? HU_QIANGMINGGANGHU : HU_QIANGGANGHU;
		vec.push_back(huType);
	}
	else
	{
		vec.push_back(HU_ZIMO);
	}
	return true;
}

bool CardManager::CheckJipinghuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;
	std::vector<Lint> vecHuType;
	int nQihuFanshu = gameInfo.m_qihuFanshu;
	int nHuType = HU_JIHU;

	//先检查十三幺 十三幺是特殊牌型，CheckHu判断不了
	bool bShisanyao = CheckShisanyao(handcard, outCard);
	if ( !bShisanyao && !CheckHu(handcard,outCard,false,isGetCard))		
	{
		return result;
	}

	//天地人胡判断
	if (gameInfo.m_canTianhu)
	{
		vecHuType.push_back(HU_TIANHU);
		nHuType = HU_TIANHU;
	}
	else if (gameInfo.m_canDihu)
	{
		vecHuType.push_back(HU_DIHU);
		nHuType = HU_DIHU;
	}
	else if (gameInfo.m_canRenhu)
	{
		vecHuType.push_back(HU_RENHU);
		nHuType = HU_RENHU;
	}

	//基础牌型判断
	if (bShisanyao)
	{
		vecHuType.push_back(HU_SHISANYAO);
		nHuType = HU_SHISANYAO;
	}
	else if (CheckJiulianbaodeng(handcard, outCard))
	{
		vecHuType.push_back(HU_JIULIANBAODENG);
		nHuType = HU_JIULIANBAODENG;
	}
	else if (CheckDasixi(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_DASIXI);
		nHuType = HU_DASIXI;
	}
	else if (CheckDasanyuan(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_DASANYUAN);
		nHuType = HU_DASANYUAN;
	}
	else if (CheckQingyaojiu(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_QINGYAOJIU);
		nHuType = HU_QINGYAOJIU;
	}
	else if (CheckZiyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_ZIYISE);
		nHuType = HU_ZIYISE;
	}
	else if (CheckXiaosixi(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_XIAOSIXI);
		nHuType = HU_XIAOSIXI;
	}
	else if (CheckXiaosanyuan(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_XIAOSANYUAN);
		nHuType = HU_XIAOSANYUAN;
	}
	else if (CheckHunyaojiu(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_HUNYAOJIU);
		nHuType = HU_HUNYAOJIU;
	}
	else if (CheckQingPeng(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_QINGPENG);
		nHuType = HU_QINGPENG;
	}
	else if (CheckQingyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_QINGYISE);
		nHuType = HU_QINGYISE;
	}
	else if (CheckHunPeng(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_HUNPENG);
		nHuType = HU_HUNPENG;
	}
	else if (CheckHunYiSe(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_HUNYISE);
		nHuType = HU_HUNYISE;
	}
	else if (CheckPengpenghu(handcard, eatCard,outCard))		
	{
		vecHuType.push_back(HU_PENGPENGHU);
		nHuType = HU_PENGPENGHU;
	}
	else if (CheckPingHu(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))		
	{
		vecHuType.push_back(HU_PING);
		nHuType = HU_PING;
	}
	else 
	{
		vecHuType.push_back(HU_JIHU);
		nHuType = HU_JIHU;
	}

	//抢杠胡
	if(CheckQiangganghu(gameInfo))
	{
		vecHuType.push_back(HU_QIANGGANGHU);
	}
	//杠上开花
	if (CheckGangshangkaihua(gameInfo))
	{
		vecHuType.push_back(HU_GANGSHANGKAIHUA);
	}
	//杠上炮
	if (CheckGangshangpao(handcard,eatCard,pengCard,agangCard,mgangCard,outCard, gameInfo))
	{
		vecHuType.push_back(HU_GANGSHANGPAO);
	}
	//海底捞
	if (CheckHaiDiLao(handcard,eatCard,pengCard,agangCard,mgangCard,outCard, gameInfo))
	{
		vecHuType.push_back(HU_HAIDILAO);
	}

	//爆胡加番
	if (nHuType <= HU_BAOHU)
	{
		//门清
		if (CheckMenQing(handcard,eatCard,pengCard,agangCard,mgangCard,outCard))
		{
			vecHuType.push_back(HU_MENQING);
		}
		//自摸
		if (isGetCard)
		{
			vecHuType.push_back(HU_ZIMO);
		}
		//三元牌
		if (true)
		{
			int num = GetJianKeNum(handcard,eatCard,pengCard,agangCard,mgangCard,outCard);
			if (1 == num)
			{
				vecHuType.push_back(HU_SANYUANPAIONE);
			}
			else if (2 == num)
			{
				vecHuType.push_back(HU_SANYUANPAITWO);
			}
		}
		//风圈、风位
		if (true)
		{
			CheckFengCardKe(handcard,eatCard,pengCard,agangCard,mgangCard,outCard, gameInfo.m_MePos, gameInfo.m_fengQuan, vecHuType);
		}
	}

	result = IsJipingFanShuCanQihu(nQihuFanshu, vecHuType);

	if (result)
	{
		vec = vecHuType;
	}
	return result;
}

Lint CardManager::GetJipingFanNumByHuType(const int hutype) const
{
	int result = 0;
	switch (hutype)
	{
	case HU_RENHU:
	case HU_DIHU:
	case HU_TIANHU:
	case HU_SHISANYAO:
	case HU_JIULIANBAODENG:
		result = 6;
		break;
	case HU_DASIXI:
		result = 6;
		break;
	case HU_DASANYUAN:
		result = 6;
		break;
	case HU_QINGYAOJIU:
		result = 6;
		break;
	case HU_ZIYISE:
		result = 6;
		break;
	case HU_XIAOSIXI:
		result = 5;
		break;
	case HU_XIAOSANYUAN:
		result = 5;
		break;
	case HU_HUNYAOJIU:
		result = 5;
		break;
	case HU_QINGPENG:
		result = 5;
		break;
	case HU_QINGYISE:
		result = 4;
		break;
	case HU_HUNPENG:
		result = 4;
		break;
	case HU_HUNYISE:
		result = 2;
		break;
	case HU_PENGPENGHU:
		result = 2;
		break;
	case HU_PING:
		result = 1;
		break;
	case HU_JIHU:
		result = 0;
		break;
	case HU_ZIMO:
		result = 1;
		break;
	case HU_GANGSHANGKAIHUA:
	case HU_GANGSHANGPAO:
	case HU_QIANGGANGHU:
	case HU_QIANGMINGGANGHU:
	case HU_HAIDILAO:
	case HU_HAIDIPAO:
		result = 3;
		break;
	case HU_MENQING:
	case HU_FENGPAIKEQUAN:
	case HU_FENGPAIKEWEI:
	case HU_SANYUANPAIONE:
		result = 1;
		break;
	case HU_SANYUANPAITWO:
		result = 2;
		break;
	default:
		result = 0;
	}
	return result;
}

Lint CardManager::GetHongkongFanFromHuType(Lint huType) const
{
	int result = 0;
	switch (huType)
	{
	case HU_TIANHU:
	case HU_DIHU:
	case HU_SHISANYAO:
	case HU_JIULIANBAODENG:
	case HU_DASIXI:
	case HU_DASANYUAN:
	case HU_QINGYAOJIU:
	case HU_EIGHTEENARHATS:
		result = 13;
		break;
	case HU_ZIYISE:
	case HU_QINGYISE:
		result = 7;
		break;
	case HU_XIAOSANYUAN:
	case HU_XIAOSIXI:
	case HU_HUNYISE:
	case HU_PENGPENGHU:
	case HU_ZIMOMENQING:
		result = 3;
		break;
	case HU_SANYUANPAITWO:
		result = 2;
		break;
	case HU_PING:
	case HU_FENGPAIKEQUAN:
	case HU_FENGPAIKEWEI:
	case HU_HAIDILAO:
	case HU_QIANGGANGHU:
	case HU_ZIMO:
	case HU_MENQING:
	case HU_GANGSHANGKAIHUA:
	case HU_ZHUANGJIA:
	case HU_SANYUANPAIONE:
		result = 1;
		break;
	case HU_JIHU:
		result = 0;
		break;
	default:
		result = 0;
	}
	return result;
}

Lint CardManager::GetMeizhouHongzhongbaoFanFromHuType(const Lint huType) const
{
	int result = 0;
	switch (huType)
	{
	case HU_TLUXURYQIDUI:
		result = 42;
		break;
	case HU_DLUXURYQIDUI:
		result = 28;
		break;
	case HU_SHISANYAO:
	case HU_TIANHU:
	case HU_DIHU:
	case HU_QINGYAOJIU:
	case HU_ZIYISE:
		result = 20;
		break;
	case HU_LUXURYQIDUI:
		result = 14;
		break;
	case HU_HUNYAOJIU:
		result = 10;
		break;
	case HU_QINGYISE:
	case HU_QIDUI:
		result = 8;
		break;
	case HU_PENGPENGHU:
	case HU_HUNYISE:
		result = 4;
		break;
	case HU_ZIMO:
	case HU_MENQING:
	case HU_XIAOHU:
		result = 2;
		break;
	case HU_WITHOUTHONGZHONG:
		result = 1;
		break;
	default:
		result = 0;
	}
	return result;
}

bool CardManager::CheckZhuanzhuanCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;
	Lint isSpecial = 0;
	bool isQingQidui = false;
	if(Ghost_CheckHu(handcard,outCard,false,isGetCard, gameInfo.m_ghostCard, ghostType))
	{
		vec.push_back(HU_XIAOHU);
		result = true;
	}
	else if (gameInfo.m_QiXiaoDui && CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, isSpecial, isQingQidui))
	{
		vec.push_back(HU_XIAOHU);
		result = true;
	}
	else if(gameInfo.m_first_turn &&CheckQishouLaizi(handcard))
	{
		vec.push_back(HU_XIAOHU);
		result = true;
	}
	return result;
}

bool CardManager::CheckOneHundredCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;

	//检查是否可以胡十三幺
	bool huShisanyao = false;
	if(CheckShisanyaoGhostType(handcard, pengCard,agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		huShisanyao = true;
		result = true;
	}
	//检查是否可以胡七对
	bool huQidui = false;
	Lint isSpecial = 0;
	bool isQingQidui = false;
	if (gameInfo.m_qiduiType && CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, isSpecial, isQingQidui))
	{
		huQidui = true;
		result = true;
	}

	bool huCommon = false;
	if (Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
	{
		huCommon = true;
		result = true;
	}
	
	if (result)
	{
		if (gameInfo.m_shisanyaoType && huShisanyao)
		{
			vec.push_back(HU_SHISANYAO);
		}
		//检查胡牌类型
		else if (isQingQidui && gameInfo.m_qingyiseType)
		{
			vec.push_back(HU_QINGQIDUI);
		}
		else if (huCommon && gameInfo.m_allwindType && CheckAllWindGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_ZIYISE);
		}
		else if (gameInfo.m_yaojiuType && !gameInfo.m_yaojiuHas1or9 && CheckYaoJiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_YAOJIU);
		}
		else if (gameInfo.m_yaojiuType && gameInfo.m_yaojiuHas1or9 && CheckYaoJiuHasOneOrNineHu(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_YAOJIU);
		}
		else if (gameInfo.m_qingyiseType && gameInfo.m_duiduihuType && CheckQingDuiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGPENG);
		}
		else if (gameInfo.m_hunyiseType && gameInfo.m_duiduihuType && CheckHunpengGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_HUNPENG);
		}
		else if (gameInfo.m_qiduiType && huQidui)
		{
			vec.push_back(HU_QIDUI);
		}
		else if (gameInfo.m_qingyiseType && CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGYISE);
		}
		else if (gameInfo.m_hunyiseType && CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_HUNYISE);
		}
		else if (gameInfo.m_duiduihuType && CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_PENGPENGHU);
		}
	}

	bool bspecial = IsOneHundredSpecialHuType(vec);
	if (!result || !bspecial)
	{
		//检查是否可以四鬼胡
		if (gameInfo.m_canfourghosthu)
		{
			if (CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_FOURGHOST);
				result = true;
			}
		}
	}

	if (result)
	{
		//杠上开花
		if (CheckGangshangkaihua(gameInfo))
		{
			vec.push_back(HU_GANGSHANGKAIHUA);
		}

		//可以胡，检查是否可以抢杠胡
		if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
		{
			auto huType = gameInfo.m_hasDianGang ? HU_QIANGMINGGANGHU : HU_QIANGGANGHU;
			vec.push_back(huType);
		}
		else
		{
			vec.push_back(HU_ZIMO);
		}
	}
	return result;
}

bool CardManager::CheckCustomTuiDaoCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;

	//检查是否可以胡十三幺
	bool bHuShisanyao = false;
	if(CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		if (gameInfo.m_shisanyaoType)
		{
			vec.push_back(HU_SHISANYAO);
			bHuShisanyao = true;
		}
		result = true;
	}
	//检查是否可以普通胡
	if (!result)
	{
		if (!Ghost_CheckHu(handcard,outCard,false,isGetCard, gameInfo.m_ghostCard, ghostType))
		{
			result = false;
		}
		else
		{
			result = true;
		}
	}

	//检查七对
	Lint isSpecial = 0;
	bool isQingQidui = false;
	bool bhuQidui = CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, isSpecial, isQingQidui);

	bool bNeedCheckNormalHu = ((result || bhuQidui) && !bHuShisanyao);
	
	if (bNeedCheckNormalHu)
	{
		//检查胡牌类型
		if (gameInfo.m_allwindType && CheckAllWindGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_ZIYISE);
		}
		else if (gameInfo.m_yaojiuType && !gameInfo.m_yaojiuHas1or9 && CheckYaoJiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_YAOJIU);
		}
		else if (gameInfo.m_yaojiuType && gameInfo.m_yaojiuHas1or9 && CheckYaoJiuHasOneOrNineHu(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_YAOJIU);
		}
		else if (gameInfo.m_qingyiseType && gameInfo.m_duiduihuType && CheckQingDuiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGPENG);
		}
		else if (gameInfo.m_qiduiType && bhuQidui)	//有四倍的时候七对分要
		{
			vec.push_back(HU_QIDUI);
			result = true;
		}
		else if (gameInfo.m_qingyiseType && CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGYISE);
		}
		else if (gameInfo.m_duiduihuType && CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_PENGPENGHU);
		}
	}

	bool bspecial = IsOneHundredSpecialHuType(vec);		//比较坑，4倍的七小对才是特殊胡
	if (!result || !bspecial)
	{
		//检查是否可以四鬼胡
		if (gameInfo.m_canfourghosthu && CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_FOURGHOST);
			result = true;
		}
		else if (bhuQidui)
		{
			vec.push_back(HU_QIDUI);
			result = true;
		}
	}

	if (result)
	{
		//杠上开花
		if (CheckGangshangkaihua(gameInfo))
		{
			vec.push_back(HU_GANGSHANGKAIHUA);
		}

		//可以胡，检查是否可以抢杠胡
		if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
		{
			auto huType = gameInfo.m_hasDianGang ? HU_QIANGMINGGANGHU : HU_QIANGGANGHU;
			vec.push_back(huType);
		}
		else
		{
			vec.push_back(HU_ZIMO);
		}
	}
	return result;
}

bool CardManager::CheckRaopingCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;
	std::vector<Lint> huTypeVec;
	//检查是否可以胡十三幺
	if (CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		//天地人胡判断
		if (gameInfo.m_canTianhu)
		{
			huTypeVec.push_back(HU_TIANHU);
		}
		else if (gameInfo.m_canDihu)
		{
			huTypeVec.push_back(HU_DIHU);
		}
		else
		{
			huTypeVec.push_back(HU_SHISANYAO);
		}
		result = true;
	}

	bool canhu = false;
	if (!result)
	{
		//检查普通胡
		if (Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
		{
			canhu = true;
		}

		//检查七小对
		Lint special = 0;
		bool isQingQidui = false;
		bool huQidui = CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, special, isQingQidui);
		if (huQidui)
		{
			canhu = true;
		}

		if (canhu)
		{
			//天地人胡判断
			if (gameInfo.m_canTianhu)
			{
				huTypeVec.push_back(HU_TIANHU);
			}
			else if (gameInfo.m_canDihu)
			{
				huTypeVec.push_back(HU_DIHU);
			}
			else if (huQidui && special == 3)
			{
				huTypeVec.push_back(HU_TLUXURYQIDUI);			//三豪华
			}
			else if (huQidui && special == 2)
			{
				huTypeVec.push_back(HU_DLUXURYQIDUI);			//双豪华
			}
			else if (CheckQingDuiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_QINGPENG);
			}
			else if (huQidui && special >= 1)
			{
				huTypeVec.push_back(HU_LUXURYQIDUI);
			}
			else if (CheckYaoJiuHasOneOrNineHu(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_YAOJIU);
			}
			//大四喜
			else if (CheckBigSixiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_DASIXI);
			}
			//大三元
			else if (CheckBigSanyuanGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_DASANYUAN);
			}
			//小四喜
			else if (CheckLittleSixiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_XIAOSIXI);
			}
			//小三元
			else if (CheckLittleSanyuanGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_XIAOSANYUAN);
			}
			else if (huQidui)
			{
				huTypeVec.push_back(HU_QIDUI);
			}
			else if (CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_QINGYISE);
			}
			else if (CheckHunpengGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_HUNPENG);
			}
			else if (CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_PENGPENGHU);
			}
			else if (CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				huTypeVec.push_back(HU_HUNYISE);
			}
			result = true;
		}
		else
		{
			return result;
		}
	}

	//杠上开花
	if (CheckGangshangkaihua(gameInfo))
	{
		huTypeVec.push_back(HU_GANGSHANGKAIHUA);
	}
	//抢杠胡
	if (CheckQiangganghu(gameInfo))
	{
		huTypeVec.push_back(HU_QIANGGANGHU);
	}
	//杠上炮
	if (gameInfo.m_gangHouPao2TimeScore && CheckGangshangpao(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, gameInfo))
	{
		huTypeVec.push_back(HU_GANGSHANGPAO);
	}
	//单吊2倍
	if (gameInfo.m_danDin2TimeScore && CheckDandiao(handcard))
	{
		huTypeVec.push_back(HU_DANDIAO);
	}
	//检查鸡胡是否可以胡
	if (huTypeVec.empty())
	{
		//鸡胡不能胡
		if (gameInfo.m_cannotJihu)
		{
			return false;
		}
		//鸡胡不能点炮胡
		if (gameInfo.m_cannotJihuDianpao && !isGetCard)
		{
			return false;
		}
		result = true;
		huTypeVec.push_back(HU_JIHU);
	}


	vec = huTypeVec;
	return result;
}


bool CardManager::CheckChaoShanCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;

	//检查是否可以胡十三幺
	if(CheckShisanyaoGhostType(handcard, pengCard,agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		//天地人胡判断
		if (gameInfo.m_canTianhu)
		{
			vec.push_back(HU_TIANHU);
		}
		else if (gameInfo.m_canDihu)
		{
			vec.push_back(HU_DIHU);
		}
		else if(gameInfo.m_shisanyaoType)
		{
			vec.push_back(HU_SHISANYAO);
		}
		result = true;
	}

	if (!result)
	{
		//检查普通胡
		if (Ghost_CheckHu(handcard,outCard,false,isGetCard, gameInfo.m_ghostCard, ghostType))
		{
			result = true;
		}

		//检查七小对
		Lint special = 0;
		bool isQingQidui = false;
		bool huQidui = CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, special, isQingQidui);
		if (huQidui)
		{
			result = true;
		}

		if (result)
		{
			//天地人胡判断
			if (gameInfo.m_canTianhu)
			{
				vec.push_back(HU_TIANHU);
			}
			else if (gameInfo.m_canDihu)
			{
				vec.push_back(HU_DIHU);
			}
			else if (gameInfo.m_eighteenarhats && CheckEighteenarhtsType(handcard, pengCard, agangCard, mgangCard, outCard))
			{
				vec.push_back(HU_EIGHTEENARHATS);
			}
			else if (gameInfo.m_tluxuryqidui && huQidui && special == 3)
			{
				vec.push_back(HU_TLUXURYQIDUI);
			}
			else if (gameInfo.m_dluxuryqidui && huQidui && special >= 2)
			{
				vec.push_back(HU_DLUXURYQIDUI);
			}
			//大四喜
			else if (gameInfo.m_bigSixiType && CheckBigSixiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_DASIXI);
			}
			//大三元
			else if (gameInfo.m_bigSanyuanType && CheckBigSanyuanGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_DASANYUAN);
			}
			//小四喜
			else if (gameInfo.m_littleSixiType && CheckLittleSixiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_XIAOSIXI);
			}
			//小三元
			else if (gameInfo.m_littleSanyuanType && CheckLittleSanyuanGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_XIAOSANYUAN);
			}
			else if (gameInfo.m_qingyiseType && gameInfo.m_duiduihuType && CheckQingDuiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_QINGPENG);
			}
			else if (gameInfo.m_luxuryqiduiType && huQidui && special >= 1)
			{
				vec.push_back(HU_LUXURYQIDUI);
			}
			else if(gameInfo.m_luxuryqiduiType && huQidui)
			{
				vec.push_back(HU_QIDUI);
			}
			else if (gameInfo.m_qingyiseType && CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_QINGYISE);
			}
			else if (gameInfo.m_duiduihuType && CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_PENGPENGHU);
			}
			else if (gameInfo.m_hunyiseType && CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_HUNYISE);
			}
			else if (gameInfo.m_doubletimeqiduiType && huQidui)
			{
				vec.push_back(HU_QIDUI);
			}
			else
			{
				if (isGetCard)
				{
					vec.push_back(HU_ZIMO);
				}
				else
				{
					vec.push_back(HU_XIAOHU);
				}
			}
		}
	}
	
	if (result)
	{
		//杠上开花
		if (CheckGangshangkaihua(gameInfo))
		{
			vec.push_back(HU_GANGSHANGKAIHUA);
		}
		//可以胡，检查是否可以抢杠胡
		if (CheckQiangganghu(gameInfo))
		{
			auto huType = HU_QIANGGANGHU;
			vec.push_back(huType);
		}
	}

	if (!result)
	{
		//检查是否可以四鬼胡
		if (gameInfo.m_canfourghosthu)
		{
			if (CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard))
			{
				//天地人胡判断
				if (gameInfo.m_canTianhu)
				{
					vec.push_back(HU_TIANHU);
				}
				else if (gameInfo.m_canDihu)
				{
					vec.push_back(HU_DIHU);
				}
				else
				{
					vec.push_back(HU_FOURGHOST);
				}
				result = true;
			}
		}
	}
	return result;
}

bool CardManager::CheckRedDragonCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard,  CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;

	//检查是否可以胡十三幺
	bool huShisanyao = false;
	if (CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		huShisanyao = true;
		result = true;
	}

	//检查七小对
	Lint special = 0;
	bool isQingQidui = false;
	bool huQidui = false;
	if (gameInfo.m_qiduiType && CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, special, isQingQidui))
	{
		huQidui = true;
		result = true;
	}
		
	//检查普通胡
	if (Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
	{
		result = true;
	}

	if (result)
	{
		bool qingyise = CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);
		qingyise &= gameInfo.m_qingyiseType;

		if (qingyise && huQidui && special == 3)
		{
			vec.push_back(HU_QINGTLUXURYQIDUI);		//清三豪华
		}
		else if (gameInfo.m_eighteenarhats && CheckEighteenarhtsType(handcard, pengCard, agangCard, mgangCard, outCard))
		{
			vec.push_back(HU_EIGHTEENARHATS);		//十八罗汉
		}
		else if (huQidui && special == 3)
		{
			vec.push_back(HU_TLUXURYQIDUI);			//三豪华
		}
		else if (qingyise && huQidui && special >= 2)
		{
			vec.push_back(HU_QINGDLUXURYQIDUI);		//清双豪华
		}
		else if (huQidui && special >= 2)
		{
			vec.push_back(HU_DLUXURYQIDUI);			//双豪华
		}
		else if (gameInfo.m_canTianhu)
		{
			vec.push_back(HU_TIANHU);				//天胡
		}
		else if (gameInfo.m_shisanyaoType && huShisanyao)
		{
			vec.push_back(HU_SHISANYAO);			//十三幺
		}
		else if (qingyise && huQidui && special >= 1)
		{
			vec.push_back(HU_QINGLUXURYQIDUI);		//清豪华七对
		}
		else if (qingyise && huQidui)
		{
			vec.push_back(HU_QINGQIDUI);		//清七对
		}
		else if (huQidui && special >= 1)
		{
			vec.push_back(HU_LUXURYQIDUI);			//豪华
		}
		else if (gameInfo.m_qingyiseType && gameInfo.m_duiduihuType && CheckQingDuiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGPENG);			//清碰
		}
		else if (gameInfo.m_qiduiType && huQidui)
		{
			vec.push_back(HU_QIDUI);			//七对
		}
		else if (gameInfo.m_qingyiseType && qingyise)
		{
			vec.push_back(HU_QINGYISE);			//清一色
		}
		else if (gameInfo.m_hunyiseType && gameInfo.m_duiduihuType && CheckHunpengGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_HUNPENG);			//混碰
		}
		else if (gameInfo.m_duiduihuType && CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_PENGPENGHU);		//碰碰胡
		}
		else if (gameInfo.m_hunyiseType && CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_HUNYISE);		//混一色
		}
		else
		{
			vec.push_back(HU_ZIMO);
		}

		//杠上开花
		if (CheckGangshangkaihua(gameInfo))
		{
			vec.push_back(HU_GANGSHANGKAIHUA);
		}
		//可以胡，检查是否可以抢杠胡
		if (CheckQiangganghu(gameInfo))
		{
			vec.push_back(HU_QIANGGANGHU);
		}

		if (CheckHaiDiLao(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, gameInfo))
		{
			vec.push_back(HU_HAIDILAO);
		}
	}
	
	return result;
}

bool CardManager::CheckHongKongCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;
	std::vector<Lint> vecHuType;
	Lint nQihuFanshu = gameInfo.m_qihuFanshu;

	//先检查十三幺 十三幺是特殊牌型，CheckHu判断不了
	bool bShisanyao = CheckShisanyao(handcard, outCard);
	if (!bShisanyao && !CheckHu(handcard, outCard, false, isGetCard))
	{
		return result;
	}

	//十三番牌型判断
	if (gameInfo.m_canTianhu)
	{
		vecHuType.push_back(HU_TIANHU);
	}
	else if (gameInfo.m_canDihu)
	{
		vecHuType.push_back(HU_DIHU);
	}

	if (bShisanyao)
	{
		vecHuType.push_back(HU_SHISANYAO);
		vec = vecHuType;
		return true;
	}
	else if (CheckJiulianbaodeng(handcard, outCard))
	{
		vecHuType.push_back(HU_JIULIANBAODENG);
		vec = vecHuType;
		return true;
	}
	else if (CheckDasixi(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_DASIXI);
		vec = vecHuType;
		return true;
	}
	else if (CheckDasanyuan(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_DASANYUAN);
		vec = vecHuType;
		return true;
	}
	else if (CheckQingyaojiu(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_QINGYAOJIU);
		vec = vecHuType;
		return true;
	}
	else if (CheckEighteenarhtsType(handcard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_EIGHTEENARHATS);
		vec = vecHuType;
		return true;
	}

	bool bSpecialHu = false;
	bool bSanyuanType = false;
	//非十三番胡牌类型
	if (CheckZiyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_ZIYISE);
		bSpecialHu = true;
	}
	if (CheckQingyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_QINGYISE);
		bSpecialHu = true;
	}
	if (CheckHunYiSe(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_HUNYISE);
		bSpecialHu = true;
	}
	if (CheckXiaosixi(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_XIAOSIXI);
		bSpecialHu = true;
		bSanyuanType = true;
	}
	if (CheckXiaosanyuan(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_XIAOSANYUAN);
		bSpecialHu = true;
		bSanyuanType = true;
	}
	if (CheckPengpenghu(handcard, eatCard, outCard))
	{
		vecHuType.push_back(HU_PENGPENGHU);
		bSpecialHu = true;
		bSanyuanType = true;
	}
	if (CheckPingHu(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		vecHuType.push_back(HU_PING);
		bSpecialHu = true;
		bSanyuanType = true;
	}
	if (!bSpecialHu)
	{
		vecHuType.push_back(HU_JIHU);
	}

	//抢杠胡
	if (CheckQiangganghu(gameInfo))
	{
		vecHuType.push_back(HU_QIANGGANGHU);
	}
	//杠上开花
	if (CheckGangshangkaihua(gameInfo))
	{
		vecHuType.push_back(HU_GANGSHANGKAIHUA);
	}
	//海底捞
	if (CheckHaiDiLao(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, gameInfo))
	{
		vecHuType.push_back(HU_HAIDILAO);
	}

	//门清
	bool bMenqing = CheckMenQing(handcard, eatCard, pengCard, agangCard, mgangCard, outCard);
	if (bMenqing && isGetCard)
	{
		vecHuType.push_back(HU_ZIMOMENQING);
	}
	else if (bMenqing)
	{
		vecHuType.push_back(HU_MENQING);
	}
	else if (isGetCard)
	{
		vecHuType.push_back(HU_ZIMO);
	}
	//三元牌
	if (!bSanyuanType)
	{
		int num = GetJianKeNum(handcard, eatCard, pengCard, agangCard, mgangCard, outCard);
		if (1 == num)
		{
			vecHuType.push_back(HU_SANYUANPAIONE);
		}
		else if (2 == num)
		{
			vecHuType.push_back(HU_SANYUANPAITWO);
		}
	}
	//风圈、风位
	CheckFengCardKe(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_MePos, gameInfo.m_fengQuan, vecHuType);
	//庄家
	if (gameInfo.m_bZhuang)
	{
		vecHuType.push_back(HU_ZHUANGJIA);
	}
	result = IsHongKongFanShuCanQihu(nQihuFanshu, vecHuType, gameInfo.m_additionFanNum);
	if (result)
	{
		vec = vecHuType;
	}
	return result;
}

bool CardManager::CheckHuizhouzhuangFlowerHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	std::vector<Lint> vecHuType;
	int nHuType = HU_JIHU;
	//七花胡判断
	if (isGetCard && gameInfo.m_bHuSevenFlower)
	{
		if (gameInfo.m_canTianhu)
		{
			vec.push_back(HU_TIANHU);
		}
		else if (gameInfo.m_canDihu)
		{
			vec.push_back(HU_DIHU);
		}
		else
		{
			vec.push_back(HU_SEVENFLOWER);
		}
		return true;
	}
	//先检查十三幺 十三幺是特殊牌型，CheckHu判断不了
	bool bShisanyao = CheckShisanyao(handcard, outCard);
	if (!bShisanyao && !CheckHu(handcard, outCard, false, isGetCard))
	{
		return false;
	}
	//门清
	bool bMenqing = CheckMenQing(handcard, eatCard, pengCard, agangCard, mgangCard, outCard);
	bool bqingyaojiu = false;
	
	if (bShisanyao)
	{
		nHuType = HU_SHISANYAO;
	}
	//全风
	else if (CheckZiyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = HU_ZIYISE;
	}
	else if (CheckHuizhouFloweryaojiu(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, bqingyaojiu))
	{
		if (bqingyaojiu)
		{
			nHuType = bMenqing ? HU_MENQINGQINGYAOJIU : HU_QINGYAOJIU;
		}
		else
		{
			nHuType = bMenqing ? HU_MENQINGHUNYAOJIU : HU_HUNYAOJIU;
		}
	}
	else if (CheckQingPeng(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = bMenqing ? HU_MENQINGQINGPENG : HU_QINGPENG;
	}
	else if (CheckQingyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = bMenqing ? HU_MENQINGQINGYISE : HU_QINGYISE;
	}
	else if (CheckHunPeng(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = bMenqing ? HU_MENQINGHUNPENG : HU_HUNPENG;
	}
	else if (CheckPengpenghu(handcard, eatCard, outCard))
	{
		nHuType = HU_PENGPENGHU;
	}
	else if (CheckHunYiSe(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = HU_HUNYISE;
	}
	else
	{
		nHuType = HU_JIHU;
	}
	//鸡胡不可以点炮胡
	if (!isGetCard && HU_JIHU == nHuType)
	{
		//检查是否可以抢杠胡
		if (!gameInfo.m_canJihuQianggang || gameInfo.m_cardState != THINK_OPERATOR_MGANG)
		{
			return false;
		}
	}
	//天胡、地胡判断
	if (gameInfo.m_canTianhu)
	{
		nHuType = HU_TIANHU;
	}
	else if (gameInfo.m_canDihu)
	{
		nHuType = HU_DIHU;
	}

	vecHuType.push_back(nHuType);
	//抢杠胡
	if (CheckQiangganghu(gameInfo))
	{
		vecHuType.push_back(HU_QIANGGANGHU);
	}
	//自摸
	if (isGetCard)
	{
		vecHuType.push_back(HU_ZIMO);
	}
	vec = vecHuType;
	return true;
}

bool CardManager::CheckJiHuFlowerHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	if ( !CheckHu(handcard, outCard, false, isGetCard))
	{
		return false;
	}

	//抢杠胡
	if (CheckQiangganghu(gameInfo))
	{
		vec.push_back(HU_QIANGGANGHU);
	}
	//杠上开花
	if (CheckGangshangkaihua(gameInfo))
	{
		vec.push_back(HU_GANGSHANGKAIHUA);
	}
	//自摸
	if (isGetCard)
	{
		vec.push_back(HU_ZIMO);
	}
	else
	{
		vec.push_back(HU_XIAOHU);
	}
	return true;
}

bool CardManager::CheckTwoPlayerTuidaoHuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	if (ghostType != PT_GHOSTCARDNO && ghostType != PT_GHOSTCARDFLOPTWO)		//检查四个鬼牌
	{
		if (CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_ZIMO);
			return true;
		}
	}

	if (Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
	{
		vec.push_back(HU_ZIMO);
		return true;
	}
	return false;
}

bool CardManager::CheckGuangdongJihuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;

	if (!Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
	{
		return false;
	}
	
	//抢杠胡
	if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
	{
		auto huType = gameInfo.m_hasDianGang ? HU_QIANGMINGGANGHU : HU_QIANGGANGHU;
		vec.push_back(huType);
	}
	else
	{
		vec.push_back(HU_ZIMO);
	}
	return true;
}

bool CardManager::CheckMeizhouHongzhongbaoCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType) const
{
	std::vector<Lint> hu_vec;
	bool result = false;
	
	//检查是否可以胡十三幺
	bool bHushisanyao = CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);
	
	//检查是否可以胡七对
	Lint nSpecial = 0;
	bool isQingQidui = false;
	bool bQidui = CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, nSpecial, isQingQidui);

	//检查是或可以四鬼胡牌
	bool bFourGhost = false;
	if (gameInfo.m_canTianhu && gameInfo.m_bZhuang)
	{
		bFourGhost = CheckQishouHongzhong(handcard, outCard);
	}
	
	//检查时候可以普通胡
	bool bCanNomalHu = Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType);

	bool bCanhu = (bHushisanyao || bQidui || bFourGhost || bCanNomalHu);

	//天胡不叠加直接胡
	if (gameInfo.m_canTianhu && bCanhu)
	{
		hu_vec.push_back(HU_TIANHU);
		vec = hu_vec;
		return true;
	}
	//地胡
	if (gameInfo.m_canDihu && bCanhu)
	{
		hu_vec.push_back(HU_DIHU);
		vec = hu_vec;
		return true;
	}

	//十三幺
	if (bHushisanyao)
	{
		hu_vec.push_back(HU_SHISANYAO);
		vec = hu_vec;
		return true;
	}

	if (bQidui)
	{
		if (nSpecial == 3)	//三豪华
		{
			hu_vec.push_back(HU_TLUXURYQIDUI);
		}
		else if (nSpecial == 2)	//双豪华
		{
			hu_vec.push_back(HU_DLUXURYQIDUI);
		}
		else if (nSpecial == 1)
		{
			hu_vec.push_back(HU_LUXURYQIDUI);
		}
		else
		{
			hu_vec.push_back(HU_QIDUI);
		}

		if (isQingQidui)
		{
			hu_vec.push_back(HU_QINGYISE);
		}
	}


	//检查普通胡
	bool bAllWind = false;
	if (bCanNomalHu && !bQidui)
	{
		//全风
		if ( CheckAllWindGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			bAllWind = true;
			hu_vec.push_back(HU_ZIYISE);
		}
		//碰碰胡
		if (!bQidui && CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			hu_vec.push_back(HU_PENGPENGHU);
		}
		//清一色
		if (CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			hu_vec.push_back(HU_QINGYISE);
		}
		//混一色
		else if ( CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			hu_vec.push_back(HU_HUNYISE);
		}
	}

	if (bCanNomalHu || bQidui)
	{
		//清幺九
		if (!bAllWind && CheckQingyaojiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			hu_vec.push_back(HU_QINGYAOJIU);
		}
		//混幺九
		else if (!bAllWind && CheckHunyaojiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			hu_vec.push_back(HU_HUNYAOJIU);
		}

		//可以胡，检查是否可以抢杠胡
		if (CheckQiangganghu(gameInfo))
		{
			if (hu_vec.empty())
			{
				hu_vec.push_back(HU_XIAOHU);
			}
			hu_vec.push_back(HU_QIANGGANGHU);
		}

		if (hu_vec.empty())
		{
			hu_vec.push_back(HU_ZIMO);
		}
		
		//检查门清
		if (CheckMenQing(handcard, eatCard, pengCard,agangCard,mgangCard,outCard))
		{
			hu_vec.push_back(HU_MENQING);
		}
		//检查无红中胡牌
		if (CheckNoHongzhong(handcard,pengCard,agangCard,mgangCard,outCard))
		{
			hu_vec.push_back(HU_WITHOUTHONGZHONG);
		}
		if (CheckGangshangkaihua(gameInfo))
		{
			hu_vec.push_back(HU_GANGSHANGKAIHUA);
		}

		//检查番数是否足够
		if (IsMeizhouHongzhongbaoCanQihu(gameInfo.m_qihuFanshu, hu_vec))
		{
			vec = hu_vec;
			result = true;
		}
	}
	return result;
}

bool CardManager::CheckGuangdongJieyangCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	std::vector<Lint> hu_vec;
	bool result = false;

	//检查是否可以胡十三幺
	bool bHushisanyao = CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);

	//检查是否可以胡七对
	Lint nSpecial = 0;
	bool isQingQidui = false;
	bool bQidui = CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, nSpecial, isQingQidui);

	//检查时候可以普通胡
	bool bCanNomalHu = Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType);

	bool bCanhu = (bHushisanyao || bQidui || bCanNomalHu);

	if (!bCanhu)
	{
		return false;
	}

	bool bQingyise = false;
	bool bHunyise = false;
	bool bPengpenghu = false;
	//清一色
	if (CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		bQingyise = true;
	}
	//混一色
	else if (CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		bHunyise = true;
	}

	if (CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		bPengpenghu = true;
	}

	//十八罗汉
	if (CheckEighteenarhtsType(handcard, pengCard, agangCard, mgangCard, outCard))
	{
		if (bQingyise)
		{
			hu_vec.push_back(HU_QINGEIGHTEENARHATS);
		}
		else
		{
			hu_vec.push_back(HU_EIGHTEENARHATS);
		}
			
	}
	else if (bQidui && nSpecial >= 2)	//双豪华七对
	{
		if (bQingyise)
		{
			hu_vec.push_back(HU_QINGDLUXURYQIDUI);
		}
		else
		{
			hu_vec.push_back(HU_DLUXURYQIDUI);
		}
			
	}
	else if (gameInfo.m_canTianhu)	//天胡
	{
		hu_vec.push_back(HU_TIANHU);
	}
	else if (gameInfo.m_canDihu)	//地胡
	{
		hu_vec.push_back(HU_DIHU);
	}
	else if (bHushisanyao)	//十三幺
	{
		hu_vec.push_back(HU_SHISANYAO);
	}
	else if (CheckQingyaojiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))	//纯幺九
	{
		hu_vec.push_back(HU_QINGYAOJIU);
	}
	else if (CheckAllWindGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))	//国字
	{
		hu_vec.push_back(HU_ZIYISE);
	}
	else if (bQingyise && bQidui && nSpecial == 1)	//清豪华七对
	{
		hu_vec.push_back(HU_QINGLUXURYQIDUI);
	}
	else if (bQingyise && bQidui)	//清七对
	{
		hu_vec.push_back(HU_QINGQIDUI);
	}
	else if (CheckJieYangHunyaojiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))	//混幺九
	{
		hu_vec.push_back(HU_HUNYAOJIU);
	}
	else if (CheckJieYangXiaoyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))	//小幺
	{
		hu_vec.push_back(HU_XIAOYAO);
	}
	else if (bQidui && nSpecial == 1)	//豪华七对
	{
		hu_vec.push_back(HU_LUXURYQIDUI);
	}
	else if (bQingyise && bPengpenghu)		//清碰
	{
		hu_vec.push_back(HU_QINGPENG);
	}
	else if (bQidui && bHunyise)		//混七对
	{
		hu_vec.push_back(HU_HUNQIDUI);
	}
	else if (bQidui && bHunyise)		//混七对
	{
		hu_vec.push_back(HU_HUNQIDUI);
	}
	else if (bPengpenghu && bHunyise)	//混碰
	{
		hu_vec.push_back(HU_HUNPENG);
	}
	else if (bQingyise)		//清一色
	{
		hu_vec.push_back(HU_QINGYISE);
	}
	else if (bQidui)//七对
	{
		hu_vec.push_back(HU_QIDUI);
	}
	else if (bHunyise)	//混一色
	{
		hu_vec.push_back(HU_HUNYISE);
	}
	else if (bPengpenghu)	//碰碰胡
	{
		hu_vec.push_back(HU_PENGPENGHU);
	}
	//不是可接炮胡 允许听10倍接炮并且不是杠 检查番数是否够
	if (!isGetCard && !gameInfo.m_thinkGang && gameInfo.m_tenfanjiepao && gameInfo.m_zimoType && gameInfo.m_cardState != THINK_OPERATOR_MGANG)
	{
		bool bcanJiePaohu = CheckJieyangCardTypeCanJiepao(hu_vec, gameInfo.m_nTingFanshu);
		if (!bcanJiePaohu)
		{
			return false;
		}
	}

	if (hu_vec.empty())
	{
		if (isGetCard)
		{
			hu_vec.push_back(HU_ZIMO);
		}
		else
		{
			hu_vec.push_back(HU_JIHU);
		}
	}

	//可以胡，检查是否可以抢杠胡
	if (CheckQiangganghu(gameInfo))
	{
		hu_vec.push_back(HU_QIANGGANGHU);
	}

	if (CheckGangshangkaihua(gameInfo))
	{
		hu_vec.push_back(HU_GANGSHANGKAIHUA);
	}
	if (CheckHaiDiLao(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, gameInfo))
	{
		hu_vec.push_back(HU_HAIDILAO);
	}
	
	vec = hu_vec;
	return true;
}

/************************************************************************/
/* 揭西麻将有鸡胡和鸡大两种大的选项，每种选项的下边有各自的碰杠胡选项 */
/************************************************************************/
bool CardManager::CheckGuangdongJiexiJihuCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType) const
{
	bool canHu = false;
	int nSpecial = 0;
	bool isQingQidui = false;
	bool isXiaoQidui  = CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, nSpecial, isQingQidui);
	bool isJihu		  = Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType);
	bool bHushisanyao = CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);
	//可以胡，检查是否可以抢杠胡
	if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
	{
		if (isXiaoQidui || isJihu || bHushisanyao)
		{
			vec.push_back(HU_QIANGGANGHU);	//抢杠胡作为一种胡牌类型，有具体牌型分数
			canHu = true;
		}

	}
	//除了抢杠胡之外，只能自摸胡
	else if (gameInfo.m_zimoType && isXiaoQidui)
	{
		vec.push_back(HU_QIDUI);
		canHu = true;
	}
	else if (gameInfo.m_zimoType && (isJihu || bHushisanyao))
	{
		vec.push_back(HU_JIHU);
		canHu = true;
	}

	if (gameInfo.m_canfourghosthu && CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard))
	{
		vec.push_back(HU_FOURGHOST);
		canHu = true;
	}
	
	if (!vec.empty() && CheckGangshangkaihua(gameInfo))
	{
		vec.push_back(HU_GANGSHANGKAIHUA);
	}
	//无鬼加倍
	if (!vec.empty() && CheckNoGhost2Times(gameInfo))
	{
		vec.push_back(HU_NOGHOST2TIMES);
	}

	return canHu;
}

bool CardManager::CheckGuangdongJiexiCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType) const
{
	if (gameInfo.m_JiexiJIDAORJIHU == PT_JIEXIJIHU)		//揭西鸡胡
	{
		return CheckGuangdongJiexiJihuCanHu(handcard, pengCard,agangCard, mgangCard, outCard, gameInfo,vec, isGetCard, ghostType);
	}

	//揭西鸡大玩法
	bool result = false;
	//检查是否可以胡十三幺
	bool bHushisanyao = CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);

	//检查是否可以胡七对
	Lint nSpecial = 0;
	bool isQingQidui = false;
	bool bQidui = CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, nSpecial, isQingQidui);
	//四鬼胡牌是一种基本胡牌类型
	bool isFourGhostHu = gameInfo.m_canfourghosthu && CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard);
	//检查时候可以普通胡
	bool bCanNomalHu = Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType);

	bool bCanhu = (bHushisanyao || bQidui || bCanNomalHu || isFourGhostHu);

	if (!bCanhu)
	{
		return false;
	}

	bool bQingyise = false;
	bool bHunyise = false;
	bool bPengpenghu = false;
	//清一色
	if (CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		bQingyise = true;
	}
	//混一色
	else if (CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		bHunyise = true;
	}

	if (CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		bPengpenghu = true;
	}

	//十八罗汉
	if (CheckEighteenarhtsType(handcard, pengCard, agangCard, mgangCard, outCard))
	{
			vec.push_back(HU_EIGHTEENARHATS);
	}
	else if (bQidui && nSpecial >= 2)	//双豪华七对
	{
			vec.push_back(HU_DLUXURYQIDUI);
	}
	else if (gameInfo.m_canTianhu)	//天胡
	{
		vec.push_back(HU_TIANHU);
	}
	else if (gameInfo.m_canDihu)	//地胡
	{
		vec.push_back(HU_DIHU);
	}
	else if (bHushisanyao)	//十三幺
	{
		vec.push_back(HU_SHISANYAO);
	}
	else if (CheckQingyaojiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))	//纯幺九
	{
		vec.push_back(HU_QINGYAOJIU);
	}
	else if ((bPengpenghu || bQidui) && CheckAllWindGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))	//问题 国字
	{
		vec.push_back(HU_ZIYISE);
	}
	else if (bQingyise && bQidui)	//清七对
	{
		vec.push_back(HU_QINGQIDUI);
	}
	else if (CheckJieYangXiaoyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))	//小幺
	{
		vec.push_back(HU_XIAOYAO);
	}
	else if (bQidui && nSpecial == 1)	//豪华七对
	{
		vec.push_back(HU_LUXURYQIDUI);
	}
	else if (bQingyise && bPengpenghu)		//清碰
	{
		vec.push_back(HU_QINGPENG);
	}
	else if (bQidui && bHunyise)		//混七对
	{
		vec.push_back(HU_HUNQIDUI);
	}
	else if (bPengpenghu && bHunyise)	//混碰
	{
		vec.push_back(HU_HUNPENG);
	}
	else if (bQingyise)		//清一色
	{
		vec.push_back(HU_QINGYISE);
	}
	else if ((gameInfo.m_zimoType || (gameInfo.m_canqiangganghu  && CheckQiangganghu(gameInfo)) ) && bQidui)//七对，只能自摸胡或者抢杠胡
	{
		vec.push_back(HU_QIDUI);
	}
	else if (gameInfo.m_zimoType || (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo)) ) //鸡胡，只能自摸胡或者抢杠胡
	{
		vec.push_back(HU_JIHU);
	}

	//可以胡，检查是否可以抢杠胡
	if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
	{
		vec.push_back(HU_QIANGGANGHU);
	}
	if (CheckGangshangkaihua(gameInfo))		
	{
		vec.push_back(HU_GANGSHANGKAIHUA);
	}

	if (isFourGhostHu)
	{
		vec.push_back(HU_FOURGHOST);
	}

	if (!vec.empty() && CheckHaiDiLao(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, gameInfo))
	{
		vec.push_back(HU_HAIDILAO);
	}
	//无鬼加倍
	if (!vec.empty() && CheckNoGhost2Times(gameInfo))
	{
		vec.push_back(HU_NOGHOST2TIMES);
	}
	
	return !vec.empty();
}

bool CardManager::CheckShaoguanCanHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard,  CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;

	//检查是否可以胡十三幺
	bool huShisanyao = false;
	if (CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		huShisanyao = true;
		result = true;
	}

	bool huCommon = false;
	if (Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
	{
		huCommon = true;
		result = true;
	}

	if (result)
	{
		if (gameInfo.m_shisanyaoType && huShisanyao || gameInfo.m_yaojiuHas1or9 && huShisanyao )
		{
			vec.push_back(HU_SHISANYAO);
		}
		else if (huCommon && CheckAllWindGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_ZIYISE);
		}
		else if (CheckYaoJiuHasOneOrNineHu(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_YAOJIU);
		}
		else if (CheckQingDuiGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGPENG);
		}
		else if (CheckHunpengGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_HUNPENG);
		}
		else if (CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGYISE);
		}
		else if (CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_HUNYISE);
		}
		else if (CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_PENGPENGHU);
		}
	}
	if (!result)
	{
		//检查是否可以四鬼胡
		if (gameInfo.m_canfourghosthu)
		{
			if (CheckFourGhost(handcard, outCard, isGetCard, gameInfo.m_ghostCard))
			{
				vec.push_back(HU_FOURGHOST);
				result = true;
			}
		}
	}

	if (result)
	{
		//杠上开花
		if (CheckGangshangkaihua(gameInfo))
		{
			vec.push_back(HU_GANGSHANGKAIHUA);
		}

		//可以胡，检查是否可以抢杠胡
		if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
		{
			auto huType = gameInfo.m_hasDianGang ? HU_QIANGMINGGANGHU : HU_QIANGGANGHU;
			vec.push_back(huType);
		}
		else
		{
			vec.push_back(HU_ZIMO);
		}
	}
	return result;
}

bool CardManager::IsOneHundredSpecialHuType(std::vector<Lint> hu_vec) const
{
	std::vector<Lint>::iterator cit = hu_vec.begin();
	for ( ; cit != hu_vec.end(); ++cit)
	{
		if (*cit == HU_SHISANYAO || *cit == HU_ZIYISE || *cit == HU_YAOJIU
			|| *cit == HU_QINGPENG || *cit == HU_QINGYISE || *cit == HU_PENGPENGHU || *cit == HU_QIDUI
			|| *cit == HU_HUNYISE || *cit == HU_HUNPENG || *cit == HU_EIGHTEENARHATS || *cit == HU_TLUXURYQIDUI
			|| *cit == HU_DLUXURYQIDUI)
		{
			return true;
		}
	}
	return false;
}

bool CardManager::CheckQingyaojiuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	EraseCard(tmp, ghostCard);
	//检查是否都是幺九
	for (Lint i = 0; i < tmp.size(); ++i)
	{
		if (IsZi(tmp[i]))
		{
			return false;
		}

		if (tmp[i]->m_number != 1 && tmp[i]->m_number != 9)
		{
			return false;
		}
	}

	return true;
}

bool CardManager::CheckHunyaojiuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	EraseCard(tmp, ghostCard);
	//检查是否都是幺九或者鬼牌
	bool bhavezi = false;
	bool bhaveoneornine = false;
	for (Lint i = 0; i < tmp.size(); ++i)
	{
		if (IsZi(tmp[i]))
		{
			bhavezi = true;
		}

		if (IsNineOne(tmp[i]))
		{
			bhaveoneornine = true;
		}

		if (!IsNineOne(tmp[i]) && !IsZi(tmp[i]))
		{
			return false;
		}
	}

	if (bhavezi && bhaveoneornine)
	{
		return true;
	}

	return false;
}

bool CardManager::CheckJieYangHunyaojiuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	EraseCard(tmp, ghostCard);

	if (tmp.size() == 0)
	{
		return true;
	}
	//检查是否都是幺九或者鬼牌
	bool bhavezi = false;
	bool bhaveoneornine = false;
	Lint color = CARD_COLOR_FLOWER;
	bool isFirst1or9 = true;
	for (Lint i = 0; i < tmp.size(); ++i)
	{
		if (IsZi(tmp[i]))
		{
			bhavezi = true;
		}
		else if (IsNineOne(tmp[i]))
		{
			bhaveoneornine = true;
			Lint color1or9 = tmp[i]->m_color;

			if (isFirst1or9)
			{
				isFirst1or9 = false;
				color = color1or9;
			}
			else if (color != color1or9)
			{
				return false;
			}
		}

		if (!IsNineOne(tmp[i]) && !IsZi(tmp[i]))
		{
			return false;
		}
	}

	if (bhavezi && bhaveoneornine)
	{
		return true;
	}

	return false;
}


bool CardManager::CheckJieYangXiaoyaoGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	EraseCard(tmp, ghostCard);

	if (tmp.size() == 0)
	{
		return true;
	}
	//检查是否都是幺九或者鬼牌
	bool bhavezi = false;
	bool bhaveoneornine = false;
	bool b1or9Qingyise = true;
	Lint color = CARD_COLOR_FLOWER;
	bool bfirst1or9 = true;
	for (Lint i = 0; i < tmp.size(); ++i)
	{
		if (IsZi(tmp[i]))
		{
			bhavezi = true;
		}
		else if (IsNineOne(tmp[i]))
		{
			bhaveoneornine = true;
			Lint color1or9 = tmp[i]->m_color;

			if (bfirst1or9)
			{
				color = color1or9;
				bfirst1or9 = false;
			}
			else if (color != color1or9)
			{
				b1or9Qingyise = false;
			}
		}

		if (!IsNineOne(tmp[i]) && !IsZi(tmp[i]))
		{
			return false;
		}
	}

	if (bhavezi && bhaveoneornine && !b1or9Qingyise)
	{
		return true;
	}

	return false;
}

bool CardManager::CheckTingShisanyaoGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCards) const
{
	CardVector handcard_tmp = handcard;

	if (handcard_tmp.size() != 13)
	{
		return false;
	}

	CardVector::const_iterator it_shisanyao = m_ShiSanYiaoVec.begin();
	for (; it_shisanyao != m_ShiSanYiaoVec.end(); ++it_shisanyao)
	{
		CardVector::const_iterator it_hand = handcard_tmp.begin();
		for (; it_hand != handcard_tmp.end(); )
		{
			if (IsSame(*it_hand, *it_shisanyao))
			{
				it_hand = handcard_tmp.erase(it_hand);
				break;
			}
			else
			{
				++it_hand;
			}
		}
	}

	for (Lint i = 0; i < ghostCards.size(); ++i)
	{
		Card card;
		card.m_color = ghostCards[i].m_color;
		card.m_number = ghostCards[i].m_number;
		EraseCard(handcard_tmp, card);
	}

	if (handcard_tmp.size() > 1)
	{
		return false;
	}
	if (handcard_tmp.size() == 1)
	{
		CardVector::const_iterator it_remind_begin = handcard_tmp.begin();
		CardVector::const_iterator it_shisanyao = m_ShiSanYiaoVec.begin();
		for (; it_shisanyao != m_ShiSanYiaoVec.end(); ++it_shisanyao)
		{
			if (IsSame(*it_remind_begin, *it_shisanyao))
			{
				return true;
			}
		}
		if (it_shisanyao == m_ShiSanYiaoVec.end())
		{
			return false;
		}
	}
	return true;
}

bool CardManager::CheckTingXiaoqidui(const CardVector& handcard, Lint ghostType, const std::vector<CardValue>& ghostCard, bool zimoType, Lint& special, bool& isQingQidui) const
{
	
	CardVector all = handcard;
	//计算癞子数，并从手牌里剔除
	int ghostNum = 0;
	if (ghostType != PT_GHOSTCARDNO)
	{
		CardVector::iterator it_all = all.begin();
		for (; it_all != all.end(); )
		{
			CardValue card;
			card.m_color = (*it_all)->m_color;
			card.m_number = (*it_all)->m_number;
			std::vector<CardValue>::const_iterator cit_ghost = std::find(ghostCard.begin(), ghostCard.end(), card);
			if (cit_ghost == ghostCard.end())
			{
				++it_all;
			}
			else
			{
				ghostNum++;
				it_all = all.erase(it_all);
			}
		}
	}

	//让玩家多一张鬼牌
	ghostNum++;

	int cardsNum = all.size();
	gCardMgr.SortCard(all);

	CardVector one, tow, three, four;
	GetSpecialOneTwoThreeFour(all, one, tow, three, four);
	int singleNum = one.size() + three.size() / 3;
	int handcardNum = cardsNum + ghostNum;

	Lint remainGhostNum = ghostNum - singleNum;
	if (remainGhostNum >= 0 && handcardNum == 14)
	{
		//查找豪华数量
		special = four.size() / 4 + three.size() / 3;
		Lint twoNum = tow.size() / 2 + one.size();
		Lint havePairGhostNum = remainGhostNum / 2;

		Lint makepairNum = twoNum > havePairGhostNum ? havePairGhostNum : twoNum;

		special += makepairNum;
		remainGhostNum = remainGhostNum - makepairNum * 2;
		if (remainGhostNum >= 4)
		{
			special += remainGhostNum / 4;
		}

		//检查是否是清七对
		bool bsame = IsSameColor(all);
		bool havezi = HaveZi(all);
		if (bsame && !havezi)
		{
			isQingQidui = true;
		}
		return true;
	}
	return false;
}

bool CardManager::CheckTingEighteenarhtsType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	
	if (tmp.size() == (EIGHTEEN - 1))
	{
		return true;
	}
	return false;
}

bool CardManager::CheckTingDuiduihuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCard) const
{
	CardVector check, one, two, three, four;
	check = handcard;
	
	Lint ghostNum = EraseCard(check, ghostCard);
	//玩家手中加个鬼牌
	ghostNum++;
	GetSpecialOneTwoThreeFour(check, one, two, three, four);

	Lint oneNum = one.size();
	Lint twoNum = two.size() / 2;
	Lint threeNum = three.size() / 3;
	Lint fourNum = four.size() / 4;

	Lint remainedGhostNum = ghostNum - oneNum - fourNum;

	if (remainedGhostNum < 0)
	{
		return false;
	}
	Lint needGhost = 0;
	Lint singleNum = oneNum + twoNum + fourNum;
	if (0 == singleNum)
	{
		needGhost = 2;
	}
	else
	{
		needGhost = oneNum + twoNum + fourNum - 1;
	}

	if (remainedGhostNum - needGhost < 0)
	{
		return false;
	}

	Lint num = (remainedGhostNum - needGhost) % 3;	//组成一副牌后剩余鬼数量
	if (!num)
	{
		return true;
	}

	return false;
}

bool CardManager::CheckTingQingyaojiuGhostType(bool bTingQidui, bool bTingDuidui, CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCard) const
{
	if (!bTingDuidui && !bTingQidui)
	{
		return false;
	}

	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());

	EraseCard(tmp, ghostCard);
	//检查是否都是幺九
	for (Lint i = 0; i < tmp.size(); ++i)
	{
		if (IsZi(tmp[i]))
		{
			return false;
		}

		if (tmp[i]->m_number != 1 && tmp[i]->m_number != 9)
		{
			return false;
		}
	}

	return true;
}

bool CardManager::CheckNoHongzhong(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmp.push_back(outCard);
	}

	for (Lint i = 0; i < tmp.size(); ++i)
	{
		if (IsHongzhong(tmp[i]))
		{
			return false;
		}
	}

	return true;
}


bool CardManager::CheckJokerChickenHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;
	bool qiduihu = false;
	bool shisanyaohu = false;
	//检查是否可以胡十三幺
	if(CheckShisanyaoGhostType(handcard, pengCard,agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		vec.push_back(HU_SHISANYAO);
		shisanyaohu = true;
		result = true;
	}

	//检查七小队
	Lint isSpecial = 0;
	bool isQingQidui = false;
	if (!result && CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, isSpecial, isQingQidui))
	{
		qiduihu = true;
		result = true;
	}

	//检查是否可以普通胡
	if (!result)
	{
		if (Ghost_CheckHu(handcard,outCard,false,isGetCard, gameInfo.m_ghostCard, ghostType))
		{
			result = true;
		}
	}

	if (result)
	{
		bool singlehang = false;
		bool bmatchKe = false, bmatchNomal = false, bmatchQidui = false;
		//检查胡牌类型
		if ( CheckFlowerMatch(handcard, outCard, isGetCard, bmatchKe, bmatchNomal, bmatchQidui, shisanyaohu, qiduihu))
		{
			singlehang = true;
			if (outCard->m_color == CARD_COLOR_FLOWER)
			{
				//花吊花
				vec.push_back(HU_FLOWERMATCHFLOWER);
			}
			else
			{
				//花吊单
				vec.push_back(HU_FLOWERMATCHSINGLE);
			}
		}
		

		//杠上开花
		if (CheckGangshangkaihua(gameInfo))
		{
			vec.push_back(HU_GANGSHANGKAIHUA);
		}
		//可以胡，检查是否可以抢杠胡
		if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
		{
			//不可以单吊抢杠胡
			if (singlehang)
			{
				vec.clear();
				return false;
			}

			auto huType = gameInfo.m_hasDianGang ? HU_QIANGMINGGANGHU : HU_QIANGGANGHU;
			vec.push_back(huType);
		}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
		else
		{
			vec.push_back(HU_ZIMO);
		}

	}
	return result;
}

bool CardManager::CheckHuizhouzhuangGhostHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	bool result = false;
	bool haveHu = false;
	bool qiduihu = false;
	bool shisanyaohu = false;
	//检查是否可以胡十三幺
	if (CheckShisanyaoGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
	{
		shisanyaohu = true;
		haveHu = true;
	}

	//检查七小队
	Lint isSpecial = 0;
	bool isQingQidui = false;
	if (gameInfo.m_QiXiaoDui && CheckXiaoqidui(handcard, outCard, isGetCard, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, isSpecial, isQingQidui))
	{
		qiduihu = true;
		haveHu = true;
	}

	bool huCommon = false;
	if (Ghost_CheckHu(handcard, outCard, false, isGetCard, gameInfo.m_ghostCard, ghostType))
	{
		huCommon = true;
		haveHu = true;
	}

	bool flmatchfl = false;
	bool flmatchnomal = false;
	bool bRealHuFlMatch = false;
	if (haveHu)
	{
		bool bmatchKe = false, bmatchNomal = false, bmatchQiDui = false;
		Lint flowermatchType = HU_FLOWERMATCHSINGLE;
		//检查单吊
		if (CheckFlowerMatch(handcard, outCard, isGetCard, bmatchKe, bmatchNomal, bmatchQiDui, shisanyaohu, qiduihu))
		{
			flmatchnomal = true;
			if (outCard->m_color == CARD_COLOR_FLOWER)
			{
				flowermatchType = HU_FLOWERMATCHFLOWER;
				flmatchfl = true;
			}
		}

		//检查可胡多种类型的牌型
		bool bHuQingyise = CheckQingyiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);
		bool bHuHunyise = CheckHunYiseGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);
		bool bHuPengpenghu = CheckDuiduihuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard);

		if (shisanyaohu)
		{
			vec.push_back(HU_SHISANYAO);	//十三幺及其单吊
			bRealHuFlMatch = (flmatchnomal ? true :  false);
		}
		else if (CheckQingyaojiuGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_QINGYAOJIU);	//清幺九及其单吊
			bRealHuFlMatch = (flmatchnomal ? true : false);
		}
		else if (huCommon && CheckAllWindGhostType(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_ZIYISE);		//全风及其单吊
			bRealHuFlMatch = (flmatchnomal ? true : false);
		}
		else if (CheckYaoJiuHasOneOrNineHu(handcard, pengCard, agangCard, mgangCard, outCard, gameInfo.m_ghostCard))
		{
			vec.push_back(HU_YAOJIU);		//幺九及其单吊
			bRealHuFlMatch = (flmatchnomal ? true : false);
		}

		//以下胡牌类型有交叉，需要分开判断是否单吊
		else if (bHuQingyise && bHuPengpenghu && bmatchKe)	//清碰单吊
		{
			vec.push_back(HU_QINGPENG);
			bRealHuFlMatch = true;
		}
		else if (isQingQidui && bmatchQiDui)			//清七对单吊
		{
			vec.push_back(HU_QINGQIDUI);
			bRealHuFlMatch = true;
		}
		else if (bHuHunyise && bHuPengpenghu && bmatchKe)	//混碰单吊
		{
			vec.push_back(HU_HUNPENG);
			bRealHuFlMatch = true;
		}
		else if (qiduihu && bmatchQiDui)	//七对单吊
		{
			vec.push_back(HU_QIDUI);
			bRealHuFlMatch = true;
		}
		else if (bHuQingyise && bmatchNomal) //清一色单吊
		{
			vec.push_back(HU_QINGYISE);
			bRealHuFlMatch = true;
		}
		else if (bHuPengpenghu && bmatchKe) //碰碰胡单吊
		{
			vec.push_back(HU_PENGPENGHU);
			bRealHuFlMatch = true;
		}
		else if (bHuHunyise && bmatchNomal && flmatchfl) //混一色花吊花
		{
			vec.push_back(HU_HUNYISE);
			bRealHuFlMatch = true;
		}
		else if (bHuQingyise && bHuPengpenghu)//清碰
		{
			vec.push_back(HU_QINGPENG);
		}
		else if (qiduihu && bHuQingyise)	//清七对
		{
			vec.push_back(HU_QINGQIDUI);
		}
		else if (bHuHunyise && bHuPengpenghu) //混碰
		{
			vec.push_back(HU_HUNPENG);
		}
		else if (qiduihu)//七对
		{
			vec.push_back(HU_QIDUI);
		}
		else if (bHuQingyise)//清一色
		{
			vec.push_back(HU_QINGYISE);
		}
		else if (bHuPengpenghu)//碰碰胡
		{
			vec.push_back(HU_PENGPENGHU);
		}
		else if (bHuHunyise)//混一色
		{
			vec.push_back(HU_HUNYISE);
			bRealHuFlMatch = (flmatchnomal ? true : false);
		}
		else if ( gameInfo.m_canlittleHu) //鸡胡
		{
			vec.push_back(HU_JIHU);
			bRealHuFlMatch = (flmatchnomal ? true : false);
		}

		//记录单吊胡类型
		if (bRealHuFlMatch)
		{
			if (flmatchfl)
			{
				vec.push_back(HU_FLOWERMATCHFLOWER);
			}
			else
			{
				vec.push_back(HU_FLOWERMATCHSINGLE);
			}
		}
	}

	if (!vec.empty())
	{
		//杠上开花
		if (CheckGangshangkaihua(gameInfo))
		{
			vec.push_back(HU_GANGSHANGKAIHUA);
		}
		//可以胡，检查是否可以抢杠胡
		if (gameInfo.m_canqiangganghu && CheckQiangganghu(gameInfo))
		{
			//不可以单吊抢杠胡
			if (flmatchnomal)
			{
				vec.clear();
				return false;
			}
			auto huType = gameInfo.m_hasDianGang ? HU_QIANGMINGGANGHU : HU_QIANGGANGHU;
			vec.push_back(huType);
		}

		if (isGetCard)
		{
			vec.push_back(HU_ZIMO);
		}
		result = true;
	}
	
	return result;
}

bool CardManager::CheckAllWindGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
	}
	EraseCard(tmp, ghostCard);
	for(Lsize i = 0 ; i < tmp.size(); ++i)
	{
		if(!IsZi(tmp[i]))
		{
			return false;
		}
	}
	return true;
}

bool CardManager::CheckTingAllWindGhostType(bool bTingQidui, bool bTingDuidui, CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const std::vector<CardValue>& ghostCard) const
{
	if (!bTingDuidui && !bTingQidui)
	{
		return false;
	}

	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	EraseCard(tmp, ghostCard);
	for (Lsize i = 0; i < tmp.size(); ++i)
	{
		if (!IsZi(tmp[i]))
		{
			return false;
		}
	}
	return true;
}

bool CardManager::CheckYaoJiuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector check,one,two,three,four;
	check = handcard;
	if(outCard)
	{
		check.push_back(outCard);
		SortCard(check);
	}
	Lint ghostNum = EraseCard(check, ghostCard);
	GetSpecialOneTwoThreeFour(check,one,two,three,four);

	Lint oneNum = one.size();
	Lint twoNum = two.size() / 2;
	Lint threeNum = three.size() /3;
	Lint fourNum = four.size() / 4;

	Lint remainedGhostNum = ghostNum - oneNum - fourNum;

	if (remainedGhostNum < 0)
	{
		return false;
	}

	Lint needGhost = 0;
	Lint singleNum = oneNum + twoNum + fourNum;			//和鬼牌在一起，组成对子的数量
	if ( 0 == singleNum)
	{
		needGhost = 2;
	}
	else
	{
		needGhost = oneNum + twoNum + fourNum - 1;
	}

	if (remainedGhostNum - needGhost < 0)
	{
		return false;
	}

	Lint num = (remainedGhostNum - needGhost) % 3;  //去掉可以组成一副牌的鬼牌
	if (num)
	{
		return false;
	}

	one.clear();
	two.clear();
	three.clear();
	four.clear();

	check.insert(check.end(),pengCard.begin(),pengCard.end());
	check.insert(check.end(),gangcard.begin(),gangcard.end());
	check.insert(check.end(),mgangcard.begin(),mgangcard.end());
	GetSpecialOneTwoThreeFour(check,one,two,three,four);
	//检查是否都是幺九字
	for (Lint i = 0; i < check.size(); ++i)
	{
		if (check[i]->m_number != 1 && check[i]->m_number != 9 && !IsZi(check[i]))
		{
			return false;
		}
	}

	//检查是否有幺九刻子
	bool haveOneKe = false;
	bool haveNineKe = false;
	for (Lint i = 0; i < three.size();)
	{
		if (!IsZi(three[i]) && three[i]->m_number == 1)
		{
			haveOneKe = true;
		}
		if (!IsZi(three[i]) && three[i]->m_number == 9)
		{
			haveNineKe = true;
		}
		i += 3;
	}

	for (Lint i = 0; i < four.size();)
	{
		if (!IsZi(four[i]) && four[i]->m_number == 1)
		{
			haveOneKe = true;
		}
		if (!IsZi(four[i]) && four[i]->m_number == 9)
		{
			haveNineKe = true;
		}
		i += 4;
	}

	if (singleNum > 1)
	{
		CardVector singleCardVec;
		singleCardVec.insert(singleCardVec.end(), one.begin(), one.end());
		singleCardVec.insert(singleCardVec.end(), two.begin(), two.end());
		singleCardVec.insert(singleCardVec.end(), four.begin(), four.end());
		for (Lint i = 0; i < singleCardVec.size(); ++i)
		{
			if (!haveOneKe && !IsZi(singleCardVec[i]) && singleCardVec[i]->m_number == 1)
			{
				haveOneKe = true;
			}
			if (!haveNineKe && !IsZi(singleCardVec[i]) && singleCardVec[i]->m_number == 9)
			{
				haveNineKe = true;
			}
		}
	}

	//检查鬼牌
	if (remainedGhostNum == needGhost + 3)
	{
		if (!haveOneKe)
		{
			haveOneKe = true;
		}else if (!haveNineKe)
		{
			haveNineKe = true;
		}
	}else if (remainedGhostNum == needGhost + 6)
	{
		haveOneKe = true;
		haveNineKe = true;
	}


	if (haveOneKe && haveNineKe)
	{
		return true;
	}

	return false;
}

bool CardManager::CheckYaoJiuHasOneOrNineHu(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector check, one, two, three, four;
	check = handcard;
	if (outCard)
	{
		check.push_back(outCard);
		SortCard(check);
	}
	Lint ghostNum = EraseCard(check, ghostCard);
	GetSpecialOneTwoThreeFour(check, one, two, three, four);

	Lint oneNum = one.size();
	Lint twoNum = two.size() / 2;
	Lint threeNum = three.size() / 3;
	Lint fourNum = four.size() / 4;

	Lint remainedGhostNum = ghostNum - oneNum - fourNum;

	if (remainedGhostNum < 0)
	{
		return false;
	}

	Lint needGhost = 0;
	Lint singleNum = oneNum + twoNum + fourNum;			//和鬼牌在一起，组成对子的数量
	if (0 == singleNum)
	{
		needGhost = 2;
	}
	else
	{
		needGhost = oneNum + twoNum + fourNum - 1;
	}

	if (remainedGhostNum - needGhost < 0)
	{
		return false;
	}

	Lint num = (remainedGhostNum - needGhost) % 3;	//剩余的鬼能否组成整付的牌（顺子刻子）
	if (num)
	{
		return false;
	}

	one.clear();
	two.clear();
	three.clear();
	four.clear();

	check.insert(check.end(), pengCard.begin(), pengCard.end());
	check.insert(check.end(), gangcard.begin(), gangcard.end());
	check.insert(check.end(), mgangcard.begin(), mgangcard.end());
	GetSpecialOneTwoThreeFour(check, one, two, three, four);
	//检查是否都是幺九字
	for (Lint i = 0; i < check.size(); ++i)
	{
		if (check[i]->m_number != 1 && check[i]->m_number != 9 && !IsZi(check[i]))
		{
			return false;
		}
	}

	//检查有没有多余鬼做将
	if (remainedGhostNum == 2 && needGhost == 2 && two.size() == 0)
	{
		return true;
	}
	//检查有没有多余鬼做刻
	if (remainedGhostNum - needGhost  > 0)
	{
		return true;
	}
	//检查手牌是否有1或9
	for (Lint i = 0; i < check.size(); ++i)
	{
		if ((check[i]->m_number == 1 || check[i]->m_number == 9) && !IsZi(check[i]))
		{
			return true;
		}
	}

	return false;
}

bool CardManager::CheckQingyiseGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}

	EraseCard(tmp, ghostCard);
	if(tmp.empty())
	{
		return false;
	}

	Lint color = tmp.front()->m_color;
	if (color == CARD_COLOR_FENG || color == CARD_COLOR_JIAN || color == CARD_COLOR_FLOWER)
	{
		return false;
	}
	for(Lsize i = 1 ; i < tmp.size(); ++i)
	{
		if(tmp[i]->m_color != color)
			return false;
	}
	return true;
}

bool CardManager::CheckDuiduihuGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector check,one,two,three,four;
	check = handcard;
	if(outCard)
	{
		check.push_back(outCard);
		SortCard(check);
	}
	Lint ghostNum = EraseCard(check, ghostCard);
	GetSpecialOneTwoThreeFour(check,one,two,three,four);

	Lint oneNum = one.size();
	Lint twoNum = two.size() / 2;
	Lint threeNum = three.size() /3;
	Lint fourNum = four.size() / 4;

	Lint remainedGhostNum = ghostNum - oneNum - fourNum;

	if (remainedGhostNum < 0)
	{
		return false;
	}
	Lint needGhost = 0;
	Lint singleNum = oneNum + twoNum + fourNum;
	if (0 == singleNum)
	{
		needGhost = 2;
	}
	else
	{
		needGhost =  oneNum + twoNum + fourNum - 1;
	}
	
	if (remainedGhostNum - needGhost < 0)
	{
		return false;
	}

	Lint num = (remainedGhostNum - needGhost) % 3;	//组成一副牌后剩余鬼数量
	if (!num)
	{
		return true;
	}

	return false;
}

bool CardManager::CheckHunYiseGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(),pengCard.begin(),pengCard.end());
	tmp.insert(tmp.end(),gangcard.begin(),gangcard.end());
	tmp.insert(tmp.end(),mgangcard.begin(),mgangcard.end());
	if(outCard)
	{
		tmp.push_back(outCard);
		SortCard(tmp);
	}
	EraseCard(tmp, ghostCard);

	CardVector::iterator cit = tmp.begin();
	bool haveZi = false;
	for (; cit != tmp.end();)
	{
		if ((*cit)->m_color > CARD_COLOR_SUO)
		{
			cit = tmp.erase(cit);
			haveZi = true;
		}
		else
		{
			++cit;
		}
	}
	if (haveZi)
	{
		return IsSameColor(tmp);
	}
	return false;
}

bool CardManager::CheckQingDuiGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	bool duiduihu = CheckDuiduihuGhostType(handcard, pengCard, gangcard, mgangcard, outCard, ghostCard);
	bool qingyise = CheckQingyiseGhostType(handcard, pengCard, gangcard, mgangcard, outCard, ghostCard);
	if (duiduihu && qingyise)
	{
		return true;
	}
	return false;
}

bool CardManager::CheckHunpengGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	bool duiduihu = CheckDuiduihuGhostType(handcard, pengCard, gangcard, mgangcard, outCard, ghostCard);
	bool hunyise = CheckHunYiseGhostType(handcard, pengCard, gangcard, mgangcard, outCard, ghostCard);
	if (duiduihu && hunyise)
	{
		return true;
	}
	return false;
}

bool CardManager::CheckBigSanyuanGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector totalCards = handcard;
	Lint outCardsJianKeNum = GetOutCardsSpecifyColorKeNum(pengCard, gangcard, mgangcard, CARD_COLOR_JIAN);
	if (outCard)
	{
		totalCards.push_back(outCard);
		SortCard(totalCards);
	}
	Lint ghostNumTotal = EraseCard(totalCards, ghostCard);	//去掉鬼牌，并获取鬼牌数量

	CardVector sanyuan, restCards;
	GetSpecifyColorCards(totalCards, sanyuan, restCards, CARD_COLOR_JIAN);	//将去掉鬼牌后手牌拆分为三元牌与其他牌

	Lint totalSanYuanCardsNum = ghostNumTotal + sanyuan.size() + outCardsJianKeNum * 3;
	if (totalSanYuanCardsNum < 9)
	{
		return false;
	}

	CardVector oneSanyuan, twoSanyuan, threeSanyuan, fourSanyuan;
	GetSpecialOneTwoThreeFour(sanyuan, oneSanyuan, twoSanyuan, threeSanyuan, fourSanyuan);

	Lint oneNum = oneSanyuan.size();
	Lint twoNum = twoSanyuan.size() / 2;
	Lint threeNum = threeSanyuan.size() / 3;
	Lint fourNum = fourSanyuan.size() / 4;

	Lint ghostRest = ghostNumTotal - oneNum * 2 - twoNum;		//单张对子配成刻子
	if (ghostRest < 0)
	{
		return false;
	}

	//计算缺少刻子需要的鬼牌
	Lint haveKe = oneNum + twoNum + threeNum + fourNum + outCardsJianKeNum;
	Lint needKe = 3 - haveKe;
	Lint needGhostNum = needKe * 3;
	if (needGhostNum > ghostRest)	//凑不成两组
	{
		return false;
	}

	ghostRest -= needGhostNum;
	std::vector<Lint> card_vector(60, 0);
	//癞子,分数量统计
	for (int x = 0; x < restCards.size(); x++)
	{
		card_vector[(restCards[x]->m_color - 1) * 10 + (restCards[x]->m_number) * 1]++;
	}
	std::vector<Lint> tmp_cards(card_vector);
	bool result = false;
	if (fourNum > 0)
	{
		ghostRest = ghostRest - 2 * fourNum + 1;
		result = explorer_zheng_function(tmp_cards, 0, ghostRest);
		return result;
	}
	else
	{
		for (int x = 0; x < card_vector.size(); x++)
		{
			if (x % 10 == 0)
			{
				x++;
			}
			if (card_vector[x] > 1)
			{
				std::vector<Lint> tmp_cards(card_vector);
				tmp_cards[x] -= 2;
				if (explorer_zheng_function(tmp_cards, 0, ghostRest))
				{
					return true;
				}
			}
			else if (card_vector[x] == 1 && ghostRest > 0)
			{
				std::vector<Lint> tmp_cards(card_vector);
				tmp_cards[x] -= 1;
				if (explorer_zheng_function(tmp_cards, 0, ghostRest - 1))
				{
					return true;
				}
			}
			else
			{
				if (ghostRest > 1)
				{
					if (explorer_zheng_function(card_vector, 0, ghostRest - 2))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CardManager::CheckLittleSanyuanGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector totalCards = handcard;
	Lint outCardsJianKeNum = GetOutCardsSpecifyColorKeNum(pengCard, gangcard, mgangcard, CARD_COLOR_JIAN);
	if (outCard)
	{
		totalCards.push_back(outCard);
		SortCard(totalCards);
	}
	Lint ghostNumTotal = EraseCard(totalCards, ghostCard);	//去掉鬼牌，并获取鬼牌数量

	CardVector sanyuan, restCards;
	GetSpecifyColorCards(totalCards, sanyuan, restCards, CARD_COLOR_JIAN);	//将去掉鬼牌后手牌拆分为三元牌与其他牌

	Lint totalSanYuanCardsNum = ghostNumTotal + sanyuan.size() + outCardsJianKeNum * 3;
	if (totalSanYuanCardsNum < 8)
	{
		return false;
	}

	CardVector oneSanyuan, twoSanyuan, threeSanyuan, fourSanyuan;
	GetSpecialOneTwoThreeFour(sanyuan, oneSanyuan, twoSanyuan, threeSanyuan, fourSanyuan);

	Lint oneNum = oneSanyuan.size();
	Lint twoNum = twoSanyuan.size() / 2;
	Lint threeNum = threeSanyuan.size() / 3;
	Lint fourNum = fourSanyuan.size() / 4;

	Lint ghostRest = ghostNumTotal - oneNum;		//单张对子配成刻子
	if (ghostRest < 0)
	{
		return false;
	}

	//计算刻子
	Lint haveKe = 0;
	//计算将牌
	if (oneNum + twoNum > 0)
	{
		ghostRest = ghostRest - oneNum - twoNum + 1;
		haveKe = oneNum + twoNum + threeNum + fourNum + outCardsJianKeNum - 1;
	}
	else
	{
		haveKe = threeNum + fourNum + outCardsJianKeNum;
		ghostRest -= 2;
	}

	if (ghostRest < 0)
	{
		return false;
	}

	if (haveKe > 2)
	{
		return false;
	}
	Lint needKe = 2 - haveKe;
	Lint needGhostNum = needKe * 3;
	if (needGhostNum > ghostRest)	//凑不成两组
	{
		return false;
	}

	ghostRest -= needGhostNum;
	std::vector<Lint> card_vector(60, 0);
	for (int x = 0; x < restCards.size(); x++)
	{
		card_vector[(restCards[x]->m_color - 1) * 10 + (restCards[x]->m_number) * 1]++;
	}
	std::vector<Lint> tmp_cards(card_vector);
	bool result = false;
	if (fourNum > 0)
	{
		ghostRest = ghostRest - 2 * fourNum;
	}
	result = explorer_zheng_function(tmp_cards, 0, ghostRest);
	return result;
}

bool CardManager::CheckBigSixiGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector totalCards = handcard;
	Lint outCardsFengKeNum = GetOutCardsSpecifyColorKeNum(pengCard, gangcard, mgangcard, CARD_COLOR_FENG);
	if (outCard)
	{
		totalCards.push_back(outCard);
		SortCard(totalCards);
	}
	Lint ghostNumTotal = EraseCard(totalCards, ghostCard);	//去掉鬼牌，并获取鬼牌数量

	CardVector fengCards, restCards;
	GetSpecifyColorCards(totalCards, fengCards, restCards, CARD_COLOR_FENG);	//将去掉鬼牌后手牌拆分为三元牌与其他牌

	Lint totalFengCardsNum = ghostNumTotal + fengCards.size() + outCardsFengKeNum * 3;
	if (totalFengCardsNum < 12)
	{
		return false;
	}

	CardVector oneFengCards, twoFengCards, threeFengCards, fourFengCards;
	GetSpecialOneTwoThreeFour(fengCards, oneFengCards, twoFengCards, threeFengCards, fourFengCards);

	Lint oneNum = oneFengCards.size();
	Lint twoNum = twoFengCards.size() / 2;
	Lint threeNum = threeFengCards.size() / 3;
	Lint fourNum = fourFengCards.size() / 4;

	Lint ghostRest = ghostNumTotal - oneNum * 2 - twoNum;		//单张对子配成刻子
	if (ghostRest < 0)
	{
		return false;
	}

	//计算刻子
	Lint haveKe = oneNum + twoNum + threeNum + fourNum + outCardsFengKeNum;
	Lint needKe = 4 - haveKe;
	Lint needGhostNum = needKe * 3;
	if (needGhostNum > ghostRest)	//凑不成两组
	{
		return false;
	}

	ghostRest -= needGhostNum;
	std::vector<Lint> card_vector(60, 0);
	for (int x = 0; x < restCards.size(); x++)
	{
		card_vector[(restCards[x]->m_color - 1) * 10 + (restCards[x]->m_number) * 1]++;
	}
	std::vector<Lint> tmp_cards(card_vector);
	bool result = false;
	if (fourNum > 0)
	{
		ghostRest = ghostRest - 2 * fourNum + 1;
		result = explorer_zheng_function(tmp_cards, 0, ghostRest);
		return result;
	}
	else
	{
		for (int x = 0; x < card_vector.size(); x++)
		{
			if (x % 10 == 0)
			{
				x++;
			}
			if (card_vector[x] > 1)
			{
				std::vector<Lint> tmp_cards(card_vector);
				tmp_cards[x] -= 2;
				if (explorer_zheng_function(tmp_cards, 0, ghostRest))
				{
					return true;
				}
			}
			else if (card_vector[x] == 1 && ghostRest > 0)
			{
				std::vector<Lint> tmp_cards(card_vector);
				tmp_cards[x] -= 1;
				if (explorer_zheng_function(tmp_cards, 0, ghostRest - 1))
				{
					return true;
				}
			}
			else
			{
				if (ghostRest > 1)
				{
					if (explorer_zheng_function(card_vector, 0, ghostRest - 2))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CardManager::CheckLittleSixiGhostType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, const std::vector<CardValue>& ghostCard) const
{
	CardVector totalCards = handcard;
	Lint outCardsFengKeNum = GetOutCardsSpecifyColorKeNum(pengCard, gangcard, mgangcard, CARD_COLOR_FENG);
	if (outCard)
	{
		totalCards.push_back(outCard);
		SortCard(totalCards);
	}
	Lint ghostNumTotal = EraseCard(totalCards, ghostCard);	//去掉鬼牌，并获取鬼牌数量

	CardVector fengCards, restCards;
	GetSpecifyColorCards(totalCards, fengCards, restCards, CARD_COLOR_FENG);	//将去掉鬼牌后手牌拆分为三元牌与其他牌

	Lint totalFengCardsNum = ghostNumTotal + fengCards.size() + outCardsFengKeNum * 3;
	if (totalFengCardsNum < 11)
	{
		return false;
	}

	CardVector oneFeng, twoFeng, threeFeng, fourFeng;
	GetSpecialOneTwoThreeFour(fengCards, oneFeng, twoFeng, threeFeng, fourFeng);

	Lint oneNum = oneFeng.size();
	Lint twoNum = twoFeng.size() / 2;
	Lint threeNum = threeFeng.size() / 3;
	Lint fourNum = fourFeng.size() / 4;

	Lint ghostRest = ghostNumTotal - oneNum;		//单张配成对子
	if (ghostRest < 0)
	{
		return false;
	}

	//计算刻子
	Lint haveKe = 0; 
	if (oneNum + twoNum > 0)
	{
		ghostRest = ghostRest - oneNum - twoNum + 1;
		haveKe = oneNum + twoNum + threeNum + fourNum + outCardsFengKeNum - 1;
	}
	else
	{
		ghostRest -= 2;
		haveKe = threeNum + fourNum + outCardsFengKeNum;
	}

	if (ghostRest < 0 || haveKe > 3)
	{
		return false;
	}

	Lint needKe = 3 - haveKe;
	Lint needGhostNum = needKe * 3;
	if (needGhostNum > ghostRest)	//凑不成两组
	{
		return false;
	}

	ghostRest -= needGhostNum;
	std::vector<Lint> card_vector(60, 0);
	for (int x = 0; x < restCards.size(); x++)
	{
		card_vector[(restCards[x]->m_color - 1) * 10 + (restCards[x]->m_number) * 1]++;
	}
	std::vector<Lint> tmp_cards(card_vector);
	bool result = false;
	if (fourNum > 0)
	{
		ghostRest = ghostRest - 2 * fourNum;
	}
	result = explorer_zheng_function(tmp_cards, 0, ghostRest);
	return result;
}

bool CardManager::CheckEighteenarhtsType(CardVector& handcard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard) const
{
	CardVector tmp = handcard;
	tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
	tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
	tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		tmp.push_back(outCard);
	}
	if (tmp.size() == EIGHTEEN)
	{
		return true;
	}
	return false;
}

bool CardManager::CheckSpecialFlowerMatchType(const CardVector& handcard,  const Card* getCard, Lint ghostNum, bool shisanyaohu, bool qixiaohuihu) const
{
	if (shisanyaohu)
	{
		return CheckShisanyaoFlowerMatchType(handcard, getCard, ghostNum);
	}
	else if (qixiaohuihu)
	{
		return CheckQixiaoduiFlowerMatchType(handcard, getCard, ghostNum);
	}
	return false;
}

bool CardManager::CheckShisanyaoFlowerMatchType(const CardVector& handcard, const Card* getCard, Lint ghostNum) const
{

	if (ghostNum < 1)
	{
		return false;
	}

	if (NULL == getCard)
	{
		return false;
	}

	CardVector tmp = handcard;
	tmp.push_back(getCard);
	SortCard(tmp);

	CardVector one,two,three,four;
	GetSpecialOneTwoThreeFour(tmp,one,two,three,four);

	if (two.size() > 0)
	{
		return false;
	}

	return true;
}

bool CardManager::CheckQixiaoduiFlowerMatchType(const CardVector& handcard, const Card* getCard, Lint ghostNum) const
{
	CardVector tmp = handcard;
	SortCard(tmp);
	CardVector one,two,three,four;
	GetSpecialOneTwoThreeFour(tmp,one,two,three,four);
	if (one.size() + three.size() / 3 > ghostNum)
	{
		return false;
	}
	return true;
}

///NewhandCard 开始
void CardManager::DealCard2(NewHandCard& v1, NewHandCard& v2, NewHandCard& v3, NewHandCard& v4, CardVector& v5, const Card specialCard[CARD_COUNT], bool needWind, bool needBaiban, bool needHongzhong, bool needWan, bool needflower)const
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	if (!needflower)
	{
		mCards.erase(mCards.begin() + 136, mCards.end());	//保留白板及花牌（删除风、中发）
	}

	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiban)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 132);	//保留白板及花牌（删除风、中发）
		}
		else if (needHongzhong)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);	//先删除风
			mCards.erase(mCards.begin() + 112, mCards.end());			//保留红中
		}
		else
		{
			mCards.erase(mCards.begin() + 108, mCards.end());
		}
	}

	//判断要不要万
	if (!needWan)
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}


	CardVector cardtmp;
	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards.erase(mCards.begin() + seed1, mCards.begin() + seed1 + 1);
		nSize = mCards.size();
	}

	SwapCardBySpecial(cardtmp, specialCard);
	CardVector::iterator it = cardtmp.begin();
	for (; it != cardtmp.begin() + 13; ++it)
	{
		v1.AddCard(*it);
	}
	for (; it != cardtmp.begin() + 26; ++it)
	{
		v2.AddCard(*it);
	}

	for (; it != cardtmp.begin() + 39; ++it)
	{
		v3.AddCard(*it);
	}
	for (; it != cardtmp.begin() + 52; ++it)
	{
		v4.AddCard(*it);
	}
	v5.insert(v5.begin(), cardtmp.begin() + 52, cardtmp.end());

	//	v5.insert(v5.begin(), cardtmp.begin() + 52, cardtmp.begin() + 52+3);


	std::reverse(v5.begin(), v5.end());		//逆序桌上牌 
}

void CardManager::DealCard(NewHandCard& v1, NewHandCard& v2, NewHandCard& v3, NewHandCard& v4, CardVector& v5, bool needWind, bool needBaiban, bool needHongzhong, bool needWan, bool needflower)const
{
	//玩法再多的情况下需要重新实现发牌算法了，现在的策略容易导致错误
	//随机算法修改
	CardVector mCards = m_cardVec;
	//先判断要不要删除花牌
	if (!needflower)
	{
		mCards.erase(mCards.begin() + 136, mCards.end());	//保留白板及花牌（删除风、中发）
	}

	//判断要不要风，不要风可能需要白板或红中做鬼
	if (!needWind)
	{
		if (needBaiban)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 132);		//保留白板
		}
		else if (needHongzhong)
		{
			mCards.erase(mCards.begin() + 108, mCards.begin() + 124);	//保留中发白
			mCards.erase(mCards.begin() + 112, mCards.end());			//保留红中
		}
		else
		{
			mCards.erase(mCards.begin() + 108, mCards.end());
		}
	}
	//判断要不要万
	if (!needWan)
	{
		mCards.erase(mCards.begin(), mCards.begin() + 36);
	}

	CardVector cardtmp;
	Lint nSize = mCards.size();
	while (nSize > 0)
	{
		Lint seed1 = L_Rand(0, nSize - 1);
		cardtmp.push_back(mCards[seed1]);
		mCards[seed1] = mCards[nSize - 1];
		nSize--;
	}
	//周伟算法 再随机一边, 设置随机因子
	std::random_device rd;
	std::mt19937 engine(rd());
	std::shuffle(cardtmp.begin(), cardtmp.end(), engine);


	CardVector::iterator it = cardtmp.begin();
	for (; it != cardtmp.begin() + 13; ++it)
	{
		v1.AddCard(*it);
	}
	for (; it != cardtmp.begin() + 26; ++it)
	{
		v2.AddCard(*it);
	}

	for (; it != cardtmp.begin() + 39; ++it)
	{
		v3.AddCard(*it);
	}
	for (; it != cardtmp.begin() + 52; ++it)
	{
		v4.AddCard(*it);
	}
	v5.insert(v5.begin(), cardtmp.begin() + 52, cardtmp.end());
}
///NewhandCard 结束
bool CardManager::IsHave(const std::vector<Card>& vec, const Card& Card)
{
	for (Lsize i = 0; i < vec.size(); ++i)
	{
		if (vec[i].m_number == Card.m_number && vec[i].m_color == Card.m_color)
			return true;
	}
	return false;
}
bool CardManager::IsSame(const Card& c1, const Card& c2) const
{
	return c1.m_color == c2.m_color&&c1.m_number == c2.m_number;
}

bool CardManager::IsQingWan(const std::vector<Card>& result) const
{
	for (std::vector<Card>::const_iterator cit = result.begin(); cit != result.end(); ++cit)
	{
		if (1 != (*cit).m_color)
		{
			return false;
		}
	}
	return true;
}
bool CardManager::IsQingTiao(const std::vector<Card>& result) const
{
	for (std::vector<Card>::const_iterator cit = result.begin(); cit != result.end(); ++cit)
	{
		if (3 != (*cit).m_color)
		{
			return false;
		}
	}
	return true;
}
bool CardManager::IsQingTong(const std::vector<Card>& result) const
{
	for (std::vector<Card>::const_iterator cit = result.begin(); cit != result.end(); ++cit)
	{
		if (2 != (*cit).m_color)
		{
			return false;
		}
	}
	return true;
}
bool CardManager::IsQingZi(const std::vector<Card>& result) const
{
	for (std::vector<Card>::const_iterator cit = result.begin(); cit != result.end(); ++cit)
	{
		if (5 != (*cit).m_color && 4 != (*cit).m_color)
		{
			return false;
		}
	}
	return true;
}


bool CardManager::xiaohu_CheckHu_ConstRef(const NewHandCard& handcard, const Card& LaiCard, bool bHandCardHavePlainGhost, bool need258Jiang)
{
	Lint  card_vector[YINGBIN_SUANFA_VECLEN] = { 0 };

	//第一步获取癞子,分数量统计
	Lint Lai_num = handcard.Serializ2Vec4YBSuanFa(card_vector, GetCardID(LaiCard));

	//玩家出了癞子	 
	if (bHandCardHavePlainGhost)
	{
		int index = (LaiCard.m_color - 1) * 10 + (LaiCard.m_number);
		if (index < 0 || index >= YINGBIN_SUANFA_VECLEN)
		{
			return false;
		}
		card_vector[index]++;
		Lai_num--;
	}

	bool juge_lai_jiang = true;
	for (int x = 1; x < YINGBIN_SUANFA_VECLEN; x++)
	{
		int index = x % 10;
		if (x % 10 == 0)
		{
			continue;
		}

		if (need258Jiang && (index != 2 && index != 5 && index != 8))
		{
			if (x == 30)
				return false;
			continue;
		}

		if (card_vector[x] > 1)
		{
			card_vector[x] -= 2;
			if (explorer_zheng_function_ConstRef(card_vector, 0, Lai_num))
			{
				return true;
			}
			card_vector[x] += 2;
		}
		else if (card_vector[x] == 1 && Lai_num > 0)
		{
			card_vector[x] -= 1;
			if (explorer_zheng_function_ConstRef(card_vector, 0, Lai_num - 1))
			{
				return true;
			}
			card_vector[x] += 1;
		}
		else {
			if (Lai_num > 1 && juge_lai_jiang)
			{
				juge_lai_jiang = false;
				if (explorer_zheng_function_ConstRef(card_vector, 0, Lai_num - 2))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CardManager::explorer_zheng_function_ConstRef(const Lint(&cards)[YINGBIN_SUANFA_VECLEN], Lint index, Lint Lai_num)
{
	if (index < YINGBIN_SUANFA_VECLEN)
	{
		if (index % 10 == 0)
		{
			index++;
		}

		if (cards[index] > 0)
		{
			Lint  tmp_cards[YINGBIN_SUANFA_VECLEN];
			memcpy(tmp_cards, cards, YINGBIN_SUANFL_CHARLEN);
			Lint ke_dif = Lai_num - judge_ke_ModifyRef(tmp_cards, index);
			if (ke_dif >= 0)
			{
				if (explorer_zheng_function_ConstRef(tmp_cards, index, ke_dif))
					return true;
			}

			if (index % 10 < 8 && index < 30)
			{
				Lint  tmp_cards2[YINGBIN_SUANFA_VECLEN];
				memcpy(tmp_cards2, cards, YINGBIN_SUANFL_CHARLEN);
				Lint shun_dif = Lai_num - judge_shun_ModifyRef(tmp_cards2, index);
				if (shun_dif >= 0)
				{
					if (explorer_zheng_function_ConstRef(tmp_cards2, index, shun_dif))
						return true;
				}
			}
			return false;
		}
		else {
			if (Lai_num > 0 && index % 10 < 8 && index < 30)
			{
				Lint  tmp_cards[YINGBIN_SUANFA_VECLEN];
				memcpy(tmp_cards, cards, YINGBIN_SUANFL_CHARLEN);
				Lint shun_dif = Lai_num - judge_shun_ModifyRef(tmp_cards, index);
				if (shun_dif >= 0)
				{
					if (explorer_zheng_function_ConstRef(tmp_cards, index, shun_dif))
					{
						return true;
					}
				}
			}
			index++;
			if (explorer_zheng_function_ConstRef(cards, index, Lai_num))
			{
				return true;
			}
			return false;
		}
	}
	else
	{
		return true;
	}
}

Lint CardManager::judge_ke_ModifyRef(Lint(&targe_vector)[YINGBIN_SUANFA_VECLEN], Lint index)
{
	Lint Lai_num = 0;
	switch (targe_vector[index])
	{
	case 4:
	case 3:
	{
		targe_vector[index] -= 3;
		Lai_num = 0;
		break;
	}
	case 2:
	{
		targe_vector[index] -= 2;
		Lai_num = 1;
		break;
	}
	case 1:
	{
		targe_vector[index] -= 1;
		Lai_num = 2;
		break;
	}
	case 0:
	{
		Lai_num = 3;
		break;
	}
	default:
		return 100;
		break;
	}
	return Lai_num;
}

Lint CardManager::judge_shun_ModifyRef(Lint(&targe_vector)[YINGBIN_SUANFA_VECLEN], Lint index)
{
	Lint Lai_num = 3;
	if (targe_vector[index] > 0)
	{
		targe_vector[index]--;
		Lai_num--;
	}
	if (targe_vector[index + 1] > 0)
	{
		targe_vector[index + 1]--;
		Lai_num--;
	}
	if (targe_vector[index + 2] > 0)
	{
		targe_vector[index + 2]--;
		Lai_num--;
	}
	return Lai_num;
}


bool CardManager::CheckDandiao(const CardVector& handcard) const
{
	if (handcard.size() == 1)
	{
		return true;
	}
	return false;
}

//所有的handcard都没有添加鬼牌
bool CardManager::LongMenZiyouzhuang_GhostCheckHu(CardVector handcard, const Card* curCard, bool is_get_card, const std::vector<CardValue>& ghostCard, Lint ghostType, Lint handGhostNum) const
{
	if (ghostType == PT_GHOSTCARDNO)
	{
		return CheckHu(handcard, curCard, false, is_get_card);
	}

	if (curCard && !is_get_card)
	{
		CardValue card;
		card.m_color = curCard->m_color;
		card.m_number = curCard->m_number;
		std::vector<CardValue>::const_iterator cit_ghost = std::find(ghostCard.begin(), ghostCard.end(), card);
		if (cit_ghost != ghostCard.end())
		{
			return false;	//不能胡别人打的鬼牌
		}
	}

	if (curCard)
	{
		handcard.push_back(curCard);
	}
	SortCard(handcard);
	std::vector<Lint> card_vector(60, 0);

	//第一步获取癞子,分数量统计
	for (int x = 0; x < handcard.size(); x++)
	{
		card_vector[(handcard[x]->m_color - 1) * 10 + (handcard[x]->m_number) * 1]++;
	}

	bool juge_lai_jiang = true;
	for (int x = 0; x < card_vector.size(); x++)
	{
		if (x % 10 == 0)
		{
			x++;
		}
		if (card_vector[x] > 1)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x] -= 2;
			if (explorer_zheng_function(tmp_cards, 0, handGhostNum))
			{
				return true;
			}
		}
		else if (card_vector[x] == 1 && handGhostNum > 0)
		{
			std::vector<Lint> tmp_cards(card_vector);
			tmp_cards[x] -= 1;
			if (explorer_zheng_function(tmp_cards, 0, handGhostNum - 1))
			{
				return true;
			}
		}
		else
		{
			if (handGhostNum > 1 && juge_lai_jiang)
			{
				juge_lai_jiang = false;
				if (explorer_zheng_function(card_vector, 0, handGhostNum - 2))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool CardManager::LongMenZiyouzhuang_CheckShisanyaoGhostType(CardVector handcard, CardVector pengCard, const Card* outCard, const Lint handGhostNum) const
{
	bool result = LongMenZiyouzhuang_NormalShisanyaoType(handcard, outCard, handGhostNum);
	if (!result)
	{
		result = LongMenZiyouzhuang_SpecialShisanyaoType(handcard, pengCard, outCard, handGhostNum, true);
	}
	if (!result)
	{
		result = LongMenZiyouzhuang_SpecialShisanyaoType(handcard, pengCard, outCard, handGhostNum, false);
	}
	return result;
}
//该函数必须配合LongMenZiyouzhuang_GhostCheckHu使用
bool CardManager::LongMenZiyouzhuang_CheckDaJiHu(CardVector handcard, const CardVector& pengCard,const CardVector& gangCard,const CardVector& mgangCard, const Card* currCard, Lint handGhostNum) const
{
	if (currCard)
	{
		handcard.push_back(currCard);
	}
	handcard.insert(handcard.end(), pengCard.begin(), pengCard.end());
	handcard.insert(handcard.end(), gangCard.begin(), gangCard.end());
	handcard.insert(handcard.end(), mgangCard.begin(), mgangCard.end());

	SortCard(handcard);
	bool isFanzi = false;	//龙门自由庄实际生效的幡子只有风箭
	bool isJiben = false;	//基本牌型
	Lint color = 0;			//基本牌型的颜色
	for (auto it:handcard)
	{
		//在LongMenZiyouzhuang_GhostCheckHu 判断可胡的情况下，只要同时有幡子和一种基本牌型就胡
		if (IsZi(it))
			isFanzi = true;
		else 
		{
			if (color == 0) {
				color = it->m_color;
				isJiben = true;
			}
			else if (color != it->m_color)
			{
				return false;
			}
		}
	}
	if (!isFanzi)	
	{
		if (handGhostNum >= 2)	//LongMenZiyouzhuang_GhostCheckHu已识别为可胡
		{
			isFanzi = true;
			handGhostNum -= 2;
		}
		else return false;
	}
	if (!isJiben)
	{
		if (handGhostNum >= 2)	//LongMenZiyouzhuang_GhostCheckHu已识别为可胡
		{
			isJiben = true;
			handGhostNum -= 2;
		}
		else return false;
	}
	if (isFanzi && isJiben)
	{
		return true;
	}
	return false;
}
bool CardManager::LongMenZiyouzhuang_CheckDuiduihuGhostType(CardVector handcard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard, const Lint ghostNum) const
{
	CardVector  one, two, three, four;
	if (outCard)
	{
		handcard.push_back(outCard);
	}
	SortCard(handcard);

	GetSpecialOneTwoThreeFour(handcard, one, two, three, four);
	Lint oneNum = one.size();
	Lint twoNum = two.size() / 2;
	Lint threeNum = three.size() / 3;
	Lint fourNum = four.size() / 4;

	Lint remainedGhostNum = ghostNum - oneNum - fourNum;

	if (remainedGhostNum < 0)
	{
		return false;
	}
	Lint needGhost = 0;
	Lint singleNum = oneNum + twoNum + fourNum;
	if (0 == singleNum)
	{
		needGhost = 2;
	}
	else
	{
		needGhost = oneNum + twoNum + fourNum - 1;
	}

	if (remainedGhostNum - needGhost < 0)
	{
		return false;
	}

	Lint num = (remainedGhostNum - needGhost) % 3;	//组成一副牌后剩余鬼数量
	if (!num)
	{
		return true;
	}

	return false;
}

bool CardManager::LongMenZiyouzhuang_CheckDaGeDaHu(CardVector handcard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard, Lint ghostNum) const
{
	if (outCard)
	{
		handcard.push_back(outCard);
	}
	handcard.insert(handcard.end(), pengCard.begin(), pengCard.end());
	handcard.insert(handcard.end(), gangcard.begin(), gangcard.end());
	handcard.insert(handcard.end(), mgangcard.begin(), mgangcard.end());
	SortCard(handcard);

	Lint color = 0;			//基本牌型的颜色
	for (auto pcard:handcard)
	{
		if (IsZi(pcard)) return false;
		if (color == 0) {
			color = pcard->m_color;
		}
		else if (color != pcard->m_color)
		{
			return false;
		}		
	}
	//至多一个对子
	CardVector one, two, three, four;
	GetSpecialOneTwoThreeFour(handcard, one, two, three, four);
	Lint oneNum = one.size();
	Lint twoNum = two.size() / 2 + four.size() / 2;
	Lint threeNum = three.size() / 3;
	if (threeNum == 0)
	{
		return false;
	}

	if (twoNum == 0)
	{
		if (oneNum >= 1 && ghostNum >= 1)
		{
			oneNum--;
			ghostNum--;
			twoNum = 1;
		}
		else if (ghostNum >= 2)
		{
			ghostNum -= 2;
			twoNum = 1;
		}
		if (twoNum == 0)	//没有拼成对子
			return false;
	}

	if (oneNum > 0)
	{
		if (ghostNum < oneNum*2)
			return false;
		ghostNum -= oneNum * 2;
	}
	if (twoNum > 1)
	{
		if (ghostNum < twoNum - 1)	//需要留一个对子做将
			return false;
		ghostNum -= twoNum;
	}
	if (ghostNum > 0 && ghostNum % 3 != 0)
		return false;

	return true;	
}

bool CardManager::LongMenZiyouzhuang_CheckFloweryaojiu(CardVector handcard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard) const
{
	handcard.insert(handcard.end(), pengCard.begin(), pengCard.end());
	handcard.insert(handcard.end(), gangcard.begin(), gangcard.end());
	handcard.insert(handcard.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		handcard.push_back(outCard);
	}
	SortCard(handcard);

	//有幺有九有幡子
	bool isOne = false;
	bool isNine = false;
	bool isFanzi = false;
	for (auto pcard : handcard)
	{
		if (pcard->m_number == 9)
		{
			isNine = true;
		}
		else if (pcard->m_number == 1 && !IsZi(pcard))
		{
			isOne = true;
		}
		else if (IsZi(pcard))
		{
			isFanzi = true;
		}
		else {
			return false;
		}
	}
	if (isOne && isNine && isFanzi)
	{
		return true;
	}
	return false;
}
/*
幺九牌必须同花色出现一次，剩余的是幡子
*/
bool CardManager::LongMenZiyouzhuang_CheckChunyaojiu(CardVector handcard,const CardVector& pengCard,const CardVector& gangcard,const CardVector& mgangcard, const Card* outCard, Lint ghostNum) const
{
	handcard.insert(handcard.end(), pengCard.begin(), pengCard.end());
	handcard.insert(handcard.end(), gangcard.begin(), gangcard.end());
	handcard.insert(handcard.end(), mgangcard.begin(), mgangcard.end());
	if (outCard)
	{
		handcard.push_back(outCard);
	}
	SortCard(handcard);
	//bool isNineSameColorWithOne = true;
	bool isOne  = false;
	bool isNine = false;
	bool isFanzi = false;
	const Card* pCardOne  = NULL;	//一牌
	Lint color = 0;			//基本牌型的颜色
	for (auto pcard : handcard)
	{
		if (IsZi(pcard))
		{
			isFanzi = true;
		}
		else  //万筒条
		{
			if (color == 0)
			{
				color = pcard->m_color;
			}
			else if (color != pcard->m_color)
			{
				return false;
			}	
			if (pcard->m_number == 1)
			{
				isOne = true;
			}
			else if (pcard->m_number == 9)
			{
				isNine = true;
			}
			else
			{
				return false;
			}
		}
	}
	if (!isNine)
	{
		if (ghostNum < 2) return false;
		ghostNum -= 2;
		isNine = true;
	}
	if (!isOne)
	{
		if (ghostNum < 2) return false;
		ghostNum -= 2;
		isOne = true;
	}
	if (!isFanzi)
	{
		if (ghostNum < 2) return false;
		ghostNum -= 2;
		isFanzi = true;
	}

	if (isNine && isOne && isFanzi)
	{
		return true;
	}
	return false;
}

bool CardManager::LongMenZiyouzhuang_NormalShisanyaoType(CardVector handcard, const Card* outCard, const Lint handGhostNum) const
{
	if (outCard)
	{
		handcard.push_back(outCard);
	}
	SortCard(handcard);

	if (handcard.size() + handGhostNum != 14)
	{
		return false;
	}

	CardVector::const_iterator it_shisanyao = m_ShiSanYiaoVec.begin();
	for (; it_shisanyao != m_ShiSanYiaoVec.end(); ++it_shisanyao)
	{
		CardVector::const_iterator it_hand = handcard.begin();
		for (; it_hand != handcard.end(); )
		{
			if (IsSame(*it_hand, *it_shisanyao))
			{
				it_hand = handcard.erase(it_hand);
				break;
			}
			else
			{
				++it_hand;
			}
		}
	}

	if (handcard.size() > 1)
	{
		return false;
	}
	if (handcard.size() == 1)
	{
		CardVector::const_iterator it_remind_begin = handcard.begin();
		CardVector::const_iterator it_shisanyao = m_ShiSanYiaoVec.begin();
		for (; it_shisanyao != m_ShiSanYiaoVec.end(); ++it_shisanyao)
		{
			if (IsSame(*it_remind_begin, *it_shisanyao))
			{
				return true;
			}
		}
		if (it_shisanyao == m_ShiSanYiaoVec.end())
		{
			return false;
		}
	}
	return true;
}
bool CardManager::LongMenZiyouzhuang_SpecialShisanyaoType(CardVector handcard, const CardVector& pengCard, const Card* outCard, Lint handGhostNum, bool isYaojiuOrFanzi) const
{
	if (outCard)
	{
		handcard.push_back(outCard);
	}
	handcard.insert(handcard.end(),pengCard.begin(),pengCard.end());

	if (handcard.size() + handGhostNum != 14)
	{
		return false;
	}
	SortCard(handcard);

	Lint cardNum = handcard.size();
	Lint duiziNum = 0;
	for (Lint i = 0; i < cardNum ; )
	{
		if (isYaojiuOrFanzi && (!IsNineOne(handcard[i]) || IsZi(handcard[i])))		//纯幺九十三幺
		{
			return false;
		}
		else if (!isYaojiuOrFanzi && !IsZi(handcard[i]))	//幡子十三幺
		{
			return false;
		}

		if (i+2 < cardNum && IsSame(handcard[i], handcard[i + 2]))
		{
			i += 3;
			continue;
		}	
		if (i+1 < cardNum && IsSame(handcard[i], handcard[i + 1]))
		{
			if (duiziNum >= 1)
			{
				if (handGhostNum >= 1)	handGhostNum--;
				else  return false;   //鬼牌不足无法组成刻子，返回失败
			}
			else duiziNum = 1;
			i += 2;
		}
		else
		{
			if (duiziNum == 0)
			{
				if (handGhostNum >= 1)	handGhostNum--;
				else  return false;   //鬼牌不足无法组成第一个对子，返回失败
				duiziNum = 1;
				i += 1;
				continue;
			}
			if (handGhostNum >= 2) handGhostNum -= 2;
			else return false;      //鬼牌不足无法组成刻子，返回失败
			i += 1;
		}
	}
	if (handGhostNum > 0)
	{
		if ((duiziNum == 0 && handGhostNum == 2)|| (duiziNum == 1 && handGhostNum == 3))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CardManager::LongMenZiyouzhuang_CheckHunpengGhostType(const CardVector& handcard, const CardVector& pengCard, const CardVector& gangcard, const CardVector& mgangcard, const Card* outCard, const Lint ghostNum) const
{
	bool duiduihu = LongMenZiyouzhuang_CheckDuiduihuGhostType(handcard, pengCard, gangcard, mgangcard, outCard, ghostNum);
	bool hunyise = LongMenZiyouzhuang_CheckHunYiSe(handcard, pengCard, gangcard, mgangcard, outCard);
	if (duiduihu && hunyise)
	{
		return true;
	}
	return false;
}

bool CardManager::LongMenZiyouzhuang_CheckHunYiSe(CardVector handcard,const CardVector& pengCard, const CardVector& gangcard,const CardVector& mgangcard,const Card* outCard) const
{
	if (outCard)
	{
		handcard.push_back(outCard);
	}
	handcard.insert(handcard.end(), pengCard.begin(), pengCard.end());
	handcard.insert(handcard.end(), gangcard.begin(), gangcard.end());
	handcard.insert(handcard.end(), mgangcard.begin(), mgangcard.end());
	if (handcard.empty())
	{
		return false;
	}
	SortCard(handcard);
	//CardVector::const_iterator cit = tmp.begin();
	CardVector::iterator cit = handcard.begin();
	bool haveZi = false;
	for (; cit != handcard.end();)
	{
		if ((*cit)->m_color > CARD_COLOR_SUO)
		{
			cit = handcard.erase(cit);
			haveZi = true;
		}
		else
		{
			++cit;
		}
	}
	if (haveZi)
	{
		return IsSameColor(handcard);
	}
	return false;
}

bool CardManager::CheckHuiYangHu(CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, const Card* outCard, OperateState& gameInfo, std::vector<Lint>& vec, bool isGetCard, int ghostType /*= 0*/) const
{
	std::vector<Lint> vecHuType;
	int nHuType = 0;
	//七花胡判断
	if (isGetCard && gameInfo.m_bHuSevenFlower)
	{
		if (gameInfo.m_canTianhu)
		{
			vec.push_back(HU_TIANHU);
		}
		else if (gameInfo.m_canDihu)
		{
			vec.push_back(HU_DIHU);
		}
		else
		{
			vec.push_back(HU_SEVENFLOWER);
		}
		return true;
	}

	Lint nSpecial = 0;
	bool isQingQidui = false;
	//这里七对可以胡点炮
	bool bQidui = gameInfo.m_QiXiaoDui &&  CheckXiaoqidui(handcard, outCard, true, ghostType, gameInfo.m_ghostCard, gameInfo.m_zimoType, nSpecial, isQingQidui);
	//先检查十三幺 十三幺是特殊牌型，CheckHu判断不了
	bool bShisanyao = CheckShisanyao(handcard, outCard);
	if (!bQidui && !bShisanyao && !CheckHu(handcard, outCard, false, isGetCard))
	{
		return false;
	}
	//门清
	bool bMenqing = CheckMenQing(handcard, eatCard, pengCard, agangCard, mgangCard, outCard);
	bool bqingyaojiu = false;

	if (bShisanyao)
	{
		nHuType = HU_SHISANYAO;
	}
	//全风
	else if (CheckZiyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = HU_ZIYISE;
	}
	else if (CheckHuiyangyaojiu(handcard, eatCard, pengCard, agangCard, mgangCard, outCard, bqingyaojiu))
	{
		if (bqingyaojiu)
		{
			nHuType = bMenqing ? HU_MENQINGQINGYAOJIU : HU_QINGYAOJIU;
		}
		else
		{
			nHuType = bMenqing ? HU_MENQINGHUNYAOJIU : HU_HUNYAOJIU;
		}
	}
	else if (LongMenZiyouzhuang_CheckDaGeDaHu(handcard, pengCard, agangCard, mgangCard, outCard, 0))
	{
		nHuType = HU_DAGEDA;
	}
	else if (CheckQingPeng(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = bMenqing ? HU_MENQINGQINGPENG : HU_QINGPENG;
	}
	else if (CheckHunPeng(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = bMenqing ? HU_MENQINGHUNPENG : HU_HUNPENG;
	}
	else if (CheckQingyise(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = bMenqing ? HU_MENQINGQINGYISE : HU_QINGYISE;
	}
	else if (bQidui)
	{
		nHuType = HU_QIDUI;
	}
	else if (CheckPengpenghu(handcard, eatCard, outCard))
	{
		nHuType = HU_PENGPENGHU;
	}
	else if (CheckHunYiSe(handcard, eatCard, pengCard, agangCard, mgangCard, outCard))
	{
		nHuType = HU_HUNYISE;
	}
	else if(isGetCard)		//平胡不可以点炮
	{
		nHuType = HU_PING;
	}
	else
	{
		return false;
	}
	//天胡、地胡判断
	if (gameInfo.m_canTianhu)
	{
		nHuType = HU_TIANHU;
	}
	else if (gameInfo.m_canDihu)
	{
		nHuType = HU_DIHU;
	}

	vecHuType.push_back(nHuType);
	//抢杠胡
	if (CheckQiangganghu(gameInfo))
	{
		vecHuType.push_back(HU_QIANGGANGHU);
	}
	//自摸
	if (isGetCard)
	{
		vecHuType.push_back(HU_ZIMO);
	}
	vec = vecHuType;
	return true;
}

bool CardManager::CheckHuiyangyaojiu(CardVector& handcard, CardVector& eatCard, CardVector& pengCard, CardVector& gangcard, CardVector& mgangcard, const Card* outCard, bool& qingyise) const
{
	if (CheckPengpenghu(handcard, eatCard, outCard))
	{
		CardVector tmp = handcard;
		tmp.insert(tmp.end(), pengCard.begin(), pengCard.end());
		tmp.insert(tmp.end(), gangcard.begin(), gangcard.end());
		tmp.insert(tmp.end(), mgangcard.begin(), mgangcard.end());

		qingyise = true;
		for (Lsize i = 0; i < tmp.size(); ++i)
		{
			if (!IsNineOne(tmp[i]) && !IsZi(tmp[i]))
			{
				return false;
			}
			if (IsZi(tmp[i]))
			{
				qingyise = false;
			}
		}
		return true;
	}

	return false;
}