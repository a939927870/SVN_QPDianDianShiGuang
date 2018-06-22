#ifndef    _DDZ_HANDLER_CLASSICAL_H_
#define    _DDZ_HANDLER_CLASSICAL_H_

#include "GameHandler.h"
#include "Desk.h"
#include "gr_card_manager_shanxi_wakeng.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

#define USER_COUNT_CLASSICAL (3)

class DDZHandler_ShanxiWakeng : public GameHandler
{
public:
	DDZHandler_ShanxiWakeng();
	bool startup(Desk *desk);
	void SetDeskDeal();
	void CheckSameIP();
	void QiangDiZhu(Lint nPos);
	void HanderUserQiangDiZhu(User* pUser,LMsgC2SQiangDiZhu* msg);
	void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	void SetDeskPlay();
	bool OnUserReconnect(User* pUser);
	void dealCard_new();
	void ProcessRobot(Lint pos, User * pUser);

	//3人抢地主状态
	enum UsersJiaoState
	{
		EWait,
		EOk,
		EAllFail,
	};
	enum JiaoState
	{
		EJIao,
		EQiang,
		EReady,
		ERefuse,
	};

	enum ClassicalSubType
	{
		EHeiwa,
		EJiaoFen,
		//EJIaoDizhu,
	};

	//////////////////////////////////////////////////////////////////////////
protected:
	void StartQiangDizhu();
	
	void clearHandler(void);
	void _clearGame();
	Lint getDeskState();

	void NotPlayCard(Lint pos);
	void PlayCard(Lint pos, CardVec_Wakeng& card, Lchar card_mode, Lchar card_type);
	void CheckPlayState(Lint pos);
	bool CheckPlayEnd(Lint pos);
	void SetDeskEnd(Lchar pos);
	void SetGoldDeskEnd(Lchar pos);
	void SendPlayInfo(bool islast = false);
	void SendPlayCard(Lchar pos, Lchar ret, Lchar action, CardVec_Wakeng& vec, bool isFirstOut = false);

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

	//
	//不同分支
	void HanderUserWakengOfHeiwa(User* pUser,LMsgC2SQiangDiZhu* msg); //
	void HanderUserWakengOfJiaofen(User* pUser,LMsgC2SQiangDiZhu* msg);	//叫分
	//void HanderUserQiangDiZhuOfJiaoDizhu(User* pUser,LMsgC2SQiangDiZhu* msg);	//叫地主

	//开始抢地主流程(不同分支）
	void StartWakengOfHeiwa();
	void StartWakengOfJiaofen();
	//void StartQiangDizhuOfJiaoDizhu();


	//
	Lint GetNextValidPos(Lint pos);
	UsersJiaoState CheckUsersJiaoDizhuState();	//检查是否完成叫地主过程
	UsersJiaoState CheckUsersHeiwaState();  //检查是否完成黑挖过程
	void DecideDizhu(Lint pos);
	void DecideWhoisFirst();
	void ReStartJiaoDizhu(Lint pos);
	void BroadCastJiaoDizhu(Lint pos, BYTE yaobu, BYTE restart);	//广播当前玩家叫地主
	void RecordVideoQiangDizhu(Lint pos, bool isQiang, const std::vector<Lint> & vec = std::vector<Lint>());
	void ShowCard2User(Lint nCurPos);
	void ShowCard2UserAll();
	inline bool IsUseBomb();

private:
	Desk* m_pDesk;

private:
	//User*			m_user[USER_COUNT_CLASSICAL];
	//Lint            m_nUserCounts;
	Lint            m_nCardCounts;

//	PK_GAME_STYLE   m_gameStyle;      // 游戏类型;
	Lint            m_nFirstCardIndex;  // 确定庄家位置的牌面索引;没有弃牌时为黑桃3，有弃牌时看所有玩家手牌中所有黑桃中最小的一张;
	Lint            m_nDiZhuPos;       //地主位置;
	//Lint            m_nMaxFanshu;      //游戏最大翻倍数;
	Lint            m_nDifen;          //游戏底分;
	Lint            m_nDizhuPlayCounts;//地主出牌次数，用于计算农民春天;
	Lint            m_nQiangDizhuState;//搶地主階段，0非搶地主階段；1黑挖；2叫分

	Lint            m_menzhua[USER_COUNT_CLASSICAL];
	Lint            m_daobudao[USER_COUNT_CLASSICAL];
	Lint            m_labula[USER_COUNT_CLASSICAL];
	Lint            m_nUserBeishu[USER_COUNT_CLASSICAL];
	JiaoState		m_jiaoDizhu[USER_COUNT_CLASSICAL];	//叫地主阶段的状态 0：叫状态 1：抢状态 2：准备好， 3：放弃
	JiaoState		m_heiwa[USER_COUNT_CLASSICAL];	//叫地主阶段的状态 0：叫状态 2：准备好， 3：放弃

	CardVec_Wakeng         m_putCard;//出去的牌;
	CardVec_Wakeng         m_handCard[USER_COUNT_CLASSICAL];//手上的牌 ;
	CardVec_Wakeng         m_leftCard;//斗地主底牌;

	CardVec_Wakeng			m_outCard[USER_COUNT_CLASSICAL];

	Lchar           m_playerCardType[USER_COUNT_CLASSICAL];//出牌类型;
	Lchar           m_curCardType;
	Lchar           m_curCardMode;
	Lchar           m_check;
	Lchar			m_curPlayCount;
	Lchar           m_win;
//	Lchar           m_readyState[USER_COUNT_CLASSICAL];//玩家准备状态;

	Lchar           m_bombCount;//炸弹数量;
	Lchar			m_firstPos;// 保存先手位置,第一把为房主，其后为上局赢家;

	Lint			m_bomb[USER_COUNT_CLASSICAL];

	CardDeal_Shanxi_Wakeng		m_dealCard;

//	Lint			m_deskState;//桌子状态;

	Lint			m_curPos;//当前操作玩家;

	Llong			m_ticktime;

	//VipLogItem*		m_vip;

	//Lint			m_reset[USER_COUNT_CLASSICAL];
	//Lstring			m_resetUser;//申请的玩家;
	//Lint			m_resetTime;//申请解算的时间;

	//Lint            m_black3first;     //第一轮出牌玩家是否可以不带黑桃3，1为可以不带，0为必须带黑桃3;
	//Lint            m_tiaopaishu;       //测试玩家调牌张数，0代表不调牌;
	//CardValue       m_specialCard[56];  //存储玩家特定发牌牌局;

	//VideoLog		m_video;//录像;

	CardHelp_Shanxi_Wakeng		m_help;
	std::vector<std::vector<Lint>>			m_curHelpInfo;
	std::vector<Lint>                       m_curHelpUnusable;
	Lint			m_baojing[USER_COUNT_CLASSICAL];

	//红四等先手位置
	Lint			m_nHongFirstIndex = 0;
	bool			m_bIsHongsi = false;

	ClassicalSubType m_subType;		//0, 黑挖， 1：叫分
};

DECLARE_GAME_HANDLER_CREATOR(game_wakeng_shanxi, DDZHandler_ShanxiWakeng)

#endif