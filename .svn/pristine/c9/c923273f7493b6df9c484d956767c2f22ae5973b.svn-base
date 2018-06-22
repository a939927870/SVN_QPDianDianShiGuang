#include "GameHandler.h"
#include "Desk.h"
#include "Card.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

class GameHandler_Hongzhong : public GameHandler
{
public:
	bool startup(Desk *desk)
	{
		if (desk)
		{
			shutdown();
			m_desk = desk;
			m_playtype.clear();
		}
		return true;
	}

	void shutdown(void)
	{
		m_desk = NULL;
		m_curOutCard = NULL;
		m_curGetCard = NULL;
		for (Lint i = 0; i< DESK_USER_COUNT; i ++)
		{
			m_louHuCard[i] = 0;
			m_startHuRet[i] = 0;
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
		}
		m_beforePos = 0;
		m_beforeType = 0;
		m_gold = 0;
		m_zhuangpos = 0;
		m_curPos = 0;
		m_endStartPos = 0;
		m_endStartCurPos = 0;
		m_isThinkQiangGang = false;
		m_needGetCard = false;
		m_deskCard.clear();
		m_curOutCards.clear();
		mGameInfo.m_gangcard.clear();
	}

	void DeakCard()
	{		
		if (!m_desk || !m_desk->m_vip)
		{
			return;
		}
		for(Lint i = 0 ; i < DESK_USER_COUNT; ++i)
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
		}
		m_deskCard.clear();
		m_curOutCard = NULL;//当前出出来的牌
		m_curGetCard = NULL;
		m_needGetCard = false;
		m_curPos = m_zhuangpos;
		m_desk->UpdateLastOptTimeAndPos(m_curPos);
		m_isThinkQiangGang = false;
		m_beforePos = INVAILD_POS;
		mGameInfo.m_gangcard.clear();
		memset(m_first_turn, true, sizeof(m_first_turn));
		memset(m_angang, 0, sizeof(m_angang));//暗杠数量
		memset(m_minggang, 0, sizeof(m_minggang));
		memset(m_diangang, 0, sizeof(m_diangang));
		memset(m_adiangang, 0, sizeof(m_adiangang));
		memset(m_louHuCard, 0, sizeof(m_louHuCard));
		memset(m_startHuRet,0,sizeof(m_startHuRet));
		memset(m_gangPos,0,sizeof(m_gangPos));
		memset(m_playerHuInfo, 0, sizeof(m_playerHuInfo));
		memset(m_playerBombInfo, 0, sizeof(m_playerBombInfo));

		//发牌   
		if(gConfig.GetDebugModel() && (m_desk->m_specialCard[0].m_color > 0 && m_desk->m_specialCard[0].m_number > 0))   //玩家指定发牌 牌局
		{
			gCardMgr.DealCard2(m_handCard,m_desk->m_desk_user_count,m_deskCard,m_desk->m_specialCard, m_desk->getGameType(),m_playtype);
		}
		else                //正常随机发牌 牌局
		{
			gCardMgr.DealCard(m_handCard,m_desk->m_desk_user_count,m_deskCard, m_desk->getGameType(),m_playtype);
		}

		//庄家多发一张牌
		Card* newCard = m_deskCard.back();
		m_handCard[m_curPos].push_back(newCard);
		m_deskCard.pop_back();
		gCardMgr.SortCard(m_handCard[m_curPos]);
		//发送消息给客户端
		for(Lint i = 0 ; i < m_desk->m_desk_user_count; ++i)
		{
			if(m_desk->m_user[i] != NULL)
			{
				LMsgS2CPlayStart msg;
				msg.m_zhuang = m_curPos;
				msg.m_pos = i;
				for(int x=0;x<m_desk->m_desk_user_count;x++)
				{
					msg.m_score.push_back(m_desk->m_vip->m_score[x]);
				}
				for(Lsize j = 0 ; j < m_handCard[i].size(); ++j)
				{
					msg.m_cardValue[j].m_number = m_handCard[i][j]->m_number;
					msg.m_cardValue[j].m_color = m_handCard[i][j]->m_color;
				}

				for(Lint j = 0 ; j < m_desk->m_desk_user_count; ++j)
				{
					msg.m_cardCount[j] = m_handCard[j].size();
				}
				msg.m_dCount = (Lint)m_deskCard.size();
				m_desk->m_user[i]->Send(msg);
			}
		}

		//录像功能
		m_video.Clear();
		Lint id[4];
		Ldouble score[4];
		memset(id,0,sizeof(id));
		memset(score,0.0f,sizeof(score));
		std::vector<CardValue> vec[4];
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
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
		m_video.DealCard(id, vec,gWork.GetCurTime().Secs(),m_zhuangpos,score,m_desk->GetDeskId(),m_desk->m_vip->m_curCircle,m_desk->m_vip->m_maxCircle,m_desk->m_flag, &m_desk->getPlayType());
	}

	void SetDeskPlay()
	{
		if (!m_desk || !m_desk->m_vip)
		{
			return;
		}
		if (m_desk->m_vip)
			m_desk->m_vip->SendInfo();

		m_desk->setDeskState(DESK_PLAY);
		DeakCard();
		CheckStartPlayCard();
	}
	void ProcessRobot(Lint pos, User * pUser)
	{
		if (pos < 0 || pos > 3)
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
			{
				//打出牌去
				if (m_desk->getDeskPlayState() == DESK_PLAY_GET_CARD && m_curPos == pos)
				{
					LMsgC2SUserPlay msg;
					msg.m_thinkInfo.m_type = THINK_OPERATOR_OUT;
					CardValue card;
					card.m_color = m_handCard[pos][0]->m_color;
					card.m_number = m_handCard[pos][0]->m_number;
					msg.m_thinkInfo.m_card.push_back(card);
					m_desk->HanderUserPlayCard(pUser, &msg);
				}
			}
			break;
		case DESK_PLAY_THINK_CARD:
			{
				if (m_thinkInfo[pos].NeedThink())
				{
					for (int i = 0; i < m_thinkInfo[pos].m_thinkData.size(); i ++)
					{
						if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
						{
							LMsgC2SUserOper msg;
							msg.m_think.m_type = THINK_OPERATOR_NULL;		//
							std::vector<Card*>& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
							for (int j = 0; j <  mCard.size(); j++)
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
							std::vector<Card*>& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
							for (int j = 0; j <  mCard.size(); j++)
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
		case DESK_PLAY_END_CARD:
			{
				if (m_desk->getDeskPlayState() == DESK_PLAY_END_CARD && m_curPos == pos)
				{
					LMsgC2SUserEndCardSelect msg;
					msg.m_flag = 0;
					m_desk->HanderUserEndSelect(pUser, &msg);
					return;
				}
			}
			break;
		default:
			{

			}
		}
	}

	void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
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
		if(pos == INVAILD_POS)
		{
			//pUser->Send(sendMsg);
			LLOG_DEBUG("HanderUserPlayCard pos error %s", pUser->m_userData.m_nike.c_str());
			return;
		}

		if(pos != m_curPos)
		{
			pUser->Send(sendMsg);
			LLOG_ERROR("HanderUserPlayCard not my pos %d:%d", pos, m_curPos);
			return;
		}

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
						gCardMgr.EraseCard(m_handCard[pos], m_curOutCard);
						m_desk->BoadCast(sendMsg);
						m_beforePos = pos;
						m_beforeType = THINK_OPERATOR_OUT;

						//录像
						std::vector<CardValue> cards;
						CardValue card;
						card.m_color = m_curOutCard->m_color;
						card.m_number = m_curOutCard->m_number;
						cards.push_back(card);
						m_video.AddOper(VIDEO_OPER_OUT_CARD, pos, cards);

						//这里玩家思考
						SetThinkIng();
						break;
					}
				}
			}
			return;
		}


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
				m_playerHuInfo[m_curPos] = WIN_SUB_ZIMO;
				CardVector winCards[DESK_USER_COUNT] ;
				for (int i = 0; i <m_desk->m_desk_user_count; i++)
				{
					if(m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
					{
						winCards[i] = m_thinkRet[i].m_card;
					}
				}
				OnGameOver(WIN_ZIMO, m_playerHuInfo, INVAILD_POS, winCards);
			}
			else if(unit->m_type == THINK_OPERATOR_AGANG)
			{
				//录相;
				VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
				//
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0],4);	
				m_desk->BoadCast(sendMsg);
				m_angang[pos] += 1;
				m_curOutCard = unit->m_card[0];
				//录像
				std::vector<CardValue> cards;
				for (int i = 0; i < 4; i ++)
				{
					CardValue card;
					card.m_color = unit->m_card[0]->m_color;
					card.m_number = unit->m_card[0]->m_number;
					cards.push_back(card);
					m_abombCard[pos].push_back(unit->m_card[0]);
				}
				m_video.AddOper(VIDEO_OPER_AN_GANG, pos, cards);
				m_beforeType = THINK_OPERATOR_AGANG;
				SetPlayIng(pos, true, true, true, true);
			}
			else if(unit->m_type == THINK_OPERATOR_MGANG)
			{
				//录相;
				VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
				//	m_desk->BoadCast(sendMsg);
				m_curOutCard = unit->m_card[0];
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0],1);	

				//这里玩家思考
				m_beforePos = pos;
				m_beforeType = THINK_OPERATOR_MGANG;
				SetThinkIng(true);
			}
			else if(unit->m_type == THINK_OPERATOR_ABU)
			{
				//录相;
				VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
				//
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0],4);	
				m_desk->BoadCast(sendMsg);
				m_angang[pos] += 1;
				//录像
				std::vector<CardValue> cards;
				for (int i = 0; i < 4; i ++)
				{
					CardValue card;
					card.m_color = unit->m_card[0]->m_color;
					card.m_number = unit->m_card[0]->m_number;
					cards.push_back(card);
					m_abombCard[pos].push_back(unit->m_card[0]);
				}
				m_video.AddOper(VIDEO_OPER_AN_BU, pos, cards);
				//这里玩家思考
				m_beforePos = pos;
				m_beforeType = THINK_OPERATOR_ABU;
				SetPlayIng(pos,true, true, true, true);
			}
			else if(unit->m_type == THINK_OPERATOR_MBU)
			{
				//录相;
				VideoDoing(unit->m_type,pos,unit->m_card[0]->m_color,unit->m_card[0]->m_number);
				m_curOutCard = unit->m_card[0];
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0],1);
				//m_desk->BoadCast(sendMsg);	

				//这里玩家思考
				m_beforePos = pos;
				m_beforeType = THINK_OPERATOR_MBU;
				SetThinkIng(true);
			}
		}
		else
		{
			LLOG_DEBUG("Desk::HanderUserPlayCard %s,%d", pUser->m_userData.m_nike.c_str(), msg->m_thinkInfo.m_type);
		}
	}

	void HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg)
	{
		LMsgS2CUserOper sendMsg;
		sendMsg.m_pos = m_curPos;
		sendMsg.m_think = msg->m_think;

		Lint pos = m_desk->GetUserPos(pUser);
		if (pos == INVAILD_POS || !m_thinkInfo[pos].NeedThink())
		{
			sendMsg.m_errorCode = 1;
			pUser->Send(sendMsg);
			LLOG_DEBUG("HanderUserEndSelect pos error pos: %d     curPos:%d", pos, m_endStartCurPos);
			return;
		}

		bool find = false;
		for(Lsize i = 0 ; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
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
				m_louHuCard[pos] = m_thinkInfo[pos].m_thinkData[0].m_score;
			}
		}

		//录相;
		VideoDoing(msg->m_think.m_type,pos,(msg->m_think.m_card.size()>0)?msg->m_think.m_card[0].m_color:0,(msg->m_think.m_card.size()>0)?msg->m_think.m_card[0].m_number:0);

		if (msg->m_think.m_type == THINK_OPERATOR_BOMB)
		{
			LMsgS2CUserOper send;
			send.m_pos = pos;
			send.m_errorCode = 0;
			send.m_think = msg->m_think;
			send.m_card.m_color = (m_curOutCard == NULL)? 1 : m_curOutCard->m_color;		//临时的 有错误
			send.m_card.m_number =(m_curOutCard == NULL)? 1 : m_curOutCard->m_number;		//临时的 有错误
			send.m_hu = m_thinkRet[pos].m_hu;
			send.m_cardCount = m_handCard[pos].size();
			for (Lint i = 0; i < send.m_cardCount; i ++)
			{
				CardValue mCard;
				mCard.m_color = m_handCard[pos][i]->m_color;
				mCard.m_number = m_handCard[pos][i]->m_number;
				send.m_cardValue.push_back(mCard);
			}
			m_desk->BoadCast(send);
		}

		//设置以及思考过了
		m_thinkInfo[pos].Reset();

		CheckThink();
	}

	void OnUserReconnect(User* pUser)
	{
		//发送当前圈数信息
		if (m_desk->m_vip)
			m_desk->m_vip->SendInfo();
		if (pUser == NULL || m_desk == NULL)
		{
			return;
		}
		Lint pos = m_desk->GetUserPos(pUser);
		if (pos == INVAILD_POS)
		{
			LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
			return;
		}
		Lint nCurPos = m_curPos;
		Lint nDeskPlayType = m_desk->getDeskPlayState();
		LMsgS2CDeskState reconn;
		reconn.m_user_count = m_desk->m_desk_user_count;
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
		for(Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			reconn.m_cardCount[i] = m_handCard[i].size();
			reconn.m_oCount[i] = m_outCard[i].size();
			reconn.m_aCount[i] = m_abombCard[i].size();
			reconn.m_mCount[i] = m_gangCard[i].size();
			reconn.m_pCount[i] = m_pengCard[i].size();
			reconn.m_eCount[i] = m_eatCard[i].size();
			reconn.m_score[i] = m_desk->m_vip->m_score[i];

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
		}

		//我的牌,客户的重连，之前莫得牌的重新拿出来发给他
		if(nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos)
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
				//reconn.m_oCount[pos]--;
				CardValue xx;
				xx.m_color = m_curOutCard->m_color;
				xx.m_number = m_curOutCard->m_number;
				reconn.m_cardValue[reconn.m_cardCount[pos]-1]=xx;
			}
		}
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
		if(nDeskPlayType == DESK_PLAY_THINK_CARD )
		{
			if (m_thinkInfo[pos].NeedThink())
			{
				LMsgS2CThink think;
				think.m_time = 15;
				think.m_flag = 1;
				m_desk->UpdateLastOptTimeAndPos(pUser);
				think.m_card.m_color = (m_curOutCard == NULL)? 1 : m_curOutCard->m_color;		//临时的 有错误 m_curOutCard->m_color;
				think.m_card.m_number = (m_curOutCard == NULL)? 1 : m_curOutCard->m_number;		//临时的 有错误m_curOutCard->m_number;
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
					think.m_think.push_back(info);
				}
				pUser->Send(think);
			}
		}

		///海底牌
		if(nDeskPlayType == DESK_PLAY_END_CARD && pos == m_endStartCurPos)
		{
			LMsgS2CUserEndCardSelect msg;
			msg.m_pos = pos;
			msg.m_time = 15;
			pUser->Send(msg);
		}

		//我出牌
		if(nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos)
		{
			LMsgS2COutCard msg;
			msg.m_time = 15;
			msg.m_pos = pos;
			m_desk->UpdateLastOptTimeAndPos(pos);
			msg.m_deskCard = (Lint)m_deskCard.size();
			msg.m_flag = (m_curGetCard&&m_needGetCard)?0:1;
			msg.m_gang = m_gangPos[pos];
			msg.m_end = m_deskCard.size()==1?1:0;
			if(m_needGetCard && m_curGetCard)
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
		if(m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD&&!m_isThinkQiangGang)
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

	void CheckStartPlayCard()
	{

		bool noBody = true;
		for(Lsize i = 0 ; i < 4; ++i)
		{
			if(m_startHu[i].size())
			{
				noBody = false;
				break;
			}
		}

		if(noBody)
		{
			//不用抓牌了，直接思考		//有BUG 思考时手里多张牌
			SetPlayIng(m_curPos,false, false, true, true,true);
			m_curGetCard = m_handCard[m_curPos].back();
			m_needGetCard = true;
		}
	}

	//摸牌
	void SetPlayIng(Lint pos,bool needGetCard,bool gang, bool needThink, bool canhu,bool first_think=false)
	{
		if (m_desk == NULL )
		{
			LLOG_DEBUG("HanderUserEndSelect NULL ERROR ");
			return;
		}
		if (pos < 0 || pos >= INVAILD_POS)
		{
			LLOG_ERROR("Desk::SetPlayIng pos error ！");
			return;
		}
		if(m_first_turn[pos]&&needThink)
		{
			mGameInfo.m_first_turn = true;
			m_first_turn[pos]=false;
		}
		else{
			mGameInfo.m_first_turn = false;
		}
		//穿庄
		if(m_deskCard.size()<=m_playtype.I_ZhaNiao&&needGetCard)
		{
			if(!m_deskCard.empty()&&gang)
			{

			}
			else{
				OnGameOver(WIN_NONE, m_playerHuInfo, INVAILD_POS, NULL);
				return;
			}	
		}
		m_curPos = pos;
		m_desk->UpdateLastOptTimeAndPos(m_curPos);
		//我摸牌思考信息
		m_thinkInfo[pos].m_thinkData.clear();
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
			mGameInfo. m_GameType = m_desk->getGameType();	// 0 湖南， 3， 长沙
			mGameInfo. b_canEat = false;		// 是否可以吃
			mGameInfo. b_canHu = canhu;		// 是否可以胡
			mGameInfo. b_onlyHu = false;
			mGameInfo. m_thinkGang = false;	// 单独处理是不是杠的牌
			mGameInfo. m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
			mGameInfo. m_playerPos = m_curPos;	// 当前一个出牌位置
			mGameInfo. m_cardState = THINK_OPERATOR_NULL;	// 当前一个出牌状态
			mGameInfo. m_endStartPos = m_endStartPos;	// 结束位置，即海底位置
			mGameInfo. m_MePos = pos;		// 玩家的位置
			mGameInfo. m_QiXiaoDui = m_playtype.H_Xiaoqidui;
			mGameInfo. m_pt_laizi = true;
			mGameInfo. m_tianhu = false;
			mGameInfo. m_dihu = false;
			m_thinkInfo[pos].m_thinkData = gCardMgr.CheckGetCardOperator(m_handCard[pos],m_pengCard[pos],m_abombCard[pos],m_gangCard[pos],m_eatCard[pos],m_curGetCard,mGameInfo);
			VideoThink(pos);
		}
		if (m_needGetCard)
		{
			m_handCard[pos].push_back(m_curGetCard);
			gCardMgr.SortCard(m_handCard[pos]);
		}

		if(m_thinkInfo[pos].HasHu())
		{
			m_thinkRet[pos] = m_thinkInfo[pos].m_thinkData.front();
			LMsgS2CUserPlay sendMsg;
			sendMsg.m_errorCode = 0;
			sendMsg.m_pos = pos;
			ThinkData thinkdate;
			thinkdate.m_type = THINK_OPERATOR_BOMB;
			if(m_curGetCard)
			{
				CardValue card;
				card.m_color = m_curGetCard->m_color;
				card.m_number = m_curGetCard->m_number;
				thinkdate.m_card.push_back(card);	
			}
			sendMsg.m_cs_card = thinkdate;
			//录相;
			VideoDoing(THINK_OPERATOR_BOMB,pos,0,0);

			if (m_curGetCard)
			{
				gCardMgr.EraseCard(m_handCard[pos], m_curGetCard);
				sendMsg.m_huCard.m_color = m_curGetCard->m_color;
				sendMsg.m_huCard.m_number = m_curGetCard->m_number;
			}
			sendMsg.m_hu = m_thinkRet[pos].m_hu;
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
			m_playerHuInfo[m_curPos] = WIN_SUB_ZIMO;
			CardVector winCards[DESK_USER_COUNT] ;
			for (int i = 0; i <m_desk->m_desk_user_count; i++)
			{
				if(m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
				{
					winCards[i] = m_thinkRet[i].m_card;
				}
			}
			OnGameOver(WIN_ZIMO, m_playerHuInfo, INVAILD_POS, winCards);
			return;
		}

		for(Lint i = 0 ; i <m_desk->m_desk_user_count; ++i)
		{
			if(m_desk->m_user[i] != NULL)
			{
				LMsgS2COutCard msg;
				msg.m_time = 15;
				msg.m_pos = pos;
				m_desk->UpdateLastOptTimeAndPos(pos);
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

					for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
					{		
						ThinkData info;
						info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
						if(first_think&&info.m_type==THINK_OPERATOR_BOMB&&!(m_handCard[pos].empty()))
						{
							if(m_handCard[pos].back())
							{
								m_thinkInfo[pos].m_thinkData[j].m_card.push_back(m_handCard[pos].back());
								msg.m_curCard.m_number = m_handCard[pos].back()->m_number;
								msg.m_curCard.m_color = m_handCard[pos].back()->m_color;
							}
						}
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
	}


	bool calcScore(Lint result,Lint winpos[],Lint bombpos, Lint gold[], Lint& bombCount,std::vector<Lint>& winPos,LMsgS2CGameOver& over)
	{
		if(!m_desk)
		{
			return false;
		}
		if(result==WIN_BOMB)
			if(bombpos<0||bombpos>m_desk->m_desk_user_count-1)
				return false;

		//计算杠牌得分
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			gold[i] += 2* (m_desk->m_desk_user_count-1)* m_angang[i];//暗杠
			gold[i] += (m_desk->m_desk_user_count-1) * m_minggang[i];//明杠
			gold[i] += (m_desk->m_desk_user_count-1) * m_diangang[i];//点杠
			for (Lint j = 0; j <m_desk->m_desk_user_count; ++j)
			{
				if (j != i)
				{
					gold[j] -= 2 * m_angang[i];
					gold[j] -= 1 * m_minggang[i];
				}
			}

			//这里是给别人点杠
			gold[i] -= (m_desk->m_desk_user_count-1) * m_adiangang[i];

			if (result == WIN_ZIMO)
			{
				if (winpos[i] == WIN_SUB_ZIMO)
				{
					winPos.push_back(i) ;
				}

			}
			else if (result == WIN_BOMB)
			{
				if (winpos[i] == WIN_SUB_BOMB)
				{
					winPos.push_back(i) ;
					bombCount++;
				}
			}
		}

		std::multiset<Lint> bird_owner;
		if(m_playtype.I_ZhaNiao>0)
		{
			LMsgS2CZhaBirdShow zha_bird_info;
			zha_bird_info.m_end_pos = m_zhuangpos;

			if(result!=WIN_NONE)
			{
				for(int x=0;x<m_playtype.I_ZhaNiao&&(!m_deskCard.empty());x++)
				{
					BirdInfo bird;
					Card* tmp_card = m_deskCard.back();
					bird.m_card.m_color = tmp_card->m_color;
					bird.m_card.m_number = tmp_card->m_number;
					if(m_playtype.H_HongZhongLaiZi)//判断是否为红中赖子玩法
					{
						if(tmp_card->m_color==4&&tmp_card->m_number==5)
						{
							bird.m_pos=zha_bird_info.m_end_pos ;
							bird_owner.insert(bird.m_pos);
						}
						else if(tmp_card->m_number%4==1)
						{
							bird.m_pos=zha_bird_info.m_end_pos ;
							bird_owner.insert(bird.m_pos);
						}
						else{
							Lint pos=(tmp_card->m_number-1)%4+zha_bird_info.m_end_pos >3?(tmp_card->m_number-1)%4+zha_bird_info.m_end_pos-4:(tmp_card->m_number-1)%4+zha_bird_info.m_end_pos;
							if(pos>-1&&pos<m_desk->m_desk_user_count)	
							{
								bird.m_pos=pos;
								bird_owner.insert(bird.m_pos);
							}
						}
					}
					zha_bird_info.m_bird_infos.push_back(bird);
					m_deskCard.pop_back();
				}
				if(!zha_bird_info.m_bird_infos.empty())
				{
					m_desk->BoadCast(zha_bird_info);
				}
				std::vector<CardValue> cards;
				for(int x=0;x<zha_bird_info.m_bird_infos.size();x++)
				{
					cards.push_back(zha_bird_info.m_bird_infos[x].m_card);
				}
				m_video.AddOper(VIDEO_OPEN_BIRD_SHOW, zha_bird_info.m_end_pos, cards);
				for(int x=0;x<zha_bird_info.m_bird_infos.size();x++)
				{
					bool change=false;
					for(auto nn = winPos.begin();nn!=winPos.end();nn++)
					{
						if(zha_bird_info.m_bird_infos[x].m_pos==*nn)
						{
							change=true;
							break;
						}
					}
					if(!change)
					{
						zha_bird_info.m_bird_infos[x].m_pos = -1;
					}	
				}
				over.m_bird_infos.insert(over.m_bird_infos.end(),zha_bird_info.m_bird_infos.begin(),zha_bird_info.m_bird_infos.end());
			}
		}

		Lint huScore[4];
		memset(huScore,0,sizeof(huScore));
		Lint hu_BaseScore[4];
		memset(hu_BaseScore,0,sizeof(hu_BaseScore));

		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			//计算胡牌得分
			if (m_playerHuInfo[i] != WIN_SUB_NONE)
			{
				bool have_laizi = false;
				switch(m_playerHuInfo[i])
				{
				case WIN_SUB_ZIMO:
					{
						if (m_curGetCard&&m_curGetCard->m_color == 4 && m_curGetCard->m_number == 5)
						{
							have_laizi = true;
						}
						hu_BaseScore[i]=2;
						break;
					}
				case WIN_SUB_BOMB:
					{
						over.m_hu[i].push_back(HU_QiangGangHu2);
						hu_BaseScore[i]=2;
						break;
					}
				}

				for(int e=0;e<m_handCard[i].size();e++)
				{
					if(m_handCard[i][e]->m_color==4&&m_handCard[i][e]->m_number==5)
					{
						have_laizi=true;
						break;
					}
				}

				if(!have_laizi)
				{
					over.m_hu[i].push_back(HU_WuHongZhong);
					hu_BaseScore[i]+=2;
				}

			}
		}

		for(Lint x=0;x<m_desk->m_desk_user_count;x++)
		{
			if (result == WIN_ZIMO)
			{
				if(!winPos.empty()&&x==winPos.front())
				{
					for(Lint i=0;i<m_desk->m_desk_user_count;i++)
					{
						if(i!=x)
						{
							Lint addscore=hu_BaseScore[x];
						
							if(m_playtype.I_ZhaNiao)
							{
								addscore+=bird_owner.count(x)*2;	
							}

							huScore[x]+=addscore;
							huScore[i]-=addscore;
						}
					}
				}
			}
			else if(result == WIN_BOMB)
			{
				if (winpos[x] == WIN_SUB_BOMB||winpos[x] == WIN_SUB_DBOMB)
				{
					Lint addscore=hu_BaseScore[x]*3;

					if(m_playtype.I_ZhaNiao)
					{
						addscore+=bird_owner.count(x)*2*3;	
					}

					huScore[x]+=addscore;
					huScore[bombpos]-=addscore;
				}
			}
		}

		for(Lint x=0;x<m_desk->m_desk_user_count;x++)
		{
			gold[x]+=huScore[x];
		}
		return true;
	}

	void SetThinkIng(bool is_thinkQiangGang=false)
	{
		bool think = false;
		if(!m_desk)
			return;
		for(Lint i = 0 ; i < m_desk->m_desk_user_count;++i)
		{
			m_thinkRet[i].Clear();
			m_thinkInfo[i].Reset();
			if(i != m_curPos)
			{
				mGameInfo.m_GameType = m_desk->getGameType();	// 0 湖南， 3， 长沙
				mGameInfo.b_canEat = m_playtype.H_CanEat;		// 是否可以吃
				mGameInfo.b_canHu = is_thinkQiangGang;		// 是否可以胡
				mGameInfo.i_canHuScore = 0;
				mGameInfo.b_onlyHu = m_beforeType == THINK_OPERATOR_MGANG || m_beforeType == THINK_OPERATOR_MBU;
				mGameInfo.m_thinkGang = false;	// 单独处理是不是杠的牌
				mGameInfo.m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
				mGameInfo.m_playerPos = m_curPos;	// 当前一个出牌位置
				mGameInfo.m_cardState = m_beforeType;	// 当前一个出牌状态
				mGameInfo.m_endStartPos = m_endStartPos;	// 结束位置，即海底位置
				mGameInfo.m_MePos = i;		// 玩家的位置
				mGameInfo.m_QiXiaoDui = m_playtype.H_Xiaoqidui;
				mGameInfo.m_pt_laizi = true;
				mGameInfo.m_tianhu = false;
				mGameInfo.m_dihu = false;
				m_thinkInfo[i].m_thinkData = gCardMgr.CheckOutCardOperator(m_handCard[i],m_pengCard[i],m_gangCard[i],m_abombCard[i],m_eatCard[i],
					m_curOutCard,mGameInfo);
				if (m_thinkInfo[i].NeedThink())
				{
					think = true;
					VideoThink(i);
				}
			}
		}

		if (think)
		{
			if(is_thinkQiangGang)
			{
				m_isThinkQiangGang = true;
			}

			bool have_hu=false;
			for(Lint i = 0 ; i < m_desk->m_desk_user_count;++i)
			{
				if(i != m_curPos)
				{
					if(m_thinkInfo[i].HasHu())
					{
						have_hu=true;
						m_thinkRet[i] = m_thinkInfo[i].m_thinkData.front();
						LMsgS2CUserOper send;
						send.m_pos = i;
						send.m_errorCode = 0;
						send.m_think.m_type = THINK_OPERATOR_BOMB;
						CardValue card;
						card.m_color = (m_curOutCard == NULL)? 1 : m_curOutCard->m_color;
						card.m_number = (m_curOutCard == NULL)? 1 : m_curOutCard->m_number;
						send.m_think.m_card.push_back(card);
						send.m_card.m_color = (m_curOutCard == NULL)? 1 : m_curOutCard->m_color;		//临时的 有错误
						send.m_card.m_number =(m_curOutCard == NULL)? 1 : m_curOutCard->m_number;		//临时的 有错误
						send.m_hu = m_thinkRet[i].m_hu;
						send.m_cardCount = m_handCard[i].size();
						for (Lint x = 0; x < send.m_cardCount; x ++)
						{
							CardValue mCard;
							mCard.m_color = m_handCard[i][x]->m_color;
							mCard.m_number = m_handCard[i][x]->m_number;
							send.m_cardValue.push_back(mCard);
						}
						m_desk->BoadCast(send);
					}
				}
			}
			if(have_hu)
			{
				CardVector winCards[DESK_USER_COUNT] ;
				for (int i = 0; i < m_desk->m_desk_user_count; i++)
				{
					if(m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
					{
						m_playerHuInfo[i] =  WIN_SUB_BOMB;
						m_playerBombInfo[i] = WIN_SUB_ABOMB;
						winCards[i] = m_thinkRet[i].m_card;
					}
				}
				OnGameOver(WIN_BOMB, m_playerHuInfo, m_beforePos, winCards);
				return;
			}

			m_desk->setDeskPlayState(DESK_PLAY_THINK_CARD);
			for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
			{
				LMsgS2CThink think;
				think.m_time = 15;
				think.m_card.m_color = m_curOutCard->m_color;
				think.m_card.m_number = m_curOutCard->m_number;
				if (m_thinkInfo[i].NeedThink())
				{
					think.m_flag = 1;
					m_desk->UpdateLastOptTimeAndPos(m_desk->m_user[i]);
					for (Lsize j = 0; j < m_thinkInfo[i].m_thinkData.size(); ++j)
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
		}
		else
		{
			ThinkEnd();
		}
	}

	void CheckThink()
	{
		if(!m_desk)
			return;
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
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
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

	void ThinkEnd()
	{
		if(!m_desk)
			return;
		for(int i = 0; i <m_desk->m_desk_user_count; i ++)
		{
			if(m_thinkInfo[i].NeedThink())
			{
				VideoDoing(99,i,0,0);
			}
			m_thinkInfo[i].Reset();
		}
		Lint huCount = 0;

		Lint pengPos = INVAILD_POS;
		Lint gangPos = INVAILD_POS;
		Lint buPos = INVAILD_POS;
		Lint chiPos = INVAILD_POS;
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
			{
				m_playerHuInfo[i] = WIN_SUB_BOMB;
				m_playerBombInfo[i] = WIN_SUB_ABOMB;
				huCount++;
			}
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)
				gangPos = i;
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_MBU)
				buPos = i;
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)
				pengPos = i;
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_CHI)
				chiPos = i;
		}

		if (huCount != 0)
		{		
			CardVector winCards[DESK_USER_COUNT] ;
			for (int i = 0; i < m_desk->m_desk_user_count; i++)
			{
				if(m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
				{
					winCards[i] = m_thinkRet[i].m_card;
				}
			}
			OnGameOver(WIN_BOMB, m_playerHuInfo, m_beforePos, winCards);
			return;
		}

		//杠
		if (gangPos != INVAILD_POS)
		{
			m_louHuCard[gangPos]=0;
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
			for (int i = 0; i < 4; i ++)
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

			for (int i = 0; i < m_desk->m_desk_user_count; i ++)
			{
				m_thinkRet[i].Clear();
			}
			//给玩家摸一张牌
			SetPlayIng(gangPos, true, true, true, true);
			return;
		}

		//补
		if (buPos != INVAILD_POS)
		{
			m_louHuCard[buPos]=0;
			LMsgS2CUserOper send;
			send.m_pos = buPos;
			send.m_errorCode = 0;
			send.m_think.m_type = m_thinkRet[buPos].m_type;
			for(Lsize i = 0 ; i < m_thinkRet[buPos].m_card.size(); ++i)
			{
				CardValue v;
				v.m_color = m_thinkRet[buPos].m_card[i]->m_color;
				v.m_number = m_thinkRet[buPos].m_card[i]->m_number;
				send.m_think.m_card.push_back(v);
			}
			send.m_card.m_color = m_curOutCard->m_color;
			send.m_card.m_number = m_curOutCard->m_number;
			m_desk->BoadCast(send);
			gCardMgr.EraseCard(m_handCard[buPos], m_thinkRet[buPos].m_card[0],3);

			//录像
			std::vector<CardValue> cards;
			for (int i = 0; i < 4; i ++)
			{
				CardValue card;
				card.m_color = m_curOutCard->m_color;
				card.m_number = m_curOutCard->m_number;
				cards.push_back(card);
				m_gangCard[buPos].push_back(m_curOutCard);
			}
			m_video.AddOper(VIDEO_OPER_OTHER_BU, buPos, cards);


			m_diangang[buPos] += 1;
			m_adiangang[m_beforePos] += 1;

			for (int i = 0; i < m_desk->m_desk_user_count; i ++)
			{
				m_thinkRet[i].Clear();
			}
			//给玩家摸一张牌
			SetPlayIng(buPos, true, true, true, true);
			return;
		}


		if (pengPos != INVAILD_POS)
		{
			m_louHuCard[pengPos]=0;
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
			for (int i = 0; i < 3; i ++)
			{
				CardValue card;
				card.m_color = m_curOutCard->m_color;
				card.m_number = m_curOutCard->m_number;
				cards.push_back(card);
				m_pengCard[pengPos].push_back(m_curOutCard);
			}
			m_video.AddOper(VIDEO_OPER_PENG_CARD, pengPos, cards);

			for (int i = 0; i <m_desk->m_desk_user_count; i ++)
			{
				m_thinkRet[i].Clear();
			}
			//碰完打一张牌
			m_curGetCard = NULL;
			SetPlayIng(pengPos, false, false, false, false);
			m_needGetCard = true;
			return;
		}

		//吃
		if (chiPos != INVAILD_POS)
		{
			m_louHuCard[chiPos]=0;
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

			if(m_curOutCard&&m_thinkRet[chiPos].m_card.size()>2&&m_thinkRet[chiPos].m_card[0]&&m_thinkRet[chiPos].m_card[1]&&m_thinkRet[chiPos].m_card[2])
			{
				send.m_card.m_color = m_curOutCard->m_color;
				send.m_card.m_number = m_curOutCard->m_number;
				m_desk->BoadCast(send);
				gCardMgr.EraseCard(m_handCard[chiPos], m_thinkRet[chiPos].m_card[0]);
				gCardMgr.EraseCard(m_handCard[chiPos], m_thinkRet[chiPos].m_card[1]);

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

				for (int i = 0; i <m_desk->m_desk_user_count; i ++)
				{
					m_thinkRet[i].Clear();
				}
				//给玩家摸一张牌
				m_curGetCard = NULL;
				SetPlayIng(chiPos, false, false, false, false);
				m_needGetCard = true;
				return;
			}
			else
			{
				LLOG_ERROR("unknow error 101");
				return;
			}
		}

		//这里没有人操作
		if (m_beforeType == THINK_OPERATOR_MGANG || m_beforeType == THINK_OPERATOR_MBU)
		{
			if(m_isThinkQiangGang)
			{
				m_isThinkQiangGang=false;
			}
			//录像
			std::vector<CardValue> cards;
			for (int i = 0; i < 4; i ++)
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
			SetPlayIng(m_beforePos,true,true, true, true);
		}
		else
		{
			m_outCard[m_beforePos].push_back(m_curOutCard);
			m_louHuCard[m_desk->GetNextPos(m_beforePos)]=0;
			SetPlayIng(m_desk->GetNextPos(m_beforePos), true, false, true, true);
		}
	}

	void OnGameOver(Lint result,Lint winpos[],Lint bombpos, CardVector winCards[])
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_DEBUG("OnGameOver NULL ERROR ");
			return;
		}

		//计算输赢结果
		Lint gold[4]={0};
		Lint bombCount = 0;
		std::vector<Lint> winPos;

		//广播结果
		LMsgS2CGameOver over;
		over.m_user_count = m_desk->m_desk_user_count;

		//计算分数
		calcScore(result, winpos, bombpos, gold, bombCount, winPos,over);	

		Lint mgang[4] = {0};
		for(Lint i = 0 ; i < m_desk->m_desk_user_count; ++i)
		{
			mgang[i] += m_minggang[i];
			mgang[i] += m_diangang[i];
		}

		Lint zhuangPos = m_zhuangpos;
		//计算庄
		if (result == WIN_ZIMO)
		{
			if(winCards)
			{
				if(!winPos.empty())
				{
					if(winPos.front()>-1&&winPos.front()<m_desk->m_desk_user_count)
					{
						m_zhuangpos = winPos.front();
						//录像
						std::vector<CardValue> cards;
						for (Lint i = 0; i < winCards[m_zhuangpos].size(); i ++)
						{
							CardValue curGetCard;
							curGetCard.m_color = winCards[m_zhuangpos][i]->m_color;
							curGetCard.m_number = winCards[m_zhuangpos][i]->m_number;
							cards.push_back(curGetCard);
						}
						m_video.AddOper(VIDEO_OPER_ZIMO, m_zhuangpos, cards);
					}
				}
			}
		}
		else if (result == WIN_BOMB)
		{
			winpos[m_beforePos] = WIN_SUB_ABOMB;		//置点炮位
			if (bombCount > 1)
				m_zhuangpos = bombpos;
			else{
				if(!winPos.empty())
				{
					if(winPos.front()>-1&&winPos.front()<m_desk->m_desk_user_count)
					{
						m_zhuangpos = winPos.front();
					}
				}
			}

			//录像
			std::vector<CardValue> cards;
			if(winCards)
			{
				for(auto x= winPos.begin();x!=winPos.end();x++)
				{
					if(*x>-1&&*x<m_desk->m_desk_user_count)
					{
						for (Lint i = 0; i < winCards[*x].size(); i ++)
						{
							CardValue curOutCard;
							curOutCard.m_color = winCards[*x][i]->m_color;
							curOutCard.m_number = winCards[*x][i]->m_number;
							cards.push_back(curOutCard);
						}
						m_video.AddOper(VIDEO_OPER_SHOUPAO, *x, cards);
					}
				}
			}
		}
		else
		{
			m_zhuangpos = m_curPos;
			//录像
			std::vector<CardValue> cards;
			m_video.AddOper(VIDEO_OPER_HUANGZHUANG, m_curPos, cards);
		}

		//保存录像
		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_playType = m_desk->getPlayType();

		over.m_result = result;
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			gold[i] = gold[i] * m_desk->GetStake();
		}
		memcpy(over.m_cs_win, winpos, sizeof(over.m_cs_win));
		memcpy(over.m_score, gold, sizeof(over.m_score));
		memcpy(over.m_agang, m_angang, sizeof(over.m_agang));
		memcpy(over.m_mgang, m_minggang, sizeof(over.m_mgang));
		memcpy(over.m_dmgang, m_diangang, sizeof(over.m_dmgang));
		memcpy(over.m_dgang, m_adiangang, sizeof(over.m_dgang));

		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			over.m_count[i] = m_handCard[i].size();

			//手牌
			for (Lint j = 0; j < over.m_count[i]; ++j)
			{
				over.m_card[i][j].m_color = m_handCard[i][j]->m_color;
				over.m_card[i][j].m_number = m_handCard[i][j]->m_number;
			}

			//胡牌类型
			if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
			{
				for (Lint k = 0; k < m_thinkRet[i].m_hu.size(); k ++)
				{
					over.m_hu[i].push_back(m_thinkRet[i].m_hu[k]);
				}
			}
		}
		if (winCards)
		{
			for(auto x= winPos.begin();x!=winPos.end();x++)
			{
				if(*x>-1&&*x<m_desk->m_desk_user_count)
				{
					for (Lint i = 0; i < winCards[*x].size(); i ++)
					{
						CardValue curGetCard;
						curGetCard.m_color = winCards[*x][i]->m_color;
						curGetCard.m_number = winCards[*x][i]->m_number;
						over.m_hucards[*x].push_back(curGetCard);
					}
				}
			}
		}

		Ldouble cal_gold[4] = { 0,0,0,0 };

		//保存结果	
		m_desk->m_vip->AddLog(m_desk->m_user, gold, cal_gold, winpos, zhuangPos, m_angang, mgang, m_playerBombInfo, m_video.m_Id, m_video.m_time);

		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		m_video.SetEveryResult(4, gold[0], gold[1], gold[2], gold[3]);
		m_video.SetEveryResult2(4, cal_gold[0], cal_gold[1], cal_gold[2], cal_gold[3]);
		video.m_sql = m_video.GetInsertSql();
		gWork.SendMsgToDb(video);

		//检测扣房卡;
		memcpy(over.m_gold, cal_gold, sizeof(cal_gold));

		m_desk->SetDeskWait();

		//是否最后一局
		over.m_end = m_desk->m_vip->isEnd()?1:0;

		m_desk->BoadCast(over);

		m_desk->HanderGameOver();

		//////////////////////////////////////////////////////////////////////////

	}

	void VideoThink(Lint pos)
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

	void VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number)
	{
		std::vector<CardValue> cards;
		CardValue card;
		card.m_number = op;
		card.m_color  = card_color*10+card_number;
		cards.push_back(card);
		m_video.AddOper(VIDEO_OPEN_DOING, pos, cards);
	}

	void	SetPlayType(std::vector<Lint>& l_playtype){
		m_playtype.init_playtype_info(l_playtype);
		if (m_desk)
		{
			m_desk->m_desk_user_count = 4;
		}
	}
	//////////////////////////////////////////////////////////////////////////
private:
	Desk			*m_desk;
	Card*			m_curOutCard;//当前出出来的牌
	Card*			m_curGetCard;//当前获取的牌

	Lint			m_louHuCard[DESK_USER_COUNT];		//是否有漏胡记录

	Lint			m_startHuRet[DESK_USER_COUNT];//起手胡牌

	ThinkTool		m_thinkInfo[DESK_USER_COUNT];//当前打牌思考状态
	ThinkUnit		m_thinkRet[DESK_USER_COUNT];//玩家返回思考结果

	StartHuVector	m_startHu[DESK_USER_COUNT];//起手胡牌

	CardVector		m_handCard[DESK_USER_COUNT];//玩家手上的牌

	CardVector		m_outCard[DESK_USER_COUNT];	//玩家出的牌

	CardVector		m_pengCard[DESK_USER_COUNT];//玩家碰的牌，

	CardVector		m_gangCard[DESK_USER_COUNT];//玩家明杠的牌

	CardVector		m_abombCard[DESK_USER_COUNT];//玩家暗杠的牌

	CardVector		m_eatCard[DESK_USER_COUNT];//玩家吃的牌

	Lint			m_angang[DESK_USER_COUNT];//暗杠数量
	Lint			m_minggang[DESK_USER_COUNT];//明杠数量
	Lint			m_diangang[DESK_USER_COUNT];//点杠数量
	Lint			m_adiangang[DESK_USER_COUNT];//被点杠数量

	Lint			m_gangPos[DESK_USER_COUNT];	//某位玩家有过杠

	Lint			m_playerHuInfo[DESK_USER_COUNT];		//玩家胡牌信息， 因为可以多胡 放炮的人只保存了位置
	Lint			m_playerBombInfo[DESK_USER_COUNT];	//玩家放炮信息，[玩家位置对应的炮的类型]


	Lint			m_beforePos;//之前操作的位置
	Lint			m_beforeType;//之前操作的类型

	Lint			m_gold;

	Lint			m_zhuangpos;//庄家位置
	Lint			m_curPos;						//当前操作玩家
	bool			m_isThinkQiangGang;				//是否在思考抢杠胡
	Lint			m_endStartPos;					//海底牌结束位置
	Lint			m_endStartCurPos;				//海底牌当前位置
	bool			m_needGetCard;

	CardVector		m_deskCard;		//桌子上剩余的牌
	CardVector		m_curOutCards;	//当前出出来的牌		(为杠写的)

	bool			m_first_turn[DESK_USER_COUNT]; //第一轮判断

	VideoLog		m_video;						//录像
	OperateState	mGameInfo;
	PlayTypeInfo    m_playtype;

};
DECLARE_GAME_HANDLER_CREATOR(HongZhongMajiang, GameHandler_Hongzhong)