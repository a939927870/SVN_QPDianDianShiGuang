#include "GuangDongOneHundredHandler.h"

bool GameHandler_GuangDongOneHundler::startup(Desk *desk)
{
	if (desk)
	{
		shutdown();
		m_desk = desk;
		SetDeskPlayNum();
	}
	return true;
}

void GameHandler_GuangDongOneHundler::shutdown(void)
{
	m_desk = NULL;
	m_curOutCard = NULL;
	m_curGetCard = NULL;
	for (Lint i = 0; i< DEFAULT_DESK_USER_COUNT; i ++)
	{
		m_louHuCard[i] = NULL;
		m_thinkInfo[i].Reset();
		m_thinkRet[i].Clear();
		m_startHu[i].clear();
		m_handCard[i].clear();
		m_outCard[i].clear();
		m_pengCard[i].clear();
		m_gangCard[i].clear();
		m_abombCard[i].clear();
		m_eatCard[i].clear();
		m_angang[i] = 0;
		m_minggang[i] = 0;
		m_diangang[i] = 0;
		m_adiangang[i] = 0;
		m_gangPos[i] = 0;
		m_playerHuInfo[i] = 0;
		m_playerBombInfo[i] = 0;
		m_maNum[i] = 0;
		m_ghostZimo[i] = 0;
		m_noghostZimo[i] = 0;
		m_successive[i] = (i==0? 1:0);
		m_redDragonNum[i] = 0;
		m_gangRelation[i].clear();
	}
	m_beforePos = 0;
	m_beforeType = 0;
	m_gold = 0;
	m_zhuangpos = 0;
	m_curPos = 0;
	m_endStartPos = 0;
	m_endStartCurPos = 0;
	m_needGetCard = false;
	m_isThinkingGang = false;
	m_deskCard.clear();
	m_curOutCards.clear();
	m_ghostCardFlop.m_color = 0;
	m_ghostCardFlop.m_number = 0;
	m_ghostCardReal.clear();
	m_oper_gang = false;
	m_gengzhuangInfo.clear();
	m_canTianHu = true;
	m_canDiHu = true;
	m_gengZhengOnceEnd = false;
	m_playCardNum = 0;
	m_operateNum = 0;
}

void GameHandler_GuangDongOneHundler::DeakCard()
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
	m_oper_gang = false;
	m_canTianHu = true;
	m_canDiHu = true;
	m_gengzhuangInfo.clear();
	m_gengZhengOnceEnd = false;
	m_playCardNum = 0;
	m_operateNum = 0;
	UpdateLastWaiteThinkRetTime();
	memset(m_nTrusteeshipCount, 0, sizeof(m_nTrusteeshipCount));
	memset(m_bUserInTrusteeship, 0, sizeof(m_bUserInTrusteeship));

	UpdateLastWaiteThinkRetTime();
	//发牌   
	gCardMgr.DealCard(m_handCard,m_deskplayer_num,m_deskCard,m_desk->m_specialCard, true, true, false, false);

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

void GameHandler_GuangDongOneHundler::SetDeskPlay()
{
	if (!m_desk || !m_desk->m_vip)
	{
		return;
	}
	m_desk->m_vip->SendInfo();

	m_desk->setDeskState(DESK_PLAY);
	DeakCard();

	//这里生成鬼牌
	GenerateGhost();

	CheckStartPlayCard();
}
void GameHandler_GuangDongOneHundler::ProcessRobot(Lint pos, User * pUser)
{
	if (pos < 0 || pos >(m_deskplayer_num - 1) || m_desk == NULL)
	{
		return;
	}
	switch(m_desk->getDeskPlayState())
	{
	case DESK_PLAY_START_HU:
		{
			if (m_startHu[pos].size() > 0)
			{
				LMsgC2SUserStartHuSelect msg;
				msg.m_type = START_HU_NULL;
				m_desk->HanderUserStartHu(pUser, &msg);
				return;
			}
		}
		break;
	case DESK_PLAY_GET_CARD:
	case DESK_PLAY_END_CARD:
		{
			//打出牌去
			if (m_curPos == pos)
			{
				LMsgC2SUserPlay msg;
				msg.m_thinkInfo.m_type = THINK_OPERATOR_OUT;
				CardValue card;
				card.m_color = m_handCard[pos][0]->m_color;
				card.m_number = m_handCard[pos][0]->m_number;

				for (size_t i = 0; i < m_handCard[pos].size(); ++i)
				{
					if (m_handCard[pos][i]->m_color == 3 && m_handCard[pos][i]->m_number == 3)
					{
						card.m_color = m_handCard[pos][i]->m_color;
						card.m_number = m_handCard[pos][i]->m_number;
						break;
					}
					else if (m_handCard[pos][i]->m_color == 1 && m_handCard[pos][i]->m_number == 5)
					{
						card.m_color = m_handCard[pos][i]->m_color;
						card.m_number = m_handCard[pos][i]->m_number;
						break;
					}
				}

				msg.m_thinkInfo.m_card.push_back(card);
				m_desk->HanderUserPlayCard(pUser, &msg);
			}
		}
		break;
	case DESK_PLAY_THINK_CARD:
		{
			if (m_thinkInfo[pos].NeedThink())
			{
				for (Lint i = 0; i < m_thinkInfo[pos].m_thinkData.size(); i ++)
				{
					if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
					{
						LMsgC2SUserOper msg;
						msg.m_think.m_type = THINK_OPERATOR_NULL;		//
						CardVector mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
						for (Lint j = 0; j <  mCard.size(); j++)
						{
							CardValue card;
							card.m_color = mCard[j]->m_color;
							card.m_number =  mCard[j]->m_number;
							msg.m_think.m_card.push_back(card) ;
						}
						m_desk->HanderUserOperCard(pUser, &msg);
						return;
					}
					else
					{
						LMsgC2SUserOper msg;
						msg.m_think.m_type = m_thinkInfo[pos].m_thinkData[i].m_type;
						CardVector mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
						for (Lint j = 0; j <  mCard.size(); j++)
						{
							CardValue card;
							card.m_color = mCard[j]->m_color;
							card.m_number =  mCard[j]->m_number;
							msg.m_think.m_card.push_back(card) ;
						}
						m_desk->HanderUserOperCard(pUser, &msg);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

void GameHandler_GuangDongOneHundler::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
{
	if (m_desk == NULL || pUser == NULL || msg == NULL)
	{
		LLOG_DEBUG("HanderUserEndSelect NULL ERROR ");
		return;
	}
	LMsgS2CUserPlay sendMsg;
	sendMsg.m_errorCode = 0;
	sendMsg.m_pos = m_curPos;
	sendMsg.m_cs_card = msg->m_thinkInfo;

	Lint pos = m_desk->GetUserPos(pUser);
	if(!IsValidPos(pos))
	{
		LLOG_ERROR("HanderUserPlayCard pos error %s", pUser->m_userData.m_nike.c_str());
		return;
	}

	if(pos != m_curPos)
	{
		sendMsg.m_errorCode = 1;
		pUser->Send(sendMsg);
		LLOG_ERROR("HanderUserPlayCard not my pos %d:%d", pos, m_curPos);
		return;
	}

	m_playCardNum++;

	if(msg->m_thinkInfo.m_type == THINK_OPERATOR_OUT)
	{
		if(m_thinkInfo[pos].NeedThink())
		{
			VideoDoing(99,pos,0,0);
		}
		if(msg->m_thinkInfo.m_card.size())
		{
			for (Lsize i = 0; i < m_handCard[pos].size(); ++i)
			{
				if (m_handCard[pos][i]->m_color == msg->m_thinkInfo.m_card[0].m_color && m_handCard[pos][i]->m_number == msg->m_thinkInfo.m_card[0].m_number)
				{
					m_curOutCard = m_handCard[pos][i];

					std::vector<CardValue> cards;
					CardValue card;
					card.m_color = m_curOutCard->m_color;
					card.m_number = m_curOutCard->m_number;
					cards.push_back(card);

					UpdateGenZhuangInfo(m_curOutCard, pos, cards);
					gCardMgr.EraseCard(m_handCard[pos], m_curOutCard);
					m_desk->BoadCast(sendMsg);
					m_beforePos = pos;
					m_beforeType = THINK_OPERATOR_OUT;
					//录像
					m_video.AddOper(VIDEO_OPER_OUT_CARD, pos, cards);
					//这里玩家思考
					SetThinkIng();
					break;
				}
			}
		}

		m_canDiHu = false;
		return;
	}

	m_canDiHu = false;
	ThinkUnit* unit = NULL;
	for(Lint i = 0 ; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
	{
		if(msg->m_thinkInfo.m_type == m_thinkInfo[pos].m_thinkData[i].m_type)
		{
			if(msg->m_thinkInfo.m_card.size() == m_thinkInfo[pos].m_thinkData[i].m_card.size())
			{
				bool find = true;
				for(Lsize j = 0 ; j < msg->m_thinkInfo.m_card.size() ; ++j)
				{
					if(msg->m_thinkInfo.m_card[j].m_color != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color ||
						msg->m_thinkInfo.m_card[j].m_number != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number)
					{
						find = false;
						break;
					}
				}

				if(find)
				{
					unit = &m_thinkInfo[pos].m_thinkData[i];
					break;
				}
			}
		}
	}

	if(unit)
	{
		if(!m_gengzhuangInfo.m_isCircleEnd)
		{
			m_gengzhuangInfo.m_isCircleEnd = true;
		}
		
		if(unit->m_type == THINK_OPERATOR_BOMB)
		{
			//录相;
			VideoDoing(unit->m_type,pos,0,0);

			if (m_curGetCard)
			{
				gCardMgr.EraseCard(m_handCard[pos], m_curGetCard);
				sendMsg.m_huCard.m_color = m_curGetCard->m_color;
				sendMsg.m_huCard.m_number = m_curGetCard->m_number;
			}
			sendMsg.m_hu = unit->m_hu;
			sendMsg.m_huCard.m_color = m_curGetCard->m_color;
			sendMsg.m_cardCount = m_handCard[pos].size();
			for (Lint i = 0; i < sendMsg.m_cardCount; i ++)
			{
				CardValue mCard;
				mCard.m_color = m_handCard[pos][i]->m_color;
				mCard.m_number = m_handCard[pos][i]->m_number;
				sendMsg.m_cardValue.push_back(mCard);
			}
			m_desk->BoadCast(sendMsg);

			m_thinkRet[m_curPos] = m_thinkInfo[m_curPos].m_thinkData[0];
			m_playerHuInfo[m_curPos] = IsGangshangKaihuaHu(m_thinkRet[m_curPos].m_hu)? WIN_SUB_DZIMO : WIN_SUB_ZIMO;
			OnGameOver(WIN_ZIMO, m_playerHuInfo, m_deskplayer_num, 1);
		}
		else if(unit->m_type == THINK_OPERATOR_AGANG)
		{
			//录相;
			VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
			//
			gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0],4);	
			m_desk->BoadCast(sendMsg);
			m_angang[pos] += 1;

			//录像
			std::vector<CardValue> cards;
			for (Lint i = 0; i < 4; i ++)
			{
				CardValue card;
				card.m_color = unit->m_card[0]->m_color;
				card.m_number = unit->m_card[0]->m_number;
				cards.push_back(card);
				m_abombCard[pos].push_back(unit->m_card[0]);
			}
			m_video.AddOper(VIDEO_OPER_AN_GANG, pos, cards);
			//这里玩家思考
			m_beforePos = pos;
			m_beforeType = THINK_OPERATOR_AGANG;
			SetPlayIng(pos, true, true, true, true);
		}
		else if(unit->m_type == THINK_OPERATOR_MGANG)
		{
			//录相;
			VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
			//m_desk->BoadCast(sendMsg);
			m_curOutCard = unit->m_card[0];
			gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0],1);	

			//这里玩家思考
			m_beforePos = pos;
			m_beforeType = THINK_OPERATOR_MGANG;
			SetThinkIng();
		}
	}
	else
	{
		LLOG_DEBUG("Desk::HanderUserPlayCard %s,%d", pUser->m_userData.m_nike.c_str(), msg->m_thinkInfo.m_type);
	}
}

void GameHandler_GuangDongOneHundler::HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg)
{
	if (msg == NULL)
	{
		return ;
	}
	LMsgS2CUserOper sendMsg;
	sendMsg.m_pos = m_curPos;
	sendMsg.m_think = msg->m_think;

	Lint pos = m_desk->GetUserPos(pUser);
	if (!IsValidPos(pos) || !m_thinkInfo[pos].NeedThink())
	{
		sendMsg.m_errorCode = 1;
		pUser->Send(sendMsg);
		LLOG_DEBUG("HanderUserEndSelect pos error pos: %d     curPos:%d", pos, m_endStartCurPos);
		return;
	}

	bool find = false;
	Lsize thinkDataSize = m_thinkInfo[pos].m_thinkData.size();
	for(Lsize i = 0 ; i < thinkDataSize; ++i)
	{
		if(m_thinkInfo[pos].m_thinkData[i].m_type == msg->m_think.m_type)
		{
			bool check = true;
			if(m_thinkInfo[pos].m_thinkData[i].m_card.size() == msg->m_think.m_card.size())
			{
				for(Lsize j = 0 ; j < msg->m_think.m_card.size(); ++j)
				{
					if(msg->m_think.m_card[j].m_color != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color || 
						msg->m_think.m_card[j].m_number != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number)
					{
						check = false;
						break;
					}
				}

				if(check)
				{
					m_thinkRet[pos] = m_thinkInfo[pos].m_thinkData[i];
					find = true;
					break;
				}
			}
		}
	}

	if (!find)
	{
		m_thinkRet[pos].m_type = THINK_OPERATOR_NULL;
		if (m_thinkInfo[pos].HasHu())//漏胡
		{
			m_louHuCard[pos] = m_curOutCard;
		}
	}

	//录相;
	VideoDoing(msg->m_think.m_type,pos,(msg->m_think.m_card.size()>0)?msg->m_think.m_card[0].m_color:0,(msg->m_think.m_card.size()>0)?msg->m_think.m_card[0].m_number:0);

	if(msg->m_think.m_type == THINK_OPERATOR_MGANG && IsMingGangkeqiang())
	{
		m_oper_gang = true;
		m_curPos = pos;
		m_beforeType = THINK_OPERATOR_MGANG;
		SetThinkIng();
		return;
	}

	//设置以及思考过了
	m_thinkInfo[pos].Reset();
	CheckThink();
}

void GameHandler_GuangDongOneHundler::OnUserReconnect(User* pUser)
{
	if (pUser == NULL || m_desk == NULL)
	{
		return;
	}
	Lint pos = m_desk->GetUserPos(pUser);
	if (!IsValidPos(pos))
	{
		LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
		return;
	}
	Lint nCurPos = m_curPos;
	Lint nDeskPlayType = m_desk->getDeskPlayState();
	LMsgS2CDeskState reconn;
	reconn.m_state = m_desk->getDeskState();
	reconn.m_pos = nCurPos;
	reconn.m_time = 15;
	reconn.m_zhuang = m_zhuangpos;
	reconn.m_myPos = pos;
	if(nCurPos != pos)
	{
		reconn.m_flag = 0;
	}
	else
	{
		reconn.m_flag = 1;			//不知道对不对
	}
	reconn.m_dCount = m_deskCard.size();
	for(Lint i = 0; i < m_deskplayer_num; ++i)
	{
		reconn.m_cardCount[i] = m_handCard[i].size();
		reconn.m_oCount[i] = m_outCard[i].size();
		reconn.m_aCount[i] = m_abombCard[i].size();
		reconn.m_mCount[i] = m_gangCard[i].size();
		reconn.m_pCount[i] = m_pengCard[i].size();
		reconn.m_eCount[i] = m_eatCard[i].size();
		reconn.m_flowerCount[i] = m_flowerCard[i].size();

		for(Lsize j = 0 ; j < m_outCard[i].size(); ++j)
		{
			reconn.m_oCard[i][j].m_color = m_outCard[i][j]->m_color;
			reconn.m_oCard[i][j].m_number = m_outCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_abombCard[i].size(); ++j)
		{
			reconn.m_aCard[i][j].m_color = m_abombCard[i][j]->m_color;
			reconn.m_aCard[i][j].m_number = m_abombCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_gangCard[i].size(); ++j)
		{
			reconn.m_mCard[i][j].m_color = m_gangCard[i][j]->m_color;
			reconn.m_mCard[i][j].m_number = m_gangCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_pengCard[i].size(); ++j)
		{
			reconn.m_pCard[i][j].m_color = m_pengCard[i][j]->m_color;
			reconn.m_pCard[i][j].m_number = m_pengCard[i][j]->m_number;
		}

		for(Lsize j = 0 ; j < m_eatCard[i].size(); ++j)
		{
			reconn.m_eCard[i][j].m_color = m_eatCard[i][j]->m_color;
			reconn.m_eCard[i][j].m_number = m_eatCard[i][j]->m_number;
		}

		for (Lsize j = 0; j < m_flowerCard[i].size(); ++j)
		{
			reconn.m_flowerCard[i][j].m_color = m_flowerCard[i][j]->m_color;
			reconn.m_flowerCard[i][j].m_number = m_flowerCard[i][j]->m_number;
		}

		//连庄
		reconn.m_successive[i] = m_successive[i];
		reconn.m_redDragonNum[i] = m_redDragonNum[i];
	}

	//我的牌,客户的重连，之前莫得牌的重新拿出来发给他
	if((nDeskPlayType == DESK_PLAY_GET_CARD || nDeskPlayType == DESK_PLAY_END_CARD || nDeskPlayType == DESK_PLAY_SEVEN_FLOWER) && m_needGetCard && pos == nCurPos)
	{
		CardVector tmp = m_handCard[pos];
		if (m_curGetCard)
		{
			reconn.m_cardCount[pos] -= 1;
			gCardMgr.EraseCard(tmp,m_curGetCard);
		}
		for(Lsize j = 0 ;j < tmp.size(); ++j)
		{
			reconn.m_cardValue[j].m_color = tmp[j]->m_color;
			reconn.m_cardValue[j].m_number = tmp[j]->m_number;
		}
	}
	else
	{
		for(Lsize j = 0 ;j < m_handCard[pos].size(); ++j)
		{
			reconn.m_cardValue[j].m_color = m_handCard[pos][j]->m_color;
			reconn.m_cardValue[j].m_number = m_handCard[pos][j]->m_number;
		}
	}

	//该出牌的玩家，多发一张牌，用于打出去。
	if(m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD)
	{
		if (m_curOutCard && pos != nCurPos)
		{
			reconn.m_cardCount[nCurPos] ++;
		}
		else if(m_curOutCard&&pos == m_beforePos&& m_beforeType == THINK_OPERATOR_OUT)
		{
			reconn.m_cardCount[pos]++;
			reconn.m_oCount[pos]--;
			CardValue xx;
			xx.m_color = m_curOutCard->m_color;
			xx.m_number = m_curOutCard->m_number;
			reconn.m_cardValue[reconn.m_cardCount[pos]-1]=xx;
		}
	}
	//鬼牌
	reconn.m_ghostCard.m_color = m_ghostCardFlop.m_color;
	reconn.m_ghostCard.m_number = m_ghostCardFlop.m_number;
	pUser->Send(reconn);


	//起手胡牌
	if(nDeskPlayType == DESK_PLAY_START_HU)
	{
		if(m_startHu[pos].size())
		{
			LMsgS2CUserStartHu hu;
			for(Lint j = 0 ; j < m_startHu[pos].size(); ++j)
			{
				StartHuInfo info;
				info.m_type = m_startHu[pos][j].m_type;
				for(Lint n = 0 ; n <  m_startHu[pos][j].m_card.size(); ++n)
				{
					CardValue v;
					v.m_color = m_startHu[pos][j].m_card[n]->m_color;
					v.m_number = m_startHu[pos][j].m_card[n]->m_number;
					info.m_card.push_back(v);
				}
				hu.m_card.push_back(info);
			}
			m_desk->m_user[pos]->Send(hu);
		}
		return;
	}


	//我思考
	if(nDeskPlayType == DESK_PLAY_THINK_CARD)
	{
		if (m_thinkInfo[pos].NeedThink())
		{
			LMsgS2CThink think;
			think.m_time = 15;
			think.m_flag = 1;
			think.m_card.m_color = (m_curOutCard == NULL)? 1 : m_curOutCard->m_color;		//临时的 有错误 m_curOutCard->m_color;
			think.m_card.m_number = (m_curOutCard == NULL)? 1 : m_curOutCard->m_number;		//临时的 有错误m_curOutCard->m_number;
			Lsize thinkDataSize = m_thinkInfo[pos].m_thinkData.size();
			for (Lsize j = 0; j < thinkDataSize; ++j)
			{		
				ThinkData info;
				info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
				for(Lsize n = 0 ; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
				{
					CardValue v;
					v.m_color = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_color;
					v.m_number = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_number;
					info.m_card.push_back(v);
				}
				think.m_think.push_back(info);
			}
			pUser->Send(think);
		}
	}
	//我出牌
	if((nDeskPlayType == DESK_PLAY_GET_CARD || nDeskPlayType == DESK_PLAY_END_CARD || nDeskPlayType == DESK_PLAY_SEVEN_FLOWER) && m_needGetCard && pos == nCurPos)
	{
		LMsgS2COutCard msg;
		msg.m_time = 15;
		msg.m_pos = pos;
		msg.m_deskCard = (Lint)m_deskCard.size();
		msg.m_flag = m_curGetCard?0:1;
		msg.m_gang = m_gangPos[pos];
		msg.m_end = m_deskCard.size()==1?1:0;
		if(m_curGetCard)
		{
			msg.m_curCard.m_color = m_curGetCard->m_color;
			msg.m_curCard.m_number = m_curGetCard->m_number;
		}
		for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
		{		
			ThinkData info;
			info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
			for(Lsize n = 0 ; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
			{
				CardValue v;
				v.m_color = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_color;
				v.m_number = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_number;
				info.m_card.push_back(v);
			}
			msg.m_think.push_back(info);
		}
		pUser->Send(msg);
	}

	//桌面上的牌重新发给玩家的桌牌
	if(m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD && m_beforeType != THINK_OPERATOR_MGANG && m_beforeType != THINK_OPERATOR_AGANG)
	{
		if (m_curOutCard)
		{
			LMsgS2CUserPlay sendMsg;
			sendMsg.m_errorCode = 0;
			sendMsg.m_pos = nCurPos;
			sendMsg.m_cs_card.m_type = THINK_OPERATOR_OUT;
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			sendMsg.m_cs_card.m_card.push_back(card);
			pUser->Send(sendMsg);
		}
	}
}

void GameHandler_GuangDongOneHundler::CheckStartPlayCard()
{
	m_needGetCard = true;
	SetPlayIng(m_curPos, m_needGetCard, false, true, true);
}

//摸牌
void GameHandler_GuangDongOneHundler::SetPlayIng(Lint pos,bool needGetCard,bool gang, bool needThink, bool canhu,bool bu)
{
	if (m_desk == NULL )
	{
		LLOG_DEBUG("HanderUserEndSelect NULL ERROR ");
		return;
	}
	//穿庄
	if(IsLiuju())
	{
		LLOG_INFO("Desk::SetPlayIng huangzhuang game over");
		OnGameOver(WIN_NONE, m_playerHuInfo, m_deskplayer_num, NULL);
		return;
	}
	m_curPos = pos;

	if (m_curPos < 0 || m_curPos >= m_deskplayer_num)
	{
		LLOG_ERROR("m_curPos is error = %d.", m_curPos);
		return ;
	}

	//我摸牌思考信息
	m_thinkInfo[pos].m_thinkData.clear();
	m_louHuCard[pos] = NULL;
	m_desk->setDeskPlayState(DESK_PLAY_GET_CARD);
	m_needGetCard = false;
	if (needGetCard)
	{
		m_needGetCard = true;
		m_curGetCard = m_deskCard.back();
		m_deskCard.pop_back();
		//录像
		std::vector<CardValue> cards;
		CardValue card;
		card.m_color = m_curGetCard->m_color;
		card.m_number = m_curGetCard->m_number;
		cards.push_back(card);
		m_video.AddOper(VIDEO_OPER_GET_CARD, pos, cards);
	}
	if (needThink)
	{
		mGameInfo.m_GameType = m_desk->getGameType();	// 0 湖南， 3， 长沙
		mGameInfo.b_canEat = false;		// 是否可以吃
		mGameInfo.b_canHu = canhu;		// 是否可以胡
		mGameInfo.b_onlyHu = m_desk->getDeskPlayState() == DESK_PLAY_END_CARD;
		mGameInfo.m_thinkGang = gang;	// 单独处理是不是杠的牌
		mGameInfo.m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
		mGameInfo.m_playerPos = m_curPos;	// 当前一个出牌位置
		mGameInfo.m_cardState = THINK_OPERATOR_NULL;	// 当前一个出牌状态
		mGameInfo.m_endStartPos = m_endStartPos;	// 结束位置，即海底位置
		mGameInfo.m_MePos = pos;		// 玩家的位置
		mGameInfo.m_bZhuang = IsZhuang(pos);
		mGameInfo.m_qihuFanshu = GetQihuFanshu();
		if (IsXiaoQiDuiType())
			mGameInfo.m_QiXiaoDui = true;
		else
			mGameInfo.m_QiXiaoDui = false;

		if (CanQiangGanghu())
			mGameInfo.m_canqiangganghu = true;
		else
			mGameInfo.m_canqiangganghu = false;

		if (CanFourGhostHu())
			mGameInfo.m_canfourghosthu = true;
		else
			mGameInfo.m_canfourghosthu = false;

		mGameInfo.m_ghostType = GetGhostType();
		mGameInfo.m_ghostCard = m_ghostCardReal;

		//一百张胡牌类型
		FillspecifichuType(mGameInfo);
		if (pos == 0 && m_curGetCard && ((m_curGetCard->m_color == 3 && m_curGetCard->m_number == 4 ) || (m_curGetCard->m_color == 1 && m_curGetCard->m_number == 8)))
		{
			m_curGetCard = m_curGetCard;
		}
		m_thinkInfo[pos].m_thinkData = gCardMgr.CheckGetCardOperator(m_handCard[pos],m_pengCard[pos],m_abombCard[pos],m_gangCard[pos],m_eatCard[pos],m_curGetCard,mGameInfo);
		VideoThink(pos);
	}

	m_canTianHu = false;

	if (m_needGetCard)
	{
		m_handCard[pos].push_back(m_curGetCard);
		gCardMgr.SortCard(m_handCard[pos]);
	}

	if (gang)
	{
		m_gangPos[pos] = m_beforePos;
	}

	for(Lint i = 0 ; i < m_deskplayer_num; ++i)
	{
		if(m_desk->m_user[i] != NULL)
		{
			LMsgS2COutCard msg;
			msg.m_time = 15;
			msg.m_pos = pos;
			msg.m_deskCard = (Lint)m_deskCard.size();
			msg.m_gang = m_gangPos[pos];
			msg.m_end = m_desk->getDeskPlayState()==DESK_PLAY_END_CARD?1:0;
			msg.m_flag = 1; 
			if(m_needGetCard)
			{
				msg.m_flag = 0;
			}

			if(pos == i)
			{
				if(m_needGetCard)
				{
					msg.m_curCard.m_number = m_curGetCard->m_number;
					msg.m_curCard.m_color = m_curGetCard->m_color;
				}

				Lsize thinkDataSize = m_thinkInfo[pos].m_thinkData.size();
				for (Lsize j = 0; j < thinkDataSize; ++j)
				{		
					ThinkData info;
					info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
					for(Lsize n = 0 ; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
					{
						CardValue v;
						v.m_color = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_color;
						v.m_number = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_number;
						info.m_card.push_back(v);
					}
					msg.m_think.push_back(info);
				}
			}

			m_desk->m_user[i]->Send(msg);
		}
	}
	UpdateLastWaiteThinkRetTime();
}

bool GameHandler_GuangDongOneHundler::calcScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[], Lint& bombCount, Lint& zhuangpos, Lint maNum[], LMsgS2CGameOver& over, Lint winner)
{
	UpdateZhuangPos(result, winpos, bombpos, zhuangpos);
	calGangScore(result, gold);

	if (IsBombHorse())
	{
		calBombHorseScore(result, winpos, bombpos, gold, maNum, over);
	}
	else
	{
		calHorsrScore(result, winpos, bombpos, gold, maNum, over, winner);
	}

	bool bBompHorseMultipleScore = IsBompHorseMultipleScoreType();
	if (!bBompHorseMultipleScore)
	{
		calCardTypeScore(result, winpos, bombpos, gold);
	}
	calGenZhuangScore(gold);
	calRedDragonSceor(winpos, bombpos, gold);
	return true;
}

void GameHandler_GuangDongOneHundler::calGenZhuangScore(Ldouble gold[])
{

	if(m_gengzhuangInfo.m_nSuccTimes > 0)
	{
		for ( Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (i !=  m_zhuangpos)
			{
				gold[i] += m_gengzhuangInfo.m_nSuccTimes * GetGengzhuangBaseScore();
			}
		}

		gold[m_zhuangpos] -= (m_deskplayer_num - 1) * m_gengzhuangInfo.m_nSuccTimes * GetGengzhuangBaseScore();
	}
}

bool GameHandler_GuangDongOneHundler::IsLiuju()
{
	if (m_deskCard.size() <= GameBuyhorseNum())
	{
		return true;
	}

	return false;
}

void GameHandler_GuangDongOneHundler::calCardTypeScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[])
{
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		Lint baseScoreY = GetSocore(i);
		//计算胡牌得分
		if (m_playerHuInfo[i] != WIN_SUB_NONE)
		{
			//杠上开花包三家
			if (IsGangshangKaihuaHu(m_thinkRet[i].m_hu)  && i != m_beforePos && m_beforeType != THINK_OPERATOR_AGANG && IsValidPos(m_beforePos) && IsGangshangkaihuaScore())
			{
				gold[m_beforePos] -= baseScoreY * (m_deskplayer_num - 1);
				gold[i] += baseScoreY * (m_deskplayer_num - 1);
			}
			else
			{
				switch(m_playerHuInfo[i])
				{
				case WIN_SUB_ZIMO:
				case WIN_SUB_DZIMO:
					{
						//庄家小自模， 每人扣2倍，自已拿6倍 
						for (Lint j = 0; j < m_deskplayer_num; ++j)
						{
							if (j != i)
							{
								gold[j] -= baseScoreY ;
							}
						}
						gold[i] += baseScoreY * (m_deskplayer_num - 1);
					}
					break;
				case WIN_SUB_DBOMB:
					{
						//抢杠胡 包三家
						gold[bombpos] -= baseScoreY * (m_deskplayer_num - 1);
						gold[i] += baseScoreY * (m_deskplayer_num - 1);
					}
					break;
				default:
					{
						LLOG_ERROR("hu_type Error : m_playerHuInfo[i] = %d.", m_playerHuInfo[i]);
					}
					break;
				}
			}

		}
		LLOG_DEBUG("HUPai:%d %d)", i, gold[i]);
	}
}

void GameHandler_GuangDongOneHundler::calGangScore(Lint result, Ldouble gold[])
{
	//判断是否荒庄荒杠
	bool bHorseGenGang = IsHorseGenGang();
	if (WIN_NONE == result && bHorseGenGang)
	{
		return;
	}

	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		//杠底分
		Lint baseScoreG = 1;
		//计算杠牌得分
		gold[i] += baseScoreG * 2 * (m_deskplayer_num - 1) * m_angang[i];		//基础分*底分*玩家*数量
		gold[i] += baseScoreG * (m_deskplayer_num - 1) * m_minggang[i];
		gold[i] += baseScoreG * (m_deskplayer_num - 1) * m_diangang[i];
		gold[i] -= baseScoreG * (m_deskplayer_num - 1) * m_adiangang[i];
		for (Lint j = 0; j < m_deskplayer_num; ++j )
		{
			if (j != i)
			{
				gold[j] -= baseScoreG * 2 * m_angang[i];
				gold[j] -= baseScoreG * 1 * m_minggang[i];
			}
		}
	}
}


bool GameHandler_GuangDongOneHundler::IsNoGhostType()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_GHOSTCARDNO)
		{
			return true;
		}
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsNoHorseType()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_ZHANIAO0)
		{
			return true;
		}
	}
	return false;
}

void GameHandler_GuangDongOneHundler::calHorsrScore(Lint result,Lint winpos[],Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over, Lint winner)
{
	//只有胡牌有马牌
	if (result != WIN_NONE)
	{
		calBuyHorseScore(winpos, bombpos, gold, maNum, over);
		if (IsHorseGenGang())
		{
			std::vector<CardValue> horsecards = GetHorseFromBirdInfo(over.m_bird_infos);
			calHorseGenGangScore(winpos, bombpos, gold, horsecards, winner);
		}
	}
}

void GameHandler_GuangDongOneHundler::calBuyHorseScore(Lint winpos[],Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over)
{
	//马牌个数计算
	LMsgS2CZhaBirdShow maPaiStruct;
	calHorseNum(maPaiStruct, winpos, maNum);
	over.m_bird_infos.insert(over.m_bird_infos.end(), maPaiStruct.m_bird_infos.begin(), maPaiStruct.m_bird_infos.end());

	bool bPayQiangganghorseScore = IsPayQianggangHorseScore();
	bool bPayGangshangkaihuahorseScore = IsGangshangkaihuaScore();
	bool bPayQiangganghorseJiaBei = CanQiangGangHuHorseJiaBei();
	Lint maNumZimo = 0;
	for ( Lint i = 0; i < m_deskplayer_num; ++i )
	{
		if (IsQiangGangHu(m_thinkRet[i].m_hu) && bPayQiangganghorseScore ) //抢杠胡，包马牌分
		{
			//马牌得分
			Lint maPaiBaseScore = GetMaPaiBaseScore(i);
			gold[i] += maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
			gold[bombpos] -= maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
		}
		else if (IsGangshangKaihuaHu(m_thinkRet[i].m_hu) && i != m_beforePos && m_beforeType != THINK_OPERATOR_AGANG && bPayGangshangkaihuahorseScore && IsValidPos(m_beforePos))
		{
			//马牌得分
			Lint maPaiBaseScore = GetMaPaiBaseScore(i);
			gold[i] += maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
			gold[m_beforePos] -= maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
		}
		else if (WIN_SUB_DZIMO == winpos[i] || WIN_SUB_ZIMO == winpos[i] || WIN_SUB_DBOMB == winpos[i])
		{
			//马牌得分
			Lint maPaiBaseScore = GetMaPaiBaseScore(i);
			gold[i] += maPaiBaseScore * maNum[i] * (m_deskplayer_num - 1);
			for ( Lint j = 0; j < m_deskplayer_num; ++j)
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


void GameHandler_GuangDongOneHundler::calBombHorseScore(Lint result, Lint winpos[], Lint bombpos, Ldouble gold[], Lint maNum[], LMsgS2CGameOver& over)
{
	return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A胡牌 
//1）碰杠、暗杠得分
//A得分：Pg/Ag * hit * 3 + each in(B,C,D) Pg/Ag * hit * 3 - Pg/Ag * n
//BCD得分：Pg/Ag * n - Pg/Ag * hit - (A)Pg/Ag * hit
//
//2）点杠得分 点杠者中x马，被点杠中y马
//A得分:3*y - 3 * x
//点杠得分 -3*y
//被点杠得分 3x
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GameHandler_GuangDongOneHundler::calHorseGenGangScore(Lint winpos[], Lint bombpos, Ldouble gold[], const std::vector<CardValue>& horsecards , Lint winner)
{
	Lint playnum = m_deskplayer_num - 1;
	//马牌个数计算
	Lint maNum[DEFAULT_DESK_USER_COUNT] = { 0 };
	calPlayerHitHorseNum(horsecards, maNum);
	Lint huPos = GetHorseGenGangHuPos(winpos, bombpos, winner);

	Lint nTotalHorseNum = GameBuyhorseNum();
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		//1.暗杠、明杠得分
		if (huPos == i)
		{
			gold[i] += (m_minggang[i] + 2 * m_angang[i]) * maNum[i] * playnum;		//胡的人自己中马得的分
			for (Lint k = 0; k < m_deskplayer_num; ++k)
			{
				if (k != huPos)
				{
					gold[k] -= (m_minggang[i] + 2 * m_angang[i]) * maNum[i];		//非胡牌的人需要扣分
				}
			}
		}
		else
		{
			gold[i] += (m_minggang[i] + 2 * m_angang[i]) * (nTotalHorseNum - maNum[i]) ;			//杀马分
			gold[huPos] += (m_minggang[i] + 2 * m_angang[i]) * (maNum[i] * playnum - nTotalHorseNum);		//胡牌的人从这得的分
			for (Lint k = 0; k < m_deskplayer_num; ++k)
			{
				if (k != huPos && k != i)
				{
					gold[k] -= (m_minggang[i] + 2 * m_angang[i]) * maNum[i];		//非胡牌的另外两个人需要扣分
				}
			}
		}
		//2.点杠得分
		std::vector<Lint> relation = m_gangRelation[i];
		std::vector<Lint>::const_iterator cit = relation.begin();
		for ( ; cit != relation.end(); ++cit)
		{
			Lint pos = *cit;
			if (IsValidPos(pos))	//防止数组越界
			{
				gold[huPos] += (playnum * maNum[i] - playnum * maNum[pos]); //胡牌的人
				gold[i] += playnum * maNum[pos];		//被点杠
				gold[pos] -= playnum * maNum[i];		//点杠
			}
			else
			{
				LLOG_ERROR("Dian Gang Position Is InValid!!! Pos = %d.", pos);
			}
			
		}
	}
	return;
}

bool GameHandler_GuangDongOneHundler::calcMaPai(Lint zhuangPos, Lint winPos, Lint maPaiNum)
{
	bool result = false;
	Lint offset = (winPos - zhuangPos) >= 0 ? (winPos - zhuangPos) : (winPos - zhuangPos + m_deskplayer_num);	
	switch(abs(offset))
	{
	case 0:
		{
			if (maPaiNum == 1 || maPaiNum == 5 || maPaiNum == 9)
				result = true;
		}
		break;
	case 1:
		{
			if (maPaiNum == 2 || maPaiNum == 6)
				result = true;
		}
		break;
	case 2:
		{
			if (maPaiNum == 3 || maPaiNum == 7)
				result = true;
		}
		break;
	case 3:
		{
			if (maPaiNum == 4 || maPaiNum == 8)
				result = true;
		}
		break;
	default:
		{
			LLOG_ERROR("CalcMaPai1: something has happend!");
		}
		break;
	}
	return result;
}

void GameHandler_GuangDongOneHundler::SetThinkIng()
{
	bool think = false;
	for(Lint i = 0 ; i < m_deskplayer_num;++i)
	{
		if (!m_oper_gang)	//点明杠需要保存玩家的思考，在ThinkEnd中Clear
		{
			m_thinkRet[i].Clear();
		}
		m_thinkInfo[i].Reset();
		if(i != m_curPos)
		{
			if (IsXiaoQiDuiType())
				mGameInfo.m_QiXiaoDui = true;
			else
				mGameInfo.m_QiXiaoDui = false;

			if (CanQiangGanghu())
				mGameInfo.m_canqiangganghu = true;
			else
				mGameInfo.m_canqiangganghu = false;

			if (m_beforeType == THINK_OPERATOR_MGANG && mGameInfo.m_canqiangganghu)
			{
				mGameInfo.b_onlyHu = true;
			}
			else
			{
				mGameInfo.b_onlyHu = false;
			}
			mGameInfo.m_GameType = m_desk->getGameType();	// 0 湖南， 3， 长沙
			mGameInfo.b_canEat =  false;		// 是否可以吃
			mGameInfo.b_canHu =  gametypeDianPao();		// 是否可以胡
			mGameInfo.m_thinkGang = false;	// 单独处理是不是杠出来的牌
			mGameInfo.m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
			mGameInfo.m_playerPos = m_curPos;	// 当前一个出牌位置
			mGameInfo.m_cardState = m_beforeType;	// 当前一个出牌状态
			mGameInfo.m_hasDianGang = m_oper_gang;	// 当前一个出牌状态
			mGameInfo.m_endStartPos = m_endStartPos;	// 结束位置，即海底位置
			mGameInfo.m_MePos = i;		// 玩家的位置
			mGameInfo.m_qihuFanshu = GetQihuFanshu();	//起胡番数
			mGameInfo.m_ghostType = GetGhostType();
			mGameInfo.m_bZhuang = IsZhuang(i);
			mGameInfo.m_ghostCard = m_ghostCardReal;
			//一百张胡牌类型
			FillspecifichuType(mGameInfo);

			m_thinkInfo[i].m_thinkData = gCardMgr.CheckOutCardOperator(m_handCard[i],m_pengCard[i],m_gangCard[i],m_abombCard[i],m_eatCard[i],
				m_curOutCard,mGameInfo);
			if (m_thinkInfo[i].NeedThink())
			{
				think = true;
				VideoThink(i);
			}
		}
		UpdateLastWaiteThinkRetTime();
	}

	if (think)
	{
		m_desk->setDeskPlayState(DESK_PLAY_THINK_CARD);
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			LMsgS2CThink think;
			think.m_time = 15;
			think.m_card.m_color = m_curOutCard->m_color;
			think.m_card.m_number = m_curOutCard->m_number;
			if (m_thinkInfo[i].NeedThink())
			{
				think.m_flag = 1;
				Lsize thinkDataSize = m_thinkInfo[i].m_thinkData.size();
				for (Lsize j = 0; j < thinkDataSize; ++j)
				{		
					ThinkData info;
					info.m_type = m_thinkInfo[i].m_thinkData[j].m_type;
					for(Lsize n = 0 ; n < m_thinkInfo[i].m_thinkData[j].m_card.size(); ++n)
					{
						CardValue v;
						v.m_color = m_thinkInfo[i].m_thinkData[j].m_card[n]->m_color;
						v.m_number = m_thinkInfo[i].m_thinkData[j].m_card[n]->m_number;
						info.m_card.push_back(v);
					}
					think.m_think.push_back(info);
				}
			}
			else
			{
				think.m_flag = 0;
			}
			m_desk->m_user[i]->Send(think);
		}
		UpdateLastWaiteThinkRetTime();
	}
	else
	{
		ThinkEnd();
	}
}

void GameHandler_GuangDongOneHundler::CheckThink()
{
	bool hu		= false;
	bool Peng	= false;
	bool Chi	= false;
	bool Gang	= false;
	bool Bu		= false;
	bool hu_New		= false;
	bool Peng_New	= false;
	bool Chi_New	= false;
	bool Gang_New	= false;
	bool Bu_New		= false;
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		//
		if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)			hu		= true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)		Peng	= true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_CHI)		Chi		= true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)		Gang	= true;
		else if (m_thinkRet[i].m_type == THINK_OPERATOR_MBU)		Bu		= true;

		//
		if(m_thinkInfo[i].NeedThink())
		{
			if (m_thinkInfo[i].HasHu())				hu_New		= true;
			else if (m_thinkInfo[i].HasPeng())		Peng_New	= true;
			else if (m_thinkInfo[i].HasChi())		Chi_New		= true;
			else if (m_thinkInfo[i].HasMGang())		Gang_New	= true;
			else if (m_thinkInfo[i].HasMBu())		Bu_New		= true;
		}
	}

	bool think = false;

	if (hu_New)	
		think = true;
	else
	{
		if (!hu)
		{
			if (Peng_New || Gang_New || Bu_New )
				think = true;
			else
			{
				if (!(Peng || Gang || Bu))
				{
					if (Chi_New)
						think = true;
				}
			}
		}
	}

	if (!think)
		ThinkEnd();
}

void GameHandler_GuangDongOneHundler::ThinkEnd()
{
	for(Lint i = 0; i < m_deskplayer_num; i ++)
	{
		m_thinkInfo[i].Reset();
	}
	Lint huCount = 0;

	Lint pengPos = m_deskplayer_num;
	Lint gangPos = m_deskplayer_num;
	Lint chiPos = m_deskplayer_num;
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
		{
			m_playerHuInfo[i] = CheckIfBigHu(m_thinkRet[i].m_hu)? WIN_SUB_DBOMB : WIN_SUB_BOMB ;
			m_playerBombInfo[i] = CheckIfBigHu(m_thinkRet[i].m_hu)? WIN_SUB_DABOMB : WIN_SUB_ABOMB;
			huCount++;

			LMsgS2CUserOper send;
			send.m_pos = i;
			send.m_errorCode = 0;
			send.m_think.m_type = m_thinkRet[i].m_type;
			//send.m_think.m_card = m_thinkRet[i].m_card;
			send.m_card.m_color = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_color;
			send.m_card.m_number = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_number;
			send.m_hu = m_thinkRet[i].m_hu;
			send.m_cardCount = m_handCard[i].size();
			for (Lint j = 0; j < send.m_cardCount; j++)
			{
				CardValue mCard;
				mCard.m_color = m_handCard[i][j]->m_color;
				mCard.m_number = m_handCard[i][j]->m_number;
				send.m_cardValue.push_back(mCard);
			}
			m_desk->BoadCast(send);
		}

		if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)
			gangPos = i;

		if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)
			pengPos = i;

		if (m_thinkRet[i].m_type == THINK_OPERATOR_CHI)
			chiPos = i;
	}

	bool bisGuo = !(huCount != 0 || gangPos != m_deskplayer_num || pengPos != m_deskplayer_num || chiPos != m_deskplayer_num);
	if(!m_gengzhuangInfo.m_isCircleEnd && !bisGuo)
	{
		m_gengzhuangInfo.m_isCircleEnd = true;
	}

	if (huCount != 0)
	{	
		Lint pos = m_beforePos;
		if (m_beforeType == THINK_OPERATOR_MGANG)
		{	
			if (m_oper_gang)
			{
				pos = m_curPos;
			}
			else
			{
				pos = m_beforePos;
			}
		}
		OnGameOver(WIN_BOMB, m_playerHuInfo, pos, huCount);
		return;
	}
	m_canDiHu = false;
	//杠
	if (IsValidPos(gangPos))
	{
		m_operateNum++;
		m_oper_gang = false; 
		LMsgS2CUserOper send;
		send.m_pos = gangPos;
		send.m_errorCode = 0;
		send.m_think.m_type = m_thinkRet[gangPos].m_type;
		for(Lsize i = 0 ; i < m_thinkRet[gangPos].m_card.size(); ++i)
		{
			CardValue v;
			v.m_color = m_thinkRet[gangPos].m_card[i]->m_color;
			v.m_number = m_thinkRet[gangPos].m_card[i]->m_number;
			send.m_think.m_card.push_back(v);
		}
		send.m_card.m_color = m_curOutCard->m_color;
		send.m_card.m_number = m_curOutCard->m_number;
		m_desk->BoadCast(send);
		gCardMgr.EraseCard(m_handCard[gangPos], m_thinkRet[gangPos].m_card[0],3);

		//录像
		std::vector<CardValue> cards;
		for (Lint i = 0; i < 4; i ++)
		{
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			cards.push_back(card);
			m_gangCard[gangPos].push_back(m_curOutCard);
		}
		m_video.AddOper(VIDEO_OPER_GANG, gangPos, cards);

		m_diangang[gangPos] += 1;
		m_adiangang[m_beforePos] += 1;
		m_gangRelation[gangPos].push_back(m_beforePos);
		for (Lint i = 0; i < m_deskplayer_num; i ++)
		{
			m_thinkRet[i].Clear();
		}
		//给玩家摸一张牌
		SetPlayIng(gangPos, true, true, true, true);
		return;
	}

	if ( IsValidPos(pengPos))
	{
		m_operateNum++;
		LMsgS2CUserOper send;
		send.m_pos = pengPos;
		send.m_errorCode = 0;
		send.m_think.m_type = m_thinkRet[pengPos].m_type;
		for(Lsize i = 0 ; i < m_thinkRet[pengPos].m_card.size(); ++i)
		{
			CardValue v;
			v.m_color = m_thinkRet[pengPos].m_card[i]->m_color;
			v.m_number = m_thinkRet[pengPos].m_card[i]->m_number;
			send.m_think.m_card.push_back(v);
		}
		send.m_card.m_color = m_curOutCard->m_color;
		send.m_card.m_number = m_curOutCard->m_number;
		m_desk->BoadCast(send);
		gCardMgr.EraseCard(m_handCard[pengPos], m_curOutCard,2);

		//录像
		std::vector<CardValue> cards;
		for (Lint i = 0; i < 3; i ++)
		{
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			cards.push_back(card);
			m_pengCard[pengPos].push_back(m_curOutCard);
		}
		m_video.AddOper(VIDEO_OPER_PENG_CARD, pengPos, cards);

		for (Lint i = 0; i < m_deskplayer_num; i ++)
		{
			m_thinkRet[i].Clear();
		}
		//碰完打一张牌
		m_curGetCard = NULL;
		SetPlayIng(pengPos, false, false, true, false);
		m_needGetCard = true;
		return;
	}

	//吃
	if (IsValidPos(chiPos))
	{
		m_operateNum++;
		LMsgS2CUserOper send;
		send.m_pos = chiPos;
		send.m_errorCode = 0;
		send.m_think.m_type = m_thinkRet[chiPos].m_type;
		for(Lsize i = 0 ; i < m_thinkRet[chiPos].m_card.size(); ++i)
		{
			CardValue v;
			v.m_color = m_thinkRet[chiPos].m_card[i]->m_color;
			v.m_number = m_thinkRet[chiPos].m_card[i]->m_number;
			send.m_think.m_card.push_back(v);
		}
		send.m_card.m_color = m_curOutCard->m_color;
		send.m_card.m_number = m_curOutCard->m_number;
		m_desk->BoadCast(send);
		gCardMgr.EraseCard(m_handCard[chiPos], m_thinkRet[chiPos].m_card);

		//录像
		std::vector<CardValue> cards;
		//手牌
		CardValue card;
		card.m_color = m_thinkRet[chiPos].m_card[0]->m_color;
		card.m_number = m_thinkRet[chiPos].m_card[0]->m_number;
		cards.push_back(card);
		//吃的牌放中间
		card.m_color = m_curOutCard->m_color;
		card.m_number = m_curOutCard->m_number;
		cards.push_back(card);
		//手牌
		card.m_color = m_thinkRet[chiPos].m_card[1]->m_color;
		card.m_number = m_thinkRet[chiPos].m_card[1]->m_number;
		cards.push_back(card);

		m_video.AddOper(VIDEO_OPER_EAT, chiPos, cards);

		m_eatCard[chiPos].push_back(m_thinkRet[chiPos].m_card[0]);
		m_eatCard[chiPos].push_back(m_curOutCard);
		m_eatCard[chiPos].push_back(m_thinkRet[chiPos].m_card[1]);

		for (Lint i = 0; i < m_deskplayer_num; i ++)
		{
			m_thinkRet[i].Clear();
		}
		//给玩家摸一张牌
		m_curGetCard = NULL;
		SetPlayIng(chiPos, false, false, true, false);
		m_needGetCard = true;
		return;
	}

	//这里没有人操作
	if (!m_isThinkingGang && (m_beforeType == THINK_OPERATOR_MGANG))
	{
		//录像
		std::vector<CardValue> cards;
		for (Lint i = 0; i < 4; i ++)
		{
			CardValue card;
			card.m_color = m_curOutCard->m_color;
			card.m_number = m_curOutCard->m_number;
			cards.push_back(card);
		}
		if (m_beforeType == THINK_OPERATOR_MGANG)
		{
			m_video.AddOper(VIDEO_OPER_MING_GANG, m_beforePos, cards);
		}
		else
		{
			m_video.AddOper(VIDEO_OPER_SELF_BU, m_beforePos, cards);
		}

		m_gangCard[m_beforePos].push_back(m_curOutCard);
		CardVector::iterator it = m_pengCard[m_beforePos].begin();
		for(; it != m_pengCard[m_beforePos].end();it+=3)
		{
			if(gCardMgr.IsSame(m_curOutCard,*it))
			{
				m_gangCard[m_beforePos].insert(m_gangCard[m_beforePos].end(),it,it+3);
				m_pengCard[m_beforePos].erase(it,it+3);
				break;
			}
		}
		//这里处理 明杠 明补发消息
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = m_beforePos;
		sendMsg.m_cs_card.m_type = m_beforeType;
		CardValue card;
		card.m_color = m_curOutCard->m_color;
		card.m_number = m_curOutCard->m_number;
		sendMsg.m_cs_card.m_card.push_back(card);
		m_desk->BoadCast(sendMsg);

		m_minggang[m_beforePos] += 1;
		SetPlayIng(m_beforePos,true,m_beforeType == THINK_OPERATOR_MGANG, true, true,true);
	}
	else
	{
		if (m_isThinkingGang)
		{
			m_isThinkingGang = false;
			if(m_beforeType == THINK_OPERATOR_MGANG || m_beforeType == THINK_OPERATOR_MBU)
			{
				SetPlayIng(m_desk->GetNextPos(m_beforePos), true, false, true, true);
			}
		}
		else
		{
			if(m_curOutCard!=NULL)
				m_outCard[m_beforePos].push_back(m_curOutCard);
			SetPlayIng(m_desk->GetNextPos(m_beforePos), true, false, true, true);
		}	
	}
}

void GameHandler_GuangDongOneHundler::OnGameOver(Lint result,Lint winpos[],Lint bombpos, Lint winner)
{
	if (m_desk == NULL || m_desk->m_vip == NULL)
	{
		LLOG_DEBUG("OnGameOver NULL ERROR ");
		return;
	}

	UpdateLastWaiteThinkRetTime();
	//计算输赢结果
	Ldouble gold[DEFAULT_DESK_USER_COUNT]={0};
	Lint bombCount = 0;
	Lint curzhuang = m_zhuangpos;
	Lint maNum[DEFAULT_DESK_USER_COUNT] = { 0 };
	//广播结果
	LMsgS2CGameOver over;
	//计算分数
	bool checkScore = calcScore(result, winpos, bombpos, gold, bombCount, curzhuang, maNum, over, winner);	

	if (!checkScore 
		&& result != WIN_NONE)
	{
		LLOG_ERROR("OnGameOver : checkScore Error = %d.", checkScore);
		return;
	}

	Lint mgang[DEFAULT_DESK_USER_COUNT] = {0};
	for(Lint i = 0 ; i < m_deskplayer_num; ++i)
	{
		mgang[i] += m_minggang[i];
		mgang[i] += m_diangang[i];
	}

	Lint prezhuangPos = m_zhuangpos;
	//计算庄
	if (result != WIN_NONE)
	{
		m_zhuangpos = curzhuang;
		//录像
		std::vector<CardValue> cards;
		CardValue curGetCard;
		if (WIN_ZIMO == result && m_curGetCard)
		{
			curGetCard.m_color = m_curGetCard->m_color;
			curGetCard.m_number = m_curGetCard->m_number;
		}
		else if (WIN_BOMB == result && m_curOutCard)
		{
			curGetCard.m_color = m_curOutCard->m_color;
			curGetCard.m_number = m_curOutCard->m_number;
		}

		over.m_gd_hucards.push_back(curGetCard);
		cards.push_back(curGetCard);
		m_video.AddOper(VIDEO_OPER_ZIMO, curzhuang, cards);
	}
	else
	{
		//录像
		std::vector<CardValue> cards;
		m_video.AddOper(VIDEO_OPER_HUANGZHUANG, m_curPos, cards);
	}
	//保存录像
	m_video.m_Id = gVipLogMgr.GetVideoId();
	m_video.m_playType = m_desk->getPlayType();
	m_video.m_nAppId = m_desk->GetDeskAppId();
	LMsgL2LDBSaveVideo video;
	video.m_type = 0;
	video.m_sql = m_video.GetInsertSql();
	if (m_desk->m_vip->m_curMatchId == 0)
	{
		gWork.SendMsgToDb(video);
	}

	over.m_result = result;
	if (m_desk->m_vip->GetMatchID()) //比赛场加入每局倍率
	{
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			gold[i] *= m_desk->m_vip->GetCurrDifen();
		}
	}

	for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
	{
		gold[i] = gold[i] * m_desk->GetStake();
	}

	memcpy(over.m_cs_win, winpos, sizeof(over.m_cs_win));
	memcpy(over.m_score, gold, sizeof(over.m_score));
	memcpy(over.m_agang, m_angang, sizeof(over.m_agang));
	memcpy(over.m_mgang, mgang, sizeof(over.m_mgang));
	memcpy(over.m_dgang, m_adiangang, sizeof(over.m_dgang));
	over.m_ghostCard = m_ghostCardReal;
	over.m_gengZhuangNum = m_gengzhuangInfo.m_nSuccTimes;
	over.m_playerNum = m_deskplayer_num;

	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		over.m_count[i] = m_handCard[i].size();
		over.m_successive[i] = m_successive[i];
		over.m_redDragonNum[i] = m_redDragonNum[i];
		//手牌
		for (Lint j = 0; j < over.m_count[i]; ++j)
		{
			over.m_card[i][j].m_color = m_handCard[i][j]->m_color;
			over.m_card[i][j].m_number = m_handCard[i][j]->m_number;
		}

		//胡牌类型
		if (m_playerHuInfo[i] != WIN_SUB_NONE)
		{
			for (Lint k = 0; k < m_thinkRet[i].m_hu.size(); k ++)
			{
				over.m_hu[i].push_back(m_thinkRet[i].m_hu[k]);
			}
		}
	}

	if(m_gengzhuangInfo.m_nSuccTimes > 0)
	{
		over.m_hu[prezhuangPos].push_back(HU_GENGZHUANGSUCC);
	}

	//刷新总结算界面自摸数据
	UpdateEndInfo(winpos, maNum, result);
	m_desk->SetDeskWait();
	//保存结果	
	Ldouble cal_gold[4] = { 0,0,0,0 };
	m_desk->m_vip->AddTuiDaoLog(m_desk->m_user, gold, cal_gold, winpos, prezhuangPos, m_angang, mgang, m_playerBombInfo, m_video.m_Id, m_maNum, m_ghostZimo, m_noghostZimo);
	memcpy(over.m_gold, cal_gold, sizeof(over.m_gold));
	//是否最后一局
	over.m_end = m_desk->m_vip->isEnd()?1:0;
	m_desk->BoadCast(over);

	if (m_desk->m_vip->GetMatchID())
	{
		m_desk->m_vip->Report2MatchServer(over);
	}

	m_desk->HanderGameOver();
}

void GameHandler_GuangDongOneHundler::VideoThink(Lint pos)
{
	if(m_thinkInfo[pos].m_thinkData.size() >0)
	{
		std::vector<CardValue> cards;
		for (auto itr=m_thinkInfo[pos].m_thinkData.begin(); itr!=m_thinkInfo[pos].m_thinkData.end(); itr++)
		{
			CardValue card;
			card.m_number = itr->m_type;
			if (itr->m_card.size()>0)
				card.m_color  = itr->m_card[0]->m_color*10+itr->m_card[0]->m_number;
			if (itr->m_card.size()>1)
				card.m_color  = card.m_color*1000+itr->m_card[1]->m_color*10+itr->m_card[1]->m_number;
			cards.push_back(card);
		}
		m_video.AddOper(VIDEO_OPEN_THINK, pos, cards);
	}
}

void GameHandler_GuangDongOneHundler::VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number)
{
	std::vector<CardValue> cards;
	CardValue card;
	card.m_number = op;
	card.m_color  = card_color*10+card_number;
	cards.push_back(card);
	m_video.AddOper(VIDEO_OPEN_DOING, pos, cards);

}

bool GameHandler_GuangDongOneHundler::CheckIfBigHu(std::vector<Lint>& vec)
{
	bool isBigHu = false;
	for (Lint i = 0; i < vec.size(); i ++)
	{
		if ((vec[i] == HU_QIANGGANGHU) || (vec[i] == HU_QIANGMINGGANGHU))
		{
			isBigHu = true;
			break;
		}
	}
	return isBigHu;
}

bool GameHandler_GuangDongOneHundler::IsQiangGangHu(std::vector<Lint>& hu)
{
	for (Lint i = 0;i < hu.size();i ++)
	{
		if ((hu[i] ==  HU_QIANGGANGHU)|| (hu[i] == HU_QIANGMINGGANGHU))
		{
			return true;
		}
	}
	return false;
}


bool GameHandler_GuangDongOneHundler::IsXiaoQiDuiType()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_QIXIAODUI)
		{
			return true;
		}
	}
	return false;
}

Lint GameHandler_GuangDongOneHundler::GameBuyhorseNum()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_ZHANIAO0)
			return 0;
		else if (*iTor == PT_ZHANIAO2)
			return 2;
		else if(*iTor == PT_ZHANIAO4)
			return 4;
		else if(*iTor == PT_ZHANIAO6)
			return 6;
		else if (*iTor == PT_ZHANIAO8)
			return 8;
		else if (*iTor == PT_CATCHHORSE)
			return 2;
		else if (*iTor == PT_BOMBHORSE)
			return 1;
		else if (*iTor == PT_ZHANIAO10)
			return 10;
		else if (*iTor == PT_ZHANIAO20)
			return 20;

	}
	return 0;
}

void GameHandler_GuangDongOneHundler::UpdateGenZhuangInfo(const Card* const outCard, const Lint pos, const std::vector<CardValue>& cards)
{
	if (!CanGengZhuang())
	{
		return;
	}
	if (IsGenZhangOnlyOnce() && m_gengZhengOnceEnd)	//判读是否跟庄结束
	{
		return;
	}

	const Card* const curOutCard = outCard;
	if (pos == m_zhuangpos)
	{
		m_gengzhuangInfo.m_isCircleEnd = false;
		m_gengzhuangInfo.m_zhuangCard.m_color = curOutCard->m_color;
		m_gengzhuangInfo.m_zhuangCard.m_number = curOutCard->m_number;
		m_gengzhuangInfo.m_cnt = 1;
	}
	else if (m_beforeType != THINK_OPERATOR_OUT ||
		((curOutCard->m_color != m_gengzhuangInfo.m_zhuangCard.m_color || curOutCard->m_number != m_gengzhuangInfo.m_zhuangCard.m_number) &&
			!gCardMgr.IsGhostCard(m_ghostCardReal, curOutCard)))
	{
		m_gengzhuangInfo.m_isCircleEnd = true;
		m_gengZhengOnceEnd = true;
	}
	else if (++m_gengzhuangInfo.m_cnt >= m_deskplayer_num)
	{
		m_gengzhuangInfo.m_isCircleEnd = true;
		m_gengZhengOnceEnd = true;
		m_gengzhuangInfo.m_nSuccTimes++;
		m_video.AddOper(VIDEO_OPER_GENGZHUANG_SUCC, pos, cards);
		LMsgS2CGengZhuangSucc sendMsg;
		m_desk->BoadCast(sendMsg);
	}
}

bool GameHandler_GuangDongOneHundler::IsZhuang(Lint pos)
{
	return pos == m_zhuangpos;
}

Lint GameHandler_GuangDongOneHundler::GetGhostType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_GHOSTCARDNO || *iTor == PT_GHOSTCARDBAIBAN || *iTor == PT_GHOSTCARDFLOP || *iTor == PT_GHOSTCARDFLOPTWO || *iTor == PT_GHOSTREDDRAGON)
			return *iTor;
	}
	return PT_GHOSTCARDNO;
}

bool GameHandler_GuangDongOneHundler::gametypeDianPao() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_DIANPAOHU)
			return true;
	}
	return false;
}

void GameHandler_GuangDongOneHundler::SetDeskPlayNum()
{
	m_deskplayer_num = m_desk->m_deskplayer_num;
}

bool GameHandler_GuangDongOneHundler::IsNeedWindCards() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_REMOVEWINDCARDS)
			return false;
	}
	return true;
}

bool GameHandler_GuangDongOneHundler::IsHorseGenGang() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_HORSEFOLLOWGANG)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsSteadilyHighType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_STEADILYHIGH)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsBombHorse() const
{
	return false;
}

bool GameHandler_GuangDongOneHundler::IsGenZhangOnlyOnce() const
{
	return true;
}

bool GameHandler_GuangDongOneHundler::IsPayQianggangHorseScore() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_QIANGGANGQUANBAOSCORE)
			return true;
	}
	return false;
}
bool GameHandler_GuangDongOneHundler::IsGangshangkaihuaScore() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_GANGSHANGKAIHUAQUANBAO)
			return true;
	}
	return false;
}
bool GameHandler_GuangDongOneHundler::IsQiduiDoubleScore() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_QIXIAOFUIDOUBLESCORE)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsGhostCardBaiban() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_GHOSTCARDBAIBAN)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::CanQiangGanghu() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_QIANGGANGHU)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::CanGengZhuang() const 
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_GENGZHUANGONE || *iTor == PT_GENGZHUANGTWO || *iTor == PT_GENGZHUANGTHREE)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsMaGenDifen() const 
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_MAGENGDIFEN)
			return true;
	}
	return false;
}

//无鬼加倍
bool GameHandler_GuangDongOneHundler::IsNoGhost2TimesScore() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_NOGHOST2TIMESSCORE)
			return true;
	}
	return false;
}
//四鬼胡
bool GameHandler_GuangDongOneHundler::CanFourGhostHu() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_FOURGHOSTHU)
			return true;
	}
	return false;
}
//四鬼加倍
bool GameHandler_GuangDongOneHundler::IsFourGhost2TimesScore() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_FOURGHOSTHU2TIMESSCORE)
			return true;
	}
	return false;
}
//对对胡加倍
bool GameHandler_GuangDongOneHundler::IsDuiduihuType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_DUIDUIHUTYPE)
			return true;
	}
	return false;
}
//清一色四倍
bool GameHandler_GuangDongOneHundler::IsQingyiseType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_QINGYISETYPE)
			return true;
	}
	return false;
}
//幺九六倍
bool GameHandler_GuangDongOneHundler::IsYaojiuType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_YAOJIUTYPE)
			return true;
	}
	return false;
}

//幺九含1或者9即可
bool GameHandler_GuangDongOneHundler::IsYaojiuHasOneOrNine() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_YAOJIU1OR9)
			return true;
	}
	return false;
}

//字一色八倍
bool GameHandler_GuangDongOneHundler::IsAllWindType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_ALLWINDTYPE)
			return true;
	}
	return false;
}
//十三幺八倍
bool GameHandler_GuangDongOneHundler::IsShisanyaoType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_SHISANYAOTYPE)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsHunyise2TimesScore() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_HUNYISETYPE)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsDLuxuryqiduiType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_DLUXURYQIDUI)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsTLluxuryqiduiType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_TLUXURYQIDUI)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsEighteenarhats() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_EIGHTEENARHATS)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsLittleSanyuanType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_LITTLESANYUAN)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsBigSanyuanType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_BIGSANYUAN)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsLittleSixiType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_LITTLESIXI)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsBigSixiType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_BIGSIXI)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsBompHorseAddScoreType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_BOMBHORSE_EXTRA_ADD)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsBompHorseMultipleScoreType() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_BOMBHORSE_EXTRA_MULTIPLE)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::IsGangshangKaihuaHu(std::vector<Lint>& hu)
{
	for (Lint i = 0;i < hu.size();i ++)
	{
		if (hu[i] ==  HU_GANGSHANGKAIHUA)
		{
			return true;
		}
	}
	return false;
}
	
Lint GameHandler_GuangDongOneHundler::GetGhostCardNum(Lint pos) const
{
	int ghostNum = 0;
	if (IsValidPos(pos))
	{
		CardVector cards = m_handCard[pos];
		if (m_curGetCard && (m_playerHuInfo[pos] == WIN_SUB_DZIMO || m_playerHuInfo[pos] == WIN_SUB_ZIMO))
		{
			cards.push_back(m_curGetCard);
		}
		for (Lint i = 0; i < cards.size(); ++i)
		{
			std::vector<CardValue>::const_iterator cit = m_ghostCardReal.begin();
			for ( ; cit != m_ghostCardReal.end(); ++cit)
			{
				if (cards[i]->m_color == cit->m_color && cards[i]->m_number == cit->m_number)
				{
					ghostNum++;
				}
			}
		}
	}
	return ghostNum;
}

bool GameHandler_GuangDongOneHundler::IsValidPos(const Lint& pos) const
{
	bool result = (pos >=0 && pos < m_deskplayer_num);
	return result;
}

Lint GameHandler_GuangDongOneHundler::GetSocore(const Lint pos)
{
	Lint base = 2;
	std::vector<Lint> hu = m_thinkRet[pos].m_hu;
	for(Lint i = 0; i < hu.size(); ++i)
	{
		if(HU_SHISANYAO == hu[i] && IsShisanyaoType())
		{
			base *= 8;
			break;
		}
		if(HU_ZIYISE == hu[i] && IsAllWindType())
		{
			base *= 8;
			break;
		}
		if (HU_QINGQIDUI == hu[i])
		{
			base *= 8;
			break;
		}
		if(HU_YAOJIU == hu[i] && IsYaojiuType())
		{
			base *= 6;
			break;
		}
		if(HU_QINGPENG == hu[i] && IsDuiduihuType() && IsQingyiseType())
		{
			base *= 6;
			break;
		}
		if (HU_QIDUI == hu[i] && IsXiaoQiDuiType())
		{
			base *= 4;
			break;
		}
		if(HU_QINGYISE == hu[i] && IsQingyiseType())
		{
			base *= 4;
			break;
		}
		if (HU_HUNPENG== hu[i] && IsDuiduihuType() && IsHunyise2TimesScore())
		{
			base *= 4;
			break;
		}
		if(HU_PENGPENGHU == hu[i] && IsDuiduihuType())
		{
			base *= 2;
			break;
		}
		if(HU_HUNYISE == hu[i] && IsHunyise2TimesScore())
		{
			base *= 2;
			break;
		}
		if(HU_FOURGHOST == hu[i] && IsFourGhost2TimesScore())
		{
			base *= 2;
			break;
		}
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

Lint GameHandler_GuangDongOneHundler::GetMaPaiBaseScore(const Lint pos)
{
	Lint baseScore = 2;

	if(IsMaGenDifen())
	{
		baseScore = GetSocore(pos);
	}
	return baseScore;
}

Lint GameHandler_GuangDongOneHundler::GetGengzhuangBaseScore()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_GENGZHUANGTHREE)
			return 3;

		if (*iTor == PT_GENGZHUANGTWO)
			return 2;

		if (*iTor == PT_GENGZHUANGONE)
			return 1;
	}
	return 1;
}

void GameHandler_GuangDongOneHundler::GenerateGhost()
{
	if (GetGhostType() == PT_GHOSTCARDFLOP)
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
			else if (card->m_color == CARD_COLOR_FENG)		//风
			{
				ghost1.m_number = number1 % 4 == 0 ? 4: number1 % 4;
				ghost1.m_color = number1 > 4 ? CARD_COLOR_JIAN : CARD_COLOR_FENG;

			}
			else if (card->m_color == CARD_COLOR_JIAN)		//箭
			{
				ghost1.m_number = number1 % 3 == 0 ? 3: number1 % 3;
				ghost1.m_color = number1 > 3 ? CARD_COLOR_FENG : CARD_COLOR_JIAN;

			}
			m_ghostCardReal.push_back(ghost1);
			//录像
			std::vector<CardValue> cards;
			CardValue cardValue;
			cardValue.m_color = m_ghostCardFlop.m_color;
			cardValue.m_number = m_ghostCardFlop.m_number;
			cards.push_back(cardValue);
			m_video.AddOper(VIDEO_GHOST_CARD, 0, cards);
		}
	}
	else if (GetGhostType() == PT_GHOSTCARDBAIBAN)
	{
		CardValue ghost1;
		ghost1.m_color = 5;
		ghost1.m_number = 3;
		m_ghostCardReal.push_back(ghost1);
	}
	LMsgS2CGhostCard ghoshCard;
	ghoshCard.m_ghostCard.m_color = m_ghostCardFlop.m_color;
	ghoshCard.m_ghostCard.m_number = m_ghostCardFlop.m_number;
	m_desk->BoadCast(ghoshCard);
}

const std::vector<CardValue> GameHandler_GuangDongOneHundler::GetGhostCard() const
{
	return m_ghostCardReal;
}

void GameHandler_GuangDongOneHundler::calHorseNum(LMsgS2CZhaBirdShow &maPaiStruct, Lint winpos[], Lint maNum[] )
{
	Lint LeftCardNum = m_deskCard.size();
	if (LeftCardNum <= 0 || GameBuyhorseNum() == PT_ZHANIAO0)
	{
		return ;
	}
	
	for (Lint i = 0; i < GameBuyhorseNum() && i < LeftCardNum; ++i)
	{
		auto itor = m_deskCard.back();
		m_deskCard.pop_back();

		//中马计算
		BirdInfo maPaiInfo;
		maPaiInfo.m_pos = -1;
		for (Lint userindex = 0; userindex < m_deskplayer_num; ++userindex)
		{
			if (WIN_SUB_DZIMO == winpos[userindex] || WIN_SUB_ZIMO == winpos[userindex] || WIN_SUB_DBOMB == winpos[userindex] || WIN_SUB_BOMB == winpos[userindex])
			{
				if (calcMaPai(m_zhuangpos, userindex, itor->m_number))
				{
					maNum[userindex]++;
					maPaiInfo.m_pos = userindex;
					maPaiStruct.m_end_pos = userindex;
				}
			}
		}
		maPaiInfo.m_card.m_color = itor->m_color;
		maPaiInfo.m_card.m_number = itor->m_number;
		maPaiStruct.m_bird_infos.push_back(maPaiInfo);
	}
	//录像
	std::vector<CardValue> cards;
	for (Lint i = 0; i < maPaiStruct.m_bird_infos.size(); ++i )
	{
		cards.push_back(maPaiStruct.m_bird_infos[i].m_card);
	}
	m_video.AddOper(VIDEO_OPEN_BIRD_SHOW, maPaiStruct.m_end_pos,cards);
}

void GameHandler_GuangDongOneHundler::calPlayerHitHorseNum(const std::vector<CardValue>& horsecards, Lint maNum[])
{
	std::vector<CardValue>::const_iterator cit = horsecards.begin();
	for ( ; cit != horsecards.end(); ++cit)
	{
		for (Lint userindex = 0; userindex < m_deskplayer_num; ++userindex)
		{
			if (calcMaPai(m_zhuangpos, userindex, cit->m_number))
			{
				maNum[userindex]++;
			}
		}
	}
}

void GameHandler_GuangDongOneHundler::calRedDragonSceor(Lint winpos[], Lint bombpos, Ldouble gold[])
{
	return;
}

void GameHandler_GuangDongOneHundler::FillspecifichuType(OperateState& gameinfo)
{
	gameinfo.m_duiduihuType = IsDuiduihuType();
	gameinfo.m_qingyiseType = IsQingyiseType();
	gameinfo.m_yaojiuType = IsYaojiuType();
	gameinfo.m_yaojiuHas1or9 = IsYaojiuHasOneOrNine();
	gameinfo.m_allwindType = IsAllWindType();
	gameinfo.m_shisanyaoType = IsShisanyaoType();
	gameinfo.m_hunyiseType = IsHunyise2TimesScore();
	gameinfo.m_qiduiType = IsXiaoQiDuiType();
}

void GameHandler_GuangDongOneHundler::UpdateEndInfo(Lint winpos[], Lint maNum[], const Lint result)
{
	if (result == WIN_NONE)
	{
		return;
	}

	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		if (WIN_SUB_DZIMO == winpos[i] || WIN_SUB_ZIMO == winpos[i])
		{
			Lint ghostNum = GetGhostCardNum(i);
			if (0 == ghostNum)
			{
				m_noghostZimo[i] = 1;
			}
			else
			{
				m_ghostZimo[i] = 1;
			}
		}
		m_maNum[i] = maNum[i];
	}
}

bool GameHandler_GuangDongOneHundler::IsMingGangkeqiang() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_MINGGANGKEQIANG)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::CanTianhu()
{
	if (!IsTiDihu10Time())
	{
		m_canTianHu = false;
	}
	return m_canTianHu;
}

bool GameHandler_GuangDongOneHundler::CanQiangGangHuHorseJiaBei()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_QIANGGANG_ZHONGMA2)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::CanHuangZhuangHuangGang()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_HUANGZHUANG_HUAGNGANG)
			return true;
	}
	return false;
}

bool GameHandler_GuangDongOneHundler::CanDihu()
{
	if (!IsTiDihu10Time())
	{
		m_canDiHu = false;
	}
	return m_canDiHu;
}

void GameHandler_GuangDongOneHundler::UpdateZhuangPos(Lint result, Lint winpos[], Lint bombpos, Lint& zhuangpos)
{
	int huNum = 0;
	for (Lint i = 0; i < m_deskplayer_num; ++i)
	{
		//判断庄
		if (result != WIN_NONE)
		{
			if (winpos[i] == WIN_SUB_ZIMO || winpos[i] == WIN_SUB_DZIMO || winpos[i] == WIN_SUB_BOMB || winpos[i] == WIN_SUB_DBOMB)
			{
				zhuangpos = i;
				huNum++;
			}
		}
		if (huNum > 1)
		{
			zhuangpos = bombpos;
		}
	}
	//连庄
	UpdateSuccessiveInfo(m_zhuangpos, zhuangpos);
}

bool GameHandler_GuangDongOneHundler::IsTiDihu10Time() const
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_TIDITYPE)
			return true;
	}
	return false;
}

void GameHandler_GuangDongOneHundler::UpdateSuccessiveInfo(Lint prezhuangPos, Lint curzhuangPos)
{
	if (IsValidPos(prezhuangPos) && IsValidPos(curzhuangPos))
	{
		if (prezhuangPos == curzhuangPos)
		{
			m_successive[prezhuangPos]++;
		}
		else
		{
			memset(m_successive, 0, sizeof(m_successive));
			m_successive[curzhuangPos] = 1;
		}
	}
}

const std::vector<CardValue> GameHandler_GuangDongOneHundler::GetHorseFromBirdInfo(const std::vector<BirdInfo>& birdinfo)
{
	std::vector<CardValue> horseinfo;
	std::vector<BirdInfo>::const_iterator cit = birdinfo.begin();
	for ( ; cit != birdinfo.end(); ++cit)
	{
		horseinfo.push_back(cit->m_card);
	}
	return horseinfo;
}

Lint GameHandler_GuangDongOneHundler::GetHorseGenGangHuPos(Lint winpos[], Lint bombpos, Lint winner) const
{
	Lint huPos = 0;
	if (winner > 1)
	{
		huPos = bombpos;
	}
	else
	{
		for (Lint i = 0; i < m_deskplayer_num; ++i)
		{
			if (WIN_SUB_DZIMO == winpos[i] || WIN_SUB_ZIMO == winpos[i] || WIN_SUB_DBOMB == winpos[i] || WIN_SUB_BOMB == winpos[i])
			{
				huPos = i;
				break;
			}
		}
	}
	return huPos;
}

Lint GameHandler_GuangDongOneHundler::GetQihuFanshu()
{
	auto iTor = m_desk->getPlayType().begin();
	for (; iTor != m_desk->getPlayType().end(); ++iTor)
	{
		if (*iTor == PT_NOFANCANHU)
			return 0;
		else if (*iTor == PT_THREEFANCANHU)
			return 3;
		else if (*iTor == PT_FOURFANCANHU)
			return 4;
		else if (*iTor == PT_FIVEFANCANHU)
			return 5;
	}
	return 0;
}
