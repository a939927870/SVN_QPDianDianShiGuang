#include "GuangDongTuiDaoHuHandler.h"

void GameHandler_GuangDongTuiDaoHu::DeakCard()
{
	if (!m_desk || !m_desk->m_vip)
	{
		return;
	}
	for(Lint i = 0 ; i < DEFAULT_DESK_USER_COUNT; ++i)
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
		m_louHuCard[i] = NULL;
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
	memset(m_gangPos,0,sizeof(m_gangPos));
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
	bool needBaiBan = IsGhostCardBaiban();
	bool needWan = true;

	UpdateLastWaiteThinkRetTime();
	memset(m_nTrusteeshipCount, 0, sizeof(m_nTrusteeshipCount));
	memset(m_bUserInTrusteeship, 0, sizeof(m_bUserInTrusteeship));

	//发牌
	switch (m_deskplayer_num)
	{
	case 2:
		if (gConfig.GetDebugModel() && (m_desk->m_specialCard[0].m_color > 0 && m_desk->m_specialCard[0].m_number > 0))   //玩家指定发牌 牌局
		{
			gCardMgr.DealTwoPlayerCard2(m_handCard[0], m_handCard[1], m_deskCard, m_desk->m_specialCard);
		}
		else                //正常随机发牌 牌局
		{
			gCardMgr.DealTwoPlayerCard(m_handCard[0], m_handCard[1], m_deskCard);
		}
		break;
	case 3:
		needWan = !IsNoWanCards();
		gCardMgr.DealCard(m_handCard, m_deskplayer_num, m_deskCard, m_desk->m_specialCard, needWind, needBaiBan, false, needWan);
		break;
	case 4:
	default:
		gCardMgr.DealCard(m_handCard, m_deskplayer_num, m_deskCard, m_desk->m_specialCard, needWind, needBaiBan, false, true);
		break;
	}

	//发送消息给客户端
	for(Lint i = 0 ; i < m_deskplayer_num; ++i)
	{
		if(m_desk->m_user[i] != NULL)
		{
			LMsgS2CPlayStart msg;
			msg.m_zhuang = m_curPos;
			msg.m_pos = i;

			for(Lsize j = 0 ; j < m_handCard[i].size(); ++j)
			{
				msg.m_cardValue[j].m_number = m_handCard[i][j]->m_number;
				msg.m_cardValue[j].m_color = m_handCard[i][j]->m_color;
			}

			for(Lint j = 0 ; j < m_deskplayer_num; ++j)
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
	m_video.DealCard(id, vec,gWork.GetCurTime().Secs(),m_zhuangpos,score,m_desk->GetDeskId(),m_desk->m_vip->m_curCircle,m_desk->m_vip->m_maxCircle,m_desk->getGameType(), &m_desk->getPlayType());
}
//无鬼加倍
bool GameHandler_GuangDongTuiDaoHu::IsNoGhost2TimesScore() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_NOGHOST2TIMESSCORE)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongTuiDaoHu::IsNoWanCards() const
{
	vector<int> pt = m_desk->getPlayType();
	vector<int>::iterator it = std::find(pt.begin(), pt.end(), (int)PT_NOWAN);
	if (it != pt.end())
		return true;
	return false;
}

Lint GameHandler_GuangDongTuiDaoHu::GetMultipleFromCard(const CardValue& card)
{
	if (card.m_color >= CARD_COLOR_FENG)
	{
		return 10;
	}
	else
	{
		return card.m_number;
	}
}

Lint GameHandler_GuangDongTuiDaoHu::GetSocore(const Lint pos)
{
	if (!IsValidPos(pos))
	{
		return 0;
	}

	Lint base = 2;
	std::vector<Lint> hu = m_thinkRet[pos].m_hu;
	if (IsQiduiDoubleScore())
	{
		for(Lint i = 0; i < hu.size(); ++i)
		{
			if(HU_QIDUI == hu[i])
			{
				base *= 2;
			}
		}
	}

	if (IsSteadilyHighType())
	{
		Lint num = 0;
		num = m_successive[pos] > 2?m_successive[pos] - 2:0;
		base += num * 2;
	}

	if (IsNoGhost2TimesScore() && (GetGhostCardNum(pos) == 0))
	{
		base *= 2;
		if(std::find(m_thinkRet[pos].m_hu.begin(),m_thinkRet[pos].m_hu.end(),HU_NOGHOST2TIMES) == m_thinkRet[pos].m_hu.end())
		{
			m_thinkRet[pos].m_hu.push_back(HU_NOGHOST2TIMES);
		}
	}
	return base;
}

void GameHandler_GuangDongTuiDaoHu::GenerateGhost()
{
	if (GetGhostType() == PT_GHOSTCARDFLOPTWO)
	{
		GenerateTwoGhost();
	}
	else if (GetGhostType() == PT_GHOSTCARDFLOP)
	{
		GenerateOneGhost();
	}
	else if (GetGhostType() == PT_GHOSTCARDBAIBAN)
	{
		CardValue ghost1;
		ghost1.m_color = 5;
		ghost1.m_number = 3;
		m_ghostCardReal.push_back(ghost1);
	}
	//录像
	std::vector<CardValue> cards;
	CardValue cardValue;
	cardValue.m_color = m_ghostCardFlop.m_color;
	cardValue.m_number = m_ghostCardFlop.m_number;
	cards.push_back(cardValue);
	m_video.AddOper(VIDEO_GHOST_CARD, 0, cards);

	//发送到客户端
	LMsgS2CGhostCard ghoshCard;
	ghoshCard.m_ghostCard.m_color = m_ghostCardFlop.m_color;
	ghoshCard.m_ghostCard.m_number = m_ghostCardFlop.m_number;
	m_desk->BoadCast(ghoshCard);
}

void GameHandler_GuangDongTuiDaoHu::GenerateTwoGhost()
{
	const Card* card = m_deskCard.back();
	m_deskCard.pop_back();
	if (card)
	{
		m_ghostCardFlop = *card;
		CardValue ghost1;
		CardValue ghost2;
		Lint number1 = card->m_number + 1;
		Lint number2 = card->m_number + 2;
		if (!gCardMgr.IsZi(card))		//万筒条
		{
			ghost1.m_number = number1 % 9 == 0 ? 9: number1 % 9;
			ghost1.m_color = card->m_color;

			ghost2.m_number = number2 % 9 == 0 ? 9: number2 % 9;
			ghost2.m_color = card->m_color;
		}
		else if (card->m_color == CARD_COLOR_FENG)		//北风
		{
			ghost1.m_number = number1 % 4 == 0 ? 4: number1 % 4;
			ghost1.m_color = number1 > 4 ? CARD_COLOR_JIAN : CARD_COLOR_FENG;

			ghost2.m_number = number2 % 4 == 0 ? 4: number2 % 4;
			ghost2.m_color = number2 > 4 ? CARD_COLOR_JIAN : CARD_COLOR_FENG;
		}
		else if (card->m_color == CARD_COLOR_JIAN)		//白板
		{
			ghost1.m_number = number1 % 3 == 0 ? 3: number1 % 3;
			ghost1.m_color = number1 > 3 ? CARD_COLOR_FENG : CARD_COLOR_JIAN;

			ghost2.m_number = number2 % 3 == 0 ? 3: number2 % 3;
			ghost2.m_color =  number2 > 3 ? CARD_COLOR_FENG : CARD_COLOR_JIAN;
		}

		m_ghostCardReal.push_back(ghost1);
		m_ghostCardReal.push_back(ghost2);
	}
}

void GameHandler_GuangDongTuiDaoHu::GenerateOneGhost()
{
	const Card* card = m_deskCard.back();
	m_deskCard.pop_back();
	if (card)
	{
		m_ghostCardFlop = *card;
		CardValue ghost1;
		Lint number1 = card->m_number + 1;
		if (!gCardMgr.IsZi(card))		//万筒条
		{
			ghost1.m_number = number1 % 9 == 0 ? 9: number1 % 9;
			ghost1.m_color = card->m_color;
		}
		else if (card->m_color == CARD_COLOR_FENG)		//北风
		{
			ghost1.m_number = number1 % 4 == 0 ? 4: number1 % 4;
			ghost1.m_color = number1 > 4 ? CARD_COLOR_JIAN : CARD_COLOR_FENG;

		}
		else if (card->m_color == CARD_COLOR_JIAN)		//白板
		{
			ghost1.m_number = number1 % 3 == 0 ? 3: number1 % 3;
			ghost1.m_color = number1 > 3 ? CARD_COLOR_FENG : CARD_COLOR_JIAN;

		}
		m_ghostCardReal.push_back(ghost1);
	}
}

bool GameHandler_GuangDongTuiDaoHu::CanFourGhostHu() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_GHOSTCARDBAIBAN || *iTor == PT_GHOSTCARDFLOP)
			return true;
	}
	return false;
}

void GameHandler_GuangDongTuiDaoHu::FillspecifichuType(OperateState& gameinfo)
{
	return;
}

bool GameHandler_GuangDongTuiDaoHu::IsBombHorse() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_BOMBHORSE)
			return true;
	}
	return false;
}

void GameHandler_GuangDongTuiDaoHu::calBombHorseScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over)
{
	if (WIN_NONE == result)
	{
		return;
	}

	Lint LeftCardNum = m_deskCard.size();
	if (LeftCardNum <= 0)
	{
		return;
	}

	const Card* card = m_deskCard.back();
	m_deskCard.pop_back();

	//录像
	std::vector<CardValue> cards;
	CardValue cardValue;
	cardValue.m_color = card->m_color;
	cardValue.m_number = card->m_number;
	cards.push_back(cardValue);
	m_video.AddOper(VIDEO_OPEN_BIRD_SHOW, 0, cards);

	//Over消息
	BirdInfo maPaiInfo;
	maPaiInfo.m_pos = 0;
	maPaiInfo.m_card = cardValue;
	over.m_bird_infos.push_back(maPaiInfo);

	//分数计算
	bool bPayQiangganghorseScore = IsPayQianggangHorseScore();
	bool bPayGangshangkaihuahorseScore = IsGangshangkaihuaScore();
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (IsQiangGangHu(m_thinkRet[i].m_hu) && bPayQiangganghorseScore) //抢杠胡，包马牌分
		{
			//马牌得分
			Lint maPaiBaseScore = GetBombHorseBaseScore(i, cardValue);
			gold[i] += maPaiBaseScore * (m_deskplayer_num - 1);
			gold[bombpos] -= maPaiBaseScore * (m_deskplayer_num - 1);
		}
		else if (IsGangshangKaihuaHu(m_thinkRet[i].m_hu) && i != m_beforePos && m_beforeType != THINK_OPERATOR_AGANG && bPayGangshangkaihuahorseScore && IsValidPos(m_beforePos))
		{
			//马牌得分
			Lint maPaiBaseScore = GetBombHorseBaseScore(i, cardValue);
			gold[i] += maPaiBaseScore * (m_deskplayer_num - 1);
			gold[m_beforePos] -= maPaiBaseScore  * (m_deskplayer_num - 1);
		}
		else if (WIN_SUB_DZIMO == winpos[i] || WIN_SUB_ZIMO == winpos[i])
		{
			//马牌得分
			Lint maPaiBaseScore = GetBombHorseBaseScore(i, cardValue);
			gold[i] += maPaiBaseScore * (m_deskplayer_num - 1);
			for (Lint j = 0; j < m_deskplayer_num; ++j)
			{
				if (i != j)
				{
					gold[j] -= maPaiBaseScore;
				}
			}
		}
	}
	return;
}

Lint GameHandler_GuangDongTuiDaoHu::GetBombHorseBaseScore(Lint pos, const CardValue& card)
{
	Lint score = 0;
	Lint multiple = GetMultipleFromCard(card);
	if (IsBompHorseAddScoreType())
	{
		score += multiple * 2;
	}
	else if (IsBompHorseMultipleScoreType())
	{
		score = GetSocore(pos);
		score *= multiple;
	}
	else
	{
		return 0;
	}
	return score;
}
