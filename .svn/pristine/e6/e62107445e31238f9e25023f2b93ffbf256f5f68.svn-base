#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "InsideNet.h"
#include "LMsgS2S.h"
#include "UserManager.h"
#include "User.h"
#include "ActiveManager.h"
#include <openssl/md5.h>
#include "DeskManager.h"
#include "LVipLog.h"
#include <boost/regex.hpp>
#include "RemoteLogThread.h"
#include "LMsgCT.h"

using namespace boost::asio;  
using boost::asio::ip::tcp; 

#define  RELOADFILE_NAME  "./__reloadconfig__"

bool CheckReloadConfigEx()
{
	FILE *fp = fopen(RELOADFILE_NAME, "r");
	if (fp)
	{
		fclose(fp);
		::remove(RELOADFILE_NAME);
		LLOG_ERROR("reload config by removing %s.", RELOADFILE_NAME);
		return true;
	}
	return false;
}

//初始化;
bool Work::Init()
{
	//设置log文件;
	LLog::Instance().SetFileName("LogicManager");

	//初始化配置文件;
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	if (!gDeskManager.Init())
	{
		LLOG_ERROR("DeskManager.Init error");
		return false;
	}

	if (!gActiveManager.Init())
	{
		LLOG_ERROR("gActiveManager.Init error");
		return false;
	}
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gInsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

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

	m_horse = "欢迎来到点点时光，请玩家文明娱乐，严禁赌博，如发现有赌博行为，将封停账号，并向公安机关举报！";

	m_buyInfo = "购买房卡请加微信 ronaldregal";
	Notice N;
	N.m_time = GetCurTime().Secs();
	N.m_title = "hello";
	N.m_content = "happy new";
	m_noticeVec.push_back(N);

	N.m_time = GetCurTime().Secs()-60*60;
	N.m_title = "游戏说明";
	N.m_content = "您在游戏中产生的任何纠纷与本平台无关";
	m_noticeVec.push_back(N);
	m_isConnect = false;
	m_hide = 0;

	LTime cur;
	m_30MSceTick = cur.MSecs();
	m_200MSceTick = cur.MSecs();
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
	m_30SceTick = cur.MSecs();
	m_600SceTick = cur.MSecs();
	m_curDate = cur.GetDate();

	return true;
}

bool Work::Final()
{
	return true;
}

//启动;
void Work::Start()
{
	m_gcService.Start();

	gInsideNet.Start();

	LRunnable::Start();

	gRLT.start();

	ConnectToCenter();

	ConnectToDb();
}

//等待;
void Work::Join()
{
	gInsideNet.Join();

	//等待逻辑线程终止;
	LRunnable::Join();

}

//停止;
void Work::Stop()
{
	m_centerClient.reset();

	m_gateInfo.clear();

	gInsideNet.Stop();

	LRunnable::Stop();

	gRLT.stop();

	m_gcService.Stop();
}

void Work::Clear()
{

}

void Work::Run()
{
	while(!GetStop())
	{
		//这里处理游戏主循环;
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
	LLOG_ERROR("LoigcManager exit!");
}

void Work::Tick(LTime& cur)
{
	//30毫秒循环一次;
	if (cur.MSecs() - m_30MSceTick > 30)
	{
		m_30MSceTick = cur.MSecs();
	}

	//200毫秒循环一次;
	if (cur.MSecs() - m_200MSceTick > 200)
	{
		m_200MSceTick = cur.MSecs();
	}

	//1000毫秒循环一次;
	if (cur.MSecs() - m_1SceTick > 1000)
	{
		//gUserManager.Tick();        //目前不需要做什么;
		//m_1SceTick = cur.MSecs();

		//金币场每秒进行一次玩家进桌分配，如果需要这里可以调整玩家进桌顺序，减少某些玩家一起的概率;
	}
	//5秒循环一次;
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
		EraseExpiredLogics(cur);
	}

	//30秒循环一次;
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		CheckConfigUpdates();
		SendGateInfoToCenter();
	}

	//5分钟循环一次;
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();

		//任务系统
		bool updated = PlayerTaskAttributeCollection::Instance()->CheckUpdates();
		if (updated)
			gUserManager.UpdatePlayerTaskList();

		Lint date = cur.GetDate();
		if (date > m_curDate)
		{
			if (!updated)
				gUserManager.UpdatePlayerTaskList();

			m_curDate = date;	//更新时间;
		}
		//任务系统End;

		//打印空闲桌子数;
		LLOG_ERROR("Free Desk Counts: %d", gDeskManager.GetFreeGoldDeskCounts());
	}

	if (m_centerClient)
		m_centerClient->OnTick(cur);
	if (m_dbClient)
		m_dbClient->OnTick(cur);
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//玩家请求登录;
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

		//////////////////////////////////////////////////////////////////////////
		//center 跟 logicmanager之间的交互;
	case MSG_CE_2_L_USER_LOGIN:
		HanderCenterUserLogin((LMsgCe2LUserLogin*)msg);
		break;
	case MSG_CE_2_L_USER_ID_INFO:
		HanderCenterUserInInfo((LMsgCe2LUserIdInfo*)msg);
		break;
	case MSG_CE_2_L_GM_CHARGE: //center发送gm充值请求;
		HanderCenterGMCharge((LMsgCe2LGMCharge*) msg);			//TO DO 效率太低;
		break;
	case MSG_CE_2_L_GM_HORSE://center发送gm跑马灯;
		HanderCenterGMHorse((LMsgCe2LGMHorse*) msg);
		break;
	case MSG_CE_2_L_GM_BUYINFO:
		HanderCenterGMBuyInfo((LMsgCe2LGMBuyInfo*) msg);
		break;
	case MSG_CE_2_L_GM_HIDE:
		HanderCenterGMHide((LMsgCe2LGMHide*) msg);
		break;
	case MSG_CE_2_LMG_GM_MODIFY_PLAYERTYPE:
		HanderCenterGMModifyPlayerType((LMsgCe2LGMModifyPlayerType*)msg);
		break;
	case MSG_CE_2_LMG_GM_CHARGE_DIAMOND:
		HanderCenterGMChargeDiamond((LMsgCe2LGMChargeDiamond*)msg);
		break;
	case MSG_CE_2_LMG_GM_PURCHASE:
		HanderCenterGMPurchase((LMsgCe2LGMPurchase*)msg);
		break;
	case MSG_CE_2_L_USER_RELOGIN:
		handerCenterUserReLogin((LMsgCe2LUserReLogin*) msg);
		break;

		//////////////////////////////////////////////////////////////////////////
		//logicmanager 和 db 之间的消息交互;
	case MSG_LDB_2_L_VIP_LOG:
		HanderDBReqVipLog((LMsgLBD2LReqVipLog*) msg);
		break;
	case MSG_LDB_2_L_VIDEO:
		HanderDBReqVideo((LMsgLDB2LReqVideo*) msg);
		break;
	case MSG_LDB_2_L_REQ_ACTIVE:
		handerDBReqActive((LMsgLDB2LActiveInfo*) msg);
		break;
	case MSG_LDB_2_L_ONE_VIP_LOG:
		HanderDBReqOneVipLog((LMsgLBD2LReqOneVipLog*) msg);
		break;
		//logicdb返回某录像是否有shareid，没有则将其从video中移至sharevideo中;
	case MSG_LDB_2_L_RETSHAREVIDEOID:
		HanderDBGetShareVideoID((LMsgLDB2LRetShareVideoId*) msg);
		break;
		//logicdb 返回 通过shareid查询到的可能的录像;
	case MSG_LDB_2_L_RETSHAREVIDEO:
		HanderDBRetShareVideo((LMsgLDB2LRetShareVideo*) msg);
		break;
	case MSG_LDB_2_LMG_BREATH:
		HanderLogicDBHeartBeat((LMsgLDB2LMGHEARTBEAT_BACK*) msg);
		break;
	case MSG_LDB_2_L_RES_SIGN_COLLECT_WIN:
		HanderDBResSignCollectWin((LMsgLDB2LResSignCollectWin*) msg);
		break;
	case MSG_LDB_2_LMG_REQ_TASK_PROGRESS:
		HanderDBRetTaskMsg((LMsgLDB2LMGTaskProgress*)msg);
		break;
	case MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS:
		HanderDBRetTaskMsg((LMsgLDB2LMGBindingRelationships*)msg);
		break;
	case MSG_LDB_2_LMG_INSERT_TASK_PROGRESS:
		HanderDBRetTaskMsg((LMsgLDB2LMGInsertTaskProgress*)msg);
		break;
	case MSG_LDB_2_LMG_INSERT_BINDING_RELATIONSHIP:
		HanderDBRetTaskMsg((LMsgLDB2LMGInsertBindingRelationship*)msg);
		break;
		//////////////////////////////////////////////////////////////////////////
		//logic 跟 logicmanager之间的交互;
	case MSG_L_2_LMG_BREATH:
		HanderLogicHeartBeat((LMsgL2LMGHEARTBEAT*) msg);
		break;
	case MSG_L_2_LMG_LOGIN:
		HanderLogicLogin((LMsgL2LMGLogin*)msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_STATE:
		HanderModifyUserState((LMsgL2LMGModifyUserState*) msg);
		break;

		//logicsrever发来更改房卡操作信息，一般为游戏结束扣房卡	;
	case MSG_L_2_LMG_MODIFY_USER_CARD:
		HanderModifyUserCardNum((LMsgL2LMGModifyCard*) msg);
		break;
	case MSG_L_2_LMG_GOLDCHANGE:
		HanderModifyUserGoldNum((LMsgL2LMGGoldChange*) msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_PLAYCOUNT:
		HanderModifyUserPlayCount((LMsgL2LMGModifyPlayCount*) msg);
		break;
	case MSG_L_2_LMG_RECYLE_DESKID:
		gDeskManager.RecycleDeskId((LMsgL2LMGRecyleDeskID*)msg);
		break;
	case MSG_L_2_LMG_RECYLE_GOLDDESKPLACE:
		gDeskManager.RecycleGoldDeskPlace((LMsgL2LMGRecyleGoldDeskPlace*)msg);
		break;
	case MSG_L_2_LMG_USE_GOLDDESKPLACE:
		gDeskManager.UseGoldDeskPlace((LMsgL2LMGUseGoldDeskPlace*)msg);
		break;
	case MSG_L_2_LMG_MODIFY_USER_BASHU:
		HanderAddUserBaishu((LMsgL2LMGAddUserBashu*) msg);
		break;
	case MSG_L_2_LMG_USER_CIRCLE_INCREASED:
		HanderUserCircleIncreased((LMsgL2LMGUserCircleIncreased*) msg);
		break;
		//////////////////////////////////////////////////////////////////////////
		//gate 跟 logicmanager之间的交互
	case MSG_G_2_LMG_BREATH:
		HanderGateHeartBeat((LMsgG2LMGHEARTBEAT*) msg);
		break;
	case MSG_G_2_LMG_LOGIN:
		HanderGateLogin((LMsgG2LMGLogin*)msg);
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
				LLOG_ERROR("Work::HanderMsg gate not exiext");
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
				LLOG_ERROR("Work::HanderMsg gate not exiext");
			}
		}
		break;
	case MSG_S_2_S_KEEP_ALIVE_ACK:
		HanderKeepAliveAck((LMsgS2SKeepAliveAck*)msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;

		// 工会服务器
	case MSG_TS_2_LMG_LOGIN:
		HanderTeamServerLogin((LMsgTS2LMG_Login*)msg);
		break;
	case MSG_TS_2_LMG_HEARTBEAT:
		HanderTeamServerHeartBeat((LMsgTS2LMG_Heartbeat*)msg);
		break;
	case MSG_TS_2_LMG_CREATE_ROOM:
		HanderTeamServerCreateDesk((LMsgTS2LMG_CreateDesk*)msg);
		break;
	case MSG_TS_2_LMG_ADD_ROOM:
		HanderTS2LMGAddDesk((LMsgTS2LMG_AddDesk*)msg);
		break;
	case MSG_TS_2_LMG_TEAM_VIDEO_LOG:
		HanderTeamVideoLog((LMsgTS2LMG_TeamVideoLog*)msg);
		break;
	case MSG_TS_2_LMG_GET_USER_INFO:
		HanderTeamGetUserInfo((LMsgTS2LMG_GetUserInfo*)msg);
		break;
	case MSG_TS_2_LMG_QUERY_IS_ONLINE:
		HanderQueryUserOnlneState((LMsgTS2LMG_QueryIsOnLine*)msg);
		break;
	case MSG_TS_2_LMG_REQ_DESKID:
		HanderTSReqDeskId((LMsgTS2LMG_ReqDeskId*)msg);
		break;
	case MSG_TS_2_LMG_REQ_EXCHAGEGOLD:
		HanderTSReqExchageGold((LMsgTS2LMG_ReqExchageGold*)msg);
		break;
	case MSG_TS_2_LMG_USER_LOGIN:
		HanderTSUserLogin((LMsgTS2LMG_UserLogin*)msg);
		break;
	default:
		break;
	}
}

void Work::HanderUserLogin(Lint id, Lint seed, Lint gateId, Lint gateUserId, Lstring& md5,Lstring& ip)
{
	std::stringstream ss;
	ss << seed << "3c6e0b8a9c15224a8228b9a98ca1531d";
	if(Md5Hex(ss.str().c_str(),ss.str().size()) != md5)
	{
		//LLOG_ERROR("Work::HanderUserLogin md5 erro %d:%d:%d:%d:%s", id, gateId, gateUserId,seed,md5.c_str()); 
		//return;
	}

	UserLoginInfo* info = GetUserLoginInfo(id);
	if (info == NULL)
	{
		//这里是登录超时，或者非法登录;
		LLOG_ERROR("Work::HanderUserLogin erro %d:%d:%d:%d", id, gateId, gateUserId,seed); 
		return;
	}

	if (info->m_seed != seed)
	{
		//这里seed不对;
		LLOG_ERROR("Work::HanderUserLogin seed error %d:%d:%d:%d:%d", id, gateId, gateUserId,seed,info->m_seed);
		return;
	}

	User* user = gUserManager.GetUserbyDataId(id);
	if (user)
	{
		user->m_userData = info->m_user;//下线期间的center更新数据.
		if (user->GetOnline())
		{
			if (user->GetUserGateId() == gateId && user->GetUserGateUserId() == gateUserId)
			{
				//这里自己登录之后，再次发送登录的消息，这里直接不理会他;
				LLOG_ERROR("Work::HanderUserLogin is online send login %s",user->m_userData.m_nike.c_str());
			}
			else
			{
				//这里玩家在线，但是另外一个连接请求登录，这里先把当前玩家踢下线，在让另外一个连接上线;
				LLOG_DEBUG("Work::HanderUserLogin user online %d:%d", user->GetUserDataId(), user->GetUserGateId());
				LMsgL2GUserOutMsg send;
				send.m_id = user->GetUserGateUserId();
				GateInfo* gateInfo = GetGateInfoById(user->GetUserGateId());
				if (gateInfo)
				{
					gateInfo->m_sp->Send(send.GetSendBuff());
				}

				UserManager::Instance().DelUser(user);

				user->SetUserGateId(gateId);
				user->SetUserGateUserId(gateUserId);
				user->SetIp(ip);
				UserManager::Instance().AddUser(user);
				//user->Login();
			}
		}
		else
		{
			//这里应该是重连了;
			LLOG_DEBUG("Work::HanderUserLogin user reconncet %d:%d", user->GetUserDataId(), user->GetUserGateId());
			LMsgL2GUserOutMsg send;
			send.m_id = user->GetUserGateUserId();
			GateInfo* gateInfo = GetGateInfoById(user->GetUserGateId());
			if (gateInfo)
			{
				gateInfo->m_sp->Send(send.GetSendBuff());
			}
			UserManager::Instance().DelUser(user);

			user->SetUserGateId(gateId);
			user->SetUserGateUserId(gateUserId);
			user->SetIp(ip);
			UserManager::Instance().AddUser(user);
			//user->Login();
			DelUserLoginInfo(user->GetUserDataId());
		}
	}
	else
	{
		//这里玩家请求正常登录;
		user = new User(info->m_user, gateId, gateUserId);
		user->SetIp(ip);
		UserManager::Instance().AddUser(user);
		//user->S2CeUserLogin();
		//user->Login();
		DelUserLoginInfo(user->GetUserDataId());		
		LLOG_DEBUG("Work::HanderUserLogin user login success %d:%d:%d:%s", user->GetUserDataId(), user->GetUserDataId(), user->GetUserGateId(),user->GetIp().c_str());
	}

	if (user)
	{
		// 通知工会服务器有玩家登陆 [2017-7-18 willing]
		LMsgLMG2TS_UserLogin noticeUserLogin;
		noticeUserLogin.nUserUnique = user->GetUserUnique();
		noticeUserLogin.nGateID = user->GetUserGateId();
		noticeUserLogin.nUserID = user->GetUserDataId();
		noticeUserLogin.nGateUserID = user->GetUserGateUserId();
		noticeUserLogin.nOnUseTeamId = user->m_userData.m_onUseTeamId;
		SendMessageToTeamServer(noticeUserLogin);
	}
}
// 处理玩家离线， 如果状态为在游戏中，并且Logic存在，则不删除；否则删除user;
void Work::HanderUserLogout(Lint gateId, Lint gateUserId)
{
	User* user = UserManager::Instance().GetUserByGateIdGateUserId(gateId, gateUserId);
	if (user)
	{
		LLOG_DEBUG("Work::HanderUserLogout %d", user->GetUserDataId());

		user->Logout();

		{
			LMsgLMG2TS_UserLogout noticeUserLogout;
			noticeUserLogout.nUserUnique = user->GetUserUnique();
			noticeUserLogout.nUserID = user->GetUserDataId();
			SendMessageToTeamServer(noticeUserLogout);
		}

		if(GetLogicInfoById(user->getUserLogicID())!= NULL || GetLogicInfoById(user->getUserGoldLogicID()) != NULL)
		{
			//玩家所在逻辑正常，只处理玩家离线状态，这里不做任何处理;

		}
		else
		{
			user->S2CeUserLogout();
			UserManager::Instance().DelUser(user);
			delete user;
		}
	}
	else
	{
		LLOG_DEBUG("Work::HanderUserLogout user not find %d:%d", gateId, gateUserId);
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	if (msg->m_sp == m_centerClient)
	{
		HanderCenterLogout();
	}
	else if(msg->m_sp == m_dbClient)
	{
		HanderDBServerLogout();
	}
	else
	{
		//WARNING：这里好像有问题，如果同时在线人数过多，这里会不停的遍历
		HanderLogicLogout(msg);
		HanderGateLogout(msg);
	}
}

void Work::HanderClientIn(LMsgIn* msg)
{
	//SocketPtrManager::Instance().AddSocketPtr(msg->m_sp);
	if (m_centerClient == msg->m_sp)
	{
		LLOG_ERROR("Connecting center on %s:%d succeed", m_centerClient->GetServerIP().c_str(), m_centerClient->GetServerPort());
		SendLoginToCenter();
		SendGateInfoToCenter();
	}
	else if (m_dbClient == msg->m_sp)
	{
		LLOG_ERROR("Connecting logicdb on %s:%d succeed", m_dbClient->GetServerIP().c_str(), m_dbClient->GetServerPort());
		SendLoginToDB();
	}
}

static char* MSDK_PATH = "/cgi-bin/sdk_pay";

static char* ADMIN_PATH = "/cgi-bin/admin";

static void SendRet(const Lstring& errorInfo, LSocketPtr send)
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

void Work::ConnectToCenter()
{
	KeepAliveSetting setting = { true, 30000, 10000 };
	m_centerClient.reset(new TCPClient(gInsideNet, gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort(), setting));
	LLOG_ERROR("Work::ConnectCenter begin %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	bool succeed = m_centerClient->Connect();
	if (succeed)
	{
		LLOG_ERROR("Work::ConnectCenter end %s:%d", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
	}
	else
	{
		LLOG_ERROR("Since failed to connect center on %s:%d, stopping...", gConfig.GetCenterIp().c_str(), gConfig.GetCenterPort());
		Sleep(1000);
		Stop();
	}
}

void Work::SendToCenter(LMsg& msg)
{
	if(m_centerClient)
	{
		m_centerClient->Send(msg.GetSendBuff());
	}
}

//发送网关信息到center
void Work::SendGateInfoToCenter()
{
	LMsgL2CeGateInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;
	auto it = m_gateInfo.begin();
	for (; it != m_gateInfo.end() && info.m_count < g_nGateNum_x; ++it)
	{
		it->second.m_userCount = gUserManager.GetGatUserCount(it->first);
		info.m_gate[info.m_count++] = (it->second);
	}
	if (m_centerClient)
	{
		m_centerClient->Send(info.GetSendBuff());
	}
}

void Work::SendLogicInfoToGates(Lint nGateID)
{
	LMsgLMG2GateLogicInfo info;
	info.m_ID = gConfig.GetServerID();
	info.m_count = 0;
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end() && info.m_count < g_nLogicNum_x; ++it)
	{
		info.m_logic[info.m_count++] = (it->second);
	}
	if (nGateID > 0)
	{
		SendMessageToGate(nGateID, info);
	}
	else
	{
		SendMessageToAllGate(info);
	}
}

//center发送 玩家请求登陆;
void Work::HanderCenterUserLogin(LMsgCe2LUserLogin*msg)
{
	//LLOG_ERROR("Work::HanderCenterUserLogin seed %d  %s", msg->m_seed,msg->user.m_openid.c_str());
	UserLoginInfo& info = m_reqLoginUser[msg->user.m_id];
	info.m_seed = msg->m_seed;
	info.m_user = msg->user;
	info.m_time = GetCurTime().Secs();
}

void Work::HanderCenterUserInInfo(LMsgCe2LUserIdInfo*msg)
{
	//LLOG_ERROR("Work::HanderCenterUserInInfo %d",msg->m_count);
	for(Lint i = 0; i < msg->m_count; ++i)
	{
		gUserManager.AddUserIdName(msg->m_info[i]);
	}
}

//center GM冲卡
void Work::HanderCenterGMCharge(LMsgCe2LGMCharge* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMCharge %d:%d:%d:%d", msg->m_userid, msg->m_cardType, msg->m_cardCount, msg->m_oper);

	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if (user)
	{
		user->AddCardCount(msg->m_cardType, msg->m_cardCount, msg->m_oper, msg->m_admin, false);
	}
	else
	{

		LLOG_ERROR("Work::HanderCenterGMCharge user not exiest %d:%d:%d:%d",  msg->m_userid, msg->m_cardType, msg->m_cardCount,msg->m_oper);
	}
}

//center GM设置跑马灯;
void Work::HanderCenterGMHorse(LMsgCe2LGMHorse* msg)
{
	//TO DO 转发Gate来处理;
	LLOG_DEBUG("Work::HanderCenterGMHorse %s",msg->m_str.c_str());
	SetHorse(msg->m_str);
}
void Work::HanderCenterGMBuyInfo(LMsgCe2LGMBuyInfo* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMBuyInfo %s", msg->m_str.c_str());
	SetBuyInfo(msg->m_str);
}

void Work::HanderCenterGMHide(LMsgCe2LGMHide* msg)
{
	SetHide(msg->m_hide);
}

void Work::HanderCenterGMModifyPlayerType(LMsgCe2LGMModifyPlayerType* msg)
{
	LLOG_DEBUG("Work::HanderCenterGMModifyPlayerType -- user id = %d, type = %s", msg->m_userId, msg->m_playerType.c_str());

	User* user = gUserManager.GetUserbyDataId(msg->m_userId);
	if(user)
	{
		user->m_userData.m_playerType = msg->m_playerType;
		LMsgS2CPlayerType playerTypeMsg;
		playerTypeMsg.m_playerType = msg->m_playerType;
		user->Send(playerTypeMsg);
	}
	else
		LLOG_ERROR("Work::HanderCenterGMModifyPlayerType -- The user does not exist. user id = %d", msg->m_userId);
}

void Work::HanderCenterGMChargeDiamond(LMsgCe2LGMChargeDiamond* msg)
{
	User* user = gUserManager.GetUserbyDataId(msg->m_userId);
	if(user)
	{
		if(!msg->m_error)
			user->m_userData.m_diamondNum += msg->m_diamondCharged;
		/*LMsgS2CUserChargeDiamond userMsg;
		userMsg.m_error = msg->m_error;
		user->Send(userMsg);*/

		if(!msg->m_error)
			user->SendItemInfo();
	}
	else
		LLOG_ERROR("Work::HanderCenterGMChargeDiamond -- The user does not exist. user id = %d", msg->m_userId);
}

void Work::HanderCenterGMPurchase(LMsgCe2LGMPurchase* msg)
{
	User* user = gUserManager.GetUserbyDataId(msg->m_userId);
	if(user)
	{
		if(!msg->m_error)
		{
			user->m_userData.m_diamondNum -= msg->m_diamondPaid;
			user->AddCardCount(msg->m_cardType, msg->m_cardAdded, msg->m_operType, "system");
		}
		LMsgS2CUserPurchase userMsg;
		userMsg.m_error = msg->m_error;
		userMsg.m_itemId = msg->m_itemId;
		user->Send(userMsg);

		if(!msg->m_error)
			user->SendItemInfo();
	}
	else
		LLOG_ERROR("Work::HanderCenterGMPurchase -- The user does not exist. user id = %d", msg->m_userId);
}

UserLoginInfo*  Work::GetUserLoginInfo(Lint userId)
{
	if (m_reqLoginUser.count(userId))
	{
		return &m_reqLoginUser[userId];
	}
	return NULL;
}


void Work::DelUserLoginInfo(Lint userId)
{
	if (m_reqLoginUser.count(userId))
	{
		m_reqLoginUser.erase(userId);
	}
}

void Work::SetHorse(Lstring& horse)
{
	m_horse = horse;

	LMsgLMG2GHorseInfo msg;
	msg.m_str = m_horse;
	SendMessageToAllGate(msg);
}

Lstring& Work::GetHorse()
{
	return m_horse;
}

void Work::SetHide(Lint hide)
{
	m_hide = hide;
}
	
Lint Work::GetHide()
{
	return m_hide;
}


void Work::SetBuyInfo(Lstring& buy)
{
	m_buyInfo = buy;
	m_vec_buyInfo.clear();
	std::string::iterator begin_index;
	bool first_flag=true;
	for(auto x=m_buyInfo.begin();x!=m_buyInfo.end();)
	{
		int tmp=3;
		bool end_flag = false;
		begin_index = x;
		while(tmp>0)
		{
			if(x!=m_buyInfo.end())
			{
				if(*x==',')
				{
					tmp--;
				}
				++x;
			}
			else{
				end_flag= true;
				break;
			}
		}
		if(!end_flag){
			if(first_flag){
				m_vec_buyInfo.push_back(std::string(begin_index,x-1));
				first_flag=false;
			}

			else
				m_vec_buyInfo.push_back(std::string(begin_index,x-1));
		}
		else{
			if(tmp==1)
			{
				m_vec_buyInfo.push_back(std::string(begin_index,m_buyInfo.end()));
			}
		}

	}
}

Lstring& Work::GetBuyInfo(Lint id)
{
	if(!m_vec_buyInfo.empty())
	{
		Lint tempID = id / 200;
		int l_index = tempID % m_vec_buyInfo.size();
		return m_vec_buyInfo[l_index];
	}
	else 
		return m_buyInfo;

}

void Work::AddNotice(Notice& notoice)
{
	m_noticeVec.push_back(notoice);
}

std::vector<Notice>& Work::GetNotice()
{
	return m_noticeVec;
}

void Work::HanderCenterLogout()
{
	LLOG_ERROR("warning center server lose connect!!!!!!");
//	Stop();
}

void Work::HanderDBServerLogout()
{
	LLOG_ERROR("warning DB server lose connect!!!!!!");
	//Stop();
	//TO DO 重连
}


void Work::HanderGateLogin(LMsgG2LMGLogin* msg)
{
	if (m_gateInfo.count(msg->m_id))
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin id exiest %d", msg->m_id);
		return;
	}

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	LLOG_ERROR("Work::HanderGateLogin Succeed! %d", msg->m_id);

	//广播GATE消息
	GateInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_ip2 = msg->m_ip2;
	info.m_port2 = msg->m_port2;
	info.m_userCount = 0;
	info.m_sp = msg->m_sp;
	m_gateInfo[info.m_id] = info;

	//同步Logic消息
	SendLogicInfoToGates(msg->m_id);
	SendTeamServerInfoToGate(msg->m_id);
	SendGateInfoToCenter();
}

void Work::HanderGateLogout(LMsgKick* msg)
{
	GateInfo* info = GetGateInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR("Work::HanderGateLogout Succeed! %d", info->m_id);
		DelGateInfo(info->m_id);
		SendGateInfoToCenter();
	}
}

GateInfo* Work::GetGateInfoBySp(LSocketPtr sp)
{
	auto it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

GateInfo* Work::GetGateInfoById(Lint id)
{
	if (m_gateInfo.count(id))
		return &m_gateInfo[id];

	return NULL;
}


void Work::DelGateInfo(Lint id)
{
	if (m_gateInfo.count(id))
	{
		m_gateInfo.erase(m_gateInfo.find(id));
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
		case MSG_C_2_S_LOGIN:
			HanderUserLogin(((LMsgC2SLogin*)msg->m_userMsg)->m_id, ((LMsgC2SLogin*)msg->m_userMsg)->m_seed, gateId, gateUserId, ((LMsgC2SLogin*)msg->m_userMsg)->m_md5, msg->m_ip);
			break;
		case MSG_C_2_S_RE_LOGIN:
			HanderUserReLogin(((LMsgC2SReLogin*)msg->m_userMsg)->m_id, ((LMsgC2SReLogin*)msg->m_userMsg)->m_seed, gateId, gateUserId, ((LMsgC2SReLogin*)msg->m_userMsg)->m_md5, msg->m_ip);
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
					LLOG_DEBUG("Work::HanderGateUserMsg user not exist %d:%d", gateId, gateUserId);//玩家离开金币场会导致这里大量日志;修改为debug日志模式;
				}
			}
			break;
		}
	}
	else
	{
		LLOG_DEBUG("Work::HanderGateUserMsg msg error %d:%d:%d", gateId, gateUserId, msg->m_userMsgId);
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
	KeepAliveSetting setting = { true, 30000, 10000 };
	m_dbClient.reset(new TCPClient(gInsideNet, gConfig.GetDBIp().c_str(), gConfig.GetDBPort(), setting));
	LLOG_ERROR("Work::ConnectToDb begin %s:%d", gConfig.GetDBIp().c_str(), gConfig.GetDBPort());
	bool succeed = m_dbClient->Connect();
	if (succeed)
		LLOG_ERROR("Work::ConnectToDb end");
	else
		LLOG_ERROR("Work::ConnectToDb failed");
}

void Work::SendMsgToDb(LMsg& msg)
{
	if(m_dbClient)
	{
		m_dbClient->Send(msg.GetSendBuff());
	}
}

//db发送玩家战绩
void Work::HanderDBReqVipLog(LMsgLBD2LReqVipLog* msg)
{
	if(!msg)
		return;
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if(user && user->GetOnline())
	{
		LMsgS2CVipLog log;
		log.m_size = msg->m_count;
		for(Lint i = 0 ; i < msg->m_count; ++i)
		{
			LVipLogItem item;
			memcpy(item.m_posUserId,msg->m_info[i].m_posUserId,sizeof(item.m_posUserId));
			item.FromString(msg->m_info[i].m_data);
			VipLogItemInfo& info = log.m_info[i];
			info.m_id = msg->m_info[i].m_id;
			info.m_time = msg->m_info[i].m_time;
			info.m_flag = msg->m_info[i].m_flag;
			info.m_reset = msg->m_info[i].m_reset;
			info.m_deskId = msg->m_info[i].m_deskId;
			// 暂时Log消息不附加游戏具体玩法;
// 			for (Lint j = 0; j < msg->m_info[i].m_playtype.size(); ++j)
// 			{
// 				info.m_playtype.push_back(msg->m_info[i].m_playtype[j]);
// 			}
			memcpy(info.m_score,msg->m_info[i].m_score,sizeof(info.m_score));
			memcpy(info.m_calScore, msg->m_info[i].m_calScore, sizeof(info.m_calScore));
			memcpy(info.m_iniScore, msg->m_info[i].m_iniScore, sizeof(info.m_iniScore));
			memcpy(info.m_userid,msg->m_info[i].m_posUserId,sizeof(info.m_userid));
			info.m_size = item.m_log.size();
			//查找玩家昵称;
			for (Lint j = 0; j < 4; ++j)
			{
				UserIdInfo* u = gUserManager.GetUserIdInfo(info.m_userid[j]);
				if (u)
				{
					info.m_nike[j] = u->m_nike;
				}
			}

			for(size_t j = 0; j < item.m_log.size(); ++j)
			{
				VipLog& l = info.m_item[j];
				VipDeskLog* dl = item.m_log[j];
				memcpy(l.m_score,dl->m_gold,sizeof(l.m_score));
				memcpy(l.m_calScore, dl->m_cal_gold, sizeof(l.m_calScore));
				l.m_time = dl->m_time;
				l.m_videoId = dl->m_videoId;
			}
		}
		user->Send(log);
	}
}

//db发送玩家录像
void Work::HanderDBReqVideo(LMsgLDB2LReqVideo* msg)
{
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if(user && user->GetOnline() && msg->m_count)
	{
		LMsgS2CVideo video;
		video.m_video = msg->m_video;
		video.m_video.PlayTypeFromString(video.m_video.m_str_playtype);
		video.m_user_count = msg->m_userCount;
		if (GetGameLogicTypeByFlag(msg->m_video.m_flag) == EMGameLogicType_DouDiZhuQP)
		{
			video.m_video.FromString(video.m_video.m_str);
			video.m_deskId = msg->m_video.m_deskId;
			video.m_curCircle = msg->m_video.m_curCircle;
			video.m_maxCircle = msg->m_video.m_maxCircle;
			video.m_flag = msg->m_video.m_flag;

			for (Lint i = 0; i < 4; ++i)
			{
				UserIdInfo* info = gUserManager.GetUserIdInfo(msg->m_video.m_userId[i]);
				if (info)
				{
					video.m_nike[i] = info->m_nike;
					video.m_sex[i] = info->m_sex;
					video.m_imageUrl[i] = info->m_headImageUrl;
				}
			}
		}
		else
		{
			video.m_video.FromString(video.m_video.m_str, video.m_user_count);
			video.m_deskId = msg->m_video.m_deskId;
			video.m_curCircle = msg->m_video.m_curCircle;
			video.m_maxCircle = msg->m_video.m_maxCircle;
			video.m_zhuang = msg->m_video.m_zhuang;
			video.m_flag = msg->m_video.m_flag;
			memcpy(video.m_score, msg->m_video.m_score, sizeof(video.m_score));

			for (Lint i = 0; i < msg->m_userCount; ++i)
			{
				UserIdInfo* info = gUserManager.GetUserIdInfo(msg->m_video.m_userId[i]);
				if (info)
				{
					video.m_nike[i] = info->m_nike;
					video.m_sex[i] = info->m_sex;
					video.m_imageUrl[i] = info->m_headImageUrl;
				}
			}
		}
		
		user->Send(video);
	}
}

//center 发送玩家活动
void Work::handerDBReqActive(LMsgLDB2LActiveInfo* msg)
{
	//处理玩家
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if(user)
	{		
		user->m_bActiveDBResponse = true;
		if (msg->m_count)
		{
			//备份数据
			user->m_Active.clear();
			for(Lint i = 0;i < msg->m_count; i ++)
			{
				LActive actorinfo;
				actorinfo.m_activeID = msg->m_DrawInfo[i].m_activeID;
				actorinfo.m_RewardID = msg->m_DrawInfo[i].m_rewardID;
				actorinfo.m_phoneNum = msg->m_DrawInfo[i].m_phoneNum;
				actorinfo.m_LasJoinDate = msg->m_DrawInfo[i].m_date;
				user->m_Active.push_back(actorinfo);
			}
		}
		if (user->GetOnline() )
		{
			user->SendNoticeDrawGift();
		}
	}
}

void Work::HanderUserReLogin(Lint id,Lint seed,Lint gateId,Lint gateUserId,Lstring& md5,Lstring& ip)
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
		SendToCenter(msg);
	}
}

void Work::handerCenterUserReLogin(LMsgCe2LUserReLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	HanderUserLogin(msg->m_userid, msg->m_seed, msg->m_gateId, msg->m_geteUserId, msg->m_md5, msg->m_ip);
}

void Work::HanderLogicLogin(LMsgL2LMGLogin* msg)
{
	if (m_logicInfo.count(msg->m_id))
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin id exiest %d", msg->m_id);
		return;
	}

	if (msg->m_key.empty())
	{
		msg->m_sp->Stop();
		LLOG_ERROR("Work::HanderGateLogin key error %d %s",msg->m_id, msg->m_key.c_str());
		return;
	}

	m_toEraseLogics.erase(msg->m_id);

	//广播Logic消息
	LLOG_ERROR("Work::HanderLogicLogin Succeed! %d", msg->m_id);
	LogicInfo info;
	info.m_id = msg->m_id;
	info.m_ip = msg->m_ip;
	info.m_port = msg->m_port;
	info.m_deskCount = 0;
	info.m_sp = msg->m_sp;
	m_logicInfo[info.m_id] = info;

	SendLogicInfoToGates();
	SendTeamServerInfoToLogic(info.m_id);
}
void Work::HanderLogicLogout(LMsgKick* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LogicInfo* info = GetLogicInfoBySp(msg->m_sp);
	if (info)
	{
		LLOG_ERROR("Work::HanderLogicLogout Succeed! %d", info->m_id);
		gDeskManager.RecycleDumpServer(info->m_id);
		DelLogicInfo(info->m_id);
		m_toEraseLogics[info->m_id] = GetCurTime().Secs();
		SendLogicInfoToGates();
	}
}

LogicInfo* Work::GetLogicInfoBySp(LSocketPtr sp)
{
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (sp == it->second.m_sp)
			return &it->second;
	}
	return NULL;
}

LogicInfo* Work::GetLogicInfoById(Lint id)
{
	if (m_logicInfo.count(id))
		return &m_logicInfo[id];

	return NULL;
}

LogicInfo* Work::GetLogicInfoByPresure(Lint flag)
{
	Lint nMiniCouunt = 100000;
	LogicInfo* logic = NULL;
	EMGameLogicType logic_type = GetGameLogicTypeByFlag(flag);
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (gConfig.GetLogic4GoldGame() == it->second.m_id)  //金币场或者特殊logic不进行自由分配;
		{
			continue;
		}
		if (it->second.m_deskCount < nMiniCouunt && -1!=it->second.m_flag)		//分配策略
		{
			EMGameLogicType type = GetGameLogicTypeByFlag(it->second.m_id);
			if (logic_type != type) continue;
			
			nMiniCouunt = it->second.m_deskCount;
			logic = &it->second;
		}
	}
	return logic;
}

void Work::DelLogicInfo(Lint id)
{
	if (m_logicInfo.count(id))
	{
		m_logicInfo.erase(m_logicInfo.find(id));
	}
}

void Work::SendMessageToAllLogic(LMsg& msg)
{
	auto logic = m_logicInfo.begin();
	for (; logic != m_logicInfo.end(); ++logic)
	{
		logic->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToAllGate(LMsg& msg)
{
	auto gate = m_gateInfo.begin();
	for (; gate != m_gateInfo.end(); ++gate)
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToLogic(Lint logicID, LMsg& msg)
{
	auto logic = m_logicInfo.find(logicID);
	if (logic != m_logicInfo.end())
	{
		logic->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::SendMessageToGate(Lint gateID, LMsg& msg)
{
	auto gate = m_gateInfo.find(gateID);
	if ( gate != m_gateInfo.end())
	{
		gate->second.m_sp->Send(msg.GetSendBuff());
	}
}

void Work::HanderModifyUserState(LMsgL2LMGModifyUserState* msg)
{
	if (msg == NULL)
	{
		return;
	}
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if (user)
	{
		LLOG_DEBUG("Work::HanderModifyUserState ID:%d, State:%d, serverID: %d", user->m_userData.m_id, msg->m_userstate, msg->m_logicID);
		user->setUserState(msg->m_userstate);
		user->setUserLogicID(msg->m_logicID);
		//Lint logicID = (LGU_STATE_CENTER == user->m_userState) ? 0 : msg->m_logicID;
		//if (gConfig.GetLogic4GoldGame() == msg->m_logicID)  //金币场;
		//{
		//	user->setUserGoldLogicID(logicID);
		//}
		//else
		//{
		//	user->setUserLogicID(logicID);
		//}
		if (LGU_STATE_CENTER == user->m_userState)
		{
			user->SendItemInfo();
			//user->SendLoginInfo();
			user->SendLoginInfo(0, user->m_userData.m_onUseTeamId, "");
			user->SendHorseInfo();
			user->SyncActiveOpen();	//登陆，同步客户端当前是否有活动;
		}
	}
}

void Work::HanderDBRetShareVideo(LMsgLDB2LRetShareVideo* msg)
{
	if(msg==NULL)
	{
		return;
	}
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if(user)
	{
		if(user->GetOnline()&&msg->m_count)
		{
			LMsgS2CShareVideo video;
			video.m_errorId = 1;
			video.m_count = msg->m_count;
			for(int x=0;x<msg->m_count;x++)
			{
				S2CShareVideo the_video;
				the_video.m_videoId = msg->m_info[x].m_videoId;
				the_video.m_time = msg->m_info[x].m_time;

				for(int i=0;i<4;i++)
				{
					UserIdInfo* u = gUserManager.GetUserIdInfo(msg->m_info[x].m_userId[i]);
					if (u)
					{
						the_video.m_userNike[i] = u->m_nike;
					}
					the_video.m_score[i] = msg->m_info[x].m_score[i];
				}
				video.m_vec_share_video.push_back(the_video);
			}
			user->Send(video);
		}
		else{
			LMsgS2CShareVideo video;
			video.m_errorId = 0;
			user->Send(video);
		}
	}
}

void Work::HanderDBGetShareVideoID(LMsgLDB2LRetShareVideoId* msg)
{
	if(msg)
	{
		LMsgS2CRetShareVideoId share_Id;
		User* the_user = gUserManager.GetUserbyDataId(msg->m_userId);
		if(msg->m_shareId ==-1)
		{
			if(the_user)
			{
				share_Id.m_errorId = 2;
				the_user->Send(share_Id);
			}
		}
		else if(msg->m_shareId !=0)
		{
			share_Id.m_shareId = msg->m_shareId;
			if(the_user)
			{
				share_Id.m_errorId = 0;
				the_user->Send(share_Id);
			}
		}
		else
		{
			share_Id.m_shareId = gDeskManager.GetShareVideoId();
			if(the_user)
			{
				share_Id.m_errorId = 0;
				the_user->Send(share_Id);
			}
			LMsgL2LDBSaveShareVideoId save_id;
			save_id.m_shareId = share_Id.m_shareId ;
			save_id.m_videoId = msg->m_videoId;
			SendMsgToDb(save_id);
			LLOG_INFO("save share video id:%s,shareID :%d",msg->m_videoId.c_str(),share_Id.m_shareId);
		}
	}
}

//处理logicserver传来的修改房卡操作;
void Work::HanderModifyUserCardNum(LMsgL2LMGModifyCard* msg)
{
	if (!msg)
	{
		return;
	}
	LLOG_DEBUG("Work::HanderModifyUserCardNum %d:%d:%d:%d", msg->m_userid, msg->cardType, msg->cardNum, msg->operType);
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if (user)
	{
		if (msg->isAddCard)
		{
			user->AddCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
		}
		else if(!gConfig.GetFree() && !gConfig.GetGameTypeFree(msg->m_nGameType))
		{
			user->DelCardCount(msg->cardType, msg->cardNum, msg->operType, msg->admin);
		}
	}
	else
	{
		LLOG_ERROR("Work::HanderCenterGMCharge user not exiest %d:%d:%d:%d",  msg->m_userid, msg->cardType, msg->cardNum, msg->operType);
	}
}

void Work::HanderModifyUserGoldNum(LMsgL2LMGGoldChange* msg)
{
	LLOG_DEBUG("Work::HanderModifyUserGoldNum %d:%d:%d", msg->m_userid, msg->m_oper, msg->m_GoldNum);
		
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if (user)
	{
		Lint nUserid = user->GetUserDataId();
		if (GAMERESULT == msg->m_oper)
		{
			Lint date = gWork.GetCurTime().GetDate();
			if (!gUserManager.m_userExDataMap.count(nUserid))
			{
				UserExData exData(1, date);
				gUserManager.m_userExDataMap[nUserid] = exData;
				LLOG_DEBUG("m_userExData:  userid:%d, times:%d, date:%d", nUserid, gUserManager.m_userExDataMap[nUserid].getGoldGameTimesOfToday(), gUserManager.m_userExDataMap[nUserid].getLastGoldGameDate());
			}
			else
			{
				if (date == gUserManager.m_userExDataMap[nUserid].getLastGoldGameDate())
				{
					gUserManager.m_userExDataMap[nUserid].addGoldGameTimes(1);
				}
				else
				{
					gUserManager.m_userExDataMap[nUserid].setGoldGameTimesOfToday(1);
					gUserManager.m_userExDataMap[nUserid].setLastGoldGameDate(date);
				}
			}
		}
		user->ChangeGoldCount(msg->m_oper, msg->m_GoldNum);
	}
	else
	{
		LLOG_ERROR("Work::HanderModifyUserGoldNum user not exiest %d:%d:%d", msg->m_userid, msg->m_oper, msg->m_GoldNum);
	}
}

void Work::HanderModifyUserPlayCount(LMsgL2LMGModifyPlayCount* msg)
{
	LLOG_DEBUG("Work::HanderModifyUserPlayCount %d : %d", msg->m_userid, msg->m_curPlayCount);

	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if (user)
	{
		user->ModifyUserPlayCount(msg->m_curPlayCount);
	}
}

void Work::HanderLogicHeartBeat(LMsgL2LMGHEARTBEAT* msg)
{
	LLOG_DEBUG("HeartBeatReceive: L_2_LMG");
	if (!msg)
		return;

	LogicInfo* logicInfo = GetLogicInfoById(msg->m_logicID);
	if (logicInfo)
	{
		LMsgLMG2LHEARTBEAT_BACK back;
		logicInfo->m_sp->Send(back.GetSendBuff());
	}
}

void Work::HanderGateHeartBeat(LMsgG2LMGHEARTBEAT* msg)
{
	LLOG_DEBUG("HeartBeatReceive: G_2_LMG");
	if (!msg)
		return;

	GateInfo* gateInfo = GetGateInfoById(msg->m_gateID);
	if (gateInfo)
	{
		LMsgLMG2GHEARTBEAT_BACK back;
		gateInfo->m_sp->Send(back.GetSendBuff());
	}
}
void Work::SendHeartBeatToLogicDB()
{
	LLOG_DEBUG("HeartBeat: LMG_2_LDB");
	LMsgLMG2LDBHEARTBEAT msg;
	msg.m_logicID = gConfig.GetServerID();
	SendMsgToDb(msg);
}

void Work::HanderLogicDBHeartBeat(LMsgLDB2LMGHEARTBEAT_BACK* msg)
{
	LLOG_DEBUG("HeartBeatReceive: LDB_2_LMG");
}

void Work::SendHeartBeatToCenter()
{
	LLOG_DEBUG("HeartBeat: LMG_2_CE");
	LMsgLMG2CEHEARTBEAT msg;
	msg.m_logicID = gConfig.GetServerID();
	if(m_centerClient)
	{
		m_centerClient->Send(msg.GetSendBuff());
	}
}

void Work::checkAndTouchLogic(){
	if(m_logicInfo.empty())return;
	
	LIniConfig	m_ini;
	std::list<std::string> fields;
	m_ini.LoadFile("logic_manager_config.ini");
	const std::string line=move(m_ini.GetString("FailoverLogics",""));

	//热更服务器;
	const char* comma_pattern=
		"(?!\\s*$)"                                
		"\\s*"                                     
		"(?:"                                      
		"'([^']*(?:[\\S\\s][^']*)*?)'"             
		"|\"([^\"]*(?:[\\S\\s][^\"]*)*?)\""        
		"|([^,'\"]*\\w[^,'\"]*?)"                  
		")"                                        
		"\\s*"                                     
		"(?:,|$)";                                 
	try{
		boost::regex pattern(comma_pattern);
		auto beg=boost::sregex_iterator(line.begin(),line.end(),pattern);
		auto end=boost::sregex_iterator();
		for(auto i=beg;i!=end; ++i){
			auto it=++i->begin();
			auto iend=i->end();
			for(;it!=iend;++it)
				if(it->length()>0){
					fields.push_back(*it);
					break;
				}
		}
	} catch(boost::regex_error& e){
		LLOG_ERROR("%s\n",e.what());
		return;
	}
	std::list<Lint> ids;	//short,no need map
	if(!fields.empty())
		for(auto i=fields.begin(),iend=fields.end();i!=iend;++i)
			try{
				ids.push_back(atoi(i->c_str()));
			}catch(...){}

	auto it=m_logicInfo.begin();
	for(; it!=m_logicInfo.end(); ++it){
		auto& info=it->second;
		info.m_flag=0;
		for(auto i=ids.begin(),iend=ids.end();i!=iend;++i)
			if(*i==info.m_id){
				info.m_flag=-1;
				break;
			}
	}

	//打印桌子数;
	auto iLogic = m_logicInfo.begin();
	for (; iLogic != m_logicInfo.end(); ++iLogic)
	{
		LLOG_ERROR("LogicIndex: %d Current Desk: %d", iLogic->first, iLogic->second.m_deskCount);
	}

	////免费付费更新;
	//Lint bFree = m_ini.GetInt("Free", 0);
	//gConfig.SetFree(bFree);
}

void Work::HanderDBReqOneVipLog(LMsgLBD2LReqOneVipLog* msg)
{
	if(!msg)
		return;
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if(user && user->GetOnline())
	{
		LMsgS2COneVipLog one_log;

		LVipLogItem item;
		item.FromString(msg->m_data);
		for (size_t i = 0; i < msg->m_playType.size(); ++i)
		{
			item.m_playtype.push_back(msg->m_playType[i]);
		}
		item.m_flag = msg->m_flag;
		one_log.m_size = item.m_log.size();
		one_log.m_user_count = getGameUserCounts(item.m_flag);
		one_log.m_flag = item.m_flag;
		for (size_t i = 0; i < item.m_playtype.size(); ++i)
		{
			one_log.m_playType.push_back(item.m_playtype[i]);
		}

		for(size_t j = 0; j < item.m_log.size(); ++j)
		{
			VipLog& l = one_log.m_videos[j];
			VipDeskLog* dl = item.m_log[j];
			memcpy(l.m_score,dl->m_gold,sizeof(l.m_score));
			memcpy(l.m_calScore, dl->m_cal_gold, sizeof(l.m_calScore));
			l.m_time = dl->m_time;
			l.m_videoId = dl->m_videoId;
		}
		user->Send(one_log);
	}
}

//db发来玩家活动;
void Work::HanderDBResSignCollectWin(LMsgLDB2LResSignCollectWin* msg)
{
	//处理玩家;
	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	Lint type = msg->m_type;
	if(type == ActiveManager::EAWin)
	{
		ActiveManager::ActiveType etype = (ActiveManager::ActiveType)msg->m_type;
		gActiveManager.SetWait(false);

		if (msg->m_count)
		{
			std::vector<LActiveSignCollectSS>	actives;	//活动信息
			//加载数据
			for(Lint i = 0;i < msg->m_count; i ++)
			{
				LActiveSignCollectSS actorinfo;
				actorinfo.m_activeType = etype;
				actorinfo.m_nickName = msg->m_DrawInfo[i].m_nickName;
				actorinfo.m_date = msg->m_DrawInfo[i].m_date;
				actives.push_back(actorinfo);
			}

			
			gActiveManager.LoadAllWinData(etype, actives);
		}
		else
		{
			gActiveManager.SetLoaded(etype, true);
		}

		
		return;
	}

	if(user)
	{		
		ActiveMgr_SignCollect::ActiveType etype = (ActiveMgr_SignCollect::ActiveType)msg->m_type;

		if (msg->m_count)
		{
			std::vector<LActiveSignCollectSS>	actives;	//活动信息
			//加载数据
			for(Lint i = 0;i < msg->m_count; i ++)
			{
				LActiveSignCollectSS actorinfo;
				actorinfo = msg->m_DrawInfo[i];
				actorinfo.m_activeType = etype;
				actives.push_back(actorinfo);
			}

			user->m_actMgr_signColl.LoadData(etype, actives);
		}
		else
		{
			user->m_actMgr_signColl.SetLoaded(etype, true);	//空数据加载
		}
	}
}

//处理logicserver传来的修改玩家新增局数的消息;
void Work::HanderAddUserBaishu(LMsgL2LMGAddUserBashu* msg)
{
	LLOG_DEBUG("Work::HanderModifyUserPlayCount %d:%d", msg->m_userid, msg->m_addBashu);

	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if (user)
	{
		if(msg->m_addBashu > 0)
			user->m_actMgr_signColl.AddChance(msg->m_addBashu);
	}
	else
	{
		LLOG_ERROR("Work::HanderModifyUserPlayCount user not exiest %d:%d",  msg->m_userid, msg->m_addBashu);
	}
}

void Work::CheckConfigUpdates()
{
	if (gConfig.CheckUpdates())
	{
		LLog::Instance().SetLogLevel(gConfig.GetLogLevel());
		checkAndTouchLogic();
	}
}

template<typename MSG>
void Work::HanderDBRetTaskMsg(MSG* msg)
{
	User* user = gUserManager.GetUserbyDataId(msg->m_userId);
	if (user != NULL)
		user->GetTaskManager()->Process(msg);
	else
		LLOG_DEBUG("Work::HanderDBRetTaskProgress, user %d does not exist. msg id = %d", msg->m_userId, msg->m_msgId);
}

void Work::HanderUserCircleIncreased(LMsgL2LMGUserCircleIncreased* msg)
{
	LLOG_DEBUG("Work::HanderUserCircleIncreased %d", msg->m_userid);

	User* user = gUserManager.GetUserbyDataId(msg->m_userid);
	if (user)
		user->GetTaskManager()->OnUserPlayingNumIncreased();
	else
		LLOG_ERROR("Work::HanderUserCircleIncreased user %d does not exist.", msg->m_userid);
}

void Work::HanderKeepAlive(LMsgS2SKeepAlive* msg)
{
	if (msg == NULL || !msg->m_sp)
		return;

	//LLOG_DEBUG("KeepAlive from %s:%d received.", msg->m_sp->GetRemoteIp().c_str(), msg->m_sp->GetRemotePort());

	LMsgS2SKeepAliveAck ack;
	msg->m_sp->Send(ack.GetSendBuff());
}

void Work::HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg)
{
	if (msg == NULL)
		return;

	if (m_centerClient == msg->m_sp)
		m_centerClient->OnKeepAliveAck();
	else if (m_dbClient == msg->m_sp)
		m_dbClient->OnKeepAliveAck();
}

void Work::EraseExpiredLogics(LTime& cur)
{
	//LLOG_DEBUG("Enter Work::EraseExpiredLogics");
	bool logicUpdated = false;
	for (auto iter = m_toEraseLogics.begin(); iter != m_toEraseLogics.end();)
	{
		if (cur.Secs() - iter->second > 120)
		{
			LLOG_ERROR("During 120 seconds, logic server %d was not reconnected with logicmanager, so clear desk infos on it", iter->first);
			gDeskManager.RecycleDumpServer(iter->first);
			DelLogicInfo(iter->first);
			logicUpdated = true;
			m_toEraseLogics.erase(iter++);
		}
		else
			++iter;
	}
	if (logicUpdated)
		SendLogicInfoToGates();

	//LLOG_DEBUG("Leave Work::EraseExpiredLogics");
}

void Work::SendLoginToCenter()
{
	LMsgL2CeLogin login;
	login.m_ID = gConfig.GetServerID();
	login.m_key = gConfig.GetCenterKey();
	login.m_ServerName = gConfig.GetServerName();
	login.m_needUserIdInfo = gUserManager.GetUserIdInfoSize() > 0 ? 0 : 1;
	m_centerClient->Send(login.GetSendBuff());
	LLOG_ERROR("Login centerserver succeed.");
}

void Work::SendLoginToDB()
{
	LMsgLMG2LdbLogin msg;
	msg.m_key = gConfig.GetDBKey();
	msg.m_serverID = gConfig.GetServerID();
	SendMsgToDb(msg);
	LLOG_ERROR("Login logicdb succeed.");
}


void Work::HanderTeamServerLogout(LMsgKick* msg)
{
	if (msg->m_sp == m_teamServerSocket)
	{
		m_strTeamServerIp.clear();
		m_nTeamServerPort = 0;
		m_TeamServerID = 0;
		m_teamServerSocket = NULL;
		m_TeamServerDeskSet.clear();
	}
}

void Work::HanderTeamServerLogin(LMsgTS2LMG_Login* msg)
{
	if (NULL == msg)
	{
		return;
	}

	LLOG_ERROR("Work::HanderTeamServerLogin");

	m_strTeamServerIp = msg->strTeamServerIp;
	m_nTeamServerPort = msg->nTeamServerPort;
	m_TeamServerID = msg->m_TeamServerID;
	m_teamServerSocket = msg->m_sp;

	/*LMsgLMG2TS_ReqDeskRet send;
	gDeskManager.HanderTSReqDeskId(send.m_deskVec);
	SendMessageToTeamServer(send);*/
	// 通知所有LogicServer 链接 这个 TeamServer [2017-7-21 willing]
	LMsgLMG2L_TeamServer LMG2L_teamServerInfo;
	LMG2L_teamServerInfo.strTeamServerIp = m_strTeamServerIp;
	LMG2L_teamServerInfo.nTeamServerPort = m_nTeamServerPort;
	SendMessageToAllLogic(LMG2L_teamServerInfo);

	// 通知所有GateServer 链接 这个 TeamServer [2017-7-21 willing]
	LMsgLMG2G_TeamServer LMG2G_teamServerInfo;
	LMG2G_teamServerInfo.m_TeamServerID = msg->m_TeamServerID;
	LMG2G_teamServerInfo.strTeamServerIp = m_strTeamServerIp;
	LMG2G_teamServerInfo.nTeamServerPort = m_nTeamServerPort;
	SendMessageToAllGate(LMG2G_teamServerInfo);
	gUserManager.SendOnlineUser2Team(msg->m_sp);
}

void Work::HanderTeamServerHeartBeat(LMsgTS2LMG_Heartbeat* msg)
{
	if (NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("Work::HanderTeamServerHeartBeat");
	if (m_teamServerSocket)
	{
		LMsgLMG2TS_Heartbeat heartbeat;
		SendMessageToTeamServer(heartbeat);
	}
}

void Work::SendMessageToTeamServer(LMsg& msg)
{
	if (m_teamServerSocket)
	{
		m_teamServerSocket->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("Work::SendMessageToTeamServer m_teamServerSocket is cannot connect!!");
	}
}

void Work::HanderTeamServerCreateDesk(LMsgTS2LMG_CreateDesk* msg)
{
	if (NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("Work::HanderTeamServerCreateDesk");

	User* pUser = gUserManager.GetUserbyDataId(msg->m_userID);
	if (pUser)
	{
		pUser->HanderTeamCreateDesk(msg);
	}
	else
	{
		LLOG_ERROR("Work::HanderTeamServerCreateDesk not fount user msg->m_userID:%d", msg->m_userID);
		// 通知 工会服务器 桌子ID 被回收 [2017-8-7 willing]
		LMsgLMG2TS_RecycleDeskId RecycleDeskIdMsg;
		RecycleDeskIdMsg.nDeskId = msg->m_deskId;
		SendMessageToTeamServer(RecycleDeskIdMsg);
		// 创建的不是免费房间，需要把刚才扣掉的卡还回去 [2017-8-1 willing]
		if (msg->m_free == 0)
		{
			// 还要通知TeamServer 把刚才预扣的卡还回去 [2017-7-23 willing]
			LMsgLMG2TS_GiveBackCard giveBackCard;
			giveBackCard.nTeamID = msg->m_teamID;
			giveBackCard.nUserID = msg->m_userID;
			giveBackCard.nFlag = msg->m_flag;
			giveBackCard.nFastPlay = msg->m_maYouDesk;
			SendMessageToTeamServer(giveBackCard);
		}
	}
}

// 是否有工会服务器的链接 [2017-7-23 willing]
bool Work::IsConnectTeamServer()
{
	if (m_teamServerSocket)
	{
		return m_teamServerSocket->IsStart();
	}

	return false;
}

void Work::HanderTeamVideoLog(LMsgTS2LMG_TeamVideoLog* msg)
{
	if (NULL == msg)
	{
		return;
	}

	User* pUser = gUserManager.GetUserbyDataId(msg->nRequestUserID);
	if (NULL == pUser)
	{
		return;
	}

	LMsgS2CVideoLog videoLog;
	videoLog.nPackageIndex = msg->nPackageIndex;

	/*if (msg->nPackageIndex != 0)
	{
	pUser->Send(videoLog);
	return;
	}*/
	for (int i = 0; i < msg->nLogCount; i++)
	{
		STeamVideoLog videoLogItem;
		videoLogItem.nTime = msg->teamVideoLog[i].nTime;
		videoLogItem.nDeskID = msg->teamVideoLog[i].nDeskID;

		for (int n = 0; n < DEFAULT_DESK_USER_COUNT; n++)
		{
			Lint nUserID = msg->teamVideoLog[i].nUserID[n];
			videoLogItem.nUserID.push_back(nUserID);
			videoLogItem.nScore.push_back(msg->teamVideoLog[i].nScore[n]);

			UserIdInfo* pUserInfo = gUserManager.GetUserIdInfo(nUserID);
			if (pUserInfo)
			{
				videoLogItem.strUserName.push_back(pUserInfo->m_nike);
			}
		}

		videoLogItem.nOptState = msg->teamVideoLog[i].nOptState;

		videoLog.teamVideoLog.push_back(videoLogItem);
	}

	pUser->Send(videoLog);
}

void Work::HanderTeamGetUserInfo(LMsgTS2LMG_GetUserInfo* msg)
{
	if (NULL == msg)
	{
		return;
	}

	User* pUser = gUserManager.GetUserbyDataId(msg->nUserID);
	if (pUser)
	{
		LMsgLMG2TS_UserInfo userInfoMsg;
		userInfoMsg.nUserID = msg->nUserID;
		userInfoMsg.strUserNick = pUser->m_userData.m_nike;
		userInfoMsg.strHeadImageUrl = pUser->m_userData.m_headImageUrl;

		SendMessageToTeamServer(userInfoMsg);
	}
}

void Work::HanderQueryUserOnlneState(LMsgTS2LMG_QueryIsOnLine* msg)
{
	if (NULL == msg)
	{
		return;
	}

	LMsgLMG2TS_UserOnLineRet userOnlineRet;
	userOnlineRet.nUserID = msg->nUserID;

	User* pUser = gUserManager.GetUserbyDataId(msg->nUserID);
	if (pUser)
	{
		userOnlineRet.nOnLine = 1;

		if (pUser->getUserLogicID() > 0)
		{
			userOnlineRet.nOnLine = 2;
		}
	}
	else
	{
		userOnlineRet.nOnLine = 0;
	}

	SendMessageToTeamServer(userOnlineRet);
}

void Work::SendTeamServerInfoToLogic(Lint nLogicID)
{
	if (m_teamServerSocket)
	{
		LMsgLMG2L_TeamServer teamServerInfo;
		teamServerInfo.strTeamServerIp = m_strTeamServerIp;
		teamServerInfo.nTeamServerPort = m_nTeamServerPort;
		SendMessageToLogic(nLogicID, teamServerInfo);
	}
}

void Work::SendTeamServerInfoToGate(Lint nGateId)
{
	if (m_teamServerSocket)
	{
		LMsgLMG2G_TeamServer teamServerInfo;
		teamServerInfo.m_TeamServerID = m_TeamServerID;
		teamServerInfo.strTeamServerIp = m_strTeamServerIp;
		teamServerInfo.nTeamServerPort = m_nTeamServerPort;
		SendMessageToGate(nGateId, teamServerInfo);
	}
}

void Work::HanderTS2LMGAddDesk(LMsgTS2LMG_AddDesk* msg)
{
	if (NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("Work::HanderTS2LMGAddDesk");

	User* pUser = gUserManager.GetUserbyDataId(msg->m_userID);
	if (pUser)
	{
		pUser->HanderTeamAddDesk(msg);
	}
	else
	{
		LLOG_DEBUG("Work::HanderTS2LMGAddDesk not fount user");
	}
}

void Work::HanderTSReqExchageGold(LMsgTS2LMG_ReqExchageGold* msg)
{
	if (NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("Work::HanderTSReqExchageGold");

	User* pUser = gUserManager.GetUserbyDataId(msg->m_userID);
	if (pUser)
	{
		pUser->HanderTeamReqExchageGold(msg);
	}
	else
	{
		LLOG_DEBUG("Work::LMsgTS2LMG_ReqExchageGold not fount user");
	}
}

void Work::HanderTSUserLogin(LMsgTS2LMG_UserLogin* msg)
{
	if (NULL == msg)
	{
		return;
	}

	LLOG_DEBUG("Work::HanderTSUserLogin");
	User* pUser = gUserManager.GetUserbyDataId(msg->nUserID);
	if (pUser)
	{
		pUser->HanderTeamUserLogin(msg);
	}
	else
	{
		LLOG_DEBUG("Work::LMsgTS2LMG_ReqExchageGold not fount user");
	}
}

void Work::HanderTSReqDeskId(LMsgTS2LMG_ReqDeskId* msg)
{
	if (msg == NULL)
		return;

	if (m_teamServerSocket)
	{
		LMsgLMG2TS_ReqDeskRet send;
		gDeskManager.HanderTSReqDeskId(send.m_deskVec);
		SendMessageToTeamServer(send);
	}
}
