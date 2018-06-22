#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"

class Desk;

class  User
{
public:
	User(LUser data, Lint gateId,Llong gateUserId);
	virtual ~User();

	virtual void	Tick();

	//获取玩家数据库里id
	Lint	GetUserDataId();

	//获取玩家唯一表示
	Llong	GetUserUnique();

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	void	SetUserGateUserId(Llong gateUserId);
	Llong	GetUserGateUserId();

	bool	GetOnline();
	void	SetOnline(bool Online) { m_online = Online; }
	void	SendLoginInfo(Lstring& buyInfo, Lint hide);
	void	SendItemInfo();
	bool    GetTuoGuan() { return m_bIsInTuoGuan; }// 比赛场托管		
	void    SetTuoGuan(bool tuoguan) { m_bIsInTuoGuan = tuoguan; }

	bool	Login();
	void	Logout();

	void	Send(LMsg& msg);

	Desk*	GetDesk();
	void	SetDesk(Desk* desk);

	void	HanderMsg(LMsg* msg);

	bool	HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg);

	bool	HanderUserAddDesk(Lint nDeskID);

	//玩家退出房间
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg);

	//玩家请求出牌
	void	HanderUserPlayCard(LMsgC2SUserPlay* msg);

	//玩家请求思考
	void	HanderUserThink(LMsgC2SUserOper* msg);

	//玩家选择起手胡
	void	HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg);

	//玩家选择海底牌
	void	HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg);

	//玩家请求通知内容
	void	HanderUserReady(LMsgC2SUserReady* msg);
	
	//申请解算房间
	void	HanderResetDesk(LMsgC2SResetDesk* msg);

	//玩家选择解算操作
	void	HanderSelectResetDesk(LMsgC2SSelectResetDesk* msg);

	//玩家请求聊天
	void	HanderUserSpeak(LMsgC2SUserSpeak* msg);

	//玩家互动
	void	HanderUserInteraction(LMsgC2SInteraction* msg);

	//心跳消息
	void	HanderUserHeart(LMsgC2SHeart* msg);

	//玩家请求进入托管或者退出托管
	void    HanderUserTuoGuan(LMsgC2S_TuoGuan* msg);

	Lstring GetIp();

	void	SetIp(Lstring& ip);

	void	Save();
	
	virtual void ModifyRoomCardCount(Ldouble nShift, Lint nReason, Lint nGameType, bool bAddDeskTotal = false);

	//改变玩家状态
	void	ModifyUserState(bool bLeaveServer = false);
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue, Lint maYouQuan = 0);
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}

	//玩家地理位置
	void	SetLocation(const Lstring& location){m_location = location;}
	const Lstring GetLocation(){return m_location;}

	void HanderUserSignInReq(LMsgC2SSignInReq* msg);
	void HanderUserSignInfoReq(LMsgC2SSignInInfoReq* msg);

	virtual bool IsRobot();

private:
	const std::vector<Lint> GetUserGainHeadIconId() const;
public:
	LUser	m_userData;
	Lint	m_gateId;		//玩家在哪个gate上面
	Llong	m_gateUserId;	//玩家在gate上面的id
	Lint	m_userState;	//玩家当前状态
	bool	m_online;
	bool    m_bIsInTuoGuan; //玩家是否被托管
	Lstring m_location;		//玩家地理位置
	std::vector< LActive >	m_Active;
	Desk*	m_desk;

	Lstring	m_ip;
};

#endif