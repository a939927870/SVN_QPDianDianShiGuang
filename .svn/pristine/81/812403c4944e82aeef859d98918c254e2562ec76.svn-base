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
	
	//md5加密算法;
	Lstring			Md5Hex(const void* data,Lsize size);
public:
	//处理http消息; 
	void			HanderHttp(LMsgHttp* msg);	
	//处理客户端连接上来消息;
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息; 
	void			HanderUserKick(LMsgKick* msg);

	//处理玩家登陆的消息; 
	void			HanderUserLogin(LMsgLMG2LLogin* msg);
	void            HanderUserLoginGold(LMsgLMG2LLogin* msg);

	//处理玩家断线重连的消息;
	void			HanderUserReLogin(Lint id,Lint seed,Lint gateId,Lint gateUserId,Lstring& md5,Lstring& ip);
	
	//处理玩家登陆的消息;
	void			HanderDeskUserLogin(User* pUser, Lint gateId, Lint gateUserId, Lstring& ip, bool firstLogin = false, Lstring buyInfo = "", Lint hide = 0, Lint gameZone = game_zone_roomcard);

	//处理玩家登出的消息; 
	void			HanderUserLogout(Lint gateId, Lint geteUserId);

	//处理玩家创建桌子;
	void			handerLMGCreateDesk(LMsgLMG2LCreateDesk* msg);

	//玩家创建金币场桌子;
	void            handerLMGIntoGoldDesk(LMsgLMG2LIntoGoldDesk* msg);

	//处理玩家加入桌子;
	void			handerLMGAddToDesk(LMsgLMG2LAddToDesk* msg);

	//处理Gate心跳;
	void			HanderGateHeartBeat(LMsgG2LHEARTBEAT* msg);
	
	//处理心跳;
	void			HanderLogicManagerHeartBeat(LMsgLMG2LHEARTBEAT_BACK* msg);

	//处理LDB心跳
	void			HanderLogicDBHeartBeat(LMsgLDB2LHEARTBEAT_BACK* msg);
	//回收桌子ID（创建失败，房间结束）;
	void			RecycleDeskId(Lint deskID);

	//回收金币场桌子的一个位置（创建失败，房间结束）;
	void			RecycleGoldDeskPlace(Lint deskID, Lint gameType);
public:
	void			ConnectToLogicManager();

	void			SendToLogicManager(LMsg& msg);

	void			SendHeartBeatToLogicManager();

	void			SendHeartBeatToLogicDB();
	//center服务器关闭;
	void			HanderCenterLogout();

	void			HanderDBServerLogout();

public:
	void			HanderGateLogin(LMsgG2LLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	GateInfo*		GetGateInfoBySp(LSocketPtr sp);
	GateInfo*		GetGateInfoById(Lint id);
	void			DelGateInfo(Lint id);

	void			HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate);

	void			HanderKeepAlive(LMsgS2SKeepAlive* msg);
	void			HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg);
public:
	//DB链接相关
	void			ConnectToDb();
	void			SendMsgToDb(LMsg& msg);

public://俱乐部------------------------------------------------------------------------
	void			HanderTsAddDeskRet(LMsgTS2L_AddDeskRet* msg);
	void			ConnectToTeamServer(Lstring strServerIp, Lshort nPort);
	void			SendMsgToTeamServer(LMsg& msg);
	void			HanderTeamServerInfo(LMsgLMG2L_TeamServer* msg);
	//俱乐部------------------------------------------------------------------------

private:
	void			SendLogicLoginToLM();
	void			SendLogicLoginToDB();

private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次

	Llong		m_200MSceTick;//200毫秒循环一次

	Llong		m_1SceTick;//1秒循环一次

	Llong		m_5SceTick;//5秒循环一次

	Llong		m_30SceTick;//30秒循环一次

	Llong		m_600SceTick;//5分钟秒循环一次

private:
	TCPClientPtr	m_logicManager;//链接center
	TCPClientPtr	m_dbClient;

	std::map<Lint, GateInfo> m_gateInfo;

	bool				m_loginLMFirst;
	
	//////////////////////////////////////////////////////////////////////////
	bool		m_isConnect;
	GarbageCollectionService	m_gcService;
	TCPClientPtr	m_teamServer;	// 连接 team Server [2017-7-10 willing]

public:
	void HanderUserModifyRoomCard(LMsgLMG2LModifyRoomCard* msg);
};

#define gWork Work::Instance()

#endif