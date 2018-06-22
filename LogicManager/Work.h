#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "GarbageCollectionService.h"

class User;

struct UserLoginInfo
{
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);

	LTime&			GetCurTime();
	
	//md5加密算法
	Lstring			Md5Hex(const void* data,Lsize size);
public:
	//处理http消息 
	void			HanderHttp(LMsgHttp* msg);	
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);

	//处理玩家登陆的消息 
	void			HanderUserLogin(Lint id,Lint seed,Lint gateId,Lint geteUserId,Lstring& md5,Lstring& ip);

	//处理玩家断线重连的消息 
	void			HanderUserReLogin(Lint id,Lint seed,Lint gateId,Lint gateUserId,Lstring& md5,Lstring& ip);
	
	//处理玩家登陆的消息 
	void			HanderUserLogout(Lint gateId, Lint geteUserId);
	
	//处理Logic心跳
	void			HanderLogicHeartBeat(LMsgL2LMGHEARTBEAT* msg);
	//处理gate心跳
	void			HanderGateHeartBeat(LMsgG2LMGHEARTBEAT* msg);
public:
	void			ConnectToCenter();

	void			SendToCenter(LMsg& msg);

	void			SendHeartBeatToLogicDB();

	void			SendHeartBeatToCenter();
	//发送网关信息到center
	void			SendGateInfoToCenter();
	
	//发送logic消息到其他gate
	void			SendLogicInfoToGates(Lint nGateID = 0);

	//center发送玩家请求登录
	void			HanderCenterUserLogin(LMsgCe2LUserLogin*msg);

	//center发送玩家id信息
	void			HanderCenterUserInInfo(LMsgCe2LUserIdInfo*msg);

	//更改玩家状态;
	void			HanderModifyUserState(LMsgL2LMGModifyUserState* msg);

	//更改玩家卡牌数;
	void			HanderModifyUserCardNum(LMsgL2LMGModifyCard* msg);

	//更改玩家金币数;
	void            HanderModifyUserGoldNum(LMsgL2LMGGoldChange* msg);

	//更改玩家玩牌局数;
	void			HanderModifyUserPlayCount(LMsgL2LMGModifyPlayCount* msg);

	//玩家局数增加
	void			HanderUserCircleIncreased(LMsgL2LMGUserCircleIncreased * msg);

	//center GM冲卡;
	void			HanderCenterGMCharge(LMsgCe2LGMCharge* msg);

	//center GM设置跑马灯;
	void			HanderCenterGMHorse(LMsgCe2LGMHorse* msg);

	//center GM设置够买提示;
	void			HanderCenterGMBuyInfo(LMsgCe2LGMBuyInfo* msg);

	void			HanderCenterGMHide(LMsgCe2LGMHide* msg);

	//center GM设置玩家类型
	void			HanderCenterGMModifyPlayerType(LMsgCe2LGMModifyPlayerType* msg);

	//center GM充值钻石;
	void			HanderCenterGMChargeDiamond(LMsgCe2LGMChargeDiamond* msg);
	//center GM购买道具;
	void			HanderCenterGMPurchase(LMsgCe2LGMPurchase* msg);

	//center 发送玩家活动;
	void			handerDBReqActive(LMsgLDB2LActiveInfo* msg);

	//处理Center发过来玩家重新登陆;
	void			handerCenterUserReLogin(LMsgCe2LUserReLogin* msg);

	//处理LDB心跳;
	void			HanderLogicDBHeartBeat(LMsgLDB2LMGHEARTBEAT_BACK* msg);

	void			HanderDBRetShareVideo(LMsgLDB2LRetShareVideo* msg);

	void			HanderDBGetShareVideoID(LMsgLDB2LRetShareVideoId* msg);
	//center服务器关闭;
	void			HanderCenterLogout();
	void			HanderDBServerLogout();
	UserLoginInfo*  GetUserLoginInfo(Lint userId);
	void			DelUserLoginInfo(Lint userId);
	
	//跑马灯相关;
	void			SetHorse(Lstring& horse);
	Lstring&		GetHorse();

	//y
	void			SetHide(Lint hide);
	Lint			GetHide();

	//够买提示相关;
	void			SetBuyInfo(Lstring& buy);
	Lstring&		GetBuyInfo(Lint id);

	//通知相关;
	void					AddNotice(Notice& notoice);
	std::vector<Notice>&	GetNotice();
public:
	void			HanderGateLogin(LMsgG2LMGLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	GateInfo*		GetGateInfoBySp(LSocketPtr sp);
	GateInfo*		GetGateInfoById(Lint id);
	void			DelGateInfo(Lint id);

	void			HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate);

	void            HanderKeepAlive(LMsgS2SKeepAlive* msg);
	void            HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg);
	void            EraseExpiredLogics(LTime& cur);
public:
	void			HanderLogicLogin(LMsgL2LMGLogin* msg);
	void			HanderLogicLogout(LMsgKick* msg);
	LogicInfo*		GetLogicInfoBySp(LSocketPtr sp);
	LogicInfo*		GetLogicInfoById(Lint id);
	LogicInfo*		GetLogicInfoByPresure(Lint flag);
	void			DelLogicInfo(Lint id);

public:
	void			SendMessageToLogic(Lint logicID, LMsg& msg);
	void			SendMessageToGate(Lint gateID, LMsg& msg);
	void			SendMessageToAllLogic(LMsg& msg);
	void			SendMessageToAllGate(LMsg& msg);
	//DB链接相关;
	void			ConnectToDb();
	void			SendMsgToDb(LMsg& msg);

	//db发送玩家战绩;
	void			HanderDBReqVipLog(LMsgLBD2LReqVipLog* msg);

	void            HanderDBReqOneVipLog(LMsgLBD2LReqOneVipLog* msg);

	//db发送玩家录像;
	void			HanderDBReqVideo(LMsgLDB2LReqVideo* msg);

	//任务
	template<typename MSG>
	void			HanderDBRetTaskMsg(MSG* msg);
	
	//十一活动
	//处理db返回的十一活动数据;
	void			HanderDBResSignCollectWin(LMsgLDB2LResSignCollectWin* msg);
	void			HanderAddUserBaishu(LMsgL2LMGAddUserBashu* msg);

	//Club
public:
	void			HanderTeamServerLogout(LMsgKick* msg);
	void			HanderTeamServerLogin(LMsgTS2LMG_Login* msg);
	void			HanderTeamServerHeartBeat(LMsgTS2LMG_Heartbeat* msg);
	void			HanderTeamServerCreateDesk(LMsgTS2LMG_CreateDesk* msg);
	void			HanderTeamVideoLog(LMsgTS2LMG_TeamVideoLog* msg);
	void			HanderTeamGetUserInfo(LMsgTS2LMG_GetUserInfo* msg);
	void			HanderQueryUserOnlneState(LMsgTS2LMG_QueryIsOnLine* msg);
	void			HanderTS2LMGAddDesk(LMsgTS2LMG_AddDesk* msg);
	void			HanderTSReqDeskId(LMsgTS2LMG_ReqDeskId* msg);
	void			HanderTSReqExchageGold(LMsgTS2LMG_ReqExchageGold* msg);
	void			HanderTSUserLogin(LMsgTS2LMG_UserLogin* msg);
	bool			IsConnectTeamServer();
	void			SendMessageToTeamServer(LMsg& msg);
	void			SendTeamServerInfoToLogic(Lint nLogicID);
	void			SendTeamServerInfoToGate(Lint nGateId);

public:
	std::unordered_set<int>	m_TeamServerDeskSet;

private:
	void			checkAndTouchLogic();
	void            CheckConfigUpdates();

	void			SendLoginToCenter();
	void			SendLoginToDB();
private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次;

	Llong		m_200MSceTick;//200毫秒循环一次;

	Llong		m_1SceTick;//1秒循环一次;

	Llong		m_5SceTick;//5秒循环一次;

	Llong		m_30SceTick;//30秒循环一次;

	Llong		m_600SceTick;//5分钟秒循环一次;

	Lint		m_curDate;	//

	bool		m_IsLogicDB;	//Logic状态， false 需要重连;
private:
	TCPClientPtr	m_centerClient;//链接center;

	std::map<Lint, GateInfo> m_gateInfo;

	std::map<Lint, LogicInfo> m_logicInfo;

	std::map<Lint, Lint>		m_toEraseLogics;

	std::map<Lint, UserLoginInfo> m_reqLoginUser;//等待登录的玩家队列;

	Lstring		m_horse;//跑马灯;

	Lstring		m_buyInfo;//够买提示框;

	std::vector<Lstring> m_vec_buyInfo;

	std::vector<Notice>	m_noticeVec;

	Lint		m_hide;

	//////////////////////////////////////////////////////////////////////////
	TCPClientPtr	m_dbClient;//链接db;
	bool		m_isConnect;
	GarbageCollectionService	m_gcService;

	LSocketPtr  m_teamServerSocket;	// 工会服务器连接的socket [2017-7-10 willing]
	Lstring		m_strTeamServerIp;
	Lshort		m_nTeamServerPort;
	Lint		m_TeamServerID;

};

#define gWork Work::Instance()

#endif