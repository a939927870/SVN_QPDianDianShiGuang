#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "DbServer.h"
#include "InsideNet.h"
#include "UserManager.h"
#include "HttpRequest.h"
#include "defines.h"
#include "RemoteLogThread.h"

#if _LINUX
#include <json/json.h>
#else
#include <jsoncpp/json.h>
#endif

//初始化;
bool Work::Init()
{
	//首先设置log文件，不然log没有开启，log文件没办法查看;
	LLog::Instance().SetFileName("CenterServer");

	//初始化配置文件;
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	//设置log级别;
	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	//网络消息初始化;
	LMsgFactory::Instance().Init();

	////数据库初始化;
	if(!gDbServer.Init())
	{
		LLOG_ERROR("gDbServer.Init error!");
		return false;
	}
	if(!m_dbsession.Init(gConfig.GetDbHost(),gConfig.GetDbUser(),gConfig.GetDbPass(),gConfig.GetDbName(),"utf8mb4",gConfig.GetDbPort()))
	{
		LLOG_ERROR("m_dbsession.Init  error!");
		return false;
	}
	if(!gUserManager.Init())
	{
		LLOG_ERROR("gUserManager.Init error");
		return false;
	}

	//外部端口初始化;
	if (!gOutsideNet.Init())
	{
		LLOG_ERROR("gInsideNet.Init error");
		return false;
	}

	//gm访问端口初始化;
	if (!gGmNet.Init())
	{
		LLOG_ERROR("gGmNet.Init error");
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
	
	//主线程开启;
	LRunnable::Start();

	//数据库线程开启;
	gDbServer.Start();

	//外部网络开启;
	gOutsideNet.Start();
	gRLT.start();
	//gm网络开启;
	gGmNet.Start();
}

//等待;
void Work::Join()
{
	gGmNet.Join();

	//等待网络线程终止;
	gOutsideNet.Join();

	////等待数据库线程终止;
	//gDbServer.Stop();

	//等待逻辑线程终止;
	LRunnable::Join();
}

//停止;
void Work::Stop()
{
	m_gateInfo.clear();

	m_logicClient.clear();

	m_spMap.clear();

	gGmNet.Stop();

	gRLT.stop();

	gOutsideNet.Stop();

	gDbServer.Stop();

	LRunnable::Stop();

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
		m_1SceTick = cur.MSecs();
	}

	//30秒循环一次;
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();
		SaveCurrentOnline();
	}

	//5分钟循环一次;
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
		HanderLogicManagerCouponsInfo();
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
	case MSG_LG_CE_USER_MSG_WRAPPER:
		HanderLGWrappedMsg((LMsgLGCEUserMsgWrapper*)msg);
		break;
	case MSG_LG_2_CE_HEARTBEAT:
		HanderLGHeartbeat((LMsgLG2CEHeartBeat*)msg);
		break;
	case MSG_L_2_CE_LOGIN:
		HanderLogicLogin((LMsgL2CeLogin*)msg);
		break;
	case MSG_L_2_CE_GATE_INFO:
		HanderLogicGateInfo((LMsgL2CeGateInfo*)msg);
		break;
	case MSG_L_2_CE_USER_SAVE:
		HanderLogicSaveUser((LMsgL2CeSaveUser*) msg);
		break;
	case MSG_L_2_CE_CARD_SAVE:
		HanderLogicSaveCardInfo((LMsgL2CeSaveCardInfo*)msg);
		break;
	case MSG_L_2_CE_USER_LOGIN:
		HanderLogicUserLogin((LMsgL2CeUserServerLogin*) msg);
		break;
	case MSG_L_2_CE_USER_LOGOUT:
		HanderLogicUserLogout((LMsgL2CeUserServerLogout*) msg);
		break;
	case MSG_L_2_CE_USER_RELOGIN:
		HanderLogicUserReLogin((LMsgL2CeUerReLogin*) msg);
		break;
	case MSG_LMG_2_CE_User_Coupons_Exchange_Info:
		HanderLogicManagerCouponsExchangeInfo((LMsgLMG2CEUserCouponsExchangeInfo*) msg);
		break;
	case MSG_LMG_2_CE_User_Coupons_Exchange_Record:
		HanderLogicManagerCouponsExchangeRecord((LMsgLMG2CEUserCouponsExchangeRecord*) msg);
		break;
	case MSG_LMG_2_CE_COUPON_CHANGED:
		HanderLogicManagerCouponChanged((LMsgLMG2CeCouponChanged*)msg);
		break;
	case MSG_LMG_2_CE_OFFLINE_USER_CARD_CHANGED:
		HanderLogicManagerOfflineUserCardChaged((LMsgLMG2CeOfflineUserCardChanged*) msg);
		break;
	case MSG_LMG_2_CE_CardExchange:
		HanderLogicManagerCardExchange((LMsgLMG2CeCardExchange*) msg);
		break;
	case MSG_LMG_2_CE_User_Save_PlayCount:
		HanderLogicManagerUserPlayCount((LMsgLMG2CeSaveUserPlayCount*) msg);
		break;
	case MSG_LMG_2_CE_GOLDCHANGE:
		HanderLogicManagerGoldChange((LMsgLMG2CeGoldChange*) msg);
		break;
	case MSG_S_2_S_KEEP_ALIVE:
		HanderKeepAlive((LMsgS2SKeepAlive*)msg);
		break;
	default:
		break;
	}
}

void Work::AddSp(LSocketPtr sp)
{
	m_spMap[sp] = GetCurTime().Secs();
}

void Work::DelSp(LSocketPtr sp)
{
	if (m_spMap.count(sp))
	{
		m_spMap.erase(m_spMap.find(sp));
	}
}

void Work::HanderUserKick(LMsgKick* msg)
{
	auto itor = m_logicClient.begin();
	for (; itor != m_logicClient.end(); itor ++)
	{
		if (msg->m_sp == itor->second)
		{
			HanderLogicLogout(msg);
			break;
		}
	}

	DelSp(msg->m_sp);
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if (msg == NULL)
	{
		return;
	}
	AddSp(msg->m_sp);
}

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
	//send->Kick();
}

void Work::HanderLGWrappedMsg(LMsgLGCEUserMsgWrapper* msg)
{
	LLOG_DEBUG("Enter Work::HanderLGWrappedMsg");
	if(msg == NULL || msg->m_userMsg == NULL)
		return;

	if(msg->m_userMsg->m_msgId == MSG_C_2_S_MSG)
	{
		LMsgS2CMsg ret;
		//HanderUserLogin((LMsgC2SMsg*)msg->m_userMsg, ret);
		//LMsgLGCEUserMsgWrapper wrapper;
		//wrapper.m_idOnGate = msg->m_idOnGate;
		//wrapper.m_userMsg = &ret;
		//wrapper.m_deleteUserMsg = false;
		//msg->m_sp->Send(wrapper.GetSendBuff());
		
		int retCode = HanderUserLogin((LMsgC2SMsg*)msg->m_userMsg, ret, msg->m_sp);
		if (retCode == 1) {
			LMsgLGCEUserMsgWrapper wrapper;
			wrapper.m_idOnGate = msg->m_idOnGate;
			wrapper.m_userMsg = msg->m_userMsg;
			wrapper.m_deleteUserMsg = false;
			msg->m_sp->Send(wrapper.GetSendBuff());
		}
		else {
			LMsgLGCEUserMsgWrapper wrapper;
			wrapper.m_idOnGate = msg->m_idOnGate;
			wrapper.m_userMsg = &ret;
			wrapper.m_deleteUserMsg = false;
			msg->m_sp->Send(wrapper.GetSendBuff());
		}
		
	}
	LLOG_DEBUG("Leave Work::HanderLGWrappedMsg");
}

//处理玩家登陆的消息; 
Lint Work::HanderUserLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret, LSocketPtr loginSocketP)
{
	if (msg == NULL)
	{
		return 0;
	}
	LLOG_DEBUG("HanderUserLogin!");
	if(msg->m_plate == "wechat")
	{
		LLOG_DEBUG("HanderUserLogin wechat!");
		return HanderUserWechatLogin(msg, ret, loginSocketP);
	}
	else if (msg->m_plate == "account")
	{
		HanderUserAccountLogin(msg, ret);
	}
	else if(gConfig.GetLocalLogin()=="true" && msg->m_plate == "local"){
	//else {
		HanderUserLocalLogin(msg, ret);
	}
	else
	{
		LLOG_ERROR("Work::HanderUserLogin plate error %s",msg->m_plate.c_str());
	}
	return 0;
}

Lint Work::HanderUserWechatLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret, LSocketPtr loginSocketP)
{
	if (msg == NULL)
	{
		return 2;
	}
	ret.m_errorCode = 1;//服务器还没启动成功;
	auto itor = m_logicClient.find(msg->m_severID);
	if (itor == m_logicClient.end())
	{
		LLOG_ERROR("Work::HanderUserWechatLogin logicid not exiext %d",msg->m_severID);
		return 3;
	}

	if (msg->m_uuid.empty() || msg->m_uuid.size()>32)
	{
		LLOG_ERROR("Error! HanderUserWechatLogin, uuid is invalid!");
		return 4;
	}
	Lstring str = msg->m_openId;
	str += msg->m_accessToken;
	str += msg->m_refreshToken;
	str += msg->m_uuid;
	str += "3c6e0b8a9c15224a8228b9a98ca1531d";
	if(HttpRequest::Instance().Md5Hex(str.c_str(),str.size()) != msg->m_md5)
	{
		if (msg->m_md5.c_str())
		{
			LLOG_ERROR("Work::HanderUserWechatLogin Client msd5 %s",msg->m_md5.c_str());
		}
		else
		{
			LLOG_ERROR("Work::HanderUserWechatLogin Client msd5 is Empty!!!");
		}
		LLOG_ERROR("Work::HanderUserWechatLogin msd5 error %s : %s : %s",msg->m_openId.c_str(),msg->m_accessToken.c_str(),msg->m_md5.c_str());
		ret.m_errorCode = 6;
		return 5;
	}

	
	DUser* user = gUserManager.GetUserByOpenId(msg->m_uuid);

	//长沙麻将openid 转 uuid 的方案;
	if(user == NULL)
	{
		user = gUserManager.GetUserByOpenId(msg->m_openId);
		if (user)
		{
			user->m_usert.m_unioid = msg->m_uuid;
			gUserManager.DelUserByOpenId(msg->m_openId);
			gUserManager.AddUser(user);

			//更新数据库;
			std::stringstream ss;
			ss << "UPDATE user SET ";
			ss << "UnionId='"<<  user->m_usert.m_unioid<<"' WHERE UnionId='";
			ss << msg->m_openId << "'";

			//LLOG_ERROR("Work::HanderUserWechatLogin sql =%s", ss.str().c_str());

			gDbServer.Push(move(ss.str()), user->m_usert.m_id);
		}
	}

	if(user && gWork.GetCurTime().Secs() - user->m_usert.m_lastLoginTime < 10*60)
	{
		
	}
	else
	{
		if (user == NULL)
		{
		
			if (1 == gConfig.GetWxAuthSwitch()) {
				LLOG_DEBUG("[DEBUG] config start wx Auth");
				if (msg->m_wxConfirmed == 0) {
					LLOG_DEBUG("[DEBUG] Work::HanderUserWechatLogin m_wxConfirmed: %d, Send to Login", msg->m_wxConfirmed);
					return 1;
				}
				else {
					LLOG_DEBUG("[DEBUG] wx has Confirmed");
				}
			}
			else {
				LLOG_DEBUG("[DEBUG] config stop ex auth");
			}
			//处理性别值异常
			if (msg->m_sex < 1 || msg->m_sex > 2)
			{
				msg->m_sex = 2;
			}

			//昵称中的单引号处理
			for(Lsize i = 0 ; i < msg->m_nikename.size(); ++i)
			{
				if(msg->m_nikename[i] == '\'')
					 msg->m_nikename[i] = ' ';
			}

			user = new DUser();
			user->m_usert.m_openid = msg->m_openId;
			user->m_usert.m_unioid = msg->m_uuid;
			user->m_usert.m_nike = msg->m_nikename;
			user->m_usert.m_sex = msg->m_sex;
			user->m_usert.m_headImageUrl = msg->m_imageUrl;
			user->m_usert.m_regTime = gWork.GetCurTime().Secs();
			user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
			user->m_usert.m_location = msg->m_location;
			user->m_usert.m_address = msg->m_address;
			if (!gUserManager.CreateUser(user))
			{
				ret.m_errorCode = 4;//创建角色失败
				delete user;
			return 6;
			}
			RLOG("register", LTimeTool::GetLocalTimeInString()
				<< "|" << msg->m_severID                //gameID;
				<< "|" << user->m_usert.m_id        //userID;
				<< "|" << user->m_usert.m_sex           //sex;
				);
			gUserManager.AddUser(user);
		}
		else
		{
			//昵称中的单引号处理
			for(Lsize i = 0 ; i < msg->m_nikename.size(); ++i)
			{
				if(msg->m_nikename[i] == '\'')
					msg->m_nikename[i] = ' ';
			}

			user->m_usert.m_nike = msg->m_nikename;
			user->m_usert.m_sex = msg->m_sex;
			user->m_usert.m_headImageUrl = msg->m_imageUrl;
			user->m_usert.m_unioid = msg->m_uuid;
		}
	}

	user->m_usert.m_location = msg->m_location;
	user->m_usert.m_address = msg->m_address;
	user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
	user->m_lastReqTime = gWork.GetCurTime().Secs();
	if (user->m_logicID != msg->m_severID && user->m_logicID != 0)
	{
		//不是在本服务器登陆的玩家, 发送消息，提示退出原APP
		LLOG_DEBUG("Work::HanderUserWechatLogin logicid not empty %d : %d",msg->m_severID,user->m_logicID);
		ret.m_errorCode = 5;
		ret.m_errorMsg = string_To_UTF8(getServerNameByID(user->m_logicID)) ;
		return 7;
	}

	user->SetCopy(true);
	LMsgCe2LUserLogin send;
	send.m_seed = L_Rand(10000000, 99999999);
	send.user = user->m_usert;
	SendMsgToLogic(send, msg->m_severID);

	ret.m_errorCode = 0;
	GateInfo* info = GetGateUsefulInfo(msg->m_severID);
	if (info)
	{
		ret.m_gateIp = info->m_ip2;
		ret.m_gatePort = info->m_port2;
	}
	ret.m_id = user->m_usert.m_id;
	ret.m_seed = send.m_seed;
	LLOG_DEBUG("Handle WeChat login success!");
	return 0;
}

void Work::HanderUserLocalLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret)
{
	if (msg == NULL)
	{
		return;
	}
	ret.m_errorCode = 0;
	auto itor = m_logicClient.find(msg->m_severID);
	if (itor == m_logicClient.end())
	{
		ret.m_errorCode = 1;
		return;
	}

	msg->m_uuid = msg->m_openId;
	if (msg->m_uuid.empty() || msg->m_uuid.size() > 32)
	{
		ret.m_errorCode = 2;
		return;
	}
	DUser* user = gUserManager.GetUserByOpenId(msg->m_uuid);
	if (user == NULL)
	{
		user = new DUser();
		user->m_usert.m_openid = msg->m_openId;
		user->m_usert.m_unioid = msg->m_uuid;
		//user->m_usert.m_nike = msg->m_nike;
		user->m_usert.m_nike = msg->m_nikename;
		user->m_usert.m_sex = msg->m_sex;
		user->m_usert.m_headImageUrl = msg->m_imageUrl;
		user->m_usert.m_regTime = gWork.GetCurTime().Secs();
		user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
		if (!gUserManager.CreateUser(user))
		{
			ret.m_errorCode = 4;//创建角色失败
			delete user;
			return;
		}
		gUserManager.AddUser(user);
	}
	if (user->m_logicID != msg->m_severID && user->m_logicID > 0)
	{
		//不是在本服务器登陆的玩家, 发送消息，提示退出原APP
		ret.m_errorCode = 5;
		ret.m_errorMsg = string_To_UTF8(getServerNameByID(user->m_logicID)) ;
		return;
	}

	user->SetCopy(true);

	LMsgCe2LUserLogin send;
	send.m_seed = L_Rand(10000000, 99999999);
	send.user = user->m_usert;
	SendMsgToLogic(send, msg->m_severID);

	GateInfo* info = GetGateUsefulInfo(msg->m_severID);
	if (info)
	{
		ret.m_gateIp = info->m_ip2;
		ret.m_gatePort = info->m_port2;
	}

	ret.m_id = user->m_usert.m_id;
	ret.m_seed = send.m_seed;
	ret.m_totalPlayNum = user->m_usert.m_totalplaynum;
}

void Work::HanderUserAccountLogin(LMsgC2SMsg* msg, LMsgS2CMsg& ret)
{
	if (msg == NULL)
	{
		return;
	}
	ret.m_errorCode = 0;
	auto itor = m_logicClient.find(msg->m_severID);
	if (itor == m_logicClient.end())
	{
		ret.m_errorCode = 1;
		//msg->m_sp->Send(ret.GetSendBuff());
		return;
	}

	msg->m_uuid = msg->m_openId;
	if (msg->m_uuid.empty() || msg->m_uuid.size() > 32)
	{
		ret.m_errorCode = 2;
		//msg->m_sp->Send(ret.GetSendBuff());
		return;
	}

	Lstring str = msg->m_openId;
	str += msg->m_accessToken;
	str += msg->m_refreshToken;
	str += msg->m_uuid;
	str += "3c6e0b8a9c15224a8228b9a98ca1531d";
	if (HttpRequest::Instance().Md5Hex(str.c_str(), str.size()) != msg->m_md5)
	{
		if (msg->m_md5.c_str())
		{
			LLOG_ERROR("Work::HanderUserAccountLogin Client msd5 %s", msg->m_md5.c_str());
		}
		else
		{
			LLOG_ERROR("Work::HanderUserWechatLogin Client msd5 is Empty!!!");
		}
		LLOG_ERROR("Work::HanderUserAccountLogin msd5 error %s : %s : %s", msg->m_openId.c_str(), msg->m_accessToken.c_str(), msg->m_md5.c_str());
		ret.m_errorCode = 6;
		return;
	}

	DUser* user = gUserManager.GetUserByOpenId(msg->m_uuid);
	if (user == NULL)
	{
		LLOG_ERROR("HanderUserAccountLogin not find %s", msg->m_uuid.c_str());
		ret.m_errorCode = 7;//非法用户
		return;
	}
	else
	{
		if (user->m_logicID != msg->m_severID && user->m_logicID > 0)
		{
			LLOG_ERROR("Invalid client, client appid = %d, user appid = %d,user unionid=%d.", msg->m_severID, user->m_logicID, user->m_logicID);
			ret.m_errorCode = 5;
			ret.m_errorMsg = string_To_UTF8(getServerNameByID(user->m_logicID));
			return;
		}

		if (user->m_usert.m_provice != msg->m_passwd)
		{
			LLOG_ERROR("HanderUserAccountLogin m_passwd error %s == %s", user->m_usert.m_provice.c_str(), msg->m_passwd.c_str());
			ret.m_errorCode = 7;//非法用户
			return;
		}
	}

	user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
	user->m_lastReqTime = gWork.GetCurTime().Secs();

	LMsgCe2LUserLogin send;
	send.m_seed = L_Rand(10000000, 99999999);
	if (!gUserManager.HaveTeamId(user->m_usert.m_id, msg->m_nTeamId))
	{
		LLOG_ERROR("HanderUserAccountLogin error no team %d %d", user->m_usert.m_id, msg->m_nTeamId);
		ret.m_errorCode = 9;
		return;
	}

	if (!gUserManager.GetTeamMember(user->m_usert.m_id, msg->m_nTeamId, user->m_usert.m_numOfCard1s, user->m_usert.m_numOfCard2s, user->m_usert.m_numOfCard3s, user->m_usert.m_totalbuynum, user->m_usert.m_totalplaynum))
	{
		LLOG_ERROR("HanderUserAccountLogin error GetTeamMember %d %d", user->m_usert.m_id, msg->m_nTeamId);
		ret.m_errorCode = 9;
		return;
	}

	user->m_usert.m_useTeamId = 0;
	user->m_usert.m_onUseTeamId = msg->m_nTeamId;
	send.user = user->m_usert;
	SendMsgToLogic(send, msg->m_severID);

	GateInfo* info = GetGateUsefulInfo(msg->m_severID);
	if (info)
	{
		ret.m_gateIp = info->m_ip2;
		ret.m_gatePort = info->m_port2;
	}

	ret.m_id = user->m_usert.m_id;
	ret.m_seed = send.m_seed;
	ret.m_totalPlayNum = user->m_usert.m_totalplaynum;


	user->SetCopy(true);
	ret.m_sex = user->m_usert.m_sex;
	ret.m_imageUrl = user->m_usert.m_headImageUrl;
	ret.m_nikename = user->m_usert.m_nike;

	LLOG_DEBUG("Handle HanderUserAccountLogin login success!");
}


/*
	errorCode 类型
	0	-	请求成功
	1	-	请求url错误
	2	-	消息类型不存
	3	-	sign不正确
	4	-	请求超时
	5	-	请求方法错误
	6	-	请求参数错误
	7	-	玩家不存在
	8	-	数据库读写错误
*/

void Work::HanderHttp(LMsgHttp* msg)
{	
	if (msg == NULL)
	{
		return;
	}
 	if (msg->m_head.size() > 4 && msg->m_head.substr(0, 4) == "GET ")
 	{
 		std::string::size_type pos = msg->m_head.find("\r\n");
 		if (pos == std::string::npos || pos < 13)
 		{
 			LLOG_ERROR("Work::HanderHttp GET error");
			SendRet("{\"errorCode\":5,\"errorMsg\":\"req function error\"}",msg->m_sp);
 			return;
 		}
 
 		std::string totalUrl = msg->m_head.substr(4, pos-13);
 		std::string::size_type pos2 = totalUrl.find('?', 0);
 		if (pos2 == std::string::npos)
 		{
 			LLOG_ERROR("Work::HanderHttp GET param error");
			SendRet("{\"errorCode\":6,\"errorMsg\":\"req param error\"}", msg->m_sp);
 			return;
 		}
 
 		std::string reqType = totalUrl.substr(0, pos2);
 		std::string reqParam = totalUrl.substr(pos2 + 1, totalUrl.size() - pos2);
 		if (reqType == ADMIN_PATH )
 		{
  			std::map<Lstring, Lstring> paraMaps;
  			HttpRequest::Instance().DeodeUrlParams(reqParam, paraMaps, true);
			if (!HanderCheckMd5(paraMaps))
			{
				SendRet("{\"errorCode\":3,\"errorMsg\":\"sign error\"}", msg->m_sp);//签名错误
				return;
			}

			//Lint time = atoi(paraMaps["time"].c_str());
			//if (gWork.GetCurTime().Secs() - time > 5)
			//{
			//	SendRet("{\"errorCode\":4,\"errorMsg\":\"time out\"}", msg->m_sp);//超时
			//	return;
			//}
  			if (paraMaps["msg"] == "charge")
				HanderGMCharge(paraMaps,msg->m_sp);
			else if(paraMaps["msg"] == "chargeDiamond")
				HanderGMChargeDiamond(paraMaps, msg->m_sp);
			else if(paraMaps["msg"] == "purchase")
				HanderGMPurchase(paraMaps, msg->m_sp);
  			else if (paraMaps["msg"] == "horse")
				HanderGMHorse(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "buy")
				HanderGMBuyInfo(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "create_account")
				HanderGMCreateAccount(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "update_account")
				HanderGMUpdateAccount(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "update_account_info")
				HanderGMUpdateAccountInfo(paraMaps, msg->m_sp);
			else if(paraMaps["msg"] == "getUserInfo")
				HanderGMGetUserInfo(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "modifyPlayerType")
				HanderGMModifyPlayerType(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "online")
				HanderGMOnlineCount(paraMaps, msg->m_sp);
			else if (paraMaps["msg"] == "close")
				HanderGMCloseServer(paraMaps, msg->m_sp);
			else
				SendRet("{\"errorCode\":2,\"errorMsg\":\"msg type error\"}", msg->m_sp);//消息类型错误
 		}
 		else
 		{
			SendRet("{\"errorCode\":1,\"errorMsg\":\"get url error\"}", msg->m_sp);//请求链接错误
 		}
 	}
}


void Work::HanderLogicLogin(LMsgL2CeLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	//判断key是否正确
	if (msg->m_key != gConfig.GetCenterKey())
	{
		msg->m_sp->Stop();
		return;
	}
	auto itor = m_logicClient.find(msg->m_ID);
	if (itor != m_logicClient.end())
	{
		if (itor->second != NULL)
		{
			if (itor->second->IsStart() && itor->second != msg->m_sp)
			{
				msg->m_sp->Stop();
				return;
			}
		}
	}

	m_logicClient[msg->m_ID] = msg->m_sp;
	serverInfo[msg->m_ID] = msg->m_ServerName;
	//发送礼券信息
	HanderLogicManagerCouponsInfo(msg->m_ID);

	gUserManager.LoadUserIdInfo(msg->m_ID);
	DelSp(m_logicClient[msg->m_ID]);
}

void Work::HanderLogicLogout(LMsgKick*msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_ERROR("Work::HanderLogicLogout");
	auto itor = m_logicClient.begin();
	for (; itor != m_logicClient.end(); itor ++)
	{
		if (msg->m_sp == itor->second)
		{
			Lint	serverID = itor->first;
			m_logicClient.erase(itor);
			auto iGate = m_gateInfo.find(serverID);
			if (iGate != m_gateInfo.end())
			{
				m_gateInfo.erase(iGate);
			}
			break;
		}
	}
}

void Work::HanderLogicGateInfo(LMsgL2CeGateInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderLogicGateInfo %d", msg->m_ID);
		return;
	}
	auto itor = m_logicClient.find(msg->m_ID);
	if (itor == m_logicClient.end())
	{
		msg->m_sp->Stop();
		return;
	}
	m_gateInfo[msg->m_ID].clear();
	for (Lint i = 0; i < msg->m_count; ++i)
	{
		m_gateInfo[msg->m_ID][msg->m_gate[i].m_id] = msg->m_gate[i];
	}
}

void Work::HanderLogicSaveCardInfo(LMsgL2CeSaveCardInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	std::stringstream ss;
	ss << "INSERT INTO charge (Time,CardType,CardNum,Type,UserId,Admin, TotalCardNum, TeamId) VALUES (";
	ss << "'" << msg->m_charge.m_time << "',";
	ss << "'" << msg->m_charge.m_cardType << "',";
	ss << "'" << std::to_string(msg->m_charge.m_cardNum) << "',";
	ss << "'" << msg->m_charge.m_type << "',";
	ss << "'" << msg->m_charge.m_userId << "',";
	ss << "'" << msg->m_charge.m_admin << "',";
	ss << "'" << std::to_string(msg->m_charge.m_iTotalCardNum) << "',";
	ss << "'" << msg->m_charge.m_teamId << "')";

	LLOG_DEBUG("Work::HanderLogincSaveCardInfo save %s", ss.str().c_str());

	gDbServer.Push(move(ss.str()), msg->m_charge.m_userId);
}


void Work::HanderLogicSaveUser(LMsgL2CeSaveUser* msg)
{
	if (msg == NULL)
	{
		return;
	}

	DUser* user = gUserManager.GetUserByOpenId(msg->m_user.m_unioid);
	if(user == NULL)
	{
		LLOG_ERROR("Work::HanderLogincSaveUser save not exiest %d:%s",msg->m_user.m_id,msg->m_user.m_unioid.c_str());
		return;
	}

	if (user->m_logicID != msg->m_ID && user->m_logicID != 0)
	{
		LLOG_ERROR("Work::HanderLogincSaveUser logic error %d:%d:%d:%s",msg->m_user.m_id,user->m_logicID,msg->m_ID,msg->m_user.m_unioid.c_str());
		return;
	}
	
	user->SetCopy(true);
	Lint diamondNum = user->m_usert.m_diamondNum;
	user->m_usert = msg->m_user;
	user->m_usert.m_diamondNum = diamondNum;		// don't need to sync diamondNum bilaterally
	gUserManager.SaveUser(user);	
}

GateInfo* Work::GetGateUsefulInfo(Lint logicID)
{
	static Lint sindex = 0;
	GateInfo* info = NULL;
	auto itor = m_gateInfo.find(logicID);
	if (itor == m_gateInfo.end())
	{
		return info;
	}
	
	Lint index = 0;
	auto it = itor->second.begin();
	for (; it != itor->second.end(); ++it)
	{
		if (index++ % itor->second.size() == sindex % itor->second.size())
		{
			info = &it->second;
			break;
		}
	}

	sindex++;
	return info;
}

void Work::SendMsgToLogic(LMsg& msg, Lint logicID)
{
	auto itor = m_logicClient.find(logicID);
	if (itor != m_logicClient.end())
	{
		itor->second->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("void Work::SendMsgToLogic ERROR: %d is invilid", logicID);
	}
}

void Work::SendMsgToLogic(LMsg& msg, Lint logicID, int iLimitCountInSendPool)
{
	auto itor = m_logicClient.find(logicID);
	if (itor != m_logicClient.end())
	{
		while(itor->second->getSendPoolSize() > iLimitCountInSendPool)
		{
			boost::this_thread::sleep(boost::posix_time::millisec(30));
		}

		itor->second->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("void Work::SendMsgToLogic ERROR: %d is invilid", logicID);
	}
}

//logicmanage发送玩家已经登录的信息;
void Work::HanderLogicUserLogin(LMsgL2CeUserServerLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::HanderLogicUserLogin, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_openID);
	if (user == NULL)
	{
		//没找到玩家;
		return;
	}

	auto itor = m_logicClient.find(msg->m_serverID);
	if (itor == m_logicClient.end())
	{
		//没找到这台服务器;
		user->m_logicID = 0;
		return;
	}
	user->m_usert.m_onUseTeamId = msg->m_useTeamId;
	user->m_usert.m_useTeamId = msg->m_useTeamId;
	user->m_logicID = msg->m_serverID;
	user->m_usert.m_lastLoginTime =  gWork.GetCurTime().Secs();
	gUserManager.SaveUserExcepteSomeInfo(user);
}

void Work::HanderLogicUserLogout(LMsgL2CeUserServerLogout* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::HanderLogicUserLogot, userID: %s, ServerID %d", msg->m_openID.c_str(), msg->m_serverID);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_openID);
	if (user == NULL)
	{
		//没找到玩家;
		return;
	}

	auto itor = m_logicClient.find(msg->m_serverID);
	if (itor == m_logicClient.end())
	{
		//没找到这台服务器;
		return;
	}
	user->m_logicID = 0;
	user->m_usert.m_onUseTeamId = 0; 
	user->m_usert.m_useTeamId = 0; 
	gUserManager.SaveUserExcepteSomeInfo(user);		//玩家离线，保存玩家数据;
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
	reqsort += "1234567890987654321";


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

void Work::HanderGMCharge(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin openId=openId cardType=1 cardNum=1 operType=1 sign=sign time=time

	LLOG_ERROR("Work::HanderGMCharge %s:%s %s %lf", param["openId"].c_str(), param["admin"].c_str(), param["teamId"].c_str(), atof(param["cardNum"].c_str()));

	Lint	errorCode = 0;
	DUser*	user = gUserManager.GetUserByOpenId(param["openId"]);
	Ldouble	nShift = atof(param["cardNum"].c_str());
	Lint	nReason = atoi(param["operType"].c_str());
	Lint	nTeamId = atoi(param["teamId"].c_str());

	if (user)
	{
		//完全在线状态
		if ((user->m_usert.m_useTeamId != 0) && (user->m_usert.m_useTeamId == user->m_usert.m_onUseTeamId) && (nTeamId == user->m_usert.m_useTeamId))
		{
			if ((nShift < 0.0f) && (user->m_logicID > 0))//这里在logic里面有备份
			{
				std::stringstream t_os;
				t_os << "{\"errorCode\":11,\"errorMsg\":\""<< user->m_usert.m_onUseTeamId <<"\"}";
				SendRet(t_os.str(), sp);//玩家在房间或没下线
				return;
			}

			HanderGMCharge_online(param, sp, user, nShift, nReason);
		}//登录状态
		else if ((user->m_usert.m_onUseTeamId != 0) && nTeamId == user->m_usert.m_onUseTeamId) //如果正在使用的逻辑
		{
			if ((nShift < 0.0f) && (user->m_logicID > 0))//这里在logic里面有备份
			{
				std::stringstream t_os;
				t_os << "{\"errorCode\":11,\"errorMsg\":\"" << user->m_usert.m_onUseTeamId << "\"}";
				SendRet(t_os.str(), sp);//玩家在房间或没下线
				return;
			}

			HanderGMCharge_online(param, sp, user, nShift, nReason);
		}
		else //完全下线状态
		{
			HanderGMCharge_onoff(param, sp, user->m_usert.m_id, nTeamId, nShift, nReason, 1);
		}
	}
	else
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"user not exiest\"}", sp);//玩家不存在
	}
}

void Work::HanderGMChargeDiamond(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	Lint errCode = 6;
	const Lstring keyList[] = 
	{
		"openId",
		"diamond",
		"userCharge",
		"orderNo",
		"serverId"
	};

	if(CheckHttpParameters(param, _countof(keyList), keyList))
	{
		errCode = 7;
		Lstring openId = param["openId"];
		DUser* user = gUserManager.GetUserByOpenId(openId);
		if(user)
		{
			Lint quantity = atoi(param["diamond"].c_str());
			Lint userCharge = atoi(param["userCharge"].c_str());
			Lstring orderNo = param["orderNo"];
			Lint serverId = atoi(param["serverId"].c_str());
			//if(quantity > 0)
			errCode = m_fService.ChargeDiamondFor(user, quantity, userCharge != 0, orderNo);
			NotifyChargingResultToLogicServer(user, errCode == 0, quantity);
		}
	}
	Lstring ret= FormatHttpResponseMsg(errCode);
	LLOG_ERROR(ret.c_str());
	SendRet(move(ret), sp);
}

void Work::HanderGMPurchase(std::map<Lstring,Lstring>& param,LSocketPtr sp)
{
	Lint errCode = 6;
	const Lstring keyList[] = 
	{
		"openId",
		"itemId",
		"diamond",
		"cardNum",
		"vipGive",
		"cardType",
		"orderNo",
		"serverId"
	};

	if(CheckHttpParameters(param, _countof(keyList), keyList))
	{
		errCode = 7;
		Lstring openId = param["openId"];
		DUser* user = gUserManager.GetUserByOpenId(openId);
		if(user)
		{
			Lint itemId = atoi(param["itemId"].c_str());
			Lint diamondPaid = atoi(param["diamond"].c_str());
			Lint cardAdded =  atoi(param["cardNum"].c_str());
			Lint cardForVIP = atoi(param["vipGive"].c_str());
			Lint cardType = atoi(param["cardType"].c_str());
			Lstring orderNo = param["orderNo"];
			Lint serverId = atoi(param["serverId"].c_str());
			errCode = 6;
			if((diamondPaid > 0) && (cardAdded > 0) && (CARD_TYPE_4 <= cardType && cardType <= CARD_TYPE_16))
				errCode = m_fService.PurchaseItem(user, itemId, diamondPaid, cardAdded, cardForVIP, cardType, orderNo);
			NotifyPurchasingResultToLogicServer(user, errCode == 0, itemId, diamondPaid, cardAdded + cardForVIP, cardType, CARDS_OPER_TYPE_PURCHASE_GIVE);
		}
	}
	Lstring ret= FormatHttpResponseMsg(errCode);
	LLOG_ERROR(ret.c_str());
	SendRet(move(ret), sp);
}

void Work::HanderGMHorse(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin horse=horse sign=sign time=time serverid=10
	LMsgCe2LGMHorse horse;
	horse.m_str = param["horse"];
	if (param["serverid"].empty())
	{
		auto iServer = m_logicClient.begin();
		for (; iServer != m_logicClient.end(); ++iServer)
		{
			SendMsgToLogic(horse, iServer->first);
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(horse, nServerID);
	}

	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMBuyInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin buy=buy sign=sign time=time serverid=10
	LMsgCe2LGMBuyInfo buy;
	buy.m_str = param["buy"];
	if (param["serverid"].empty())
	{
		auto iServer = m_logicClient.begin();
		for (; iServer != m_logicClient.end(); ++iServer)
		{
			SendMsgToLogic(buy, iServer->first);
		}
	}
	else
	{
		Lint nServerID = atoi(param["serverid"].c_str());
		SendMsgToLogic(buy, nServerID);
	}
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}

void Work::HanderGMGetUserInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	Lint errCode = 6;
	const Lstring keyList[] = 
	{
		"openId",
	};

	if(CheckHttpParameters(param, _countof(keyList), keyList))
	{
		errCode = 7;
		DUser* user = gUserManager.GetUserByOpenId(param["openId"]);
		if(user != NULL)
		{
			char chId[20] = {0}, chCardNum[20] = {0}, chDiamondNum[20] = {0}, chSex[20] = {0};
			sprintf_s(chId, 20, "%d", user->m_usert.m_id);
			sprintf_s(chCardNum, 20, "%f", user->m_usert.m_numOfCard2s);
			sprintf_s(chDiamondNum, 20, "%d", user->m_usert.m_diamondNum);
			sprintf_s(chSex, 20, "%d", user->m_usert.m_sex);

			Lstring strRet;
			strRet += "{\"errorCode\":0,\"errorMsg\":\"success\"";

			strRet += ",\"id\":\"" + Lstring(chId) + "\"";
			strRet += ",\"playerType\":\"" + user->m_usert.m_playerType + "\"";
			//strRet += ",\"nike\":\"" + user->m_usert.m_nike + "\"";
			strRet += ",\"cardNum\":\"" + Lstring(chCardNum) + "\"";
			strRet += ",\"diamondNum\":\"" + Lstring(chDiamondNum) + "\"";
			strRet += ",\"headPotoUrl\":\"" + user->m_usert.m_headImageUrl + "\"";
			strRet += ",\"sex\":\"" + Lstring(chSex) + "\"";

			strRet += "}";

			SendRet(move(strRet), sp);
			errCode = 0;
		}
	}
	Lstring errInfo = FormatHttpResponseMsg(errCode);
	LLOG_ERROR(errInfo.c_str());
	if(errCode != 0)
		SendRet(move(errInfo), sp);
}

void Work::HanderGMModifyPlayerType(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	Lint errCode = 6;
	const Lstring keyList[] = 
	{
		"openId",
		"playerType",
	};

	if(CheckHttpParameters(param, _countof(keyList), keyList))
	{
		errCode = 7;
		DUser*	user = gUserManager.GetUserByOpenId(param["openId"]);
		if(user != NULL)
		{
			user->m_usert.m_playerType = param["playerType"];
			gUserManager.SaveUserExcepteSomeInfo(user);

			if(user->m_logicID)
			{
				LMsgCe2LGMModifyPlayerType gmModifyPlayerType;
				gmModifyPlayerType.m_userId = user->m_usert.m_id;
				gmModifyPlayerType.m_playerType = user->m_usert.m_playerType;
				SendMsgToLogic(gmModifyPlayerType, user->m_logicID);
			}
			errCode = 0;
		}
	}
	Lstring ret = FormatHttpResponseMsg(errCode);
	LLOG_ERROR(ret.c_str());
	SendRet(move(ret), sp);
}

void Work::HanderGMOnlineCount(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	//admin=admin sign=sign time=time

	std::stringstream ss;
	ss << "{\"errorCode\":0,\"errorMsg\":\"success\",\"data\":[";

	Lint nCount = 0;
	auto it = m_gateInfo.begin();
	for (; it != m_gateInfo.end(); it ++)
	{
		auto itor = it->second.begin();
		for (; itor != it->second.end(); itor ++)
		{
			nCount += itor->second.m_userCount;
		}
	}

	ss << "{\"count\":" << nCount << "},";

	Lstring tmp = ss.str().substr(0, ss.str().size() - 1);
	tmp += "]}";

	SendRet(tmp, sp);
}

void Work::HanderGMCloseServer(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_DEBUG("Work::HanderGMCloseServer %s", param["cmd"].c_str());
	if (param["cmd"] == "close_server_ty_ll")
	{
		SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
		Stop();
		
	}
	else
	{
		SendRet("{\"errorCode\":10,\"errorMsg\":\"close cmd error\"}", sp);//成功
		//Stop();
	}
}

void Work::HanderLGHeartbeat(LMsgLG2CEHeartBeat* msg)
{
	if(msg == NULL)
		return;
	LLOG_DEBUG("heartbeat from logingate %s:%d received.", msg->m_sp->GetRemoteIp().c_str(), msg->m_sp->GetRemotePort());
	auto iter = m_spMap.find(msg->m_sp);
	if(iter != m_spMap.end())
		iter->second = GetCurTime().Secs();
}

DBSession& Work::GetDbSession()
{
	return m_dbsession;
}

Lstring& Work::getServerNameByID(Lint serverID)
{
	static Lstring unknowServer = "未知麻将";
	std::map<Lint, Lstring>::iterator itor = serverInfo.find(serverID);
	if (itor != serverInfo.end())
	{
		return serverInfo[serverID];
	}
	return unknowServer;
}

void Work::SaveCurrentOnline()
{
	auto iServer = m_gateInfo.begin();
	for (; iServer != m_gateInfo.end(); iServer ++)
	{
		Lint nCount = 0;
		auto iGate = iServer->second.begin();
		for (; iGate != iServer->second.end(); iGate ++)
		{
			nCount += iGate->second.m_userCount;
			//考虑在此增加金币场玩家在线打点记录;不增加数据库读写负担;Gate中需区分金币场和房卡场玩家;
		}

		std::stringstream ss;	
		ss << "insert into onlinelog(DateTime, serverID, serverName, onlineCount) values('";
		ss << gWork.GetCurTime().Secs() << "','";
		ss << iServer->first << "','";
		ss << string_To_UTF8(getServerNameByID(iServer->first))  << "','";
		ss << nCount << "')";

		//LLOG_DEBUG("Work::HanderUserWechatLogin sql =%s", ss.str().c_str());
		gDbServer.Push(move(ss.str()), 0);
	}
}

void Work::HanderLogicUserReLogin(LMsgL2CeUerReLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LLOG_DEBUG("Work::HanderLogicUserReLogin, userID: %s, ServerID %d", msg->m_uuid.c_str(), msg->m_ID);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_uuid);
	if (user == NULL)
	{
		//没找到玩家;
		return;
	}

	auto itor = m_logicClient.find(msg->m_ID);
	if (itor == m_logicClient.end())
	{
		//没找到这台服务器;
		return;
	}
	if (user->m_logicID != 0)
	{
		auto itor = m_logicClient.find(user->m_logicID);
		if (itor == m_logicClient.end())
		{
			//没找到这台服务器, 或这台服务器宕机了， 允许登陆其他服务器;
		}		
		else if (user->m_logicID != msg->m_ID )
		{
			//玩家所在所务器正常连接，并且玩家在这台服务器;
			return;
		}
	}

	//允许玩家登陆, 直接记录， 防止有玩家两个APP同时请求
	user->m_logicID = msg->m_ID;

	LMsgCe2LUserLogin send;
	send.m_seed = msg->m_seed;
	send.user = user->m_usert;
	SendMsgToLogic(send, msg->m_ID);


	LMsgCe2LUserReLogin relogin;
	relogin.m_ip = msg->m_ip;
	relogin.m_gateId = msg->m_gateId;
	relogin.m_geteUserId = msg->m_geteUserId;
	relogin.m_md5 = msg->m_md5;
	relogin.m_seed = msg->m_seed;
	relogin.m_userid = msg->m_userid;
	SendMsgToLogic(relogin, msg->m_ID);
}

/*
void Work::HanderGMSetHide(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_DEBUG("Work::HanderGMSetHide %s", param["cmd"].c_str());
	
}*/

Lstring Work::GetHttpErrDescription(Lint errCode)
{
	const char* descriptions[] =
	{
		"success",
		"get url error",
		"msg type error",
		"sign error",
		"time out",
		"req function error",
		"req param error",
		"user not exist",
		"failed to access database",
	};

	Lstring description;
	if(errCode >= 0 && errCode < _countof(descriptions))
		description = descriptions[errCode];
	return move(description);
}

Lstring Work::FormatHttpResponseMsg(Lint errCode)
{
	std::stringstream ss;
	Lstring errMsg = GetHttpErrDescription(errCode);
	ss << "{\"errorCode\":" << errCode << ",\"errorMsg\":\"" << errMsg << "\"}";
	return move(ss.str());
}

bool Work::CheckHttpParameters(std::map<Lstring, Lstring>& param, int keyNum, const Lstring* keyList)
{
	bool ok = true;
	for(Lint i = 0; i < keyNum && ok; i++)
		ok = param.find(keyList[i]) != param.end();
	return ok;
}

void Work::NotifyPurchasingResultToLogicServer(DUser* user, bool succeed, Lint itemId, Lint diamondPaid, Lint cardAdded, Lint cardType, Lint operType)
{
	if(user->m_logicID)
	{
		LMsgCe2LGMPurchase msg;
		msg.m_userId = user->m_usert.m_id;
		msg.m_error = succeed ? 0 : 1;
		msg.m_itemId = itemId;
		msg.m_diamondPaid = diamondPaid;
		msg.m_cardAdded = cardAdded;
		msg.m_cardType = cardType;
		msg.m_operType = operType;
		SendMsgToLogic(msg, user->m_logicID);
	}
}

void Work::NotifyChargingResultToLogicServer(DUser* user, bool succeed, Lint diamondCharged)
{
	if(user->m_logicID)
	{
		LMsgCe2LGMChargeDiamond msg;
		msg.m_userId = user->m_usert.m_id;
		msg.m_error = succeed ? 0 : 1;
		msg.m_diamondCharged = diamondCharged;
		SendMsgToLogic(msg, user->m_logicID);
	}
}

void Work::initGiftInfo()
{
	m_giftInfo.clear();
	//DbTool tool;
	MYSQL* m = m_dbsession.GetMysql();
	
	if (m == NULL)
	{
		LLOG_ERROR("Work::initGiftInfo mysql null");
		return;
	}

	std::stringstream ss;	
	ss << "SELECT Id,exchange_coupons FROM gift";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::initGiftInfo sql error %s", mysql_error(m));
		return ;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	while (row)
	{
		gift_info x;
		x.m_giftId = atoi(*row++);
		x.m_couponsNeedCount = atoi(*row++);
		m_giftInfo[x.m_giftId] = x;
		row = mysql_fetch_row(res);
	}
// 	for(auto x=m_giftInfo.begin();x!=m_giftInfo.end();x++)
// 	{
// 		std::cout <<"get gift" << x->second.m_giftId << ","<< x->second.m_couponsNeedCount <<std::endl;
// 	}
	mysql_free_result(res);
}


void Work::HanderLogicManagerCouponsInfo(Lint serverID)
{
	LMsgCE2LMGUserGiftInfo info_message;
	initGiftInfo();
	info_message.m_gift_count = m_giftInfo.size();
	for(auto x = m_giftInfo.begin();x!=m_giftInfo.end();x++)
	{
		gift_info info;
		info.m_couponsNeedCount = x->second.m_couponsNeedCount;
		info.m_giftId = x->second.m_giftId;
		info_message.m_gift_info.push_back(info);
	}
	if(serverID!=0)
	{
		auto logic = m_logicClient.find(serverID);
		if(logic!= m_logicClient.end())
		{
			logic->second->Send(info_message.GetSendBuff());
		}
	//	std::cout << "send gift message to server:" << serverID<< std::endl;
	}
	else
	{
		for(auto x = m_logicClient.begin();x!=m_logicClient.end();x++)
		{
			x->second->Send(info_message.GetSendBuff());
		}
	//	std::cout << "boast gift message to server"<< std::endl;
	}
	
}

void Work::HanderLogicManagerCouponsExchangeInfo(LMsgLMG2CEUserCouponsExchangeInfo* msg)
{
	if(!msg)
	{
		LLOG_ERROR("Work::HanderLogicManagerCouponsInfo msg=NULL");
		return;
	}
	DUser* user =  gUserManager.GetUserByOpenId(msg->m_unionId);
	if(!user)
	{
		LLOG_ERROR("Work::HanderLogicManagerCouponsInfo user is not exist");
		return;
	}
	if(m_giftInfo.count(msg->m_giftId)>0)
	{
		//user->m_usert.m_couponNum-=m_giftInfo[msg->m_giftId].m_couponsNeedCount;
		//gUserManager.SaveUser(user);
		gUserManager.UpdateUserCoupon(msg->m_unionId, -m_giftInfo[msg->m_giftId].m_couponsNeedCount, CouponChangedReason::ExchangeGift);

		std::stringstream ss;	
		ss << "insert into gift_exchange_record(UserId, GiftId, GiftNeedCoupons, State,Time,PhoneNumber) values('";
		ss << user->m_usert.m_id << "','";
		ss << msg->m_giftId << "','";
		ss << m_giftInfo[msg->m_giftId].m_couponsNeedCount << "','";
		ss << 1 << "','";
		ss << gWork.GetCurTime().Secs() << "','";
		ss << msg->m_phoneNum.c_str() << "')";

		Lstring* sql = new Lstring(ss.str());
		//LLOG_DEBUG("Work::HanderUserWechatLogin sql =%s", ss.str().c_str());
		gDbServer.Push(move(ss.str()), user->m_usert.m_id);
	}
	else{
		LLOG_ERROR("Work::HanderLogicManagerCouponsExchangeInfo gift not exist");
	}
}

void Work::HanderLogicManagerCouponsExchangeRecord(LMsgLMG2CEUserCouponsExchangeRecord* msg)
{
	if(!msg)
	{
		return;
	}
	DUser* user =  gUserManager.GetUserByOpenId(msg->m_unionId);
	if(!user)
	{
		LLOG_ERROR("Work::HanderLogicManagerCouponsInfo user is not exist");
		return;
	}

	MYSQL* m = m_dbsession.GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("Work::HanderLogicManagerCouponsInfo mysql null");
		return;
	}

	std::stringstream ss;	
	ss << "SELECT * FROM gift_exchange_record where UserId =" << user->m_usert.m_id << " order by Time desc ";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::HanderLogicManagerCouponsInfo sql error %s", mysql_error(m));
		return ;
	}

	MYSQL_RES* res = mysql_store_result(m);
	MYSQL_ROW row = mysql_fetch_row(res);
	LMsgCE2LMGUserCouponsExchangeRecord record;
	record.m_userId = user->m_usert.m_id;
	while (row)
	{
		gift_exchange_record x;
		row++;
		row++;
		x.m_giftId = atoi(*row++);
		x.m_coupons_need = atoi(*row++);
		x.m_state = atoi(*row++);
		x.m_time = atoi(*row++);
		x.m_phoneNum = *row++;
		record.m_record.push_back(x);
		if(record.m_record.size()==10)
		{
			break;
		}
		row = mysql_fetch_row(res);
	}
	record.m_record_count = record.m_record.size();
	mysql_free_result(res);
	SendMsgToLogic(record,msg->m_logicId);
}

void Work::HanderLogicManagerCouponChanged(LMsgLMG2CeCouponChanged * msg)
{
	gUserManager.UpdateUserCoupon(msg->m_unionId, msg->m_count, msg->m_changedReason);
}

void Work::HanderLogicManagerOfflineUserCardChaged(LMsgLMG2CeOfflineUserCardChanged* msg)
{
	if(msg == NULL)
		return;
	LLOG_DEBUG("Work::HanderLogicManagerOfflineUserCardChaged, unionId = %s, cardNum = %d", msg->m_unionId.c_str(), msg->m_count);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_unionId);
	if(user != NULL)
	{
		user->m_usert.m_numOfCard2s += msg->m_count;
		gUserManager.SaveUser(user);

		LMsgL2CeSaveCardInfo saveCardInfo;
		saveCardInfo.m_charge.m_time = GetCurTime().Secs();
		saveCardInfo.m_charge.m_cardType = msg->m_cardType;
		saveCardInfo.m_charge.m_cardNum = msg->m_count;
		saveCardInfo.m_charge.m_type = msg->m_operType;
		saveCardInfo.m_charge.m_userId = user->m_usert.m_id;
		saveCardInfo.m_charge.m_admin = msg->m_admin;
		saveCardInfo.m_charge.m_iTotalCardNum = user->m_usert.m_numOfCard2s;
		saveCardInfo.m_charge.m_teamId = msg->m_teamId;
		HanderLogicSaveCardInfo(&saveCardInfo);

		// 如果用户上线了，需要发送给logicmanager，防止数据不一致;

		if(user->m_logicID != 0)
		{
			LMsgCe2LGMCharge gmcharge;			// 消息的名字需要改;
			gmcharge.m_cardType = msg->m_cardType;
			gmcharge.m_cardCount = msg->m_count;
			gmcharge.m_oper = msg->m_operType;
			gmcharge.m_userid = user->m_usert.m_id;
			gmcharge.m_admin = msg->m_admin;
			SendMsgToLogic(gmcharge, user->m_logicID);
		}
	}
	else
		LLOG_ERROR("Work::HanderLogicManagerOfflineUserCardChaged: user %s does not exist.", msg->m_unionId.c_str());
}

void Work::HanderLogicManagerCardExchange(LMsgLMG2CeCardExchange* msg)
{
	if(msg == NULL)
		return;
	LLOG_DEBUG("Work::HanderLogicManagerCardExchange, id = %s, oper = %d,exchangeType=%d,cardNum = %d", msg->m_unionId.c_str(),msg->m_oper,msg->m_exchangeType,msg->m_cardNum);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_unionId);
	//完全在线状态，才能生效
	if(user && (user->m_usert.m_useTeamId != 0) && (user->m_usert.m_useTeamId == user->m_usert.m_onUseTeamId))
	{
		switch(msg->m_exchangeType)
		{
		case 0:
			{
				if(msg->m_cardType==CARD_TYPE_8)
				{
					user->m_usert.m_numOfCard2s-=msg->m_cardNum;
				}
				else if(msg->m_cardType==CARD_TYPE_16)
				{
					user->m_usert.m_numOfCard2s-=msg->m_cardNum*2;
				}
				break;
			}

		case 1:
			if(msg->m_cardType==CARD_TYPE_8)
			{
				user->m_usert.m_numOfCard2s+=msg->m_cardNum;
			}
			else if(msg->m_cardType==CARD_TYPE_16)
			{
				user->m_usert.m_numOfCard2s+=msg->m_cardNum*2;
			}
			break;
		default:
			LLOG_ERROR("Work::HanderLogicManagerCardExchange error msg exchangetype");
			return;
		}
		if(msg->m_oper == CARDS_OPER_TYPE_INIT)
		{
			user->m_usert.m_new=1;
		}
		gUserManager.SaveUser(user);


		LMsgL2CeSaveCardInfo charge_msg;
		LCharge& charge = charge_msg.m_charge;
		charge.m_admin = "admin";
		charge.m_userId = user->m_usert.m_id;
		charge.m_cardType = msg->m_cardType;
		charge.m_cardNum = msg->m_cardNum;
		charge.m_type = msg->m_oper;
		charge.m_time = gWork.GetCurTime().Secs();
		charge.m_iTotalCardNum = user->m_usert.m_numOfCard2s;
		charge.m_teamId = user->m_usert.m_useTeamId;
		HanderLogicSaveCardInfo(&charge_msg);
	}
	else
	{
		if (user)
		{
			LLOG_ERROR("Work::HanderLogicManagerCardExchange error m_useTeamId %d m_onUseTeamId %d", user->m_usert.m_useTeamId, user->m_usert.m_onUseTeamId);
		}
		else
		{
			LLOG_ERROR("Work::HanderLogicManagerCardExchange error no user");
		}
	}
}

void Work::HanderLogicManagerUserPlayCount(LMsgLMG2CeSaveUserPlayCount* msg)
{
	if(msg == NULL)
		return;
	DUser* user = gUserManager.GetUserByOpenId(msg->m_unionid);
	if(user)
	{
		user->m_usert.m_totalplaynum = msg->m_playCount;
		gUserManager.SaveUserExcepteSomeInfo(user);
	}
}

void Work::HanderLogicManagerGoldChange(LMsgLMG2CeGoldChange* msg)
{
	if(msg == NULL)
		return;
	LLOG_DEBUG("Work::HanderLogicManagerGold, id = %s, changenum = %d,changereason=%d", msg->m_unionId.c_str(), msg->m_GoldNum, msg->m_oper);
	DUser* user = gUserManager.GetUserByOpenId(msg->m_unionId);
	if(user)
	{
		switch(msg->m_oper)
		{
		case SYSTEMFIRSTGIVE:
			{
				user->m_usert.m_Gold = msg->m_GoldNum;
				user->m_usert.m_NewGolduser = 1;
				break;
			}
		case GIVEPERDAY:
			user->m_usert.m_Gold += msg->m_GoldNum;
			user->m_usert.m_LastGiftTime = msg->m_lastTime;
			user->m_usert.m_RemainingGifts = msg->m_RemainingGifts;
			break;
		case GAMERESULT:
		case BETUSED:
		case BETWIN:
		case GOLDCOSUME:
		case CARDCHANGE2GOLD:
			user->m_usert.m_Gold += msg->m_GoldNum;
			break;
		default:
			LLOG_ERROR("Work::HanderLogicManagerGoldChange error msg changetype");
			return;
		}
		gUserManager.SaveUserGold(user);
	}
}

void Work::HanderKeepAlive(LMsgS2SKeepAlive* msg)
{
	if (msg == NULL || !msg->m_sp)
		return;
	//LLOG_DEBUG("KeepAlive from %s:%d received.", msg->m_sp->GetRemoteIp().c_str(), msg->m_sp->GetRemotePort());

	LMsgS2SKeepAliveAck ack;
	msg->m_sp->Send(ack.GetSendBuff());
}

void Work::HanderGMCharge_online(std::map<Lstring, Lstring>& param, LSocketPtr sp, DUser* user, Ldouble nShift, Lint nReason)
{
	if (nShift > 0)
	{
		user->m_usert.m_numOfCard2s += nShift;
		user->m_usert.m_totalbuynum += nShift;
	}
	else
	{
		if (user->m_usert.m_numOfCard2s >= (-nShift))
		{
			user->m_usert.m_numOfCard2s += nShift;
		}
		else
		{
			nShift = user->m_usert.m_numOfCard2s;
			user->m_usert.m_numOfCard2s = 0.0f;
		}
	}

	if (user->m_logicID)//这里在logic里面有备份_
	{
		LMsgCe2LGMCharge gmcharge;
		gmcharge.m_cardType = CARD_TYPE_8;
		gmcharge.m_cardCount = nShift;
		gmcharge.m_oper = nReason;
		gmcharge.m_userid = user->m_usert.m_id;
		gmcharge.m_admin = param["admin"];
		SendMsgToLogic(gmcharge, user->m_logicID);
	}

	gUserManager.SaveUser(user);
	SaveRoomCardChangeLog2DB__(user->m_usert.m_id, nReason, nShift, user->m_usert.m_numOfCard2s, param["admin"], user->m_usert.m_onUseTeamId);
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}


void Work::HanderGMCharge_onoff(std::map<Lstring, Lstring>& param, LSocketPtr sp, Lint nUserId, Lint nTeamId, Ldouble nShift, Lint nReason, Lint appId)
{
	Lint numOfCard1s = 0;
	Ldouble numOfCard2s = 0;
	Lint numOfCard3s = 0;
	Ldouble totalbuynum = 0;
	Lint totalplaynum = 0;
	if (!gUserManager.GetTeamMember(nUserId, nTeamId, numOfCard1s, numOfCard2s, numOfCard3s, totalbuynum, totalplaynum))
	{
		LLOG_ERROR("HanderGMCharge_onoff error GetTeamMember %d %d", nUserId, nTeamId);
		SendRet("{\"errorCode\":6,\"errorMsg\":\"req param error\"}", sp);
		return;
	}

	if (nShift > 0)
	{
		numOfCard2s += nShift;
		totalbuynum += nShift;
	}
	else
	{
		if (numOfCard2s >= (-nShift))
		{
			numOfCard2s += nShift;
		}
		else
		{
			nShift = numOfCard2s;
			numOfCard2s = 0.0f;
		}
	}

	gUserManager.UpdateTeamMember(nUserId, nTeamId, numOfCard1s, numOfCard2s, numOfCard3s, totalbuynum, totalplaynum);
	SaveRoomCardChangeLog2DB__(nUserId, nReason, nShift, numOfCard2s, param["admin"], nTeamId);
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}


void Work::SaveRoomCardChangeLog2DB__(int nDBID, int nReason, Ldouble nShift, int nResulst, const std::string& Admin, Lint nTeamID)
{
	int nSeconds = GetCurTime().Secs();
	LTime time;
	time.SetSecs(nSeconds);
	std::stringstream ss;
	ss << "INSERT INTO charge (Time,CardType,CardNum,Type,UserId,Admin, TotalCardNum, TeamId) VALUES (";
	ss << "'" << nSeconds << "',";
	ss << "'" << 2 << "',";
	ss << "'" << std::to_string(nShift) << "',";
	ss << "'" << nReason << "',";
	ss << "'" << nDBID << "',";
	ss << "'" << Admin << "',";
	ss << "'" << nResulst << "',";
	ss << "'" << nTeamID << "')";

	LLOG_DEBUG("Work::HanderLogincSaveCardInfo save %s", ss.str().c_str());

	gDbServer.Push(move(ss.str()), nDBID);
}

void Work::HanderGMCreateAccount(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderGMCreateAccount %s:%s", param["openId"].c_str(), param["password"].c_str());

	Lstring uuid = param["openId"];
	Lstring password = param["password"];
	Lint nAppId = atoi(param["nAppId"].c_str());
	Lint severID = atoi(param["severID"].c_str());
	Lint sex = atoi(param["sex"].c_str());
	Lstring	imageUrl = param["imageUrl"];
	Lstring	nikename = param["nikename"];
	Lstring openId = uuid;

	/*if (!gConfig.findApp(nAppId))
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req nAppId param error\"}", sp);
		LLOG_ERROR("Unknown App,AppId = %d!", nAppId);
		return;
	}*/

	if (uuid.length() <= 0 || openId.length() <= 0)
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req openId or password param error\"}", sp);
		LLOG_ERROR("Unknown uuid = %s openId %s", uuid.c_str(), openId.c_str());
		return;
	}

	auto itor = m_logicClient.find(severID);
	if (itor == m_logicClient.end())
	{
		SendRet("{\"errorCode\":1,\"errorMsg\":\"req severID param error\"}", sp);
		LLOG_ERROR("Work::HanderGMCreateAccount logicid not exiext %d", severID);
		return;
	}

	DUser* user = gUserManager.GetUserByOpenId(uuid);
	if (user)
	{
		SendRet("{\"errorCode\":8,\"errorMsg\":\"req uuid param repeart\"}", sp);
		LLOG_ERROR("Work::HanderGMCreateAccount have user %s", uuid.c_str());
		return;
	}

	//处理性别值异常
	if (sex < 1 || sex > 2)
	{
		sex = 2;
	}

	//昵称中的单引号处理
	for (Lsize i = 0; i < nikename.size(); ++i)
	{
		if (nikename[i] == '\'')
			nikename[i] = ' ';
	}

	user = new DUser();
	user->m_usert.m_openid = openId;
	user->m_usert.m_unioid = uuid;
	user->m_usert.m_nike = nikename;
	user->m_usert.m_sex = sex;
	user->m_usert.m_headImageUrl = imageUrl;
	user->m_usert.m_regTime = gWork.GetCurTime().Secs();
	user->m_usert.m_lastLoginTime = gWork.GetCurTime().Secs();
	//user->m_usert.m_nAppId = nAppId;
	user->m_usert.m_provice = password;
	if (!gUserManager.CreateUser(user))
	{
		SendRet("{\"errorCode\":4,\"errorMsg\":\"create error\"}", sp);
		LLOG_ERROR("Work::HanderGMCreateAccount create user error %s", uuid.c_str());
		return;
	}
	gUserManager.AddUser(user);
	std::stringstream ss;
	ss << "{\"errorCode\":0,\"errorMsg\":\"success\",\"data\":\"";
	ss << user->m_usert.m_id << "\"}";
	SendRet(ss.str(), sp);//成功
}

void Work::HanderGMUpdateAccount(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	LLOG_ERROR("Work::HanderGMUpdateAccount %s:%s", param["openId"].c_str(), param["password"].c_str());

	Lstring uuid = param["openId"];
	Lstring password = param["password"];
	Lint nAppId = atoi(param["nAppId"].c_str());

	auto itor = m_logicClient.find(nAppId);
	if (itor == m_logicClient.end())
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req nAppId param error\"}", sp);
		LLOG_ERROR("Unknown App,AppId = %d!", nAppId);
		return;
	}
	/*if (!gConfig.findApp(nAppId))
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req nAppId param error\"}", sp);
		LLOG_ERROR("Unknown App,AppId = %d!", nAppId);
		return;
	}*/

	if (uuid.length() <= 0 || password.length() <= 0)
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req openId or password param error\"}", sp);
		LLOG_ERROR("Unknown uuid = %s openId %s", uuid.c_str(), password.c_str());
		return;
	}

	DUser* user = gUserManager.GetUserByOpenId(uuid);
	if (!user)
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req uuid not find\"}", sp);
		LLOG_ERROR("Work::HanderGMUpdateAccount have user %s", uuid.c_str());
		return;
	}

	user->m_usert.m_provice = password;
	gUserManager.SaveUser(user);
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}


void Work::HanderGMUpdateAccountInfo(std::map<Lstring, Lstring>& param, LSocketPtr sp)
{
	Lstring uuid = param["openId"];
	Lint nAppId = atoi(param["nAppId"].c_str());
	Lint sex = atoi(param["sex"].c_str());
	Lstring	imageUrl = param["imageUrl"];
	Lstring	nikename = param["nikename"];

	LLOG_ERROR("Work::HanderGMUpdateAccountInfo %s:%d %d %s %s", uuid.c_str(), nAppId, sex, imageUrl.c_str(), nikename.c_str());

	auto itor = m_logicClient.find(nAppId);
	if (itor == m_logicClient.end())
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req nAppId param error\"}", sp);
		LLOG_ERROR("Unknown App,AppId = %d!", nAppId);
		return;
	}
	/*if (!gConfig.findApp(nAppId))
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req nAppId param error\"}", sp);
		LLOG_ERROR("Unknown App,AppId = %d!", nAppId);
		return;
	}*/

	if (uuid.length() <= 0)
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req openId or password param error\"}", sp);
		LLOG_ERROR("HanderGMUpdateAccountInfo Unknown uuid = %s", uuid.c_str());
		return;
	}

	DUser* user = gUserManager.GetUserByOpenId(uuid);
	if (!user)
	{
		SendRet("{\"errorCode\":7,\"errorMsg\":\"req uuid not find\"}", sp);
		LLOG_ERROR("Work::HanderGMUpdateAccountInfo have user %s", uuid.c_str());
		return;
	}

	user->m_usert.m_sex = sex;
	user->m_usert.m_headImageUrl = imageUrl;
	user->m_usert.m_nike = nikename;
	gUserManager.SaveUser(user);
	SendRet("{\"errorCode\":0,\"errorMsg\":\"success\"}", sp);//成功
}


