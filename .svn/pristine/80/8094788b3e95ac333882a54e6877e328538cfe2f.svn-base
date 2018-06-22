#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "DBSession.h"
#include "DbServer.h"
#include "FinancialService.h"
#include "DUser.h"
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
	void			SaveCurrentOnline();
	LTime&			GetCurTime();

public:	
	void		HanderLogicLogin(LMsgL2CeLogin* msg);
	void		HanderLogicGateInfo(LMsgL2CeGateInfo* msg);
	void		HanderLogicSaveUser(LMsgL2CeSaveUser* msg);
	void		HanderLogicSaveCardInfo(LMsgL2CeSaveCardInfo* msg);
	void		HanderLogicLogout(LMsgKick*msg);
	GateInfo*	GetGateUsefulInfo(Lint logicID);
	void		SendMsgToLogic(LMsg& msg, Lint logicID);
	void		SendMsgToLogic(LMsg& msg, Lint logicID, int iLimitCountInSendPool);
	void		HanderLogicUserLogin(LMsgL2CeUserServerLogin* msg);
	void		HanderLogicUserLogout(LMsgL2CeUserServerLogout* msg);
	Lstring&	getServerNameByID(Lint serverID);
	void		HanderLogicUserReLogin(LMsgL2CeUerReLogin* msg);
	void		HanderLogicManagerCouponsInfo(Lint serverId=0);
	void		HanderLogicManagerCouponsExchangeInfo(LMsgLMG2CEUserCouponsExchangeInfo* msg);
	void		HanderLogicManagerCouponsExchangeRecord(LMsgLMG2CEUserCouponsExchangeRecord* msg);
	void		HanderLogicManagerCouponChanged(LMsgLMG2CeCouponChanged* msg);
	void		HanderLogicManagerOfflineUserCardChaged(LMsgLMG2CeOfflineUserCardChanged* msg);
	void		HanderLogicManagerCardExchange(LMsgLMG2CeCardExchange* msg);
	void        HanderLogicManagerUserPlayCount(LMsgLMG2CeSaveUserPlayCount* msg);
	void        HanderLogicManagerGoldChange(LMsgLMG2CeGoldChange* msg);

public:
	void		HanderGMCharge(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderGMChargeDiamond(std::map<Lstring,Lstring>& param,LSocketPtr sp);
	void		HanderGMHorse(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMCreateAccount(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMUpdateAccount(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMUpdateAccountInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMBuyInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMGetUserInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMModifyPlayerType(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMOnlineCount(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	void		HanderGMCloseServer(std::map<Lstring, Lstring>& param, LSocketPtr sp);
	bool		HanderCheckMd5(std::map<Lstring, Lstring>& param);
	void		HanderGMPurchase(std::map<Lstring,Lstring>& param,LSocketPtr sp);

public:
	void		HanderLGWrappedMsg(LMsgLGCEUserMsgWrapper* msg);
	Lint		HanderUserLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret, LSocketPtr loginSocketP);
	void		HanderUserLocalLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret);
	void		HanderUserAccountLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret);
	Lint		HanderUserWechatLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret, LSocketPtr loginSocketP);
	void		HanderLGHeartbeat(LMsgLG2CEHeartBeat* msg);
	void        HanderKeepAlive(LMsgS2SKeepAlive* msg);
	DBSession&	GetDbSession();
public:
	void			initGiftInfo();
public:
	//处理http消息 ;
	void			HanderHttp(LMsgHttp* msg);	
	//处理客户端连接上来消息;
	void			HanderClientIn(LMsgIn* msg);
	//处理客户端掉线的消息 ;
	void			HanderUserKick(LMsgKick* msg);
	
	//处理玩家登陆的消息; 
	void			HanderUserMsg(LMsgC2SMsg* msg);

	//socket管理;
	void			AddSp(LSocketPtr sp);
	void			DelSp(LSocketPtr sp);

private:
	Lstring			GetHttpErrDescription(Lint errCode);
	Lstring			FormatHttpResponseMsg(Lint errCode);
	bool			CheckHttpParameters(std::map<Lstring, Lstring>& param, int keyNum, const Lstring* keyList);
	void			NotifyPurchasingResultToLogicServer(DUser* user, bool succeed, Lint itemId, Lint diamondPaid, Lint cardAdded, Lint cardType, Lint operType);
	void			NotifyChargingResultToLogicServer(DUser* user, bool succeed, Lint diamondCharged);

private:
	void		SaveRoomCardChangeLog2DB__(int nDBID, int nReason, Ldouble nShift, int nResulst, const std::string& Admin, Lint nTeamID);
	void		HanderGMCharge_online(std::map<Lstring, Lstring>& param, LSocketPtr sp, DUser* user, Ldouble nShift, Lint nReason);
	void		HanderGMCharge_onoff(std::map<Lstring, Lstring>& param, LSocketPtr sp, Lint nUserId, Lint nTeamId, Ldouble nShift, Lint nReason, Lint appId);

private:
	LTime			m_tickTimer;

	Llong			m_30MSceTick;//30毫秒循环一次;

	Llong			m_200MSceTick;//200毫秒循环一次;

	Llong			m_1SceTick;//1秒循环一次;

	Llong			m_30SceTick;//30秒循环一次;

	Llong			m_600SceTick;//5分钟秒循环一次;

private:
	//	LSocketPtr	m_logicClient;
	//	std::map<Lint, GateInfo> m_gateInfo;
	std::map<Lint, LSocketPtr> m_logicClient;
	std::map<Lint, std::map<Lint, GateInfo> > m_gateInfo;
	std::map<LSocketPtr, Lint>	m_spMap;
	std::map<Lint, Lstring> serverInfo;
    std::map<Lint, gift_info> m_giftInfo;
	DBSession				m_dbsession;
	FinancialService		m_fService;
	GarbageCollectionService	m_gcService;
};


#define gWork Work::Instance()

#endif