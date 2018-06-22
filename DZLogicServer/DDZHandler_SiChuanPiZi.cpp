#include "DDZHandler_SiChuanPiZi.h"
#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include <cmath>

bool GameHandler_DDZ_SiChuanPiZi::startup(Desk *desk)
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

void GameHandler_DDZ_SiChuanPiZi::clearHandler(void)
{
	_clearGame();
	m_firstPos = 0;   //需要保留信息到下一局，不能初始化;
	m_nDifen = 1; // 游戏底分默认为1;
}

//初始化每局游戏，与初始化桌子有区别，有信息需要保留;
void GameHandler_DDZ_SiChuanPiZi::_clearGame()
{
	m_score.Reset();

	m_dianshu = INVAILD_VAL;
	m_pokerPoint = INVAILD_VAL;
	m_pokerStyle = INVAILD_VAL;
	m_lightPokerIndex = INVAILD_VAL;	//明牌
	m_dizhuFriendPokerIndex = INVAILD_VAL;	//地主腿子的牌
	m_nTuiZiPos = INVAILD_VAL;	//地主腿子位置
	m_springType = EInvalid,

	m_curPos = 0;
	m_playPos = -1;
	m_ticktime = gWork.GetCurTime().MSecs();
	for (Lint i = 0; i < USER_COUNT_PiZi; ++i)
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

void GameHandler_DDZ_SiChuanPiZi::SetDeskDeal()
{
	if (m_pDesk && m_pDesk->m_vip)
		m_pDesk->m_vip->SendInfo();

	//发牌
	dealCard_new();

	//录像记录
	Lint userId[USER_COUNT_PiZi] = {0};
	Ldouble score[USER_COUNT_PiZi]={0};
	
	std::vector<Lint> vec[USER_COUNT_PiZi];
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
	
#if USEASSERT
	assert(m_pDesk->m_extValue.size() > 0);
#endif
	if(m_pDesk->m_extValue.size() > 0)
	{
		vecGameType.push_back(game_para_diOrFan);
		vecGameType.push_back(m_pDesk->m_extValue[0]);	//加底加翻
	}
	
	m_pDesk->m_video.DealCard(userId,vec,gWork.GetCurTime().Secs(),0,score,m_pDesk->m_id, m_pDesk->m_vip->m_curCircle, m_pDesk->m_vip->m_maxCircle, m_pDesk->m_gameStyle, vecGameType);
	m_curHelpInfo.clear();
	m_curHelpUnusable.clear();
	m_curPos = m_firstPos;
	
	m_pDesk->m_deskState = QIANG_DIZHU;

	TellNext(3);
}

void GameHandler_DDZ_SiChuanPiZi::CheckSameIP()
{
	if (m_pDesk->m_nUserCounts != 3)
	{
		return;
	}
	Lstring szUserIP[4];
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

void GameHandler_DDZ_SiChuanPiZi::QiangDiZhu(Lint nPos)
{
	LMsgS2CAskDiZhu msg;
	msg.m_pos = nPos;
	msg.m_difen = 0;
	BoadCast(msg);
	return;
}

void GameHandler_DDZ_SiChuanPiZi::HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg)
{
#if USEASSERT
	assert(m_pDesk);
	assert(msg);
#endif

	if (m_pDesk->m_deskState != QIANG_DIZHU)
	{
		LLOG_ERROR("SiChuanPiZI:not QiangDIzhu Time");
		return;
	}
	if (GetUserPos(pUser) != msg->m_pos)
	{
		LLOG_ERROR("user position is not msg' position: %d!", msg->m_pos );
		return;
	}
	if (m_curPos == msg->m_pos)
	{
		if (3 == msg->m_operType )  //倒;
		{
			HanderDao(msg->m_playerOper, msg->m_pos);
		}
		else if(5 == msg->m_operType)	//花色
		{
			HanderHuaSe(msg->m_playerOper);
		}
		else if(6 == msg->m_operType)
		{
			HanderDizhuTouShaizi();
		}
	}
	else   //错误消息;
	{
	}
}

void GameHandler_DDZ_SiChuanPiZi::HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg)
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
			CardStyle_SiChuanPiZi::Instance()->SortCard(tmp);


			CardStyleNumberVec style;

			//癞子奇异牌判断;
			if(card_style_error != msg->m_style)
			{
				BYTE nStype = msg->m_style;
				BYTE cardNumber = card_number_type_null;
				
				if(CardStyle_SiChuanPiZi::Instance()->CheckCardStyle(tmp, nStype, cardNumber))
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
				CardStyle_SiChuanPiZi::Instance()->CheckCardStyle(tmp,style,tmp.size() == m_handCard[pos].size(), m_curCardMode);
			}

			//判断能不能出;
			int index = CardStyle_SiChuanPiZi::Instance()->CanBig(m_curPlayCount, m_curCardMode, m_curCardType, style);
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
Lint GameHandler_DDZ_SiChuanPiZi::getDeskState()
{
	return m_pDesk->m_deskState;
}

void GameHandler_DDZ_SiChuanPiZi::NotPlayCard(Lint pos)
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

void GameHandler_DDZ_SiChuanPiZi::PlayCard(Lint pos, CardVec& card, Lchar card_mode, Lchar card_type)
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
	CardStyle_SiChuanPiZi::Instance()->EarseCard(m_handCard[pos],card);
	m_putCard.insert(m_putCard.end(),card.begin(),card.end());
	m_outCard[pos] = card;

	//m_outCard[GetPrevPos(pos)].clear();
	CheckPlayState(pos);
}

void GameHandler_DDZ_SiChuanPiZi::CheckPlayState(Lint pos)
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
		CardStyle_SiChuanPiZi::Instance()->CheckCardStyle(m_handCard[m_curPos], style, true);
		if (style.size() > 0)  // 检测能否一手出完,能够一手出完;
		{
			islast = true;
			if (m_handCard[m_curPos].size() > 4 && ( CardStyle_SiChuanPiZi::Instance()->CheckHasBomb(m_handCard[m_curPos]) || CardStyle_SiChuanPiZi::Instance()->HasRocket(m_handCard[m_curPos]) ) )
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
			CardStyle_SiChuanPiZi::Instance()->GetLaizi(m_handCard[m_curPos], laizi, normal);
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

bool GameHandler_DDZ_SiChuanPiZi::CheckPlayEnd(Lint pos)
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
		for (Lint i = 0; i < USER_COUNT_PiZi; ++i)   // 包含弃牌堆信息;
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

void GameHandler_DDZ_SiChuanPiZi::SetDeskEnd(Lchar pos)
{
	Lint win[USER_COUNT_PiZi] = {0};
	if (pos < m_pDesk->m_nUserCounts && m_pDesk->m_nUserCounts >= 0)
	{
		//胜负局数;
		for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
		{
			if(pos == m_nDiZhuPos || pos == m_nTuiZiPos)
			{
				win[m_nDiZhuPos] = 1;
				win[m_nTuiZiPos] = 1;
				break;
			}
			else if(i != m_nTuiZiPos && i != m_nDiZhuPos)
			{
				win[i] = 1;
			}
		}
	}
	
	LMsgS2CGameOver pack;
	pack.m_win =  (pos);

	Ldouble gold[USER_COUNT_PiZi] = {0};

	GetScore(pos, gold);

	//数据填充;
	Lint chuntian[USER_COUNT_PiZi] = {0};
	for (Lint i = 0 ; i < m_pDesk->m_nUserCounts; ++i)
	{
		pack.m_count[i] = m_handCard[i].size();
		pack.m_nike[i] = m_pDesk->m_user[i]->m_userData.m_nike;

		if (i == pos && m_springType >= EDizhuSpring)
		{
			chuntian[i] = 1;
		}
	}
	for (Lint i = 0; i < USER_COUNT_PiZi; ++i)
	{
		gold[i] = gold[i] * m_pDesk->GetStake();
	}
	memcpy(pack.m_score,gold,sizeof(gold));
	memcpy(pack.m_bomb,m_bomb,sizeof(m_bomb));
	memcpy(pack.m_chuntian, chuntian, sizeof(chuntian));
	memset(m_pDesk->m_readyState,0,sizeof(m_pDesk->m_readyState));

	m_curPos = pos;

	//（春）反春录像记录;
	if(m_springType >= EDizhuSpring)
	{
		Lint videoChunType = m_springType == EDizhuSpring? VIDEO_OPER_CHUNTIAN : VIDEO_OPER_FANCHUN ;
		std::vector<Lint> vec;
		m_pDesk->m_video.AddOper(videoChunType, pos, vec);
	}

	Ldouble cal_gold[4] = { 0,0,0,0 };

	if (m_pDesk->m_video.m_time > 0)   // 防止未开始的局记入录像;
	{
		//保存录像;
		m_pDesk->m_video.m_Id = gVipLogMgr.GetVideoId();
		m_pDesk->m_vip->AddLog(m_pDesk->m_user, gold, cal_gold, win, m_bomb, m_pDesk->m_video.m_Id);

		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		m_pDesk->m_video.SetEveryResult(4, gold[0], gold[1], gold[2], gold[3]);
		m_pDesk->m_video.SetEveryResult2(4, cal_gold[0], cal_gold[1], cal_gold[2], cal_gold[3]);
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

void GameHandler_DDZ_SiChuanPiZi::SetDeskPlay()
{
	m_pDesk->m_deskState = DESK_PLAY;

	for(Lsize i = 0 ; i < USER_COUNT_PiZi; ++i)
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

bool GameHandler_DDZ_SiChuanPiZi::OnUserReconnect(User* pUser)
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
	pack2.m_Int_1.push_back(m_dizhuFriendPokerIndex);	//管家牌;
	pack2.m_Int_1.push_back(m_score.GetTurnCount());	//当前翻数;
	if(pos == m_nTuiZiPos)
	{
		pack2.m_Int_1.push_back(m_nTuiZiPos);				//地主管家pos;
	}
	//end
	
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
	for(size_t i = 0 ; i < m_handCard[pos].size(); ++i) //自己手牌;
	{	
		pack2.m_card.push_back(m_handCard[pos][i]->GetIndex());
	}
	if (m_pDesk->m_deskState == QIANG_DIZHU)
	{
		pack2.m_diZhuPos = m_nDiZhuPos;
	}
	else
	{
		if(pos == m_nDiZhuPos)
		{
			for (size_t i = 0; i < m_leftCard.size(); ++i)
			{
				pack2.m_dipai.push_back(m_leftCard[i]->GetIndex()); 
			}
		}
		
	}

	Lint turnCount = m_score.GetScore();
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
		pack2.m_nUserBeishu[i] = turnCount;
	}

	pUser->Send(pack2);

	if (QIANG_DIZHU == m_pDesk->m_deskState)  //抢地主期间断线重连;
	{
		LMsgS2CQDZQuery_Sichuan msg;
		msg.m_pos = m_curPos;
		msg.m_operType = m_nQiangDizhuState;
		msg.m_dianshu = m_dianshu;
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

void GameHandler_DDZ_SiChuanPiZi::SendPlayInfo(bool islast)
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

void GameHandler_DDZ_SiChuanPiZi::SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec& vec, bool isFirstOut)
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

void GameHandler_DDZ_SiChuanPiZi::dealCard_new()
{
#if USEASSERT
	assert(m_pDesk);
#endif
	if(!m_pDesk)
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
	m_nCardCounts = 13;
	if (1) //0 == m_gameStyle || 3 == m_gameStyle;
	{
		m_dealCard.DealCard(m_handCard, 4, m_pDesk->m_specialCard, m_pDesk->m_tiaopaishu);
		m_dealCard.GetLeftCard(m_leftCard);   //剩余牌消息;
	}

	//发牌前决定地主等信息
	DecideDizhu();

	//发牌&整理（13张）
	CardStyle_SiChuanPiZi::Instance()->SortCard(m_handCard[0]);
	CardStyle_SiChuanPiZi::Instance()->SortCard(m_handCard[1]);
	CardStyle_SiChuanPiZi::Instance()->SortCard(m_handCard[2]);
	CardStyle_SiChuanPiZi::Instance()->SortCard(m_handCard[3]);

	for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
	{
		if(m_pDesk->m_user[i])
		{
			LMsgS2CPlayStart msg;

			msg.m_hpos = m_nDiZhuPos;	//地主位置
			
			msg.m_gameStyle = (Lint)m_pDesk->m_gameStyle;
			for(size_t j = 0 ; j < m_handCard[i].size(); ++j)
			{
				msg.m_card.push_back(m_handCard[i][j]->GetIndex());
			}
			m_pDesk->m_user[i]->Send(msg);
		}
	}
}

void GameHandler_DDZ_SiChuanPiZi::DoBomb(Lint pos)
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
bool GameHandler_DDZ_SiChuanPiZi::IsPlayByFilter( Lint type, bool last){

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


bool GameHandler_DDZ_SiChuanPiZi::IsAllUserDone(User::UserRoomState state)
{
#if USEASSERT
	assert(m_pDesk);
#endif

	for(int i = 0; i < m_pDesk->m_nUserCounts; i++)
	{
		if(m_pDesk->m_user[i])
		{
			if(!m_pDesk->m_user[i]->GetDone(state))
			{
				return false;
			}
		}
	}

	return true;
}

void GameHandler_DDZ_SiChuanPiZi::BoadCastUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nOperRel)
{
	//UpdateUserBeishu();
	if (MSG_S_2_C_QDZOPER_NEW == nMsgID)
	{
		LMsgS2CQdzOper_Sichuan msg;
		msg.m_pos = nCurPos;
		msg.m_operType = nOperType;
		msg.m_playerOper = nOperRel;

		Lint turnCount = m_score.GetScore();
		for (Lint i = 0; i  < m_pDesk->m_nUserCounts; ++i)
		{
			msg.m_nUserBeishu[i] = turnCount;
		}
		
		BoadCast(msg);
	}
	std::vector<Lint> vec;
	m_pDesk->m_video.AddOper( nOperType*100 + nOperRel, nCurPos,vec);
}


void GameHandler_DDZ_SiChuanPiZi::QueryUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType)
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

void GameHandler_DDZ_SiChuanPiZi::ProcessRobot(Lint pos, User * pUser)
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
			if(m_nQiangDizhuState == 5)
			{
				msg.m_playerOper = L_Rand(1, 4);
			}
			else
			{
				msg.m_playerOper = rand()%2;
			}

			HanderUserQiangDiZhuOper(pUser, &msg);
		}
		break;
	}
}

//农民春天，地主反春
bool GameHandler_DDZ_SiChuanPiZi::IsNongMinSpring(Lint winPos)	
{
	if (winPos == m_nDiZhuPos || winPos == m_nTuiZiPos)
	{
		return false;
	}

	bool isSpring = m_nDizhuPlayCounts == 1;
	
	if(m_nDiZhuPos != m_nTuiZiPos)
	{
#if USEASSERT
		assert(m_nTuiZiPos != INVAILD_VAL);
#endif

		isSpring = (isSpring && m_handCard[m_nTuiZiPos].size() == m_nCardCounts); //管家不能出牌
	}

	if(isSpring)
	{
		m_springType = ENongminSpring;
	}

	return isSpring;
}

//地主是否春天
bool GameHandler_DDZ_SiChuanPiZi::IsLandlordSpring(Lint winPos)	
{
	if (winPos != m_nDiZhuPos && winPos != m_nTuiZiPos) //地主没胜利;
	{
		return false;
	}

	std::vector<BYTE> vecDizhu;
	std::vector<BYTE> vecNongmin;

#if USEASSERT
	assert(m_nTuiZiPos != INVAILD_VAL);
#endif

	for(int i = 0; i< m_pDesk->m_nUserCounts; ++i)
	{
		if(m_pDesk->m_user[i])
		{
			if(i == m_nDiZhuPos || i == m_nTuiZiPos)
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

void GameHandler_DDZ_SiChuanPiZi::GetScore(Lint winPos, Ldouble * gold)
{
	if (winPos == m_nDiZhuPos || winPos == m_nTuiZiPos) //地主胜利;
	{
#if USEASSERT
		assert(m_nTuiZiPos != INVAILD_VAL);
		assert(m_nDiZhuPos != INVAILD_VAL);
#endif

		//
		bool isDizhuSpr = IsLandlordSpring(winPos);
		if(isDizhuSpr)	//地主春天
		{
			m_score.AddBomb(1);
		}

		//
		if(m_nDiZhuPos == m_nTuiZiPos) //一打三
		{


			int score = m_score.GetScore();
			int scoreWin = score * 3;
#if USEASSERT
			assert(score > 0);
#endif

			gold[m_nDiZhuPos] = scoreWin;
			for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
			{
				if(i != m_nDiZhuPos && m_pDesk->m_user[i])
				{
					gold[i] = - score;
				}
			}
		}
		else	//二打二
		{
			int score = m_score.GetScore();
#if USEASSERT
			assert(score > 0);
#endif

			gold[m_nDiZhuPos] = score;
			gold[m_nTuiZiPos] = score;

			for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
			{
				if((i != m_nDiZhuPos && i != m_nTuiZiPos) && m_pDesk->m_user[i])
				{
					gold[i] = - score;
				}
			}
		}
	}
	//农民胜利;
	else if(winPos != INVAILD_POS)
	{
#if USEASSERT
		assert(m_nTuiZiPos != INVAILD_VAL);
		assert(m_nDiZhuPos != INVAILD_VAL);
#endif

		m_firstPos = winPos;

		if (IsNongMinSpring(winPos)) //地主只出牌一次,农民春天;
		{
			m_score.AddBomb(2);
		}
		gold[m_nDiZhuPos] = 0;
	

		int score = m_score.GetScore();

#if USEASSERT
		assert(score > 0);
#endif

		if(m_nDiZhuPos == m_nTuiZiPos)	//一打三
		{
			int score = m_score.GetScore();
			int scoreLost = score * 3; 

#if USEASSERT
			assert(score > 0);
#endif

			gold[m_nDiZhuPos] = -scoreLost;
			for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
			{
				if(i != m_nDiZhuPos && m_pDesk->m_user[i])
				{
					gold[i] = +score;
				}
			}
		}
		else	//2打2
		{
			int score = m_score.GetScore();

#if USEASSERT
			assert(score > 0);
#endif

			gold[m_nDiZhuPos] = -score;
			gold[m_nTuiZiPos] = -score;

			for(int i = 0; i < m_pDesk->m_nUserCounts; ++i)
			{
				if((i != m_nDiZhuPos && i != m_nTuiZiPos) && m_pDesk->m_user[i])
				{
					gold[i] = + score;
				}
			}
		}
	}
	else	{}  // pos == INVAILD_POS, do noting;
}

void GameHandler_DDZ_SiChuanPiZi::SetAllUserRoomState(User::UserRoomState state)
{
	for(int i = 0; i < 4; i++)
	{
		if(m_pDesk->m_user[i])
		{
			m_pDesk->m_user[i]->SetRoomState(state);
		}
	}
}

void GameHandler_DDZ_SiChuanPiZi::SetAllUserDone(User::UserRoomState state, bool isDone)
{
#if USEASSERT
	assert(m_pDesk);
#endif

	for(int i = 0; i < 4; i++)
	{
		if(m_pDesk->m_user[i])
		{
			m_pDesk->m_user[i]->SetDone(state, isDone);
		}
	}
}

void GameHandler_DDZ_SiChuanPiZi::SetUserDone(Lint pos, User::UserRoomState state, bool isDone)
{
#if USEASSERT
	assert(pos >= 0 && pos < 4);
#endif
	if(m_pDesk->m_user[pos])
	{
		m_pDesk->m_user[pos]->SetDone(state, isDone);
	}
}

bool GameHandler_DDZ_SiChuanPiZi::GetUserDone(Lint pos, User::UserRoomState state)
{
#if USEASSERT
	assert(pos >= 0 && pos < 4);
	assert(m_pDesk);
#endif

	if(m_pDesk->m_user[pos])
	{
		return m_pDesk->m_user[pos]->GetDone(state);
	}

	return false;
}

void GameHandler_DDZ_SiChuanPiZi::DecideDizhu()
{
	//-----------------------------------
	//确定谁是地主
	Lint dizhuIndex = L_Rand(0, 3);
	m_nDiZhuPos = dizhuIndex;
	m_firstPos = m_nDiZhuPos;

	//确定明牌（选地主的一张牌为名牌）
	Lint showPoker = INVAILD_VAL;
	for(Lint i = 0; i < m_nCardCounts ; ++i)
	{	
		auto & card = m_handCard[m_nDiZhuPos].at(i);
		if(!card->GetLaizi())
		{
			showPoker = card->GetIndex();
			break;
		}
	}

#if USEASSERT
	assert(showPoker != INVAILD_VAL);
#endif

	m_lightPokerIndex = showPoker; //记录明牌

	//确定点数
	Lint dianshu = L_Rand(2,12);
	m_dianshu = dianshu;
	m_pokerPoint = (dianshu - 2) > 0 ? (dianshu - 2) : (dianshu + 11);

	//------------------------------------------
}

void GameHandler_DDZ_SiChuanPiZi::TellNext(BYTE state)
{
#if USEASSERT
	assert(m_pDesk->m_deskState == QIANG_DIZHU);
#endif

	if(state == 3)
	{
		TellDao();
	} 
	else if( state == 5 )
	{
		TellHuaSe();
	}
	else if( state == 6 )
	{
		TellDizhuTouShaizi();
	}
	else
	{
#if USEASSERT
		assert(false);
#endif
	}
}

void GameHandler_DDZ_SiChuanPiZi::TellDao()
{
	BeforeTellDao();

	SetAllUserRoomState(User::EAcceptTurnOver);
	SetAllUserDone(User::EAcceptTurnOver, false);
	
	//进入倒不倒
#if USEASSERT
	assert(m_curPos == m_nDiZhuPos);	//
#endif

	m_curPos = GetNextPos(m_curPos);	//地主下家
	QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
}

void GameHandler_DDZ_SiChuanPiZi::TellHuaSe()
{
	SetAllUserRoomState(User::EHuaSe);
	SetAllUserDone(User::EHuaSe, false);

	//当前应该是地主
#if USEASSERT
	assert(m_curPos == m_nDiZhuPos);	//
#endif
	
	
	//QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_SHAIZI_HUASE);
	m_nQiangDizhuState = OPER_SHAIZI_HUASE;
	LMsgS2CQDZQuery_Sichuan msg;
	msg.m_pos = m_curPos;
	msg.m_operType = OPER_SHAIZI_HUASE;
	msg.m_dianshu = m_dianshu;
	BoadCast(msg);
}


void GameHandler_DDZ_SiChuanPiZi::TellDizhuTouShaizi()
{
	//进入地主掷骰子
#if USEASSERT
	assert(m_curPos == m_nDiZhuPos);	//
#endif
	QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DIZHU_SHAIZI);
}


void GameHandler_DDZ_SiChuanPiZi::DoPlay()
{
	//状态
	SetAllUserRoomState(User::EPlayStart);
	SetAllUserDone(User::EHuaSe, true);

	//悶抓不能拉，遊戲開始;
	SetDeskPlay();
	return;
}

void GameHandler_DDZ_SiChuanPiZi::HanderDao(Lint oper ,Lint pos)
{
	if(GetUserDone(m_curPos, User::EAcceptTurnOver))
	{
		LLOG_INFO("玩家倒过不允许再倒", pos);
		return;
	}
	SetUserDone(m_curPos, User::EAcceptTurnOver, true);


	//必倒牌判断
	bool isRocket = CardStyle_SiChuanPiZi::Instance()->HasRocket(m_handCard[m_curPos]);
	bool is2Count4Bomb = CardStyle_SiChuanPiZi::Instance()->Has2Count4Bomb(m_handCard[m_curPos]);

	//炸弹判断
	//bool isHasBomb	= CardStyle_SiChuanPiZi::Instance()->CheckHasBomb(m_handCard[m_curPos]);
	CardVec Normal,Laizi;
	CardStyle_SiChuanPiZi::Instance()->GetLaizi(m_handCard[m_curPos], Laizi, Normal);
	CardVec one, tow, three, four;
	CardStyle_SiChuanPiZi::Instance()->GetSingleDoubleThreeFour(Normal, one, tow, three, four);
	bool isHasBomb = four.size() >= 8 || (four.size()>=4 && Laizi.size() >= 1); 
	//end

	BYTE dao = 1; //倒1， 不到0
	if(1 == oper || isRocket || is2Count4Bomb || isHasBomb)
	{
		m_score.AddTrunOver(1);	//倒一次
		dao = 1;
		m_daobudao[m_curPos] = 1;
	} 
	else
	{
		dao = 0;
		m_daobudao[m_curPos] = 0;
	}

	BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DAO, dao);

	//全操作过
	if(IsAllUserDone(User::EAcceptTurnOver))
	{		
		//选花色
		TellNext(6);
	}
	else
	{
		m_curPos = m_pDesk->GetNextPos(m_curPos);
		QueryUserOper(m_curPos, MSG_S_2_C_QDZQUERY_NEW, OPER_DAO);
		return;
	}
}


void GameHandler_DDZ_SiChuanPiZi::HanderHuaSe(Lint oper)
{
	Lint style = oper;

	if(style > 0 && style <= 4)
	{
		m_pokerStyle = style;
		Card card;
#if USEASSERT
		assert(m_pokerPoint != INVAILD_VAL);
#endif
		card.SetColorAndNumber(style, m_pokerPoint);
		Lint pokerIndex = card.GetIndex();
		m_dizhuFriendPokerIndex = pokerIndex;


		//确定地主腿子的位置

		BYTE bHas = false;
		for(int i = 0; i < 4 ; ++i)
		{
			for(size_t j = 0; j < m_handCard[i].size(); ++j)
			{
				if(m_handCard[i][j]->GetIndex() == m_dizhuFriendPokerIndex)
				{
					m_nTuiZiPos = i;
					bHas = true;
					break;
				}
			}
		}
		if(m_curPos == m_nDiZhuPos)
		{
			for(size_t i = 0; i < m_leftCard.size(); ++i)
			{
				if(m_leftCard[i]->GetIndex() == m_dizhuFriendPokerIndex)
				{
					m_nTuiZiPos = m_curPos;
					bHas = true;
				}
			}
		}
#if USEASSERT
		assert(bHas);
#endif
		//end

		BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_SHAIZI_HUASE, pokerIndex);

		//记录录像
		std::vector<Lint> cards;
		cards.push_back(pokerIndex);
		m_pDesk->m_video.AddOper(VIDEO_OPER_GUANJIA, m_nTuiZiPos, cards);
	}
	else
	{
		//客户端上传数据错误
#if USEASSERT
		assert(false);
#endif
	}

	//添加地主牌
#if USEASSERT
	assert(m_nDiZhuPos != INVAILD_VAL);
#endif

	Lint dipaiSize = m_leftCard.size();
	for(int i = 0 ; i < dipaiSize ; ++i)
	{
		m_handCard[m_nDiZhuPos].push_back(m_leftCard[i]);
	}
	//end	
	
	//广播谁是地主
	BroadCastDizhu();

	DoPlay();
}

void GameHandler_DDZ_SiChuanPiZi::HanderDizhuTouShaizi()
{
#if USEASSERT
	assert(m_curPos == m_nDiZhuPos);	//
#endif

	//花色
	TellNext(5);
	//BoadCastUserOper(m_curPos, MSG_S_2_C_QDZOPER_NEW, OPER_DIZHU_SHAIZI, 0);
}

void GameHandler_DDZ_SiChuanPiZi::BroadCastDizhu()
{
#if USEASSERT
	assert(m_nDiZhuPos != INVAILD_VAL);
#endif

	//---------------------------------------------
	for(int nPos = 0; nPos < m_pDesk->m_nUserCounts; ++nPos)
	{
		//下发谁是地主
		LMsgS2CWhoIsDiZhu dizhuposMsg;
		dizhuposMsg.m_pos =  m_nDiZhuPos;
		dizhuposMsg.m_difen = 1; //msg->m_difen;

		//地主
		if(nPos == m_nDiZhuPos)
		{
			for(size_t i = 0 ; i < m_leftCard.size(); ++i)
			{
				dizhuposMsg.m_LeftCard.push_back(m_leftCard[i]->GetIndex());
			}
		}

		m_pDesk->m_user[nPos]->Send(dizhuposMsg);
	}	
	//end---------------------------------------

	//录像
	std::vector<Lint> cards;
	Lint leftSize = m_leftCard.size();
	for(int i = 0; i < leftSize; ++i)
	{
		cards.push_back(m_leftCard[i]->GetIndex());
	}
	m_pDesk->m_video.AddOper(VIDEO_OPER_DIPAI, m_nDiZhuPos, cards);
}

void GameHandler_DDZ_SiChuanPiZi::BeforeTellDao()
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

bool GameHandler_DDZ_SiChuanPiZi::IsLeftLaizi(const CardVec & out, Lint pos)
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


///////////////////////////////////////////// 调用desk函数; //////////////////////////////////////////////////