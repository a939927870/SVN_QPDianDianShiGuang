#include "DDZHandler_SanLai.h"
#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include <cmath>

bool GameHandler_DDZ_3lai::startup(Desk *desk)
{
	if (desk)
	{
		clearHandler();
		m_pDesk = desk;
		m_nCardCounts = 17;
	}
	return true;
}

void GameHandler_DDZ_3lai::clearHandler(void)
{
	_clearGame();
	m_firstPos = 0;   //需要保留信息到下一局，不能初始化;
}

//初始化每局游戏，与初始化桌子有区别，有信息需要保留;
void GameHandler_DDZ_3lai::_clearGame()
{
	m_nDifen = 2; // 低分首叫地主即变为2;
	m_curPos = 0;
	m_playPos = -1;
	m_ticktime = gWork.GetCurTime().MSecs();
	for (Lint i = 0; i < USER_COUNT_3LAI; ++i)
	{
		m_bomb[i] = 0;
		m_handCard[i].clear();
		m_playerCardType[i] = 0;
		m_outCard[i].clear();
		m_baojing[i] = 0;
		m_bJiaoOrNo[i] = -1;
	}
	m_nQiangDizhuState = INVAILD_VAL;
	m_nDizhuPlayCounts = 0;
	m_nDiZhuPos = INVAILD_VAL;
	m_leftCard.clear();
	m_bombCount = 0;
	m_laiziCard = 0;  //癞子初始化为0,非真正牌值;
	memset(m_baojing,0,sizeof(m_baojing)); //跑的快报单信息;

	m_dealCard.ClearLaizi();
}

void GameHandler_DDZ_3lai::SetDeskDeal()
{
	if (m_pDesk && m_pDesk->m_vip)
		m_pDesk->m_vip->SendInfo();

	dealCard_new();
	Lint userId[USER_COUNT_3LAI] = {0};
	Ldouble score[USER_COUNT_3LAI]={0};
	std::vector<Lint> vec[USER_COUNT_3LAI];

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

	srand((int)time(0));
	m_firstPos = rand() % 3;
	m_curPos = m_firstPos;

	m_pDesk->m_deskState = QIANG_DIZHU;
	LMsgS2CQDZQuery_Sichuan msg;
	msg.m_pos = m_curPos;
	msg.m_operType = OPER_MINGZHUA;
	m_nQiangDizhuState = OPER_MINGZHUA;
	BoadCast(msg);
}

void GameHandler_DDZ_3lai::CheckSameIP()
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

void GameHandler_DDZ_3lai::QiangDiZhu(Lint nPos)
{
	LMsgS2CAskDiZhu msg;
	msg.m_pos = nPos;
	msg.m_difen = 0;
	BoadCast(msg);
	return;
}

void GameHandler_DDZ_3lai::HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg)
{
	if (m_pDesk->m_deskState != QIANG_DIZHU)
	{
		LLOG_ERROR("Sanlai:not QiangDIzhu Time");
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
			m_bJiaoOrNo[m_curPos] = 1;  //玩家叫了地主，可以在结束前进行最后一次抢地主;

			m_nDiZhuPos = m_curPos;  //非最终地主，会被修改;
			//通知其他人该玩家抢了地主;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_YES);

			//询问其他玩家抢不抢;
			Lint tempPos = GetNextPos(m_curPos);
			//第一次叫地主时候：下家是否抢地主的停止条件是再次到自己;
			while (tempPos != m_curPos)
			{
				if (0 != m_bJiaoOrNo[tempPos] )  //如果下家没有对叫不叫发言过不叫;
				{
					m_curPos = tempPos;
					QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO, m_nDifen+1);
					return;
				}
				tempPos = GetNextPos(tempPos);
			}
			{  //能到这里说明只有最后一家叫了地主;
				LMsgS2CWhoIsDiZhu dizhuposMsg;
				dizhuposMsg.m_pos = m_nDiZhuPos;
				for(size_t i = 0 ; i <m_leftCard.size(); ++i)
				{
					dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
					m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
				}
				CardStyle_3Lai::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
				BoadCast(dizhuposMsg);
				m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);
				SetDeskPlay();
			}
		}
		else if (OPER_MINGZHUA == msg->m_operType && OPER_NO == msg->m_playerOper && (GetNextPos(m_curPos) != m_firstPos))//不叫;
		{
			m_bJiaoOrNo[m_curPos] = 0;  //玩家不叫,需要记录;
			//通知其他玩家该玩家不抓;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_NO);
			//询问下一个玩家是否抓;
			m_curPos = GetNextPos(m_curPos);
			QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_MINGZHUA, m_nDifen+1);
		}
		else if (OPER_MINGZHUA == msg->m_operType && OPER_NO == msg->m_playerOper && (GetNextPos(m_curPos) == m_firstPos))//不叫;
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

			srand((int)time(0));
			m_firstPos = rand() % 3;
			m_curPos = m_firstPos;
			LMsgS2CQDZQuery_Sichuan msg;
			msg.m_pos = m_curPos;
			msg.m_operType = OPER_MINGZHUA;
			m_nQiangDizhuState = OPER_MINGZHUA;
			BoadCast(msg);
		}
		else if (OPER_DAO == msg->m_operType && OPER_YES == msg->m_playerOper)  //抢地主;
		{
			//通知其他玩家该玩家抢地主;
			++m_nDifen;
			m_nDiZhuPos = m_curPos;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DAO, OPER_YES);
			if(1 == m_bJiaoOrNo[m_curPos])//叫地主完成，开始游戏;(该消息时，只有抢的人叫过才回结束，不然必然有其他玩家可以抢)
			{
				LMsgS2CWhoIsDiZhu dizhuposMsg;
				dizhuposMsg.m_pos = m_nDiZhuPos;
				for(size_t i = 0 ; i <m_leftCard.size(); ++i)
				{
					dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
					m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
				}
				CardStyle_3Lai::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
				BoadCast(dizhuposMsg);
				m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);

				SetDeskPlay();
				return;
			}

			Lint tempPos = GetNextPos(m_curPos);
			while(tempPos != m_curPos)			//下家不一定有资格抢;
			{
				if (0 != m_bJiaoOrNo[tempPos])
				{
					m_curPos = tempPos;
					QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO, m_nDifen+1);
					return;
				}
				tempPos = GetNextPos(tempPos);
			}
			//运行到此有错误;
			LLOG_ERROR("MSG error: Sanlai,Line 261");
		}
		else if (OPER_DAO == msg->m_operType && OPER_NO == msg->m_playerOper) //不抢地主;
		{
			//通知其他玩家该玩家不抢;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DAO, OPER_NO);

			Lint tempPos = GetNextPos(m_curPos);
			while(tempPos != m_curPos && 1 != m_bJiaoOrNo[m_curPos])			//下家不一定有资格抢;寻找下个有资格抢地主的玩家;
			{
				if (0 != m_bJiaoOrNo[tempPos] && tempPos != m_nDiZhuPos)
				{
					m_curPos = tempPos;
					QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO, m_nDifen+1);
					return;
				}
				if (1 == m_bJiaoOrNo[tempPos])
				{
					break;
				}
				tempPos = GetNextPos(tempPos);
			}

			//首家叫地主者已经抢了或者只有一人叫地主;
			{
				LMsgS2CWhoIsDiZhu dizhuposMsg;
				dizhuposMsg.m_pos = m_nDiZhuPos;
				for(size_t i = 0 ; i <m_leftCard.size(); ++i)
				{
					dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
					m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
				}
				CardStyle_3Lai::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
				BoadCast(dizhuposMsg);
				m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);
				SetDeskPlay();
				return;
			}
		}
	}
	else   //错误消息;
	{
	}
}

void GameHandler_DDZ_3lai::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
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
			CardStyle_3Lai::Instance()->SortCard(tmp);

			//判断能不能出;
			CardStyleNumberVec style;
			Lint objCardMode = msg->m_style == card_style_error? m_curCardMode : msg->m_style; 
			CardStyle_3Lai::Instance()->CheckCardStyle(tmp,style,tmp.size() == m_handCard[pos].size(), objCardMode);
			int index = CardStyle_3Lai::Instance()->CanBig(m_curPlayCount,m_curCardMode,m_curCardType,style);
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

Lint GameHandler_DDZ_3lai::getDeskState()
{
	return m_pDesk->m_deskState;
}

void GameHandler_DDZ_3lai::NotPlayCard(Lint pos)
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

void GameHandler_DDZ_3lai::PlayCard(Lint pos, CardVec& card, Lchar card_mode, Lchar card_type)
{
	//录像;
	std::vector<Lint> vec;
	for(Lsize i = 0 ; i < card.size(); ++i)
	{
		vec.push_back(card[i]->GetIndex());
	}
	std::vector<Lint> vec_laizi;
	for(size_t i = 0 ; i < card.size(); ++i)
	{
		if(card[i]->GetLaizi())
		{
			vec_laizi.push_back(card[i]->GetLaiziNumber());
		}
	}
	m_pDesk->m_video.AddOper(card_mode,pos,vec,vec_laizi);

	bool isFisrtOut = false;
	if (m_curCardMode == card_style_error)
	{
		isFisrtOut = true;
	}
	m_curCardMode = card_mode;
	m_curCardType = card_type;
	m_playPos = pos;
	m_curPlayCount = (Lchar)card.size();
	if(m_curCardMode == card_style_rocket || m_curCardMode == card_style_bomb1 ||
		m_curCardMode == card_style_bomb2 || m_curCardMode == card_style_bomb3)
	{
		m_bombCount += 1;
		m_bomb[pos] += 1;
	}
	SendPlayCard(pos,play_card_success,play_card,card, isFisrtOut);

	m_playerCardType[pos] = card_type;
	CardStyle_3Lai::Instance()->EarseCard(m_handCard[pos],card);
	m_putCard.insert(m_putCard.end(),card.begin(),card.end());
	m_outCard[pos] = card;

	//m_outCard[GetPrevPos(pos)].clear();
	CheckPlayState(pos);
}

void GameHandler_DDZ_3lai::CheckPlayState(Lint pos)
{
	if(CheckPlayEnd(pos))
	{
		return;
	}

	bool islast = false;  // 用于判断下一个出牌玩家是否最后一手牌;
	if(m_handCard[pos].size() <= 2 && m_handCard[pos].size() > 0  && m_baojing[pos] == 0 )
	{
		LMsgS2COnlyOneCard msg;
		msg.m_pos = pos;
		msg.m_leftCards = m_handCard[pos].size();
		BoadCast(msg);
		m_baojing[pos] = 1;
	}

	m_curHelpInfo.clear();

	m_curPos = GetNextPos(pos);
	if(m_playPos == m_curPos)
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
		islast = false;
		//CardStyleNumberVec style;
		//CardStyle_3Lai::Instance()->CheckCardStyle(m_handCard[m_curPos], style, m_handCard[m_curPos].size() == m_handCard[m_curPos].size());
		//if (style.size() > 0)  // 检测能否一手出完,能够一手出完;
		//{
		//	islast = true;
		//	if (m_handCard[m_curPos].size() > 4 && ( CardStyle_3Lai::Instance()->CheckHasBomb(m_handCard[m_curPos]) || CardStyle_3Lai::Instance()->HasRocket(m_handCard[m_curPos]) ) )
		//	{
		//		islast = false;
		//		m_curHelpInfo.clear();
		//	}
		//	else
		//	{
		//		Lint nLen = m_handCard[m_curPos].size();
		//		std::vector<int>  yishoupai;
		//		for (Lint i = 0; i < nLen; ++i)
		//		{
		//			yishoupai.push_back(m_handCard[m_curPos][i]->GetIndex());
		//		}
		//		m_curHelpInfo.push_back(yishoupai);
		//	}
		//}
	}
	else
	{   // 有人能够要牌，不会进入下个玩家判断;
		m_check = play_card_not_check;
		m_curHelpInfo = m_help.HelpMe(m_curCardMode, m_curCardType, m_curPlayCount, m_handCard[m_curPos], m_curHelpUnusable, m_handCard[GetNextPos(m_curPos)].size() == 1);
	}

	SendPlayInfo(islast);
}

bool GameHandler_DDZ_3lai::CheckPlayEnd(Lint pos)
{
	if(m_handCard[pos].size() == 0)  //两人斗地主需要让牌;
	{
		//最后一个炸弹;跑的快炸弹没有被人打时才算炸弹，导致最后一个炸弹少计算，这里补上;
		//if(m_curCardMode == card_style_rocket || m_curCardMode == card_style_bomb1 ||
		//	m_curCardMode == card_style_bomb2 || m_curCardMode == card_style_bomb3)
		//{
		//	m_bomb[pos] += 1;
		//}
		// 在这里增加玩家剩余牌数;
		LMsgS2CCard msg;
		for (Lint i = 0; i < USER_COUNT_3LAI; ++i)
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

void GameHandler_DDZ_3lai::SetDeskEnd(Lchar pos)
{
	Lint win[USER_COUNT_3LAI] = {0};
	if (pos < m_pDesk->m_nUserCounts && m_pDesk->m_nUserCounts >= 0)
	{
		win[pos] = 1;
	}

	Ldouble gold[USER_COUNT_3LAI] = {0};
	Lint chuntian[USER_COUNT_3LAI] = {0};
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
		Lint nextPos1 = GetNextPos(pos);
		Lint nextPos2 = GetNextPos(nextPos1);
		if (m_nCardCounts == m_handCard[nextPos1].size() && m_nCardCounts == m_handCard[nextPos2].size()) //地主春天;
		{
			nBombs += 1;
			chuntian[pos] = 1;
			std::vector<Lint> vec;
			m_pDesk->m_video.AddOper(VIDEO_OPER_CHUNTIAN, pos, vec);
		}
		gold[nextPos1] = gold[nextPos2] = (Lint) (-m_nDifen * pow((double)2, nBombs));
		gold[pos] = -2 * gold[nextPos1];

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
				win[i] = 1;
				gold[i] = (Lint) (m_nDifen * pow((double)2, nBombs));
				gold[m_nDiZhuPos] -= gold[i];
			}
		}	
	}
	else	{}  // pos == INVAILD_POS, do noting;

	for (Lint i = 0; i < USER_COUNT_3LAI; ++i)
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
		m_pDesk->m_video.SetEveryResult(3, gold[0], gold[1], gold[2]);
		m_pDesk->m_video.SetEveryResult2(3, cal_gold[0], cal_gold[1], cal_gold[2]);
		video.m_sql = m_pDesk->m_video.GetInsertSql();
		gWork.SendMsgToDb(video);

		m_pDesk->m_video.Clear();   //
	}

	//检测扣房卡;
	memcpy(pack.m_gold, cal_gold, sizeof(cal_gold));

	SetDeskWait();
	if(m_pDesk->m_vip->isEnd())
	{
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

void GameHandler_DDZ_3lai::GetLaiZi()
{
	srand((int)time(0));
	m_laiziCard = rand() % 13 + 1;
	m_dealCard.SetLaizi(m_laiziCard);

	Lint laizi_client;//转为客户端识别值;
	laizi_client = m_laiziCard <= 11 ? (m_laiziCard + 2) : (m_laiziCard - 11);
	BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_LAIZI, laizi_client);
}

void GameHandler_DDZ_3lai::SetDeskPlay()
{
	GetLaiZi();

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

bool GameHandler_DDZ_3lai::OnUserReconnect(User* pUser)
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
	pack2.m_pokerStyle = m_curCardMode;
	pack2.m_pokerNumber = m_curCardType;

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
		if (m_laiziCard != 0)
		{
			Lint laizi_client;//转为客户端识别值;
			laizi_client = m_laiziCard <= 11 ? (m_laiziCard + 2) : (m_laiziCard - 11);
			pack2.m_laiziCard.push_back(laizi_client);
		}
	}

	for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i) //所有玩家出牌;
	{
		pack2.m_CurBomb[i] = m_bomb[i];
		pack2.m_cardNum[i] = m_handCard[i].size();
		for(size_t j = 0 ; j < m_outCard[i].size(); ++j)
		{
			pack2.m_out[i].push_back(m_outCard[i][j]->GetIndex());
			if (m_outCard[i][j]->GetLaizi())
			{
				pack2.m_laiziChange[i].push_back(m_outCard[i][j]->GetLaiziNumber());
			}
		}
		pack2.m_nUserBeishu[i] = m_nDifen;
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

void GameHandler_DDZ_3lai::SendPlayInfo(bool islast)
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

void GameHandler_DDZ_3lai::SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec& vec, bool isFirstOut)
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
			if(vec[i]->GetLaizi())
			{
				pack.m_laiziNumber.push_back(vec[i]->GetLaiziNumber());
			}
			pack.m_card.push_back(vec[i]->GetIndex());
		}
		if(action == 0)	
		{
			pack.m_type = m_curCardMode;
			pack.m_typeNumber = m_curCardType;
		}
		else
		{
			pack.m_type = 0;
		}
		BoadCast(pack);
	}
}

void GameHandler_DDZ_3lai::dealCard_new()
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

	m_dealCard.DealCard(m_handCard, m_pDesk->m_nUserCounts, m_pDesk->m_specialCard, m_pDesk->m_tiaopaishu);
	m_dealCard.GetLeftCard(m_leftCard);   //剩余牌消息;

	for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		CardStyle_3Lai::Instance()->SortCard(m_handCard[i]);
		if(m_pDesk->m_user[i])
		{
			LMsgS2CPlayStart msg;
			msg.m_gameStyle = (Lint)m_pDesk->m_gameStyle;
			for(size_t j = 0 ; j < m_handCard[i].size(); ++j)
			{
				msg.m_card.push_back(m_handCard[i][j]->GetIndex());
			}
			m_pDesk->m_user[i]->Send(msg);
		}
	}
}

void GameHandler_DDZ_3lai::ProcessRobot(Lint pos, User * pUser)
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
						msg.m_style = card_style_error;
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

void GameHandler_DDZ_3lai::QueryUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nPara1)
{
	if (MSG_S_2_C_QDZQUERY_NEW == nMsgID)
	{
		m_nQiangDizhuState = nOperType;
		LMsgS2CQDZQuery_Sichuan msg;
		msg.m_pos = nCurPos;
		msg.m_operType = nOperType;
		msg.m_dianshu = nPara1;
		BoadCast(msg);
	}
}

void GameHandler_DDZ_3lai::BoadCastUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nOperRel, Lint isRestart)
{
	if (MSG_S_2_C_QDZOPER_NEW == nMsgID && nOperType != OPER_LAIZI)
	{
		LMsgS2CQdzOper_Sichuan msg;
		msg.m_pos = nCurPos;
		msg.m_operType = nOperType;
		msg.m_playerOper = nOperRel;
		msg.m_blank = isRestart;
		BoadCast(msg);

		std::vector<Lint> vec;
		m_pDesk->m_video.AddOper( nOperType*100 + nOperRel, nCurPos,vec);
	}
	if (MSG_S_2_C_QDZOPER_NEW == nMsgID && nOperType == OPER_LAIZI)
	{
		LMsgS2CQdzOper_Sichuan msg;
		msg.m_pos = nCurPos;
		msg.m_operType = nOperType;
		msg.m_playerOper = nOperRel;
		msg.m_blank = isRestart;
		BoadCast(msg);

		std::vector<Lint> vec;
		vec.push_back(nOperRel);
		m_pDesk->m_video.AddOper(nOperType*100, 0,vec);  //癞子记入录像;特殊处理，不太符合规范;
	}
}
///////////////////////////////////////////// 调用desk函数; //////////////////////////////////////////////////