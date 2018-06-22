#ifndef    _DDZ_HANDLER_ARENAPVE_H_
#define    _DDZ_HANDLER_ARENAPVE_H_

#include "GameHandler.h"
#include "Desk.h"
#include "gr_card_manager_ArenaPVE.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

#define USER_COUNT_ARENAPVE (3)
class GameHandler_ARENAPVE : public GameHandler
{
public:
	bool startup(Desk *desk);
	void SetDeskDeal();
	void QiangDiZhu(Lint nPos);
	void HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg);
	void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	void SetDeskPlay();
	bool OnUserReconnect(User* pUser);
	void dealCard_new();
	void ProcessRobot(Lint pos, User * pUser);

	//////////////////////////////////////////////////////////////////////////
protected:
	void clearHandler(void);
	void _clearGame();
	Lint getDeskState();

	void NotPlayCard(Lint pos);
	void PlayCard(Lint pos, CardVec& card, Lchar card_mode, Lchar card_type);
	void CheckPlayState(Lint pos);
	bool CheckPlayEnd(Lint pos);
	void SetDeskEnd(Lchar pos);
	void SendPlayInfo(bool islast = false);
	void SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec& vec, bool isFirstOut = false);
	void QueryUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nPara1);
	void BoadCastUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nOperRel, Lint isRestart = 0);//blank备用选项;
	void GetLaiZi();

	///////////////////////////////////////////// 调用desk函数; //////////////////////////////////////////////////
protected:
	void BoadCast(LMsg& msg){ m_pDesk->BoadCast(msg); }
	void BoadCastWithOutUser(LMsg& msg, User* user){ m_pDesk->BoadCastWithOutUser(msg, user); }
	Lint GetUserPos(User* user){ return m_pDesk->GetUserPos(user); }
	Lint GetNextPos(Lint m_curPos){ return m_pDesk->GetNextPos(m_curPos); }
	Lint GetPrevPos(Lint m_curPos){ return m_pDesk->GetPrevPos(m_curPos); }

	void SetDeskWait(){ m_pDesk->SetDeskWait(); }
	void ClearUser(){ m_pDesk->ClearUser(); };
	void SetDeskFree(){m_pDesk->SetDeskFree();}
	void ResetClear(){m_pDesk->ResetClear();}


private:
	Desk* m_pDesk;

private:
	Lint            m_nCardCounts;

	Lint            m_nDiZhuPos;       //地主位置;
	Lint            m_nDifen;          //游戏底分;
	Lint            m_nDizhuPlayCounts;//地主出牌次数，用于计算农民春天;
	Lint            m_nQiangDizhuState;//搶地主階段，0非搶地主階段；1悶抓階段；2抓不抓階段，3倒階段；4拉階段;

	CardVec         m_putCard;//出去的牌;
	CardVec         m_handCard[USER_COUNT_ARENAPVE];//手上的牌 ;
	CardVec         m_leftCard;//斗地主底牌;

	CardVec			m_outCard[USER_COUNT_ARENAPVE];

	Lchar           m_playerCardType[USER_COUNT_ARENAPVE];//出牌类型;
	Lchar           m_curCardType;
	Lchar           m_curCardMode;
	Lchar           m_check;	//没有上家（上轮出完;
	Lchar			m_curPlayCount;
	Lchar           m_win;

	Lchar           m_bombCount;//炸弹数量;
	Lchar			m_firstPos;// 保存先手位置,随机先手;
	Lint            m_bJiaoOrNo[USER_COUNT_ARENAPVE]; //首先玩家是否叫了地主，以此判断该玩家是否可以抢地主;

	Lint			m_bomb[USER_COUNT_ARENAPVE];
	Lint            m_nUserBeishu;

	CardDeal_ArenaPVE		m_dealCard;

	Lint			m_curPos;//当前操作玩家;
	Lint			m_playPos;	//出牌玩家;
	Lint            m_laiziCard; //皮子牌的原本牌面值;

	Llong			m_ticktime;

	CardHelp_ArenaPVE		m_help;
	std::vector<std::vector<Lint>>			m_curHelpInfo;
	std::vector<Lint>                       m_curHelpUnusable;
	Lint			m_baojing[USER_COUNT_ARENAPVE];
};

DECLARE_GAME_HANDLER_CREATOR(game_Arena_2V1_PVE, GameHandler_ARENAPVE)

#endif