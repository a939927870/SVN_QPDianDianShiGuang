#ifndef    _DDZ_HANDLER_2PLAYERS_H_
#define    _DDZ_HANDLER_2PLAYERS_H_

#include "GameHandler.h"
#include "Desk.h"
#include "gr_card_manager.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

#define USER_COUNT_2PERS (2)
class GameHandler_DDZ_2Players : public GameHandler
{
public:
	bool startup(Desk *desk);
	void SetDeskDeal();
	void CheckSameIP();
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
	void QueryUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType);
	void BoadCastUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nOperRel, Lint isRestart = 0);//blank备用选项;

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
	//User*			m_user[USER_COUNT_2PERS];
	//Lint            m_nUserCounts;
	Lint            m_nCardCounts;

	//	PK_GAME_STYLE   m_gameStyle;      // 游戏类型;
	Lint            m_nFirstCardIndex;  // 确定庄家位置的牌面索引;没有弃牌时为黑桃3，有弃牌时看所有玩家手牌中所有黑桃中最小的一张;
	Lint            m_nDiZhuPos;       //地主位置;
	//	Lint            m_nMaxFanshu;      //游戏最大翻倍数;
	Lint            m_nDifen;          //游戏底分;
	Lint            m_nDizhuPlayCounts;//地主出牌次数，用于计算农民春天;
	Lint            m_nQiangDizhuState;//搶地主階段，0非搶地主階段；1悶抓階段；2抓不抓階段，3倒階段；4拉階段;

	Lint            m_nDaocishu;
	Lint            m_firstPosCard; //二人斗地主需一张名牌，拿到该名牌玩家首先进行抢地主操作;

	CardVec         m_putCard;//出去的牌;
	CardVec         m_handCard[USER_COUNT_2PERS];//手上的牌 ;
	CardVec         m_leftCard;//斗地主底牌;

	CardVec			m_outCard[USER_COUNT_2PERS];

	Lchar           m_playerCardType[USER_COUNT_2PERS];//出牌类型;
	Lchar           m_curCardType;
	Lchar           m_curCardMode;
	Lchar           m_check;	//没有上家（上轮出完;
	Lchar			m_curPlayCount;
	Lchar           m_win;
	//	Lchar           m_readyState[USER_COUNT_2PERS];//玩家准备状态;

	Lchar           m_bombCount;//炸弹数量;
	Lchar			m_firstPos;// 保存先手位置,第一把为房主，其后为上局赢家;

	Lint			m_bomb[USER_COUNT_2PERS];
	Lint            m_nUserBeishu;

	CardDeal		m_dealCard;

	//	Lint			m_deskState;//桌子状态;

	Lint			m_curPos;//当前操作玩家;

	Llong			m_ticktime;

	//VipLogItem*		m_vip;

	//Lint			m_reset[USER_COUNT_2PERS];
	//Lstring			m_resetUser;//申请的玩家;
	//Lint			m_resetTime;//申请解算的时间;

	//Lint            m_black3first;     //第一轮出牌玩家是否可以不带黑桃3，1为可以不带，0为必须带黑桃3;
	//Lint            m_tiaopaishu;       //测试玩家调牌张数，0代表不调牌;
	//CardValue       m_specialCard[56];  //存储玩家特定发牌牌局;

	//VideoLog		m_video;//录像;

	CardHelp		m_help;
	std::vector<std::vector<Lint>>			m_curHelpInfo;
	std::vector<Lint>                       m_curHelpUnusable;
	Lint			m_baojing[USER_COUNT_2PERS];
};

DECLARE_GAME_HANDLER_CREATOR(game_ddz_2players, GameHandler_DDZ_2Players)

#endif