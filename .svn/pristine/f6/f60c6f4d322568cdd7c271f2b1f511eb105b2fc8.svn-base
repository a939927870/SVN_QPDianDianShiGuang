#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "GateUser.h"
#include "LMsgS2S.h"
#include "GarbageCollectionService.h"

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
	
public:
	//处理http消息 
	void			HanderHttp(LMsgHttp* msg);	
	//处理客户端连接上来消息
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 
	void			HanderUserKick(LMsgKick* msg);
	//处理LogicManager同步的Logic消息
	void			HanderLogicInfo(LMsgLMG2GateLogicInfo* msg);
	//处理跑马灯消息
	void			HanderHorseInfo(LMsgLMG2GHorseInfo* msg);
	//修改User状态
	void			HanderModifyUserState(LMsgL2GModifyUserState* msg);
	//收到心跳
	void			HanderLogicHeartBeat(LMsgL2GHEARTBEAT_BACK* msg);
	//收到心跳
	void			HanderLogicManagerHeartBeat(LMsgLMG2GHEARTBEAT_BACK* msg);

	//处理TeamServer消息
	void			HanderTeamServerInfo(LMsgLMG2G_TeamServer* msg);
public:
	//发送心跳包
	void			SendHeartBeatToLogic();	

	void			SendHeartBeatToLogicManager();

	void			ConnectToLogicManager();
	
	//发送消息到所有User
	void			SendMessageToAllUse(LMsg* msg);

	void			SendToLogicUserOut(GateUser* user);
	//void			HanderLogicUserOut(Lint id);

	void			HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg);

	LogicInfo*		GetLogicInfoBySp(LSocketPtr sp);
	LogicInfo*		GetLogicInfoById(Lint id);

	//socket管理;
	void			AddSp(LSocketPtr sp);
	void			DelSp(LSocketPtr sp);
protected:
	//true: 查找到并且删除， false：没有查询到;
	bool			_findLogicSPAndDel(LSocketPtr sp); 
	bool			_isLogicSP(LSocketPtr sp);
	TCPClientPtr	GetLogicClientBySP(LSocketPtr sp);

private:
	void			SendGateLoginToLM();
	void			SendGateLoginToLogic(TCPClientPtr logicSP);

	void			SendMsgToTeamServer(LMsg& msg);
	TCPClientPtr	GetTeamServerTCPClientBySP(LSocketPtr sp);
	bool			IsTeamServerSP(LSocketPtr sp);
	void			SendGateLoginToTeamServer(TCPClientPtr sp);

private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次;

	Llong		m_200MSceTick;//200毫秒循环一次;

	Llong		m_1SceTick;//1秒循环一次;

	Llong		m_5SceTick;//5秒循环一次;
	
	Llong		m_30SceTick;//30秒循环一次;
	
	Llong		m_600SceTick;//5分钟秒循环一次;

private:
	TCPClientPtr  m_logicManager;
	std::map<Lint, LogicInfo>	m_logicInfo;
	std::map<LSocketPtr, Lint>	m_spMap;
	GarbageCollectionService	m_gcService;
	std::map<Lint, TeamServerInfo>	m_TeamServerInfoMap;

};


#define gWork Work::Instance()

#endif