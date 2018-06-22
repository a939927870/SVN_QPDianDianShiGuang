#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"

class Desk;

class  User
{
public:
	User(LUser data, Lint gateId, Lint gateUserId);
	virtual ~User();

	virtual void	Tick();

	//获取玩家数据库里id
	virtual Lint GetUserDataId(bool bRandId = false);

	//获取玩家唯一表示
	Llong   GetUserUnique();

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	void	SetUserGateUserId(const Lint gateUserId);
	Lint    GetUserGateUserId();

	bool	GetOnline();
	void	SendLoginInfo(Lstring& buyInfo, Lint hide);
	void	SendItemInfo();
	void    SendManaged();

	bool	Login();
	void	Logout();

	void	Send(LMsg& msg);

	Desk*	GetDesk();
	void	SetDesk(Desk* desk);

	void	HanderMsg(LMsg* msg);
	//玩家创建房间
	Lint	HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg);

	//玩家创建房间 异常回馈
	void	HanderUserCreateDeskStateError(LMsgC2SCreateDesk* msg);

	//玩家进入房间
	Lint	HanderUserAddDesk(Lint nDeskID);

	//玩家进入房间 异常回馈
	void	HanderUserAddDeskStateError(LMsgC2SAddDesk* msg);

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
	void	HanderResutDesk(LMsgC2SResetDesk* msg);

	//玩家选择解算操作
	void	HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg);

	void    HanderSelectPiao(LMsgC2SPiaoSelect* msg);
	//玩家请求聊天
	void	HanderUserSpeak(LMsgC2SUserSpeak* msg);

	//心跳消息
	void	HanderUserHeart(LMsgC2SHeart* msg);

	void    HanderAttackManaged(LMsgC2SCancelManaged * msg);

	void    CheckSportStatus();

	Lstring GetIp();

	void	SetIp(Lstring& ip);

//	void	Save();
	
	//房卡相关操作
	//获取房卡数量
	bool	IfCardEnough(Lint cardType, Lint num);

	//增加房卡;
	void	AddCardCount(Lint cardType, Ldouble count, Lint operType, const Lstring& admin, bool bNeedSave = true);

	//删除房卡;
	void	DelCardCount(Lint cardType, Ldouble count, Lint operType, Lint nGameType, const Lstring& admin);

	//改变玩家状态
	void	ModifyUserState(bool bLeaveServer = false);
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue, Lint maYouQuan = 0);
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	Lint	getUserGateIdx(){return m_gateUserId;}
	void	setUserGateIdx(Lint nValue){m_gateUserId = nValue;}
	void    setLongitueLatitude(const Lstring& longitue, const Lstring& latitude) { m_longitue = longitue;  m_latitude = latitude; }
	void    getLongitueLatitude(Lstring& longitue, Lstring& latitude) { longitue = m_longitue;  latitude = m_latitude; }

	inline void  attarchManaged() { m_managed = true; }
	inline void  cancelManaged() { m_managed = false; }
	inline bool  isManaged() { return m_managed; }

	virtual bool IsRobot();

public:
	LUser	m_userData;
	Lint	m_gateId;		//玩家在哪个gate上面
	Lint	m_gateUserId;	//玩家在gate上面的id
	Lint	m_userState;	//玩家当前状态
	bool	m_online;
	Lstring m_longitue;     //经度
	Lstring m_latitude;		//纬度
	std::vector< LActive >	m_Active;
	Desk*	m_desk;

	Lstring	m_ip;
	bool    m_managed; //是否被托管
public://比赛场用
	Lint    m_rank;
	Lint	m_score;
	Lint	m_overScore;
	Lint    m_sportId;
	Lint    m_process; //阶段
	void	initPlayGroundData(Lint rank,Lint score,Lint overScore,  Lint sportId, Lint process);
	void    clearPlayGroundData();
};

#endif