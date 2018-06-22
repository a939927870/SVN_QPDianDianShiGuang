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


//缓兵之计 先放这里了.... 赶进度   所有的指针换成对象, 因为使用新手牌的时候,原来的牌的指针已经无法找回了, 只知道什么牌
struct ThinkUnitEntity
{
	ThinkUnitEntity()
	{
		m_type = 0;
	}

	void Clear()
	{
		m_type = 0;
		m_card.clear();
		m_hu.clear();
	}
	Lint m_type;
	std::vector<Card>	m_card;
	std::vector<Lint> m_hu;//胡牌的类型
private:
	//	ThinkUnitEntity(const ThinkUnitEntity&);
	//	ThinkUnitEntity& operator =(const ThinkUnitEntity&);
};

typedef std::vector<ThinkUnitEntity> ThinkVecEntity;

struct ThinkPoolEntity
{
	ThinkVecEntity	m_thinkData;

	void Reset()
	{
		m_thinkData.clear();
	}

	ThinkPoolEntity()
	{
		Reset();
	}

	bool HasHu()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
				return true;
		}
		return false;
	}

	bool HasPeng()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_PENG)
				return true;
		}
		return false;
	}

	bool HasChi()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_CHI)
				return true;
		}
		return false;
	}

	bool HasMBu()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_MBU)
				return true;
		}
		return false;
	}

	bool HasMGang()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_MGANG)
				return true;
		}
		return false;
	}

	bool NeedThink()
	{
		return !m_thinkData.empty();
	}
private:
	ThinkPoolEntity(const ThinkPoolEntity&);
	//	ThinkPoolEntity& operator =(const ThinkPoolEntity&);
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
	void			SyncMsgUserEndSelect(Lint nPos, Lint state, Lint color, Lint number);
	void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg);
	void			HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg);
	void			HanderUserInteraction(User* pUser, LMsgC2SInteraction* msg);
	void			HanderUserOutDesk(User* pUser);
	void			HanderResetDesk(User* pUser,LMsgC2SResetDesk* msg);
	void			HanderSelectResetDesk(User* pUser,LMsgC2SSelectResetDesk* msg);
	void			HanderGameOver();

	void            HanderUserTuoGuan(User* pUser, LMsgC2S_TuoGuan* msg);

	//////////////////////////////////////////////////////////////////////////
	Lint			getGameType();
	std::vector<Lint>&	getPlayType();
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
	void			CheckReset();
	void			CheckExpired(LTime& curr);
	bool			CheckRoom(LTime& curr);
	void			ClearUser();
	void			BoadCast(LMsg& msg);
	void			BoadCastWithOutUser(LMsg& msg,User* user);
	void			ResetClear();
	void		    SpecialCardClear();   //初始化 特定牌标志
	void			SetDeskPlayerNum(Lint flag);
	void			SetDeskPlayerNum(Lint flag, Lint playerNum);

	void			SetDeskAppId(Lint nAppId);
	Lint			GetDeskAppId() const;
	Lint			GetDeskPlayerNum()const;
	bool			IsAaRoom() const;
	void			SetAaRoomCostPlayerCardNum(Lint costCardNum);
	Lint			GetAaRoomCostPlayerCardNum() const; 

	void			SetDeskCanInteraction(Lint canInteraction);
	Lint			GetDeskCanInteraction()const;
	bool			isCheat(User* user);
	void            SetMatchDeskPlay();      //通知进入比赛桌子， 每局都发送
	void            ResetTuoGuan();
	void			BrocastUserNoGold(Lint exPos);

	void			DelCard(Ldouble gold[4]);
private:
	bool			IsValidPos(Lint pos);
	void            SwitchScoreToUser_(Ldouble gold[4]);
	bool            isNewPlay();
private:
	Lint			m_deskState;					//桌子状态
	Lint			m_deskPlayState;				//打牌状态
	Lint			m_flag;							//房间规则
	std::vector<Lint>	m_playtype;					//玩法规则
	Lstring			m_resetUser;					//申请的玩家
	Lint			m_resetTime;					//申请解算的时间		
	Lint			m_canInteraction;				//是否可以交互
	Lint m_nAppId;
public:
	Lint			m_id;							//桌子ID
	User*			m_user[DEFAULT_DESK_USER_COUNT];		//玩家列表
	Lint			m_readyState[DEFAULT_DESK_USER_COUNT];	//玩家准备状态 
	GameHandler*	mGameHandler;					//具体游戏处理器
	VipLogItem*		m_vip;							//录相
	Card            m_specialCard[CARD_COUNT];		//存储玩家定义牌序
	Lint			m_reset[DEFAULT_DESK_USER_COUNT];		//玩家装态重置
	Lint			m_deskplayer_num;				//玩家数量
	LTime			m_deskCreatTime;				//房间创建时间
	bool			m_antiCheat;					//反作弊
	Lint			m_aaRoom;						//AA房间
	Lint			m_aaRoomCostPlayerCardNum;		//AA房间需要消耗的房卡数量

	//比赛场
	Lint			m_nDeskUserCount;				//玩家数量
	time_t          m_LastWaiteThinkRetTime;        //处理托管逻辑的waittime，由handler移到desk中 方便消息处理
	time_t          m_DeskStartPlayTime;            //每局开始比赛时间
	Lint            m_DeskRemainPlayTime;           //每局剩余比赛时间 单位s

	void			SetTeamID(Lint nTeamID);
	Lint			GetTeamID() { return m_nTeamID; }

	void			SetTeamName(Lstring strTeamName);
	Lstring			GetTeamName() { return m_strTeamName; }

	void			SetFreeFlag(Lint nFreeFlag) { m_freeFlag = nFreeFlag; }
	Lint			GetFreeFlag() { return m_freeFlag; }

	void			SetFastPlay(Lint value) { m_fastPlay = value; }
	bool			IsFastPlay() { return  m_fastPlay == 1 ? true : false; }
	void			FastPlayResetDesk();

	void			SetProportionate(Lint nProportionate) { m_proportionate = nProportionate; }
	Lint			GetProportionate() { return m_proportionate; }
	void			SetStake(Lint nStake) { m_stake = nStake; }
	Lint			GetStake() { return m_stake; }

	void			SetCircleGoldLimit(Lint nCircleGoldLimit) { m_circleGoldLimit = nCircleGoldLimit; }
	Lint			GetCircleGoldLimit() { return m_circleGoldLimit; }

public:
	bool					IsMatchDesk();
	int						GetMatchID();

	Lint			m_nTeamID;						// 所属工会 [2017-7-23 willing]
	Lstring			m_strTeamName;					// 工会名称 [2017-7-25 willing]
	Lint			m_freeFlag;						// 是否是免费玩法的桌子，1 是， 0 不是 [2017-8-1 willing]
	Lint			m_fastPlay;						//快速组局房间 
	Lint			m_proportionate = 0;            //抽成率
	Lint			m_circleGoldLimit = 0;          //每局进入的金币数
	Lint			m_stake = 0;                    //押注倍率

public:
	bool			CanGameStart();
	void            BrocastUserNoGold();

private:
	Lint			m_waitChargeTime = 0;					//等待充值的时间

};

typedef std::map<unsigned long,Desk*> DeskMap;
typedef std::queue<Desk*> DeskVec;
#endif