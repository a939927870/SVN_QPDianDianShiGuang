#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"

class Desk;

class  User
{
public:
	User(LUser data, Lint gateId,Lint gateUserId);
	virtual ~User();

	virtual void	Tick();

	//获取玩家数据库里id;
	Lint	GetUserDataId();

	//获取玩家唯一表示;
	Llong	GetUserUnique();

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	void	SetUserGateUserId(Lint gateUserId);
	Lint	GetUserGateUserId();

	bool	GetOnline();
	void	SendLoginInfo(Lstring& buyInfo, Lint hide);
	void	SendItemInfo();
    void    send2LMGGoldChange(Lint changeNum, Lint changeReason, Lint goldNum);
	bool	Login(Lint gameZone = game_zone_roomcard);
	void	Logout();

	void	Send(LMsg& msg);

	Desk*	GetDesk();
	void	SetDesk(Desk* desk);
	Desk*   GetGoldDesk();
	void    SetGoldDesk(Desk* desk);

	void	HanderMsg(LMsg* msg);
	//玩家创建房间;
	Lint	HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg);

	//玩家进入房间;
	Lint	HanderUserAddDesk(Lint nDeskID);

	Lint    HanderUserIntoGoldDesk(LMsgLMG2LIntoGoldDesk* msg);

	//玩家退出房间;
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg);

	//处理玩家抢地主请求;
	void HanderUserQiangDiZhu(LMsgC2SQiangDiZhu* msg);
	
	//处理玩家四川斗地主玩法抢地主相关操作请求;
	void HanderUserQiangDiZhuOper(LMsgC2SQdzOper_Sichuan* msg);

	//玩家请求出牌;
	void	HanderUserPlayCard(LMsgC2SUserPlay* msg);

	//玩家请求通知内容;
	void	HanderUserReady(LMsgC2SUserReady* msg);
	
	//申请解算房间;
	void	HanderResutDesk(LMsgC2SResetDesk* msg);

	//金币桌相关操作;
	void    HanderAskGoldDeskState(LMsgC2SAskGoldDeskState* msg);

	//玩家选择解算操作;
	void	HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg);

	//玩家请求聊天;
	void	HanderUserSpeak(LMsgC2SUserSpeak* msg);

	//心跳消息;
	void	HanderUserHeart(LMsgC2SHeart* msg);

	Lstring GetIp();

	void	SetIp(Lstring& ip);

	void	Save();
	
	//房卡相关操作;
	//获取房卡数量;
	bool	IfCardEnough(Lint cardType, Ldouble num);
	//增加房卡;
	void	AddCardCount(Lint cardType, Ldouble count,Lint operType,const Lstring& admin, bool bNeedSave = true);
	//删除房卡;
	void	DelCardCount(Lint cardType, Ldouble count, Lint operType, Lint nGameType, const Lstring& admin);
	


	//改变玩家状态;
	void	ModifyUserState(bool bLeaveServer = false);
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue, Lint maYouQuan = 0);
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	Lint	getUserGateIdx(){return m_gateUserId;}
	void	setUserGateIdx(Lint nValue){m_gateUserId = nValue;}
	void    setTuoguan();
	void    unSetTuoguan();

	void    setLeaveGoldDesk(){m_bLeaveGoldDesk = true;}
	void    unsetLeaveGoldDesk(){m_bLeaveGoldDesk = false;}
	bool    getLeaveGoldDesk(){return m_bLeaveGoldDesk;}

	//
	virtual bool	IsRobot();
public:
	LUser	m_userData;
	Lint	m_gateId;		//玩家在哪个gate上面;
	Lint	m_gateUserId;	//玩家在gate上面的id;
	Lint	m_userState;	//玩家当前状态;
	bool	m_online;
	std::vector< LActive >	m_Active;
	Desk*	m_desk;
	Desk*   m_goldDesk;
	bool    m_bTuoGuan;  //是否进入机器人托管;
	bool    m_bLeaveGoldDesk;  //玩家是否离开金币场桌子;

	Lint    m_nWaitTime;  //等待玩家操作时间,超时则自动托管;

	Lstring	m_ip;

//玩家房间状态;
public:
	enum UserRoomState
	{
		EInvalid,
		EReady,
		EHuaSe,
		EAcceptTurnOver,
		EPlayStart,
		EMax,
	};

	UserRoomState GetRoomState(){ return m_userRoomState;}

	bool GetDone(UserRoomState state)
	{ 
		if(m_userRoomState != state) 	
		{
			return false;
		} 
		return m_isDone[state];
	}

	void SetDone(UserRoomState state, bool isDone) { m_isDone[state] = isDone;};
	void SetRoomState(UserRoomState state) { m_userRoomState = state; }
private:
	UserRoomState m_userRoomState;
	bool m_isDone[EMax];

//十一活动
public:
	//保存把数
	void SavePlayCount(const Lstring& admin);
	void addBashu(Lint baishu);
private:
	Lint m_bashu;
	
};

#endif