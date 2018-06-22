#ifndef _WORK_H_
#define _WORK_H_

#include <map>
#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMsgCT.h"
#include "LSocket.h"
#include "DBSession.h"
#include "GarbageCollectionService.h"

using namespace std;

struct SGateInfo
{
	Lint		nGateId;
	LSocketPtr	pSocketPtr;
	Llong		nHeartbeatTimestamp;
};

struct SLogicInfo
{
	Lint		nLogicId;
	LSocketPtr	pSocketPtr;
	Llong		nHeartbeatTimestamp;
};

class Work:public LRunnable,public LSingleton<Work>
{
	typedef std::map<Lstring, Lstring>	HttpParamMap;
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			SendToLogicManager(LMsg& msg);

	void			SendToGate(LMsg& msg, Llong nUserUnique);

	void			SendToGateByUserID(LMsg& msg, Lint nUserID);

	//void			CheckConnect();
	bool			IsInMatchDesk(Lint userId);
	DBSession&		GetDbSession();

	LTime&			GetCurTime();
private:
	void			Tick(LTime& cur);

private:
	void			HanderMsg(LMsg* msg);

	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	// Http 消息 [2017-7-12 willing]
	void			HanderHttpMsg(LMsgHttp* msg);

	void			HanderGateMsg(LMsgG2TUserMsg* msg, const LSocketPtr& pGateSocketPtr);
	// 链接者断开网络连接 [2017-7-17 willing]
	void			HanderClientKick(LMsgKick* msg);
	// LogicManager 回应的心跳 [2017-7-10 willing]
	void			HanderLogicManagerHeartBeat(LMsgLMG2TS_Heartbeat* msg);
	// LogicDB 回应的心跳 [2017-7-10 willing]
	//void			HanderLogicDBHeartBeat(LMsgLDB2TS_Heartbeat* msg);
	// LogicServer 登陆 [2017-7-10 willing]
	void			HanderLogicServerLogin(LMsgL2TS_Login* msg);
	// LogicServer 发来的心跳 [2017-7-10 willing]
	void			HanderLogicServerHeartBeat(LMsgL2TS_Heartbeat* msg);
	// GateServer 发来的心跳 [2017-7-12 willing]
	void			HanderGateServerHeartBeat(LMsgG2TS_Heartbeat* msg);
	// GateServer 登陆
	void			HanderGateServerLogin(LMsgG2TS_Login* msg);
	void			HanderLogicOrGateLogout(LMsgLMG2TS_LogicGateLogout* msg);
	void			HanderUserLogin(LMsgLMG2TS_UserLogin* msg);
	void			HanderUserLogout(LMsgLMG2TS_UserLogout* msg);
	void			HanderCreateRoom(LMsgLMG2TS_CreateDesk* msg);
	void			HanderLM2TSAddDesk(LMsgLMG2TS_AddDesk* msg);
	void			HanderGiveBackCard(LMsgLMG2TS_GiveBackCard* msg);
	void			HanderDelCard(LMsgL2TS_DelCard* msg);
	void			HanderVideoLog(LMsgL2TS_VideoLog* msg);
	void			HanderAddDesk(LMsgL2TS_AddDesk* msg);
	void			HanderGameStateChange(LMsgLS2TS_GameState* msg);
	void			HanderUserInfo(LMsgLMG2TS_UserInfo* msg);
	void			HanderNewTeamDesk(LMsgL2TS_NewTeamDesk* msg);
	void			HanderTeamDeskUserChg(LMsgL2TS_TeamDeskUserChg* msg);
	void			HanderDelTeamDesk(LMsgL2TS_DelTeamDesk* msg);
	void			HanderUserOnlineRst(LMsgLMG2TS_UserOnLineRet* msg);
	void			HanderLmgRecycleDeskId(LMsgLMG2TS_RecycleDeskId* msg);

	void			HanderQueryTeam(LMsgC2SQuerTeam* msg, Llong nUserUnique);
	void			HanderReqJoinTeam(LMsgC2SReqJoin* msg, Llong nUserUnique);
	void			HanderReqTeamList(LMsgC2SReqTeamList* msg, Llong nUserUnique);
	void			HanderGetTeamInfo(LMsgC2SGetTeamInfo* msg, Llong nUserUnique);
	void			handerMemberList(LMsgC2SGetTeamMemberList* msg, Llong nUserUnique);
	void			HanderCancelJoin(LMsgC2SCancelJoin* msg, Llong nUserUnique);
	void			HanderLeaveTeam(LMsgC2SLeaveTeam* msg, Llong nUserUnique);
	void			HanderGetVideoLog(LMsgC2SGetVideoLog* msg, Llong nUserUnique);
	void			HanderGetAllTeamDesk(LMsgC2SGetAllTeamDesk* msg, Llong nUserUnique);
	void			HanderSubscribeTeamDesk(LMsgC2SSubscribeTeamDesk* msg, Llong nUserUnique);
	void			HanderGetTeamOptLog(LMsgC2SSubscribeTeamDesk* msg, Llong nUserUnique);
	void			HanderGetRangeVideoLog(LMsgC2SGetRangeVideoLog* msg, Llong nUserUnique);

	void			HanderUserCreatePlayConfig(LMsgC2SCreatePlayConfig* msg, Llong nUserUnique);
	void			HanderUserFastPlay(LMsgC2SFastPlay* msg, Llong nUserUnique);
	void			ConnectToLogicManager();
	//void			ReconnectManagerLogic();
	void			SendHeartBeatToLogicManager();

	bool			HanderCheckMd5(std::map<Lstring, Lstring>& param);

	void			HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg);
	void			HanderKeepAlive(LMsgS2SKeepAlive* msg);
	//创建快速场
	void			HanderUserMatchInfo(LMsgMa2LM_UserMatchInfo* msg);
	void			HanderLMGSendDeskID(LMsgLMG2TS_ReqDeskRet* msg);
	void			HanderOptMemberReq(LMsgC2SOptMember* msg, Llong nUserUnique);
	void			HanderOptTeamInfo(LMsgC2SOptTeamInfo* msg, Llong nUserUnique);
	void			HanderAdminInviteOther(LMsgC2SAdminInviteOther* msg,Llong nUserUnique);

	void			HanderReqExchageGold(LMsgC2SReqExchageGold* msg, Llong nUserUnique);
	void			HanderReqExchageGoldLog(LMsgC2SReqExchageGoldLog* msg, Llong nUserUnique);
	void			HanderCanncelExchageGold(LMsgC2SCanncelExchageGold* msg, Llong nUserUnique);
	void            HanderInviteInRoom(LMsgC2SInviteInRoom* msg, Llong nUserUnique);
	void			HanderLM2TSReqExchageGold(LMsgLMG2TS_ReqExchageGold* msg);

public:
	//房卡相关操作
	void			ModifyRoomCardCount(Lint userId, Lint nShift, Lint nReason);

private:
	void			GetTeamInfo_(LMsgS2CQueryTeamInfo &teamInfoMsg, Lint nUserID, Lint nTeamID);

protected:
	void			ClearOldTeamVideoLog();

private:
	LTime			m_tickTimer;

	Llong			m_30MSecTick;//30毫秒循环一次

	Llong			m_200MSecTick;//200毫秒循环一次

	Llong			m_1SecTick;//1秒循环一次

	Llong			m_5SecTick;	//5秒循环一次

	Llong			m_30SecTick;//30秒循环一次

	Llong			m_600SecTick;//5分钟秒循环一次

	Llong			m_30MinTick;// 三十分钟执行一次循环 [2017-7-25 willing]

private:
	TCPClientPtr	m_logicManagerClient;	// 链接LogicManager [2017-7-10 willing]
	Llong			m_lastlogicMgrTimestamp;// 最后收到 LogicManager 的时间戳[2017-7-10 willing]

	std::map<Lint, SGateInfo> m_gateInfo;	// 所有连接的GateServer [2017-7-10 willing]
	std::map<Lint, SLogicInfo> m_logicInfo;	// 所有连接的LogicServer [2017-7-10 willing]

	DBSession		m_dbsession;
	std::unordered_set<int>	m_MatchJoinSet;			//参加比赛场的玩家id
	GarbageCollectionService m_gcService;
};

#define gWork Work::Instance()

#endif