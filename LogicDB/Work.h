#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "DBSession.h"

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
	//处理客户端连接上来消息;
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 ;
	void			HanderUserKick(LMsgKick* msg);

	//清除24小时以上日志， 每天早上6点执行 ;
	void			ClearOldLogVideo(Lint hour);

public:

	void			SendToLogic(LMsg& msg);

	//logicmanager登录进来;
	void			HanderLogicManagerLogin(LMsgLMG2LdbLogin* msg);

	//logic登录进来;
	void			HanderLogicLogin(LMsgL2LdbLogin* msg);

	//logic请求玩家战绩;
	void			HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg);

	//logic请求玩家录像;
	void			HanderLogicReqVideo(LMsgL2LDBReqVideo* msg);

	//logic请求保存战绩;
	void			HanderLogicReqSaveLog(LMsgL2LDBSaveLogItem* msg);

	//logic请求保存录像;
	void			HanderLogicReqSaveVideo(LMsgL2LDBSaveVideo* msg);

	//处理抽奖信息请求;
	void		HandleLogicReqActive(LMsgL2LDBReqActive* msg);

	//保存抽奖信息;
	void		HandleLogicSaveSyncDraw(LMsgL2LDBSyncDraw* msg);

	//修改抽奖电话;
	void		HandleLogicUpdatePhone(LMsgL2LDBSyncPhone* msg);

	//logic请求某分享录像;
	void			HanderLogicReqShareVideo(LMsgL2LDBGetShareVideo* msg);

	//logic请求玩家分享的录像;
	void			HanderLogicGetShareVideo(LMsgL2LDBGetSsharedVideo* msg);

	//logic请求某轮玩家战绩;
	void			HanderLogicReqOneVipLog(LMsgL2LDBReqOneVipLog* msg);

	//logic请求玩家获取某分享录像Id;
	void			HanderLogicGetShareVideoId(LMsgL2LDBGGetShareVideoId* msg);

	//保存share video id;
	void			HanderLogicSaveShareVideoId(LMsgL2LDBSaveShareVideoId* msg); 

	//心跳;
	void		HandleLMGHeartBeat(LMsgLMG2LDBHEARTBEAT* msg);

	//心跳;
	void		HandleLHeartBeat(LMsgL2LDBHEARTBEAT* msg);

	//任务
	void		HandleExecSQL(LMsgL2LDBExecSql* msg);							//执行sql
	void		HandleReqTaskProgress(LMsgLMG2LDBReqTaskProgress* msg);			//请求任务进度
	void		HandleReqBindingRelationships(LMsgLMG2LDBReqBindingRelationships* msg);		//请求绑定关系
	void		HandleInsertTaskProgress(LMsgLMG2LDBInsertTaskProgress* msg);	//插入任务进度
	void		HandleInsertBindingRelationship(LMsgLMG2LDBInsertBindingRelationship* msg);

	//十一活动
	void		HandleLogicReqSignCollectWin(LMsgL2LDBReqSignCollectWin* msg);	//请求2016十一活动(签到，集字，中奖）
	void		ResLoadActiveSign(Lint userId);
	void		ResLoadActiveCollect(Lint userId);
	void		ResLoadActiveWin(Lint userId);

	//
	void		HandleLogicSaveSignCollectWin(LMsgL2LDBSaveSignCollectWin* msg);	//保存签到，集字，中奖结果
	void		SaveSign(Lint userid, Lint date);
	void		SaveCollect(Lint isUpdate, Lint userid, Lint date, const std::vector<Lint>& words, Lint drawChanceCount, Lint hadGift);
	void		SaveWin(Lint userId, Lint date, const Lstring& name);

	void        HanderKeepAlive(LMsgS2SKeepAlive* msg);

private:
	bool		CheckDelLogVideoTime(Lint hour, Lint min);
	void		SendToClient(LSocketPtr client, LMsg& msg);

	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30毫秒循环一次

	Llong		m_200MSceTick;//200毫秒循环一次

	Llong		m_1SceTick;//1秒循环一次

	Llong		m_30SceTick;//30秒循环一次

	Llong		m_600SceTick;//5分钟秒循环一次

private:
	LSocketPtr	m_logicManager;//链接logic

	std::map<Lint, LSocketPtr> m_logicServer;

	DBSession		m_dbsession;
};


#define gWork Work::Instance()

#endif