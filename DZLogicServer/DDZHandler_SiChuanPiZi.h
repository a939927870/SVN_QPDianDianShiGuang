#ifndef    _DDZ_HANDLER_SICHUANPIZI_H_
#define    _DDZ_HANDLER_SICHUANPIZI_H_

#include "GameHandler.h"
#include "Desk.h"
#include "gr_card_manager_sichuanpizi.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"

#define USER_COUNT_PiZi			(4)


//四川痞子计分逻辑
class Score	
{
public:
	enum PVPType 
	{
		EInvalid,
		E1V3,
		E2V2,
	};

	enum TurnOverType
	{
		ETInvalid,
		ETAddPoint, //加点(加底）
		ETTurn,	//翻倍
	};

	Score()
	{
		Reset();
	}
	~Score()
	{

	}

	void Reset()
	{
		m_type = ETInvalid;
		m_turnoverCountLimit = 0;
		m_currentTurnoverCount = 0;
		memset(m_userTurnOverCount, 0, sizeof(m_userTurnOverCount));
		m_baseScore = 0;
		m_pvpType = EInvalid;
		m_currentScore = 0;
	}
	void Init(BYTE type, BYTE turnOverCountLimit, Lint baseScore) 
	{
#if USEASSERT
		assert(type > 0);
#endif
		m_type = type;
		m_turnoverCountLimit = turnOverCountLimit;
		m_baseScore = baseScore;
		m_currentScore = m_baseScore;
	}
	void AddTrunOver(BYTE turnCount)	//翻操作
	{
#if USEASSERT
		assert(turnCount == 1);
		assert(turnCount > 0);
#endif

		if(m_type == 1)		//加底
		{
			m_currentScore = m_currentScore += turnCount; 
			m_currentTurnoverCount += turnCount;
		} 
		else if(m_type == 2)	//翻倍
		{
			if(m_currentTurnoverCount < m_turnoverCountLimit)
			{
				m_currentScore = (Lint)(m_currentScore * pow((double)2, turnCount));
				m_currentTurnoverCount += turnCount;
			}
			else
			{
				m_currentScore = m_currentScore + turnCount;
				m_currentTurnoverCount += turnCount;
			}

		}
	}
	void AddBomb(BYTE bombCount)	//炸弹操作
	{
#if USEASSERT
		assert(bombCount > 0);
#endif

		if(m_type == 1)
		{
			if(m_currentTurnoverCount < m_turnoverCountLimit)
			{
				if(m_currentTurnoverCount + bombCount > m_turnoverCountLimit)
				{
					m_currentScore = (Lint) (m_currentScore * pow((double)2, ( m_turnoverCountLimit - m_currentTurnoverCount)) + (m_currentTurnoverCount + bombCount - m_turnoverCountLimit));
					m_currentTurnoverCount += bombCount;
				}
				else
				{
					m_currentScore = (Lint) (m_currentScore * pow((double)2, bombCount));
					m_currentTurnoverCount += bombCount;
				}
			}
			else
			{
				m_currentScore = m_currentScore + bombCount;
				m_currentTurnoverCount += bombCount;
			}
		}
		else if (m_type == 2)
		{
			if(m_currentTurnoverCount < m_turnoverCountLimit)
			{
				if(m_currentTurnoverCount + bombCount > m_turnoverCountLimit)
				{
					m_currentScore = (Lint) (m_currentScore * pow((double)2, ( m_turnoverCountLimit - m_currentTurnoverCount)) + (m_currentTurnoverCount + bombCount - m_turnoverCountLimit));
					m_currentTurnoverCount += bombCount;
				}
				else
				{
					m_currentScore = (Lint) (m_currentScore * pow((double)2, bombCount));
					m_currentTurnoverCount += bombCount;
				}
			}
			else
			{
				m_currentScore = m_currentScore + bombCount;
				m_currentTurnoverCount += bombCount;
			}
		}
	}

	Lint GetScore() 
	{
		return m_currentScore;
	}

	Lint GetTurnCount() 
	{
		return m_currentTurnoverCount;
	}

private:
	BYTE	m_type;		//0无效， 加底1，翻倍2
	BYTE	m_turnoverCountLimit;	//翻倍限制
	BYTE	m_currentTurnoverCount;	//当前翻倍数

	BYTE	m_userTurnOverCount[DESK_USER_COUNT];	//玩家倒不倒记录

	Lint	m_baseScore;				//底分
	Lint	m_currentScore;			//当前分数
	PVPType	m_pvpType;				//对战类型（没用到）

};

class GameHandler_DDZ_SiChuanPiZi : public GameHandler
{
public:
	bool startup(Desk *desk);
	void SetDeskDeal();
	void CheckSameIP();
	void QiangDiZhu(Lint nPos);
	void SetDeskPlay();
	bool OnUserReconnect(User* pUser);
	void dealCard_new();
	void ProcessRobot(Lint pos, User * pUser);

	//handler
	void HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg);
	void HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);



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

	//
	bool IsPlayByFilter( Lint type, bool last);	//是否能出牌
	bool IsLeftLaizi(const CardVec & tmp, Lint pos);

	///////////////////////////////////////////// 调用desk函数; //////////////////////////////////////////////////
protected:
	//广播
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
	bool IsNongMinSpring(Lint winPos);
	bool IsLandlordSpring(Lint winPos);
	void DoBomb(Lint pos);
	void GetScore(Lint winPos, Ldouble * gold);

	//需不需要的广播???
	void BoadCastUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType, Lint nOperRel);
	void QueryUserOper(Lint nCurPos, Lint nMsgID, Lint nOperType);
	//
	void DecideDizhu();

	//玩家状态
	void SetAllUserDone(User::UserRoomState state, bool isDone);
	void SetUserDone(Lint pos, User::UserRoomState state, bool isDone);
	bool GetUserDone(Lint pos, User::UserRoomState state);
	void SetAllUserRoomState(User::UserRoomState state);
	bool IsAllUserDone(User::UserRoomState state);
	
	//抢地主阶段的状态切换
	void TellNext(BYTE state);
	void BeforeTellDao();
	void TellDao();
	void TellHuaSe();	
	void TellDizhuTouShaizi(); //告诉地主投色子
	void DoPlay();
	void HanderDao(Lint oper ,Lint pos);
	void HanderHuaSe(Lint oper);
	void HanderDizhuTouShaizi();	//
	//
	void BroadCastDizhu();

private:
	Desk* m_pDesk;

private:
	//User*			m_user[USER_COUNT_PiZi];
	//Lint            m_nUserCounts;
	Lint            m_nCardCounts;

	//	PK_GAME_STYLE   m_gameStyle;      // 游戏类型;
	Lint            m_nFirstCardIndex;  // 确定庄家位置的牌面索引;没有弃牌时为黑桃3，有弃牌时看所有玩家手牌中所有黑桃中最小的一张;
	Lint            m_nDiZhuPos;       //地主位置;
	//	Lint            m_nMaxFanshu;      //游戏最大翻倍数;
	Lint            m_nDifen;          //游戏底分;
	Lint            m_nDizhuPlayCounts;//地主出牌次数，用于计算农民春天;
	Lint            m_nQiangDizhuState;//搶地主階段，0非搶地主階段；1悶抓階段；2抓不抓階段，3倒階段；4拉階段;

	Lint            m_menzhua[USER_COUNT_PiZi];
	Lint            m_daobudao[USER_COUNT_PiZi];
	Lint            m_labula[USER_COUNT_PiZi];
	//Lint            m_nUserBeishu[USER_COUNT_PiZi];

	CardVec         m_putCard;//出去的牌;
	CardVec         m_handCard[USER_COUNT_PiZi];//手上的牌 ;
	CardVec         m_leftCard;//斗地主底牌;

	CardVec			m_outCard[USER_COUNT_PiZi];

	Lchar           m_playerCardType[USER_COUNT_PiZi];//出牌类型;
	Lchar           m_curCardType;	//number
	Lchar           m_curCardMode;	//style
	Lchar           m_check;
	Lchar			m_curPlayCount;
	Lchar           m_win;
	//	Lchar           m_readyState[USER_COUNT_PiZi];//玩家准备状态;

	//Lchar           m_bombCount;//炸弹数量;
	Lchar			m_firstPos;// 保存先手位置,第一把为房主，其后为上局赢家;

	Lint			m_bomb[USER_COUNT_PiZi];	//记录每一个人的使用炸弹数

	CardDeal_SiChuanPiZi		m_dealCard;

	//	Lint			m_deskState;//桌子状态;

	Lint			m_curPos;//当前操作玩家;
	Lint			m_playPos;	//出牌玩家

	Llong			m_ticktime;

	//VipLogItem*		m_vip;

	//Lint			m_reset[USER_COUNT_PiZi];
	//Lstring			m_resetUser;//申请的玩家;
	//Lint			m_resetTime;//申请解算的时间;

	//Lint            m_black3first;     //第一轮出牌玩家是否可以不带黑桃3，1为可以不带，0为必须带黑桃3;
	//Lint            m_tiaopaishu;       //测试玩家调牌张数，0代表不调牌;
	//CardValue       m_specialCard[56];  //存储玩家特定发牌牌局;

	//VideoLog		m_video;//录像;

	CardHelp_SiChuanPiZi		m_help;
	std::vector<std::vector<Lint>>			m_curHelpInfo;
	std::vector<Lint>                       m_curHelpUnusable;
	Lint			m_baojing[USER_COUNT_PiZi];

	//点数(
	BYTE			m_dianshu;
	BYTE			m_pokerPoint;
	BYTE			m_pokerStyle;
	Lint			m_lightPokerIndex;	//明牌
	Lint			m_dizhuFriendPokerIndex;	//地主腿子的牌
	Lint			m_nTuiZiPos;	//地主腿子位置
	//记录春天信息
	enum springType
	{
		EInvalid,
		EDizhuSpring,
		ENongminSpring,
	};
	springType			m_springType;	//0, 无效， 1地主春天，2农民春天
	//end

	Score			m_score;
};

DECLARE_GAME_HANDLER_CREATOR(game_ddz_sichuan_pz, GameHandler_DDZ_SiChuanPiZi)

#endif