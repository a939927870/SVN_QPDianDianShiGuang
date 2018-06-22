#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "OutsideNet.h"
#include "HttpRequest.h"
#include "DbServer.h"
#include "TeamManager.h"
#include "LMsgCT.h"
#include "GateUserManager.h"
#include "AlertWork.h"
#include "TeamErrorCode.h"
#include "DeskManager.h"
#include "HanderHttp.h"

//////////////////////////////////////////////////////////////////////////
static const char* TEAMP_SERVER_PATH = "/cgi-bin/admin";
//////////////////////////////////////////////////////////////////////////

//初始化
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看
	LLog::Instance().SetFileName("TeamServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	//设置log级别
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	//网络消息初始化
	LMsgFactory::Instance().Init();

	//外部端口初始化
	if (!gOutsideNet.Init())
	{
		LLOG_ERROR("gOutsideNet.Init error");
		return false;
	}

	// 数据初始化 [2017-7-14 willing]
	if ( !gDbServer.Init() )
	{
		LLOG_ERROR("gDbServer.Init error");
		return false;
	}

	// 初始化报警线程 [2017-7-23 willing]
	if ( !gAlertWork.Init() )
	{
		LLOG_ERROR("gAlertWork.Init error");
		return false;
	}

	if (!m_dbsession.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), 
							gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		return false;
	}

	if ( !gTeamManager.Init() )
	{
		LLOG_ERROR("TeamInfo Load error");
		return false;
	}

	// HTTP 监听初始化 [2017-7-12 willing]
	if ( !gHttpNet.Init() )
	{
		LLOG_ERROR("gHttpNet.Init error");
		return false;
	}

	m_logicManagerClient = NULL;

	LTime cur;

	m_lastlogicMgrTimestamp = cur.MSecs();

	m_30MSecTick = cur.MSecs();
	m_200MSecTick = cur.MSecs();
	m_1SecTick = cur.MSecs();
	m_5SecTick = cur.MSecs();
	m_30SecTick = cur.MSecs();
	m_600SecTick = cur.MSecs();
	m_30MinTick = cur.MSecs();
	LLOG_ERROR("Sever Compile date: %s, %s", __DATE__, __TIME__);
	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	//主线程开启
	LRunnable::Start();

	gAlertWork.Start();

	//开启监听
	gOutsideNet.Start();

	//数据库线程开启
	gDbServer.Start();

	ConnectToLogicManager();

	gHttpNet.Start();

	m_gcService.Start();
}

//等待
void Work::Join()
{
	//等待网络线程终止
	gOutsideNet.Join();

	gHttpNet.Join();

	gAlertWork.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止
void Work::Stop()
{
	m_logicManagerClient.reset();
	gOutsideNet.Stop();

	gHttpNet.Stop();

	gDbServer.Stop();

	gAlertWork.Stop();

	LRunnable::Stop();
}

void Work::Clear()
{
}

void Work::Run()
{
	while(!GetStop())
	{
		//这里处理游戏主循环
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
			Tick(cur);	
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			continue;
		}
		else
		{
            HanderMsg(msg);

			m_gcService.Collect(msg);
		}
	}
}

void Work::Tick(LTime& cur)
{
	//30毫秒循环一次
	if (cur.MSecs() - m_30MSecTick > 30)
	{
		m_30MSecTick = cur.MSecs();
	}

	//200毫秒循环一次
	if (cur.MSecs() - m_200MSecTick > 200)
	{
		m_200MSecTick = cur.MSecs();
	}

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SecTick > 1000)
	{
		m_1SecTick = cur.MSecs();
		//CheckConnect();
	}

	//5秒循环一次
	if (cur.MSecs() - m_5SecTick > 5000)
	{
		m_5SecTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SecTick > 30*1000)
	{
		m_30SecTick = cur.MSecs();

		gConfig.Init();
		LLog::Instance().SetLogLevel(gConfig.GetLogLevel());
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SecTick > 5*60 * 1000)
	{
		m_600SecTick = cur.MSecs();

		gDeskManager.ClearTimeOutDesk();
	}

	// 30 分钟执行一次 [2017-7-25 willing]
	if ( cur.MSecs() - m_30MinTick > 30 * 60 * 1000)
	{
		// 删除15天以前的工会战绩 [2017-7-25 willing]
		ClearOldTeamVideoLog();
		m_30MinTick = cur.MSecs();
	}
	if (m_logicManagerClient)
	{
		m_logicManagerClient->OnTick(cur);
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}

	switch(msg->m_msgId)
	{
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE_ACK:			
		HanderKeepAliveAck((LMsgS2SKeepAliveAck*)msg);				
		break;
	case MSG_S_2_S_KEEP_ALIVE:				
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);					
		break;
	case MSG_HTTP:
		HanderHttpMsg((LMsgHttp*)msg);
		break;
	case MSG_CLIENT_KICK:
		HanderClientKick((LMsgKick*)msg);
		break;
	case MSG_LMG_2_TS_HEARTBEAT:
		HanderLogicManagerHeartBeat((LMsgLMG2TS_Heartbeat*)msg);
		break;
	case MSG_L_2_TS_LOGIN:
		HanderLogicServerLogin((LMsgL2TS_Login*) msg);
		break;
	case MSG_L_2_TS_HEARTBEAT:
		HanderLogicServerHeartBeat((LMsgL2TS_Heartbeat*) msg);
		break;
	case MSG_G_2_TS_HEARTBEAT:
		HanderGateServerHeartBeat((LMsgG2TS_Heartbeat*) msg);
		break;
	case MSG_G_2_TS_LOGIN:
		HanderGateServerLogin((LMsgG2TS_Login*) msg);
		break;
	case MSG_LMG_2_TS_LG_LOGOUT:
		HanderLogicOrGateLogout((LMsgLMG2TS_LogicGateLogout*)msg);
		break;
	case MSG_G_2_T_USER_MSG:
		HanderGateMsg((LMsgG2TUserMsg*)msg, msg->m_sp);
		break;
	case MSG_LMG_2_TS_USER_LOGIN:
		HanderUserLogin((LMsgLMG2TS_UserLogin*) msg);
		break;
	case MSG_LMG_2_TS_USER_LOGOUT:
		HanderUserLogout((LMsgLMG2TS_UserLogout*) msg);
		break;
	case MSG_LMG_2_TS_CREATE_ROOM:
		HanderCreateRoom((LMsgLMG2TS_CreateDesk*) msg);
		break;
	case MSG_LMG_2_TS_ADD_ROOM:
		HanderLM2TSAddDesk((LMsgLMG2TS_AddDesk*)msg);
		break;
	case MSG_LMG_2_TS_GIVE_BACK:
		HanderGiveBackCard((LMsgLMG2TS_GiveBackCard*)msg);
		break;
	case MSG_L_2_TS_DEL_CARD:
		HanderDelCard((LMsgL2TS_DelCard*)msg);
		break;
	case MSG_L_2_TS_VIDEO_LOG:
		HanderVideoLog((LMsgL2TS_VideoLog*)msg);
		break;
	case MSG_L_2_TS_ADD_DESK:
		HanderAddDesk((LMsgL2TS_AddDesk*)msg);
		break;
	case MSG_L_2_TS_GAME_STATE:
		HanderGameStateChange((LMsgLS2TS_GameState*)msg);
		break;
	case MSG_LMG_2_TS_USER_INFO:
		HanderUserInfo((LMsgLMG2TS_UserInfo*)msg);
		break;
	case MSG_L_2_TS_NEW_TEAM_DESK:
		HanderNewTeamDesk((LMsgL2TS_NewTeamDesk*)msg);
		break;
	case MSG_L_2_TS_TEAM_DESK_USER_CHG:
		HanderTeamDeskUserChg((LMsgL2TS_TeamDeskUserChg*)msg);
		break;
	case MSG_L_2_TS_DEL_TEAM_DESK:
		HanderDelTeamDesk((LMsgL2TS_DelTeamDesk*)msg);
		break;
	case MSG_LMG_2_TS_USER_ONLINE_RET:
		HanderUserOnlineRst((LMsgLMG2TS_UserOnLineRet*)msg);
		break;
	case MSG_LMG_2_TS_RecycleDeskId:
		HanderLmgRecycleDeskId((LMsgLMG2TS_RecycleDeskId*)msg);
		break;
	case MSG_LMG_2_TS__REQ_DESKID_REQ:
		HanderLMGSendDeskID((LMsgLMG2TS_ReqDeskRet*)msg);
		break;
	case MSG_MA_2_LM_USER_MATCH_INFO:
		HanderUserMatchInfo((LMsgMa2LM_UserMatchInfo*)msg);
		break;
	case MSG_LMG_2_TS_REQ_EXCHAGEGOLDM:
		HanderLM2TSReqExchageGold((LMsgLMG2TS_ReqExchageGold*)msg);
		break;
	default:
		LLOG_ERROR("Work::HanderMsg -- Wrong msg. msgid = %d %x", msg->m_msgId, msg->m_msgId);
		break;
	}
}

void Work::HanderUserMatchInfo(LMsgMa2LM_UserMatchInfo* msg)
{
	if (USER_JOINMATCH_JOIN == msg->m_Type)
	{
		m_MatchJoinSet.insert(msg->m_nUserDBID);
	}
	else
	{
		m_MatchJoinSet.erase(msg->m_nUserDBID);
	}
}
void Work::ConnectToLogicManager(void)
{
#ifdef _DEBUG
	KeepAliveSetting setting = { true, 150000, 60000 };
#else // _DEBUG
	KeepAliveSetting setting = { true, 30000, 60000 };
#endif // _DEBUG
	bool succeed = false;
	//	while (!succeed)
	{
		m_logicManagerClient.reset(new TCPClient(gOutsideNet, gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort(), setting));
		succeed = m_logicManagerClient->Connect();
		if (succeed)
		{
			//		LLOG_ERROR("Work::ConnectToLM end OK %s:%d", gConfig.GetLMIp().c_str(), gConfig.GetLMPort());
			//	break;
		}
		else
		{
			LLOG_ERROR("Since failed to connect LM on %s:%d, Retry...", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
			Sleep(1000);
			Stop();
		}
	}
	LLOG_ERROR("Work::ConnectToLM OK:%d, %s:%d", succeed ? 1 : 0, gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
}

//void Work::ReconnectManagerLogic()
//{
//	try
//	{
//		if (m_logicManagerClient)
//		{
//			m_logicManagerClient->Stop();
//			m_logicManagerClient.reset();
//		}
//
//		m_logicManagerClient = gOutsideNet.GetNewSocket(boost::bind(&LNet::RecvMsgPack, &gOutsideNet, boost::placeholders::_1, boost::placeholders::_2), NULL);
//		if (m_logicManagerClient)
//		{
//			m_logicManagerClient->AsyncConnect(gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort());
//			LLOG_ERROR("ReconnectManagerLogic::Disconected with logicmanager, reconnect...: ip:%s, prot:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
//		}
//		else
//		{
//			LLOG_ERROR("Work::ReconnectManagerLogic:: m_logicManger is null.");
//		}
//	}
//	catch (...)
//	{
//		LLOG_ERROR("Work::ReconnectManagerLogic:: catch exception.");
//	}
//}

void Work::SendToLogicManager(LMsg& msg)
{
	if (m_logicManagerClient)
	{
		m_logicManagerClient->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("Work::SendToLogicManager error:Disconected with logicmanager");
	}
}

void Work::SendToGate(LMsg& msg, Llong nUserUnique)
{
	Lint nGateId = gGateUserManager.GetUserGateID(nUserUnique);
	std::map<Lint, SGateInfo>::iterator iter = m_gateInfo.find(nGateId);
	if ( iter != m_gateInfo.end() )
	{
		LMsgT2GUserMsg userMsg;

		userMsg.m_id = gGateUserManager.GetGateUserID(nUserUnique);
		userMsg.m_dataBuff = msg.GetSendBuff();

		SGateInfo& gateInfo = iter->second;
		gateInfo.pSocketPtr->Send(userMsg.GetSendBuff());
	}
}

void Work::SendToGateByUserID(LMsg& msg, Lint nUserID)
{
	Llong nUserUnique = gGateUserManager.GetUnique(nUserID);
	if ( nUserUnique != 0 )
	{
		SendToGate(msg, nUserUnique);
	}
}

//// 检测网络连接 [2017-7-10 willing]
//void Work::CheckConnect()
//{
//	LTime cur;
//	if ( (cur.MSecs() - m_lastlogicMgrTimestamp) >= 10*1000 )
//	{
//		// 10秒没有心跳，重连 [2017-7-10 willing]
//		ReconnectManagerLogic();
//		m_lastlogicMgrTimestamp = cur.MSecs();
//	}
//}

DBSession&	Work::GetDbSession()
{
	return m_dbsession;
}

void Work::SendHeartBeatToLogicManager()
{
	LMsgTS2LMG_Heartbeat Heartbeat;
	SendToLogicManager(Heartbeat);
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if ( NULL == msg )
	{
		return;
	}
	if ( msg->m_sp == m_logicManagerClient)
	{
		LMsgTS2LMG_Login logic_mgr_login;
		logic_mgr_login.m_TeamServerID = gConfig.GetTeamServerID();
		logic_mgr_login.strTeamServerIp = gConfig.GetOutsideIp();
		logic_mgr_login.nTeamServerPort = gConfig.GetOutsidePort();
		m_logicManagerClient->Send(logic_mgr_login.GetSendBuff());
		m_lastlogicMgrTimestamp = LTime().MSecs();
		LLOG_DEBUG("Work::HanderClientIn connect to LM");
	}
}

void Work::HanderHttpMsg(LMsgHttp* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_head.size() > 4 && msg->m_head.substr(0, 4) == "GET ")
	{
		/*std::string::size_type pos = msg->m_head.find_last_of("HTTP");
		if (pos == std::string::npos || pos < 8)
		{
			LLOG_ERROR("Work::HanderHttp GET error");
			gHanderHttp.SendRet("{\"errorCode\":5,\"errorMsg\":\"req function error\"}", msg->m_sp);
			return;
		}

		std::string totalUrl = msg->m_head.substr(4, pos - 8);*/
		std::string::size_type pos = msg->m_head.find("\r\n");
		if (pos == std::string::npos || pos < 13)
		{
			LLOG_ERROR("Work::HanderHttp GET error");
			gHanderHttp.SendRet("{\"errorCode\":5,\"errorMsg\":\"req function error\"}", msg->m_sp);
			return;
		}

		std::string totalUrl = msg->m_head.substr(4, pos - 13);
		std::string::size_type pos2 = totalUrl.find('?', 0);
		if (pos2 == std::string::npos)
		{
			LLOG_ERROR("Work::HanderHttp GET param error");
			gHanderHttp.SendRet("{\"errorCode\":6,\"errorMsg\":\"req param error\"}", msg->m_sp);
			return;
		}

		std::string reqType = totalUrl.substr(0, pos2);
		std::string reqParam = totalUrl.substr(pos2 + 1, totalUrl.size() - pos2);

		if (reqType == TEAMP_SERVER_PATH)
		{
			HttpParamMap paraMaps;
			HttpRequest::Instance().DeodeUrlParams(reqParam, paraMaps, true);

			//if (!HanderCheckMd5(paraMaps))
			//{
			//	//gHanderHttp.SendRet("{\"errorCode\":3,\"errorMsg\":\"sign error\"}", msg->m_sp);//签名错误
			//	return;
			//}

			if (paraMaps["msg"] == "create")
			{
				LLOG_DEBUG("Club------Recv create msg,Begin Create club!");
				gHanderHttp.HanderHttpCreatTeam(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "modify")
			{
				LLOG_DEBUG("Club------Recv modify msg");
				gHanderHttp.HanderHttpModifyTeam(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "memopt")
			{
				LLOG_DEBUG("Club------Recv memopt msg");
				gHanderHttp.HanderHttpJoinTeam(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "cardalert")
			{
				LLOG_DEBUG("Club------Recv cardalert msg");
				gHanderHttp.HanderHttpCardAlert(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "marklog")
			{
				LLOG_DEBUG("Club------Recv marklog msg");
				gHanderHttp.HanderHttpMarkLog(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "perdaylimit")
			{
				LLOG_DEBUG("Club------Recv perdaylimit msg");
				gHanderHttp.HanderHttpPerDayLimit(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "delteam")
			{
				LLOG_DEBUG("Club------Recv delteam msg");
				gHanderHttp.HanderHttpDelTeam(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "inviteusers")
			{
				LLOG_DEBUG("Club------Recv inviteusers msg");
				gHanderHttp.HanderHttpAddMembers(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "inviteuser")
			{
				LLOG_DEBUG("Club------Recv inviteuser msg");
				gHanderHttp.HanderHttpAddMember(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "reqjointeam")
			{
				LLOG_DEBUG("Club------Recv reqjointeam msg");
				gHanderHttp.HanderHttpOtherJoinTeam(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "userinfo")
			{
				LLOG_DEBUG("Club------Recv userinfo msg");
				gHanderHttp.HanderHttpQueryUserInfo(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "userinfos")
			{
				gHanderHttp.HanderHttpQueryUserInfos(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "exchagelist")
			{
				gHanderHttp.HanderHttpQueryExchageList(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "optexchage")
			{
				gHanderHttp.HanderHttpOptExchage(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "teamlist")
			{
				LLOG_DEBUG("Club------Recv teamlist msg");
				gHanderHttp.HanderHttpQueryTeamList(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "teaminfo")
			{
				LLOG_DEBUG("Club------Recv teaminfo msg");
				gHanderHttp.HanderHttpQueryTeamInfo(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "memlist")
			{
				LLOG_DEBUG("Club------Recv memlist msg");
				gHanderHttp.HanderHttpQueryMemberList(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "memcount")
			{
				LLOG_DEBUG("Club------Recv memcount msg");
				gHanderHttp.HanderHttpQueryMemberCount(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "loglist")
			{
				LLOG_DEBUG("Club------Recv loglist msg");
				gHanderHttp.HanderHttpQueryLogList(paraMaps, msg->m_sp);
			}
			else if (paraMaps["msg"] == "winnum")
			{
				LLOG_DEBUG("Club------Recv winnum msg");
				gHanderHttp.HanderHttpQueryWinnum(paraMaps, msg->m_sp);
			}
			else
			{
				LLOG_DEBUG("Club------Recv error msg");
				gHanderHttp.SendRet("{\"errorCode\":2,\"errorMsg\":\"msg type error\"}", msg->m_sp);//消息类型错误
			}
		}
		else
		{
			gHanderHttp.SendRet("{\"errorCode\":1,\"errorMsg\":\"get url error\"}", msg->m_sp);//请求链接错误
		}
	}
}

// 链接者断开网络连接 [2017-7-17 willing]
void Work::HanderClientKick(LMsgKick* msg)
{
	if (msg->m_sp == m_logicManagerClient)
	{
		LLOG_ERROR("warning:lose connection with LM server!!!");//等待重连
	}
	else
	{
		std::map<Lint, SGateInfo>::iterator iter = m_gateInfo.begin();
		std::map<Lint, SGateInfo>::iterator iend = m_gateInfo.end();

		for ( ; iter != iend; iter++ )
		{
			SGateInfo& gateInfo = iter->second;
			if ( gateInfo.pSocketPtr == msg->m_sp )
			{
				m_gateInfo.erase(iter);
				break;
			}
		}

		std::map<Lint, SLogicInfo>::iterator iter_logic = m_logicInfo.begin();
		std::map<Lint, SLogicInfo>::iterator iend_logic = m_logicInfo.end();
		for ( ; iter_logic != iend_logic; iter_logic++ )
		{
			SLogicInfo& logicInfo = iter_logic->second;
			if ( logicInfo.pSocketPtr == msg->m_sp )
			{
				m_logicInfo.erase(iter_logic);
				break;
			}
		}
	}
}

// LogicManager 回应的心跳 [2017-7-10 willing]
void Work::HanderLogicManagerHeartBeat(LMsgLMG2TS_Heartbeat* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	LLOG_DEBUG("LogicManager HeartBeat");

	m_lastlogicMgrTimestamp = LTime().MSecs();
}

void Work::HanderLogicServerLogin(LMsgL2TS_Login* msg)
{
	if ( NULL == msg || NULL == msg->m_sp)
	{
		return;
	}

	LLOG_DEBUG("logic_server %d login",msg->nLogicServerID);
	
	SLogicInfo logicInfo;
	logicInfo.nHeartbeatTimestamp = LTime().MSecs();
	logicInfo.nLogicId = msg->nLogicServerID;
	logicInfo.pSocketPtr = msg->m_sp;

	m_logicInfo[msg->nLogicServerID] = logicInfo;

}

void Work::HanderLogicServerHeartBeat(LMsgL2TS_Heartbeat* msg)
{
	if ( NULL == msg || NULL == msg->m_sp )
	{
		return;
	}

	LLOG_DEBUG("LogicServer %d HeartBeat", msg->nLogicServerID);

	std::map<Lint, SLogicInfo>::iterator iter = m_logicInfo.find(msg->nLogicServerID);
	if ( iter != m_logicInfo.end() )
	{
		LMsgTS2L_Heartbeat heartbeat;
		if (iter->second.pSocketPtr)
		{
			iter->second.pSocketPtr->Send(heartbeat.GetSendBuff());
		}
	}
	else
	{
		LLOG_ERROR("Work::HanderLogicServerHeartBeat not fount logic server, id=%d", msg->nLogicServerID);
	}
}

// GateServer 发来的心跳 [2017-7-12 willing]
void Work::HanderGateServerHeartBeat(LMsgG2TS_Heartbeat* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	LLOG_DEBUG("GateServer %d HeartBeat", msg->nGateId);
	std::map<Lint, SGateInfo>::iterator iter = m_gateInfo.find(msg->nGateId);
	if ( iter != m_gateInfo.end() )
	{
		LMsgTS2G_Heartbeat heartbeat;
		if (iter->second.pSocketPtr )
		{
			iter->second.pSocketPtr->Send(heartbeat.GetSendBuff());
		}
	}
	else
	{
		LLOG_ERROR("Work::HanderGateServerHeartBeat not fount gate server, id=%d", msg->nGateId);
	}
}

// GateServer 登陆
void Work::HanderGateServerLogin(LMsgG2TS_Login* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	SGateInfo gateInfo;
	gateInfo.nGateId = msg->nGateId;
	gateInfo.nHeartbeatTimestamp = LTime().MSecs();
	gateInfo.pSocketPtr = msg->m_sp;
	LLOG_ERROR("GateID:%d Login succeed!!!",msg->nGateId);
	m_gateInfo[msg->nGateId] = gateInfo;
}

void Work::HanderLogicOrGateLogout(LMsgLMG2TS_LogicGateLogout* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	if ( 1 == msg->nIsLogic )
	{
		std::map<Lint, SLogicInfo>::iterator iter = m_logicInfo.find(msg->nServerID);
		if (iter != m_logicInfo.end())
		{
			LLOG_ERROR("LogicServer ID:%d Logout!!!",iter->second.nLogicId);
			m_logicInfo.erase(iter);
		}
	}
	else
	{
		std::map<Lint, SGateInfo>::iterator iter = m_gateInfo.find(msg->nServerID);
		if (iter != m_gateInfo.end())
		{
			LLOG_ERROR("GateServer ID:%d Logout!!!", iter->second.nGateId);
			m_gateInfo.erase(iter);
		}
	}
}

bool Work::HanderCheckMd5(std::map<Lstring, Lstring>& param)
{
	std::vector<std::string> keys;
	for (std::map<std::string, std::string>::const_iterator it = param.begin(); it != param.end(); ++it)
	{
		const std::string& strKey = it->first;
		if (strKey.compare("sign") != 0)
			keys.push_back(strKey);
	}
	std::sort(keys.begin(), keys.end());
	std::string reqsort;
	for (size_t i = 0; i < keys.size(); ++i)
	{
		reqsort += keys[i] + param[keys[i]];
	}
	reqsort += "2t66e0b8a9c152574a8228b12456aohb246l1";


	std::string sign = HttpRequest::Instance().Md5Hex(reqsort.c_str(), reqsort.length());
	LLOG_DEBUG("Work::HanderCheckMd5,source:%s,sign:%s", reqsort.c_str(), sign.c_str());

	//加密参数不对
	if (sign != param["sign"])
	{
		LLOG_ERROR("Work::HanderCheckMd5 sign error %s:%s", param["sign"].c_str(), sign.c_str());
		return true;
	}

	return true;
}

void Work::HanderGateMsg(LMsgG2TUserMsg* msg, const LSocketPtr& pGateSocketPtr)
{
	if (NULL == msg->m_userMsg)
	{
		return;
	}

	Llong nUserUnique = ((Llong)(msg->m_gateID) << 32 | (Llong)(msg->m_userGateId));

	switch (msg->m_userMsgId)
	{
	case MSG_C_2_S_QueryTeam:
		HanderQueryTeam((LMsgC2SQuerTeam*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_REQ_JOIN:
		HanderReqJoinTeam((LMsgC2SReqJoin*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_REQ_TEAM_LIST:
		HanderReqTeamList((LMsgC2SReqTeamList*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_TEAM_INFO:
		HanderGetTeamInfo((LMsgC2SGetTeamInfo*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_Member_List:
		handerMemberList((LMsgC2SGetTeamMemberList*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_Cancel_Join:
		HanderCancelJoin((LMsgC2SCancelJoin*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_Leave_Team:
		HanderLeaveTeam((LMsgC2SLeaveTeam*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_GET_VIDEO_LOG:
		HanderGetVideoLog((LMsgC2SGetVideoLog*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_GetAllTeamDesk:
		HanderGetAllTeamDesk((LMsgC2SGetAllTeamDesk*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_SubscribeTeamDesk:
		HanderSubscribeTeamDesk((LMsgC2SSubscribeTeamDesk*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_GetTeamOptLog:
		HanderGetTeamOptLog((LMsgC2SSubscribeTeamDesk*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_GET_RANGE_VIDEO_LOG:
		HanderGetRangeVideoLog((LMsgC2SGetRangeVideoLog*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_CREATE_KUAISU_JU:
		HanderUserCreatePlayConfig((LMsgC2SCreatePlayConfig*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_FAST_PLAY:
		HanderUserFastPlay((LMsgC2SFastPlay*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_OPT_MEM:
		HanderOptMemberReq((LMsgC2SOptMember*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_OPT_TEAMINFO:
		HanderOptTeamInfo((LMsgC2SOptTeamInfo*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_ADMIN_IVITE:
		HanderAdminInviteOther((LMsgC2SAdminInviteOther*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_REQ_EXCHAGEGOLD:
		HanderReqExchageGold((LMsgC2SReqExchageGold*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_REQ_EXCHAGEGOLD_LOG:
		HanderReqExchageGoldLog((LMsgC2SReqExchageGoldLog*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_CANNCEL_EXCHAGEGOLD:
		HanderCanncelExchageGold((LMsgC2SCanncelExchageGold*)msg->m_userMsg, nUserUnique);
		break;
	case MSG_C_2_S_INVITE_INROOM:
		HanderInviteInRoom((LMsgC2SInviteInRoom*)msg->m_userMsg, nUserUnique);
		break;
	default:
		break;
	}
}

void Work::HanderAdminInviteOther(LMsgC2SAdminInviteOther* msg, Llong nUserUnique)
{
	if (msg == NULL || nUserUnique == 0)
		return;
	Lint userId = gGateUserManager.GetUserID(nUserUnique);
	if (userId != msg->m_userid)
		return;
	gTeamManager.HanderTeamAdminInviteOther(msg, nUserUnique);
}

void Work::HanderReqExchageGold(LMsgC2SReqExchageGold* msg, Llong nUserUnique)
{
	if (msg == NULL || nUserUnique == 0)
		return;
	Lint userId = gGateUserManager.GetUserID(nUserUnique);
	if (userId <= 0) return;

	LLOG_DEBUG("HanderReqExchageGold userid %d touserid %d num %d", userId, msg->nToUserId, msg->nGoldNum);

	LMsgTS2LMG_ReqExchageGold sengMsg;
	sengMsg.m_userID = userId;
	sengMsg.m_toUserId = msg->nToUserId;
	sengMsg.m_goldNum = msg->nGoldNum;
	SendToLogicManager(sengMsg);
}

void  Work::HanderInviteInRoom(LMsgC2SInviteInRoom* msg, Llong nUserUnique)
{
	if (msg == NULL || nUserUnique == 0)
		return;
	Lint userId = gGateUserManager.GetUserID(nUserUnique);
	if (userId <= 0) return;

	gTeamManager.SendInviteInRoom(userId, msg->memberId);
}

void Work::HanderReqExchageGoldLog(LMsgC2SReqExchageGoldLog* msg, Llong nUserUnique)
{
	if (msg == NULL || nUserUnique == 0)
		return;
	Lint userId = gGateUserManager.GetUserID(nUserUnique);
	if (userId <= 0) return;

	gTeamManager.SendAllExchageGoldOptLog(userId);
}

void Work::HanderCanncelExchageGold(LMsgC2SCanncelExchageGold* msg, Llong nUserUnique)
{
	if (msg == NULL || nUserUnique == 0)
		return;

	Lint userId = gGateUserManager.GetUserID(nUserUnique);
	if (userId <= 0) return;

	LLOG_DEBUG("HanderCanncelExchageGold userid %d %ld", userId, msg->lLogId);
	SExchageGoldOptLog * log = gTeamManager.GetTeamExchageOpt(msg->lLogId);
	if (!log)
	{
		LMsgS2CCanncelExchageGold send;
		send.nResCode = 1;
		SendToGate(send, nUserUnique);
		LLOG_ERROR("HanderCanncelExchageGold error llogid %d %ld", userId, msg->lLogId);
		return;
	}

	if (log->nFromUserId != userId)
	{
		LMsgS2CCanncelExchageGold send;
		send.nResCode = 1;
		SendToGate(send, nUserUnique);
		LLOG_ERROR("HanderCanncelExchageGold nFromUserId llogid %d %ld", userId, log->nFromUserId);
		return;
	}

	if (log->nOpt != 0)
	{
		LMsgS2CCanncelExchageGold send;
		send.nResCode = 2;
		SendToGate(send, nUserUnique);
		LLOG_ERROR("HanderCanncelExchageGold nFromUserId llogid %d %ld", userId, log->nFromUserId);
		return;
	}

	log->nOpt = 2;
	gTeamManager.AddTeamExchageOpt(log, true);
	//通知改变数据
	ModifyRoomCardCount(userId, log->nGoldNum, CARDS_OPER_TYPE_CANNCEL_EXCHAGEGOLD);
}

bool Work::IsInMatchDesk(Lint userId)
{
	return m_MatchJoinSet.count(userId) ? true : false;
}

void Work::HanderOptMemberReq(LMsgC2SOptMember* msg, Llong nUserUnique)
{
	gTeamManager.TeamManagerOptMem(msg, nUserUnique);
}

void Work::GetTeamInfo_(LMsgS2CQueryTeamInfo &teamInfoMsg, Lint nUserID, Lint nTeamID)
{
	STeamInfo& teamInfo = (STeamInfo &)gTeamManager.FindTeam(nTeamID);
	teamInfoMsg.nTeamID = teamInfo.nTeamID;
	
	Lint tmp_teamid;
	teamInfoMsg.m_isInDesk = gDeskManager.IsInDesk(nUserID, tmp_teamid) ? 1 : 0;
	if (teamInfo.nTeamID > 0)
	{
		teamInfoMsg.nCreaterID = teamInfo.nAdminID;
		teamInfoMsg.strCreaterName = teamInfo.strCreaterName;
		teamInfoMsg.strTeamName = teamInfo.strTeamName;
		teamInfoMsg.nMemCount = teamInfo.nMemCount;
		teamInfoMsg.nOnLineCount = teamInfo.nOnLineCount;
		teamInfoMsg.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
		// 获取查询玩家在该工会中的状态 [2017-7-29 willing]
		teamInfoMsg.nState = gTeamManager.GetMemStateInTeam(teamInfo.nTeamID, nUserID);

		{
			STeamDescInfoMsg& teamDescInfo = teamInfoMsg.teamInfo;
			if (teamInfo.GetUserTeamState(nUserID) == ETMTS_NORMAL)
			{
				// 找到了条件的工会信息，放入到消息列表中 [2017-7-20 willing]
				teamDescInfo.nTeamID = teamInfo.nTeamID;
				teamDescInfo.strTeamName = teamInfo.strTeamName;
				teamDescInfo.nCreaterID = teamInfo.nAdminID;
				teamDescInfo.m_CreaterHeadImageURL = teamInfo.strCreaterImageUrl;
				teamDescInfo.strCreaterName = teamInfo.strCreaterName;
				teamDescInfo.nMemCount = teamInfo.nMemCount;
				teamDescInfo.nOnLienCount = teamInfo.nOnLineCount;
				teamDescInfo.m_isMaster = teamInfo.nAdminID == nUserID ? 1 : 0;
				teamDescInfo.m_boardInfo = teamInfo.m_boardInfo;
				teamDescInfo.m_nCards = teamInfo.nCards;
				teamDescInfo.nReqJoinTime = teamInfo.nCreateTime;

				teamDescInfo.nManageId = teamInfo.nManageId;
				teamDescInfo.nCountyId = teamInfo.nCountyId;
				teamDescInfo.nDeskCntLimit = teamInfo.nDeskCntLimit;
				teamDescInfo.nDeskProportionate = teamInfo.nDeskProportionate;
				teamDescInfo.vPlayTypes.assign(teamInfo.vPlayTypes.begin(), teamInfo.vPlayTypes.end());

				/*auto it = gTeamManager.m_tTeamPlayConfigMap.find(teamInfo.nTeamID);
				if (it != gTeamManager.m_tTeamPlayConfigMap.end())
				{
					teamDescInfo.m_playConfig.m_roomType = it->second.m_roomType;
					teamDescInfo.m_playConfig.m_playEnum = it->second.m_playEnum;
					teamDescInfo.m_playConfig.m_teamID = it->second.m_teamID;
					teamDescInfo.m_playConfig.m_playtype = it->second.m_playtype;
					teamDescInfo.m_playConfig.m_nCreateTime = it->second.m_time;
				}*/
			}
		}
	}
}

void Work::HanderQueryTeam(LMsgC2SQuerTeam* msg, Llong nUserUnique)
{
	
	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);
	LMsgS2CQueryTeamInfo teamInfoMsg;
	GetTeamInfo_(teamInfoMsg, nUserID, msg->nTeamID);
	SendToGate(teamInfoMsg, nUserUnique);
}

void Work::HanderUserLogin(LMsgLMG2TS_UserLogin* msg)
{
	LMsgTS2LMG_UserLogin sendmsg;
	LMsgS2CQueryTeamInfo teamInfoMsg;
	sendmsg.nUserID = msg->nUserID;
	sendmsg.nResCode = gTeamManager.CheckCanLogin(msg->nUserID, msg->nOnUseTeamId, sendmsg.nOnUseTeamId);
	if (sendmsg.nResCode == ETEC_SUCCESS)
	{
		gGateUserManager.AddUser(msg->nUserUnique, msg->nGateID, msg->nUserID, msg->nGateUserID, msg->nOnUseTeamId);
		gTeamManager.ModifyMemGameState(msg->nUserID, ETMGS_ONLINE);
		GetTeamInfo_(teamInfoMsg, msg->nUserID, msg->nOnUseTeamId);
	}
	else
	{
		const STeamInfo& teamInfo = gTeamManager.FindTeam(msg->nOnUseTeamId);
		sendmsg.sOnUseTeamName = teamInfo.strTeamName;
	}

	sendmsg.teaminfo.nTeamID = teamInfoMsg.nTeamID;				// 要查询的工会ID,如果查询不到 该值为0 [2017-7-17 willing]
	sendmsg.teaminfo.nCreaterID = teamInfoMsg.nCreaterID;				// 创建者ID [2017-7-14 willing]
	sendmsg.teaminfo.strCreaterName = teamInfoMsg.strCreaterName;		// 工会名称 [2017-7-14 willing]
	sendmsg.teaminfo.strTeamName = teamInfoMsg.strTeamName;			// 工会名称 [2017-7-14 willing]
	sendmsg.teaminfo.nMemCount = teamInfoMsg.nMemCount;
	sendmsg.teaminfo.nOnLineCount = teamInfoMsg.nOnLineCount;
	sendmsg.teaminfo.m_CreaterHeadImageURL = teamInfoMsg.m_CreaterHeadImageURL;
	sendmsg.teaminfo.nState = teamInfoMsg.nState;					// 查询玩家在该工会中的状态 [2017-7-29 willing]
	sendmsg.teaminfo.m_isInDesk = teamInfoMsg.m_isInDesk;		//是否加入了房间
	sendmsg.teaminfo.teamInfo = teamInfoMsg.teamInfo;//公会本身的信息

	SendToLogicManager(sendmsg);
}

void Work::HanderUserLogout(LMsgLMG2TS_UserLogout* msg)
{
	gGateUserManager.RmvUser(msg->nUserUnique);
	gTeamManager.ModifyMemGameState(msg->nUserID, ETMGS_OFFLINE);
}
void Work::HanderLM2TSAddDesk(LMsgLMG2TS_AddDesk* msg)
{
	Lint result = gTeamManager.CheckCanAddRoom(msg->m_userID, msg->m_deskId);
	if (result == 0)
	{
		Lint teamId = gDeskManager.GetTeamIDByDeskID(msg->m_deskId);
		Lint limit_num = 0;
		if (!gDeskManager.CheckDeskInfoLimit(teamId, msg->m_deskId, msg->m_card_num, limit_num))
		{
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskId;
			ret.m_errorCode = ETEC_GOLD_NOT_LIMIT;
			gWork.SendToGate(ret, gGateUserManager.GetUnique(msg->m_userID));
			return;
		}

		LMsgTS2LMG_AddDesk send;
		send.m_location = msg->m_location;
		send.m_userID = msg->m_userID;
		send.m_deskIds.push_back(msg->m_deskId);
		send.m_limitGolds.push_back(limit_num);
		SendToLogicManager(send);
	}
	else
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = result;
		//21:会员状态不正常   17：已加入房间，不能再加入其它房间  200：不是俱乐部成员，不能加入此俱乐部
		gWork.SendToGate(ret, gGateUserManager.GetUnique(msg->m_userID));
		LLOG_ERROR("Work::HanderLM2TSAddDesk user CheckCanAddRoom false");
	}
	
}

void Work::HanderLM2TSReqExchageGold(LMsgLMG2TS_ReqExchageGold* msg)
{
	if (!msg) return;
	LLOG_DEBUG("HanderLM2TSReqExchageGold %d touserid %d num %d", msg->m_logMsg.nFromUserId, msg->m_logMsg.nToUserId, msg->m_logMsg.nGoldNum);
	
	SExchageGoldOptLog * newLog = new SExchageGoldOptLog;
	*newLog = msg->m_logMsg;
	gTeamManager.AddTeamExchageOpt(newLog, true);

	LMsgS2CReqExchageGold ret;
	ret.nResCode = 0;
	gWork.SendToGate(ret, gGateUserManager.GetUnique(msg->m_logMsg.nFromUserId));
}

void Work::HanderCreateRoom(LMsgLMG2TS_CreateDesk* msg)
{
	Lint nRet = 0;
	if ( msg->m_free == 0 )
	{
		nRet = gTeamManager.CheckCanCreateRoom(msg->m_teamID, msg->m_userID, msg->m_flag);
	}

	if ( nRet == 0 )
	{
		// 可以创建房间 [2017-7-23 willing]
		LLOG_ERROR("Work::HanderCreateRoom userID=%d teamID=%d", msg->m_userID, msg->m_teamID);

		if ( msg->m_free == 0 )
		{
			// 预扣房卡 [2017-7-23 willing]
			gTeamManager.YuKouCard(msg->m_teamID, msg->m_userID, msg->m_flag);
		}

		// 广播房卡改变 [2017-7-23 willing]
		gTeamManager.BroadcastTeamInfoChange(msg->m_teamID);

		// 把创建的信息再发回LogicManager [2017-7-23 willing]
		LMsgTS2LMG_CreateDesk createDesk;
		createDesk.m_circleGoldLimit = msg->m_circleGoldLimit;
		createDesk.m_stake = msg->m_stake;
		createDesk.m_userID = msg->m_userID;
		createDesk.m_playtypeCount = msg->m_playtype.size();
		createDesk.m_free = msg->m_free;

		createDesk.m_flag = msg->m_flag;						//房间类型，4圈，8圈，12圈
		createDesk.m_secret = msg->m_secret;					//房间密码，如果为空，服务器会随机一个密码
		createDesk.m_gold = msg->m_gold;						//房间抵住，单位元，最低为1元，最多不能超过100
		createDesk.m_state = msg->m_state;					//玩法规则 0 转转   1 长沙	//贵州废弃
		createDesk.m_robotNum = msg->m_robotNum;					// 0,不加机器人，1，2，3加机器人数量
		createDesk.m_maYouDesk = 2;
		for (int i = 0; i < CARD_COUNT; i++)
		{
			createDesk.m_cardValue[i].m_color = msg->m_cardValue[i].m_color;
			createDesk.m_cardValue[i].m_number = msg->m_cardValue[i].m_number;
		}
		createDesk.m_lenth = msg->m_lenth;
		for (int i = 0; i < createDesk.m_playtypeCount; i++)
		{
			createDesk.m_playtype.push_back(msg->m_playtype[i]);
		}

		createDesk.m_extValue.assign(createDesk.m_playtype.begin(), createDesk.m_playtype.end());
		//createDesk.m_up_down_ji = msg->m_up_down_ji;				//上下鸡
		//createDesk.m_all_ji = msg->m_all_ji;					//满堂鸡
		//for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		//{
		//	createDesk.m_queNum[i] = msg->m_queNum[i];
		//}
		//createDesk.m_lian_zhuang = msg->m_lian_zhuang;				// 连庄是否算分 [2016-9-12 willing]
		createDesk.m_zhuangPos = msg->m_zhuangPos;                // 调试模式设置庄的位置 [2016-10-27 willing]
		createDesk.m_useTeamCard = msg->m_useTeamCard;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
		createDesk.m_teamID = msg->m_teamID;					// 工会ID [2017-7-23 willing]
		createDesk.m_playerNum = msg->m_playerNum;					// 工会ID [2017-7-23 willing]
		createDesk.m_nMaxFanshu = msg->m_nMaxFanshu;

		const STeamInfo& teamInfo = gTeamManager.FindTeam(msg->m_teamID);

		createDesk.m_teamName = teamInfo.strTeamName;
		createDesk.m_proportionate = teamInfo.nDeskProportionate;
		LLOG_DEBUG("Work::HanderCreateRoom m_userID:%d", msg->m_userID);
		SendToLogicManager(createDesk);
	}
	else
	{
		LLOG_ERROR("Work::HanderCreateRoom cannot create desk, error=%d",nRet);

		LMsgC2SCreateTeamDeskError ret;
		ret.nErrorCode = nRet;
		gWork.SendToGate(ret,gGateUserManager.GetUnique(msg->m_userID));
	}
}

void Work::HanderGiveBackCard(LMsgLMG2TS_GiveBackCard* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	gTeamManager.GivebackCard(msg->nTeamID, msg->nUserID, msg->nFlag,msg->nFastPlay);
}

void Work::HanderDelCard(LMsgL2TS_DelCard* msg)
{
	if (NULL == msg)
	{
		return;
	}

	gTeamManager.RealDelCard(msg->nTeamID, msg->nUserID, msg->nFlag);
}

void Work::HanderReqJoinTeam(LMsgC2SReqJoin* msg, Llong nUserUnique)
{
	if ( NULL == msg )
	{
		return;
	}

	LMsgS2CJoinRst joinRstMsg;
	joinRstMsg.nTeamID = msg->nTeamID;
	joinRstMsg.nJoinRst = ETEC_SUCCESS;		// 默认申请成功 [2017-7-19 willing]

	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);

	do 
	{
		// 判断加入的工会是否超过了上限 [2017-7-21 willing]
		if ( gTeamManager.GetJoinCount(nUserID) >= gConfig.GetMemJoinTeamMaxCount() )
		{
			joinRstMsg.nJoinRst = ETEC_JOIN_MAX_TEAM;
			break;
		}

		// 判断指定的工会是否存在 [2017-7-19 willing]
		const STeamInfo& teamInfo = gTeamManager.FindTeam(msg->nTeamID);
		if (teamInfo.nTeamID == 0)
		{
			joinRstMsg.nJoinRst = ETEC_TEAM_NOT_EXIST;
			break;
		}

		// 判断工会人数是否达到了上限 [2017-7-22 willing]
		if ( teamInfo.nMemCount >= gConfig.GetMaxTeamMemCount() )
		{
			joinRstMsg.nJoinRst = ETEC_TEAM_MAX_COUNT;
			break;
		}

		// 判断是否已经加入了该工会 [2017-7-19 willing]
		Lint nState = gTeamManager.GetMemStateInTeam(msg->nTeamID, nUserID);
		if (nState == ETMTS_JOIN)
		{
			joinRstMsg.nJoinRst = ETEC_ALREADY_JOIN;
		}
		else if (nState == ETMTS_NORMAL)
		{
			joinRstMsg.nJoinRst = ETEC_TEAM_NORMAL;
		}
		else if (nState == ETMTS_LEAVE)
		{
			joinRstMsg.nJoinRst = ETEC_TEAM_LEAVE;
		}

	} while (false);

	if (joinRstMsg.nJoinRst == ETEC_SUCCESS)
	{
		STeamMember member;
		member.nTeamID = msg->nTeamID;
		member.nUserID = nUserID;
		member.strHeadImageUrl = "";		// 需要向LogicManager请求 [2017-7-19 willing]		
		member.nTodayUserCardNum = 0;	
		member.nGameState = ETMGS_ONLINE;
		member.nTeamState = ETMTS_JOIN;
		member.nLastUserCardTime = 0;	
		member.nJoinTime = LTime().Secs();
		member.m_reqTime = 0;
		gTeamManager.AddMemToTeam(member,true);

		LLOG_ERROR("Work::HanderReqJoinTeam TeamID=%d UserID=%d", msg->nTeamID, nUserID);

		// 向LogicManager请求玩家的头像和昵称 [2017-7-26 willing]
		LMsgTS2LMG_GetUserInfo getUserInfo;
		getUserInfo.nUserID = nUserID;
		SendToLogicManager(getUserInfo);

		// 记录操作日志 [2017-8-9 willing]
		gTeamManager.SaveTameMemOptLog(msg->nTeamID, nUserID, ETMOT_REQ_JOIN_TEAM);
	}

	SendToGate(joinRstMsg, nUserUnique);
}

void Work::HanderReqTeamList(LMsgC2SReqTeamList* msg, Llong nUserUnique)
{
	if ( NULL == msg )
	{
		return;
	}

	gTeamManager.SendTeamList(nUserUnique, msg->nTeamState);
}

void Work::HanderGetTeamInfo(LMsgC2SGetTeamInfo* msg, Llong nUserUnique)
{
	if ( NULL == msg )
	{
		return;
	}

	const STeamInfo& teamInfo = gTeamManager.FindTeam(msg->nTeamID);

	LMsgS2CTeamInfo teamInfoMsg;
	teamInfoMsg.nTeamID					= teamInfo.nTeamID;
	teamInfoMsg.strCreaterName			= teamInfo.strCreaterName;
	teamInfoMsg.strTeamName				= teamInfo.strTeamName;
	teamInfoMsg.nCards					= teamInfo.nCards - teamInfo.nYuKouCard;
	teamInfoMsg.nCreateTime				= teamInfo.nCreateTime;
	teamInfoMsg.nMemCardLimitPerDay		= teamInfo.nMemCardLimitPerDay;
	teamInfoMsg.nState					= teamInfo.nState;
	teamInfoMsg.nOnLineCount			= teamInfo.nOnLineCount;
	teamInfoMsg.nMemCount				= teamInfo.nMemCount;
	teamInfoMsg.m_boardInfo				= teamInfo.m_boardInfo;
	teamInfoMsg.nTodayUserCard = gTeamManager.GetMemTodayUserCard(msg->nTeamID, gGateUserManager.GetUserID(nUserUnique));


	gWork.SendToGate(teamInfoMsg, nUserUnique);
}

void Work::handerMemberList(LMsgC2SGetTeamMemberList* msg, Llong nUserUnique)
{
	if ( NULL == msg )
	{
		return;
	}

	gTeamManager.SendMemberList(msg->nTeamID, nUserUnique);
}

void Work::HanderCancelJoin(LMsgC2SCancelJoin* msg, Llong nUserUnique)
{
	if (NULL == msg)
	{
		return;
	}

	gTeamManager.CancelJoin(msg->nTeamID, nUserUnique);
}

void Work::HanderLeaveTeam(LMsgC2SLeaveTeam* msg, Llong nUserUnique)
{
	if (NULL == msg)
	{
		return;
	}

	gTeamManager.RequestLeaveTeam(msg->nTeamID, nUserUnique);
}

void Work::HanderVideoLog(LMsgL2TS_VideoLog* msg)
{
	if (NULL == msg)
	{
		return;
	}

	std::stringstream strStream;

	strStream << "insert into team_log(DeskID,TeamID,UserId1,UserId2,UserId3,UserId4,Score1,Score2,Score3,Score4,";
	strStream << "Nike1,HeadImageUrl1,Nike2,HeadImageUrl2,Nike3,HeadImageUrl3,Nike4,HeadImageUrl4,PlayFlag,Time) value(";
	strStream << msg->nDeskID << ",";
	strStream << msg->nTeamID << ",";
	strStream << msg->nUserID[0] << ",";
	strStream << msg->nUserID[1] << ",";
	strStream << msg->nUserID[2] << ",";
	strStream << msg->nUserID[3] << ",";
	strStream << msg->nScore[0] << ",";
	strStream << msg->nScore[1] << ",";
	strStream << msg->nScore[2] << ",";
	strStream << msg->nScore[3] << ",";

	for (int i = 0; i < 4; i++)
	{
		const STeamMember& member = gTeamManager.FindMember(msg->nTeamID, msg->nUserID[i]);
		if (member.nTeamID == msg->nTeamID)
		{
			strStream << "'" << member.strUserName << "',";
			strStream << "'" << member.strHeadImageUrl << "',";
		}
		else
		{
			strStream << "'',";
			strStream << "'',";
		}
	}
	strStream << msg->nFlag << ",";
	strStream << LTime().Secs() << ");";

	gDbServer.Push(move(strStream.str()), msg->nUserID[0]);
}

void Work::HanderAddDesk(LMsgL2TS_AddDesk* msg)
{
	if ( NULL == msg )
	{
		return;
	}
	Lint nRet = 0;
	Lint nState = gTeamManager.GetMemStateInTeam(msg->nTeamID, msg->m_userid);
	if ( nState == ETMTS_NORMAL )
	{
		nRet = 1;
	}

	LMsgTS2L_AddDeskRet addDeskRet;
	addDeskRet.nTeamID = msg->nTeamID;
	addDeskRet.nRet = nRet;
	addDeskRet.m_userid = msg->m_userid;
	addDeskRet.m_gateId = msg->m_gateId;
	addDeskRet.m_geteUserId = msg->m_geteUserId;
	addDeskRet.m_deskID = msg->m_deskID;
	addDeskRet.m_ip = msg->m_ip;
	addDeskRet.m_usert = msg->m_usert;
	addDeskRet.m_location = msg->m_location;

	msg->m_sp->Send(addDeskRet.GetSendBuff());

	LLOG_INFO("Work::HanderAddDesk ret=%d", nRet);
}

void Work::HanderGameStateChange(LMsgLS2TS_GameState* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	//LLOG_DEBUG("Work::HanderGameStateChange UserID=%d, IsInGame=%d", msg->nUserID, msg->nIsInGame);

	if ( msg->nIsInGame )
	{
		gTeamManager.ModifyMemGameState(msg->nUserID, ETMGS_GAME);
	}
	else
	{
		gTeamManager.ModifyMemGameState(msg->nUserID, ETMGS_ONLINE);
	}
	
}

void Work::HanderUserInfo(LMsgLMG2TS_UserInfo* msg)
{
	if (NULL == msg)
	{
		return;
	}

	gTeamManager.SetUserInfo(msg->nUserID, msg->strUserNick, msg->strHeadImageUrl);
}

void Work::HanderGetVideoLog(LMsgC2SGetVideoLog* msg, Llong nUserUnique)
{
	if ( NULL == msg )
	{
		return;
	}
	LTime curTime;
	//struct tm tmBegin;
	//tmBegin.tm_year = msg->nYear - 1900;
	//tmBegin.tm_mon = msg->nMonth - 1;
	//tmBegin.tm_mday = msg->nDay;
	/*tmBegin.tm_year = curTime.GetYear() - 1900;
	tmBegin.tm_mon = curTime.GetMonth();
	tmBegin.tm_mday = curTime.GetMday();
	tmBegin.tm_hour = 0;
	tmBegin.tm_min = 0;
	tmBegin.tm_sec = 0;*/
	bool is_admin = gTeamManager.GetAdminIdbyTeamID(msg->nTeamID) == msg->nUserID;
	//time_t beginTimestamp = mktime(&tmBegin);
	time_t beginTimestamp = 0;
	time_t endTimestamp = curTime.Secs();
	time_t yushu = endTimestamp % (24 * 60 * 60);
	time_t cur_date_s = endTimestamp - yushu;
	if(is_admin)
		beginTimestamp = cur_date_s - 6 * 24 * 3600;
	else 
		beginTimestamp = cur_date_s - 24 * 3600;

	gTeamManager.SendVideoLog(is_admin,msg->nTeamID, msg->nUserID, beginTimestamp, endTimestamp, nUserUnique);
}

void Work::ClearOldTeamVideoLog()
{
	std::stringstream strStream;

	strStream << "delete from team_log ";
	strStream << "where Time < ";
	strStream << gWork.GetCurTime().Secs() - 15 * 24 * 60 * 60 << ";";

	gDbServer.Push(move(strStream.str()), 1);
}

void Work::HanderNewTeamDesk(LMsgL2TS_NewTeamDesk* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	const STeamMember& teamMember = gTeamManager.FindMember(msg->nTeamID,msg->nCreaterUserID);
	if (teamMember.nTeamID == 0)
	{
		LLOG_ERROR("Work::HanderNewTeamDesk not found team,id=%d", msg->nTeamID);
		return;
	}

	STeamDeskInfo deskInfo;

	deskInfo.nTeamID = msg->nTeamID;
	deskInfo.nDeskID = msg->nDeskID;
	deskInfo.nUserID[0] = msg->nCreaterUserID;
	deskInfo.strHeadImageUrl[0] = teamMember.strHeadImageUrl;
	deskInfo.nMajongType = msg->nMajongType;
	deskInfo.nMaxCircle = msg->nMaxCircle;
	deskInfo.nMaxUserCount = msg->nMaxUserCount;
	deskInfo.nCurUserCount++;
	deskInfo.m_nike[0] = teamMember.strUserName;
	/*deskInfo.nUpDownJi = msg->nUpDownJi;
	deskInfo.nAllJi = msg->nAllJi;
	deskInfo.nLianZhuang = msg->nLianZhuang;*/
	deskInfo.nPlayTypeCount = msg->nPlayTypeCount;
	for ( int i=0; i<deskInfo.nPlayTypeCount; i++ )
	{
		deskInfo.tPlayType.push_back(msg->tPlayType[i]);
	}
	deskInfo.nCreateTime = LTime().Secs();
	deskInfo.nCircleGoldLimit = msg->nCircleGoldLimit;
	deskInfo.nStake = msg->nStake;
	deskInfo.nMaxFanshu = msg->nMaxFanshu;
	
	gDeskManager.AddDeskInfo(deskInfo,msg->m_fastDesk == 1);
}
 
void Work::HanderTeamDeskUserChg(LMsgL2TS_TeamDeskUserChg* msg)
{
	if (NULL == msg)
	{
		return;
	}

	gDeskManager.DeskUserChg(msg->nTeamID, msg->nDeskID, msg->nUserID, msg->nJoinOrLeave);
}

void Work::HanderDelTeamDesk(LMsgL2TS_DelTeamDesk* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	gDeskManager.DelDesk(msg->nTeamID, msg->nDeskID);
}

void Work::HanderGetAllTeamDesk(LMsgC2SGetAllTeamDesk* msg, Llong nUserUnique)
{
	if ( NULL == msg )
	{
		return;
	}

	gDeskManager.SendAllDesk(msg->nTeamID,nUserUnique);
}

void Work::HanderSubscribeTeamDesk(LMsgC2SSubscribeTeamDesk* msg, Llong nUserUnique)
{
	if ( NULL == msg )
	{
		return;
	}

	gTeamManager.SetCareForDesk(msg->nTeamID,gGateUserManager.GetUserID(nUserUnique), msg->nCareFor);
}

void Work::HanderGetTeamOptLog(LMsgC2SSubscribeTeamDesk* msg, Llong nUserUnique)
{
	if (NULL == msg)
	{
		return;
	}

	gTeamManager.SendTeamOptLog(nUserUnique);
}
void Work::HanderUserCreatePlayConfig(LMsgC2SCreatePlayConfig* msg, Llong nUserUnique)
{
	if (NULL == msg)
	{
		return;
	}

	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);
	if (nUserID > 0)
	{
		STeamPlayConfig config;
		config.m_playEnum = msg->m_playEnum;
		config.m_playtype = msg->m_playtype;
		config.m_roomType = msg->m_roomType;
		config.m_teamID = msg->m_teamID;
		config.m_userID = nUserID;
		config.m_time = LTime().Secs();
		config.m_circleGoldLimit = msg->m_circleGoldLimit;
		gTeamManager.AddTeamPlayConfig(config, true);
		
		/*auto it = gTeamManager.m_tTeamPlayConfigMap.find(config.m_teamID);
		if (it != gTeamManager.m_tTeamPlayConfigMap.end())
		{
			LMsgS2CTeamPlayConfig send;
			send.m_playConfig.m_roomType = it->second.m_roomType;
			send.m_playConfig.m_playEnum = it->second.m_playEnum;
			send.m_playConfig.m_playtype = it->second.m_playtype;
			send.m_playConfig.m_nCreateTime = it->second.m_time;
			send.m_playConfig.m_teamID = it->second.m_teamID;
			gWork.SendToGate(send, nUserUnique);
		}*/
	}
	
}

void Work::HanderUserFastPlay(LMsgC2SFastPlay* msg,Llong nUserUnique)
{
	if (NULL == msg)
	{
		return;
	}

	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);
	if (nUserID > 0)
	{
		gTeamManager.HanderUserFastPlay(msg->m_teamID, nUserID,nUserUnique);
	}
}
void Work::HanderGetRangeVideoLog(LMsgC2SGetRangeVideoLog* msg, Llong nUserUnique)
{
	if (NULL == msg)
	{
		return;
	}

	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);

	struct tm tmBegin;
	tmBegin.tm_year = msg->nYear - 1900;
	tmBegin.tm_mon = msg->nMonth - 1;
	tmBegin.tm_mday = msg->nDay;
	tmBegin.tm_hour = 0;
	tmBegin.tm_min = 0;
	tmBegin.tm_sec = 0;

	struct tm tmEnd;
	tmEnd.tm_year = msg->nEndYear - 1900;
	tmEnd.tm_mon = msg->nEndMonth - 1;
	tmEnd.tm_mday = msg->nEndDay;
	tmEnd.tm_hour = 23;
	tmEnd.tm_min = 59;
	tmEnd.tm_sec = 59;

	time_t beginTimestamp = mktime(&tmBegin);
	time_t endTimestamp = mktime(&tmEnd);
	bool admin = true;
	gTeamManager.SendVideoLog(admin,msg->nTeamID, nUserID, beginTimestamp, endTimestamp, nUserUnique);
}

void Work::HanderUserOnlineRst(LMsgLMG2TS_UserOnLineRet* msg)
{
	if (NULL == msg)
	{
		return;
	}

	Lint nOnLine = ETMGS_OFFLINE;

	switch (msg->nOnLine)
	{
	case 0:
		nOnLine = ETMGS_OFFLINE;
		break;
	case 1:
		nOnLine = ETMGS_ONLINE;
		break;
	case 2:
		nOnLine = ETMGS_GAME;
		break;
	default:
		LLOG_ERROR("Work::HanderUserOnlineRst unknow online flag=%d", msg->nOnLine);
		nOnLine = ETMGS_OFFLINE;
		break;
	}

	gTeamManager.ModifyMemGameState(msg->nUserID, nOnLine);
}

void Work::HanderLmgRecycleDeskId(LMsgLMG2TS_RecycleDeskId* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	Lint nTeamID = gDeskManager.GetTeamIDByDeskID(msg->nDeskId);

	if ( nTeamID == 0 )
	{
		LLOG_DEBUG("Work::HanderLmgRecycleDeskId null teamid :%d,nteamID:%d", msg->nDeskId, nTeamID);
		return;
	}
	LLOG_DEBUG("Work::HanderLmgRecycleDeskId:%d,nteamID:%d", msg->nDeskId, nTeamID);
	gDeskManager.DelDesk(nTeamID, msg->nDeskId);
}

void Work::HanderLMGSendDeskID(LMsgLMG2TS_ReqDeskRet* msg)
{
	if (NULL == msg)
	{
		return;
	}
	gDeskManager.PushFastDesk(msg->m_deskVec);
}

void Work::HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg)
{
	if (msg == NULL)
		return;

	if (m_logicManagerClient == msg->m_sp)
		m_logicManagerClient->OnKeepAliveAck();
}

void Work::HanderKeepAlive(LMsgS2SKeepAlive* msg)
{
	if (msg == NULL || !msg->m_sp)
		return;
	LMsgS2SKeepAliveAck ack;
	msg->m_sp->Send(ack.GetSendBuff());
}

void Work::HanderOptTeamInfo(LMsgC2SOptTeamInfo* msg,Llong nUserUnique)
{
	if (NULL == msg)
	{
		return;
	}

	Lint nUserID = gGateUserManager.GetUserID(nUserUnique);
	if (nUserID > 0)
	{
		gTeamManager.HanderOptTeamInfo(msg, nUserID, nUserUnique);
	}
}

void Work::ModifyRoomCardCount(Lint userId, Lint nShift, Lint nReason)
{
	LMsgL2LMGModifyCard msg;
	msg.admin = "admin";
	msg.cardType = CARD_TYPE_8;
	msg.cardNum = nShift;
	if (nShift)
	{
		msg.isAddCard = 1;
	}
	else
	{
		msg.isAddCard = 0;
	}
	
	msg.operType = nReason;
	msg.m_userid = userId;

	SendToLogicManager(msg);
	LLOG_ERROR("User::ModifyRoomCardCount userid %d Banlance:%d nReason=%d", userId, nShift, nReason);
}