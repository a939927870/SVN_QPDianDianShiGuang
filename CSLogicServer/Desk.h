#ifndef _DESK_H_
#define _DESK_H_

#include "User.h"
#include "Card.h"
#include "AI.h"
#include "LTime.h"
#include "LVideo.h"
#include "GameHandler.h"
#include "GameDefine.h"

class Room;
struct VipLogItem;

struct ThinkTool
{
	ThinkVec	m_thinkData;

	void Reset()
	{
		m_thinkData.clear();
	}

	ThinkTool()
	{
		Reset();
	}

	bool HasHu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
				return true;
		}
		return false;
	}

	bool HasPeng()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_PENG)
				return true;
		}
		return false;
	}

	bool HasChi()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_CHI)
				return true;
		}
		return false;
	}

	bool HasMBu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_MBU)
				return true;
		}
		return false;
	}

	bool HasMGang()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_MGANG)
				return true;
		}
		return false;
	}

	bool HasTing()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_TING)
				return true;
		}
		return false;
	}

	bool NeedThink()
	{
		return !m_thinkData.empty();
	}
};


class Desk
{
public:
	Desk();
	~Desk();
	void			Init();
	Lint			GetDeskId();

	void			SetVip(VipLogItem* vip);
	bool			CreateRegister(GameType gameType);
	VipLogItem*		GetVip();

	void			Tick(LTime& curr);

	void			HanderUserReady(User* pUser,LMsgC2SUserReady* msg);
	void			HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg);
	void			HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg);

	void			HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg);		//玩家处理海底牌

	void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg);
	void			HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg);

	void			HanderUserOutDesk(User* pUser);

	void			HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg);

	void			HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg);

	void			HanderUserSelectPiao(User* pUser, LMsgC2SPiaoSelect* msg);

	void			HanderGameOver();
	void            HanderResetDesk();
	//////////////////////////////////////////////////////////////////////////
	Lint			getGameType();
	std::vector<Lint>&	getPlayType();
	void			setPlayType(std::vector<Lint>& playtype);
	Lint			getDeskState();
	void			setDeskState(Lint nState);
	Lint			getDeskPlayState();
	void			setDeskPlayState(Lint nDeskPlayState);
	//////////////////////////////////////////////////////////////////////////


	void			ResetEnd();
	bool			OnUserReconnect(User* pUser);

	void			OnUserInRoom(User* user);
	void			OnUserOutRoom(User* user);

	Lint			GetFreePos();
	Lint			GetUserPos(User* user);
	User*			GetPosUser(Lint pos);

	Lint			GetNextPos(Lint prev);
	Lint			GetPrevPos(Lint next);

	Lint			GetUserCount();


	void			CheckGameStart();

	void			SetDeskWait();
	void			OnDeskThingIng();//等待玩家思考中
	void			OnDeskWait();
	void			SetAllReady();


	void			SetDeskFree();
	void			OnDeskFree();

	bool			HasRobot();	//是否有机器人;
	void			SetTeamID(Lint nTeamID);
	Lint			GetTeamID() { return m_nTeamID; }

	void			SetTeamName(Lstring strTeamName);
	Lstring			GetTeamName() { return m_strTeamName; }

	void			SetProportionate(Lint nProportionate) { m_proportionate = nProportionate; }
	Lint			GetProportionate() { return m_proportionate; }
	void			SetStake(Lint nStake) { m_stake = nStake; }
	Lint			GetStake() { return m_stake; }

	void			SetCircleGoldLimit(Lint nCircleGoldLimit) { m_circleGoldLimit = nCircleGoldLimit; }
	Lint			GetCircleGoldLimit() { return m_circleGoldLimit; }

	void			CheckReset();
	void            CheckMatch();
	void			ClearUser();
	void			BoadCast(LMsg& msg);
	void			BoadCastWithOutUser(LMsg& msg,User* user);

	void			ResetClear();

	void		    SpecialCardClear();   //初始化 特定牌标志

	void			SendSportGameResult();
	void			SendErrSportGameResult(Lint err);
	void            SetState(Lint state); //玩法规则
	Lint            GetState();
	Lint			GetLastOptTime();
	void			UpdateLastOptTimeAndPos(Lint optpos);
	void			UpdateLastOptTimeAndPos(User* user);
	inline Lint		GetCurOptPos() { return m_curOptPos; }
	void			ClearLastOptTime();
	void            CloseOnMatchReset(); //关闭比赛场重置状态

	bool			CheckRoom(LTime& curr);
	void			DelCard(Ldouble gold[4]);

private:
	void            checkMatchWait_(Lint curTime, Lint lastoptTime, Lint inNum);
	void            checkMatchPlayer_(Lint curTime, Lint lastoptTime, Lint inNum);
	void            openOnMatchReset_();  //开启比赛场重置状态
	bool            onMatchReset_();      //是否正处于关闭状态

protected:
	void            SwitchScoreToUser_(Ldouble gold[4]);
	bool            isNewPlay();

private:
	Lint			m_deskState;					//桌子状态
	Lint			m_deskPlayState;				//打牌状态
	Lstring			m_resetUser;					//申请的玩家
	Lint			m_resetTime;					//申请解算的时间


public:
	Lint			m_flag;							//房间规则
	std::vector<Lint>	m_playtype;					//玩法规则
	Lint			m_id;							//桌子ID
	User*			m_user[DESK_USER_COUNT];		//玩家列表
	Lint			m_readyState[DESK_USER_COUNT];	//玩家准备状态 
	GameHandler*	mGameHandler;					//具体游戏处理器
	VipLogItem*		m_vip;							//录相
	Card            m_specialCard[CARD_COUNT];		//存储玩家定义牌序
	Lint			m_reset[DESK_USER_COUNT];		//玩家装态重置
	Lint            m_desk_user_count;
	Lint            m_State;                     //logicmanager 传递的玩法规则

	Lint            m_lastOptTime;				//最后等待操作的时间
	Lint            m_curOptPos;                //当前操作的玩家
	bool            m_bOnReset;                 //是否重新重置比赛场中，如果是，那么

	Lint			m_nTeamID = 0;						// 所属工会 [2017-7-23 willing]
	Lstring			m_strTeamName;					// 工会名称 [2017-7-25 willing]

	Lint			m_proportionate = 0;            //抽成率
	Lint			m_circleGoldLimit = 0;          //每局进入的金币数
	Lint			m_stake = 0;                    //押注倍率

public:
	bool			CanGameStart();
	void            BrocastUserNoGold();
	void			BrocastUserNoGold(Lint exPos);

private:
	Lint			m_waitChargeTime = 0;					//等待充值的时间
};

typedef std::map<DWORD,Desk*> DeskMap;
typedef std::queue<Desk*> DeskVec;
#endif