#include "User.h"
#include "Work.h"
#include "RoomVip.h"
#include "UserManager.h"
#include "Config.h"

User::User(LUser data, Lint gateId, Llong gateUserId)
:m_userData(data), m_userState(0)
{
	m_gateId = gateId;
	m_gateUserId = gateUserId;
	m_online = true;
	m_desk = NULL;
	m_Active.clear();
	m_bIsInTuoGuan = false;
}
	
User::~User()
{
}

void User::Tick()
{

}

//获取玩家数据库里id
Lint User::GetUserDataId()
{
	return m_userData.m_id;
}

//获取玩家唯一表示
Llong	User::GetUserUnique()
{
	Llong id = ((Llong)GetUserGateId() << 48 | (Llong)GetUserGateUserId());
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

void User::SetUserGateUserId(Llong gateUserId)
{
	m_gateUserId = gateUserId;
}

Llong User::GetUserGateUserId()
{
	return m_gateUserId;
}

bool User::GetOnline()
{
	return m_online;
}

bool User::Login()
{
	//这个方法要改，这样调用太LOW了
	bool b_login = false;
	m_userData.m_lastLoginTime = gWork.GetCurTime().Secs();
	m_online = true;

	if(GetDesk())
	{
		b_login = GetDesk()->OnUserReconnect(this);
	}
	return b_login;
}

void User::Logout()
{
	m_online = false;
	//m_gateId = 0;
	//m_gateUserId = 0;
	//Save();
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

void User::SendLoginInfo(Lstring& buyInfo, Lint hide)
{
	LMsgS2CLogin send;
	send.m_id = m_userData.m_id;
	send.m_errorCode = 0;
	send.m_nike = m_userData.m_nike;
	send.m_exp = 0;
	send.m_sex = m_userData.m_sex;
	send.m_new = m_userData.m_new;
	send.m_state = GetDesk() ? 1 : 0;
	send.m_ip = m_ip;
	send.m_face = m_userData.m_headImageUrl;
	send.m_buyInfo = buyInfo;
	send.m_hide = hide;
	send.m_gm = m_userData.m_gm;
	send.m_card1 = 0;
	send.m_card2 = 0;
	send.m_card3 = 0;
	send.m_nCurTime = gWork.GetCurTime().Secs();
	std::vector<Lint> gain_head_icon_vec = GetUserGainHeadIconId();
	send.m_gainHeadIconId = gain_head_icon_vec;
	send.m_curHeadIconId = m_userData.m_curHeadIconId;

	send.nState = LGU_STATE_DESK; // 查询玩家在该工会中的状态 [2017-7-29 willing]
	send.m_isInDesk = 1;		//是否加入了房间
	send.teamInfo = m_userData.m_teaminfo.teamInfo;//公会本身的信息

	Send(send);
}

const std::vector<Lint> User::GetUserGainHeadIconId() const
{
	Lstring strGainHeadIconId = m_userData.m_gainHeadIconId;
	std::vector<Lstring> strSplid;
	std::vector<Lint> gainHeadIconIdVec;
	L_ParseString(strGainHeadIconId, strSplid, ";");
	if (strSplid.size() > 0)
	{
		for (Lint j = 0; j < strSplid.size(); ++j)
		{
			gainHeadIconIdVec.push_back(atoi(strSplid[j].c_str()));
		}
	}
	return gainHeadIconIdVec;
}

void User::SendItemInfo()
{
	LMsgS2CItemInfo send;
	send.m_card1 = m_userData.m_numOfCard1s;
	send.m_card2 = m_userData.m_numOfCard2s;
	send.m_card3 = m_userData.m_numOfCard3s;
	Send(send);
}

void User::SetDesk(Desk* desk)
{
	m_desk = desk;
}

Desk* User::GetDesk()
{
	return m_desk;
}

void User::HanderMsg(LMsg* msg)
{
	switch (msg->m_msgId)
	{
	case MSG_C_2_S_READY:
		HanderUserReady((LMsgC2SUserReady*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM:
		HanderResetDesk((LMsgC2SResetDesk*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM_SELECT:
		HanderSelectResetDesk((LMsgC2SSelectResetDesk*) msg);
		break;
	case MSG_C_2_S_LEAVE_ROOM:
		HanderUserLeaveDesk((LMsgC2SLeaveDesk*) msg);
		break;
	case MSG_C_2_S_PLAY_CARD:
		HanderUserPlayCard((LMsgC2SUserPlay*) msg);
		break;
	case MSG_C_2_S_USER_OPER:
		HanderUserThink((LMsgC2SUserOper*) msg);
		break;
	case MSG_C_2_S_START_HU_SELECT:
		HanderStartHuSelect((LMsgC2SUserStartHuSelect*) msg);
		break;
	case MSG_C_2_S_END_SELECT:
		HanderEndCardSelect((LMsgC2SUserEndCardSelect*) msg);
		break;
	case MSG_C_2_S_USER_SPEAK:
		HanderUserSpeak((LMsgC2SUserSpeak*) msg);
		break;
	case MSG_C_2_S_INTERACTION:
		HanderUserInteraction((LMsgC2SInteraction*) msg);
		break;
	case MSG_C_2_S_HEART:
		HanderUserHeart((LMsgC2SHeart*) msg);
		break;
	case MSG_C_2_S_SIGNIN_REQ:
		HanderUserSignInReq((LMsgC2SSignInReq*)msg);
		break;
	case MSG_C_2_S_SIGNIN_INFO_REQ:
		HanderUserSignInfoReq((LMsgC2SSignInInfoReq*)msg);
		break;
	case MSG_C_2_S_TUOGUAN:
		HanderUserTuoGuan((LMsgC2S_TuoGuan*)msg);
		break;
	default:
		LLOG_ERROR("LogicServer Message Error: %d", msg->m_msgId);
		break;
	}
}

bool User::HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg)
{
	LLOG_DEBUG("User::HanderUserCreateDesk %d", msg->m_deskID);
	if (m_desk != NULL)
	{
		LLOG_ERROR("User::HanderUserCreateDesk %d", msg->m_deskID);
		return false;
	}

	return gRoomVip.CreateVipDesk(msg, this);
}

bool User::HanderUserAddDesk(Lint nDeskID)
{
	LLOG_DEBUG("User::HanderUserAddDesk %d", nDeskID);
	if (m_desk != NULL)
	{
		LLOG_ERROR("User::HanderUserAddDesk %d", nDeskID);
		return false;
	}

	return gRoomVip.AddToVipDesk(this, nDeskID);
}

void User::HanderUserReady(LMsgC2SUserReady*msg)
{
	if (m_desk)
	{
		m_desk->HanderUserReady(this, msg);
	}
}

void User::HanderResetDesk(LMsgC2SResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderResetDesk(this,msg);
	}
}

void User::HanderSelectResetDesk(LMsgC2SSelectResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderSelectResetDesk(this,msg);
	}
}

void User::HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg)
{
	gRoomVip.LeaveToVipDesk(msg,this);
}

void User::HanderUserPlayCard(LMsgC2SUserPlay* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserPlayCard(this,msg);
	}
}

void User::HanderUserThink(LMsgC2SUserOper* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserOperCard(this,msg);
	}
}

void User::HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserStartHu(this,msg);
	}
}

void User::HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg)
{
	if(m_desk)
	{
		m_desk->HanderUserEndSelect(this,msg);
	}
}

void User::HanderUserSpeak(LMsgC2SUserSpeak* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserSpeak(this, msg);
	}
}

void User::HanderUserInteraction(LMsgC2SInteraction* msg)
{
	if (m_desk)
	{
		m_desk->HanderUserInteraction(this, msg);
	}
}

void User::HanderUserHeart(LMsgC2SHeart* msg)
{
	LMsgS2CHeart send;
	send.m_time = gWork.GetCurTime().Secs();
	Send(send);
}

void User::HanderUserTuoGuan(LMsgC2S_TuoGuan* msg)
{
	bool update = false;
	if (msg->m_ReqType == JOIN_TUOGUAN)
	{
		if (GetTuoGuan()) return;
		SetTuoGuan(true);
		update = true;
	}
	else if (msg->m_ReqType == QUIT_TUOGUAN)
	{
		if (!GetTuoGuan()) return;
		SetTuoGuan(false);
		update = true;
	}
	if (update)
	{
		if (m_desk)
		{
			m_desk->HanderUserTuoGuan(this, msg);
		}
	}

}


void User::HanderUserSignInReq(LMsgC2SSignInReq* msg) {
	gWork.SendToLogicManager(*msg);
	return;
	
}
void User::HanderUserSignInfoReq(LMsgC2SSignInInfoReq* msg) {
	gWork.SendToLogicManager(*msg);
	return;
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
	gWork.SendToLogicManager(save);
}

void User::ModifyRoomCardCount(Ldouble nShift, Lint nReason, Lint nGameType, bool bAddDeskTotal/*=false*/)
{
	LLOG_INFO("User::DelCardCount type=%d,count=%d,nGameType=%d", nShift, nReason, nGameType);
	m_userData.m_numOfCard2s += nShift;
	if (bAddDeskTotal && m_desk && m_desk->m_vip)
	{
		m_desk->m_vip->AddCardCountRealDel(-nShift);
	}

	LMsgL2LMGModifyCard msg;
	msg.admin = "gdlogic";
	msg.cardType = CARD_TYPE_8;
	msg.cardNum = nShift;
	msg.isAddCard = bAddDeskTotal ? 1 : 0;
	msg.operType = nReason;
	msg.m_userid = m_userData.m_id;
 
	gWork.SendToLogicManager(msg);
}
void User::ModifyUserState(bool bLeaveServer)
{
	LMsgL2LMGModifyUserState msg_logic;
	msg_logic.m_userid = m_userData.m_id;
	msg_logic.m_logicID = bLeaveServer? 0 : gConfig.GetServerID();
	msg_logic.m_userstate = getUserState();
	gWork.SendToLogicManager(msg_logic);

	LMsgL2GModifyUserState msg_gate;
	msg_gate.m_gateid = GetUserGateUserId();
	msg_gate.m_logicID =  bLeaveServer? 0 : gConfig.GetServerID();
	msg_gate.m_userstate = getUserState();
	GateInfo* gateInfo = gWork.GetGateInfoById(GetUserGateId());
	if (gateInfo)
	{
		gateInfo->m_sp->Send(msg_gate.GetSendBuff());
	}
}

void User::setUserState(Lint nValue, Lint maYouQuan /*= 0*/)
{
	m_userState = nValue;
	Lint nPos = DEFAULT_DESK_USER_COUNT;
	if (m_desk)
	{
		nPos = m_desk->GetUserPos(this);
	}
	if (maYouQuan > 0)
	{
		// 把状态同步给 工会服务器 [2017-7-25 willing]
		LMsgLS2TS_GameState gameState;
		gameState.nIsInGame = (nValue == LGU_STATE_CENTER) ? 0 : 1;
		gameState.nUserID = GetUserDataId();
		gameState.nPos = nPos;
		gWork.SendMsgToTeamServer(gameState);
	}
}

bool User::IsRobot()
{
	return false;
}

