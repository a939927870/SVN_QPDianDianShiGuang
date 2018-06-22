#include "DDZHandler_2Player.h"
#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include <cmath>

bool GameHandler_DDZ_2Players::startup(Desk *desk)
{
	if (desk)
	{
		clearHandler();
		m_pDesk = desk;
		m_nCardCounts = 17;
	}
	return true;
}

void GameHandler_DDZ_2Players::clearHandler(void)
{
	_clearGame();
	m_firstPos = 0;   //需要保留信息到下一局，不能初始化;
	m_nDifen = 2; // 游戏底分默认为1;
}

//初始化每局游戏，与初始化桌子有区别，有信息需要保留;
void GameHandler_DDZ_2Players::_clearGame()
{
	m_curPos = 0;
	m_ticktime = gWork.GetCurTime().MSecs();
	for (Lint i = 0; i < USER_COUNT_2PERS; ++i)
	{
		m_bomb[i] = 0;
		m_handCard[i].clear();
		m_playerCardType[i] = 0;
		m_outCard[i].clear();
		m_baojing[i] = 0;
	}
	m_nQiangDizhuState = INVAILD_VAL;
	m_nDizhuPlayCounts = 0;
	m_nDiZhuPos = INVAILD_VAL;
	m_nDaocishu = 0;
	m_leftCard.clear();
	m_bombCount = 0;
	memset(m_baojing,0,sizeof(m_baojing)); //跑的快报单信息;
}

void GameHandler_DDZ_2Players::SetDeskDeal()
{
	if (m_pDesk && m_pDesk->m_vip)
		m_pDesk->m_vip->SendInfo();

	dealCard_new();
	Lint userId[USER_COUNT_2PERS] = {0};
	Ldouble score[USER_COUNT_2PERS]={0};
	std::vector<Lint> vec[USER_COUNT_2PERS];

	for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
	{
		if(m_pDesk->m_user[i])
			userId[i] = m_pDesk->m_user[i]->GetUserDataId();

		score[i] = m_pDesk->m_vip->GetUserScore(m_pDesk->m_user[i]);
		for(size_t j = 0 ; j < m_handCard[i].size(); ++j)
		{
			vec[i].push_back(m_handCard[i][j]->GetIndex());
		}
	}
	std::vector<Lint> vecGameType;
	vecGameType.push_back(game_para_maxfan);
	vecGameType.push_back(m_pDesk->m_nMaxFanshu);
	m_pDesk->m_video.DealCard(userId,vec,gWork.GetCurTime().Secs(),0,score,m_pDesk->m_id, m_pDesk->m_vip->m_curCircle, m_pDesk->m_vip->m_maxCircle, m_pDesk->m_gameStyle, vecGameType);
	m_curHelpInfo.clear();
	m_curHelpUnusable.clear();
	m_curPos = m_firstPos;

	m_pDesk->m_deskState = QIANG_DIZHU;
	LMsgS2CQDZQuery_Sichuan msg;
	msg.m_pos = m_curPos;
	msg.m_operType = OPER_MINGZHUA;
	m_nQiangDizhuState = OPER_MINGZHUA;
	BoadCast(msg);
}

void GameHandler_DDZ_2Players::CheckSameIP()
{
	if (m_pDesk->m_nUserCounts != 3)
	{
		return;
	}
	Lstring szUserIP[3];
	for (Lint userNum = 0; userNum < m_pDesk->m_nUserCounts; ++userNum)
	{
		szUserIP[userNum] = m_pDesk->m_user[userNum]->GetIp();
	}
	Lint sameIPPos0;
	Lint sameIPPos1;
	Lint sameTimes = 0;
	for (Lint ii = 0; ii < 3; ++ii)
	{
		for (Lint jj = ii + 1; jj < 3; ++jj)
		{
			if (szUserIP[ii] == szUserIP[jj])
			{
				++sameTimes;
				sameIPPos0 = ii;
				sameIPPos1 = jj;
			}
		}
	}
	if (1 == sameTimes)   // 三个IP均相等不用发消息;
	{
		LMsgS2CSameIP msg;
		msg.m_pos[0] = sameIPPos0;
		msg.m_pos[1] = sameIPPos1;
		msg.m_nike[0] = m_pDesk->m_user[sameIPPos0]->m_userData.m_nike;
		msg.m_nike[1] = m_pDesk->m_user[sameIPPos1]->m_userData.m_nike;
		BoadCast(msg);
	}
}

void GameHandler_DDZ_2Players::QiangDiZhu(Lint nPos)
{
	LMsgS2CAskDiZhu msg;
	msg.m_pos = nPos;
	msg.m_difen = 0;
	BoadCast(msg);
	return;
}

void GameHandler_DDZ_2Players::HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg)
{
	if (m_pDesk->m_deskState != QIANG_DIZHU)
	{
		LLOG_ERROR("2Player:not QiangDIzhu Time");
		return;
	}
	if (GetUserPos(pUser) != msg->m_pos)
	{
		LLOG_ERROR("user position is not msg' position: %d!", msg->m_pos );
		return;
	}
	if (m_curPos == msg->m_pos)
	{
		if (OPER_MINGZHUA == msg->m_operType && OPER_YES == msg->m_playerOper) //抓;
		{
			//通知其他人该玩家抢了地主;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_YES);
			//询问其他玩家抢不抢;
			if (m_curPos == m_firstPos)
			{
				m_curPos = GetNextPos(m_curPos);
				QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
			}
			else
			{
				m_nDiZhuPos = m_curPos;
				LMsgS2CWhoIsDiZhu dizhuposMsg;
				dizhuposMsg.m_pos = m_nDiZhuPos;
				for(size_t i = 0 ; i <m_leftCard.size(); ++i)
				{
					dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
					m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
				}
				CardStyle::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
				BoadCast(dizhuposMsg);
				m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);
				SetDeskPlay();
			}
		}
		else if (OPER_MINGZHUA == msg->m_operType && OPER_NO == msg->m_playerOper && (m_curPos == m_firstPos))//不叫;
		{
			//通知其他玩家该玩家不抓;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_NO);
			//询问下一个玩家是否抓;
			m_curPos = GetNextPos(m_curPos);
			QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_MINGZHUA);
		}
		else if (OPER_MINGZHUA == msg->m_operType && OPER_NO == msg->m_playerOper && (m_curPos != m_firstPos))//不叫;
		{
			//通知其他玩家该玩家不抓;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_NO, 1);
			//无人抢地主,重新发牌;
			dealCard_new();
			//重新发牌录像;
			std::vector<Lint> vec;
			for (Lint i = 0; i < m_pDesk->m_nUserCounts; ++i)
			{
				for (size_t j = 0; j < m_handCard[i].size(); ++j)
				{
					vec.push_back(m_handCard[i][j]->GetIndex());
				}
			}
			m_pDesk->m_video.AddOper(VIDEO_OPER_REDEALCARD, 0, vec);
			m_curPos = m_firstPos;
			LMsgS2CQDZQuery_Sichuan msg;
			msg.m_pos = m_curPos;
			msg.m_operType = OPER_MINGZHUA;
			m_nQiangDizhuState = OPER_MINGZHUA;
			BoadCast(msg);
		}
		else if (OPER_DAO == msg->m_operType && OPER_YES == msg->m_playerOper)  //抢地主;
		{
			//通知其他玩家该玩家倒;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DAO, OPER_YES);
			++m_nDaocishu;
			if (m_nDaocishu >= 4 ) //倒次数超过限制，开始游戏;
			{
				m_nDiZhuPos = m_curPos;
				LMsgS2CWhoIsDiZhu dizhuposMsg;
				dizhuposMsg.m_pos = m_nDiZhuPos;
				for(size_t i = 0 ; i <m_leftCard.size(); ++i)
				{
					dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
					m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
				}
				CardStyle::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
				BoadCast(dizhuposMsg);
				m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);

				SetDeskPlay();
				return;
			}
			else
			{
				m_curPos = GetNextPos(m_curPos);
				QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
			}
		}
		else if (OPER_DAO == msg->m_operType && OPER_NO == msg->m_playerOper) //不倒;
		{
			//通知其他玩家该玩家不倒;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DAO, OPER_NO);
			m_curPos = GetPrevPos(m_curPos);
			m_nDiZhuPos = m_curPos;
			LMsgS2CWhoIsDiZhu dizhuposMsg;
			dizhuposMsg.m_pos = m_nDiZhuPos;
			for(size_t i = 0 ; i <m_leftCard.size(); ++i)
			{
				dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
				m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
			}
			CardStyle::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
			BoadCast(dizhuposMsg);
			m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);

			SetDeskPlay();
		}
	}
	else   //错误消息;
	{
	}


}

void GameHandler_DDZ_2Players::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
{
	Lchar pos = GetUserPos(pUser);
	if(pos >= INVAILD_POS || getDeskState() != DESK_PLAY)
	{
		LLOG_ERROR("invalid pos:%d", pos);
		return;
	}

	if(m_curPos != pos)
	{
		return;
	}

	switch(msg->m_flag)
	{
	case not_play_card:
		{
			if(m_check == play_card_need_cheek)
			{
				//最先开始，不能不出;
				LLOG_ERROR("not_play_card error pos = %d",pos);
				return;
			}
			NotPlayCard(pos);
		}
		break;
	case play_card:
		{
			//排序删除重复项;
			std::sort(msg->m_card.begin(), msg->m_card.end());
			std::vector<Lint>::iterator it = std::unique(msg->m_card.begin(),msg->m_card.end());
			msg->m_card.erase(it, msg->m_card.end());

			//寻找手牌;
			CardVec tmp;
			for(Lsize i = 0 ; i < msg->m_card.size(); ++i)
			{
				for(Lsize j = 0 ; j < m_handCard[pos].size(); ++j)
				{
					if(msg->m_card[i] ==  m_handCard[pos][j]->GetIndex())
					{
						tmp.push_back(m_handCard[pos][j]);
						break;
					}
				}
			}
			CardStyle::Instance()->SortCard(tmp);

			//判断能不能出;
			CardStyleNumberVec style;
			CardStyle::Instance()->CheckCardStyle(tmp,style,tmp.size() == m_handCard[pos].size());
			int index = CardStyle::Instance()->CanBig(m_curPlayCount,m_curCardMode,m_curCardType,style);
			if(index != -1)
			{
				PlayCard(pos,tmp,style[index].m_bCardStyle,style[index].m_bCardNumber);
			}
			else
			{
				LMsgS2CUserPlay pack;
				pack.m_errorCode=1;
				pUser->Send(pack);
				//				LLOG_ERROR("play card_mode_error pos = %d",pos);     // 出牌牌型错误不需打印日志;
			}
		}
		break;
	}
}

Lint GameHandler_DDZ_2Players::getDeskState()
{
	return m_pDesk->m_deskState;
}

void GameHandler_DDZ_2Players::NotPlayCard(Lint pos)
{
	//录像;
	std::vector<Lint> vi;
	m_pDesk->m_video.AddOper(VIDEO_OPER_NULL,pos, vi);

	CardVec vTemp;
	SendPlayCard(pos,play_card_success,not_play_card,vTemp);
	m_playerCardType[pos] = card_number_type_null;
	m_outCard[pos].clear();
	CheckPlayState(pos);
}

void GameHandler_DDZ_2Players::PlayCard(Lint pos, CardVec& card, Lchar card_mode, Lchar card_type)
{
	//录像;
	std::vector<Lint> vec;
	for(Lsize i = 0 ; i < card.size(); ++i)
	{
		vec.push_back(card[i]->GetIndex());
	}
	m_pDesk->m_video.AddOper(card_mode,pos,vec);

	bool isFisrtOut = false;
	if (m_curCardMode == card_style_error)
	{
		isFisrtOut = true;
	}
	m_curCardMode = card_mode;
	m_curCardType = card_type;
	m_curPlayCount = (Lchar)card.size();
	if(m_curCardMode == card_style_rocket || m_curCardMode == card_style_bomb1 ||
		m_curCardMode == card_style_bomb2 || m_curCardMode == card_style_bomb3)
	{
		m_bombCount += 1;
		m_bomb[pos] += 1;
	}
	SendPlayCard(pos,play_card_success,play_card,card, isFisrtOut);

	m_playerCardType[pos] = card_type;
	CardStyle::Instance()->EarseCard(m_handCard[pos],card);
	m_putCard.insert(m_putCard.end(),card.begin(),card.end());
	m_outCard[pos] = card;

	//m_outCard[GetPrevPos(pos)].clear();
	CheckPlayState(pos);
}

void GameHandler_DDZ_2Players::CheckPlayState(Lint pos)
{
	if(CheckPlayEnd(pos))
	{
		return;
	}

	bool islast = false;  // 用于判断下一个出牌玩家是否最后一手牌;
	Lint nRangPai = 0;
	nRangPai = (pos == m_nDiZhuPos) ? 0 : ( m_nDaocishu + 1 );
	if(m_handCard[pos].size() <= (size_t) (2 + nRangPai) && m_handCard[pos].size() > 0  && m_baojing[pos] == 0 )
	{
		LMsgS2COnlyOneCard msg;
		msg.m_pos = pos;
		msg.m_leftCards = m_handCard[pos].size() - nRangPai;
		BoadCast(msg);
		m_baojing[pos] = 1;
	}

	m_curHelpInfo.clear();

	m_curPos = GetNextPos(pos);
	if(m_playerCardType[m_curPos] == m_curCardType )
	{   // 出牌后无人能大到此;
		//if(m_curCardMode == card_style_rocket || m_curCardMode == card_style_bomb1 ||
		//m_curCardMode == card_style_bomb2 || m_curCardMode == card_style_bomb3)
		//{
		//	m_bomb[m_curPos] += 1;
		//}

		memset(m_playerCardType,card_number_type_null,sizeof(m_playerCardType));
		m_curCardMode = card_style_error;
		m_curCardType = card_number_type_null;
		m_check = play_card_need_cheek;
		m_curPlayCount = 0;

		m_outCard[0].clear();
		m_outCard[1].clear();

		// 一手出牌;
		CardStyleNumberVec style;
		CardStyle::Instance()->CheckCardStyle(m_handCard[m_curPos], style, true);
		if (style.size() > 0)  // 检测能否一手出完,能够一手出完;
		{
			islast = true;
			if (m_handCard[m_curPos].size() > 4 && ( CardStyle::Instance()->CheckHasBomb(m_handCard[m_curPos]) || CardStyle::Instance()->HasRocket(m_handCard[m_curPos]) ) )
			{
				islast = false;
				m_curHelpInfo.clear();
			}
			else
			{
				Lint nLen = m_handCard[m_curPos].size();
				std::vector<int>  yishoupai;
				for (Lint i = 0; i < nLen; ++i)
				{
					yishoupai.push_back(m_handCard[m_curPos][i]->GetIndex());
				}
				m_curHelpInfo.push_back(yishoupai);
			}
		}
	}
	else
	{   // 有人能够要牌，不会进入下个玩家判断;
		m_check = play_card_not_check;
		m_curHelpInfo = m_help.HelpMe(m_curCardMode, m_curCardType, m_curPlayCount, m_handCard[m_curPos], m_curHelpUnusable, m_handCard[GetNextPos(m_curPos)].size() == 1);

		// 提示客户端自动出牌;
		//if (m_curHelpInfo.size() == 1 && m_curHelpInfo[0].size() == m_handCard[m_curPos].size() )  // 能够一手出完;
		//{
		//	islast = true;
		//}
	}

	SendPlayInfo(islast);
}

bool GameHandler_DDZ_2Players::CheckPlayEnd(Lint pos)
{
	if(m_handCard[pos].size() == 0 || ( pos != m_nDiZhuPos && m_handCard[pos].size() <= (size_t) 1 + m_nDaocishu) )  //两人斗地主需要让牌;
	{
		//最后一个炸弹;跑的快炸弹没有被人打时才算炸弹，导致最后一个炸弹少计算，这里补上;
		//if(m_curCardMode == card_style_rocket || m_curCardMode == card_style_bomb1 ||
		//	m_curCardMode == card_style_bomb2 || m_curCardMode == card_style_bomb3)
		//{
		//	m_bomb[pos] += 1;
		//}
		// 在这里增加玩家剩余牌数;
		LMsgS2CCard msg;
		for (Lint i = 0; i < USER_COUNT_2PERS; ++i)
		{
			for (size_t j = 0; j < m_handCard[i].size(); ++j)
			{
				msg.m_cards[i].push_back(m_handCard[i][j]->GetIndex());
			}
		}
		for (Lint i = 0; i < m_pDesk->m_nUserCounts; ++i)
		{
			msg.m_pos = i;
			m_pDesk->m_user[i]->Send(msg);
		}
		SetDeskEnd(pos);
		return true;
	}

	return false;
}

void GameHandler_DDZ_2Players::SetDeskEnd(Lchar pos)
{
	Lint win[USER_COUNT_2PERS] = {0};
	if (pos < m_pDesk->m_nUserCounts && m_pDesk->m_nUserCounts >= 0)
	{
		win[pos] = 1;
	}

	Ldouble gold[USER_COUNT_2PERS] = {0};
	Lint chuntian[USER_COUNT_2PERS] = {0};
	LMsgS2CGameOver pack;
	pack.m_win =  (pos);

	Lint nBombs = 0;
	for (Lint i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		nBombs += m_bomb[i];
	}
	Lint nMaxFanshu = m_pDesk->m_nMaxFanshu;
	nMaxFanshu = nMaxFanshu < 1 ? 3 : nMaxFanshu;  //客户端没有送来番数则按照最大三番处理;
	nBombs = nBombs > nMaxFanshu ? nMaxFanshu : nBombs;
	if (pos == m_nDiZhuPos) //地主胜利;
	{
		Lint otherPos = GetNextPos(pos);
		if (m_nCardCounts == m_handCard[otherPos].size()) //地主春天;
		{
			nBombs += 1;
			chuntian[pos] = 1;
			std::vector<Lint> vec;
			m_pDesk->m_video.AddOper(VIDEO_OPER_CHUNTIAN, pos, vec);
		}
		gold[otherPos] = (Lint) (-(m_nDifen + m_nDaocishu) * pow((double)2, nBombs));
		gold[pos] = -gold[otherPos];

		for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
		{
			pack.m_count[i] = m_handCard[i].size();
			pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;
		}
	}
	//农民胜利;
	else if(pos != INVAILD_POS)
	{
		if (m_nDizhuPlayCounts == 1) //地主只出牌一次,农民春天;
		{
			nBombs += 1;
			chuntian[pos] = 1;
			std::vector<Lint> vec;
			m_pDesk->m_video.AddOper(VIDEO_OPER_FANCHUN, pos, vec);
		}
		gold[m_nDiZhuPos] = 0;
		for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
		{
			pack.m_count[i] = m_handCard[i].size();
			pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;
			if (i != m_nDiZhuPos)
			{
				gold[i] = (Lint) ((m_nDifen + m_nDaocishu) * pow((double)2, nBombs));
				gold[m_nDiZhuPos] -= gold[i];
			}
		}	
	}
	else	{}  // pos == INVAILD_POS, do noting;

	for (Lint i = 0; i < USER_COUNT_2PERS; ++i)
	{
		gold[i] = gold[i] * m_pDesk->GetStake();
	}

	memcpy(pack.m_score,gold,sizeof(gold));
	memcpy(pack.m_bomb,m_bomb,sizeof(m_bomb));
	memcpy(pack.m_chuntian,chuntian,sizeof(chuntian));

	memset(m_pDesk->m_readyState,0,sizeof(m_pDesk->m_readyState));

	m_curPos = pos;

	Ldouble cal_gold[4] = { 0,0,0,0 };
	if (m_pDesk->m_video.m_time > 0)   // 防止未开始的局记入录像;
	{
		//保存录像;
		m_pDesk->m_video.m_Id = gVipLogMgr.GetVideoId();
		m_pDesk->m_vip->AddLog(m_pDesk->m_user, gold, cal_gold, win, m_bomb, m_pDesk->m_video.m_Id);

		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		m_pDesk->m_video.SetEveryResult(2, gold[0], gold[1]);
		m_pDesk->m_video.SetEveryResult2(2, cal_gold[0], cal_gold[1]);
		video.m_sql = m_pDesk->m_video.GetInsertSql();
		gWork.SendMsgToDb(video);

		m_pDesk->m_video.Clear();   //
	}

	//检测扣房卡;
	memcpy(pack.m_gold, cal_gold, sizeof(cal_gold));

	SetDeskWait();

	if(m_pDesk->m_vip->isEnd())
	{
		if(m_pDesk->m_vip->isFinalEnd())
		{
			m_pDesk->FinalEnd();
		}

		m_pDesk->m_vip->SendEnd();
		m_pDesk->m_vip->m_desk = NULL;
		SetDeskFree();
		ClearUser();
		m_pDesk->SetVip(NULL);
		ResetClear();
	}
	else
	{
		BoadCast(pack);
	}
}

void GameHandler_DDZ_2Players::SetDeskPlay()
{
	m_pDesk->m_deskState = DESK_PLAY;

	memset(m_playerCardType,card_number_type_null,sizeof(m_playerCardType));
	m_curCardMode = card_style_error;
	m_curCardType = card_number_type_null;
	m_check = play_card_need_cheek;

	//地主首先出牌;
	m_curPos = m_nDiZhuPos;

	m_putCard.clear();
	m_curHelpInfo.clear();
	m_curPlayCount = 0;

	SendPlayInfo();
}

bool GameHandler_DDZ_2Players::OnUserReconnect(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
		return false;
	}

	//发送桌子状态;
	LMsgS2CDeskState pack2;
	pack2.m_gameStyle = m_pDesk->m_gameStyle;
	pack2.m_diZhuPos = m_nDiZhuPos;
	pack2.m_Int_1.push_back(m_firstPos);
	pack2.m_Int_1.push_back(m_firstPosCard);
	Lint nBombs = 0;
	pack2.m_state = m_pDesk->m_deskState;
	for(size_t i = 0 ; i < m_handCard[pos].size(); ++i) //自己手牌;
	{	
		pack2.m_card.push_back(m_handCard[pos][i]->GetIndex());
	}
	if (m_pDesk->m_deskState == QIANG_DIZHU)
	{
		pack2.m_diZhuPos = INVAILD_VAL;
	}
	else
	{
		for (size_t i = 0; i < m_leftCard.size(); ++i)
		{
			pack2.m_dipai.push_back(m_leftCard[i]->GetIndex()); 
		}
	}

	for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i) //所有玩家出牌;
	{
		pack2.m_CurBomb[i] = m_bomb[i];
		pack2.m_cardNum[i] = m_handCard[i].size();
		for(size_t j = 0 ; j < m_outCard[i].size(); ++j)
		{
			pack2.m_out[i].push_back(m_outCard[i][j]->GetIndex());
		}
		if (INVAILD_VAL == m_nDiZhuPos && m_curPos == m_firstPos && m_nDaocishu == 0)
		{
			pack2.m_nUserBeishu[i] = 0;
		}
		else
		{
			pack2.m_nUserBeishu[i] = (2 + m_nDaocishu);		
		}
	}

	pUser->Send(pack2);

	if (QIANG_DIZHU == m_pDesk->m_deskState && pos == m_curPos)  //抢地主期间断线重连;
	{
		LMsgS2CQDZQuery_Sichuan msg;
		msg.m_pos = m_curPos;
		msg.m_operType = m_nQiangDizhuState;
		BoadCast(msg);
	}
	else
	{
		//当前出牌的玩家;
		LMsgS2COutCard msg;
		msg.m_pos = (m_curPos);
		msg.m_flag = (m_check);
		msg.m_last = 0;
		msg.m_time = 20;

		if(m_curPos == pos)
		{
			msg.m_card = m_curHelpInfo;
			if (play_card_not_check == msg.m_flag)
			{
				msg.m_cardUnusable = m_curHelpUnusable;
			}
		}
		pUser->Send(msg);

		for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
		{
			if(m_handCard[i].size() <= 2)
			{
				LMsgS2COnlyOneCard cardmsg;
				cardmsg.m_leftCards = m_handCard[i].size();
				cardmsg.m_pos = i;
				pUser->Send(cardmsg);
			}
		}
	}
	return true;
}

void GameHandler_DDZ_2Players::SendPlayInfo(bool islast)
{
	for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
	{
		LMsgS2COutCard msg;
		msg.m_pos= (m_curPos);
		msg.m_flag = (m_check);
		msg.m_time = 20;
		msg.m_last = islast;

		if(m_curPos == i)
		{
			msg.m_card = m_curHelpInfo;
			msg.m_cardUnusable = m_curHelpUnusable;
		}
		if(m_pDesk->m_user[i])
			m_pDesk->m_user[i]->Send(msg);
	}
}

void GameHandler_DDZ_2Players::SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec& vec, bool isFirstOut)
{
	std::vector<Lint>vv;
	for(Lsize i = 0 ; i < vec.size(); ++i)
	{
		vv.push_back(vec[i]->GetIndex());
	}

	LMsgS2CUserPlay pack;
	pack.m_errorCode=(ret);
	if(ret == 1)
	{
		m_pDesk->m_user[pos]->Send(pack);
	}
	else
	{
		pack.m_pos= pos;
		pack.m_flag = action;
		if (action != 1 && pos == m_nDiZhuPos)
		{
			++m_nDizhuPlayCounts;
		}
		if (true == isFirstOut)
		{
			pack.m_flag = 2;
		}

		for(size_t i = 0 ; i < vec.size(); ++i)
		{
			pack.m_card.push_back(vec[i]->GetIndex());
		}
		if(action == 0)
		{
			pack.m_type = m_curCardMode;
		}
		else
		{
			pack.m_type = 0;
		}
		BoadCast(pack);
	}
}

void GameHandler_DDZ_2Players::dealCard_new()
{
	_clearGame();
	for(Lint i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		m_handCard[i].clear();
		m_outCard[i].clear();
	}
	m_leftCard.clear();

	if (!gConfig.GetDebugModel())  // 非内测模式不允许调牌;
	{
		m_pDesk->m_tiaopaishu = 0;
	}

	m_dealCard.DealCard_2Player(m_handCard[0], m_handCard[1], m_pDesk->m_specialCard, m_pDesk->m_tiaopaishu);
	m_dealCard.GetLeftCard_2Player(m_leftCard);   //剩余牌消息;

	CardStyle::Instance()->SortCard(m_handCard[0]);
	CardStyle::Instance()->SortCard(m_handCard[1]);

	for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		if(m_pDesk->m_user[i])
		{
			LMsgS2CPlayStart msg;
			msg.m_gameStyle = (Lint)m_pDesk->m_gameStyle;
			for(size_t j = 0 ; j < m_handCard[i].size(); ++j)
			{
				msg.m_card.push_back(m_handCard[i][j]->GetIndex());
			}
			srand((int)time(0));
			Lint firstPos =	rand() % 2;
			srand((int)time(0));
			m_firstPosCard = m_handCard[firstPos][rand() % 17]->GetIndex();
			msg.m_firstPos_card.push_back(firstPos);
			msg.m_firstPos_card.push_back(m_firstPosCard);
			m_pDesk->m_user[i]->Send(msg);
			m_firstPos = firstPos;
		}
	}
}

void GameHandler_DDZ_2Players::ProcessRobot(Lint pos, User * pUser)
{
	switch (m_pDesk->m_deskState)
	{
	case DESK_PLAY:
		{
			if (m_curPos == pos)
			{
				LMsgC2SUserPlay msg;
				if( m_check == 0)
				{
					msg.m_flag = 0;
					msg.m_card.push_back(m_handCard[pos].back()->GetIndex());
				}
				else
				{
					if(m_curHelpInfo.size())
					{
						msg.m_flag = 0;
						msg.m_card = m_curHelpInfo.back();
					}
					else
					{
						msg.m_flag = 1;
					}
				}
				HanderUserPlayCard(pUser, &msg);
			}
		}
		break;
	case QIANG_DIZHU:
		if (pos == m_curPos)
		{
			LMsgC2SQdzOper_Sichuan msg;
			msg.m_pos = pos;
			msg.m_operType = m_nQiangDizhuState;
			msg.m_playerOper = rand()%2;
			HanderUserQiangDiZhuOper(pUser, &msg);
		}
		break;
	}
}

void GameHandler_DDZ_2Players::QueryUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType)
{
	if (MSG_S_2_C_QDZQUERY_NEW == nMsgID)
	{
		m_nQiangDizhuState = nOperType;
		LMsgS2CQDZQuery_Sichuan msg;
		msg.m_pos = nCurPos;
		msg.m_operType = nOperType;
		BoadCast(msg);
	}
}

void GameHandler_DDZ_2Players::BoadCastUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nOperRel, Lint isRestart)
{
	if (MSG_S_2_C_QDZOPER_NEW == nMsgID)
	{
		LMsgS2CQdzOper_Sichuan msg;
		msg.m_pos = nCurPos;
		msg.m_operType = nOperType;
		msg.m_playerOper = nOperRel;
		msg.m_blank = isRestart;
		BoadCast(msg);
	}
	std::vector<Lint> vec;
	m_pDesk->m_video.AddOper( nOperType*100 + nOperRel, nCurPos,vec);
}
///////////////////////////////////////////// 调用desk函数; //////////////////////////////////////////////////