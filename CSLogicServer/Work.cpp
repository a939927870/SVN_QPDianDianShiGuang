#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "UserManager.h"
#include "User.h"
#include "Desk.h"
#include "RoomVip.h"
#include "RobotManager.h"
#include "BaseCommonFunc.h"
#include <openssl/md5.h>
#include "RemoteLogThread.h"

using namespace boost::asio;  
using boost::asio::ip::tcp; 

//初始化
bool Work::Init()
{
	//设置log文件
	LLog::Instance().SetFileName("LogicServer");

	//初始化配置文件
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}
	//remote log
	if (gRLT.init(gConfig.GetRemoteLogThreadStart(),
		(const char*)gConfig.GetRemoteLogUrl().c_str(),
		(const char*)gConfig.GetRemoteLogCacheFileName().c_str(),
		(size_t)gConfig.GetMaxCachedLogSize(),
		(size_t)gConfig.GetMaxCachedLogNum(),
		(size_t)gConfig.GetMaxFileLogSize()) < 0)
	{
		LLOG_ERROR("gRLT.init error");
		return false;
	}

	if (!CardManager::Instance().Init())
	{
		LLOG_ERROR("CardManager::Instance().Init() error");
		return false;
	}

	if(!gVipLogMgr.Init())
	{
		LLOG_ERROR("gVipLogMgr.Init() error");
		return false;
	}

	if(!gRoomVip.Init())
	{
		LLOG_ERROR("gRoomVip.Init() error");
		return false;
	}

	//初始化机器人
	if(!gRobotManager.Init("settings/RobotConfig.txt") && gConfig.GetIfAddRobot())
	{
		LLOG_ERROR("gRobotManager.Init() error");
		return false;
	}
	
	LTime cur;
	m_30MSceTick = cur.MSecs();
	m_200MSceTick = cur.MSecs();
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();

	m_loginLMFirst = true;

	return true;
}

bool Work::Final()
{
	return true;
}

//启动
void Work::Start()
{
	m_gcService.Start();

	gInsideNet.Start();

	LRunnable::Start();

	gRLT.start();  //remote log

	ConnectToLogicManager();

	ConnectToDb();
}

//等待
void Work::Join()
{
	gInsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();

}

//停止
void Work::Stop()
{
	m_logicManager.reset();
	m_teamServer.reset();

	m_gateInfo.clear();

	gInsideNet.Stop();

	LRunnable::Stop();

	gRLT.stop();  //remote log

	m_gcService.Stop();
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
			//delete msg;
			m_gcService.Collect(msg);
		}
	}
}

void Work::Tick(LTime& cur)
{
	//30毫秒循环一次
	if (cur.MSecs() - m_30MSceTick > 30)
	{
		m_30MSceTick = cur.MSecs();
	}

	//200毫秒循环一次
	if (cur.MSecs() - m_200MSceTick > 200)
	{
		m_200MSceTick = cur.MSecs();
	}

	//1000毫秒循环一次
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		gUserManager.Tick();
		gRoomVip.Tick(cur);
		m_1SceTick = cur.MSecs();
		CheckSportResult(cur);
	}

	//5秒循环一次
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
	}

	//30秒循环一次
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		gVipLogMgr.Tick();
		CheckConfigUpdates();
	}

	//5分钟循环一次
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
	}

	if(m_logicManager)
		m_logicManager->OnTick(cur);
	if(m_dbClient)
		m_dbClient->OnTick(cur);
	if (m_matchInfo.m_client)
		m_matchInfo.m_client->OnTick(cur);
	if (m_teamServer)
		m_teamServer->OnTick(cur);
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//玩家请求登录
	switch(msg->m_msgId)
	{
	case MSG_CLIENT_KICK:
		HanderUserKick((LMsgKick*)msg);
		break;
	case MSG_CLIENT_IN:
		HanderClientIn((LMsgIn*)msg);
		break;
	case MSG_HTTP:
		HanderHttp((LMsgHttp*)msg);
		break;
	case MSG_G_2_L_LOGIN:
		HanderGateLogin((LMsgG2LLogin*)msg);
		break;
		//////////////////////////////////////////////////////////////////////////
		//logic 和 db 之间的消息交互
		//////////////////////////////////////////////////////////////////////////
		//gate 跟 logic之间的交互
	case MSG_LMG_2_L_USER_LOGIN:		
		HanderUserLogin((LMsgLMG2LLogin*)msg);
		break;
	case MSG_LMG_2_L_CREATE_DESK:
		handerLMGCreateDesk((LMsgLMG2LCreateDesk*)msg);
		break;
	case MSG_LMG_2_L_ADDTO_DESK:
		handerLMGAddToDesk((LMsgLMG2LAddToDesk*)msg);
		break;
	case MSG_LMG_2_CE_SPORT_RESET_DESK:
		handerLMGSportResetDesk((LMsgLMG2CESportResetDesk*)msg);
		break;
	case MSG_LMG_2_L_MATCH_ADD_DESK:
		handerLMGMatchAddDesk((LMsgLMG2LMatchAddDesk*)msg);
		break;
	case MSG_LMG_2_L_SYNC_MATCH:
		HanderMatchInfo((LMsgLMG2LogicMatchInfo*)msg);
		break;
	case MSG_LMG_2_L_MODIFY_ROOM_CARD:
		HanderUserModifyRoomCard((LMsgLMG2LModifyRoomCard*)msg);
		break;
	case MSG_G_2_L_USER_MSG:
		{
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				HanderGateUserMsg((LMsgG2LUserMsg*)msg,gate);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate does not exist");
			}
		}
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		{
			LMsgG2LUserOutMsg* gmsg = (LMsgG2LUserOutMsg*)msg;
			GateInfo* gate = GetGateInfoBySp(msg->m_sp);
			if (gate)
			{
				HanderUserLogout(gate->m_id,gmsg->m_userGateId);
			}
			else
			{
				LLOG_ERROR("Work::HanderMsg gate does not exist");
			}
		}
		break;
	case MSG_LMG_2_L_TEAMSERVER:
		HanderTeamServerInfo((LMsgLMG2L_TeamServer*)msg);
		break;
	case MSG_TS_2_L_ADD_DESK_RET:
		HanderTsAddDeskRet((LMsgTS2L_AddDeskRet*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE_ACK:
		HanderKeepAliveAck((LMsgS2SKeepAliveAck*)msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserLogin(LMsgLMG2LLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser)
	{
		//同步卡数
		pUser->m_userData.m_numOfCard2s = msg->m_card_num;

		if (pUser->GetDesk())
		{
			LLOG_INFO("Work::HanderUserLogin user is already on desk %d:%d:%d:%d", pUser->m_userData.m_id, pUser->GetDesk()->m_id, pUser->GetUserGateId(), pUser->GetUserGateUserId());
			HanderDeskUserLogin(pUser, msg->m_gateId, msg->m_geteUserId,  msg->m_ip, true, msg->m_buyInfo, msg->m_hide);
		}
		else
		{
			pUser->setUserState(LGU_STATE_CENTER, 0);
			pUser->ModifyUserState(true);
			UserManager::Instance().DelUser(pUser);
			delete pUser;

			LMsgL2LMGModifyUserState send;
			send.m_logicID = 0;
			send.m_userid = msg->m_userid;
			send.m_userstate = LGU_STATE_CENTER;
			SendToLogicManager(send);
			SendToMatchServer(send);
			LLOG_ERROR("Work::HanderUserLogin user is not on desk %d:%d:%d", pUser->m_userData.m_id, pUser->getUserGateID(), pUser->GetUserGateUserId());
		}
	}
	else
	{
		LMsgL2LMGModifyUserState send;
		send.m_logicID = 0;
		send.m_userid = msg->m_userid;
		send.m_userstate = LGU_STATE_CENTER;
		SendToLogicManager(send);
		SendToMatchServer(send);
		LLOG_ERROR("Work::HanderUserLogin user does not exist %d", msg->m_userid);
	}
}

void Work::HanderDeskUserLogin(User* pUser, Lint gateId, Lint gateUserId, Lstring& ip, bool firstLogin , Lstring buyInfo , Lint hide )
{
	if (!pUser)
	{
		return;
	}
	UserManager::Instance().DelUser(pUser);
	pUser->SetUserGateId(gateId);
	pUser->SetUserGateUserId(gateUserId);
	pUser->SetIp(ip);
	UserManager::Instance().AddUser(pUser);
	if (firstLogin)
	{
		pUser->SendItemInfo();
		pUser->SendLoginInfo(buyInfo, hide);
	}
	pUser->Login();
	Lint maYouDest = 0;
	pUser->setUserState(LGU_STATE_DESK, maYouDest);
	pUser->ModifyUserState();
}

void Work::HanderUserLogout(Lint gateId, Lint gateUserId)
{
	User* user = UserManager::Instance().GetUserByGateIdGateUserId(gateId, gateUserId);
	if (user)
	{
		LLOG_DEBUG("Work::HanderUserLogout %d:%d:%d", user->GetUserDataId(), gateId, gateUserId);

		user->Logout();
		if(!user->GetDesk())
		{
			//不在桌子中
			LLOG_INFO("Work::HanderUserLogout user is not on desk %d:%d:%d", user->GetUserDataId(), gateId, gateUserId);
			user->setUserState(LGU_STATE_CENTER);
			user->ModifyUserState(true);
			UserManager::Instance().DelUser(user);
			delete user;
		}
		else
		{
			//在桌子中
			LLOG_INFO("Work::HanderUserLogout user is on desk %d:%d:%d", user->GetUserDataId(), gateId, gateUserId);
			user->GetDesk()->HanderUserOutDesk(user);
			user->setUserState(LGU_STATE_CENTER);
			user->ModifyUserState();
		}
	}
	else
	{
		LLOG_ERROR("Work::HanderUserLogout user does exist %d:%d", gateId, gateUserId);
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if (msg->m_sp == m_logicManager)
	{
		HanderCenterLogout();
	}
	else if(msg->m_sp == m_dbClient)
	{
		HanderDBServerLogout();
	}
	else if (msg->m_sp == m_teamServer)
	{
		LLOG_ERROR("warning TeamServer lose connect!!!!!!");
	}
	else
	{
		HanderGateLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	//SocketPtrManager::Instance().AddSocketPtr(msg->m_sp);
	if(m_logicManager == msg->m_sp)
	{
		LLOG_ERROR("Connecting logicmanager on %s:%d succeed", m_logicManager->GetServerIP().c_str(), m_logicManager->GetServerPort());
		SendLogicLoginToLM();
	}
	else if (m_matchInfo.m_client == msg->m_sp)
	{
		LLOG_ERROR("Connecting match on %s:%d succeed", m_matchInfo.m_client->GetServerIP().c_str(), m_matchInfo.m_client->GetServerPort());
		SendLogicLoginToM();
	}
	else if(m_dbClient == msg->m_sp)
	{
		LLOG_ERROR("Connecting logicdb on %s:%d succeed", m_dbClient->GetServerIP().c_str(), m_dbClient->GetServerPort());
		SendLogicLoginToDB();
	}
	if (m_teamServer == msg->m_sp)
	{
		LMsgL2TS_Login loginMsg;
		loginMsg.nLogicServerID = gConfig.GetServerID();
		m_teamServer->Send(loginMsg.GetSendBuff());
		//SendMsgToTeamServer(loginMsg);
	}
}

static char* MSDK_PATH = "/cgi-bin/sdk_pay";

static char* ADMIN_PATH = "/cgi-bin/admin";

static void SendRet(Lstring errorInfo, LSocketPtr send)
{
	char strOK[512] = { 0 };

	std::string strSendBuf;
	strSendBuf.reserve(1024);
	strSendBuf += "HTTP/1.1 200 OK\r\n";

	sprintf_s(strOK, sizeof(strOK)-1, "Content-Length:%d\r\n", errorInfo.size());
	strSendBuf += strOK;
	strSendBuf += "Content-Type: text/html; charset=utf-8\r\n\r\n";

	memset(strOK,0,sizeof(strOK));
	sprintf_s(strOK, sizeof(strOK)-1, "%s", errorInfo.c_str());
	strSendBuf += strOK;

	LBuffPtr buff(new LBuff());
	buff->Write(strSendBuf.c_str(), strSendBuf.size());
	send->Send(buff);
	send->Kick();
}
void Work::HanderHttp(LMsgHttp* msg)
{	

}

void Work::ConnectToLogicManager()
{
	KeepAliveSetting setting = {true, 30000, 10000};
	m_logicManager.reset(new TCPClient(gInsideNet, gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort(), setting));
	LLOG_ERROR("Work::ConnectToLogicManager begin %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
	m_logicManager->Connect();
	LLOG_ERROR("Work::ConnectToLogicManager end %s:%d", gConfig.GetLogicManagerIp().c_str(), gConfig.GetLogicManagerPort());
}

void Work::SendToLogicManager(LMsg& msg)
{
	if(m_logicManager)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
}

void Work::HanderCenterLogout()
{
	LLOG_ERROR("warning logicmanager server lose connect!!!!!!");
	//等待重连

}

void Work::HanderDBServerLogout()
{
	LLOG_ERROR("warning DB server lose connect!!!!!!");
	//等待重连
}

void Work::HanderGateLogin(LMsgG2LLogin* msg)
{
	LLOG_DEBUG("Work::HanderGateLogin Gate Login: %d", msg->m_id);

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	auto iter = m_gateInfo.find(msg->m_id);
	if (iter != m_gateInfo.end())
	{
		if(iter->second.m_sp)
		{
			if(iter->second.m_sp != msg->m_sp)
			{
				iter->second.m_sp->Stop();
				DelGateInfo(msg->m_id);
				LLOG_ERROR("Work::HanderGateLogin: delete already existed gate info %d", msg->m_id);
			}
			else
				return;
		}

	}

	GateInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_userCount = 0;
	info.m_sp = msg->m_sp;
	m_gateInfo[info.m_id] = info;
}

void Work::HanderGateLogout(LMsgKick* msg)
{
	GateInfo* info = GetGateInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR(" Work::HanderGateLogout: %d", info->m_id);
		DelGateInfo(info->m_id);
	}
}

GateInfo* Work::GetGateInfoBySp(LSocketPtr sp)
{
	std::map<Lint, GateInfo>::iterator it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

GateInfo* Work::GetGateInfoById(Lint id)
{
	auto iter = m_gateInfo.find(id);
	if (iter != m_gateInfo.end())
		return &iter->second;

	return NULL;
}

void Work::DelGateInfo(Lint id)
{
	auto iter = m_gateInfo.find(id);
	if (iter != m_gateInfo.end())
	{
		m_gateInfo.erase(iter);
	}
}

void Work::HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate)
{
	Lint  gateUserId = msg->m_userGateId;
	Lint gateId = gate->m_id;

	if (msg->m_userMsg)
	{
		switch (msg->m_userMsgId)
		{
		case MSG_C_2_S_LOGIN:// xiangrongcheng 更改：如果在房间，发送重连就直接把消息直接推到桌子上
			//HanderUserLogin(((LMsgC2SLogin*)msg->m_userMsg)->m_id, ((LMsgC2SLogin*)msg->m_userMsg)->m_seed, gateId, gateUserId, ((LMsgC2SLogin*)msg->m_userMsg)->m_md5, msg->m_ip);
			//break;
		case MSG_C_2_S_RE_LOGIN:
			//HanderUserReLogin(((LMsgC2SReLogin*)msg->m_userMsg)->m_id, ((LMsgC2SReLogin*)msg->m_userMsg)->m_seed, gateId, gateUserId, ((LMsgC2SReLogin*)msg->m_userMsg)->m_md5, msg->m_ip);
			break;
		default:
			{
				User* user = gUserManager.GetUserByGateIdGateUserId(gateId,gateUserId);
				if (user)
				{
					user->HanderMsg(msg->m_userMsg);
				}
				else
				{
					LLOG_ERROR("Work::HanderGateUserMsg user does not exist %d:%d", gateId, gateUserId);
				}
			}
			break;
		}
	}
	else
	{
		LLOG_ERROR("Work::HanderGateUserMsg msg error %d:%d:%d", gateId, gateUserId, msg->m_userMsgId);
	}
}

Lstring Work::Md5Hex(const void* src ,Lsize len)
{
	unsigned char sign[17];
	memset(sign,0,sizeof(sign));

	MD5((const unsigned char*)src,len,sign);

	//转换成16进制
	char signHex[33];
	memset(signHex,0,sizeof(signHex));
	for(int i = 0 ; i < 16; ++i)
	{
		sprintf(signHex + i*2,"%02x", sign[i]);
	}

	return std::string(signHex);
}

//////////////////////////////////////////////////////////////////////////
//DB链接相关
void Work::ConnectToDb()
{
	KeepAliveSetting setting = {true, 30000, 10000};
	m_dbClient.reset(new TCPClient(gInsideNet, gConfig.GetDBIp().c_str(), gConfig.GetDBPort(), setting));
	LLOG_ERROR("Work::ConnectToDb begin %s:%d", gConfig.GetDBIp().c_str(), gConfig.GetDBPort());
	m_dbClient->Connect();
	LLOG_ERROR("Work::ConnectToDb end");
}

void Work::SendMsgToDb(LMsg& msg)
{
	if(m_dbClient)
	{
		m_dbClient->Send(msg.GetSendBuff());
	}
}

void  Work::PushAsynLMsgL2LMGSportResult(LMsgL2LMGSportResult& msg)
{
	m_delaySportResult.push_back(msg);
	m_delaySportTime.push_back(GetCurTime().Secs());
}

void Work::HanderUserReLogin(Lint id,Lint seed,Lint gateId, Lint gateUserId,Lstring& md5,Lstring& ip)
{
	User* user = gUserManager.GetUserbyDataId(id);
	if (user)
	{
		LMsgL2CeUerReLogin msg;
		msg.m_ID = gConfig.GetServerID();
		msg.m_userid = id;
		msg.m_seed = seed;
		msg.m_md5 = md5;
		msg.m_gateId = gateId;
		msg.m_geteUserId = gateUserId;
		msg.m_ip = ip;
		msg.m_uuid = user->m_userData.m_unioid;
		SendToLogicManager(msg);
	}
}

void Work::handerLMGCreateDesk(LMsgLMG2LCreateDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		LLOG_ERROR("user:%d create desk again,in deskid:%d,create deskid:%d",pUser->GetUserDataId(), pUser->GetDesk()->m_id,msg->m_deskID);
		pUser->initPlayGroundData(msg->m_rank,msg->m_score,msg->m_overScore, msg->m_state, msg->m_process);
		//找到玩家了,判断是否有桌子,有就加入桌子
		HanderDeskUserLogin(pUser, msg->m_gateId, msg->m_geteUserId,  msg->m_ip);
		RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
	}
	else
	{
		//没有的话，覆盖数据，创建桌子
		User* pUser = new User(msg->m_usert, msg->m_gateId, msg->m_geteUserId);
		pUser->initPlayGroundData(msg->m_rank,msg->m_score,msg->m_overScore, msg->m_state, msg->m_process);
		if (pUser)
		{
			pUser->SetIp(msg->m_ip);
			UserManager::Instance().AddUser(pUser);
			pUser->Login();	
			
			//if(msg->m_msgId == )
			//创建桌子
			Lint nErrorCode = pUser->HanderUserCreateDesk(msg);
			if (nErrorCode == 0)
			{
				//创建房间成功
				pUser->setUserState(LGU_STATE_DESK, 2);
				pUser->ModifyUserState();
			}
			else
			{
				//创建房间失败
				LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed! ErrorCode: %d deskid %d", nErrorCode, msg->m_deskID);
				HanderUserLogout(msg->m_gateId, msg->m_geteUserId);
				RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
			}
		}
		else
		{
			//创建房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk CreateTale Failed No User!");
			HanderUserLogout(msg->m_gateId, msg->m_geteUserId);
			RecycleDeskId(msg->m_deskID);	//通知LOGIC，回收桌子ID
		}
	}
}

void Work::handerLMGAddToDesk(LMsgLMG2LAddToDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		LLOG_ERROR("user:%d add desk again,in deskid:%d,create deskid:%d", pUser->GetUserDataId(), pUser->GetDesk()->m_id, msg->m_deskID);
		pUser->initPlayGroundData(msg->m_rank,msg->m_score,msg->m_overScore, msg->m_sportId, msg->m_process);
		//找到玩家了， 判断是否有桌子，有就加入桌子
		HanderDeskUserLogin(pUser, msg->m_gateId, msg->m_geteUserId,  msg->m_ip);
	}
	else
	{
		User* pUser = new User(msg->m_usert, msg->m_gateId, msg->m_geteUserId);
		if (pUser)
		{
			//这里判断用户是否能进入房间
			{
				Desk* desk = gRoomVip.GetDeskById(msg->m_deskID);
				if (desk && desk->GetCircleGoldLimit() && desk->GetCircleGoldLimit() > msg->m_usert.m_numOfCard2s)
				{
					LLOG_ERROR("handerLMGAddToDesk:: has desk limit %d me %d", desk->GetCircleGoldLimit(), msg->m_usert.m_numOfCard2s);
					LMsgS2CAddDeskRet ret;
					ret.m_deskId = msg->m_deskID;
					ret.m_errorCode = 101;
					pUser->Send(ret);

					HanderUserLogout(msg->m_gateId, msg->m_geteUserId);
					return;
				}
			}

			pUser->initPlayGroundData(msg->m_rank,msg->m_score,msg->m_overScore, msg->m_sportId, msg->m_process);
			pUser->SetIp(msg->m_ip);
			UserManager::Instance().AddUser(pUser);
			pUser->Login();	

			Lint nError = pUser->HanderUserAddDesk(msg->m_deskID);
			if (nError == 0)
			{
				//加入房间成功
				pUser->setUserState(LGU_STATE_DESK, 0);
				pUser->ModifyUserState();
			}
			else
			{
				//加入房间失败
				LLOG_ERROR("Work::handerLMGAddToDesk AddTable Failed! ErrorCode: %d deskid %d", nError, msg->m_deskID);
				HanderUserLogout(msg->m_gateId, msg->m_geteUserId);
			}
		}
		else
		{
			//加入房间失败
			LLOG_ERROR("Work::handerLMGAddToDesk AddTable Failed No User!");
			HanderUserLogout(msg->m_gateId, msg->m_geteUserId);
		}
	}
}

void Work::RecycleDeskId(Lint deskID)
{
	LMsgL2LMGRecyleDeskID msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_deskID = deskID;
	gWork.SendToLogicManager(msg);
}

void Work::SendLogicLoginToLM()
{
	LMsgL2LMGLogin login;
	login.m_id = gConfig.GetServerID();
	login.m_ip = gConfig.GetInsideIp();
	login.m_key = gConfig.GetCenterKey();
	login.m_port = gConfig.GetInsidePort();
	login.m_firstLogin = m_loginLMFirst ? 1 : 0;
	m_logicManager->Send(login.GetSendBuff());
	LLOG_ERROR("Login logicmanager succeed.");
	m_loginLMFirst = false;
}

void Work::SendLogicLoginToDB()
{
	LMsgL2LdbLogin msg;
	msg.m_key = gConfig.GetDBKey();
	msg.m_serverID = gConfig.GetServerID();
	SendMsgToDb(msg);
	LLOG_ERROR("Login logicdb succeed.");
}

void Work::SendLogicLoginToM()
{
	LMsgL2MLogin login;
	login.m_id = gConfig.GetServerID();
	login.m_ip = gConfig.GetInsideIp();
	login.m_key = gConfig.GetCenterKey();
	login.m_port = gConfig.GetInsidePort();
	login.m_firstLogin = m_loginLMFirst ? 1 : 0;
	SendToMatchServer(login);
	LLOG_ERROR("Login match succeed.");
}

void Work::CheckConfigUpdates()
{
	if (gConfig.CheckUpdates())
	{
		LLog::Instance().SetLogLevel(gConfig.GetLogLevel());
	}
}

void Work::HanderKeepAlive(LMsgS2SKeepAlive* msg)
{
	if(msg == NULL || !msg->m_sp)
		return;
	//LLOG_DEBUG("KeepAlive from %s:%d received.", msg->m_sp->GetRemoteIp().c_str(), msg->m_sp->GetRemotePort());

	LMsgS2SKeepAliveAck ack;
	msg->m_sp->Send(ack.GetSendBuff());
}

void Work::HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg)
{
	if(msg == NULL)
		return;

	if(m_logicManager == msg->m_sp)
		m_logicManager->OnKeepAliveAck();
	else if(m_dbClient == msg->m_sp)
		m_dbClient->OnKeepAliveAck();
	else if (m_matchInfo.m_client == msg->m_sp)
		m_matchInfo.m_client->OnKeepAliveAck();
	else if (m_teamServer == msg->m_sp)
		m_teamServer->OnKeepAliveAck();
}

void Work::HanderTsAddDeskRet(LMsgTS2L_AddDeskRet* msg)
{
	if (NULL == msg)
	{
		return;
	}

	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser && pUser->GetDesk())
	{
		//找到玩家了， 判断是否有桌子，有就加入桌子
		HanderDeskUserLogin(pUser, msg->m_gateId, msg->m_geteUserId, msg->m_ip);
	}
	else
	{
		User* pUser = NULL;
		try
		{
			pUser = new User(msg->m_usert, msg->m_gateId, msg->m_geteUserId);
		}
		catch (const std::exception& e)
		{
			LLOG_DEBUG("Work::handerLMGAddToDesk new UserException:%s!!", e.what());
		}
		if (NULL != pUser)
		{
			if (msg->nRet == 0)
			{
				LMsgS2CAddDeskRet ret;
				ret.m_deskId = msg->m_deskID;
				ret.m_errorCode = 4; // 和房主不是在同一个工会，无法加入 [2017-7-24 willing]
				pUser->Send(ret);

				return;
			}
			pUser->SetIp(msg->m_ip);
			UserManager::Instance().AddUser(pUser);
			pUser->Login();
			//pUser->SetLocation(msg->m_location);
			bool bSuccess = pUser->HanderUserAddDesk(msg->m_deskID);
			if (bSuccess)
			{
				//加入房间成功
				pUser->setUserState(LGU_STATE_DESK, 1);
				pUser->ModifyUserState();
			}
			else
			{
				//加入房间失败
				LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed! ");
				HanderUserLogout(msg->m_gateId, msg->m_geteUserId);
			}
		}
		else
		{
			//加入房间失败
			LLOG_ERROR("Work::handerLMGCreateDesk AddTable Failed No User!");
			HanderUserLogout(msg->m_gateId, msg->m_geteUserId);
		}
	}
}

// TeamServer 连接相关
void Work::ConnectToTeamServer(Lstring strServerIp, Lshort nPort)
{
	/*KeepAliveSetting setting = { true, 15000, 10000 };*/
#ifdef _DEBUG
	KeepAliveSetting setting = { true, 150000, 5000 };
#else // _DEBUG
	KeepAliveSetting setting = { true, 30000, 5000 };
#endif // _DEBUG
	m_teamServer.reset(new TCPClient(gInsideNet, strServerIp, nPort, setting));
	bool succeed = m_teamServer->Connect();
	if (!succeed)
		LLOG_ERROR("Work::ConnectToTeamServer failed to connect to TeamServer %s:%d", strServerIp.c_str(), nPort);
	else
		LLOG_ERROR("Succeed connect to TeamServer %s:%d", strServerIp.c_str(), nPort);
}

void Work::SendMsgToTeamServer(LMsg& msg)
{
	if (m_teamServer)
	{
		m_teamServer->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_DEBUG("LogicServer connect to TeamServer client is dump");
	}
}

void Work::HanderTeamServerInfo(LMsgLMG2L_TeamServer* msg)
{
	if (NULL == msg)
	{
		return;
	}
	ConnectToTeamServer(msg->strTeamServerIp, msg->nTeamServerPort);
}

void Work::HanderUserModifyRoomCard(LMsgLMG2LModifyRoomCard* msg)
{
	if (msg == NULL) return;
	LLOG_ERROR("HanderUserModifyRoomCard userid %d cardnum %d", msg->m_userid, msg->m_card_num);
	User* pUser = gUserManager.GetUserbyDataId(msg->m_userid);
	if (pUser)
	{
		//同步卡数
		pUser->m_userData.m_numOfCard2s = msg->m_card_num;
		Desk* desk = pUser->GetDesk();
		if (desk)
		{
			VipLogItem* log = desk->GetVip();
			if (NULL != log)
			{
				log->SetUserScore(pUser, pUser->m_userData.m_numOfCard2s);

				LMsgS2CDeskUserScoreCharge sendMsg;
				sendMsg.m_userId = pUser->GetUserDataId();
				sendMsg.m_pos = log->GetUserPos(pUser);
				sendMsg.m_score = log->GetUserScore(pUser);
				sendMsg.m_oper = msg->m_oper;
				desk->BoadCast(sendMsg);
				LLOG_DEBUG("HanderUserModifyRoomCard BoadCast");
			}
			//通知变化
			desk->BrocastUserNoGold();
		}
	}
}
