#include "User.h"
#include "Work.h"
#include "RoomVip.h"
#include "UserManager.h"
#include "Config.h"

User::User(LUser data, Lint gateId, Lint gateUserId)
:m_userData(data)
{
	m_gateId = gateId;
	m_gateUserId = gateUserId;
	m_online = true;
	m_desk = NULL;
	m_Active.clear();
	m_longitue = "0";     //经度
	m_latitude = "0";		//纬度
	m_rank = 0;
	m_score = 0;
	m_overScore = 0;
	m_sportId = 0;
	m_managed = false;
	m_process = 0;
}
	
User::~User()
{
}

void User::Tick()
{

}

//获取玩家数据库里id
Lint User::GetUserDataId(bool bRandId)
{
	return m_userData.m_id;
}

//获取玩家唯一表示
Llong User::GetUserUnique()
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
	send.m_serverTime = gWork.GetCurTime().Secs();

	//新加内容
	send.nState = LGU_STATE_DESK; // 查询玩家在该工会中的状态 [2017-7-29 willing]
	send.m_isInDesk = 1;		//是否加入了房间
	send.m_nTeamId = m_userData.m_teaminfo.nTeamID;
	send.m_sTeamName = m_userData.m_teaminfo.strTeamName;
	send.teamInfo = m_userData.m_teaminfo.teamInfo;//公会本身的信息

	Send(send);
}

void User::SendItemInfo()
{
	LMsgS2CItemInfo send;
	send.m_card1 = m_userData.m_numOfCard1s;
	send.m_card2 = m_userData.m_numOfCard2s;
	send.m_card3 = m_userData.m_numOfCard3s;
	send.m_diamondNum = m_userData.m_diamondNum;
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
	//std::cout<<"==========User::HanderMsg "<<msg->m_msgId<<std::endl;
	switch (msg->m_msgId)
	{
	case MSG_C_2_S_READY:
		HanderUserReady((LMsgC2SUserReady*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM:
		HanderResutDesk((LMsgC2SResetDesk*) msg);
		break;
	case MSG_C_2_S_RESET_ROOM_SELECT:
		HanderSelectResutDesk((LMsgC2SSelectResetDesk*) msg);
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
	case MSG_C_2_S_Piao_Select:
		HanderSelectPiao((LMsgC2SPiaoSelect*) msg);
		break;
	case MSG_C_2_S_CREATE_ROOM: //由于玩家状态来不急切换，消息会推送到这里
		HanderUserCreateDeskStateError((LMsgC2SCreateDesk*)msg);			//TO DO
		break;
	case MSG_C_2_S_ADD_ROOM:
		HanderUserAddDeskStateError((LMsgC2SAddDesk*)msg);
		break;
	//case MSG_C_2_S_VIP_LOG:
	//	//HanderUserVipLog((LMsgC2SVipLog*) msg);
	//	break;
	//case MSG_C_2_S_VIDEO: 
	//	//HanderUserVideo((LMsgC2SVideo*) msg);
	//	break;
	//case MSG_C_2_S_REQUEST_DRAW:
	//	//HanderUserDraw((LMsgC2SRequestDraw*) msg);
	//	break;
	//case MSG_C_2_S_WRITE_PHONE:
	//	//HanderWritePhone((LMsgC2SWritePhone*) msg);
	//	break;
	//case MSG_C_2_S_ACTIVE_LOG:
	//	//HanderUserReqActive((LMsgC2SActiveLog*) msg);
	//	break;
	//case MSG_C_2_S_NOTICE:

	//	break;
	case MSG_C_2_S_HEART:
		HanderUserHeart((LMsgC2SHeart*) msg);
		break;
	case MSG_C_2_S_CANCEL_MANAGED:
		HanderAttackManaged((LMsgC2SCancelManaged *) msg);
		break;
	case MSG_C_2_S_LOGIN: //如果重新登录，检查比赛场的内容
		CheckSportStatus(); 
		break;
	default:
		LLOG_ERROR("LogicServer Message Error: %d:%d:%d:%d", m_userData.m_id, msg->m_msgId, m_gateId, m_gateUserId);
		break;
	}
}

Lint User::HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg)
{
	LLOG_DEBUG("User::HanderUserCreateDesk %d", msg->m_deskID);
	return gRoomVip.CreateVipDesk(msg, this);
}

void User::HanderUserCreateDeskStateError(LMsgC2SCreateDesk* msg)
{
	LLOG_DEBUG("User::HanderUserCreateDeskStateError %d", GetUserDataId());
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 4;
	Send(ret);
}

Lint User::HanderUserAddDesk(Lint nDeskID)
{
	LLOG_DEBUG("User::HanderUserAddDesk %d", nDeskID);
	return gRoomVip.AddToVipDesk(this, nDeskID);
}

//玩家进入房间 异常回馈
void User::HanderUserAddDeskStateError(LMsgC2SAddDesk* msg)
{
	LLOG_DEBUG("User::HanderUserAddDeskStateError %d", GetUserDataId());
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = msg->m_deskId;
	ret.m_errorCode = 4;
	Send(ret);
}

void User::HanderUserReady(LMsgC2SUserReady*msg)
{
	if (m_desk)
	{
		m_desk->HanderUserReady(this, msg);
	}
}

void User::HanderResutDesk(LMsgC2SResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderResutDesk(this,msg);
	}
}

void User::HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg)
{
	if(m_desk)
	{
		m_desk->HanderSelectResutDesk(this,msg);
	}
}

void User::HanderSelectPiao(LMsgC2SPiaoSelect * msg)
{
	if (m_desk)
	{
		m_desk->HanderUserSelectPiao(this, msg);
	}
}

void User::HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg)
{
	gRoomVip.LeaveToVipDesk(msg,this);
}

void User::HanderUserPlayCard(LMsgC2SUserPlay* msg)
{
	if (isManaged())
	{
		LLOG_ERROR("HanderUserThink user is managed");
		SendManaged();
		return;
	}

	if(m_desk)
	{
		m_desk->HanderUserPlayCard(this,msg);
	}
}

void User::HanderUserThink(LMsgC2SUserOper* msg)
{
	if (isManaged())
	{
		LLOG_ERROR("HanderUserThink user is managed");
		SendManaged();
		return;
	}

	if(m_desk)
	{
		m_desk->HanderUserOperCard(this,msg);
	}
}

void User::HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg)
{
	if (isManaged())
	{
		LLOG_ERROR("HanderStartHuSelect user is managed");
		SendManaged();
		return;
	}

	if(m_desk)
	{
		m_desk->HanderUserStartHu(this,msg);
	}
}

void User::HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg)
{
	if (isManaged())
	{
		LLOG_ERROR("HanderEndCardSelect user is managed");
		SendManaged();
		return;
	}

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

void User::HanderUserHeart(LMsgC2SHeart* msg)
{
	LMsgS2CHeart send;
	send.m_time = gWork.GetCurTime().Secs();
	Send(send);
}

Lstring User::GetIp()
{
	return m_ip;
}

void User::SetIp(Lstring& ip)
{
	m_ip = ip;
}

// void User::Save()
// {
// 	LMsgL2CeSaveUser save;
// 	save.m_ID = gConfig.GetServerID();
// 	save.m_type = m_online?1:0;
// 	save.m_user = m_userData;
// 	gWork.SendToLogicManager(save);
// }

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

void User::ModifyUserState(bool bLeaveServer)
{
	LMsgL2LMGModifyUserState msg_logic;
	msg_logic.m_userid = m_userData.m_id;
	msg_logic.m_logicID = bLeaveServer? 0 : gConfig.GetServerID();
	msg_logic.m_userstate = getUserState();
	gWork.SendToLogicManager(msg_logic);
	gWork.SendToMatchServer(msg_logic);

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

void User::AddCardCount(Lint cardType, Ldouble count, Lint operType, const Lstring& admin, bool bNeedSave)
{
	LLOG_INFO("User::AddCardCount type=%d,count=%d,operType=%d", cardType, count, operType);
	Ldouble addCount = 0;
	switch (cardType)
	{
	case CARD_TYPE_4:
		m_userData.m_numOfCard2s += count / 2;
		m_userData.m_totalbuynum += count / 2;
		addCount = count / 2;
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
		addCount = count * 2;
		//m_userData.m_numOfCard3s += count;
		break;
	default:
		break;
	}

	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 1;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	gWork.SendToLogicManager(msg);
}

void User::DelCardCount(Lint cardType, Ldouble count, Lint operType, Lint nGameType, const Lstring& admin)
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

	LMsgL2LMGModifyCard msg;
	msg.admin = admin;
	msg.cardType = cardType;
	msg.cardNum = count;
	msg.isAddCard = 0;
	msg.operType = operType;
	msg.m_userid = m_userData.m_id;
	msg.m_nGameType = nGameType;
	gWork.SendToLogicManager(msg);
}

