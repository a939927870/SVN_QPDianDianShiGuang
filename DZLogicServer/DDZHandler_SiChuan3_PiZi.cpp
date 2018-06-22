#include "DDZHandler_SiChuan3_PiZi.h"
#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include <cmath>

bool GameHandler_DDZ_SiChuan3_PiZi::startup(Desk *desk)
{
	if (desk)
	{
		clearHandler();
		m_pDesk = desk;
	}

	//计分器重置
	m_score.Reset();
	
	return true;
}

void GameHandler_DDZ_SiChuan3_PiZi::clearHandler(void)
{
	_clearGame();
	m_firstPos = 0;   //需要保留信息到下一局，不能初始化;
	m_nDifen = 1; // 游戏底分默认为1;
}

//初始化每局游戏，与初始化桌子有区别，有信息需要保留;
void GameHandler_DDZ_SiChuan3_PiZi::_clearGame()
{
	m_score.Reset();

	//m_dianshu = INVAILD_VAL;
	//m_pokerPoint = INVAILD_VAL;
	//m_pokerStyle = INVAILD_VAL;
	//m_lightPokerIndex = INVAILD_VAL;	//明牌
	//m_dizhuFriendPokerIndex = INVAILD_VAL;	//地主腿子的牌
	//m_nTuiZiPos = INVAILD_VAL;	//地主腿子位置
	m_springType = EInvalid,

	m_curPos = 0;
	m_playPos = -1;
	m_ticktime = gWork.GetCurTime().MSecs();
	for (Lint i = 0; i < USER_COUNT_PiZi3; ++i)
	{
		m_bomb[i] = 0;
		m_handCard[i].clear();
		m_playerCardType[i] = 0;
		m_outCard[i].clear();
		m_baojing[i] = 0;

		m_daobudao[i] = INVAILD_VAL;
		m_labula[i] = INVAILD_VAL;
		m_menzhua[i] = INVAILD_VAL;
		//m_nUserBeishu[i] = 1;
	}
	m_nQiangDizhuState = INVAILD_VAL;
	m_nDizhuPlayCounts = 0;
	m_nDiZhuPos = INVAILD_VAL;
	m_leftCard.clear();
	memset(m_baojing,0,sizeof(m_baojing)); //跑的快报单信息;
}

void GameHandler_DDZ_SiChuan3_PiZi::SetDeskDeal()
{
	if (m_pDesk && m_pDesk->m_vip)
		m_pDesk->m_vip->SendInfo();

	dealCard_new();
	Lint userId[USER_COUNT_PiZi3] = {0};
	Ldouble score[USER_COUNT_PiZi3]={0};
	std::vector<Lint> vec[USER_COUNT_PiZi3];

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
	
#if USEASSERT
	assert(m_pDesk->m_extValue.size() > 0);
#endif
	if(m_pDesk->m_extValue.size() > 0)
	{
		vecGameType.push_back(game_para_diOrFan);
		vecGameType.push_back(m_pDesk->m_extValue[0]);	//加底加翻
	}
	InitScore();
	
	m_pDesk->m_video.DealCard(userId,vec,gWork.GetCurTime().Secs(),0,score,m_pDesk->m_id, m_pDesk->m_vip->m_curCircle, m_pDesk->m_vip->m_maxCircle, m_pDesk->m_gameStyle, vecGameType);
	m_curHelpInfo.clear();
	m_curHelpUnusable.clear();

	srand((int)time(0));
	m_firstPos = rand() % 3;
	m_curPos = m_firstPos;

	m_pDesk->m_deskState = QIANG_DIZHU;
	LMsgS2CQDZQuery_Sichuan msg;
	msg.m_pos = m_curPos;
	msg.m_operType = OPER_MENZHUA;
	m_nQiangDizhuState = OPER_MENZHUA;
	BoadCast(msg);
	return;
}

void GameHandler_DDZ_SiChuan3_PiZi::CheckSameIP()
{
	if (m_pDesk->m_nUserCounts != 3)
	{
		return;
	}
	Lstring szUserIP[USER_COUNT_PiZi3];
	for (Lint userNum = 0; userNum < m_pDesk->m_nUserCounts; ++userNum)
	{
		szUserIP[userNum] = m_pDesk->m_user[userNum]->GetIp();
	}
	Lint sameIPPos0;
	Lint sameIPPos1;
	Lint sameTimes = 0;
	for (Lint ii = 0; ii < USER_COUNT_PiZi3; ++ii)
	{
		for (Lint jj = ii + 1; jj < USER_COUNT_PiZi3; ++jj)
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

void GameHandler_DDZ_SiChuan3_PiZi::QiangDiZhu(Lint nPos)
{
	LMsgS2CAskDiZhu msg;
	msg.m_pos = nPos;
	msg.m_difen = 0;
	BoadCast(msg);
	return;
}

void GameHandler_DDZ_SiChuan3_PiZi::HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg)
{
#if USEASSERT
	assert(m_pDesk);
	assert(msg);
#endif

	if (m_pDesk->m_deskState != QIANG_DIZHU)
	{
		LLOG_ERROR("Sichuan3 Game:not QiangDIzhu Time");
		return;
	}
	if (GetUserPos(pUser) != msg->m_pos)
	{
		LLOG_ERROR("user position is not msg' position: %d!", msg->m_pos );
		return;
	}
	if (m_curPos == msg->m_pos)
	{
		if (1 == msg->m_operType && 1 == msg->m_playerOper) // 闷抓;
		{
			m_nDiZhuPos = m_curPos;
			m_score.SetDizhuPos(m_nDiZhuPos);
			//玩家闷抓;
			m_menzhua[m_curPos] = 1;
			m_score.AddMen(1, m_curPos);
			//通知所有玩家该玩家闷抓了地主;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MENZHUA, OPER_YES);
			//给该玩家亮牌;
			LMsgS2CWhoIsDiZhu dizhuposMsg;
			dizhuposMsg.m_pos = m_nDiZhuPos;
			for(size_t i = 0 ; i < m_leftCard.size(); ++i)
			{
				dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
				m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
			}
			CardStyle_SiChuan3_PiZi::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
			ShowCard2User(m_curPos);
			m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);
			m_pDesk->m_user[m_nDiZhuPos]->Send(dizhuposMsg);
			//询问其他玩家倒不倒;
			m_curPos = GetNextPos(m_curPos);
			QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
		}
		else if (1 == msg->m_operType && 0 == msg->m_playerOper) //看牌;
		{
			//没有闷抓;
			m_menzhua[m_curPos] = 0;
			//通知其他玩家该玩家看牌;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MENZHUA, OPER_NO);
			//给该玩家亮牌;
			ShowCard2User(m_curPos);
			QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_MINGZHUA);
		}
		else if (2 == msg->m_operType && 1 == msg->m_playerOper) //抓;
		{
			m_nDiZhuPos = m_curPos;
			m_score.SetDizhuPos(m_nDiZhuPos);
			//通知其他人该玩家抢了地主;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_YES);
			LMsgS2CWhoIsDiZhu dizhuposMsg;
			dizhuposMsg.m_pos = m_nDiZhuPos;
			for(size_t i = 0 ; i <m_leftCard.size(); ++i)
			{
				dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
				m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
			}
			CardStyle_SiChuan3_PiZi::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
			m_pDesk->m_user[m_nDiZhuPos]->Send(dizhuposMsg);
			m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);
			//给其他玩家亮牌;
			for (Lint nPos = GetNextPos(m_curPos); nPos != m_curPos;nPos = GetNextPos(nPos))
			{
				if (INVAILD_VAL == m_menzhua[nPos]) //已经看牌的玩家不在看牌;
				{
					ShowCard2User(nPos);
				}
			}
			//询问其他玩家倒不倒;
			m_curPos = GetNextPos(m_curPos);
			QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
		}
		else if (2 == msg->m_operType && 0 == msg->m_playerOper && (m_curPos == m_firstPos))//不抓;
		{
			//判断必抓牌;
			if (CardStyle_SiChuan3_PiZi::Instance()->IsBiZhua(m_handCard[m_curPos]))
			{
				QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_MINGZHUA);
				return;
			}
			//通知其他玩家该玩家不抓;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_NO);
			//询问下一个玩家是否闷抓;
			m_curPos = GetNextPos(m_curPos);
			QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_MENZHUA);
		}
		else if (2 == msg->m_operType && 0 == msg->m_playerOper && (m_curPos != m_firstPos))//不抓;
		{
			//判断必抓牌;
			if (CardStyle_SiChuan3_PiZi::Instance()->IsBiZhua(m_handCard[m_curPos]))
			{
				QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_MINGZHUA);
				return;
			}
			//通知其他玩家该玩家不抓;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MINGZHUA, OPER_NO);
			//最后一个玩家自动闷抓;
			m_curPos = GetNextPos(m_curPos);
			m_nDiZhuPos = m_curPos;
			m_score.SetDizhuPos(m_nDiZhuPos);
			m_menzhua[m_curPos] = 1;
			m_score.AddMen(1, m_curPos);
			//通知所有玩家该玩家闷抓了地主;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_MENZHUA, OPER_YES);

			//给该玩家亮牌;
			LMsgS2CWhoIsDiZhu dizhuposMsg;
			dizhuposMsg.m_pos = m_nDiZhuPos;
			for(size_t i = 0 ; i < m_leftCard.size(); ++i)
			{
				dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
				m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
			}
			CardStyle_SiChuan3_PiZi::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
			ShowCard2User(m_curPos);
			m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, dizhuposMsg.m_LeftCard);
			m_pDesk->m_user[m_nDiZhuPos]->Send(dizhuposMsg);

			//开始游戏;
			SetDeskPlay();
		}
		else if (OPER_LA == msg->m_operType && OPER_YES == msg->m_playerOper && (m_curPos == m_nDiZhuPos))//拉;
		{
			//通知其他玩家庄家拉;
			m_labula[m_nDiZhuPos] = 1;
			m_score.AddLa(1, m_nDiZhuPos);
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_LA, OPER_YES);
			//开始打牌;
			SetDeskPlay();
		}
		else if (OPER_LA == msg->m_operType && OPER_NO == msg->m_playerOper && (m_curPos == m_nDiZhuPos))//不拉;
		{
			//通知其他玩家庄家不拉;
			m_labula[m_nDiZhuPos] = 0;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_LA, OPER_NO);
			//开始打牌;
			SetDeskPlay();
		}
		else if (3 == msg->m_operType && 1 == msg->m_playerOper)  //倒;
		{
			//通知其他玩家该玩家倒;
			m_daobudao[msg->m_pos] = 1;
			m_score.AddDao(1, msg->m_pos);
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DAO, OPER_YES);
			//如果闷抓，给该玩家亮牌;
			if (1 == m_menzhua[m_nDiZhuPos] && -1 == m_menzhua[m_curPos])
			{
				ShowCard2User(msg->m_pos);
			}
			m_curPos = GetNextPos(m_curPos);
			if (m_curPos != m_firstPos )
			{
				QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
				return;
			}
			if (1 == m_menzhua[m_nDiZhuPos])
			{
				//悶抓不能拉，遊戲開始;
				SetDeskPlay();
				return;
			}
			m_curPos = m_nDiZhuPos;
			QueryUserOper(m_nDiZhuPos, MSG_S_2_C_QDZQUERY_NEW, OPER_LA);
		}
		else if (3 == msg->m_operType && 0 == msg->m_playerOper) //不倒;
		{
			//判断必倒牌;
			if (1 != m_menzhua[m_nDiZhuPos] && CardStyle_SiChuan3_PiZi::Instance()->IsBiDao(m_handCard[m_curPos]))
			{
				QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
				return;
			}
			m_daobudao[msg->m_pos] = 0;
			//通知其他玩家该玩家不倒;
			BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DAO, OPER_NO);
			//如果闷抓，该玩家未看牌;给该玩家亮牌;
			if (1 == m_menzhua[m_nDiZhuPos] && -1 == m_menzhua[m_curPos])
			{
				ShowCard2User(msg->m_pos);
			}
			m_curPos = GetNextPos(m_curPos);
			if (m_curPos != m_firstPos )
			{
				QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
				return;
			}
			if (1 == m_menzhua[m_nDiZhuPos])
			{
				//悶抓不能拉，遊戲開始;
				SetDeskPlay();
				return;
			}
			//所有玩家倒完成;发送地主拉不拉;或者开始游戏;
			if (OPER_YES == m_daobudao[GetNextPos(m_nDiZhuPos)] || OPER_YES == m_daobudao[GetPrevPos(m_nDiZhuPos)] )
			{
				QueryUserOper(m_nDiZhuPos, MSG_S_2_C_QDZQUERY_NEW, OPER_LA);
			}
			else
			{
				//开始打牌;
				SetDeskPlay();
			}
		}
	}
	else   //错误消息;
	{
	}
}

void GameHandler_DDZ_SiChuan3_PiZi::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
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
			CardStyle_SiChuan3_PiZi::Instance()->SortCard(tmp);


			CardStyleNumberVec style;

			//癞子奇异牌判断;
			if(card_style_error != msg->m_style)
			{
				BYTE nStype = msg->m_style;
				BYTE cardNumber = card_number_type_null;
				
				if(CardStyle_SiChuan3_PiZi::Instance()->CheckCardStyle(tmp, nStype, cardNumber))
				{
					CardStyleNumber node;
					node.m_bCardStyle = nStype;
					node.m_bCardNumber = cardNumber;
					node.m_bCardCount = BYTE(tmp.size());
					style.push_back(node);
				}
			}
			else
			{
				CardStyle_SiChuan3_PiZi::Instance()->CheckCardStyle(tmp,style,tmp.size() == m_handCard[pos].size(), m_curCardMode);
			}

			//判断能不能出;
			int index = CardStyle_SiChuan3_PiZi::Instance()->CanBig(m_curPlayCount, m_curCardMode, m_curCardType, style);
			if(index != -1 && IsPlayByFilter(style[index].m_bCardStyle, tmp.size() == m_handCard[pos].size()) && !IsLeftLaizi(tmp, pos))
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

//私有函数;
Lint GameHandler_DDZ_SiChuan3_PiZi::getDeskState()
{
	return m_pDesk->m_deskState;
}

void GameHandler_DDZ_SiChuan3_PiZi::NotPlayCard(Lint pos)
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

void GameHandler_DDZ_SiChuan3_PiZi::PlayCard(Lint pos, CardVec& card, Lchar card_mode, Lchar card_type)
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

	//assert(pos == m_curPos);
	m_playPos = pos;

	m_curPlayCount = (Lchar)card.size();

	DoBomb(pos);

	SendPlayCard(pos,play_card_success,play_card,card, isFisrtOut);

	m_playerCardType[pos] = card_type;
	CardStyle_SiChuan3_PiZi::Instance()->EarseCard(m_handCard[pos],card);
	m_putCard.insert(m_putCard.end(),card.begin(),card.end());
	m_outCard[pos] = card;

	//m_outCard[GetPrevPos(pos)].clear();
	CheckPlayState(pos);
}

void GameHandler_DDZ_SiChuan3_PiZi::CheckPlayState(Lint pos)
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
	if(m_playPos == m_curPos )
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
		m_outCard[2].clear();


		// 一手出牌;
		CardStyleNumberVec style;
		CardStyle_SiChuan3_PiZi::Instance()->CheckCardStyle(m_handCard[m_curPos], style, true);
		if (style.size() > 0)  // 检测能否一手出完,能够一手出完;
		{
			islast = true;
			if (m_handCard[m_curPos].size() > 4 && ( CardStyle_SiChuan3_PiZi::Instance()->CheckHasBomb(m_handCard[m_curPos]) || CardStyle_SiChuan3_PiZi::Instance()->HasRocket(m_handCard[m_curPos]) ) )
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

		//目前只对普通牌进行提示;
		m_curHelpInfo.clear();
		//m_curHelpInfo = m_help.HelpMe(m_curCardMode, m_curCardType, m_curPlayCount, m_handCard[m_curPos], m_curHelpUnusable, m_handCard[GetNextPos(m_curPos)].size() == 1);

		//有癞子牌的玩家;
		/*if(m_curHelpInfo.size() == 0 && m_pDesk && m_pDesk->m_user && m_pDesk->m_user[m_curPos] && !m_pDesk->m_user[m_curPos]->IsRobot())
		{
			CardVec laizi, normal;
			CardStyle_SiChuan3_PiZi::Instance()->GetLaizi(m_handCard[m_curPos], laizi, normal);
			if(laizi.size() >= 1)
			{
				std::vector<Lint> ret;
				ret.push_back(laizi[0]->GetIndex());
				m_curHelpInfo.push_back(ret);
			}
		}*/

		// 提示客户端自动出牌;
		//if (m_curHelpInfo.size() == 1 && m_curHelpInfo[0].size() == m_handCard[m_curPos].size() )  // 能够一手出完;
		//{
		//	islast = true;
		//}
	}

	SendPlayInfo(islast);
}

bool GameHandler_DDZ_SiChuan3_PiZi::CheckPlayEnd(Lint pos)
{
	if(m_handCard[pos].size() == 0)
	{
		//最后一个炸弹;跑的快炸弹没有被人打时才算炸弹，导致最后一个炸弹少计算，这里补上;
		//if(m_curCardMode == card_style_rocket || m_curCardMode == card_style_bomb1 ||
		//	m_curCardMode == card_style_bomb2 || m_curCardMode == card_style_bomb3)
		//{
		//	m_bomb[pos] += 1;
		//}
		// 在这里增加玩家剩余牌数;
		LMsgS2CCard msg;
		for (Lint i = 0; i < USER_COUNT_PiZi3; ++i)   // 包含弃牌堆信息;
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

void GameHandler_DDZ_SiChuan3_PiZi::SetDeskEnd(Lchar pos)
{
	Lint win[USER_COUNT_PiZi3] = {0};
	if (pos < m_pDesk->m_nUserCounts && m_pDesk->m_nUserCounts >= 0)
	{
		//胜负局数;
		for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
		{
			if(pos == m_nDiZhuPos)
			{
				win[m_nDiZhuPos] = 1;
				break;
			}
			else if(i != m_nDiZhuPos)
			{
				win[i] = 1;
			}
		}
	}
	
	LMsgS2CGameOver pack;
	pack.m_win =  (pos);

	Ldouble gold[USER_COUNT_PiZi3] = {0};

	GetScore(pos, gold);



	//数据填充
	Lint chuntian[USER_COUNT_PiZi3] = {0};
	for (Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
	{
		pack.m_count[i] = m_handCard[i].size();
		pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;

		if (i == pos && m_springType >= EDizhuSpring)
		{
			chuntian[i] = 1;
		}
	}

	memcpy(pack.m_score,gold,sizeof(gold));
	memcpy(pack.m_bomb,m_bomb,sizeof(m_bomb));
	memcpy(pack.m_chuntian, chuntian, sizeof(chuntian));
	memset(m_pDesk->m_readyState,0,sizeof(m_pDesk->m_readyState));

	m_curPos = pos;

	//（春）反春录像记录
	if(m_springType >= EDizhuSpring)
	{
		Lint videoChunType = m_springType == EDizhuSpring? VIDEO_OPER_CHUNTIAN : VIDEO_OPER_FANCHUN ;
		std::vector<Lint> vec;
		m_pDesk->m_video.AddOper(videoChunType, pos, vec);
	}
	for (Lint i = 0; i < USER_COUNT_PiZi3; ++i)
	{
		gold[i] = gold[i] * m_pDesk->GetStake();
	}
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

void GameHandler_DDZ_SiChuan3_PiZi::SetDeskPlay()
{
	//四川斗地主，如果是明抓地主，游戏开始前给其他玩家展示地主底牌;
	if (0 == m_menzhua[m_nDiZhuPos])
	{
		LMsgS2CWhoIsDiZhu dizhuposMsg;
		dizhuposMsg.m_pos = m_nDiZhuPos;
		for(size_t i = 0 ; i <m_leftCard.size(); ++i)
		{
			dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
		}
		for (Lint j = 0; j < m_pDesk->m_nUserCounts; ++j)
		{
			if (m_nDiZhuPos != j)
			{
				m_pDesk->m_user[j]->Send(dizhuposMsg);
			}
		}
	}

	m_pDesk->m_deskState = DESK_PLAY;

	for(Lsize i = 0 ; i < USER_COUNT_PiZi3; ++i)
	{
		m_outCard[i].clear();
		m_bomb[i] = 0;
	}
	memset(m_playerCardType,card_number_type_null,sizeof(m_playerCardType));
	m_curCardMode = card_style_error;
	m_curCardType = card_number_type_null;
	m_check = play_card_need_cheek;

	//地主首先出牌;
	m_curPos = m_nDiZhuPos;
	m_firstPos = m_nDiZhuPos;

	//m_bombCount = 0;
	m_putCard.clear();
	m_curHelpInfo.clear();
	m_curPlayCount = 0;

	SendPlayInfo();
}

bool GameHandler_DDZ_SiChuan3_PiZi::OnUserReconnect(User* pUser)
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

	//附加桌子信息;
	pack2.m_Int_1.push_back(m_score.GetTurnCount());	//当前翻数;
	Lint nBombs = 0;
	pack2.m_state = m_pDesk->m_deskState;
	assert(m_pDesk->m_nUserCounts >= 0);
	pack2.m_daobudao.resize(m_pDesk->m_nUserCounts);
	pack2.m_labula.resize(m_pDesk->m_nUserCounts);
	for (Lint i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		pack2.m_CurBomb[i] = m_bomb[i];
		pack2.m_daobudao[i] = m_daobudao[i];
		pack2.m_labula[i] = m_labula[i];
	}
	if (m_pDesk->m_deskState == QIANG_DIZHU)
	{
			pack2.m_diZhuPos = m_nDiZhuPos;
			if ( m_menzhua[pos] != INVAILD_VAL || INVAILD_VAL != m_daobudao[pos] || (INVAILD_VAL != m_nDiZhuPos && m_menzhua[m_nDiZhuPos] == OPER_NO) )
			{
				for(size_t i = 0 ; i < m_handCard[pos].size(); ++i) //自己手牌;
				{	
					pack2.m_card.push_back(m_handCard[pos][i]->GetIndex());
				}
			}
	}
	else
	{
		if (pos == m_nDiZhuPos || 0 == m_menzhua[m_nDiZhuPos])  //明抓底牌展示给所有人;
		{
			for (size_t i = 0; i < m_leftCard.size(); ++i)
			{
				pack2.m_dipai.push_back(m_leftCard[i]->GetIndex());  //四川斗地主底牌只有地主知道;
			}
		}
		for(size_t i = 0 ; i < m_handCard[pos].size(); ++i) //自己手牌;
		{	
			pack2.m_card.push_back(m_handCard[pos][i]->GetIndex());
		}
	}

	for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i) //所有玩家出牌;
	{
		pack2.m_cardNum[i] = m_handCard[i].size();
		for(size_t j = 0 ; j < m_outCard[i].size(); ++j)
		{
			if(m_outCard[i][j]->GetLaizi())
			{
				pack2.m_laiziNumber.push_back(m_outCard[i][j]->GetLaiziNumber());
			}
			
			pack2.m_out[i].push_back(m_outCard[i][j]->GetIndex());
		}
		pack2.m_nUserBeishu[i] = m_score.GetScore(pos);
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

void GameHandler_DDZ_SiChuan3_PiZi::SendPlayInfo(bool islast)
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

void GameHandler_DDZ_SiChuan3_PiZi::SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec& vec, bool isFirstOut)
{
	/*std::vector<Lint>vv;
	for(Lsize i = 0 ; i < vec.size(); ++i)
	{
		vv.push_back(vec[i]->GetIndex());
	}*/ //没有用到

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
		}
		else
		{
			pack.m_type = 0;
		}
		BoadCast(pack);
	}
}

void GameHandler_DDZ_SiChuan3_PiZi::dealCard_new()
{
#if USEASSERT
	assert(m_pDesk);
#endif

	if (!m_pDesk)
	{
		LLOG_ERROR("dealCard_new m_pDesk is nullptr");
		return;
	}

	_clearGame();

	if (!gConfig.GetDebugModel())  // 非内测模式不允许调牌;
	{
		m_pDesk->m_tiaopaishu = 0;
	}

	//有机器人时，给玩家调癞子
	if (gConfig.GetIfAddRobot() && m_pDesk->HasRobot())  
	{
		CardValue cardV;
		cardV.m_color = 0;
		cardV.m_number = card_number_type_LZ + 2;

#if USEASSERT
		assert(m_pDesk->m_tiaopaishu <= 54 && m_pDesk->m_tiaopaishu >= 0);
#endif
		for(int i = m_pDesk->m_tiaopaishu; i >= 0 ; --i)
		{
			m_pDesk->m_specialCard[i+1] = m_pDesk->m_specialCard[i];
		}

		m_pDesk->m_specialCard[0] = cardV;
		m_pDesk->m_tiaopaishu += 1;
	}
	//end
	
	//分配牌
	m_nCardCounts = 17;
	if (1) //0 == m_gameStyle || 3 == m_gameStyle;
	{
		m_dealCard.DealCard(m_handCard, 3, m_pDesk->m_specialCard, m_pDesk->m_tiaopaishu);
		m_dealCard.GetLeftCard(m_leftCard);   //剩余牌消息;
	}

	//发牌&整理（17张）
	CardStyle_SiChuan3_PiZi::Instance()->SortCard(m_handCard[0]);
	CardStyle_SiChuan3_PiZi::Instance()->SortCard(m_handCard[1]);
	CardStyle_SiChuan3_PiZi::Instance()->SortCard(m_handCard[2]);

	for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		if(m_pDesk->m_user[i])
		{
			LMsgS2CPlayStart msg;
			msg.m_gameStyle = (Lint)m_pDesk->m_gameStyle;
			m_pDesk->m_user[i]->Send(msg);
		}
	}
}

void GameHandler_DDZ_SiChuan3_PiZi::DoBomb(Lint pos)
{
#if USEASSERT
	assert(pos == m_curPos);
#endif

	if(m_curCardMode == card_style_rocket ||
		m_curCardMode == card_style_bomb1 ||	//普通1
		m_curCardMode == card_style_bomb2 ||
		m_curCardMode == card_style_bomb3)
	{
		m_score.AddBomb(1);
		m_bomb[pos] += 1;
	}
	else if(m_curCardMode == card_style_supper_rocket ||
		m_curCardMode == card_style_super_bomb)	//重炸
	{
		m_score.AddBomb(2);
		m_bomb[pos] += 1;
	}
	else if(m_curCardMode == card_style_bomb_double)	//连炸3
	{
		m_score.AddBomb(3);
		m_bomb[pos] += 1;
	}
}

//先临时实现
bool GameHandler_DDZ_SiChuan3_PiZi::IsPlayByFilter( Lint type, bool last){

	if(type == card_style_three && !last)	//痞子斗地主一个规则（不能单出3对）除非
	{
		return false;
	}

	if(type == card_style_three_double)		//三代对2 
	{
		return false;
	}

	if(type == card_style_bomb_and_single || type == card_style_four2)	//4带2
	{
		return false;
	}

	if(type == card_style_three_list_double)	//飞机带对
	{
		return false;
	}

	if(last && type == card_style_three_list)	//连得的飞机不能最后出
	{
		return false;
	}

	if(type == card_style_laizi)		//癞子牌不能单出
	{
		return false;
	}

	return true;
}


//bool GameHandler_DDZ_SiChuan3_PiZi::IsAllUserDone(User::UserRoomState state)
//{
//#if USEASSERT
//	assert(m_pDesk);
//#endif
//
//	for(int i = 0; i < m_pDesk->m_nUserCounts; i++)
//	{
//		if(m_pDesk->m_user[i])
//		{
//			if(!m_pDesk->m_user[i]->GetDone(state))
//			{
//				return false;
//			}
//		}
//	}
//
//	return true;
//}

void GameHandler_DDZ_SiChuan3_PiZi::BoadCastUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nOperRel)
{
	//UpdateUserBeishu();
	if (MSG_S_2_C_QDZOPER_NEW == nMsgID)
	{
		LMsgS2CQdzOper_Sichuan msg;
		msg.m_pos = nCurPos;
		msg.m_operType = nOperType;
		msg.m_playerOper = nOperRel;

		for (Lint i = 0; i  < m_pDesk->m_nUserCounts; ++i)
		{
			msg.m_nUserBeishu[i] = m_score.GetScore(i);
		}
		
		BoadCast(msg);
	}
	std::vector<Lint> vec;
	m_pDesk->m_video.AddOper( nOperType*100 + nOperRel, nCurPos,vec);
}


void GameHandler_DDZ_SiChuan3_PiZi::QueryUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType)
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

void GameHandler_DDZ_SiChuan3_PiZi::ProcessRobot(Lint pos, User * pUser)
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
					msg.m_style = 0;
				}
				else
				{
					if(m_curHelpInfo.size())
					{
						msg.m_flag = 0;
						msg.m_card = m_curHelpInfo.back();
						msg.m_style = 0;
					}
					else
					{
						msg.m_flag = 1;
						msg.m_style = 0;
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

//农民春天，地主反春;
bool GameHandler_DDZ_SiChuan3_PiZi::IsNongMinSpring(Lint winPos)	
{
	if (winPos == m_nDiZhuPos)
	{
		return false;
	}

	bool isSpring = m_nDizhuPlayCounts == 1;
	
	if(isSpring)
	{
		m_springType = ENongminSpring;
	}

	return isSpring;
}

//地主是否春天;
bool GameHandler_DDZ_SiChuan3_PiZi::IsLandlordSpring(Lint winPos)	
{
	if (winPos != m_nDiZhuPos) //地主没胜利;
	{
		return false;
	}

	std::vector<BYTE> vecDizhu;
	std::vector<BYTE> vecNongmin;

	for(int i = 0; i< m_pDesk->m_nUserCounts; ++i)
	{
		if(m_pDesk->m_user[i])
		{
			if(i == m_nDiZhuPos)
			{
				vecDizhu.push_back(i);
			}
			else
			{
				vecNongmin.push_back(i);
			}
		}
	}

#if USEASSERT
	assert(vecDizhu.size() + vecNongmin.size() == 4);
#endif

	int nongminSize = vecNongmin.size();

	for (int i = 0; i <nongminSize ; ++i)
	{
		if(m_nCardCounts != m_handCard[vecNongmin[i]].size())
		{
			return false;
		}
	}

	m_springType = EDizhuSpring;

	return true;
}

void GameHandler_DDZ_SiChuan3_PiZi::GetScore(Lint winPos, Ldouble * gold)
{
	if (winPos == m_nDiZhuPos) //地主胜利;
	{
#if USEASSERT
		assert(m_nDiZhuPos != INVAILD_VAL);
#endif

		//
		bool isDizhuSpr = IsLandlordSpring(winPos);
		if(isDizhuSpr)	//地主春天
		{
			m_score.AddBomb(1);
		}

		//
		if(true) //一打2
		{


			//int score = m_score.GetScore();
			//int scoreWin = score * 2;
#if USEASSERT
			assert(score > 0);
#endif

			gold[m_nDiZhuPos] = m_score.GetScore(m_nDiZhuPos);
			for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
			{
				if(i != m_nDiZhuPos && m_pDesk->m_user[i])
				{
					gold[i] = - m_score.GetScore(i);
				}
			}
		}
	}
	//农民胜利;
	else if(winPos != INVAILD_POS)
	{
#if USEASSERT
		assert(m_nDiZhuPos != INVAILD_VAL);
#endif

		m_firstPos = winPos;

		if (IsNongMinSpring(winPos)) //地主只出牌一次,农民春天;
		{
			m_score.AddBomb(2);
		}
		gold[m_nDiZhuPos] = 0;
	

		//int score = m_score.GetScore();

#if USEASSERT
		assert(score > 0);
#endif

		if(true)	//一打2
		{
			//int score = m_score.GetScore();
			//int scoreLost = score * 2; 

#if USEASSERT
			assert(score > 0);
#endif

			gold[m_nDiZhuPos] = -m_score.GetScore(m_nDiZhuPos);
			for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
			{
				if(i != m_nDiZhuPos && m_pDesk->m_user[i])
				{
					gold[i] = +m_score.GetScore(i);
				}
			}
		}
	}
	else	{}  // pos == INVAILD_POS, do noting;
}

void GameHandler_DDZ_SiChuan3_PiZi::InitScore()
{
	//计分器重置
	m_score.Reset();

	if(m_pDesk->m_extValue.size() == 0)
	{
		m_score.Init(1, m_pDesk->m_nMaxFanshu, 1); //默认
	}
	else
	{
		Lint type = m_pDesk->m_extValue[0];
		m_score.Init(type, m_pDesk->m_nMaxFanshu, 1); //
	}

	
	
}

bool GameHandler_DDZ_SiChuan3_PiZi::IsLeftLaizi(const CardVec & out, Lint pos)
{
#if USEASSERT
	assert(pos >= 0 && pos < m_pDesk->m_nUserCounts);
#endif
	
	Lint allSize = m_handCard[pos].size();
	if(out.size() + 1 == allSize)
	{
		Lint size = out.size();
		bool isInOut = false;
		bool isInAll = false;
		for(int i = 0; i < allSize; ++i)
		{
			if(i < size && out[i]->GetLaizi())
			{
				isInOut = true;
				return false;
			}

			if(m_handCard[pos][i]->GetLaizi())
			{
				isInAll = true;
			}
		}

		if(!isInOut && isInAll)
		{
			return true;
		}
	}
	
	return false;
}

void GameHandler_DDZ_SiChuan3_PiZi::ShowCard2User(Lint nCurPos)
{
	LMsgS2CShowCards msg;
	msg.m_pos = nCurPos;
	std::vector<Lint> vec;
	for (size_t i = 0; i < m_handCard[nCurPos].size(); ++i)
	{
		msg.m_MyCard.push_back(m_handCard[nCurPos][i]->GetIndex());
		vec.push_back(m_handCard[nCurPos][i]->GetIndex());
	}
	m_pDesk->m_user[nCurPos]->Send(msg);
	// 录像;
	m_pDesk->m_video.AddOper(VIDEO_OPER_SHOWCARDS, nCurPos, vec);
}

///////////////////////////////////////////// 调用desk函数; //////////////////////////////////////////////////