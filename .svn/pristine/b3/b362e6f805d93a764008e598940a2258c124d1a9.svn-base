#include "DDZHandler_classical.h"
#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include <cmath>
extern unsigned int g_nGoldGameNums;
const int g_nGoldPerConsume = 200;

bool GameHandler_DDZ_Classical::startup(Desk *desk)
{
	if (desk)
	{
		clearHandler();
		m_pDesk = desk;
	}

	return true;
}

void GameHandler_DDZ_Classical::clearHandler(void)
{
	_clearGame();
	m_firstPos = 0;   //需要保留信息到下一局，不能初始化;
	m_subType = EClassical;
}

//初始化每局游戏，与初始化桌子有区别，有信息需要保留;
void GameHandler_DDZ_Classical::_clearGame()
{
	m_curPos = 0;
	m_ticktime = gWork.GetCurTime().MSecs();
	for (Lint i = 0; i < 3; ++i)
	{
		m_bomb[i] = 0;
		m_handCard[i].clear();
		m_playerCardType[i] = 0;
		m_outCard[i].clear();
		m_baojing[i] = 0;

		m_daobudao[i] = INVAILD_VAL;
		m_labula[i] = INVAILD_VAL;
		m_menzhua[i] = INVAILD_VAL;
		m_jiaoDizhu[i] = EJIao;
		m_nUserBeishu[i] = 1;
	}
	m_nQiangDizhuState = INVAILD_VAL;
	m_nDizhuPlayCounts = 0;
	m_nDiZhuPos = INVAILD_VAL;
	m_leftCard.clear();
	m_nDifen = 1; // 游戏底分默认为1;

	memset(m_baojing,0,sizeof(m_baojing)); //跑的快报单信息;
}

void GameHandler_DDZ_Classical::SetDeskDeal()
{
	//init(没找到合适的位置）;
	if(!m_pDesk) 
	{
		LLOG_ERROR("SetDeskDeal m_pDesk is nullptr");
		return;
	}
#if USEASSERT
	assert(m_pDesk->m_extValue.size() > 0);
#endif



	if(m_pDesk && m_pDesk->m_extValue.size() > 0)
	{
		m_subType = (ClassicalSubType)m_pDesk->m_extValue[0];
	}
	else
	{
		m_subType = EClassical;
	}

	if (m_pDesk && m_pDesk->m_vip)
	{
		m_pDesk->m_vip->m_subFlag = m_subType;	//游戏子类型;
		m_pDesk->m_vip->SendInfo();
	}
	dealCard_new();
	Lint userId[USER_COUNT_CLASSICAL] = {0};
	Ldouble score[USER_COUNT_CLASSICAL]={0};
	std::vector<Lint> vec[USER_COUNT_CLASSICAL];

	if (game_zone_gold == m_pDesk->m_gameZone)
	{
		for (Lint i = 0; i < m_pDesk->m_nUserCounts; ++i)
		{
			m_pDesk->m_user[i]->m_userData.m_Gold -= g_nGoldPerConsume;
			m_pDesk->m_user[i]->send2LMGGoldChange(-g_nGoldPerConsume, GOLDCOSUME, m_pDesk->m_user[i]->m_userData.m_Gold);
		}
	}

	for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
	{
		if(m_pDesk->m_user[i])
			userId[i] = m_pDesk->m_user[i]->GetUserDataId();

		score[i] = m_pDesk->m_vip->GetUserScore(m_pDesk->m_user[i]);

		//if (game_ddz_classical == m_gameStyle)
		//{
		for(size_t j = 0 ; j < m_handCard[i].size(); ++j)
		{
			vec[i].push_back(m_handCard[i][j]->GetIndex());
		}
		//}
	}
	std::vector<Lint> vecGameType;
	vecGameType.push_back(game_para_maxfan);
	vecGameType.push_back(m_pDesk->m_nMaxFanshu);
	
	BYTE subType = m_subType;
	vecGameType.push_back(game_para_classic3_subType);
	vecGameType.push_back(subType);
	
	m_pDesk->m_video.DealCard(userId,vec,gWork.GetCurTime().Secs(),0,score,m_pDesk->m_id, m_pDesk->m_vip->m_curCircle, m_pDesk->m_vip->m_maxCircle, m_pDesk->m_gameStyle, vecGameType);
	m_curHelpInfo.clear();
	m_curHelpUnusable.clear();

	StartQiangDizhu();
}

void GameHandler_DDZ_Classical::CheckSameIP()
{
	if (m_pDesk->m_nUserCounts != USER_COUNT_CLASSICAL)
	{
		return;
	}
	Lstring szUserIP[USER_COUNT_CLASSICAL];
	for (Lint userNum = 0; userNum < m_pDesk->m_nUserCounts; ++userNum)
	{
		szUserIP[userNum] = m_pDesk->m_user[userNum]->GetIp();
	}
	Lint sameIPPos0;
	Lint sameIPPos1;
	Lint sameTimes = 0;
	for (Lint ii = 0; ii < USER_COUNT_CLASSICAL; ++ii)
	{
		for (Lint jj = ii + 1; jj < USER_COUNT_CLASSICAL; ++jj)
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

void GameHandler_DDZ_Classical::QiangDiZhu(Lint nPos)
{
	LMsgS2CAskDiZhu msg;
	msg.m_pos = nPos;
	msg.m_difen = m_nDifen;
	if(m_subType == EJIaoDizhu)
		msg.m_state = m_jiaoDizhu[nPos];	//当前状态;
	BoadCast(msg);

	//玩家抢地主倒计时;
	m_pDesk->setTimeOfWaitUser(2, nPos, 21);
	return;
}

void GameHandler_DDZ_Classical::HanderUserQiangDiZhu(User* pUser,LMsgC2SQiangDiZhu* msg)
{
	if(m_subType == EClassical)
	{
		HanderUserQiangDiZhuOfClassical(pUser, msg);
	}
	else if(m_subType == EJiaoFen)
	{
		HanderUserQiangDiZhuOfJiaofen(pUser, msg);
	}
	else if(m_subType == EJIaoDizhu)
	{
		HanderUserQiangDiZhuOfJiaoDizhu(pUser, msg);
	}
}

void GameHandler_DDZ_Classical::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
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

//私有函数;
Lint GameHandler_DDZ_Classical::getDeskState()
{
	return m_pDesk->m_deskState;
}

void GameHandler_DDZ_Classical::NotPlayCard(Lint pos)
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

void GameHandler_DDZ_Classical::PlayCard(Lint pos, CardVec& card, Lchar card_mode, Lchar card_type)
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

void GameHandler_DDZ_Classical::CheckPlayState(Lint pos)
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
		m_outCard[2].clear();


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

bool GameHandler_DDZ_Classical::CheckPlayEnd(Lint pos)
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
		for (Lint i = 0; i < USER_COUNT_CLASSICAL; ++i)   // 包含弃牌堆信息;
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

void GameHandler_DDZ_Classical::SetDeskEnd(Lchar pos)
{
	if (game_zone_gold == m_pDesk->m_gameZone)
	{
		SetGoldDeskEnd(pos);
		return;
	}
	Lint win[USER_COUNT_CLASSICAL] = {0};

	Ldouble gold[USER_COUNT_CLASSICAL] = {0};
	Lint chuntian[USER_COUNT_CLASSICAL] = {0};
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
		m_firstPos = pos;
		win[pos] = 1;
		Lint nextPos1 = GetNextPos(pos);
		Lint nextPos2 = GetNextPos(nextPos1);
		if (m_nCardCounts == m_handCard[nextPos1].size() && m_nCardCounts == m_handCard[nextPos2].size()) //地主春天;
		{
			nBombs += 1;
			chuntian[pos] = 1;
			std::vector<Lint> vec;
			m_pDesk->m_video.AddOper(VIDEO_OPER_CHUNTIAN, pos, vec);
		}
		gold[nextPos1] = (Lint) (-m_nDifen * pow((double)2, nBombs) * m_nUserBeishu[nextPos1]);
		gold[nextPos2] = (Lint) (-m_nDifen * pow((double)2, nBombs) * m_nUserBeishu[nextPos2]);
		gold[pos] = -gold[nextPos1] - gold[nextPos2];

		for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
		{
			pack.m_count[i] = m_handCard[i].size();
			pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;
		}
	}
	//农民胜利;
	else if(pos != INVAILD_POS)
	{
		m_firstPos = pos;
		if (m_nDizhuPlayCounts == 1) //地主只出牌一次,农民春天;
		{
			nBombs += 1;
			for (Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
			{
				if (m_handCard[i].size() == 0)
				{
					chuntian[i] = 1;
					std::vector<Lint> vec;
					m_pDesk->m_video.AddOper(VIDEO_OPER_FANCHUN, i, vec);
				}
			}
		}
		gold[m_nDiZhuPos] = 0;
		for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
		{
			pack.m_count[i] = m_handCard[i].size();
			pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;
			if (i != m_nDiZhuPos)
			{
				win[i] = 1;
				gold[i] = (Lint) (m_nDifen * pow((double)2, nBombs) * m_nUserBeishu[i]);
				gold[m_nDiZhuPos] -= gold[i];
			}
		}	
	}
	else	{}  // pos == INVAILD_POS, do noting;

	for (Lint i = 0; i < USER_COUNT_CLASSICAL; ++i)
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

void GameHandler_DDZ_Classical::SetGoldDeskEnd(Lchar pos)
{
	Lint actuBeishu = m_nDifen;
	m_nDifen = 20;  //金币场底分为20分; 
	Lint win[USER_COUNT_CLASSICAL] = {0};
	Lint gold[USER_COUNT_CLASSICAL] = {0};
	Lint chuntian[USER_COUNT_CLASSICAL] = {0};
	LMsgS2CGameOver pack;
	pack.m_win =  (pos);

	Lint nBombs = 0;
	for (Lint i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		nBombs += m_bomb[i];
	}
	if (pos == m_nDiZhuPos) //地主胜利;
	{
		m_firstPos = pos;
		win[pos] = 1;
		Lint nextPos1 = GetNextPos(pos);
		Lint nextPos2 = GetNextPos(nextPos1);
		if (m_nCardCounts == m_handCard[nextPos1].size() && m_nCardCounts == m_handCard[nextPos2].size()) //地主春天;
		{
			nBombs += 1;
			chuntian[pos] = 1;
			std::vector<Lint> vec;
			m_pDesk->m_video.AddOper(VIDEO_OPER_CHUNTIAN, pos, vec);
		}
		Lint nMaxBeishu1 = (Lint) pow((double)2, nBombs) * actuBeishu;
		Lint nMaxBeishu2 = (Lint) pow((double)2, nBombs) * actuBeishu;
		nMaxBeishu1 = nMaxBeishu1 > 256 ? 256 : nMaxBeishu1; 
		nMaxBeishu2 = nMaxBeishu2 > 256 ? 256 : nMaxBeishu2; 
		gold[nextPos1] = -m_nDifen * nMaxBeishu1;
		gold[nextPos2] = -m_nDifen * nMaxBeishu2;
		gold[pos] = -gold[nextPos1] - gold[nextPos2];

		for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
		{
			pack.m_count[i] = m_handCard[i].size();
			pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;
		}
	}
	//农民胜利;
	else if(pos != INVAILD_POS)
	{
		m_firstPos = pos;
		if (m_nDizhuPlayCounts == 1) //地主只出牌一次,农民春天;
		{
			nBombs += 1;
			for (Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
			{
				if (m_handCard[i].size() == 0)
				{
					chuntian[i] = 1;
					std::vector<Lint> vec;
					m_pDesk->m_video.AddOper(VIDEO_OPER_FANCHUN, i, vec);
				}
			}
		}
		gold[m_nDiZhuPos] = 0;
		for(Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
		{
			pack.m_count[i] = m_handCard[i].size();
			pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;
			if (i != m_nDiZhuPos)
			{
				win[i] = 1;
				Lint nMaxBeishu = (Lint) pow((double)2, nBombs) * actuBeishu;
				nMaxBeishu = nMaxBeishu > 256 ? 256 : nMaxBeishu; 
				gold[i] = m_nDifen * nMaxBeishu;
				gold[m_nDiZhuPos] -= gold[i];
			}
		}	
	}
	else	{}  // pos == INVAILD_POS, do noting;

	memcpy(pack.m_score,gold,sizeof(gold));
	memcpy(pack.m_bomb,m_bomb,sizeof(m_bomb));
	memcpy(pack.m_chuntian,chuntian,sizeof(chuntian));
	memset(m_pDesk->m_readyState,0,sizeof(m_pDesk->m_readyState));

	//金币变化;通知存储并通知客户端;
	for (Lint ii = 0; ii < m_pDesk->m_nUserCounts; ++ii)
	{
		m_pDesk->m_user[ii]->m_userData.m_Gold += gold[ii];
		if (m_pDesk->m_user[ii]->m_userData.m_Gold < 0)
		{
			gold[ii] -= m_pDesk->m_user[ii]->m_userData.m_Gold ;
			m_pDesk->m_user[ii]->m_userData.m_Gold = 0;
		}
		m_pDesk->m_user[ii]->send2LMGGoldChange(gold[ii], GAMERESULT, m_pDesk->m_user[ii]->m_userData.m_Gold);
	}
	m_pDesk->m_vip->m_curCircle += 1;
	SetDeskWait();
	BoadCast(pack);
	if(m_pDesk->m_vip->isEnd())
	{
		if(m_pDesk->m_vip->isFinalEnd())
		{
			m_pDesk->FinalEnd();
		}
		//m_pDesk->m_vip->SendEnd();
		m_pDesk->m_vip->m_desk = NULL;
		SetDeskFree();
		ClearUser();
		m_pDesk->SetVip(NULL);
		ResetClear();

		//统计金币场次;
		g_nGoldGameNums++;
	}
}

void GameHandler_DDZ_Classical::SetDeskPlay()
{
	m_pDesk->m_deskState = DESK_PLAY;

	for(Lsize i = 0 ; i < USER_COUNT_CLASSICAL; ++i)
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

	m_bombCount = 0;
	m_putCard.clear();
	m_curHelpInfo.clear();
	m_curPlayCount = 0;

	SendPlayInfo();
}

bool GameHandler_DDZ_Classical::OnUserReconnect(User* pUser)
{
	Lint pos = GetUserPos(pUser);
	if (pos == INVAILD_POS)
	{
		LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
		return false;
	}

	if(!m_pDesk)
	{
		LLOG_ERROR("OnUserReconnect m_pDesk is nullptr");
		return false;
	}


	//发送桌子状态;
	LMsgS2CDeskState pack2;
	pack2.m_gameStyle = m_pDesk->m_gameStyle;
	pack2.m_diZhuPos = m_nDiZhuPos;
	pack2.m_difen = m_nDifen;
	Lint nBombs = 0;
	pack2.m_state = m_pDesk->m_deskState;
	for (Lint i = 0; i < 3; ++i)
	{
		pack2.m_CurBomb[i] = m_bomb[i];
	}
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
		pack2.m_cardNum[i] = m_handCard[i].size();
		for(size_t j = 0 ; j < m_outCard[i].size(); ++j)
		{
			pack2.m_out[i].push_back(m_outCard[i][j]->GetIndex());
		}
		pack2.m_nUserBeishu[i] = m_nUserBeishu[i];

		//Gold房間增加玩家是否託管表示;
		if (m_pDesk->m_user[i] && game_zone_gold == m_pDesk->m_gameZone)
		{
			pack2.m_IsTuoguan.push_back(m_pDesk->m_user[i]->m_bTuoGuan);
		}
	}

	pUser->Send(pack2);

	if (QIANG_DIZHU == m_pDesk->m_deskState)  //抢地主期间断线重连;
	{
		QiangDiZhu(m_curPos);
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

void GameHandler_DDZ_Classical::SendPlayInfo(bool islast)
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

		//玩家出牌计时;
		m_pDesk->setTimeOfWaitUser(1, m_curPos, 21);
	}
}

void GameHandler_DDZ_Classical::SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec& vec, bool isFirstOut)
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

		//玩家出牌取消重置託管倒計時;
		m_pDesk->unSetTimeOfWaitUser(1, pos);
	}
}

void GameHandler_DDZ_Classical::dealCard_new()
{
	_clearGame();

	if (!gConfig.GetDebugModel())  // 非内测模式不允许调牌;
	{
		m_pDesk->m_tiaopaishu = 0;
	}
	m_nCardCounts = 17;
	if (1) //0 == m_gameStyle || 3 == m_gameStyle;
	{
		m_dealCard.DealCard(m_handCard[0], m_handCard[1], m_handCard[2], m_pDesk->m_specialCard, m_pDesk->m_tiaopaishu);
		m_dealCard.GetLeftCard(m_leftCard);   //剩余牌消息;
	}

	CardStyle::Instance()->SortCard(m_handCard[0]);
	CardStyle::Instance()->SortCard(m_handCard[1]);
	CardStyle::Instance()->SortCard(m_handCard[2]);

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
			m_pDesk->m_user[i]->Send(msg);
		}
	}
}

void GameHandler_DDZ_Classical::ProcessRobot(Lint pos, User * pUser)
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
					if (game_zone_gold == m_pDesk->m_gameZone)
					{
						msg.m_flag = 1;
					}
					else if(m_curHelpInfo.size())
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
			LMsgC2SQiangDiZhu msg;
			msg.m_pos = pos;

			if(m_subType == EJiaoFen)
			{
				
				msg.m_difen = m_nDifen + 1;
			}
			else
			{
				msg.m_difen = m_nDifen;
			}
			
			msg.m_yaobu = 1;
			HanderUserQiangDiZhu(pUser, &msg);
		}
		break;
	}
}

void GameHandler_DDZ_Classical::HanderUserQiangDiZhuOfClassical(User* pUser,LMsgC2SQiangDiZhu* msg)
{
	if (m_pDesk->m_deskState != QIANG_DIZHU)
	{
		LLOG_ERROR("Classical Game:not QiangDIzhu Time");
		return;
	}
	if (GetUserPos(pUser) != msg->m_pos)
	{
		LLOG_ERROR("user position is not msg' position: %d!", msg->m_pos );
		return;
	}
	if (msg->m_pos != m_curPos)
	{
		LLOG_ERROR("user position error! %d, is not pos: %d",m_curPos, msg->m_pos );
		return;
	}
	//广播玩家选择结果;
	LMsgS2CAnsDiZhu AnsMsg;
	AnsMsg.m_pos = msg->m_pos;
	AnsMsg.m_yaobu = msg->m_yaobu;
	AnsMsg.m_difen = msg->m_difen;
	AnsMsg.m_restart = 0;
	if (1 != msg->m_yaobu && GetNextPos(msg->m_pos) == m_firstPos)
	{
		AnsMsg.m_restart = 1;
	}
	BoadCast(AnsMsg);

	//收到客户端操作信息，重置客户端等待时间;
	m_pDesk->unSetTimeOfWaitUser(2, msg->m_pos);
	if (1 == msg->m_yaobu)
	{
		m_nDiZhuPos = msg->m_pos;
		LMsgS2CWhoIsDiZhu dizhuposMsg;
		dizhuposMsg.m_pos = m_nDiZhuPos;
		dizhuposMsg.m_difen = msg->m_difen;
		for(size_t i = 0 ; i < m_leftCard.size(); ++i)
		{
			dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
			m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
		}
		CardStyle::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
		BoadCast(dizhuposMsg);
		std::vector<Lint> vec;
		m_pDesk->m_video.AddOper(VIDEO_OPER_QIANGDIZHU,m_nDiZhuPos,vec);
		//先将抢地主记入录像;
		// 记入录像;
		for(Lsize i = 0 ; i < m_leftCard.size(); ++i)
		{
			vec.push_back(m_leftCard[i]->GetIndex());
		}
		m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI,m_nDiZhuPos,vec);
		//开始打牌;
		SetDeskPlay();
	}
	else
	{
		if (GetNextPos(msg->m_pos) != m_firstPos)
		{
			//录像;
			std::vector<Lint> vi;
			m_pDesk->m_video.AddOper(VIDEO_OPER_BUQIANG,msg->m_pos,vi);
			//下家继续抢地主;
			Lint pos = GetNextPos(msg->m_pos);
			m_curPos = pos;
			QiangDiZhu(pos);
		}
		else
		{
			//不抢地主录像;
			std::vector<Lint> vi;
			m_pDesk->m_video.AddOper(VIDEO_OPER_BUQIANG,msg->m_pos,vi);
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
			m_pDesk->m_deskState = QIANG_DIZHU;
			QiangDiZhu(m_curPos); // 重新抢地主;
		}
	}
	return;
}

void GameHandler_DDZ_Classical::HanderUserQiangDiZhuOfJiaofen(User* pUser,LMsgC2SQiangDiZhu* msg)
{
	if (m_pDesk->m_deskState != QIANG_DIZHU)
	{
		LLOG_ERROR("Classical Jiaofen: not QiangDIzhu Time");
		return;
	}
	if (GetUserPos(pUser) != msg->m_pos)
	{
		LLOG_ERROR("user position is not msg' position: %d!", msg->m_pos );
		return;
	}
	if (msg->m_pos != m_curPos)
	{
		LLOG_ERROR("user position error! %d, is not pos: %d",m_curPos, msg->m_pos );
		return;
	}

	Lint pos = m_curPos;

	if (1 == msg->m_yaobu && m_jiaoDizhu[pos] != ERefuse)	//叫或抢
	{
		if(m_jiaoDizhu[pos] == EJIao)
		{
			Lint jiaoFen = msg->m_difen;

			//强制升分数
			if(jiaoFen <= m_nDifen)
				jiaoFen = m_nDifen + 1;

			m_nDifen = jiaoFen;


			//录像
			std::vector<Lint> vec;
			vec.push_back(jiaoFen);
			RecordVideoQiangDizhu(pos, true, vec);


			if(jiaoFen < 3)
			{
				if(m_jiaoDizhu[pos] != ERefuse)
				{
					m_jiaoDizhu[pos] = EReady;
				}
			}
			else
			{
#if USEASSERT
				assert(jiaoFen <=3);
#endif

				for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
				{
					if(m_jiaoDizhu[i] != ERefuse)
					{
						m_jiaoDizhu[i] = EReady;
					}
				}
			}
		}
		else
		{
			//没有这个情况
#if USEASSERT
			assert(false);
#endif
		}


		m_firstPos = pos;

	}
	else
	{
		RecordVideoQiangDizhu(pos, false);

		m_jiaoDizhu[pos] = ERefuse;	
	}

	UsersJiaoState usersState = CheckUsersJiaoDizhuState();

	BroadCastJiaoDizhu(pos, msg->m_yaobu, usersState == EAllFail);

	if(usersState == EWait)
	{
		Lint nextPos = GetNextValidPos(pos);
#if USEASSERT
		assert(nextPos != INVAILD_VAL && m_jiaoDizhu[nextPos] != ERefuse && m_jiaoDizhu[nextPos] != EReady );
#endif
		//异常处理
		if(nextPos == INVAILD_VAL)
		{
#if USEASSERT
			assert(false);
#endif
			//重新洗牌
			ReStartJiaoDizhu(pos);
			return;
		}

		//下家继续抢地主;
		m_curPos = nextPos;
		QiangDiZhu(nextPos);
	}
	else if(usersState == EAllFail)
	{
		//重新洗牌
		ReStartJiaoDizhu(pos);
	}
	else if(usersState == EOk)
	{
		DecideDizhu(m_firstPos);
	}

	return;
}

void GameHandler_DDZ_Classical::HanderUserQiangDiZhuOfJiaoDizhu(User* pUser,LMsgC2SQiangDiZhu* msg)
{
	if (m_pDesk->m_deskState != QIANG_DIZHU)
	{
		LLOG_ERROR("Classical JiaoDIzhu:not QiangDIzhu Time");
		return;
	}
	if (GetUserPos(pUser) != msg->m_pos)
	{
		LLOG_ERROR("user position is not msg' position: %d!", msg->m_pos );
		return;
	}
	if (msg->m_pos != m_curPos)
	{
		LLOG_ERROR("user position error! %d, is not pos: %d",m_curPos, msg->m_pos );
		return;
	}
	
	Lint pos = m_curPos;

	do {
		if (1 == msg->m_yaobu)	//叫或抢;
		{
			if(m_jiaoDizhu[pos] == ERefuse){ /*assert(false);*/return;} //拒绝过的不能再要;
			if(m_jiaoDizhu[pos] == EReady) { /*assert(false);*/return;} //要过且准备好了;

			if(m_jiaoDizhu[pos] == EJIao)
			{

				//录像（叫地主）;
				RecordVideoQiangDizhu(pos, true);


				for(int i = 0; i < m_pDesk->m_nUserCounts ; ++i)
				{
					if(m_jiaoDizhu[i] != ERefuse)	//刷整个状态倒抢（拒绝的除外）;
					{
						m_jiaoDizhu[i] = EQiang;
					}
				}
				//分
				m_nDifen *= 2;
			}
			else if (m_jiaoDizhu[pos] == EQiang)
			{
				//录像（抢地主）;
				RecordVideoQiangDizhu(pos, true);

				m_jiaoDizhu[pos] = EReady;
		
				//分数;
				m_nDifen *= 2;
			}
			else
			{
				//没有这个情况;
#if USEASSERT
				assert(false);
#endif
			}

			//
			m_firstPos = pos;
		}
		else
		{
			if(m_jiaoDizhu[pos] == EReady) {/*assert(false); */return;} //准备好的不能再拒绝不要;
		
			
			//记录像（不抢或者不叫）;
			RecordVideoQiangDizhu(pos, false);

			//不要;
			m_jiaoDizhu[pos] = ERefuse;	
		}
	}
	while(0);

	UsersJiaoState usersState = CheckUsersJiaoDizhuState();

	BroadCastJiaoDizhu(pos, msg->m_yaobu, usersState == EAllFail);

	//收到客户端操作信息，重置客户端等待时间;
	m_pDesk->unSetTimeOfWaitUser(2, msg->m_pos);

	if(usersState == EWait)
	{
		Lint nextPos = GetNextValidPos(pos);
#if USEASSERT
		assert(nextPos != INVAILD_VAL && m_jiaoDizhu[nextPos] != ERefuse && m_jiaoDizhu[nextPos] != EReady );
#endif
		if(nextPos == INVAILD_VAL)
		{
#if USEASSERT
			assert(false);
#endif
			//重新洗牌;
			ReStartJiaoDizhu(pos);
			return;
		}

		m_curPos = nextPos;
		QiangDiZhu(nextPos);
	}
	else if(usersState == EAllFail)
	{
		//重新洗牌;
		ReStartJiaoDizhu(pos);
	}
	else if(usersState == EOk)
	{
		DecideDizhu(m_firstPos);
	}
		
	return;
}

void GameHandler_DDZ_Classical::StartQiangDizhu()
{
	if(m_subType == EClassical)
	{
		StartQiangDizhuOfClassical();
	}
	else if(m_subType == EJiaoFen)
	{
		StartQiangDizhuOfJiaofen();		
	}
	else if(m_subType == EJIaoDizhu)
	{
		StartQiangDizhuOfJiaoDizhu();
	}
}

void GameHandler_DDZ_Classical::StartQiangDizhuOfClassical()
{
	m_pDesk->m_deskState = QIANG_DIZHU;
	m_curPos = m_firstPos;
	QiangDiZhu(m_curPos); // 从房主开始询问是否抢地主;
}

void GameHandler_DDZ_Classical::StartQiangDizhuOfJiaofen()
{
	//
	m_nDifen = 0;
	//

	m_pDesk->m_deskState = QIANG_DIZHU;
	Lint firstPos = L_Rand(0,2);
	m_firstPos = firstPos;
	m_curPos = m_firstPos;
	QiangDiZhu(m_curPos); // 从房主开始询问是否抢地主;
}

void GameHandler_DDZ_Classical::StartQiangDizhuOfJiaoDizhu()
{
	m_pDesk->m_deskState = QIANG_DIZHU;
	Lint firstPos = L_Rand(0,2);
	m_firstPos = firstPos;
	m_curPos = m_firstPos;
	QiangDiZhu(m_curPos); // 从房主开始询问是否抢地主;
}

Lint GameHandler_DDZ_Classical::GetNextValidPos(Lint pos)
{
	Lint currPos = pos;
	Lint nextPos = INVAILD_VAL;
	
	Lint i = 0;

	while((nextPos = GetNextPos(currPos)) != pos)
	{
		if(m_jiaoDizhu[nextPos] != ERefuse && m_jiaoDizhu[nextPos] != EReady)
		{
			return nextPos;
		}
		else
		{
			currPos = nextPos;
		}

		i++;
	}

#if USEASSERT
	assert(i < 3);
#endif

	return INVAILD_VAL;
}

GameHandler_DDZ_Classical::UsersJiaoState GameHandler_DDZ_Classical::CheckUsersJiaoDizhuState()
{
	Lint readyCount = 0;
	Lint refuseCount = 0;
	Lint qiangCount = 0;

	for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		if(m_jiaoDizhu[i] == EJIao)
		{
			return EWait;
		}
		else if(m_jiaoDizhu[i] == EQiang)
		{
			qiangCount += 1;
		}
		else if(m_jiaoDizhu[i] == EReady)
		{
			readyCount += 1;
		}
		else if(m_jiaoDizhu[i] == ERefuse)
		{
			refuseCount += 1;
		}
	}

	if(qiangCount > 1 || (qiangCount == 1 && readyCount > 0))	//都在抢，或者有抢有ready
	{
		return EWait;
	}

	if(qiangCount == 1 && refuseCount == 2)	//只有一个就绪的
	{
		return EOk;
	}

	if(readyCount > 0)
	{
		return EOk;
	}
	else
	{
#if USEASSERT
		assert(refuseCount == m_pDesk->m_nUserCounts);
#endif
		return EAllFail;
	}
}




void GameHandler_DDZ_Classical::DecideDizhu(Lint pos)
{
#if USEASSERT
	assert(pos >=0 && pos < m_pDesk->m_nUserCounts);
#endif

	m_nDiZhuPos = pos;
	LMsgS2CWhoIsDiZhu dizhuposMsg;
	dizhuposMsg.m_pos = m_nDiZhuPos;
	dizhuposMsg.m_difen = m_nDifen;
	for(size_t i = 0 ; i < m_leftCard.size(); ++i)
	{
		dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
		m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
	}
	CardStyle::Instance()->SortCard(m_handCard[m_nDiZhuPos]);
	BoadCast(dizhuposMsg);
	

	//先将抢地主记入录像;
	// 记入录像;
	std::vector<Lint> vec;
	for(Lsize i = 0 ; i < m_leftCard.size(); ++i)
	{
		vec.push_back(m_leftCard[i]->GetIndex());
	}
	m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI,m_nDiZhuPos,vec);
	//开始打牌;
	SetDeskPlay();
}

void GameHandler_DDZ_Classical::ReStartJiaoDizhu(Lint pos)
{
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

	StartQiangDizhu();
}

void GameHandler_DDZ_Classical::RecordVideoQiangDizhu(Lint pos, bool isQiang, const std::vector<Lint> & vec)
{
#if USEASSERT
	assert(pos >= 0 && pos < 3);
#endif

	JiaoState state = m_jiaoDizhu[pos];
	Lint subQiangType = INVAILD_VAL;

	if(isQiang)	//抢
	{
		if(state == EQiang)
		{
			subQiangType = VIDEO_OPER_QIANGDIZHU;
		}
		else if(state == EJIao)
		{
			subQiangType = VIDEO_OPER_JIAODIZHU;
		}
	}
	else //不抢
	{
		if(state == EQiang)
			subQiangType = VIDEO_OPER_BUQIANG;
		else if(state == EJIao)
			subQiangType = VIDEO_OPER_BUJIAODIZHU;
	}

#if USEASSERT
	assert(subQiangType != INVAILD_VAL);
#endif

	m_pDesk->m_video.AddOper(subQiangType, pos, vec);
}

void GameHandler_DDZ_Classical::BroadCastJiaoDizhu(Lint pos, BYTE yaobu, BYTE restart)
{
	//广播玩家选择结果;
	LMsgS2CAnsDiZhu AnsMsg;
	AnsMsg.m_pos = pos;
	AnsMsg.m_yaobu = yaobu; //叫不叫
	AnsMsg.m_difen = m_nDifen;
	AnsMsg.m_restart = restart;
	//AnsMsg.m_state = m_jiaoDizhu[pos];
	
	BoadCast(AnsMsg);
}



///////////////////////////////////////////// 调用desk函数; //////////////////////////////////////////////////