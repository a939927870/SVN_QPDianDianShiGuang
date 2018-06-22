#include "Work.h"
#include "LLog.h"
#include "Config.h"
#include "OutsideNet.h"
#include "LMsgS2S.h"
#include "GateUser.h"
#include "GarbageCollectionService.h"
#include "BaseCommonFunc.h"

//初始化;
bool Work::Init()
{
	//设置log文件;
	LLog::Instance().SetFileName("GateServer");
	
	//初始化配置文件;
	if(!gConfig.Init())
	{
		LLOG_ERROR("gConfig.Init error");
		return false;
	}

	LLog::Instance().SetLogLevel(gConfig.GetLogLevel());

	LMsgFactory::Instance().Init();

	if (!gOutsideNet.Init())
	{
		LLOG_ERROR("gOutsideNet.Init error");
		return false;
	}

	if (!gGateUserManager.Init())
	{
		LLOG_ERROR("gGateUserManager.Init error");
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

	ConnectToLogicManager();

	gOutsideNet.Start();

	LRunnable::Start();
}

//等待;
void Work::Join()
{
	gOutsideNet.Join();

	//等待逻辑线程终止
	LRunnable::Join();
}

//停止;
void Work::Stop()
{
	m_spMap.clear();

	m_logicInfo.clear();

	gOutsideNet.Stop();

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
		m_1SceTick = cur.MSecs();
	}
	//5秒循环一次
	if (cur.MSecs() - m_5SceTick > 5000)
	{
		m_5SceTick = cur.MSecs();
	}
	
	//30秒循环一次;
	if (cur.MSecs() - m_30SceTick > 30*1000)
	{
		m_30SceTick = cur.MSecs();

		std::map<LSocketPtr, Lint>::iterator it = m_spMap.begin();
		for (; it != m_spMap.end();)
		{
			if (GetCurTime().Secs() - it->second > 30)
			{
				LMsgKick kick;
				kick.m_sp = it->first;
				it->first->Stop();
				m_spMap.erase(it++);

				HanderUserKick(&kick);
			}
			else
			{
				++it;
			}
		}
	}

	//5分钟循环一次;
	if (cur.MSecs() - m_600SceTick > 5*60 * 1000)
	{
		m_600SceTick = cur.MSecs();
	}

	if (m_logicManager)
		m_logicManager->OnTick(cur);

	for (auto iter = m_logicInfo.begin(); iter != m_logicInfo.end(); iter++)
	{
		if (iter->second.m_client)
			iter->second.m_client->OnTick(cur);
	}

	for (auto iter = m_TeamServerInfoMap.begin(); iter != m_TeamServerInfoMap.end(); iter++)
	{
		if (iter->second.m_TeamServerTcpClient)
			iter->second.m_TeamServerTcpClient->OnTick(cur);
	}
}

LTime& Work::GetCurTime()
{
	return m_tickTimer;
}

void Work::HanderMsg(LMsg* msg)
{
	//LLOG_ERROR("Work::HanderMsg received messageID %d", msg->m_msgId);
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
	case MSG_LMG_2_G_SYNC_LOGIC:
		HanderLogicInfo((LMsgLMG2GateLogicInfo*) msg);
		break;
	case MSG_LMG_2_G_HORSE_INFO:
		HanderHorseInfo((LMsgLMG2GHorseInfo*) msg);
		break;
	case  MSG_L_2_G_MODIFY_USER_STATE:
		HanderModifyUserState((LMsgL2GModifyUserState*) msg);
		break;
	case MSG_L_2_G_BREATH:
		HanderLogicHeartBeat((LMsgL2GHEARTBEAT_BACK*) msg);
		break;
	case MSG_LMG_2_G_BREATH:
		HanderLogicManagerHeartBeat((LMsgLMG2GHEARTBEAT_BACK*) msg);
		break;
	case MSG_T_2_G_USER_MSG:
	{
		LMsgT2GUserMsg* t2gMsg = (LMsgT2GUserMsg*)msg;
		GateUser* user = gGateUserManager.GetUserById(t2gMsg->m_id);
		if (user)
		{
			user->m_sp->Send(t2gMsg->m_dataBuff);
		}
		else
		{
			LLOG_DEBUG("ERROR: Work::HanderMsg MSG_T_2_G_USER_MSG user not exiest %d", t2gMsg->m_id);
		}
	}
	break;
	case MSG_LMG_2_G_TEAMSERVER:
		HanderTeamServerInfo((LMsgLMG2G_TeamServer*)msg);
		break;
	case MSG_G_2_L_USER_MSG:
		{
			LLOG_DEBUG("Work::HanderMsg: MSG_G_2_L_USER_MSG");
			GateUser* user = gGateUserManager.GetUserBySp(msg->m_sp);
			if (user)
			{
				//区分玩家状态，将消息发送至Logic;
				if (user->getUserState() == LGU_STATE_DESK && user->getUserLogicID() > 0)
				{
					//发送消息到Logic
					LMsgG2LUserMsg* userMsg = (LMsgG2LUserMsg*)msg;
					userMsg->m_userGateId = user->m_id;
					LogicInfo* logic = GetLogicInfoById(user->getUserLogicID());
					if (logic)
					{
						if (logic->m_client)  //
						{
							logic->m_client->Send(userMsg->GetSendBuff());
						}	
						if (MSG_C_2_S_ADD_ROOM == userMsg->m_userMsgId)   //22返回房间消息，需要LM处理，因金币场引起，不好改；暂时放这里;
						{
							if (m_logicManager)
							{
								m_logicManager->Send(userMsg->GetSendBuff());
							}
						}
					}
					else
					{
						//当前逻辑不存在，踢下该用户，让他重连;
						LLOG_DEBUG("logic is NULL ,userleave gateid:%d  logicid:%d   ip:%s", user->m_id, user->m_logicID, user->m_sp->GetRemoteIp().c_str());
						user->m_sp->Stop();
						DelSp(user->m_sp);
						gGateUserManager.DelUser(user);
						return;
					}
				}
				else
				{
					//发送消息到LogicManager;
					LMsgG2LUserMsg* userMsg = (LMsgG2LUserMsg*)msg;
					userMsg->m_userGateId = user->m_id;
					if (m_logicManager)
					{
						m_logicManager->Send(userMsg->GetSendBuff());
					}
				}
				user->m_login += 1;
				LLOG_DEBUG("HanderMsg user gateid:%d  logicid:%d   ip:%s", user->m_id, user->m_logicID, user->m_sp->GetRemoteIp().c_str());
				AddSp(msg->m_sp);
			}
			else
			{
				LMsgG2LUserMsg* userMsg = (LMsgG2LUserMsg*)msg;
				LLOG_ERROR("ERROR: Work::HanderMsg MSG_G_2_L_USER_MSG user not exiest %d", userMsg->m_userMsgId);
			}
		}
		break;
	case MSG_G_2_T_USER_MSG:
	{
		LMsgG2TUserMsg* userMsg = (LMsgG2TUserMsg*)msg;
		GateUser* user = gGateUserManager.GetUserBySp(msg->m_sp);
		if (user)
		{
			userMsg->m_userGateId = user->m_id;
			userMsg->m_gateID = gConfig.GetId();
		}
		SendMsgToTeamServer(*userMsg);
	}
	break;
	case MSG_L_2_G_USER_MSG:
		{
			  LMsgL2GUserMsg* lmsg = (LMsgL2GUserMsg*)msg;
			  GateUser* user = gGateUserManager.GetUserById(lmsg->m_id);
			  if (user)
			  {
				  user->m_sp->Send(lmsg->m_dataBuff);
			  }
			  else
			  {
				  LLOG_DEBUG("ERROR: Work::HanderMsg MSG_L_2_G_USER_MSG user not exiest %d", lmsg->m_id);
			  }
		}
		break;
	case MSG_L_2_G_USER_OUT_MSG:
		{
			LMsgL2GUserOutMsg*lmsg = (LMsgL2GUserOutMsg*)msg;
			GateUser* user = gGateUserManager.GetUserById(lmsg->m_id);
			if (user)
			{
				LLOG_DEBUG("MSG_L_2_G_USER_OUT_MSG: user leave from gate  gateid:%d  logicid:%d   ip:%s", user->m_id, user->m_logicID, user->m_sp->GetRemoteIp().c_str());
				user->m_sp->Stop();
				DelSp(user->m_sp);
				gGateUserManager.DelUser(user);
			}
			else
			{
				LLOG_DEBUG("ERROR: Work::HanderMsg MSG_L_2_G_USER_OUT_MSG user not exiest %d", lmsg->m_id);
			}
		}
		break;
	case MSG_S_2_S_KEEP_ALIVE_ACK:
		HanderKeepAliveAck((LMsgS2SKeepAliveAck*)msg);
		break;
	default:
		break;
	}
}


void Work::HanderTeamServerInfo(LMsgLMG2G_TeamServer* msg)
{
	if (NULL == msg)
	{
		return;
	}
	if (msg->m_TeamServerID != 0)
	{
		TeamServerInfo info;
		info.m_nTeamServerID = msg->m_TeamServerID;
		info.m_sTeamServerPort = msg->nTeamServerPort;
		info.m_strTeamServerIP = msg->strTeamServerIp;
#ifdef _DEBUG
		KeepAliveSetting setting = { true, 150000, 5000 };
#else // _DEBUG
		KeepAliveSetting setting = { true, 30000, 5000 };
#endif // _DEBUG
		MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
		{
			LMsg* msg = ::ParseInsideNetPacket(recv, s);
			if (msg)
				this->Push(msg);
		};
		m_TeamServerInfoMap[msg->m_TeamServerID] = info;
		m_TeamServerInfoMap[msg->m_TeamServerID].m_TeamServerTcpClient.reset(new TCPClient(gOutsideNet, msg->strTeamServerIp, msg->nTeamServerPort, setting, receiver));
		bool succeed = m_TeamServerInfoMap[msg->m_TeamServerID].m_TeamServerTcpClient->Connect();
		if (!succeed)
			LLOG_ERROR("Failed to connect with TeamServer.");
		else
			LLOG_ERROR("Succeed to connect with TeamServer.");
	}
}

TCPClientPtr Work::GetTeamServerTCPClientBySP(LSocketPtr sp)
{
	TCPClientPtr client;
	for (auto itSp = m_TeamServerInfoMap.begin(); itSp != m_TeamServerInfoMap.end(); itSp++)
	{
		if (itSp->second.m_TeamServerTcpClient == sp)
		{
			client = itSp->second.m_TeamServerTcpClient;
			break;
		}
	}
	return client;
}

bool Work::IsTeamServerSP(LSocketPtr sp)
{
	for (auto itSp = m_TeamServerInfoMap.begin(); itSp != m_TeamServerInfoMap.end(); itSp++)
	{
		if (sp == itSp->second.m_TeamServerTcpClient)
		{
			return true;
		}
	}
	return false;
}

void Work::SendMsgToTeamServer(LMsg& msg)
{
	for (auto itMatch = m_TeamServerInfoMap.begin(); itMatch != m_TeamServerInfoMap.end(); ++itMatch)
	{
		if (itMatch->second.m_TeamServerTcpClient)
		{
			itMatch->second.m_TeamServerTcpClient->Send(msg.GetSendBuff());
		}
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
	// logic断线不作处理，等待重连;
	if(m_logicManager == msg->m_sp || _isLogicSP(msg->m_sp))
	{	return;	}

	//玩家离线;
	GateUser* user = gGateUserManager.GetUserBySp(msg->m_sp);
	if (user)
	{
		if(user->m_login)
			SendToLogicUserOut(user);

		LLOG_DEBUG("kick user gateid:%d  logicid:%d   ip:%s", user->m_id, user->m_logicID, user->m_sp->GetRemoteIp().c_str());
		gGateUserManager.DelUser(user);
	}

	DelSp(msg->m_sp);
}

void Work::HanderClientIn(LMsgIn* msg)
{
	if (msg->m_sp == m_logicManager)
	{
		LLOG_ERROR("Connecting logicmanager on %s:%d succeed", m_logicManager->GetServerIP().c_str(), m_logicManager->GetServerPort());
		SendGateLoginToLM();
		return;
	}

	auto logicClient = GetLogicClientBySP(msg->m_sp);
	if (logicClient)
	{
		LLOG_ERROR("Connecting logic on %s:%d succeed", logicClient->GetServerIP().c_str(), logicClient->GetServerPort());
		SendGateLoginToLogic(logicClient);
		return;
	}

	auto TeamTCPClient = GetTeamServerTCPClientBySP(msg->m_sp);
	if (TeamTCPClient)
	{
		LLOG_ERROR("Connecting TeamServer on %s:%d succeed", TeamTCPClient->GetServerIP().c_str(), TeamTCPClient->GetServerPort());
		SendGateLoginToTeamServer(TeamTCPClient);
		return;
	}

	AddSp(msg->m_sp);

	GateUser* info = gGateUserManager.GetUserBySp(msg->m_sp);
	if (info != NULL)
	{
		LLOG_DEBUG("Work::HanderClientIn already in %d",info->m_id);
		return;
	}

	//这里添加一个新连接进来
	gGateUserManager.CreateUser(msg->m_sp);
}

void Work::HanderHttp(LMsgHttp* msg)
{	
}

void Work::ConnectToLogicManager()
{
	KeepAliveSetting setting = { true, 15000, 5000 };
	MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
	{
		LMsg* msg = ::ParseInsideNetPacket(recv, s);
		if (msg)
			this->Push(msg);
	};

	m_logicManager.reset(new TCPClient(gOutsideNet, gConfig.GetLogicManagerIp(), gConfig.GetLogicManagerPort(), setting, receiver));
	bool succeed = m_logicManager->Connect();
	if (!succeed)
		LLOG_ERROR("Failed to connect with logicmanager.");
}

void Work::SendToLogicUserOut(GateUser* user)
{
	LMsgG2LUserOutMsg send;
	send.m_userGateId = user->m_id;
	if (m_logicManager)
	{
		//通各LogicManager下线
		m_logicManager->Send(send.GetSendBuff());
	}
	if (user->getUserLogicID() > 0)
	{
		LogicInfo* logic = GetLogicInfoById(user->getUserLogicID());
		if (logic != NULL && logic->m_client)
		{
			//通知Logic下线
			logic->m_client->Send(send.GetSendBuff());
		}
	}
}

void Work::HanderLogicInfo(LMsgLMG2GateLogicInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (msg->m_ID <= 0)
	{
		LLOG_ERROR("Work::HanderLogicInfo %d", msg->m_ID);
		return;
	}

	std::map<int, LogicInfo> mapFromLM;
	for(int i = 0; i < msg->m_count; i++)
	{
		mapFromLM[msg->m_logic[i].m_id] = msg->m_logic[i];
	}

	//遍历哪些logic已经不在logicmanager上了;
	for(auto itLogic = m_logicInfo.begin(); itLogic != m_logicInfo.end();)
	{
		if(mapFromLM.find(itLogic->first) == mapFromLM.end()) //这个logic已经不在logicmananger上了;
		{
			LLOG_ERROR("Logic %d does not exist, so delete it", itLogic->first);
			if (itLogic->second.m_client)
			{
				itLogic->second.m_client->Disconnect();
			}		
			itLogic = m_logicInfo.erase(itLogic);
		}
		else //如果依然存在;
		{
			mapFromLM.erase(itLogic->first);
			itLogic++;
		}
	}

	//加入新进入的;
	for(auto itNew = mapFromLM.begin(); itNew != mapFromLM.end(); itNew++)
	{
		m_logicInfo[itNew->first] = itNew->second;
		KeepAliveSetting setting = { true, 15000, 5000 };
		MsgPackReceiver receiver = [this](LBuffPtr recv, LSocketPtr s)
		{
			LMsg* msg = ::ParseInsideNetPacket(recv, s);
			if (msg)
				this->Push(msg);
		};

		m_logicInfo[itNew->first].m_client.reset(new TCPClient(gOutsideNet, itNew->second.m_ip, itNew->second.m_port, setting, receiver));
		m_logicInfo[itNew->first].m_client->Connect();
	}
}

void Work::HanderHorseInfo(LMsgLMG2GHorseInfo* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LMsgS2CHorseInfo horse;
	horse.m_str = msg->m_str;
	SendMessageToAllUse(&horse);
}

void Work::HanderModifyUserState(LMsgL2GModifyUserState* msg)
{
	if (msg == NULL)
	{
		return;
	}

	GateUser* user = gGateUserManager.GetUserById(msg->m_gateid);
	if (user)
	{
		LLOG_DEBUG("Work::HanderModifyUserState ID:%d, State:%d, serverID: %d, userip:%s", user->m_id, msg->m_userstate, msg->m_logicID, user->m_sp->GetRemoteIp().c_str());
		user->setUserState(msg->m_userstate);
		user->setUserLogicID(msg->m_logicID);
	}
	else
	{
		LLOG_DEBUG("ERROR: Work::HanderModifyUserState Not Exist ID:%d, State:%d, serverID: %d", msg->m_gateid, msg->m_userstate, msg->m_logicID);
	}
}

void Work::SendMessageToAllUse(LMsg* msg)
{
	if (msg == NULL)
	{
		return;
	}
	auto it = m_spMap.begin();
	for (; it != m_spMap.end(); ++ it)
	{
		it->first->Send(msg->GetSendBuff());
	}
}

LogicInfo* Work::GetLogicInfoBySp(LSocketPtr sp)
{
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (sp == it->second.m_client)
			return &it->second;
	}
	return NULL;
}

LogicInfo* Work::GetLogicInfoById(Lint id)
{
	auto iter = m_logicInfo.find(id);
	if (iter != m_logicInfo.end())
		return &iter->second;

	return NULL;
}

bool Work::_findLogicSPAndDel(LSocketPtr sp)
{
	for(auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); itSp++)
	{
		if(itSp->second.m_sp == sp)
		{
			m_logicInfo.erase(itSp);
			return true;
		}
	}

	return false;
}

bool Work::_isLogicSP(LSocketPtr sp)
{
	for(auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); itSp++)
	{
		if(sp == itSp->second.m_client)
		{
			return true;
		}
	}
	return false;
}

void Work::SendHeartBeatToLogic()
{
	LLOG_DEBUG("HeartBeat: G_2_L");
	LMsgG2LHEARTBEAT msg;
	msg.m_gateID = gConfig.GetId();
	auto it = m_logicInfo.begin();
	for (; it != m_logicInfo.end(); ++it)
	{
		if (it->second.m_sp)
		{
			it->second.m_sp->Send(msg.GetSendBuff());
		}
		else
		{
			LLOG_ERROR("ERROR: SendHeartBeatToLogic logicID: %d", it->first);
			m_logicInfo.erase(m_logicInfo.find(it->first));
			return;
		}
	}
}

void Work::SendHeartBeatToLogicManager()
{
	LLOG_DEBUG("HeartBeat: G_2_LMG");
	LMsgG2LMGHEARTBEAT msg;
	msg.m_gateID = gConfig.GetId();
	if (m_logicManager)
	{
		m_logicManager->Send(msg.GetSendBuff());
	}
	else
	{
		LLOG_ERROR("ERROR: SendHeartBeatToLogicManager");
	}
}

void Work::HanderLogicHeartBeat(LMsgL2GHEARTBEAT_BACK* msg)
{
	LLOG_DEBUG("HeartBeatBack: L_2_G");
	if(!msg)
		return;
	//后续处理断线重连;

}

void Work::HanderLogicManagerHeartBeat(LMsgLMG2GHEARTBEAT_BACK* msg)
{
	LLOG_DEBUG("HeartBeatBack: LMG_2_G");
	if(!msg)
		return;
	//后续处理断线重连

}

void Work::SendGateLoginToLM()
{
	LMsgG2LMGLogin login;
	login.m_id = gConfig.GetId();
	login.m_ip = gConfig.GetOutsideIp();
	login.m_key = gConfig.GetLogicKey();
	login.m_port = gConfig.GetOutsidePort();

	login.m_ip2 = gConfig.GetOutsideIp2();
	login.m_port2 = gConfig.GetOutsidePort2();
	m_logicManager->Send(login.GetSendBuff());
	LLOG_ERROR("Login LogicManager succeed.");
}

void Work::SendGateLoginToLogic(TCPClientPtr logicClient)
{
	LMsgG2LLogin login;
	login.m_id = gConfig.GetId();
	login.m_ip = gConfig.GetOutsideIp();
	login.m_key = gConfig.GetLogicKey();
	login.m_port = gConfig.GetOutsidePort();
	logicClient->Send(login.GetSendBuff());
	LLOG_ERROR("Login logic %s:%d succeed", logicClient->GetServerIP().c_str(), logicClient->GetServerPort());
}

TCPClientPtr Work::GetLogicClientBySP(LSocketPtr sp)
{
	TCPClientPtr client;
	for (auto itSp = m_logicInfo.begin(); itSp != m_logicInfo.end(); itSp++)
	{
		if (itSp->second.m_client == sp)
		{
			client = itSp->second.m_client;
			break;
		}
	}
	return client;
}

void Work::HanderKeepAliveAck(LMsgS2SKeepAliveAck* msg)
{
	if (msg == NULL)
		return;

	if (m_logicManager == msg->m_sp)
		m_logicManager->OnKeepAliveAck();
	else if (TCPClientPtr logicClient = GetLogicClientBySP(msg->m_sp))
		logicClient->OnKeepAliveAck();
}

void Work::SendGateLoginToTeamServer(TCPClientPtr sp)
{
	LMsgG2TS_Login loginToTeamServer;
	loginToTeamServer.nGateId = gConfig.GetId();
	if (sp)
	{
		sp->Send(loginToTeamServer.GetSendBuff());
	}
	LLOG_ERROR("Login TeamServer succeed.");
}
