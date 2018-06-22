#include "GameHandler.h"
#include "Desk.h"
#include "Card.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

class GameHandler_XiaoYi : public GameHandler
{
	//最后胡的那张牌以及它所对应的中发白，黑三风个数
	class LastCard
	{
	public:
		Lint  m_nCardNC;    //最后胡的那张牌

		Lint  m_nType;       //0 平胡   1 七对    2 十三幺     

		Lint  m_nHuColor;   //待胡花色

		LastCard() :m_nCardNC(0),  m_nType(0), m_nHuColor(0) {}

		LastCard(Lint nCardNC, Lint nHuColor, Lint nZhong, Lint nHei) :m_nCardNC(nCardNC), m_nType(0), m_nHuColor(nHuColor) {}

		bool operator<(const LastCard& other) const
		{
			if (m_nCardNC < other.m_nCardNC)
			{
				return true;
			}
			else if (m_nCardNC > other.m_nCardNC)
			{
				return false;
			}
			else if (m_nType  < other.m_nType)
			{
				return true;
			}
			else if (m_nType  > other.m_nType)
			{
				return false;
			}
			else if (m_nHuColor  < other.m_nHuColor)
			{
				return true;
			}
			else if (m_nHuColor  > other.m_nHuColor)
			{
				return false;
			}
			else
			{
				return false;
			}
		}
	};

	//听牌状态下的可胡的牌
	typedef  std::set<LastCard>   LastCardSet;
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
		for (Lint i = 0; i< DESK_USER_COUNT; i++)
		{
			m_thinkInfo[i].Reset();
			m_thinkRet[i].Clear();
			m_handCard[i].clear();
			m_outCard[i].clear();
			m_pengCard[i].clear();
			m_minggangCard[i].clear();
			m_angangCard[i].clear();
			m_eatCard[i].clear();
			m_angang[i] = 0;
			m_minggang[i] = 0;
			m_diangang[i] = 0;
			m_adiangang[i] = 0;
			m_gangPos[i] = 0;
			m_playerHuInfo[i] = 0;
			m_playerBombInfo[i] = 0;
			m_playerIsTing[i] = 0;
			m_isAutoCard[i] = false;
			m_tingCard[i].clear();
			m_setNowTingCard[i].clear();
			m_isTingHu[i] = false;
		}
		memset(m_diangangRelation, 0, sizeof(m_diangangRelation));

		m_beforePos = 0;
		m_beforeType = 0;
		m_gold = 0;
		m_zhuangpos = 0;
		m_curPos = 0;
		m_needGetCard = false;
		m_isThinkingQiangGang = false;
		m_deskCard.clear();
		m_curOutCards.clear();
		m_first_turn = false;
		mGameInfo.m_gangcard.clear();
	}

	void DeakCard()
	{
		if (!m_desk || !m_desk->m_vip)
		{
			return;
		}
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			m_handCard[i].clear();
			m_outCard[i].clear();
			m_pengCard[i].clear();
			m_minggangCard[i].clear();
			m_angangCard[i].clear();
			m_eatCard[i].clear();
			m_thinkInfo[i].Reset();
			m_thinkRet[i].Clear();
			m_playerIsTing[i] = 0;
			m_isAutoCard[i] = false;
			m_tingCard[i].clear();
			m_setNowTingCard[i].clear();
			m_isTingHu[i] = false;
		}
		m_deskCard.clear();
		m_curOutCard = NULL;//当前出出来的牌
		m_curGetCard = NULL;
		m_needGetCard = false;
		m_isThinkingQiangGang = false;
		m_haidi_zhuangpos = INVAILD_POS;
		m_curPos = m_zhuangpos;
		m_desk->UpdateLastOptTimeAndPos(m_curPos);
		m_beforePos = INVAILD_POS;
		mGameInfo.m_gangcard.clear();
		m_first_turn = true;
		
		memset(m_diangangRelation, 0, sizeof(m_diangangRelation));
		memset(m_angang, 0, sizeof(m_angang));//暗杠数量
		memset(m_minggang, 0, sizeof(m_minggang));
		memset(m_diangang, 0, sizeof(m_diangang));
		memset(m_adiangang, 0, sizeof(m_adiangang));
		memset(m_gangPos, 0, sizeof(m_gangPos));
		memset(m_playerHuInfo, 0, sizeof(m_playerHuInfo));
		memset(m_playerBombInfo, 0, sizeof(m_playerBombInfo));

		//发牌   
		if (gConfig.GetDebugModel() && (m_desk->m_specialCard[0].m_color > 0 && m_desk->m_specialCard[0].m_number > 0))   //玩家指定发牌 牌局
		{
			gCardMgr.DealCard2(m_handCard, m_desk->m_desk_user_count, m_deskCard, m_desk->m_specialCard, m_desk->getGameType(), m_playtype);
		}
		else                //正常随机发牌 牌局
		{
			gCardMgr.DealCard(m_handCard, m_desk->m_desk_user_count, m_deskCard, m_desk->getGameType(), m_playtype);
		}

		////庄家多发一张牌
		//Card* newCard = m_deskCard.back();
		//m_handCard[m_curPos].push_back(newCard);
		//m_deskCard.pop_back();
		gCardMgr.SortCard(m_handCard[m_curPos]);

		//发送消息给客户端
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			if (m_desk->m_user[i] != NULL)
			{
				LMsgS2CPlayStart msg;
				msg.m_zhuang = m_curPos;
				msg.m_pos = i;
				for (int x = 0; x<m_desk->m_desk_user_count; x++)
				{
					msg.m_score.push_back(m_desk->m_vip->m_score[x]);
				}
				for (Lsize j = 0; j < m_handCard[i].size(); ++j)
				{
					msg.m_cardValue[j].m_number = m_handCard[i][j]->m_number;
					msg.m_cardValue[j].m_color = m_handCard[i][j]->m_color;
				}

				for (Lint j = 0; j < m_desk->m_desk_user_count; ++j)
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
		memset(id, 0, sizeof(id));
		memset(score, 0, sizeof(score));
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
		m_video.DealCard(id, vec, gWork.GetCurTime().Secs(), m_zhuangpos, score, m_desk->GetDeskId(), m_desk->m_vip->m_curCircle, m_desk->m_vip->m_maxCircle, m_desk->m_flag, &m_desk->getPlayType());
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
		switch (m_desk->getDeskPlayState())
		{
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
				for (Lint i = 0; i < m_handCard[pos].size(); ++i)
				{
					if ((m_handCard[pos][i]->m_color == 4 && m_handCard[pos][i]->m_number == 4)
						|| (m_handCard[pos][i]->m_color == 3 && m_handCard[pos][i]->m_number == 1)
						|| (m_handCard[pos][i]->m_color == 2 && m_handCard[pos][i]->m_number == 6)
						|| (m_handCard[pos][i]->m_color == 2 && m_handCard[pos][i]->m_number == 9))
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
				for (int i = 0; i < m_thinkInfo[pos].m_thinkData.size(); i++)
				{
					if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
					{
						LMsgC2SUserOper msg;
						msg.m_think.m_type = THINK_OPERATOR_NULL;		//
						std::vector<Card*>& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
						for (int j = 0; j < mCard.size(); j++)
						{
							CardValue card;
							card.m_color = mCard[j]->m_color;
							card.m_number = mCard[j]->m_number;
							msg.m_think.m_card.push_back(card);
						}
						m_desk->HanderUserOperCard(pUser, &msg);
						return;
					}
					else
					{
						LMsgC2SUserOper msg;
						msg.m_think.m_type = m_thinkInfo[pos].m_thinkData[i].m_type;
						std::vector<Card*>& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;
						for (int j = 0; j < mCard.size(); j++)
						{
							CardValue card;
							card.m_color = mCard[j]->m_color;
							card.m_number = mCard[j]->m_number;
							msg.m_think.m_card.push_back(card);
						}
						m_desk->HanderUserOperCard(pUser, &msg);
					}
				}
			}
		}
		break;
		default:
		{

		}
		}
	}

	//同步位置
	void _S_BrocastTingInfo(Lint pos)
	{
		LMsgS2CUserOperTing sendTing;
		sendTing.m_pos = pos;
		if (m_playtype.H_ANTING)
		{
			sendTing.m_type = 1;
		}
		else
		{
			sendTing.m_card.m_color = m_tingCard[pos].GetNCIndex();
		}
		sendTing.m_card.m_number = m_tingCard[pos].GetOutIndex();
		m_desk->BoadCast(sendTing);
	}

	void HanderUserPlayCard(User* pUser, LMsgC2SUserPlay* msg)
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
		if (pos == INVAILD_POS)
		{
			//pUser->Send(sendMsg);
			LLOG_DEBUG("HanderUserPlayCard pos error %s", pUser->m_userData.m_nike.c_str());
			return;
		}

		if (pos != m_curPos)
		{
			sendMsg.m_errorCode = 1;
			pUser->Send(sendMsg);
			LLOG_ERROR("HanderUserPlayCard not my pos %d:%d", pos, m_curPos);
			return;
		}

		if (msg->m_thinkInfo.m_type == THINK_OPERATOR_OUT)
		{
			if (m_thinkInfo[pos].NeedThink())
			{
				VideoDoing(99, pos, 0, 0);
			}
			if (msg->m_thinkInfo.m_card.size())
			{
				for (Lsize i = 0; i < m_handCard[pos].size(); ++i)
				{
					if (2 == m_playerIsTing[pos])
					{
						//进入听牌状态只能出摸到的牌
						if (NULL == m_curGetCard)
						{
							//出牌错误
							LLOG_ERROR("ERROR HanderUserPlayCard pos %s is ting = 2 but out card c = %d n = %d", pUser->m_userData.m_nike.c_str(), msg->m_thinkInfo.m_card[0].m_color, msg->m_thinkInfo.m_card[0].m_number);
							return;
						}
						if (m_curGetCard->GetNCIndex() != msg->m_thinkInfo.m_card[0].GetNCIndex())
						{
							LLOG_DEBUG("XiaoYiHandler HanderUserPlayCard THINK_OPERATOR_OUT outcard error!");
							msg->m_thinkInfo.m_card[0].m_color = m_curGetCard->m_color;
							msg->m_thinkInfo.m_card[0].m_number = m_curGetCard->m_number;
						}
					}

					if (m_handCard[pos][i]->m_color == msg->m_thinkInfo.m_card[0].m_color && m_handCard[pos][i]->m_number == msg->m_thinkInfo.m_card[0].m_number)
					{
						if (m_beforeType == THINK_OPERATOR_TING && 1 == m_playerIsTing[pos])
						{
							Lint tmp_ncindex = msg->m_thinkInfo.m_card[0].GetNCIndex();
							if (m_setNowTingCard[pos].find(tmp_ncindex) == m_setNowTingCard[pos].end())
							{
								LLOG_DEBUG("XiaoYiHandler  THINK_OPERATOR_TING outcard error! %d", tmp_ncindex);
								return;
							}
						}

						Lint tmpbeforetype = m_beforeType;
						{
							//出牌逻辑
							m_curOutCard = m_handCard[pos][i];
							gCardMgr.EraseCard(m_handCard[pos], m_curOutCard);
							m_beforePos = pos;
							m_beforeType = THINK_OPERATOR_OUT;

							//录像
							std::vector<CardValue> cards;
							CardValue card;
							card.m_color = m_curOutCard->m_color;
							card.m_number = m_curOutCard->m_number;
							cards.push_back(card);
							m_video.AddOper(VIDEO_OPER_OUT_CARD, pos, cards);

							//修改一下逻辑 在出牌时这张牌已经到该玩家的已出列表内
							m_outCard[pos].push_back(m_curOutCard);
							m_first_turn = false;
						}

						m_isTingHu[pos] = false;
						if (tmpbeforetype == THINK_OPERATOR_TING && 1 == m_playerIsTing[pos])
						{
							Lint tmp_ncindex = msg->m_thinkInfo.m_card[0].GetNCIndex();

							//听牌阶段为2
							m_playerIsTing[pos] = 2;
							m_isAutoCard[pos] = true;
							m_isTingHu[pos] = true;

							m_thinkRet[pos].Clear();
							m_thinkInfo[pos].Reset();
							m_tingCard[pos].setLiangPai(tmp_ncindex, m_outCard[pos].size());

							if (!m_playtype.H_ANTING)
							{
								m_desk->BoadCast(sendMsg);
							}

							//广播给所有人听牌状态
							LMsgS2CUserPlay sendMsg2;
							sendMsg2.m_errorCode = 0;
							sendMsg2.m_pos = pos;
							sendMsg2.m_cs_card.m_type = THINK_OPERATOR_TING;

							//暗听广播出牌
							if (!m_playtype.H_ANTING)
							{
								sendMsg2.m_cs_card.m_card.push_back(msg->m_thinkInfo.m_card[0]);
							}
							m_desk->BoadCast(sendMsg2);

							//录相;                         亮牌放在听牌中，花色位置放牌的NC值，牌值位置放亮牌位置（手牌、碰牌或杠牌）
							VideoDoing(THINK_OPERATOR_TING, pos, msg->m_thinkInfo.m_card[0].m_color, msg->m_thinkInfo.m_card[0].m_number);
							if (!m_playtype.H_ANTING)
							{
								SetThinkIng();
							}
							else
							{
								ThinkEnd();
							}
							break;
						}

						//这里玩家思考
						m_desk->BoadCast(sendMsg);
						SetThinkIng();
						break;
					}
				}
			}
			return;
		}

		ThinkUnit* unit = NULL;
		for (Lint i = 0; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
		{
			if (msg->m_thinkInfo.m_type == m_thinkInfo[pos].m_thinkData[i].m_type)
			{
				if (msg->m_thinkInfo.m_card.size() == m_thinkInfo[pos].m_thinkData[i].m_card.size())
				{
					bool find = true;
					for (Lsize j = 0; j < msg->m_thinkInfo.m_card.size(); ++j)
					{
						if (msg->m_thinkInfo.m_card[j].m_color != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color ||
							msg->m_thinkInfo.m_card[j].m_number != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number)
						{
							find = false;
							break;
						}
					}

					if (find)
					{
						unit = &m_thinkInfo[pos].m_thinkData[i];
						break;
					}
				}
			}
		}

		if (unit)
		{
			if (m_first_turn && (unit->m_type != THINK_OPERATOR_AGANG&&unit->m_type != THINK_OPERATOR_ABU))
			{
				m_first_turn = false;
			}
			if (unit->m_type == THINK_OPERATOR_BOMB)
			{
				//录相;
				VideoDoing(unit->m_type, pos, 0, 0);

				if (m_curGetCard)
				{
					gCardMgr.EraseCard(m_handCard[pos], m_curGetCard);
					sendMsg.m_huCard.m_color = m_curGetCard->m_color;
					sendMsg.m_huCard.m_number = m_curGetCard->m_number;
				}
				sendMsg.m_hu = unit->m_hu;
				sendMsg.m_huCard.m_color = m_curGetCard->m_color;
				sendMsg.m_cardCount = m_handCard[pos].size();
				for (Lint i = 0; i < sendMsg.m_cardCount; i++)
				{
					CardValue mCard;
					mCard.m_color = m_handCard[pos][i]->m_color;
					mCard.m_number = m_handCard[pos][i]->m_number;
					sendMsg.m_cardValue.push_back(mCard);
				}
				m_desk->BoadCast(sendMsg);

				m_thinkRet[m_curPos] = m_thinkInfo[m_curPos].m_thinkData[0];
				m_playerHuInfo[m_curPos] = WIN_SUB_ZIMO;
				CardVector winCards[DESK_USER_COUNT];
				winCards[m_curPos] = unit->m_card;
				OnGameOver(WIN_ZIMO, m_playerHuInfo, INVAILD_POS, winCards);
			}
			else if (unit->m_type == THINK_OPERATOR_AGANG)
			{
				if (m_playerIsTing[pos] == 2)
				{
					if (unit->m_card[0]->GetNCIndex() == m_tingCard[pos].m_ncCard)
					{
						m_tingCard[pos].setType(2);
						m_tingCard[pos].resetOutIndex();
						_S_BrocastTingInfo(pos);
					}
				}
				//录相;
				VideoDoing(unit->m_type, pos, unit->m_card[0]->m_color, unit->m_card[0]->m_number);
				//
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0], 4);
				m_desk->BoadCast(sendMsg);
				m_angang[pos] += 1;

				//录像
				std::vector<CardValue> cards;
				for (int i = 0; i < 4; i++)
				{
					CardValue card;
					card.m_color = unit->m_card[0]->m_color;
					card.m_number = unit->m_card[0]->m_number;
					cards.push_back(card);
					m_angangCard[pos].push_back(unit->m_card[0]);
				}
				m_curOutCard = unit->m_card[0];
				m_video.AddOper(VIDEO_OPER_AN_GANG, pos, cards);
				//这里玩家思考
				m_beforePos = pos;
				m_beforeType = THINK_OPERATOR_AGANG;
				if (m_first_turn)
				{
					SetPlayIng(pos, true, true, true, true, false, true);
					m_first_turn = false;
				}
				else
					SetPlayIng(pos, true, true, true, true);
			}
			else if (unit->m_type == THINK_OPERATOR_MGANG)
			{
				//如果杠到亮牌，那么亮牌的类型应该改为杠
				if (m_playerIsTing[pos] == 2)
				{
					if (unit->m_card[0]->GetNCIndex() == m_tingCard[pos].m_ncCard)
					{
						m_tingCard[pos].setType(2);
						m_tingCard[pos].resetOutIndex();
						_S_BrocastTingInfo(pos);
					}
				}
				//录相;
				VideoDoing(unit->m_type, pos, unit->m_card[0]->m_color, unit->m_card[0]->m_number);
				//m_desk->BoadCast(sendMsg);
				m_curOutCard = unit->m_card[0];
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0], 1);

				//这里玩家思考
				m_beforePos = pos;
				m_beforeType = THINK_OPERATOR_MGANG;
				SetThinkIng(true);
			}
			else if (unit->m_type == THINK_OPERATOR_ABU)
			{
				//如果杠到亮牌，那么亮牌的类型应该改为杠
				if (m_playerIsTing[pos] == 2)
				{
					if (unit->m_card[0]->GetNCIndex() == m_tingCard[pos].m_ncCard)
					{
						m_tingCard[pos].setType(2);
						m_tingCard[pos].resetOutIndex();
						_S_BrocastTingInfo(pos);
					}
				}
				//录相;
				VideoDoing(unit->m_type, pos, unit->m_card[0]->m_color, unit->m_card[0]->m_number);
				//
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0], 4);
				m_desk->BoadCast(sendMsg);
				m_angang[pos] += 1;
				//录像
				std::vector<CardValue> cards;
				for (int i = 0; i < 4; i++)
				{
					CardValue card;
					card.m_color = unit->m_card[0]->m_color;
					card.m_number = unit->m_card[0]->m_number;
					cards.push_back(card);
					m_angangCard[pos].push_back(unit->m_card[0]);
				}
				m_video.AddOper(VIDEO_OPER_AN_BU, pos, cards);
				//这里玩家思考
				m_beforePos = pos;
				m_beforeType = THINK_OPERATOR_ABU;
				if (m_first_turn)
				{
					SetPlayIng(pos, true, true, true, true, false, true);
					m_first_turn = false;
				}
				else
					SetPlayIng(pos, true, true, true, true);
			}
			else if (unit->m_type == THINK_OPERATOR_MBU)
			{
				//如果杠到亮牌，那么亮牌的类型应该改为杠
				if (m_playerIsTing[pos] == 2)
				{
					if (unit->m_card[0]->GetNCIndex() == m_tingCard[pos].m_ncCard)
					{
						m_tingCard[pos].setType(2);
						m_tingCard[pos].resetOutIndex();
						_S_BrocastTingInfo(pos);
					}
				}
				//录相;
				VideoDoing(unit->m_type, pos, unit->m_card[0]->m_color, unit->m_card[0]->m_number);
				//
				gCardMgr.EraseCard(m_handCard[pos], unit->m_card[0], 4);
				m_curOutCard = unit->m_card[0];
				//这里玩家思考
				m_beforePos = pos;
				m_beforeType = THINK_OPERATOR_MBU;
				SetThinkIng(true);
			}//听牌
			else if (unit->m_type == THINK_OPERATOR_TING)
			{
				//录相;
				VideoDoing(unit->m_type, pos, 0, 0);

				//该玩家置为听牌状态
				m_playerIsTing[pos] = 1;
				/*m_thinkRet[pos].Clear();
				m_thinkInfo[pos].Reset();*/
				m_beforeType = THINK_OPERATOR_TING;

				LMsgS2CCanTing send;
				std::map<Lint, LastCardSet>::iterator IT = m_setNowTingCard[pos].begin();
				while (IT != m_setNowTingCard[pos].end())
				{
					send.m_tingCards.push_back(IT->first);
					++IT;
				}
				pUser->Send(send);
			}
			else
			{
				LLOG_DEBUG("HandlerObject::HanderUserPlayCard think type %d", unit->m_type);
			}
		}
		else
		{  //取消听牌会走到这里
			if (m_playerIsTing[pos] == 1)
			{
				m_playerIsTing[pos] = 0;
			}
			LLOG_DEBUG("Desk::HanderUserPlayCard %s,%d", pUser->m_userData.m_nike.c_str(), msg->m_thinkInfo.m_type);
		}
	}

	void HanderUserOperCard(User* pUser, LMsgC2SUserOper* msg)
	{
		LMsgS2CUserOper sendMsg;
		sendMsg.m_pos = m_curPos;
		sendMsg.m_think = msg->m_think;

		Lint pos = m_desk->GetUserPos(pUser);
		if (pos == INVAILD_POS || !m_thinkInfo[pos].NeedThink())
		{
			sendMsg.m_errorCode = 1;
			pUser->Send(sendMsg);
			return;
		}

		bool find = false;
		for (Lsize i = 0; i < m_thinkInfo[pos].m_thinkData.size(); ++i)
		{
			if (m_thinkInfo[pos].m_thinkData[i].m_type == msg->m_think.m_type)
			{
				bool check = true;
				if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_CHI)
				{
					for (Lsize j = 0; j < msg->m_think.m_card.size() && j<2; ++j)
					{
						if (msg->m_think.m_card[j].m_color != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color ||
							msg->m_think.m_card[j].m_number != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number)
						{
							check = false;
							break;
						}
					}

					if (check)
					{
						m_thinkRet[pos] = m_thinkInfo[pos].m_thinkData[i];
						find = true;
						break;
					}
				}
				else if (m_thinkInfo[pos].m_thinkData[i].m_card.size() == msg->m_think.m_card.size())
				{
					for (Lsize j = 0; j < msg->m_think.m_card.size(); ++j)
					{
						if (msg->m_think.m_card[j].m_color != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_color ||
							msg->m_think.m_card[j].m_number != m_thinkInfo[pos].m_thinkData[i].m_card[j]->m_number)
						{
							check = false;
							break;
						}
					}

					if (check)
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
		}

		//录相;
		VideoDoing(msg->m_think.m_type, pos, (msg->m_think.m_card.size()>0) ? msg->m_think.m_card[0].m_color : 0, (msg->m_think.m_card.size()>0) ? msg->m_think.m_card[0].m_number : 0);

		if (msg->m_think.m_type == THINK_OPERATOR_BOMB)
		{
			LMsgS2CUserOper send;
			send.m_pos = pos;
			send.m_errorCode = 0;
			send.m_think.m_type = THINK_OPERATOR_BOMB;
			for (int x = 0; x<m_thinkRet[pos].m_card.size(); x++)
			{
				if (m_thinkRet[pos].m_card[x])
				{
					CardValue card;
					card.m_color = m_thinkRet[pos].m_card[x]->m_color;
					card.m_number = m_thinkRet[pos].m_card[x]->m_number;
					send.m_think.m_card.push_back(card);
				}
			}
			if (!m_thinkRet[pos].m_card.empty())
			{
				for (int x = 0; x<m_thinkRet[pos].m_card.size(); x++)
				{
					if (x == 0 && m_thinkRet[pos].m_card[x])
					{
						send.m_card.m_color = m_thinkRet[pos].m_card[x]->m_color;
						send.m_card.m_number = m_thinkRet[pos].m_card[x]->m_number;
					}
					else if (x == 1 && m_thinkRet[pos].m_card[x])
					{
						send.m_hucard2.m_color = m_thinkRet[pos].m_card[x]->m_color;
						send.m_hucard2.m_number = m_thinkRet[pos].m_card[x]->m_number;
					}
				}
			}

			//胡牌类型
			send.m_hu = m_thinkRet[pos].m_hu;
			send.m_cardCount = m_handCard[pos].size();
			for (Lint i = 0; i < send.m_cardCount; i++)
			{
				CardValue mCard;
				if (m_handCard[pos][i])
				{
					mCard.m_color = m_handCard[pos][i]->m_color;
					mCard.m_number = m_handCard[pos][i]->m_number;
					send.m_cardValue.push_back(mCard);
				}
			}
			if (m_desk)
				m_desk->BoadCast(send);
		}
		else
		{
			m_isTingHu[pos] = false;
		}
		
		//设置以及思考过了
		m_thinkInfo[pos].Reset();

		if (!continueThink())
		{
			CheckThink();
		}
	}


	//是否需要继续思考
	bool continueThink()
	{
		bool needThink = false;

		bool hasBomb = false;	//已经存在点炮胡牌者

								//由距离放炮者最近的玩家开始查找，仅距离最近的下家可以胡牌
		for (Lint index = 1; index < DESK_USER_COUNT; ++index)
		{
			Lint correctPos = (m_curPos + index) % DESK_USER_COUNT;

			if (hasBomb)	//已经存在点炮胡牌者
			{
				if (m_thinkRet[correctPos].m_type == THINK_OPERATOR_BOMB)
				{
					m_thinkRet[correctPos].Clear();	//手动置为空（不可胡牌）
				}
			}
			else	//还没有找到胡牌者
			{
				if (m_thinkRet[correctPos].m_type == THINK_OPERATOR_BOMB)	//已经胡牌
				{
					hasBomb = true;
				}
				else if (m_thinkInfo[correctPos].NeedThink())	//需要思考
				{
					if (m_thinkInfo[correctPos].HasHu())
					{
						needThink = true;
						break;
					}
				}
			}
		}

		//存在距离最近的胡牌者，则不需要再思考
		if (hasBomb)
		{
			needThink = false;
		}

		return needThink;
	}

	void OnUserReconnect(User* pUser)
	{
		if (pUser == NULL || m_desk == NULL)
		{
			return;
		}
		LLOG_DEBUG("Desk::OnUserReconnect pos1 %d", pUser->GetUserDataId());
		//发送当前圈数信息
		if (m_desk->m_vip)
			m_desk->m_vip->SendInfo();
		Lint pos = m_desk->GetUserPos(pUser);
		if (pos == INVAILD_POS)
		{
			LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
			return;
		}
		Lint nCurPos = m_curPos;
		Lint nDeskPlayType = m_desk->getDeskPlayState();
		//	VideoDoing(VIDEO_PLAYER_RECONNECT,pos,0,0);
		LMsgS2CDeskState reconn;
		reconn.m_state = m_desk->getDeskState();
		reconn.m_pos = nCurPos;
		reconn.m_time = 15;
		reconn.m_zhuang = m_zhuangpos;
		reconn.m_myPos = pos;
		reconn.m_user_count = m_desk->m_desk_user_count;
		reconn.m_isBaoTing = (m_playerIsTing[pos] == 2);
		if (nCurPos != pos)
		{
			reconn.m_flag = 0;
		}
		else
		{
			reconn.m_flag = 1;			//不知道对不对
		}
		reconn.m_dCount = m_deskCard.size();
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			reconn.m_cardCount[i] = m_handCard[i].size();
			reconn.m_oCount[i] = m_outCard[i].size();
			reconn.m_aCount[i] = m_angangCard[i].size();
			reconn.m_mCount[i] = m_minggangCard[i].size();
			reconn.m_pCount[i] = m_pengCard[i].size();
			reconn.m_eCount[i] = m_eatCard[i].size();
			reconn.m_score[i] = m_desk->m_vip->m_score[i];

			for (Lsize j = 0; j < m_outCard[i].size(); ++j)
			{
				if (m_playtype.H_ANTING && pos != i)
				{
					Lint ncindex = m_tingCard[i].GetNCIndex();
					if (m_playerIsTing[i] == 2 && ncindex && j == (ncindex - 1))
					{
						reconn.m_oCard[i][j].m_color = 0;
						reconn.m_oCard[i][j].m_number = 0;
					}
					else
					{
						reconn.m_oCard[i][j].m_color = m_outCard[i][j]->m_color;
						reconn.m_oCard[i][j].m_number = m_outCard[i][j]->m_number;
					}
				}
				else
				{
					reconn.m_oCard[i][j].m_color = m_outCard[i][j]->m_color;
					reconn.m_oCard[i][j].m_number = m_outCard[i][j]->m_number;
				}
			}

			for (Lsize j = 0; j < m_angangCard[i].size(); ++j)
			{
				reconn.m_aCard[i][j].m_color = m_angangCard[i][j]->m_color;
				reconn.m_aCard[i][j].m_number = m_angangCard[i][j]->m_number;
			}

			for (Lsize j = 0; j < m_minggangCard[i].size(); ++j)
			{
				reconn.m_mCard[i][j].m_color = m_minggangCard[i][j]->m_color;
				reconn.m_mCard[i][j].m_number = m_minggangCard[i][j]->m_number;
			}

			for (Lsize j = 0; j < m_pengCard[i].size(); ++j)
			{
				reconn.m_pCard[i][j].m_color = m_pengCard[i][j]->m_color;
				reconn.m_pCard[i][j].m_number = m_pengCard[i][j]->m_number;
			}

			for (Lsize j = 0; j < m_eatCard[i].size(); ++j)
			{
				reconn.m_eCard[i][j].m_color = m_eatCard[i][j]->m_color;
				reconn.m_eCard[i][j].m_number = m_eatCard[i][j]->m_number;
			}

		}

		//我的牌,客户的重连，之前莫得牌的重新拿出来发给他
		if (nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos)
		{
			CardVector tmp = m_handCard[pos];
			if (m_curGetCard)
			{
				reconn.m_cardCount[pos] -= 1;
				gCardMgr.EraseCard(tmp, m_curGetCard);
			}
			for (Lsize j = 0; j < tmp.size(); ++j)
			{
				reconn.m_cardValue[j].m_color = tmp[j]->m_color;
				reconn.m_cardValue[j].m_number = tmp[j]->m_number;
			}
		}
		else
		{
			for (Lsize j = 0; j < m_handCard[pos].size(); ++j)
			{
				reconn.m_cardValue[j].m_color = m_handCard[pos][j]->m_color;
				reconn.m_cardValue[j].m_number = m_handCard[pos][j]->m_number;
			}
		}

		//该出牌的玩家，多发一张牌，用于打出去。
		if (m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD)
		{
			if (m_curOutCard && pos != nCurPos)
			{
				reconn.m_cardCount[nCurPos] ++;
			}
			else if (m_curOutCard&&pos == m_beforePos&& m_beforeType == THINK_OPERATOR_OUT)
			{
				reconn.m_cardCount[pos]++;
				//reconn.m_oCount[pos]--;
				CardValue xx;
				xx.m_color = m_curOutCard->m_color;
				xx.m_number = m_curOutCard->m_number;
				reconn.m_cardValue[reconn.m_cardCount[pos] - 1] = xx;
			}
		}

		//听的信息
		if (m_playtype.H_ANTING)
		{
			reconn.m_tingType = 1;
		}

		LLOG_DEBUG("Desk::OnUserReconnect pos2 %d", pUser->GetUserDataId());
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			if (m_playerIsTing[i] == 2)
			{
				reconn.m_tingPos[i] = i;
				if (i == pos || !m_playtype.H_ANTING)
				{
					reconn.m_tingCard[i].m_color = m_tingCard[i].GetNCIndex();
				}
				reconn.m_tingCard[i].m_number = m_tingCard[i].GetOutIndex();
			}
		}

		pUser->Send(reconn);
		LLOG_DEBUG("Desk::OnUserReconnect pos3 %d", pUser->GetUserDataId());

		//我思考
		if (nDeskPlayType == DESK_PLAY_THINK_CARD)
		{
			if (m_thinkInfo[pos].NeedThink())
			{
				LMsgS2CThink think;
				think.m_time = 15;
				think.m_flag = 1;
				m_desk->UpdateLastOptTimeAndPos(pUser);
				think.m_card.m_color = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_color;		//临时的 有错误 m_curOutCard->m_color;
				think.m_card.m_number = (m_curOutCard == NULL) ? 1 : m_curOutCard->m_number;		//临时的 有错误m_curOutCard->m_number;
				for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
				{
					ThinkData info;
					info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;

					for (Lsize n = 0; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
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
		LLOG_DEBUG("Desk::OnUserReconnect pos4 %d", pUser->GetUserDataId());
		//我出牌
		if (nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos)
		{
			LMsgS2COutCard msg;
			msg.m_time = 15;
			msg.m_pos = pos;
			m_desk->UpdateLastOptTimeAndPos(pos);
			msg.m_deskCard = (Lint)m_deskCard.size();
			msg.m_flag = (m_curGetCard&&m_needGetCard) ? 0 : 1;
			msg.m_gang = m_gangPos[pos];
			msg.m_end = m_deskCard.size() == 1 ? 1 : 0;
			if (m_needGetCard && m_curGetCard)
			{
				msg.m_curCard.m_color = m_curGetCard->m_color;
				msg.m_curCard.m_number = m_curGetCard->m_number;
			}

			if (m_playerIsTing[pos] != 1)
			{
				for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
				{
					ThinkData info;
					info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
					for (Lsize n = 0; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
					{
						CardValue v;
						v.m_color = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_color;
						v.m_number = m_thinkInfo[pos].m_thinkData[j].m_card[n]->m_number;
						info.m_card.push_back(v);
					}
					msg.m_think.push_back(info);
				}
			}
			
			pUser->Send(msg);
		}
		LLOG_DEBUG("Desk::OnUserReconnect pos5 %d", pUser->GetUserDataId());
		//我出牌
		if (m_playerIsTing[pos] == 1)
		{
			LMsgS2CCanTing send;
			std::map<Lint, LastCardSet>::iterator IT = m_setNowTingCard[pos].begin();
			while (IT != m_setNowTingCard[pos].end())
			{
				send.m_tingCards.push_back(IT->first);
				++IT;
			}
			pUser->Send(send);
		} 
		LLOG_DEBUG("Desk::OnUserReconnect pos6 %d", pUser->GetUserDataId());
		//桌面上的牌重新发给玩家的桌牌
		if (m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD && !m_isThinkingQiangGang)
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
		LLOG_DEBUG("Desk::OnUserReconnect pos8== %d", pUser->GetUserDataId());
	}

	void CheckStartPlayCard()
	{
		if (!m_desk)
		{
			return;
		}
		//不用抓牌了，直接思考		//有BUG 思考时手里多张牌
		m_desk->setDeskPlayState(DESK_PLAY_GET_CARD);
		//SetPlayIng(m_curPos, false, false, true, true, false, true);
		//开牌就检查是否可以听
		SetPlayIng(m_curPos, true, true, true, true, false, true);
		//m_curGetCard = m_handCard[m_curPos].back();
		m_needGetCard = true;
	}

	//摸牌
	void SetPlayIng(Lint pos, bool needGetCard, bool gang, bool needThink, bool canhu, bool bu = false, bool first_think = false)
	{
		if (m_desk == NULL)
		{
			LLOG_DEBUG("HanderUserEndSelect NULL ERROR ");
			return;
		}
		//穿庄
		if (!gang && m_deskCard.size() <= 16 && needGetCard)
		{
			LLOG_DEBUG("Desk::SetPlayIng huangzhuang game over");
			OnGameOver(WIN_NONE, m_playerHuInfo, INVAILD_POS, NULL);
			return;
		}
		m_curPos = pos;
		m_desk->UpdateLastOptTimeAndPos(m_curPos);

		//我摸牌思考信息
		m_thinkInfo[pos].m_thinkData.clear();
		if (m_desk->getDeskPlayState() != DESK_PLAY_END_CARD)
		{
			m_desk->setDeskPlayState(DESK_PLAY_GET_CARD);
		}
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

		if (needThink) //|| m_desk->getDeskPlayState() == DESK_PLAY_END_CARD
		{
			if (m_playerIsTing[m_curPos] != 2)
			{
				canhu = false;
			}
				
			mGameInfo.m_GameType = m_desk->getGameType();	// 0 湖南， 3， 长沙
			mGameInfo.b_canEat = false;		// 是否可以吃
			mGameInfo.b_canHu = canhu;		// 是否可以胡
			mGameInfo.b_onlyHu = (m_desk->getDeskPlayState() == DESK_PLAY_END_CARD || (m_playerIsTing[m_curPos] == 2));
			mGameInfo.m_thinkGang = gang;	// 单独处理是不是杠的牌
			mGameInfo.m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
			mGameInfo.m_playerPos = m_curPos;	// 当前一个出牌位置
			mGameInfo.m_cardState = THINK_OPERATOR_NULL;	// 当前一个出牌状态
			mGameInfo.m_MePos = pos;		// 玩家的位置
			mGameInfo.m_QiXiaoDui = false;
			mGameInfo.m_pt_laizi = false;
			mGameInfo.m_tianhu = first_think;
			mGameInfo.m_dihu = false;
			mGameInfo.b_menqing = m_playtype.H_MENQING;
			mGameInfo.b_menqingjiangjianghu = m_playtype.H_MENQINGPENGPENGHU;

			if (canhu && m_curGetCard)
			{
				if (m_curGetCard->m_color <= 3)
				{
					mGameInfo.b_canHu = (m_curGetCard->m_number >= 4);
				}
			}
			m_thinkInfo[pos].m_thinkData = gCardMgr.CheckGetCardOperator(m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_curGetCard, mGameInfo);
			VideoThink(pos);
		}

		if (m_needGetCard)
		{
			m_handCard[pos].push_back(m_curGetCard);
			gCardMgr.SortCard(m_handCard[pos]);
		}

		if (!m_playerIsTing[m_curPos])
		{
			CheckTing(pos, m_setNowTingCard[pos]);
		}

		for (Lint i = 0; i < m_desk->m_desk_user_count;  ++i)
		{
			if (m_desk->m_user[i] != NULL)
			{
				LMsgS2COutCard msg;
				msg.m_time = 15;
				msg.m_pos = pos;
				m_desk->UpdateLastOptTimeAndPos(pos);
				msg.m_deskCard = (Lint)m_deskCard.size();
				msg.m_gang = m_gangPos[pos];
				msg.m_end = m_desk->getDeskPlayState() == DESK_PLAY_END_CARD ? 1 : 0;
				msg.m_flag = 1;
				if (m_needGetCard)
				{
					msg.m_flag = 0;
				}

				if (pos == i)
				{
					if (m_needGetCard)
					{
						msg.m_curCard.m_number = m_curGetCard->m_number;
						msg.m_curCard.m_color = m_curGetCard->m_color;
					}

					for (Lsize j = 0; j < m_thinkInfo[pos].m_thinkData.size(); ++j)
					{
						ThinkData info;
						info.m_type = m_thinkInfo[pos].m_thinkData[j].m_type;
						if (first_think&&info.m_type == THINK_OPERATOR_BOMB && !(m_handCard[pos].empty()))
						{
							if (m_handCard[pos].back())
							{
								m_thinkInfo[pos].m_thinkData[j].m_card.push_back(m_handCard[pos].back());
								msg.m_curCard.m_number = m_handCard[pos].back()->m_number;
								msg.m_curCard.m_color = m_handCard[pos].back()->m_color;
							}
						}
						for (Lsize n = 0; n < m_thinkInfo[pos].m_thinkData[j].m_card.size(); ++n)
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

	bool calcScore(Lint result, Lint winpos[], Lint bombpos, Lint gold[], Lint& bombCount, std::vector<Lint>& winPos, LMsgS2CGameOver& over)
	{
		if (!m_desk)
		{
			return false;
		}
		if (result == WIN_BOMB && (bombpos<0 || bombpos>m_desk->m_desk_user_count - 1))
		{
			return false;
		}
		Lint baseScore[4] = { 0 };
		Lint tmppos = 0;
		if (m_curPos != INVAILD_POS)
		{
			tmppos = m_curPos;
		}

		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			if (result == WIN_ZIMO)
			{
				if (winpos[tmppos] == WIN_SUB_ZIMO)
				{
					winPos.push_back(tmppos);
					break;
				}
			}
			else if (result == WIN_BOMB)
			{
				if (winpos[tmppos] == WIN_SUB_BOMB)
				{
					winPos.push_back(tmppos);
					break;
				}
			}
			tmppos = m_desk->GetNextPos(tmppos);
		}

		if (winPos.size() == 0)
		{
			LLOG_ERROR("calcScore winPos.size() = %d", winPos.size());
			return false;
		}

		Lint finalWinPos = winPos.front();
		for (Lint i = 0; i < m_desk->GetUserCount(); ++i)
		{
			if (i != finalWinPos)
			{
				winpos[i] = 0;
				m_thinkRet[i].m_hu.clear();
			}
		}

		if (result == WIN_ZIMO)
		{
			if (!m_curGetCard)
			{
				LLOG_ERROR("calcScore m_curGetCard null");
				return false;
			}
			if (m_curGetCard)
			{
				LLOG_DEBUG("calcScore2 m_curGetCard m_color  %d m_number %d", m_curGetCard->m_color, m_curGetCard->m_number);
				for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
				{
					if (i != finalWinPos)
					{
						if (m_curGetCard->m_color > 3)
						{
							gold[finalWinPos] += 10 * 2;
							gold[i] += -(10 * 2);
						}
						else
						{
							gold[finalWinPos] += m_curGetCard->m_number * 2;
							gold[i] += -(m_curGetCard->m_number * 2);
						}
					}
				}
			}

			//杠随胡
			for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
			{
				gold[i] += (m_minggangCard[i].size()/4 * 5) * 3;
				gold[i] += (m_angang[i] * 10) * 3;
				LLOG_DEBUG("calcScore m_diangang %d m_angang %d m_minggang %d", m_diangang[i], m_angang[i], m_minggang[i]);
				for (Lint j = 0; j < m_desk->m_desk_user_count; ++j)
				{
					if (i != j)
					{
						gold[j] -= (m_minggangCard[i].size()/4 * 5);
						gold[j] -= (m_angang[i] * 10);
					}
				}
			}
		}

		if(result == WIN_BOMB)
		{
			if (!m_curOutCard)
			{
				LLOG_ERROR("calcScore m_curOutCard null");
				return false;
			}
			if (m_isAutoCard[bombpos] && !m_isTingHu[bombpos]) //如果已经自动打牌，和自摸一样，区别就是点炮多少分就多少分
			{
				LLOG_DEBUG("calcScore2 m_curOutCard m_color  %d m_number %d", m_curOutCard->m_color, m_curOutCard->m_number);
				for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
				{
					if (i != finalWinPos)
					{
						if (m_curOutCard->m_color > 3)
						{
							gold[finalWinPos] += 10;
							gold[i] += -(10);
						}
						else
						{
							gold[finalWinPos] += m_curOutCard->m_number;
							gold[i] += -(m_curOutCard->m_number);
						}
					}
				}

				//杠随胡
				for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
				{
					gold[i] += (m_minggangCard[i].size() / 4 * 5) * 3;
					gold[i] += (m_angang[i] * 10) * 3;
					LLOG_DEBUG("calcScore m_diangang %d m_angang %d m_minggang %d", m_diangang[i], m_angang[i], m_minggang[i]);
					for (Lint j = 0; j < m_desk->m_desk_user_count; ++j)
					{
						if (i != j)
						{
							gold[j] -= (m_minggangCard[i].size() / 4 * 5);
							gold[j] -= (m_angang[i] * 10);
						}
					}
				}
			}
			else
			{
				{
					LLOG_DEBUG("calcScore2 m_curOutCard m_color  %d m_number %d", m_curOutCard->m_color, m_curOutCard->m_number);
					if (m_curOutCard->m_color > 3)
					{
						gold[finalWinPos] += 10 * 4;
						gold[bombpos] += -(10 * 4);
					}
					else
					{
						gold[finalWinPos] += m_curOutCard->m_number * 4;
						gold[bombpos] += -(m_curOutCard->m_number * 4);
					}
				}

				//杠随胡
				for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
				{
					if (i != bombpos)
					{
						gold[i] += (m_minggangCard[i].size() / 4 * 5) * 4;
						gold[i] += (m_angang[i] * 10) * 4;
						gold[bombpos] -= (m_minggangCard[i].size() / 4 * 5) * 4;
						gold[bombpos] -= (m_angang[i] * 10) * 4;
						LLOG_DEBUG("calcScore2 m_diangang %d m_angang %d m_minggang %d", m_diangang[i], m_angang[i], m_minggang[i]);
					}
				}
			}
		}

		LLOG_DEBUG("HUPai: (1: %d) (2: %d) (3: %d) (4: %d)", gold[0], gold[1], gold[2], gold[3]);
		return true;
	}

	//获得一张卡后检查报听
	bool CheckTing(Lint pos, std::map<Lint, LastCardSet>& lastCardMap)
	{
		CardVector handcard = m_handCard[pos];
		lastCardMap.clear();

		Lint nPreCardIdx = 0;       //前一张牌的花色与值
		bool bRet = false;   //返回值

		bool getCardFirst = true; //第一张与摸到的牌相同，需要忽略

		gCardMgr.SortCard(handcard);

		for (Lsize nHandCardIdx = 0; nHandCardIdx < handcard.size(); ++nHandCardIdx)
		{
			Lint nDelCardIdx = handcard[nHandCardIdx]->GetNCIndex();   //得到将要剔除的那张牌
			Card * checkCd = handcard[nHandCardIdx];
			if (nPreCardIdx == nDelCardIdx)
			{
				continue;  //与前一张牌相同，不需要再检查听牌
			}

			nPreCardIdx = nDelCardIdx;
			CardVector tmpHandCard(handcard);   //构造临时的一手牌
												//剔除一张牌，再看看剩下的牌是否可以听牌
			CardVector::iterator IT = tmpHandCard.begin();
			while (IT != tmpHandCard.end())
			{
				if (*IT == tmpHandCard[nHandCardIdx])
				{
					tmpHandCard.erase(IT);
					break;
				}
				++IT;
			}

			//如果能听平胡
			std::vector<Card> hu_cards;
			if (gCardMgr.XiaoYi_CheckTing_GetHuCards(hu_cards, tmpHandCard, m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos]))
			{
				LastCardSet tmpset;
				auto iter = hu_cards.begin();
				while (iter != hu_cards.end())
				{
					LastCard lastcard;
					lastcard.m_nCardNC = (*iter).GetNCIndex();
					tmpset.insert(lastcard);
					++iter;
				}
				lastCardMap[nDelCardIdx] = tmpset;
				bRet = true;
			}
		}

		if (!bRet)
		{
			return false;
		}

		ThinkUnit unit;
		unit.m_type = THINK_OPERATOR_TING;
		m_thinkInfo[pos].m_thinkData.push_back(unit);
		VideoThink(pos);
		return true;
	}

	void SetThinkIng(bool is_think_qianggang = false, bool first_think = false)
	{
		if (!m_desk)
			return;
		bool think = false;
		if (is_think_qianggang && m_curOutCard)
		{
			LLOG_DEBUG("YiYangMajiang setthinking isqinaggang = true, outcard:%d", m_curOutCard->m_color * 10 + m_curOutCard->m_number);
		}
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			m_thinkRet[i].Clear();
			m_thinkInfo[i].Reset();
			if (i != m_curPos)
			{
				mGameInfo.m_GameType = m_desk->getGameType();	// 0 湖南， 3， 长沙
				mGameInfo.b_canEat = false;		// 是否可以吃
				mGameInfo.b_canHu = (m_playerIsTing[i] == 2);
				mGameInfo.b_onlyHu = (m_beforeType == THINK_OPERATOR_MGANG || m_beforeType == THINK_OPERATOR_MBU || (m_playerIsTing[i] == 2));
				mGameInfo.b_guoGang = false;
				mGameInfo.m_thinkGang = false;	// 单独处理是不是杠出来的牌
				mGameInfo.m_deskState = m_desk->getDeskPlayState();	// 当前局牌状态
				mGameInfo.m_playerPos = m_curPos;	// 当前一个出牌位置
				mGameInfo.m_cardState = m_beforeType;	// 当前一个出牌状态
				mGameInfo.m_MePos = i;		// 玩家的位置
				mGameInfo.m_QiXiaoDui = false;
				mGameInfo.m_pt_laizi = false;
				mGameInfo.m_tianhu = false;
				mGameInfo.m_dihu = first_think;
				mGameInfo.b_menqing = m_playtype.H_MENQING;
				mGameInfo.b_menqingjiangjianghu = m_playtype.H_MENQINGPENGPENGHU;

				if (mGameInfo.b_canHu && m_curOutCard)
				{
					if (m_curOutCard->m_color <= 3)
					{
						mGameInfo.b_canHu = (m_curOutCard->m_number >= 5);
					}
				}

				m_thinkInfo[i].m_thinkData = gCardMgr.CheckOutCardOperator(m_handCard[i], m_pengCard[i], m_angangCard[i], m_minggangCard[i], m_eatCard[i], m_curOutCard, mGameInfo);
				if (m_thinkInfo[i].NeedThink())
				{
					think = true;
					VideoThink(i);
				}
			}
		}

		if (think)
		{
			if (is_think_qianggang)
			{
				m_isThinkingQiangGang = true;
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
						for (Lsize n = 0; n < m_thinkInfo[i].m_thinkData[j].m_card.size(); ++n)
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

	Lint CheckPositionPrior(Lint CurPos, Lint first_pos, Lint second_pos)
	{
		if (!m_desk)
			return first_pos;
		Lint dif1 = first_pos - CurPos >= 0 ? first_pos - CurPos : first_pos - CurPos + m_desk->m_desk_user_count;
		Lint dif2 = second_pos - CurPos >= 0 ? second_pos - CurPos : second_pos - CurPos + m_desk->m_desk_user_count;
		if (dif1<dif2)
			return first_pos;
		else
			return second_pos;
	}

	void CheckThink()
	{
		if (!m_desk)
			return;
		{
			Lint hu = -1;
			Lint Peng = -1;
			Lint Chi = -1;
			Lint Gang = -1;
			Lint Bu = -1;
			Lint hu_New = -1;
			Lint Peng_New = -1;
			Lint Chi_New = -1;
			Lint Gang_New = -1;
			Lint Bu_New = -1;
			for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
			{
				int pos = i;
				if (m_beforePos != INVAILD_POS)
				{
					pos = (m_beforePos - i) < 0 ? (m_beforePos - i + m_desk->m_desk_user_count) : m_beforePos - i;
				}

				if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB)		hu = pos;
				if (m_thinkRet[pos].m_type == THINK_OPERATOR_PENG)		Peng = pos;
				if (m_thinkRet[pos].m_type == THINK_OPERATOR_CHI)		Chi = pos;
				if (m_thinkRet[pos].m_type == THINK_OPERATOR_MGANG)	    Gang = pos;
				if (m_thinkRet[pos].m_type == THINK_OPERATOR_MBU)		Bu = pos;

				if (m_thinkInfo[pos].NeedThink())
				{
					if (m_thinkInfo[pos].HasHu())			hu_New = pos;
					if (m_thinkInfo[pos].HasMGang())		Gang_New = pos;
					if (m_thinkInfo[pos].HasMBu())			Bu_New = pos;
					if (m_thinkInfo[pos].HasPeng())			Peng_New = pos;
					if (m_thinkInfo[pos].HasChi())			Chi_New = pos;
				}
			}

			bool think = false;

			if (hu_New != -1)
				think = true;
			else
			{
				if (hu == -1)
				{
					if (Gang != -1)
					{
						if (Gang_New != -1)
						{
							if (Gang_New == CheckPositionPrior(m_beforePos, Gang, Gang_New))
							{
								think = true;
							}
						}
					}
					else if (Bu != -1)
					{
						if (Gang_New != -1)
						{
							think = true;
						}
					}
					else if (Peng != -1)
					{
						if (Gang_New != -1 || Bu_New != -1)
						{
							think = true;
						}
						else if (Peng_New != -1)
						{
							if (Peng_New == CheckPositionPrior(m_beforePos, Peng, Peng_New))
							{
								think = true;
							}
						}
					}
					else if (Chi != -1)
					{
						if (Gang_New != -1 || Bu_New != -1 || Peng_New != -1)
						{
							think = true;
						}
						else if (Chi_New != -1)
							think = true;
					}
					else {
						if (Gang_New != -1 || Bu_New != -1 || Peng_New != -1 || Chi_New != -1)
						{
							think = true;
						}
					}
				}
			}

			if (!think)
				ThinkEnd();
		}
	}

	void ThinkEnd()
	{
		if (!m_desk)
			return;
		for (int i = 0; i < m_desk->m_desk_user_count; i++)
		{
			if (m_thinkInfo[i].NeedThink())
			{
				VideoDoing(99, i, 0, 0);
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
			int pos = i;
			if (m_beforePos != INVAILD_POS)
			{
				pos = (m_beforePos - i)<0 ? (m_beforePos - i + m_desk->m_desk_user_count) : m_beforePos - i;
			}

			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB)
			{
				m_playerHuInfo[pos] = WIN_SUB_BOMB;
				m_playerBombInfo[pos] = WIN_SUB_ABOMB;
				huCount++;
			}
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_MGANG)
				gangPos = pos;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_MBU)
				buPos = pos;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_PENG)
				pengPos = pos;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_CHI)
				chiPos = pos;
		}

		if (huCount != 0)
		{
			CardVector winCards[DESK_USER_COUNT];
			for (int i = 0; i < m_desk->m_desk_user_count; i++)
			{
				if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
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
			if (m_playerIsTing[m_beforePos] == 2)
			{
				if (m_curOutCard && m_curOutCard->GetNCIndex() == m_tingCard[m_beforePos].m_ncCard)
				{
					m_tingCard[m_beforePos].setType(2);
					m_tingCard[m_beforePos].resetOutIndex();
					_S_BrocastTingInfo(m_beforePos);
				}
			}

			LMsgS2CUserOper send;
			send.m_pos = gangPos;
			send.m_errorCode = 0;
			send.m_think.m_type = m_thinkRet[gangPos].m_type;
			for (Lsize i = 0; i < m_thinkRet[gangPos].m_card.size(); ++i)
			{
				CardValue v;
				if (m_thinkRet[gangPos].m_card[i])
				{
					v.m_color = m_thinkRet[gangPos].m_card[i]->m_color;
					v.m_number = m_thinkRet[gangPos].m_card[i]->m_number;
					send.m_think.m_card.push_back(v);
				}
			}
			if (m_curOutCard)
			{
				send.m_card.m_color = m_curOutCard->m_color;
				send.m_card.m_number = m_curOutCard->m_number;

				m_desk->BoadCast(send);
				gCardMgr.EraseCard(m_handCard[gangPos], m_thinkRet[gangPos].m_card[0], 3);

				//录像
				std::vector<CardValue> cards;
				for (int i = 0; i < 4; i++)
				{
					CardValue card;
					card.m_color = m_curOutCard->m_color;
					card.m_number = m_curOutCard->m_number;
					cards.push_back(card);
					m_minggangCard[gangPos].push_back(m_curOutCard);
				}
				m_video.AddOper(VIDEO_OPER_GANG, gangPos, cards);

				m_diangang[gangPos] += 1;
				m_adiangang[m_beforePos] += 1;

				for (int i = 0; i < m_desk->m_desk_user_count; i++)
				{
					m_thinkRet[i].Clear();
				}
				m_beforePos = gangPos;
				m_beforeType = THINK_OPERATOR_MGANG;

				//给玩家摸一张牌
				SetPlayIng(gangPos, true, true, true, true);
				//SetThinkIng(true);
				return;
			}
		}

		//补
		if (buPos != INVAILD_POS)
		{
			if (m_playerIsTing[m_beforePos] == 2)
			{
				if (m_curOutCard && m_curOutCard->GetNCIndex() == m_tingCard[m_beforePos].m_ncCard)
				{
					m_tingCard[m_beforePos].setType(2);
					m_tingCard[m_beforePos].resetOutIndex();
					_S_BrocastTingInfo(m_beforePos);
				}
			}

			LMsgS2CUserOper send;
			send.m_pos = buPos;
			send.m_errorCode = 0;
			send.m_think.m_type = m_thinkRet[buPos].m_type;
			for (Lsize i = 0; i < m_thinkRet[buPos].m_card.size(); ++i)
			{
				CardValue v;
				v.m_color = m_thinkRet[buPos].m_card[i]->m_color;
				v.m_number = m_thinkRet[buPos].m_card[i]->m_number;
				send.m_think.m_card.push_back(v);
			}

			if (m_curOutCard)
			{
				send.m_card.m_color = m_curOutCard->m_color;
				send.m_card.m_number = m_curOutCard->m_number;

				m_desk->BoadCast(send);
				gCardMgr.EraseCard(m_handCard[buPos], m_thinkRet[buPos].m_card[0], 3);

				//把对方桌上的最后一张出的牌去掉
				gCardMgr.EraseCard(m_outCard[m_beforePos], m_curOutCard);

				//录像
				std::vector<CardValue> cards;
				for (int i = 0; i < 4; i++)
				{
					CardValue card;
					card.m_color = m_curOutCard->m_color;
					card.m_number = m_curOutCard->m_number;
					cards.push_back(card);
					m_minggangCard[buPos].push_back(m_curOutCard);
				}
				m_video.AddOper(VIDEO_OPER_OTHER_BU, buPos, cards);


				m_diangang[buPos] += 1;
				m_adiangang[m_beforePos] += 1;

				for (int i = 0; i < m_desk->m_desk_user_count; i++)
				{
					m_thinkRet[i].Clear();
				}
				m_beforePos = buPos;
				m_beforeType = THINK_OPERATOR_MBU;
				//给玩家摸一张牌
				SetPlayIng(buPos, true, true, true, true, true);
				//SetThinkIng(true);
				return;
			}
		}

		if (pengPos != INVAILD_POS)
		{
			if (m_playerIsTing[m_beforePos] == 2)
			{
				if (m_curOutCard && m_curOutCard->GetNCIndex() == m_tingCard[m_beforePos].m_ncCard && m_outCard[m_beforePos].size() == m_tingCard[m_beforePos].GetOutIndex())
				{
					m_tingCard[m_beforePos].setType(1);
					m_tingCard[m_beforePos].resetOutIndex();
					_S_BrocastTingInfo(m_beforePos);
				}
			}

			LMsgS2CUserOper send;
			send.m_pos = pengPos;
			send.m_errorCode = 0;
			send.m_think.m_type = m_thinkRet[pengPos].m_type;
			for (Lsize i = 0; i < m_thinkRet[pengPos].m_card.size(); ++i)
			{
				CardValue v;
				v.m_color = m_thinkRet[pengPos].m_card[i]->m_color;
				v.m_number = m_thinkRet[pengPos].m_card[i]->m_number;
				send.m_think.m_card.push_back(v);
			}

			if (m_curOutCard)
			{
				send.m_card.m_color = m_curOutCard->m_color;
				send.m_card.m_number = m_curOutCard->m_number;

				m_desk->BoadCast(send);
				gCardMgr.EraseCard(m_handCard[pengPos], m_curOutCard, 2);

				//把对方桌上的最后一张出的牌去掉
				gCardMgr.EraseCard(m_outCard[m_beforePos], m_curOutCard);

				//录像
				std::vector<CardValue> cards;
				for (int i = 0; i < 3; i++)
				{
					CardValue card;
					card.m_color = m_curOutCard->m_color;
					card.m_number = m_curOutCard->m_number;
					cards.push_back(card);
					m_pengCard[pengPos].push_back(m_curOutCard);
				}
				m_video.AddOper(VIDEO_OPER_PENG_CARD, pengPos, cards);

				for (int i = 0; i < m_desk->m_desk_user_count; i++)
				{
					m_thinkRet[i].Clear();
				}
				//碰完打一张牌
				m_curGetCard = NULL;
				SetPlayIng(pengPos, false, false, true, false);
				m_needGetCard = true;
				return;
			}
		}

		//吃
		if (chiPos != INVAILD_POS)
		{
			LMsgS2CUserOper send;
			send.m_pos = chiPos;
			send.m_errorCode = 0;
			send.m_think.m_type = m_thinkRet[chiPos].m_type;
			for (Lsize i = 0; i < m_thinkRet[chiPos].m_card.size(); ++i)
			{
				CardValue v;
				v.m_color = m_thinkRet[chiPos].m_card[i]->m_color;
				v.m_number = m_thinkRet[chiPos].m_card[i]->m_number;
				send.m_think.m_card.push_back(v);
			}

			if (m_curOutCard)
			{
				send.m_card.m_color = m_curOutCard->m_color;
				send.m_card.m_number = m_curOutCard->m_number;

				m_desk->BoadCast(send);
				if (m_thinkRet[chiPos].m_card.size() > 2 && m_thinkRet[chiPos].m_card[2] && m_thinkRet[chiPos].m_card[0] && m_thinkRet[chiPos].m_card[1])
				{
					gCardMgr.EraseCard(m_handCard[chiPos], m_thinkRet[chiPos].m_card[0]);
					gCardMgr.EraseCard(m_handCard[chiPos], m_thinkRet[chiPos].m_card[1]);

					//把对方桌上的最后一张出的牌去掉
					gCardMgr.EraseCard(m_outCard[m_beforePos], m_curOutCard);

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

					for (int i = 0; i < m_desk->m_desk_user_count; i++)
					{
						m_thinkRet[i].Clear();
					}
					//给玩家摸一张牌
					m_curGetCard = NULL;
					SetPlayIng(chiPos, false, false, true, false);
					m_needGetCard = true;
					return;
				}
			}
		}

		//这里没有人操作
		if (m_beforeType == THINK_OPERATOR_MGANG || m_beforeType == THINK_OPERATOR_MBU)
		{
			if (m_isThinkingQiangGang)
			{
				m_isThinkingQiangGang = false;
			}
			//录像
			std::vector<CardValue> cards;
			for (int i = 0; i < 4; i++)
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
			m_minggangCard[m_beforePos].push_back(m_curOutCard);
			m_minggang[m_beforePos]++;
			m_adiangang[m_beforePos] += 1;
			m_diangangRelation[m_beforePos][gangPos]++;
			CardVector::iterator it = m_pengCard[m_beforePos].begin();
			for (; it != m_pengCard[m_beforePos].end(); it += 3)
			{
				if (gCardMgr.IsSame(m_curOutCard, *it))
				{
					m_minggangCard[m_beforePos].insert(m_minggangCard[m_beforePos].end(), it, it + 3);
					m_pengCard[m_beforePos].erase(it, it + 3);
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

			SetPlayIng(m_beforePos, true, true, true, true);
		}
		else
		{
			/*if (DESK_PLAY_BAO_TING != m_desk->getDeskPlayState())
			{
				if (m_curOutCard != NULL)
					m_outCard[m_beforePos].push_back(m_curOutCard);
			}*/
			SetPlayIng(m_desk->GetNextPos(m_beforePos), true, false, true, true);
		}
	}

	void OnGameOver(Lint result, Lint winpos[], Lint bombpos, CardVector winCards[])
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_DEBUG("OnGameOver NULL ERROR ");
			return;
		}

		//计算输赢结果
		Lint gold[4] = { 0 };
		Lint bombCount = 0;
		std::vector<Lint> winPos;
		//广播结果
		LMsgS2CGameOver over;
		over.m_user_count = m_desk->m_desk_user_count;

		//计算分数
		bool checkScore = calcScore(result, winpos, bombpos, gold, bombCount, winPos, over);

		Lint zhuangPos = m_zhuangpos;
		//计算庄
		if (result == WIN_ZIMO)
		{
			if (winCards)
			{
				if (!winPos.empty())
				{
					if (winPos.front()>-1 && winPos.front()< m_desk->m_desk_user_count)
					{
						m_zhuangpos = winPos.front();
						//录像
						std::vector<CardValue> cards;
						for (Lint i = 0; i < winCards[m_zhuangpos].size(); i++)
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
			if (!winPos.empty())
			{
				m_zhuangpos = winPos.front();
			}

			//录像
			std::vector<CardValue> cards;
			if (winCards)
			{
				for (auto x = winPos.begin(); x != winPos.end(); x++)
				{
					if (*x>-1 && *x< m_desk->m_desk_user_count)
					{
						for (Lint i = 0; i < winCards[*x].size(); i++)
						{
							if (winCards[*x][i])
							{
								CardValue curOutCard;
								curOutCard.m_color = winCards[*x][i]->m_color;
								curOutCard.m_number = winCards[*x][i]->m_number;
								cards.push_back(curOutCard);
							}
						}
						m_video.AddOper(VIDEO_OPER_SHOUPAO, *x, cards);
					}
				}

			}

			//如果点炮，已经自动打牌，那么情况，不显示点炮
			if (m_isAutoCard[m_beforePos] && !m_isTingHu[m_beforePos])
			{
				for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
				{
					winpos[i] = 0;
					m_playerBombInfo[i] = 0;
				}
				winpos[m_zhuangpos] = WIN_SUB_ABOMB2;		//置点炮位
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
		memcpy(over.m_cs_win, winpos, sizeof(over.m_cs_win));
		memcpy(over.m_score, gold, sizeof(over.m_score));
		memcpy(over.m_agang, m_angang, sizeof(over.m_agang));
		memcpy(over.m_mgang, m_minggang, sizeof(over.m_mgang));
		memcpy(over.m_dmgang, m_diangang, sizeof(over.m_dmgang));
		memcpy(over.m_dgang, m_adiangang, sizeof(over.m_dgang));

		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			Lsize oversz = m_handCard[i].size();
			over.m_count[i] = oversz;

			//手牌
			Lsize cardPos = 0;
			for (Lint j = 0; j < oversz; ++j)
			{
				over.m_card[i][cardPos].m_color = m_handCard[i][j]->m_color;
				over.m_card[i][cardPos++].m_number = m_handCard[i][j]->m_number;
			}

			//胡牌类型
			if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
			{
				over.m_hu[i] = m_thinkRet[i].m_hu;
			}
		}
		if (winCards)
		{
			for (auto x = winPos.begin(); x != winPos.end(); x++)
			{
				if (*x>-1 && *x<m_desk->m_desk_user_count)
				{
					for (Lint i = 0; i < winCards[*x].size(); i++)
					{
						CardValue curGetCard;
						curGetCard.m_color = winCards[*x][i]->m_color;
						curGetCard.m_number = winCards[*x][i]->m_number;
						over.m_hucards[*x].push_back(curGetCard);
						LLOG_DEBUG("GameHandler_HengYang onGameOver pos:%d, card:%d", *x, winCards[*x][i]->m_color * 10 + winCards[*x][i]->m_number);
					}
				}
			}
		}
		m_desk->SetDeskWait();
		Lint mgang[4] = { 0 };
		for (Lint i = 0; i < m_desk->m_desk_user_count; ++i)
		{
			mgang[i] += m_minggang[i];
			mgang[i] += m_diangang[i];
		}

		//保存结果	
		Ldouble cal_gold[4] = { 0,0,0,0 };

		//保存结果	
		m_desk->m_vip->AddLog(m_desk->m_user, gold, cal_gold, winpos, zhuangPos, m_angang, mgang, m_playerBombInfo, m_video.m_Id, m_video.m_time);

		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		m_video.SetEveryResult(4, gold[0], gold[1], gold[2], gold[3]);
		m_video.SetEveryResult2(4, cal_gold[0], cal_gold[1], cal_gold[2], cal_gold[3]);
		video.m_sql = m_video.GetInsertSql();
		gWork.SendMsgToDb(video);

		//是否最后一局
		over.m_end = m_desk->m_vip->isEnd() ? 1 : 0;
		m_desk->BoadCast(over);

		m_desk->HanderGameOver();

		//////////////////////////////////////////////////////////////////////////
	}

	void VideoThink(Lint pos)
	{
		if (m_thinkInfo[pos].m_thinkData.size() >0)
		{
			std::vector<CardValue> cards;
			for (auto itr = m_thinkInfo[pos].m_thinkData.begin(); itr != m_thinkInfo[pos].m_thinkData.end(); itr++)
			{
				CardValue card;
				card.m_number = itr->m_type;
				if (itr->m_card.size()>0)
					card.m_color = itr->m_card[0]->m_color * 10 + itr->m_card[0]->m_number;
				if (itr->m_card.size()>1)
					card.m_color = card.m_color * 1000 + itr->m_card[1]->m_color * 10 + itr->m_card[1]->m_number;
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
		card.m_color = card_color * 10 + card_number;
		cards.push_back(card);
		m_video.AddOper(VIDEO_OPEN_DOING, pos, cards);

	}

	void	SetPlayType(std::vector<Lint>& l_playtype) {
		m_playtype.init_playtype_info(l_playtype);
		m_desk->m_desk_user_count = 4;
		//m_playtype.H_ANTING = true;
	}
	//////////////////////////////////////////////////////////////////////////
private:
	Desk			*m_desk;
	Card*			m_curOutCard;//当前出出来的牌
	Card*			m_curGetCard;//当前获取的牌

	Lint			m_playerIsTing[DESK_USER_COUNT];	    //记录玩家是否已经听  0 = 无听 1 = 进入听牌状态未出牌 2 = 听牌状态之后(只能出摸到的牌)
	std::map<Lint, LastCardSet>	m_setNowTingCard[DESK_USER_COUNT];
	bool            m_isAutoCard[DESK_USER_COUNT];//自动打牌

	ThinkTool		m_thinkInfo[DESK_USER_COUNT];//当前打牌思考状态
	ThinkUnit		m_thinkRet[DESK_USER_COUNT];//玩家返回思考结果

	CardVector		m_handCard[DESK_USER_COUNT];//玩家手上的牌

	CardVector		m_outCard[DESK_USER_COUNT];	//玩家出的牌

	CardVector		m_pengCard[DESK_USER_COUNT];//玩家碰的牌，

	CardVector		m_minggangCard[DESK_USER_COUNT];//玩家明杠的牌

	CardVector		m_angangCard[DESK_USER_COUNT];//玩家暗杠的牌
	CardVector		m_eatCard[DESK_USER_COUNT];//玩家吃的牌
	LiangPai        m_tingCard[DESK_USER_COUNT];//玩家听的牌

	Lint			m_angang[DESK_USER_COUNT];//暗杠数量
	Lint			m_minggang[DESK_USER_COUNT];//明杠数量
	Lint			m_diangang[DESK_USER_COUNT];//点杠数量
	Lint			m_adiangang[DESK_USER_COUNT];//被点杠数量
	Lint			m_diangangRelation[DESK_USER_COUNT][DESK_USER_COUNT];//A给B点杠的次数

	Lint			m_gangPos[DESK_USER_COUNT];	//某位玩家有过杠

	Lint			m_playerHuInfo[DESK_USER_COUNT];		//玩家胡牌信息， 因为可以多胡 放炮的人只保存了位置
	Lint			m_playerBombInfo[DESK_USER_COUNT];	//玩家放炮信息，[玩家位置对应的炮的类型]


	Lint			m_beforePos;//之前操作的位置
	Lint			m_beforeType;//之前操作的类型

	Lint			m_gold;

	Lint			m_zhuangpos;//庄家位置
	Lint			m_curPos;						//当前操作玩家

	bool			m_needGetCard;

	bool			m_isThinkingQiangGang;				//当前是否在思考抢杠
	CardVector		m_deskCard;		//桌子上剩余的牌
	CardVector		m_curOutCards;	//当前出出来的牌		(为杠写的)
	Lint			m_haidi_zhuangpos;
	VideoLog		m_video;						//录像
	bool			m_first_turn;
	OperateState	mGameInfo;
	PlayTypeInfo    m_playtype;
	bool            m_isTingHu[DESK_USER_COUNT];

};

DECLARE_GAME_HANDLER_CREATOR(XiaoYiMajiang, GameHandler_XiaoYi)