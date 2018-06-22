#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"
#include "PlayerTask/PlayerTaskManager.h"
#include "ActiveManager_SignCollect.h"


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

	void	Login(Lint& inlogic);
	void	Logout();

	void	Send(LMsg& msg);

	void	SendLoginInfo(Lint errorCode, Lint onUseTeamId, Lstring onUseTeamName);
	void	SendItemInfo();
	void	SendHorseInfo();
	void    SendHuoDong();

	void	S2CeUserLogin();
	void	S2CeUserLogout();
	Lint	GetDeskID();
	void	SetDeskID(Lint deskid);

	void	HanderMsg(LMsg* msg);
	//玩家创建房间;
	void	HanderUserCreateDesk(LMsgC2SCreateDesk* msg);

	//玩家创建金币场房间;
	void	HanderUserCreateGoldDesk(LMsgC2SCreateDesk* msg);

	//玩家进入房间;
	void	HanderUserAddDesk(LMsgC2SAddDesk* msg);

	//玩家请求战绩;
	void	HanderUserVipLog(LMsgC2SVipLog* msg);

	//玩家请求录像;
	void	HanderUserVideo(LMsgC2SVideo* msg);

	//请求玩家任务信息
	void	HanderUserGetPlayerTask(LMsgC2SGetTask* msg);

	//心跳消息;
	void	HanderUserHeart(LMsgC2SHeart* msg);

	//玩家请求抽奖;
	void	HanderUserDraw(LMsgC2SRequestDraw* msg);
	//玩家请求写入电话;
	void	HanderWritePhone(LMsgC2SWritePhone* msg);
	//玩家请求活动抽奖记录;
	void	HanderUserReqActive(LMsgC2SActiveLog* msg);
	//通知玩家抽奖;
	void	SendNoticeDrawGift();
	//请求LDB 玩家的活动信息;
	void	RequestActiveInfo(LMsgC2SActiveInfoRequest* msg = NULL);
	void	HanderUserRequest_Share_Card(LMsgC2SRequest_Share_Card* msg);
	//通知客户端活动是否开启;
	void	SyncActiveOpen();	

	//玩家请求获取某分享id录像;
	void    HanderUserShareVideo(LMsgC2SShareVideo* msg);

	void    HanderUserOneVipLog(LMsgC2SOneVipLog* msg);

	//玩家请求分享某录像;
	void    HanderUserReqShareVideo(LMsgC2SReqShareVideo* msg);

	//玩家请求获取分享的录像;
	void	HanderUserGetShareVideo(LMsgC2SGetShareVideo* msg);

	//获取指定玩家信息
	void	HanderGetUserInfo(LMsgC2SGetUserInfo* msg);

	void    HanderUserWealthChange(LMsgC2SWealthChange* msg);

	//add by arena;
	void    HanderUserArenaMSG(LMsg* msg);
	void    HanderUserIntoArena(LMsgC2SIntoArena* msg);
	void    HanderUserQuitArena(LMsgC2SQuitArena* msg);
	//add end

	Lstring GetIp();

	void	SetIp(Lstring& ip);

	void	Save();
	
	//房卡相关操作;
	//获取房卡数量;
	bool	IfCardEnough(Lint cardType, Lint num);
	bool    IfGoldEnough(Lint nGold);
	bool    GiveUserGold(Lint giveGoldNum);  //赠送玩家金币，每次两次，每次1000;
	void    SaveGoldChange(Lint oper,Lint cardNum);
	void    send2UserGoldChange(Lint changeNum, Lint changeReason, Lint goldSum);
	//增加房卡;
	void	AddCardCount(Lint cardType, Ldouble count,Lint operType,Lstring admin, bool bNeedSave = true);
	//删除房卡;
	void	DelCardCount(Lint cardType, Ldouble count, Lint operType, Lstring admin);

	//玩家金币改变;
	void    ChangeGoldCount(Lint changeReason, Ldouble changeNum);

	void    SaveCardExchange(Lint oper,Lint cardType, Ldouble cardNum);
	//修改玩家打的局数，更改数据;
	void	ModifyUserPlayCount(Lint nCount);

	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserLogicID(){return m_logicID;}
	Lint	getUserGoldLogicID(){return m_goldLogicID;}
	void    setUserGoldLogicID(Lint nValue){m_goldLogicID = nValue;}
	void	setUserLogicID(Lint nValue){m_logicID = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	Lint	getUserGateIdx(){return m_gateUserId;}
	void	setUserGateIdx(Lint nValue){m_gateUserId = nValue;}
	Lstring&	getUserIPStr(){return m_ip;}
	Lint        getNeedCard(Lint nCounts);
	PlayerTaskManager* GetTaskManager(void) { return &m_taskMgr;}
	Lint  getChange2WealthNum(Lint Weath1Type, Lint Weath1Num, Lint Weath2Type);

	// 工会服务器通知创建房间 [2017-7-23 willing]
	void	HanderTeamCreateDesk(LMsgTS2LMG_CreateDesk* msg);
	void	HanderTeamAddDesk(LMsgTS2LMG_AddDesk* msg);
	void	HanderTeamReqExchageGold(LMsgTS2LMG_ReqExchageGold* msg);
	void	HanderTeamUserLogin(LMsgTS2LMG_UserLogin* msg);

public:
	LUser	m_userData;					//玩家数据;
	std::vector< LActive >	m_Active;	//活动信息;
	Lint	m_gateId;					//玩家在哪个gate上面;
	Lint	m_gateUserId;				//玩家在gate上面的id;
	Lint	m_logicID;					//玩家在哪个logic上面;
	Lint    m_goldLogicID;              //玩家在哪个goldLogic上面;
	Lint	m_userState;				//玩家当前状态;
	Lint	m_userDeskID;				//玩家桌子ID;
	bool	m_online;
//	Desk*	m_desk;
	
	Lstring	m_ip;
	bool	m_bActiveDBResponse;             //分享活动，防止读数据库失败造成玩家多次赠卡;

private:

	Lint    GetLeftTimesToday();             //获取当天剩余抽奖次数;
	Lint    CheckQualification();            //检查玩家是否具有抽奖资格;
	void    ConsumeForDial();
	Lint    GetTimesOfGold();
	PlayerTaskManager		m_taskMgr;
	Lint    GetApplicableId_(std::vector<Lint>& deskIds, std::vector<Lint>& limitGolds);
public:
	//2016年10一活动（数据及消息）
	void HanderUserSignCollect(LMsgC2SReqSignCollect * msg);	//签到,集字动作
	void HanderUserSignCollectWinState(LMsgC2SReqSignCollectWinState * msg);	//签到状态，集字， 胜利状态
	//
	void ResSign();
	void ResCollect();

	void ResSignState();
	void ResCollectState();
	void ResWinState();

	//启动加载活动
	void SyncLoadSignCollectWin();

	
	ActiveMgr_SignCollect	m_actMgr_signColl;
};

enum USER_WEALTH_TYPE
{
	USER_WEALTH = 0,	            //空;
	USER_WEALTH_CARD_TYPE_4,		//房卡类型1;
	USER_WEALTH_CARD_TYPE_8,		//房卡类型2;
	USER_WEALTH_CARD_TYPE_16,       //房卡类型3;
	USER_WEALTH_GOLD,  		//金币;
};

#endif