#include "GuangdongJihuHandler.h"

void GuangdongJihuPlayerHu::DeakCard()
{
	if (!m_desk || !m_desk->m_vip)
	{
		return;
	}
	for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
	{
		m_handCard[i].clear();
		m_outCard[i].clear();
		m_pengCard[i].clear();
		m_gangCard[i].clear();
		m_abombCard[i].clear();
		m_startHu[i].clear();
		m_eatCard[i].clear();
		m_thinkInfo[i].Reset();
		m_thinkRet[i].Clear();
		m_gangRelation[i].clear();
	}
	m_deskCard.clear();
	m_curOutCard = NULL;//当前出出来的牌
	m_curGetCard = NULL;
	m_needGetCard = false;
	m_isThinkingGang = false;
	m_curPos = m_zhuangpos;
	m_beforePos = DEFAULT_INVAILD_POS;
	memset(m_angang, 0, sizeof(m_angang));//暗杠数量
	memset(m_minggang, 0, sizeof(m_minggang));
	memset(m_diangang, 0, sizeof(m_diangang));
	memset(m_adiangang, 0, sizeof(m_adiangang));
	memset(m_louHuCard, 0, sizeof(m_louHuCard));
	memset(m_gangPos, 0, sizeof(m_gangPos));
	memset(m_playerHuInfo, 0, sizeof(m_playerHuInfo));
	memset(m_playerBombInfo, 0, sizeof(m_playerBombInfo));
	memset(m_maNum, 0, sizeof(m_maNum));
	memset(m_ghostZimo, 0, sizeof(m_ghostZimo));
	memset(m_noghostZimo, 0, sizeof(m_noghostZimo));
	m_ghostCardFlop.m_color = 0;
	m_ghostCardFlop.m_number = 0;
	m_ghostCardReal.clear();
	m_gengzhuangInfo.clear();
	m_gengZhengOnceEnd = false;
	//
	bool needWind = IsNeedWindCards();
	UpdateLastWaiteThinkRetTime();
	//发牌   
	if (gConfig.GetDebugModel() && (m_desk->m_specialCard[0].m_color > 0 && m_desk->m_specialCard[0].m_number > 0))   //玩家指定发牌 牌局
	{
		gCardMgr.DealGuangdongJiHuCard2(m_handCard[0], m_handCard[1], m_handCard[2], m_handCard[3],  m_deskCard, m_desk->m_specialCard, needWind);
	}
	else                //正常随机发牌 牌局
	{
		gCardMgr.DealGuangdongJiHuCard(m_handCard[0], m_handCard[1], m_handCard[2], m_handCard[3], m_deskCard, needWind);
	}

	gCardMgr.SortCard(m_handCard[m_curPos]);
	//发送消息给客户端
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (m_desk->m_user[i] != NULL)
		{
			LMsgS2CPlayStart msg;
			msg.m_zhuang = m_curPos;
			msg.m_pos = i;

			for (Lsize j = 0; j < m_handCard[i].size(); ++j)
			{
				msg.m_cardValue[j].m_number = m_handCard[i][j]->m_number;
				msg.m_cardValue[j].m_color = m_handCard[i][j]->m_color;
			}

			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				msg.m_cardCount[j] = m_handCard[j].size();
			}
			msg.m_dCount = (Lint)m_deskCard.size();
			m_desk->m_user[i]->Send(msg);
		}
	}

	//录像功能
	m_video.Clear();
	Lint id[DEFAULT_DESK_USER_COUNT];
	Ldouble score[DEFAULT_DESK_USER_COUNT];
	std::vector<CardValue> vec[DEFAULT_DESK_USER_COUNT];
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		id[i] = m_desk->m_user[i]->GetUserDataId();
		score[i] = m_desk->m_vip->GetUserScore(m_desk->m_user[i]);
		for (Lint j = 0; j < m_handCard[i].size(); ++j)
		{
			CardValue v;
			v.m_color = m_handCard[i][j]->m_color;
			v.m_number = m_handCard[i][j]->m_number;
			vec[i].push_back(v);
		}
	}
	m_video.DealCard(id, vec, gWork.GetCurTime().Secs(), m_zhuangpos, score, m_desk->GetDeskId(), m_desk->m_vip->m_curCircle, m_desk->m_vip->m_maxCircle, m_desk->getGameType(), &m_desk->getPlayType());
}

void GuangdongJihuPlayerHu::calGenZhuangScore(Ldouble gold[])
{
	if (m_gengzhuangInfo.m_nSuccTimes > 0)
	{
		Lint gengzhuangScore = (0x1 << m_gengzhuangInfo.m_nSuccTimes) - 1;
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (i != m_zhuangpos)
			{
				gold[i] += gengzhuangScore;
			}
		}
		gold[m_zhuangpos] -= (m_deskplayer_num - 1) * gengzhuangScore;
	}
}

bool GuangdongJihuPlayerHu::IsLiuju()
{
	if (m_deskCard.empty())
	{
		return true;
	}
	return false;
}

void GuangdongJihuPlayerHu::calGuangdongJihuHorseNum(LMsgS2CZhaBirdShow &maPaiStruct, Lint winpos[], Lint maNum[], bool& bHorseHitAll)
{
	Lint LeftCardNum = m_deskCard.size();
	//全中
	if (LeftCardNum == 0)
	{
		Lint nHitHorse = GetTotalHorseNum();
		for (Lint userindex = 0; userindex < m_deskplayer_num; ++userindex)
		{
			if (WIN_SUB_DZIMO == winpos[userindex] || WIN_SUB_ZIMO == winpos[userindex] || WIN_SUB_DBOMB == winpos[userindex] || WIN_SUB_BOMB == winpos[userindex])
			{
				maNum[userindex] = nHitHorse;
			}
		}
		bHorseHitAll = true;

		std::vector<CardValue> cards;
		CardValue card;
		cards.push_back(card);
		m_video.AddOper(VIDEO_OPER_HIT_ALL_HORSE, nHitHorse, cards);
		return;
	}
	//中马计算
	bool bHitLastCard = false;
	const Card* lastCard = m_deskCard.back();
	const Lint nTotalHorseNum = GetTotalHorseNum();
	for (Lint i = 0; i < nTotalHorseNum; ++i)
	{
		BirdInfo maPaiInfo;
		maPaiInfo.m_pos = -1;
		for (Lint userindex = 0; userindex < m_deskplayer_num; ++userindex)
		{
			if (WIN_SUB_DZIMO == winpos[userindex] || WIN_SUB_ZIMO == winpos[userindex] || WIN_SUB_DBOMB == winpos[userindex] || WIN_SUB_BOMB == winpos[userindex])
			{
				if (calcMaPai(m_zhuangpos, userindex, lastCard->m_number))
				{
					maNum[userindex]++;
					maPaiInfo.m_pos = userindex;
					maPaiStruct.m_end_pos = userindex;
				}
			}
		}
		maPaiInfo.m_card.m_color = lastCard->m_color;
		maPaiInfo.m_card.m_number = lastCard->m_number;
		maPaiStruct.m_bird_infos.push_back(maPaiInfo);

		if (!m_deskCard.empty())
		{
			m_deskCard.pop_back();
		}
		
		if (!m_deskCard.empty())
		{
			lastCard = m_deskCard.back();
		}
	}
	//录像
	std::vector<CardValue> cards;
	for (Lint i = 0; i < maPaiStruct.m_bird_infos.size(); ++i)
	{
		cards.push_back(maPaiStruct.m_bird_infos[i].m_card);
	}
	m_video.AddOper(VIDEO_OPEN_BIRD_SHOW, maPaiStruct.m_end_pos, cards);
}

bool GuangdongJihuPlayerHu::calcScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[], Lint& bombCount, Lint& zhuangpos, Lint maNum[], LMsgS2CGameOver& over, Lint winner)
{
	UpdateZhuangPos(result, winpos, bombpos, zhuangpos);
	calGangScore(result, gold);
	calHorsrScore(result, winpos, bombpos, gold, maNum, over, winner);
	calCardTypeScore(result, winpos, bombpos, gold);
	calGenZhuangScore(gold);
	return true;
}

Lint GuangdongJihuPlayerHu::GetSocore(const Lint pos)
{
	Lint base = 2;
	if (GetGhostCardNum(pos) == 0 && !IsNoGhostType())
	{
		base += 2;
	}
	return base;
}

bool GuangdongJihuPlayerHu::IsGenZhangOnlyOnce() const
{
	return false;
}

void GuangdongJihuPlayerHu::calBuyHorseScore(Lint winpos[], Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over)
{
	if (IsNoHorseType())
	{
		return;
	}
	//马牌个数计算
	LMsgS2CZhaBirdShow maPaiStruct;
	bool bHorseHitAll = false;
	calGuangdongJihuHorseNum(maPaiStruct, winpos, maNum, bHorseHitAll);
	if (bHorseHitAll)
	{
		over.m_nHorseHitAll = GetTotalHorseNum();
	}
	else
	{
		over.m_bird_infos.insert(over.m_bird_infos.end(), maPaiStruct.m_bird_infos.begin(), maPaiStruct.m_bird_infos.end());
	}
	

	bool bPayQiangganghorseScore = IsPayQianggangHorseScore();
	Lint maNumZimo = 0;
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (IsQiangGangHu(m_thinkRet[i].m_hu) && bPayQiangganghorseScore) //抢杠胡，包马牌分
		{
			//马牌得分
			Lint maPaiBaseScore = GetMaPaiBaseScore(i);
			gold[i] += maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
			gold[bombpos] -= maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
		}
		else if (WIN_SUB_DZIMO == winpos[i] || WIN_SUB_ZIMO == winpos[i] || WIN_SUB_DBOMB == winpos[i])
		{
			//马牌得分
			Lint maPaiBaseScore = GetMaPaiBaseScore(i);
			gold[i] += maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				if (i != j)
				{
					gold[j] -= maPaiBaseScore * maNum[i];
				}
			}
		}
		else if (WIN_SUB_BOMB == winpos[i])
		{
			//马牌得分
			Lint maPaiBaseScore = GetMaPaiBaseScore(i);
			gold[i] += maPaiBaseScore * maNum[i];
			gold[bombpos] -= maPaiBaseScore * maNum[i];
		}
	}
	return;
}

Lint GuangdongJihuPlayerHu::GetTotalHorseNum()
{
	Lint ret = GameBuyhorseNum();

	if (IsSteadilyHighType())
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (m_successive[i] > 2)
			{
				ret += (m_successive[i] - 2) * 2;
				return ret;
			}
		}
	}
	
	return ret;
}

