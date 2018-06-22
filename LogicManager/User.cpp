#include "User.h"
#include "Work.h"
#include "UserManager.h"
#include "Config.h"
#include "ActiveManager.h"
#include "DeskManager.h"
#include "RemoteLogThread.h"
#include <boost/algorithm/string/replace.hpp>
#include "LMsgCT.h"

const unsigned int g_ngiveGoldNewPlayer = 3000;
const unsigned int g_ngiveGoldPerDay = 1000;
const unsigned int g_nGiveGoleTimes = 2;

User::User(LUser data, Lint gateId, Lint gateUserId)
:m_userData(data), m_taskMgr(this), m_goldLogicID(0), m_bActiveDBResponse(false)
{
	m_gateId = gateId;
	m_gateUserId = gateUserId;
	m_online = true;
	m_userDeskID = 0;
	m_userState = 0;
	m_logicID = 0;
	m_Active.clear();
}
	
User::~User()
{
}

void User::Tick()
{

}

//获取玩家数据库里id;
Lint User::GetUserDataId()
{
	return m_userData.m_id;
}

//获取玩家唯一表示;
Llong	User::GetUserUnique()
{
	Llong id = ((Llong)GetUserGateId() << 32 | (Llong)GetUserGateUserId());
	return id;
}

void User::SetUserGateId(Lint gateId)
{
	m_gateId = gateId;
}

Lint User::GetUserGateId()
{
	return m_gateId;
}

void User::SetUserGateUserId(Lint gateUserId)
{
	m_gateUserId = gateUserId;
}

Lint User::GetUserGateUserId()
{
	return m_gateUserId;
}

bool User::GetOnline()
{
	return m_online;
}

void User::Login(Lint& inlogic)
{
	inlogic = 0;
	if (0 == m_userData.m_NewGolduser)  //首次赠送金币后置1;
	{
		SaveGoldChange(SYSTEMFIRSTGIVE, g_ngiveGoldNewPlayer);
		m_userData.m_Gold = g_ngiveGoldNewPlayer;
		m_userData.m_NewGolduser = 1;
		send2UserGoldChange(g_ngiveGoldNewPlayer, SYSTEMFIRSTGIVE, g_ngiveGoldNewPlayer);
	}

	m_userData.m_lastLoginTime = gWork.GetCurTime().Secs();
	m_online = true;
	if(getUserLogicID() > 0 || getUserGoldLogicID() > 0)
	{
		PK_GAME_ZONE gameZone = game_zone_roomcard;
		LogicInfo* logic = gWork.GetLogicInfoById(getUserLogicID());
		if (NULL == logic)  //没有房卡场则寻找金币场logic;
		{
			logic = gWork.GetLogicInfoById(getUserGoldLogicID());
			gameZone = game_zone_gold;
		}
		if (logic != NULL)
		{
			//玩家重新登陆到桌子上,如果重启的Logic会失去玩家信息，登录有问题;
			LMsgLMG2LLogin msg;
			msg.m_userid = m_userData.m_id;
			msg.m_gateId = GetUserGateId();
			msg.m_geteUserId = GetUserGateUserId();
			msg.m_gameZone = gameZone;
			msg.m_ip = getUserIPStr();
			msg.m_buyInfo = gWork.GetBuyInfo(m_userData.m_id);
			msg.m_hide = gWork.GetHide();
			logic->m_sp->Send(msg.GetSendBuff());
			inlogic = 1;
		}
		else
		{   //Logic已经关闭会运行到这里;
			SendItemInfo();
			//SendLoginInfo();
			//测试
			if (m_userData.m_onUseTeamId == 0)
				SendLoginInfo(0, m_userData.m_onUseTeamId, "");
		}
	}
	else
	{
		SendItemInfo();
		//SendLoginInfo();
		//测试
		if (m_userData.m_onUseTeamId == 0)
		SendLoginInfo(0, m_userData.m_onUseTeamId, "");
	}
	SendHorseInfo();
	SyncActiveOpen();	//登陆，同步客户端当前是否有活动;
	SendHuoDong();    //新版发送活动;

	UserIdInfo u;
	u.m_id = GetUserDataId();
	u.m_nike = m_userData.m_nike;
	u.m_unionId = m_userData.m_unioid;
	u.m_sex = m_userData.m_sex;
	u.m_headImageUrl = m_userData.m_headImageUrl;
	gUserManager.AddUserIdName(u);

	//Save();
	//任务系统
	m_taskMgr.LoadTasks();
	//十一活动
	SyncLoadSignCollectWin();
	
	Lstring l_nike = m_userData.m_nike;
	boost::algorithm::replace_all_copy(l_nike.begin(), m_userData.m_nike, "|", "-");
	RLOG("login", getUserIPStr()
		<< "|" << LTimeTool::GetLocalTimeInString()
		<< "|" << m_userData.m_id
		<< "|" << m_userData.m_numOfCard2s
		<< "|" << l_nike
		<< "|" << gConfig.GetServerID()
		<< "|" << gConfig.GetInsideIp());
}

void User::Logout()
{
	m_online = false;
	//m_gateId = 0;
	//m_gateUserId = 0;
	//Save();

	m_actMgr_signColl.SaveToDb(GetUserDataId());

	m_actMgr_signColl.ReSet();

	Lstring l_nike = m_userData.m_nike;
	boost::algorithm::replace_all_copy(l_nike.begin(), m_userData.m_nike, "|", "-");
	LTime lLastLoginTime;
	lLastLoginTime.SetSecs(m_userData.m_lastLoginTime);
	RLOG("logout", getUserIPStr()
		<< "|" << LTimeTool::GetLocalTimeInString()
		<< "|" << m_userData.m_id
		<< "|" << m_userData.m_numOfCard2s
		<< "|" << l_nike
		<< "|" << gConfig.GetServerID()
		<< "|" << gConfig.GetInsideIp()
		<< "|" << LTimeTool::GetLocalTimeInString(lLastLoginTime)
		);
}

void User::Send(LMsg& msg)
{
	GateInfo* info = gWork.GetGateInfoById(m_gateId);
	if (info)
	{
		LMsgL2GUserMsg send;
		send.m_id = m_gateUserId;
		send.m_dataBuff = msg.GetSendBuff();
		info->m_sp->Send(send.GetSendBuff());
	}
}

void User::SendLoginInfo(Lint errorCode, Lint onUseTeamId, Lstring onUseTeamName)
{
	LMsgS2CLogin send;
	send.m_id = m_userData.m_id;
	send.m_errorCode = errorCode;
	send.m_nike = m_userData.m_nike;
	send.m_exp = 0;
	send.m_sex = m_userData.m_sex;
	send.m_new = m_userData.m_new;
	send.m_state = getUserLogicID()> 0 ? 1 : 0;
	send.m_ip = m_ip;
	send.m_playerType = m_userData.m_playerType;
	send.m_face = m_userData.m_headImageUrl;
	send.m_buyInfo = gWork.GetBuyInfo(m_userData.m_id);
	send.m_hide = gWork.GetHide();
	send.m_gm = m_userData.m_gm;
	/*send.m_curHeadIconId = m_userData.m_curHeadIconId;
	send.m_phone = m_userData.m_phone;*/

	/*std::vector<Lint> gain_head_icon_vec = GetUserGainHeadIconId();
	send.m_gainHeadIconId = gain_head_icon_vec;*/
	send.m_nCurTime = gWork.GetCurTime().Secs();

	send.m_nTeamId = onUseTeamId;
	if (onUseTeamName.length())
	{
		send.m_sTeamName = onUseTeamName;
	}
	else
	{
		send.m_sTeamName = m_userData.m_teaminfo.strTeamName;
	}
	

	send.nState = m_userData.m_teaminfo.nState;					// 查询玩家在该工会中的状态 [2017-7-29 willing]
	send.m_isInDesk = m_userData.m_teaminfo.m_isInDesk;		//是否加入了房间
	send.teamInfo = m_userData.m_teaminfo.teamInfo;//公会本身的信息

	if (m_userData.m_new == 0)
	{
		const Lint nRewardCard = 0;
		m_userData.m_new = 1;
		send.m_card1 = 0;
		send.m_card2 = 0;
		send.m_card3 = 0;
		AddCardCount(CARD_TYPE_8, 0, CARDS_OPER_TYPE_INIT, "system");
		Lstring l_nike = m_userData.m_nike;
		boost::algorithm::replace_all_copy(l_nike.begin(), m_userData.m_nike, "|", "-");
		RLOG("reg", getUserIPStr()
			<< "|" << LTimeTool::GetLocalTimeInString()
			<< "|" << m_userData.m_id
			<< "|" << m_userData.m_numOfCard2s
			<< "|" << l_nike
			<< "|" << gConfig.GetServerID()
			<< "|" << gConfig.GetInsideIp());
	}
	else {
		send.m_card1 = 0;
		send.m_card2 = 0;
		send.m_card3 = 0;
	}
	send.m_gold = m_userData.m_Gold;
	Send(send);
}

void User::SendItemInfo()
{
	LMsgS2CItemInfo send;
	send.m_card1 = m_userData.m_numOfCard1s;
	send.m_card2 = m_userData.m_numOfCard2s;
	send.m_card3 = m_userData.m_numOfCard3s;
	send.m_gold  = m_userData.m_Gold;
	send.m_diamondNum = m_userData.m_diamondNum;
	Send(send);
}

void User::SendHorseInfo()
{
	LMsgS2CHorseInfo send;
	send.m_str = gWork.GetHorse();
	Send(send);
}

void User::S2CeUserLogin()
{
	LMsgL2CeUserServerLogin msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_openID = m_userData.m_unioid;
	msg.m_useTeamId = m_userData.m_onUseTeamId;
	LLOG_DEBUG("Work::HanderLogicUserLogin, userID: %s, ServerID %d", msg.m_openID.c_str(), msg.m_serverID);
	gWork.SendToCenter(msg);
}

void User::S2CeUserLogout()
{
	LMsgL2CeUserServerLogout msg;
	msg.m_serverID = gConfig.GetServerID();
	msg.m_openID = m_userData.m_unioid;
	LLOG_DEBUG("Work::HanderLogicUserLogot, userID: %s, ServerID %d", msg.m_openID.c_str(), msg.m_serverID);
	gWork.SendToCenter(msg);
}

void User::SetDeskID(Lint deskid)
{
	m_userDeskID = deskid;
}

Lint User::GetDeskID()
{
	return m_userDeskID;
}

void User::HanderMsg(LMsg* msg)
{
	switch (msg->m_msgId)
	{
	case MSG_C_2_S_CREATE_ROOM:
		HanderUserCreateDesk((LMsgC2SCreateDesk*)msg);
		break;
	case MSG_C_2_S_ADD_ROOM:
		HanderUserAddDesk((LMsgC2SAddDesk*)msg);
		break;
	case MSG_C_2_S_VIP_LOG:
		HanderUserVipLog((LMsgC2SVipLog*) msg);
		break;
	case MSG_C_2_S_VIDEO: 
		HanderUserVideo((LMsgC2SVideo*) msg);
		break;
	case MSG_C_2_S_HEART:
		HanderUserHeart((LMsgC2SHeart*) msg);
		break;
	case MSG_C_2_S_REQUEST_DRAW:
		HanderUserDraw((LMsgC2SRequestDraw*) msg);
		break;
	case MSG_C_2_S_WRITE_PHONE:
		HanderWritePhone((LMsgC2SWritePhone*) msg);
		break;
	case MSG_C_2_S_ACTIVE_LOG:
		HanderUserReqActive((LMsgC2SActiveLog*) msg);
		break;
	case MSG_S_2_C_ACTIVE_OPEN:
		RequestActiveInfo((LMsgC2SActiveInfoRequest*) msg);
		break;
		//玩家请求查看某shareid的分享录像;
	case MSG_C_2_S_SHAREVIDEO:
		HanderUserShareVideo((LMsgC2SShareVideo*) msg);
		break;
		//玩家请求分享某录像;
	case MSG_C_2_S_REQSHAREVIDEO:
		HanderUserReqShareVideo((LMsgC2SReqShareVideo*) msg);
		break;
		//玩家通过精确的videoid查看分享的某录像;
	case MSG_C_2_S_GETSHAREVIDEO:
		HanderUserGetShareVideo((LMsgC2SGetShareVideo*) msg);
		break;
	case MSG_C_2_S_ONE_VIP_LOG:
		HanderUserOneVipLog((LMsgC2SOneVipLog*) msg);
		break;
	case MSG_C_2_S_REQ_SIGN_COLLECT:
		HanderUserSignCollect((LMsgC2SReqSignCollect*) msg);
		break;
	case MSG_C_2_S_REQ_SIGN_COLLECT_WIN_STATE:
		HanderUserSignCollectWinState((LMsgC2SReqSignCollectWinState*) msg);
		break;
	case MSG_C_2_S_GET_TASK:
		HanderUserGetPlayerTask((LMsgC2SGetTask*)msg);
		break;
	case MSG_C_2_S_GET_USER_INFO:
		HanderGetUserInfo((LMsgC2SGetUserInfo*)msg);
		break;
	case MSG_C_2_S_RECV_TASK_REWARD:
	case MSG_C_2_S_GET_INVITING_INFO:
	case MSG_C_2_S_BIND_INVITER:
	case MSG_C_2_S_USER_SHARED:
		m_taskMgr.Process(msg);
		break;
	case MSG_C_2_S_REQUEST_SHARE_CARD:
		HanderUserRequest_Share_Card((LMsgC2SRequest_Share_Card*)msg);
		break;
	case MSG_C_2_S_WEALTH_CHANGE:
		HanderUserWealthChange((LMsgC2SWealthChange*) msg);
		break;
	default:
		if (msg->m_msgId > MSG_MIN_ARENA_MSGID && msg->m_msgId < MSG_MAX_ARENA_MSGID)	
		{
			HanderUserArenaMSG(msg);
		}
		else	{  LLOG_ERROR("LogicManager Message Error: %d", msg->m_msgId);	}
		break;
	}
}

Lint User::getNeedCard(Lint nCounts)
{
	Lint nCards = 1;
	switch (nCounts)
	{
	case 1:
		nCards = 2;
		break;
	case 2:
		nCards = 3;
		break;
	case 3:
		nCards = 5;
		break;
	}
	return nCards;
}

//////////////////////////////////////////////////////////////////////////
//注释玩家操作;

void User::HanderUserCreateDesk(LMsgC2SCreateDesk * msg)
{
	if (msg == NULL)
	{
		return;
	}
	if (game_zone_gold == msg->m_GameZone)
	{
		HanderUserCreateGoldDesk(msg);
		return;
	}

	bool bisFreeTime = false;

	// 如果创建的是使用工会创建房间，则需要转发到工会服务器去处理 [2017-7-23 willing]
	if (msg->m_useTeamCard == 1)
	{
		if (!gWork.IsConnectTeamServer())
		{
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = 8;			// 工会服务器不存在，无法使用工会创建房间 [2017-7-23 willing]
			Send(ret);
			return;
		}

		LMsgLMG2TS_CreateDesk createDeskMsg;
		createDeskMsg.m_userID = m_userData.m_id;
		createDeskMsg.m_playtypeCount = msg->m_extValue.size();
		createDeskMsg.m_free = bisFreeTime ? 1 : 0;

		createDeskMsg.m_flag = msg->m_flag;						//房间类型，4圈，8圈，12圈
		createDeskMsg.m_secret = msg->m_secret;					//房间密码，如果为空，服务器会随机一个密码
		createDeskMsg.m_gold = msg->m_gold;						//房间抵住，单位元，最低为1元，最多不能超过100
		createDeskMsg.m_state = msg->m_gameType;					//玩法规则 
		createDeskMsg.m_robotNum = msg->m_robotNum;					// 0,不加机器人，1，2，3加机器人数量
		for (int i = 0; i < CARD_COUNT; i++)
		{
			createDeskMsg.m_cardValue[i].m_color = msg->m_cardValue[i].m_color;
			createDeskMsg.m_cardValue[i].m_number = msg->m_cardValue[i].m_number;
		}
		createDeskMsg.m_lenth = msg->m_lenth;
		for (int i = 0; i < createDeskMsg.m_playtypeCount; i++)
		{
			createDeskMsg.m_playtype.push_back(msg->m_extValue[i]);
		}

		if (createDeskMsg.m_playtypeCount == 0 && msg->m_playtype.size())
		{
			createDeskMsg.m_playtype.clear();
			createDeskMsg.m_playtype.assign(msg->m_playtype.begin(), msg->m_playtype.end());
		}

		createDeskMsg.m_useTeamCard = msg->m_useTeamCard;		// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
		createDeskMsg.m_teamID = msg->m_teamID;					// 工会ID [2017-7-23 willing]
		createDeskMsg.m_circleGoldLimit = msg->m_circleGoldLimit;
		createDeskMsg.m_stake = msg->m_stake;
		createDeskMsg.m_playerNum = msg->m_playerNum;
		createDeskMsg.m_nMaxFanshu = msg->m_nMaxFanshu;
		gWork.SendMessageToTeamServer(createDeskMsg);

		return;
	}

	////房卡不足;
	//Lint needCardnum = getNeedCard(msg->m_flag);
	//if(!gConfig.GetFree() && !gConfig.GetGameTypeFree(msg->m_gameType) && !IfCardEnough(CARD_TYPE_8, needCardnum))    //目前这里只是用普通卡,m_flag为客户端房卡数;
	//{
	//	LLOG_DEBUG("ERROR: User::HanderLMGUserCreateDesk Card Not Enough!s!");
	//	LMsgS2CCreateDeskRet ret;
	//	ret.m_errorCode = 1;
	//	Send(ret);
	//	return;
	//}
	LLOG_DEBUG("User::HanderUserCreateDesk %d", msg->m_flag);
	LMsgLMG2LCreateDesk send ;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_geteUserId = GetUserGateUserId();
	send.m_ip = GetIp();
	send.m_usert = m_userData;	
	send.m_flag = msg->m_flag;
	send.m_state = msg->m_flag;
	send.m_secret = msg->m_secret;
	send.m_gold = msg->m_gold;
	send.m_gameType = msg->m_gameType;
	send.m_nMaxFanshu = msg->m_nMaxFanshu;
	send.m_robotNum = msg->m_robotNum;
	send.m_black3first = msg->m_black3first;
	send.m_lenth = msg->m_lenth;
	send.m_extValue = msg->m_extValue;
	send.m_proportionate = gConfig.GetProportionate();
	memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );

	LogicInfo* logic = gWork.GetLogicInfoById(getUserLogicID());	//获取玩家当前的逻辑
	if (logic == NULL)
	{
		logic = gWork.GetLogicInfoByPresure(msg->m_gameType);						//获取负载最小的逻辑
	}
	else
	{
		LLOG_ERROR("User::HanderUserCreateDesk Current ServerID Not Null %d", getUserLogicID());
	}
	if (logic == NULL)
	{
		LLOG_ERROR("ERROR: User::HanderLMGUserCreateDesk Logic Not Exist!");
		return;
	}
	send.m_deskID = gDeskManager.GetFreeDeskId(logic->m_id);
	logic->m_deskCount ++;		//逻辑桌子数加一
	logic->m_sp->Send(send.GetSendBuff());
}

void User::HanderUserCreateGoldDesk(LMsgC2SCreateDesk * msg)
{
	if (msg == NULL)
	{
		LLOG_DEBUG("Create GoldDesk ERROR: mis point is NULL!");
		return;
	}

	LogicInfo* logic = gWork.GetLogicInfoById(getUserLogicID());	//获取玩家当前的逻辑;
	if (NULL != logic && getUserLogicID() != gConfig.GetLogic4GoldGame())
	{
		LMsgS2CCreateDeskRet msg;
		msg.m_errorCode = 3;
		Send(msg);
		LLOG_DEBUG("User::HanderUserCreateGoldDesk Current ServerID Not Null %d", getUserLogicID());
		return;
	}
	else
	{
		//根据需要调整金币场所在logic,当前最好使得玩家在固定server上,保证速度匹配成功;
		logic = gWork.GetLogicInfoById( gConfig.GetLogic4GoldGame() );		
	}
	if (logic == NULL)//金币场logic不在线则直接退出;
	{
		LMsgS2CCreateDeskRet msg;
		msg.m_errorCode = 4;
		Send(msg);
		LLOG_ERROR("ERROR: User::HanderLMGUserCreateGoldDesk Logic Not Exist!");
		return;
	}

	//金币不足;
	if (0 == m_userData.m_NewGolduser)  //首次赠送金币后置1;
	{
		SaveGoldChange(SYSTEMFIRSTGIVE, g_ngiveGoldNewPlayer);
		m_userData.m_Gold = g_ngiveGoldNewPlayer;
		m_userData.m_NewGolduser = 1;
		send2UserGoldChange(g_ngiveGoldNewPlayer, SYSTEMFIRSTGIVE, g_ngiveGoldNewPlayer);
	}
	if( !IfGoldEnough(g_ngiveGoldPerDay) )    
	{
		if ( !GiveUserGold(g_ngiveGoldPerDay))  //赠送次数超限，赠送失败;
		{
			LLOG_DEBUG("Create GoldDesk ERROR: Gold is Not Enough!");
			LMsgS2CCreateDeskRet ret;
			ret.m_errorCode = 11;   //进入金币场失败;
			Send(ret);
			return;
		}
		return;
	}
	LLOG_DEBUG("User::HanderUserCreateGoldDeskd %d", msg->m_flag);
	LMsgLMG2LIntoGoldDesk send ;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_geteUserId = GetUserGateUserId();
	send.m_ip = GetIp();
	send.m_usert = m_userData;	
	send.m_gameType = msg->m_gameType;
	send.m_robotNum = msg->m_robotNum;
	send.m_lenth = msg->m_lenth;
	send.m_extValue = msg->m_extValue;
	memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue) );



	//玩家加入等待列表，人满后开房;暂不如此实现;
	//gUserManager.AddUser2Wait(this);

	//首先获取还没有满员的金币场桌子填补;
	send.m_deskID = gDeskManager.GetFreeGoldDeskId(logic->m_id, msg->m_gameType);
	logic->m_deskCount++;		//逻辑桌子数加一
	logic->m_sp->Send(send.GetSendBuff());
	LLOG_DEBUG("user into GoldDesk, send to LogicServer, deskId:%d", send.m_deskID);
}

void User::HanderUserAddDesk(LMsgC2SAddDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	if (msg->m_useTeamCard == 0)
	{
		if (gWork.m_TeamServerDeskSet.count(msg->m_deskId))
			msg->m_useTeamCard = 1;
	}

	// 如果创建的是使用工会创建房间，则需要转发到工会服务器去处理 [2017-7-23 willing]
	if (getUserLogicID() == 0 && msg->m_useTeamCard == 1)
	{
		if (!gWork.IsConnectTeamServer())
		{
			LMsgS2CAddDeskRet ret;
			ret.m_deskId = msg->m_deskId;
			ret.m_errorCode = 3;
			Send(ret);
			return;
		}
		LMsgLMG2TS_AddDesk addDeskMsg;
		addDeskMsg.m_userID = m_userData.m_id;
		addDeskMsg.m_location = msg->m_location;
		addDeskMsg.m_deskId = msg->m_deskId;
		addDeskMsg.m_card_num = m_userData.m_numOfCard2s;
		gWork.SendMessageToTeamServer(addDeskMsg);
		return;
	}


	LLOG_DEBUG("User::HanderUserAddDesk %d", msg->m_deskId);
	LMsgLMG2LAddToDesk send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_geteUserId = GetUserGateUserId();
	send.m_deskID = msg->m_deskId;
	send.m_ip = GetIp();
	send.m_usert = m_userData;
	LogicInfo* logic = gWork.GetLogicInfoById(getUserLogicID());	//获取玩家当前的逻辑
	if (logic == NULL)
	{
		Lint logicID = gDeskManager.getServerIDbyDeskID(msg->m_deskId);
		if (gConfig.GetLogic4GoldGame() != logicID)    //金币场禁止通过房间号进入;
		{
			logic = gWork.GetLogicInfoById(logicID);
		}
	}
	else
	{
		LLOG_DEBUG("User::HanderUserAddDesk Current ServerID Not Null %d", getUserLogicID());
	}
	if (logic == NULL)
	{

		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_DEBUG("ERROR: User::HanderUserAddDesk Logic Not Exist!");
		return;
	}
	logic->m_sp->Send(send.GetSendBuff());

}

void User::HanderUserVipLog(LMsgC2SVipLog* msg)
{
	if(!msg)
		return;
	LLOG_DEBUG("User::HanderUserVipLog %d:%d",msg->m_userId,m_userData.m_gm);
	LMsgL2LBDReqVipLog log;
	log.m_userId = GetUserDataId();
	if (m_userData.m_gm)
	{
		log.m_reqUserId = msg->m_userId;
		LLOG_ERROR("User::HanderUserVipLog %d:%d", log.m_reqUserId, log.m_userId);
	}
	else
	{
		log.m_reqUserId = GetUserDataId();
	}
	gWork.SendMsgToDb(log);
}

void User::HanderUserOneVipLog(LMsgC2SOneVipLog* msg)
{
	if(!msg)
		return;
	LMsgL2LDBReqOneVipLog dbmessage;
	dbmessage.m_log_Id = msg->m_log_id;
	dbmessage.m_reqUserId = m_userData.m_id;
	gWork.SendMsgToDb(dbmessage);
}

void User::HanderUserVideo(LMsgC2SVideo* msg)
{
	LMsgL2LDBReqVideo video;
	video.m_userId = GetUserDataId();
	video.m_videoId = msg->m_videoId;
	//gWork.SendToCenter(video);
	gWork.SendMsgToDb(video);
}

void User::HanderUserHeart(LMsgC2SHeart* msg)
{
	LMsgS2CHeart send;
	send.m_time = gWork.GetCurTime().Secs();
	Send(send);
}

void User::HanderUserWealthChange(LMsgC2SWealthChange* msg)
{
	getChange2WealthNum(msg->m_Weath1Type, msg->m_Weath1Num, msg->m_Weath2Type);
}

Lint User::getChange2WealthNum(Lint Weath1Type, Lint Weath1Num, Lint Weath2Type)
{
	Lint nWealth2Num = 0;
	switch (Weath1Type)
	{
	case USER_WEALTH_CARD_TYPE_4:
	case USER_WEALTH_CARD_TYPE_8:
	case USER_WEALTH_CARD_TYPE_16:
		if (IfCardEnough(Weath1Type, Weath1Num))
		{
			if (USER_WEALTH_GOLD == Weath2Type)
			{
				switch (Weath1Num)
				{
				case 3:
					nWealth2Num = 4500;
					ChangeGoldCount(CARDCHANGE2GOLD, nWealth2Num);
					DelCardCount(Weath1Type, 3, CARDS_OPER_TYPE_CHANGE2GOLD, "3card2gold");
					break;
				case 9:
					nWealth2Num = 15000;
					ChangeGoldCount(CARDCHANGE2GOLD, nWealth2Num);
					DelCardCount(Weath1Type, 9, CARDS_OPER_TYPE_CHANGE2GOLD, "9card2gold");
					break;
				case 20:
					nWealth2Num = 35000;
					ChangeGoldCount(CARDCHANGE2GOLD, nWealth2Num);
					DelCardCount(Weath1Type, 20, CARDS_OPER_TYPE_CHANGE2GOLD, "20card2gold");
					break;
				}
			}
		}
		break;
	default:
		break;
	}
	return nWealth2Num;
}

void User::HanderUserDraw(LMsgC2SRequestDraw* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LMsgS2CFinishDraw finish;
	//判断活动是否开启
	Lint date = gWork.GetCurTime().GetDate();
	if(!gActiveManager.IsActiveOpoen(date))
	{
		finish.m_errorCode = 2;
		Send(finish);
		return;
	}
	//判断自已今天是否还能奖:
	if (GetLeftTimesToday() <= 0)
	{
		finish.m_errorCode = 1;
		Send(finish);
		return;
	}
	//如果有其他条件，在此判断;
	finish.m_errorCode = CheckQualification();
	if (finish.m_errorCode)
	{
		Send(finish);
		return;
	}
	//可以抽奖
	Lint activeID = gActiveManager.GetLastActiveID();
	ActiveRun_RetGift drawInfo;
	if ( !gActiveManager.Run(activeID, drawInfo))
	{
		finish.m_errorCode = 3;		//对不起，本次抽奖失败，请重新抽奖。
		Send(finish);
		return;
	}

	//抽奖成功消耗;
	ConsumeForDial();
	gActiveManager.CheckGiftSafe(activeID, drawInfo);

	if (drawInfo.Gift_type == GT_ROOMCARD)
	{
		//赠房卡
		AddCardCount(CARD_TYPE_8, drawInfo.Gift_count, CARDS_OPER_TYPE_ACTIVE, "system");
		finish.m_NeedPhoneNum = 0;
	}
	else if(drawInfo.Gift_type == GT_PHONECARD)
	{
		//话费
		finish.m_NeedPhoneNum = 1;
	}
	else if (GT_GOLD == drawInfo.Gift_type)
	{
		//奖励金币；
		ChangeGoldCount(BETWIN, drawInfo.Gift_count);
	}
	else
	{
		finish.m_NeedPhoneNum = 0;
	}

	finish.m_errorCode = 0;
	finish.m_date = date;
	finish.m_RewardID = drawInfo.giftID;
	Send(finish);

	//写内存
	LActive active ;
	active.m_activeID = activeID;
	active.m_RewardID = drawInfo.giftID;
	active.m_LasJoinDate = date;
	m_Active.push_back(active);

	//发到Center
	LMsgL2LDBSyncDraw sync;
	sync.m_activeID = activeID;
	sync.m_userId = GetUserDataId();
	sync.m_giftID = drawInfo.giftID;
	sync.m_date = date;
	gWork.SendMsgToDb(sync);
	return;
}

Lint User::CheckQualification()
{
	if (1088 == gActiveManager.GetLastActiveID() )   //每天金币场够六局，可以消耗1000金币抽奖一次，每日最多五次;
	{
		//检查金币;
		if (m_userData.m_Gold < 1000)
		{
			return 4;  //金币不足1000;
		}
		//检查是否够打够局数;
		Lint nTimes = gActiveManager.getTimesPerday() - GetLeftTimesToday(); //已抽奖次数;
		if ((nTimes + 1) * 6<= GetTimesOfGold())
		{
			return 0;  //可以抽奖;
		}
		else
		{
			return 5;   //没打够相应局数;
		}
	}
	return 0;
}

void User::ConsumeForDial()
{
	if (1088 == gActiveManager.GetLastActiveID() )   //每天金币场够六局，可以消耗1000金币抽奖一次，每日最多五次;
	{
		//扣除金币;
		ChangeGoldCount(BETUSED, -1000);
	}
}

Lint User::GetTimesOfGold()
{
	Lint date = gWork.GetCurTime().GetDate();
	if (gUserManager.m_userExDataMap.count(GetUserDataId()) && 
		date == gUserManager.m_userExDataMap[GetUserDataId()].getLastGoldGameDate())
	{
		return gUserManager.m_userExDataMap[GetUserDataId()].getGoldGameTimesOfToday();
	}
	return 0;
}

void User::HanderWritePhone(LMsgC2SWritePhone* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LMsgS2CWritePhone send;
	LMsgL2LDBSyncPhone sync;
	Lint activeID = gActiveManager.GetLastActiveID();
	Lint date = gWork.GetCurTime().GetDate();
	if (msg->m_activeID != activeID)
	{
		send.m_errorCode = 2;
		Send(send);
		return;
	}
	//写入内存
	if (m_Active.size() == 0)
	{
		send.m_errorCode = 1;
		Send(send);
		return;
	}
	bool find = false;
	std::vector< LActive >::iterator itor = m_Active.begin();
	for (;itor != m_Active.end(); ++itor )
	{
		if ((itor)->m_LasJoinDate == date)
		{
			(itor)->m_phoneNum = msg->m_phoneNum;
			send.m_PhoneNum = msg->m_phoneNum;
			send.m_errorCode = 0;
			sync.m_activeID = activeID;
			sync.m_userId = GetUserDataId();
			sync.m_date = (itor)->m_LasJoinDate;
			sync.m_phone = msg->m_phoneNum;

			find = true;
			break;
		}
	}
	if (!find)
	{
		send.m_errorCode = 1;
		Send(send);
		return;
	}
	//发送Center
	Send(send);
	gWork.SendMsgToDb(sync);
}

void User::HanderUserReqActive(LMsgC2SActiveLog* msg)
{
	if (msg == NULL)
	{
		return;
	}
	LMsgS2CDrawInfo drawInfo;
	drawInfo.m_num = 0;
	std::vector< LActive >::iterator itor = m_Active.begin();
	for (;itor != m_Active.end(); ++itor )
	{
		LActiveInfo info;
		info.m_activeID = (itor)->m_activeID;
		info.m_rewardID = (itor)->m_RewardID;
		info.m_phoneNum = (itor)->m_phoneNum;
		info.m_date = (itor)->m_LasJoinDate;
		drawInfo.m_DrawInfo.push_back(info);
		drawInfo.m_num++;
	}
	Send(drawInfo);
}

static bool SortFun(const LActive& c1, const LActive& c2)
{
	if(c1.m_LasJoinDate > c2.m_LasJoinDate)
	{
		return true;
	}

	return false;
}

void User::SendNoticeDrawGift()
{
	//判断活动是否开启
	Lint date = gWork.GetCurTime().GetDate();
	do
	{
		LMsgS2CRet_Share_Card send;
		send.m_date = date;
		int lastJoindate = 0;
		for (auto it = m_Active.begin(); it != m_Active.end(); ++it)
		{
			if (it->m_activeID == DAILY_SHARE_CARD)
			{
				lastJoindate = lastJoindate > it->m_LasJoinDate ? lastJoindate : it->m_LasJoinDate;
			}
		}
		if (lastJoindate == date)
		{
			send.m_playerState = 1;
		}
		Send(send);
	} while (0);
	if(!gActiveManager.IsActiveOpoen(date))
	{
		LMsgS2CActiveInfo send;
		send.m_error = 1;
		Send(send);
		return;
	}
	Lint activeID = gActiveManager.GetLastActiveID();
	LMsgS2CActiveInfo send;
	send.m_activeID = activeID;
	send.m_LeftNums = GetLeftTimesToday();

	//未抽过奖
	if (m_Active.size() == 0)
	{
		send.m_LastJoinDate = 0;	//未参与过
		send.m_LastGiftState = 0;	//未抽过奖
		Send(send);
		return;
	}
	//抽过奖
	std::sort(m_Active.begin(),m_Active.end(), SortFun);
	send.m_LastJoinDate = m_Active[0].m_LasJoinDate;
	send.m_RewardID = m_Active[0].m_RewardID;
	if (m_Active[0].m_LasJoinDate == date)
	{
		send.m_LastGiftState = 1;
		send.m_NeedPhoneNum = 0;
		if (gActiveManager.IsNeedPhoneNum(activeID, m_Active[0].m_RewardID - 1) && m_Active[0].m_phoneNum == "")
		{
			send.m_NeedPhoneNum = 1;
		}
	}
	else
	{
		send.m_LastGiftState = 0;
	}
	Send(send);
}

//获取当天剩余抽奖次数;
Lint User::GetLeftTimesToday()   
{
	//判断活动是否开启
	Lint date = gWork.GetCurTime().GetDate();
	if (!gActiveManager.IsActiveOpoen(date))
	{
		return 0;
	}
	Lint nLeftNums = gActiveManager.getTimesPerday();
	for (unsigned int i = 0; i < m_Active.size(); ++i)
	{
		if (1088 == m_Active[i].m_activeID && m_Active[i].m_LasJoinDate == date)
		{
			nLeftNums--;
		}
	}
	return nLeftNums;
}

Lstring User::GetIp()
{
	return m_ip;
}

void User::SetIp(Lstring& ip)
{
	m_ip = ip;
}

void User::Save()
{
	LMsgL2CeSaveUser save;
	save.m_ID = gConfig.GetServerID();
	save.m_type = m_online?1:0;
	save.m_user = m_userData;
	gWork.SendToCenter(save);
}

bool User::IfCardEnough(Lint cardType, Lint num)
{
	bool bEnough = false;
	switch (cardType)
	{
	case CARD_TYPE_4:
		{
			if( 2 * m_userData.m_numOfCard2s >= num)
				bEnough = true;
		}
		break;
	case CARD_TYPE_8:
		{
			if( m_userData.m_numOfCard2s >= num)
				bEnough = true;
		}
		break;
	case CARD_TYPE_16:
		{
			if( m_userData.m_numOfCard2s >= 2 * num)
				bEnough = true;
		}
		break;
	default:
		break;
	}

	return bEnough;
}

void User::AddCardCount(Lint cardType, Ldouble count, Lint operType,Lstring admin, bool bNeedSave)
{
	LLOG_INFO("User::AddCardCount type=%d,count=%d,operType=%d", cardType, count, operType);
	Ldouble addCount = 0;
	switch (cardType)
	{
	case CARD_TYPE_4:
		m_userData.m_numOfCard2s += count / 2;
		m_userData.m_totalbuynum += count / 2;
		addCount = count/2;
		//m_userData.m_numOfCard1s += count;
		break;
	case CARD_TYPE_8:
		m_userData.m_numOfCard2s += count;
		m_userData.m_totalbuynum += count;
		addCount = count;
		break;
	case CARD_TYPE_16:
		m_userData.m_numOfCard2s += count * 2;
		m_userData.m_totalbuynum += count * 2;
		addCount = count*2;
		//m_userData.m_numOfCard3s += count;
		break;
	default:
		break;
	}
	SaveCardExchange(operType,cardType, count);
	SendItemInfo();
}

void User::DelCardCount(Lint cardType, Ldouble count, Lint operType,Lstring admin)
{
	LLOG_INFO("User::DelCardCount type=%d,count=%d,operType=%d", cardType, count, operType);
	Ldouble delCount = 0;
	switch (cardType)
	{
	case CARD_TYPE_4:
		delCount = count / 2;
		if (m_userData.m_numOfCard2s >= count / 2)
			m_userData.m_numOfCard2s -= count / 2;
		else
			m_userData.m_numOfCard2s = 0;
		break;
	case CARD_TYPE_8:
		delCount = count;
		if (m_userData.m_numOfCard2s >= count)
			m_userData.m_numOfCard2s -= count;
		else
			m_userData.m_numOfCard2s = 0;
		break;
	case CARD_TYPE_16:
		delCount = count * 2;
		if (m_userData.m_numOfCard2s >= count * 2)
			m_userData.m_numOfCard2s -= count * 2;
		else
			m_userData.m_numOfCard2s = 0;
	default:
		break;
	}

	SaveCardExchange(operType,cardType, count);
	SendItemInfo();
}

void User::ChangeGoldCount(Lint changeReason, Ldouble changeNum)
{
	m_userData.m_Gold += changeNum;
	SaveGoldChange(changeReason, changeNum);
	send2UserGoldChange(changeNum, changeReason, m_userData.m_Gold);
}

void User::RequestActiveInfo(LMsgC2SActiveInfoRequest* msg)
{
	Lint date = gWork.GetCurTime().GetDate();
	//if(!gActiveManager.IsActiveOpoen(date))
	//{
	//	if (gConfig.GetShareGet1Card() != 0)
	//	{
	//		LMsgL2LDBReqActive active;
	//		active.m_userId = GetUserDataId();
	//		active.m_activeId = DAILY_SHARE_CARD; //
	//		gWork.SendMsgToDb(active);
	//	}
	//	return;
	//}
	if (m_Active.size() > 0)
	{
		//如果内存有信息，不再请求，直接返回;
		SendNoticeDrawGift();
		return;
	}
	LMsgL2LDBReqActive active;
	active.m_userId = GetUserDataId();
	active.m_activeId = gActiveManager.GetLastActiveID();
	gWork.SendMsgToDb(active);
}

void User::SendHuoDong()
{
	return;
}

void User::SyncActiveOpen()
{
	Lint date = gWork.GetCurTime().GetDate();
	if(!gActiveManager.IsActiveOpoen(date))
	{
		return;
	}
	LMsgS2CActiveOpen active;
	active.m_activeID = gActiveManager.GetLastActiveID();
	Send(active);
}

void User::SyncLoadSignCollectWin()
{
	Lint date = gWork.GetCurTime().GetDate();
	if(!m_actMgr_signColl.IsActiveOpen(date))
	{
		return;
	}
	
	std::vector<ActiveMgr_SignCollect::ActiveType> activeTypes = m_actMgr_signColl.GetActiveTypes();
	Lint length = activeTypes.size();
	for(int i = 0; i< length; ++i)
	{
		if(!m_actMgr_signColl.IsActiveLoaded(activeTypes[i]))
		{
			LMsgL2LDBReqSignCollectWin askActive;
			askActive.m_activeType = activeTypes[i];
			askActive.m_userId =  GetUserDataId();
			gWork.SendMsgToDb(askActive);
		}
	}

	//请求全局数据（有用户时请求;
	if(!gActiveManager.GetLoaded(ActiveManager::EAWin) && !gActiveManager.GetWait())
	{	
		gActiveManager.SetWait(true);
		LMsgL2LDBReqSignCollectWin askActive;
		askActive.m_activeType = ActiveManager::EAWin;
		askActive.m_userId =  GetUserDataId();
		gWork.SendMsgToDb(askActive);
	}

}

void User::ModifyUserPlayCount(Lint nCount)
{
	m_userData.m_totalplaynum = nCount;
	LMsgLMG2CeSaveUserPlayCount msg;
	msg.m_unionid = m_userData.m_unioid;
	msg.m_playCount = nCount;
	gWork.SendToCenter(msg);
}

void User::HanderUserShareVideo(LMsgC2SShareVideo* msg)
{
	LMsgL2LDBGetShareVideo get_video;
	if(msg)
	{
		get_video.m_shareId = msg->m_shareID;
		get_video.m_userId = m_userData.m_id;
		gWork.SendMsgToDb(get_video);
	}
}

void User::HanderUserReqShareVideo(LMsgC2SReqShareVideo* msg)
{
	if(msg)
	{
		LLOG_INFO("User search for share id,video id:%d",msg->m_videoid.c_str());
		LMsgL2LDBGGetShareVideoId get_share_id;
		get_share_id.m_userId = m_userData.m_id;
		get_share_id.m_videoId = msg->m_videoid;
		gWork.SendMsgToDb(get_share_id);
	}
}

void User::HanderUserGetShareVideo(LMsgC2SGetShareVideo* msg)
{
	if(msg)
	{
		LMsgL2LDBGetSsharedVideo video;
		video.m_userId = GetUserDataId();
		video.m_videoId = msg->m_videoId;
		gWork.SendMsgToDb(video);
	}

}

//十一活动新玩法
void User::HanderUserSignCollect(LMsgC2SReqSignCollect * msg)
{
	if (msg == NULL)
	{
		return;
	}

	if(msg->m_type == ActiveMgr_SignCollect::ESign)
	{
		ResSign();
	}
	else if(msg->m_type == ActiveMgr_SignCollect::ECollectCard)
	{
		ResCollect();
	}
}

void User::ResSign()
{
	LMsgS2CResSign resSign;

	//判断活动是否开启
	Lint date = gWork.GetCurTime().GetDate();
	ActiveMgr_SignCollect::ActiveType type = ActiveMgr_SignCollect::ESign;
	if(!m_actMgr_signColl.IsActiveOpen(type, date))	//没开启
	{
		resSign.m_errorCode = 2;
		Send(resSign);
		return;
	}

	if(m_actMgr_signColl.IsSignByDate(date))	//今天是否签到过
	{
		resSign.m_errorCode = 1;
		Send(resSign);
		return;
	}

	//抽奖
	ActiveOutInfo out;
	if ( !m_actMgr_signColl.run(type, date,  out)) //run
	{
		resSign.m_errorCode = 3;		//对不起，本次抽奖失败，请重新抽奖。
		Send(resSign);
		return;
	}

	//保存db
	LMsgL2LDBSaveSignCollectWin syncSign;
	syncSign.m_userId = GetUserDataId();
	syncSign.m_date = date;
	syncSign.m_activeType = ActiveMgr_SignCollect::ESign;
	syncSign.m_drawChanceCount= 0;	//不需要用到
	gWork.SendMsgToDb(syncSign);


	//检查最终大奖
	if(m_actMgr_signColl.CheckFinalGift(type))
	{
		Lint Gift_type = GT_ROOMCARD;
		if (Gift_type == GT_ROOMCARD)
		{
			//赠房卡
			AddCardCount(CARD_TYPE_8, 11, CARDS_OPER_TYPE_ACTIVE, "system");
		}

		resSign.m_errorCode = 0;
		resSign.m_rewardCardCount = 11;
		resSign.m_date = date;

		Send(resSign);


		//保存db
		LMsgL2LDBSaveSignCollectWin syncSign;
		syncSign.m_userId = GetUserDataId();
		syncSign.m_activeType = ActiveMgr_SignCollect::ESign;	//保存
		syncSign.m_date = date;
		gWork.SendMsgToDb(syncSign);
		return;
	}
	else
	{
		//下发奖品
		Lint Gift_type = GT_ROOMCARD;
		if (Gift_type == GT_ROOMCARD)
		{
			//赠房卡
			AddCardCount(CARD_TYPE_8, 1, CARDS_OPER_TYPE_ACTIVE, "system");
		}

		resSign.m_errorCode = 0;
		resSign.m_rewardCardCount = 1;
		resSign.m_date = date;

		Send(resSign);
		return;
	}

	return;
}

void User::ResCollect()
{
	LMsgS2CResCollect resColl;

	//判断活动是否开启
	Lint date = gWork.GetCurTime().GetDate();
	ActiveMgr_SignCollect::ActiveType type = ActiveMgr_SignCollect::ECollectCard;

	if(!m_actMgr_signColl.IsActiveOpen(type, date))	//没开启
	{
		resColl.m_errorCode = 2;
		Send(resColl);
		return;
	}

	if(!m_actMgr_signColl.IsLeftDrawByDate(date))	//今天是否还有抽奖机会
	{
		resColl.m_errorCode = 1;
		Send(resColl);
		return;
	}

	//可以抽奖
	ActiveOutInfo infoOut;
	if ( !m_actMgr_signColl.run(type, date, infoOut)) //run
	{
		resColl.m_errorCode = 3;		//对不起，本次抽奖失败，请重新抽奖。
		Send(resColl);
		return;
	}

	//抽过奖
	if(infoOut.m_hadGift == 1)
	{
		resColl.m_errorCode = 4;		//抽过了
		Send(resColl);
		return;
	}


	//检查最终大奖
	if(m_actMgr_signColl.CheckFinalGift(type))
	{
		Lint Gift_type = GT_ROOMCARD;
		if (Gift_type == GT_ROOMCARD)
		{
			//赠房卡
			AddCardCount(CARD_TYPE_8, 101, CARDS_OPER_TYPE_ACTIVE, "system");
		}

		resColl.m_errorCode = 0;
		resColl.m_rewardCardCount = 101;
		resColl.m_word = infoOut.m_word;
		resColl.m_date = date;
		resColl.m_chanceCount = infoOut.m_drawChanceCount;

		Send(resColl);


		//保存集字信息
		LMsgL2LDBSaveSignCollectWin syncColl;
		syncColl.m_isUpdate = m_actMgr_signColl.GetCollectUpdate();;	//update
		syncColl.m_userId = GetUserDataId();
		syncColl.m_activeType = ActiveMgr_SignCollect::ECollectCard;
		assert(infoOut.m_drawChanceCount >= 0);
		syncColl.m_drawChanceCount = 0;
		assert(infoOut.m_words.size() > 0);
		syncColl.m_words = infoOut.m_words;
		syncColl.m_date = date;
		syncColl.m_hadGift = 1;
		gWork.SendMsgToDb(syncColl);

		LActiveSignCollectSS newWin;
		newWin.m_nickName = m_userData.m_nike;
		newWin.m_date = date;
		//增加中奖者
		if(gActiveManager.AddWin(newWin))
		{
			//保存中奖者
			LMsgL2LDBSaveSignCollectWin syncColl;
			syncColl.m_userId = GetUserDataId();
			syncColl.m_activeType = ActiveManager::EAWin;
			syncColl.m_date = date;
			syncColl.m_name = m_userData.m_nike;
			gWork.SendMsgToDb(syncColl);
		}

		return;
	}


	resColl.m_errorCode = 0;
	resColl.m_date = date;
	resColl.m_rewardCardCount = 0;
	resColl.m_word = infoOut.m_word;
	resColl.m_chanceCount = infoOut.m_drawChanceCount;
	Send(resColl);

	if(infoOut.m_isNewWord)
	{
		//保存集字信息
		LMsgL2LDBSaveSignCollectWin syncColl;
		syncColl.m_isUpdate = m_actMgr_signColl.GetCollectUpdate();
		syncColl.m_userId = GetUserDataId();
		syncColl.m_activeType = ActiveMgr_SignCollect::ECollectCard;
		assert(infoOut.m_drawChanceCount >= 0);
		syncColl.m_drawChanceCount = infoOut.m_drawChanceCount;
		assert(infoOut.m_words.size() > 0);
		syncColl.m_words = infoOut.m_words;
		syncColl.m_date = date;
		syncColl.m_hadGift = infoOut.m_hadGift;
		gWork.SendMsgToDb(syncColl);

		m_actMgr_signColl.SetCollectUpdate(true);
	}
	return;
}

//-------------------查询签到状态-----------------------------
void User::HanderUserSignCollectWinState(LMsgC2SReqSignCollectWinState * msg)
{
	if (msg == NULL)
	{
		return;
	}
	
	if(msg->m_type == ActiveMgr_SignCollect::ESign)
	{
		ResSignState();
	}
	else if(msg->m_type == ActiveMgr_SignCollect::ECollectCard)
	{
		ResCollectState();
	}
	else if(msg->m_type == ActiveManager::EAWin)
	{
		ResWinState();
	}
}

void User::ResSignState()
{

	LMsgS2CResSignState drawInfo;
	drawInfo.m_num = 0;
	auto actives = m_actMgr_signColl.GetActive(ActiveMgr_SignCollect::ESign);
	std::vector< LActiveSignCollectSS>::iterator itor = actives.begin();

	Lint todayDate = gWork.GetCurTime().GetDate();
	drawInfo.m_signState = m_actMgr_signColl.IsSignByDate(todayDate); //今天是否签到过

	for (;itor != actives.end(); ++itor )
	{
		LActiveSignCollect info;
		info.m_activeType = itor->m_activeType;
		info.m_date = itor->m_date;
		drawInfo.m_DrawInfo.push_back(info);
		drawInfo.m_num++;
	}
	Send(drawInfo);
}

void User::ResCollectState()
{
	LMsgS2CResCollectState drawInfo;
	drawInfo.m_num = 0;
	auto actives = m_actMgr_signColl.GetActive(ActiveMgr_SignCollect::ECollectCard);
	std::vector< LActiveSignCollectSS >::iterator itor = actives.begin();
	for (;itor != actives.end(); ++itor )
	{
		LActiveSignCollect info;
		info.m_activeType = itor->m_activeType;
		info.m_date = itor->m_date;
		info.m_drawChanceCount = itor->m_drawChanceCount;
		info.m_words = itor->m_words;
		drawInfo.m_DrawInfo.push_back(info);
		drawInfo.m_num++;
	}
	Send(drawInfo);

}

void User::ResWinState()
{
	LMsgS2CResWinState drawInfo;
	drawInfo.m_num = 0;
	auto actives = gActiveManager.GetActive(ActiveManager::EAWin);
	std::vector< LActiveSignCollectSS >::iterator itor = actives.begin();
	for (;itor != actives.end(); ++itor )
	{
		LActiveSignCollect info;
		info.m_activeType = itor->m_activeType;
		info.m_date = itor->m_date;
		info.m_nickName = itor->m_nickName;
		drawInfo.m_DrawInfo.push_back(info);
		drawInfo.m_num++;
	}
	Send(drawInfo);
}

void User::SaveCardExchange(Lint oper, Lint cardType, Ldouble cardNum)
{
	LMsgLMG2CeCardExchange msg;
	switch(oper)
	{
	case CARDS_OPER_TYPE_CREATE_ROOM :
	case CARDS_OPER_TYPE_CHANGE2GOLD:
	case CARDS_OPER_TYPE_ROOM_FAIL_CARD:
	case CARDS_OPER_TYPE_ROOM_COST_CARD:
	case CARDS_OPER_TYPE_REQ_EXCHAGEGOLD:
		msg.m_exchangeType = 0;
		break;
	case CARDS_OPER_TYPE_INIT:
	case CARDS_OPER_TYPE_ACTIVE:
	case CARDS_OPER_TYPE_INVITING:
	case CARDS_OPER_TYPE_PLAYING:
	case CARDS_OPER_TYPE_ROOM_WIN_CARD:
	case CARDS_OPER_TYPE_ROOM_WIN_CARD_EXADD:
	case CARDS_OPER_TYPE_CANNCEL_EXCHAGEGOLD:
	case CARDS_OPER_TYPE_HTTP_CANNCEL_EXCHAGEGOLD:
	case CARDS_OPER_TYPE_HTTP_REQ_EXCHAGEGOLD:
		msg.m_exchangeType = 1;
		break;
	case CARDS_OPER_TYPE_CHARGE:
	case CARDS_OPER_TYPE_PURCHASE_GIVE:
	case CARDS_OPER_TYPE_VIP_GIVE:
	{
		if (CARDS_OPER_TYPE_CHARGE == oper && getUserLogicID() > 0)
		{
			LogicInfo* logic = gWork.GetLogicInfoById(getUserLogicID());
			if (logic != NULL)
			{
				LMsgLMG2LModifyRoomCard msg;
				msg.m_userid = m_userData.m_id;
				msg.m_card_num = m_userData.m_numOfCard2s;
				msg.m_oper = oper;
				logic->m_sp->Send(msg.GetSendBuff());
			}
		}
	}
		return;
	default:
		LLOG_ERROR("User::SaveCardExchange error oper type");
		return;
	}
	msg.m_oper = oper;
	msg.m_unionId = m_userData.m_unioid;
	msg.m_cardType = cardType;
	msg.m_cardNum = cardNum;
	gWork.SendToCenter(msg);
}

void User::SaveGoldChange(Lint oper,Lint cardNum)
{
	LMsgLMG2CeGoldChange msg;
	msg.m_oper = oper;
	msg.m_GoldNum = cardNum;
	msg.m_unionId = m_userData.m_unioid;
	gWork.SendToCenter(msg);
}

bool User::IfGoldEnough(Lint nGold)
{
	return m_userData.m_Gold >= nGold;
}

bool User::GiveUserGold(Lint giveGoldNum)
{
	LTime m_lastTime;
	m_lastTime.SetSecs(m_userData.m_LastGiftTime);
	LTime m_thisTime = gWork.GetCurTime();
	if (m_lastTime.GetDate() == m_thisTime.GetDate() && m_userData.m_RemainingGifts <= 0)
	{
		return false;
	}
	if (m_thisTime.GetDate() > m_lastTime.GetDate())
	{
		m_userData.m_RemainingGifts = g_nGiveGoleTimes;
	}
	m_userData.m_LastGiftTime = m_thisTime.Secs();
	m_userData.m_Gold += giveGoldNum;
	m_userData.m_RemainingGifts--;

	LMsgLMG2CeGoldChange msg;
	msg.m_oper = GIVEPERDAY;
	msg.m_GoldNum = giveGoldNum;
	msg.m_unionId = m_userData.m_unioid;
	msg.m_lastTime = m_userData.m_LastGiftTime;
	msg.m_RemainingGifts = m_userData.m_RemainingGifts;
	gWork.SendToCenter(msg);

	LMsgS2CGoldChange msg2client;
	msg2client.m_changeNum = giveGoldNum;
	msg2client.m_changeReason = GIVEPERDAY;
	msg2client.m_giveTimes = (g_nGiveGoleTimes - m_userData.m_RemainingGifts);
	msg2client.m_GoldNum = m_userData.m_Gold;
	Send(msg2client);
	return true;
}

void User::send2UserGoldChange(Lint changeNum, Lint changeReason, Lint goldSum)
{
	LMsgS2CGoldChange msg;
	msg.m_changeNum = changeNum;
	msg.m_changeReason = changeReason;
	msg.m_GoldNum = goldSum;
	Send(msg);
}

void User::HanderUserGetPlayerTask(LMsgC2SGetTask * msg)
{
	if (msg == NULL)
		return;
	LMsgS2CTaskList ret;
	if (msg->m_taskId != 0xFFFF)
	{
		TaskBasicInfo info;
		ret.m_tasks.push_back(info);
	}
	else
		ret.m_tasks = m_taskMgr.GetTaskList();
	Send(ret);
}

void User::HanderGetUserInfo(LMsgC2SGetUserInfo * msg)
{
	LMsgS2CUserInfo ret;
	ret.m_error = ErrorCode::UserNotExist;
	UserIdInfo* idInfo = gUserManager.GetUserIdInfo(msg->m_userId);
	if (idInfo != NULL)
	{
		ret.m_user.m_id = idInfo->m_id;
		ret.m_user.m_unionId = idInfo->m_unionId;
		ret.m_user.m_nickName = idInfo->m_nike;
		ret.m_user.m_headImgUrl = idInfo->m_headImageUrl;
		ret.m_error = ErrorCode::ErrorNone;
	}
	Send(ret);
}

void User::HanderUserRequest_Share_Card(LMsgC2SRequest_Share_Card * msg)
{
	if (msg == NULL || gConfig.GetShareGet1Card() == 0 || msg->m_activeID != DAILY_SHARE_CARD || !m_bActiveDBResponse)
	{
		return;
	}

	Lint date = gWork.GetCurTime().GetDate();
	LMsgS2CRet_Share_Card finish;
	finish.m_date = date;

	for (auto itor = m_Active.begin(); itor != m_Active.end(); ++itor)
	{
		if ((itor)->m_activeID == DAILY_SHARE_CARD && (itor)->m_LasJoinDate >= date)
		{
			finish.m_playerState = 1;
			Send(finish);
			return;
		}
	}
	LActive active;
	active.m_activeID = DAILY_SHARE_CARD;
	active.m_LasJoinDate = date;
	active.m_RewardID = 3;
	m_Active.push_back(active);
	LMsgL2LDBSyncDraw sync;
	sync.m_activeID = DAILY_SHARE_CARD;
	sync.m_userId = GetUserDataId();
	sync.m_date = date;
	sync.m_giftID = 3;
	gWork.SendMsgToDb(sync);

	Lint nCardNums = 1;
	if (date >= 20170124 && date <= 20170202)
	{
		nCardNums = 2;
	}
	AddCardCount(CARD_TYPE_8, nCardNums, CARDS_OPER_TYPE_ACTIVE, "system");
	finish.m_playerState = 2;
	finish.m_nCardNum = nCardNums;
	Send(finish);
	LLOG_ERROR("User::ShareGet1Card %d", m_userData.m_id);
	return;
}

//add by arena

//add end 

// 工会服务器通知创建房间 [2017-7-23 willing]
void User::HanderTeamCreateDesk(LMsgTS2LMG_CreateDesk* msg)
{
	//兼容不赋值人数的桌子
	const int defPlayerNum = 4;
	//Lint ptPlayerNum = GetRoomPlayerNum(msg->m_state);
	Lint assignedPlayerNum = msg->m_playerNum;
	//Lint playerNum = ptPlayerNum < defPlayerNum ? ptPlayerNum : assignedPlayerNum > 1 ? assignedPlayerNum : defPlayerNum;
	//根据玩家数目,在小玩法里标识玩家数目

	/*std::vector<Lint>& playtype = msg->m_playtype;
	PlayTypeAppendPlayerNum(playerNum, playtype);*/


	LMsgLMG2LCreateDesk send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_geteUserId = GetUserGateUserId();
	send.m_ip = GetIp();
	send.m_usert = m_userData;
	send.m_flag = msg->m_flag;
	send.m_secret = msg->m_secret;
	send.m_gold = msg->m_gold;
	send.m_gameType = msg->m_state;
	send.m_state = msg->m_state; // 游戏类型(对应宏定义 GameType)
	send.m_nMaxFanshu = msg->m_nMaxFanshu;
	send.m_robotNum = msg->m_robotNum;
	send.m_black3first = msg->m_black3first;
	send.m_lenth = msg->m_lenth;
	send.m_extValue = msg->m_extValue;
	/*send.m_playTypeCount = msg->m_playtype.size();
	send.m_playtype = msg->m_playtype;*/
	if (msg->m_proportionate == 0) msg->m_proportionate = 1;
	send.m_proportionate = msg->m_proportionate;
	memcpy(send.m_cardValue, msg->m_cardValue, sizeof(send.m_cardValue));

	send.m_maYouDesk = msg->m_maYouDesk;
	send.m_useTeamCard = msg->m_useTeamCard;
	send.m_teamID = msg->m_teamID;
	send.m_teamName = msg->m_teamName;
	send.m_free = msg->m_free;
	if (msg->m_circleGoldLimit == 0) msg->m_circleGoldLimit = 1;
	send.m_circleGoldLimit = msg->m_circleGoldLimit;
	send.m_stake = msg->m_stake;
	//send.m_playerNum = playerNum;
	LogicInfo* logic = gWork.GetLogicInfoById(getUserLogicID());	//获取玩家当前的逻辑
	if (logic == NULL)
	{
		logic = gWork.GetLogicInfoByPresure(msg->m_state);						//获取负载最小的逻辑
		LLOG_ERROR("HanderTeamCreateDesk logic == NULL");
	}
	else //if( msg->m_maYouDesk == 1)
	{
		LLOG_ERROR("User::HanderTeamCreateDesk Current ServerID Not Null %d", getUserLogicID());
		LMsgLMG2TS_RecycleDeskId RecycleDeskIdMsg;
		RecycleDeskIdMsg.nDeskId = msg->m_deskId;
		gWork.SendMessageToTeamServer(RecycleDeskIdMsg);
		gWork.m_TeamServerDeskSet.erase(msg->m_deskId);
		// 如果创建的不是免费房间，需要把刚才扣掉的卡还回去 [2017-8-1 willing]
		if (msg->m_free == 0)
		{
			LMsgLMG2TS_GiveBackCard giveBackCard;
			giveBackCard.nTeamID = msg->m_teamID;
			giveBackCard.nFlag = msg->m_flag;
			giveBackCard.nUserID = m_userData.m_id;
			giveBackCard.nFastPlay = msg->m_maYouDesk;
			gWork.SendMessageToTeamServer(giveBackCard);
		}
		//玩家重新登陆到桌子上
		LMsgLMG2LLogin msg;
		msg.m_userid = m_userData.m_id;
		msg.m_gateId = GetUserGateId();
		msg.m_geteUserId = GetUserGateUserId();
		msg.m_ip = getUserIPStr();
		msg.m_buyInfo = gWork.GetBuyInfo(m_userData.m_id);
		msg.m_hide = gWork.GetHide();
		msg.m_card_num = m_userData.m_numOfCard2s;
		logic->m_sp->Send(msg.GetSendBuff());
		return;
	}

	if (msg->m_circleGoldLimit == 0) msg->m_circleGoldLimit = 1;
	//机器人不判断钱
	if (logic && !IfCardEnough(CARD_TYPE_8, msg->m_circleGoldLimit) && !(msg->m_robotNum >= 2))
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 5;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderLMGUserCreateDesk m_circleGoldLimit IfCardEnough!");
		LMsgLMG2TS_RecycleDeskId RecycleDeskIdMsg;
		RecycleDeskIdMsg.nDeskId = msg->m_deskId;
		gWork.SendMessageToTeamServer(RecycleDeskIdMsg);
		// 如果创建的不是免费房间，需要把刚才扣掉的卡还回去 [2017-8-1 willing]
		if (msg->m_free == 0)
		{
			LMsgLMG2TS_GiveBackCard giveBackCard;
			giveBackCard.nTeamID = msg->m_teamID;
			giveBackCard.nFlag = msg->m_flag;
			giveBackCard.nUserID = m_userData.m_id;
			giveBackCard.nFastPlay = msg->m_maYouDesk;
			gWork.SendMessageToTeamServer(giveBackCard);
		}

		return;
	}

	if (logic == NULL)
	{
		LMsgS2CCreateDeskRet ret;
		ret.m_errorCode = 4;
		Send(ret);

		LLOG_ERROR("ERROR: User::HanderLMGUserCreateDesk Logic Not Exist!");
		LMsgLMG2TS_RecycleDeskId RecycleDeskIdMsg;
		RecycleDeskIdMsg.nDeskId = msg->m_deskId;
		gWork.SendMessageToTeamServer(RecycleDeskIdMsg);
		// 如果创建的不是免费房间，需要把刚才扣掉的卡还回去 [2017-8-1 willing]
		if (msg->m_free == 0)
		{
			LMsgLMG2TS_GiveBackCard giveBackCard;
			giveBackCard.nTeamID = msg->m_teamID;
			giveBackCard.nFlag = msg->m_flag;
			giveBackCard.nUserID = m_userData.m_id;
			giveBackCard.nFastPlay = msg->m_maYouDesk;
			gWork.SendMessageToTeamServer(giveBackCard);
		}

		return;
	}
	if (/*msg->m_maYouDesk == 1 &&*/ msg->m_deskId > 0)
	{
		//快速组局的玩家id与logicID的对应关系需要初始化
		gDeskManager.HanderFastDeskID(msg->m_deskId, logic->m_id);
		/*if (IsFreeNow(0, msg->m_state))
		{
			send.m_free = 1;
			LMsgLMG2TS_GiveBackCard giveBackCard;
			giveBackCard.nTeamID = msg->m_teamID;
			giveBackCard.nFlag = msg->m_flag;
			giveBackCard.nFastPlay = msg->m_maYouDesk;
			gWork.SendMessageToTeamServer(giveBackCard);
		}*/
		send.m_deskID = msg->m_deskId;
	}
	else
		send.m_deskID = gDeskManager.GetFreeDeskId(logic->m_id);

	logic->m_deskCount++;		//逻辑桌子数加一
	gWork.m_TeamServerDeskSet.insert(send.m_deskID);
	logic->m_sp->Send(send.GetSendBuff());
}

Lint User::GetApplicableId_(std::vector<Lint>& deskIds, std::vector<Lint>& limitGolds)
{
	Lint retid = 0;
	if (deskIds.size() != limitGolds.size())
	{
		return retid;
	}
	
	Lint desksz = deskIds.size();
	for (Lint i = desksz - 1; i >= 0; --i)
	{
		if (limitGolds[i] >= m_userData.m_numOfCard2s);
		{
			retid = deskIds[i];
			break;
		}
	}

	return retid;
}

void User::HanderTeamAddDesk(LMsgTS2LMG_AddDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	/*if (IsInMatch())
	{
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = msg->m_deskId;
		ret.m_errorCode = 100;
		Send(ret);
		LLOG_DEBUG("ERROR: User::HanderUserAddDesk InMatch!");
		return;
	}*/

	Lint deskId = GetApplicableId_(msg->m_deskIds, msg->m_limitGolds);
	if (deskId == 0)
	{
		LLOG_DEBUG("User::HanderTeamAddDesk not sutibel %d", GetUserGateUserId());
		LMsgS2CAddDeskRet ret;
		ret.m_deskId = deskId;
		ret.m_errorCode = 101;
		Send(ret);
		return;
	}

	LLOG_DEBUG("User::HanderTeamAddDesk %d", deskId);
	LMsgLMG2LAddToDesk send;
	send.m_userid = m_userData.m_id;
	send.m_gateId = GetUserGateId();
	send.m_geteUserId = GetUserGateUserId();
	send.m_deskID = deskId;
	send.m_ip = GetIp();
	send.m_usert = m_userData;
	send.m_location = msg->m_location;
	LogicInfo* logic = gWork.GetLogicInfoById(getUserLogicID());	//获取玩家当前的逻辑
	if (logic == NULL)
	{
		Lint logicID = gDeskManager.getServerIDbyDeskID(deskId);
		logic = gWork.GetLogicInfoById(logicID);
	}
	else
	{
		LLOG_ERROR("User::HanderUserAddDesk Current ServerID Not Null %d", getUserLogicID());
	}
	if (logic == NULL)
	{
		LMsgLMG2TS_RecycleDeskId RecycleDeskIdMsg;
		RecycleDeskIdMsg.nDeskId = deskId;
		gWork.SendMessageToTeamServer(RecycleDeskIdMsg);

		LMsgS2CAddDeskRet ret;
		ret.m_deskId = deskId;
		ret.m_errorCode = 2;
		Send(ret);

		LLOG_DEBUG("ERROR: User::HanderUserAddDesk Logic Not Exist!");
		return;
	}

	logic->m_sp->Send(send.GetSendBuff());
}
//MaYouQuan end

void User::HanderTeamReqExchageGold(LMsgTS2LMG_ReqExchageGold* msg)
{
	if (msg == NULL)
	{
		return;
	}

	if (msg->m_goldNum <= 0)
	{
		LLOG_ERROR("HanderTeamReqExchageGold m_goldNum %d", msg->m_goldNum);
		return;
	}

	if (msg->m_toUserId == GetUserDataId())
	{
		LMsgS2CReqExchageGold ret;
		ret.nResCode = 3;
		Send(ret);
		LLOG_ERROR("HanderTeamReqExchageGold nofind user %d", msg->m_toUserId);
		return;
	}

	UserIdInfo * toUserInfo = gUserManager.GetUserIdInfo(msg->m_toUserId);
	if (!toUserInfo)
	{
		LMsgS2CReqExchageGold ret;
		ret.nResCode = 2;
		Send(ret);
		LLOG_ERROR("HanderTeamReqExchageGold nofind user %d", msg->m_toUserId);
		return;
	}

	if (!IfCardEnough(CARD_TYPE_8, msg->m_goldNum))
	{
		LMsgS2CReqExchageGold ret;
		ret.nResCode = 1;
		Send(ret);
		LLOG_ERROR("HanderTeamReqExchageGold %d need %d ", m_userData.m_numOfCard2s, msg->m_goldNum);
	}

	//兑换
	DelCardCount(CARD_TYPE_8, msg->m_goldNum, CARDS_OPER_TYPE_REQ_EXCHAGEGOLD, "admin");

	LMsgLMG2TS_ReqExchageGold sendmsg;
	sendmsg.m_logMsg.nFromUserId = GetUserDataId();
	sendmsg.m_logMsg.nToUserId = msg->m_toUserId;
	sendmsg.m_logMsg.nGoldNum = msg->m_goldNum;
	sendmsg.m_logMsg.sFromName = m_userData.m_nike;
	sendmsg.m_logMsg.sToName = toUserInfo->m_nike;
	sendmsg.m_logMsg.nReqTime = gWork.GetCurTime().Secs();
	gWork.SendMessageToTeamServer(sendmsg);
}

void User::HanderTeamUserLogin(LMsgTS2LMG_UserLogin* msg)
{
	if (msg == NULL)
	{
		return;
	}

	//正常登陆
	if (msg->nResCode == 0)
	{
		m_userData.m_teaminfo = msg->teaminfo;
		Lint inlogic = 0;
		Login(inlogic);
		if (!inlogic)
		{
			SendLoginInfo(0, m_userData.m_onUseTeamId, msg->sOnUseTeamName);
		}
		S2CeUserLogin();
		return;
	}

	if (msg->nOnUseTeamId == 0)
	{
		return;
	}

	//异常登陆
	m_userData.m_onUseTeamId = msg->nOnUseTeamId;
	SendLoginInfo(msg->nResCode, msg->nOnUseTeamId, msg->sOnUseTeamName);

	//这里玩家异常登陆
	LLOG_ERROR("Work::HanderTeamUserLogin user error login %d:%d error %d", GetUserDataId(), GetUserGateId(), msg->nResCode);
	LMsgL2GUserOutMsg send;
	send.m_id = GetUserGateUserId();
	GateInfo* gateInfo = gWork.GetGateInfoById(GetUserGateId());
	if (gateInfo) gateInfo->m_sp->Send(send.GetSendBuff());
}
