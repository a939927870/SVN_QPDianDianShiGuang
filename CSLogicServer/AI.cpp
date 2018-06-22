#include "AI.h"


AI::AI()
{

}

AI::~AI()
{

}

void AI::CheckCard(CardVector& card)
{
	m_fourCard.clear();
	m_threeCard.clear();
	m_twoCard.clear();
	m_oneCard.clear();
	for(Lint i = 0 ; i < 14; ++i)
	{
		m_checkCard[i].clear();
	}

	gCardMgr.GetSpecialOneTwoThreeFour(card,m_oneCard,m_twoCard,m_threeCard,m_fourCard);
	
	CardVector tmp = m_oneCard;
	tmp.insert(tmp.end(),m_twoCard.begin(),m_twoCard.end()); 
	gCardMgr.SortCard(tmp);

	Lint number = 0,color = 0,length = 0;
	Lsize i = 0,index = 0;
	for( ; i < tmp.size(); ++i)
	{
		Card* card = tmp[i];
		if(card->m_color != color || card->m_number > number+2)
		{
			if(length > 0)
			{
				for(Lint j = length-1 ; j >= 0; --j)
				{
					m_checkCard[index].push_back(tmp[i-j-1]);
				}
				index += 1;
			}
			length = 1;
		}
		else 
		{
			length += 1;
		}
		color = card->m_color;
		number = card->m_number;
	}

	if(length > 0)
	{
		for(Lint j = length-1 ; j >= 0; --j)
		{
			m_checkCard[index].push_back(tmp[i-j-1]);
		}
		index += 1;
	}

	for(Lint j = 0 ; j < 14; ++j)
	{
		if(m_checkCard[j].size() >= 3)
		{
			CardVector tmp = m_checkCard[j];
			gCardMgr.EarseSameCard(tmp);

			if(tmp.size() >= 3)
			{
				CardVector cnt;
				for(Lsize n = 0 ; n < tmp.size()-3;)
				{
					if(tmp[n]->m_color == tmp[n+2]->m_color && 
						tmp[n]->m_number+2 == tmp[n+2]->m_number)
					{
						cnt.insert(cnt.end(),tmp.begin()+n,tmp.begin()+n+3);
						n += 3;
					}
					else
					{
						n += 1;
					}
				}

				if(cnt.size())
				{
					m_okCard.insert(m_okCard.end(),cnt.begin(),cnt.end());
					gCardMgr.EraseCard(m_checkCard[j],cnt);
				}
			}
		}
	}
}

Card* AI::CheckOutCard(CardVector& handcard, CardVector& eatCard, CardVector& bombCard, CardVector& abombCard,Lint& flag)
{
	Card* pCard = NULL;
	flag = 1;

	//首先检查出什么牌能听
	for (Lsize i = 0; i < handcard.size(); ++i)
	{
		CardVector tmp = handcard;
		tmp.erase(tmp.begin() + i);
		if (gCardMgr.CheckCanTing(tmp))
		{
			return handcard[i];
		}
	}

	CheckCard(handcard);

	//检查按杠
	if(m_fourCard.size())
	{
		flag = 2;
		pCard = m_fourCard.front();
		return pCard;
	}

	//检测单张
	for(Lint i = 0; i < 14; ++i)
	{
		if(m_checkCard[i].size() == 1)
		{
			pCard = m_checkCard[i].front();
			m_checkCard[i].clear();
			return pCard; 
		}
	}

	//卡张边张
	for (Lint i = 0; i < 14; ++i)
	{
		if (m_checkCard[i].size() == 2 && !gCardMgr.IsSame(m_checkCard[i]))
		{
			if (!gCardMgr.IsContinue(m_checkCard[i]) || gCardMgr.IsNineOne(m_checkCard[i][0]) || gCardMgr.IsNineOne(m_checkCard[i][1]))
			{
				pCard = m_checkCard[i].front();
				m_checkCard[i].erase(m_checkCard[i].begin());
				return pCard;
			}
		}
	}

	//检测，多余的一张
	for(Lint i = 0; i < 14; ++i)
	{
		if(m_checkCard[i].size()%3 == 1 && gCardMgr.IsContinue(m_checkCard[i]))
		{
			pCard = m_checkCard[i].front();
			if(gCardMgr.IsNineOne(pCard))
			{
				pCard = m_checkCard[i].back();
			}
			gCardMgr.EraseCard(m_checkCard[i],pCard);
			return pCard;
		}
	}

	for(Lint i = 0; i < 14; ++i)
	{
		if(m_checkCard[i].size() && !gCardMgr.IsContinue(m_checkCard[i]))
		{
			pCard = m_checkCard[i].front();
			m_checkCard[i].erase(m_checkCard[i].begin());
			return pCard;
		}
	}

	for(Lint i = 0; i < 14; ++i)
	{
		if(m_checkCard[i].size())
		{
			pCard = m_checkCard[i].front();
			m_checkCard[i].erase(m_checkCard[i].begin());
			return pCard;
		}
	}

	pCard = handcard.back();

	return pCard;
}

Lint AI::ThinkCard(CardVector& handcard, Card* outCard)
{

	//小于四张不让吃，碰，杠
	if(handcard.size() <= 1)
	{
		return THINK_OPERATOR_NULL;
	}

	if(gCardMgr.CheckCanHu(handcard,outCard))
	{
		return THINK_OPERATOR_BOMB;
	}

	if(gCardMgr.CheckCanGang(handcard,outCard))
	{
		return THINK_OPERATOR_GANG;
	}

	if(gCardMgr.CheckCanPeng(handcard,outCard))
	{
		return THINK_OPERATOR_PENG;
	}


	return THINK_OPERATOR_NO;
}
