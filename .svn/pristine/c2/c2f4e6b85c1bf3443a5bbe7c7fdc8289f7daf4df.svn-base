#include "OutsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "BaseCommonFunc.h"


bool OutsideNet::Init()
{
	SetMsgHeaderChecker(boost::bind(&OutsideNet::CheckMsgHeader, this, boost::placeholders::_1, boost::placeholders::_2));
	CreateMsgIdTable();
	LNet::Init("0.0.0.0", gConfig.GetOutsidePort(), 4, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	bool succeed = false;
	int msgid = MSG_ERROR_MSG;
	
	try{
		msgpack::unpacked  unpack;
		msgpack::unpack(&unpack, recv->Data() + recv->GetOffset(), recv->Size() - recv->GetOffset());
		msgpack::object obj = unpack.get();
		ReadMapData(obj, "m_msgId", msgid);

		if ((msgid > MSG_C_And_T_Begin) && (msgid < MSG_C_And_T_End))
		{
			LMsgG2TUserMsg *msg = new LMsgG2TUserMsg();
			msg->m_userGateId = 0;
			msg->m_userMsgId = msgid;
			msg->m_dataBuff = recv;
			msg->m_ip = s->GetRemoteIp();
			msg->m_sp = s;
			gWork.Push(msg);
			succeed = true;
		}
		else
		if (msgid > MSG_ERROR_MSG && msgid < MSG_S_2_C_MAX)//玩家消息;
		{
			LMsgG2LUserMsg* msg = new LMsgG2LUserMsg();
			msg->m_userGateId = 0;
			msg->m_userMsgId = msgid;
			msg->m_dataBuff = recv;
			msg->m_ip = s->GetRemoteIp();
			msg->m_sp = s;
			gWork.Push(msg);
			succeed = true;
		}
		else
		{
			recv->Read(msgid);
			LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
			if (msg != NULL)
			{
				msg->Read(*recv.get());
				msg->m_sp = s;
				gWork.Push(msg);
				succeed = true;
				//LLOG_DEBUG("OutsideNet::RecvMsgPack recv msgid=%d", msgid);
			}
			else
			{
				//LLOG_ERROR("OutsideNet::RecvMsgPack error msgId=%d", msgid);
			}
		}

	}catch (...)
	{
		LLOG_ERROR("OutsideNet::RecvMsgPack error");
	}
	if(!succeed)
	{
		DisableIP(s->GetRemoteIp());
		s->Kick();
	}
}

bool OutsideNet::CheckMsgHeader(LSocketPtr s, const MsgHeader& header)
{
	bool succeed = false;
	if (header.bodySize >= 10 && header.bodySize <= 2048)
	{
		Lint msgId = InverseTransfromMsgId(header.timestamp, header.msgId);
		succeed = IsValidMsgId(msgId);
		if (!succeed)
		{
			LLOG_ERROR("msg header msgid error,msgid:%d", msgId);
		}
	}
	else
	{
		Lint msgId = InverseTransfromMsgId(header.timestamp, header.msgId);
		LLOG_ERROR("msg header bodySize error,msgid:%d,msgsize:%d", msgId, header.bodySize);
	}

	if (!succeed)
		DisableIP(s->GetRemoteIp());
	return succeed;
}

void OutsideNet::CreateMsgIdTable()
{
	m_msgIdTable.insert(MSG_C_2_S_MSG);
	m_msgIdTable.insert(MSG_C_2_S_RE_LOGIN);
	m_msgIdTable.insert(MSG_C_2_S_LOGIN);
	m_msgIdTable.insert(MSG_C_2_S_HEART);
	m_msgIdTable.insert(MSG_C_2_S_NOTICE);
	m_msgIdTable.insert(MSG_C_2_S_CREATE_ROOM);
	m_msgIdTable.insert(MSG_C_2_S_ADD_ROOM);
	m_msgIdTable.insert(MSG_C_2_S_LEAVE_ROOM);
	m_msgIdTable.insert(MSG_C_2_S_RESET_ROOM);
	m_msgIdTable.insert(MSG_C_2_S_RESET_ROOM_SELECT);
	m_msgIdTable.insert(MSG_C_2_S_READY);
	m_msgIdTable.insert(MSG_C_2_S_PLAY_CARD);
	m_msgIdTable.insert(MSG_C_2_S_USER_OPER);
	m_msgIdTable.insert(MSG_C_2_S_USER_SPEAK);
	m_msgIdTable.insert(MSG_C_2_S_USER_OUT_ROOM);
	m_msgIdTable.insert(MSG_C_2_S_START_HU_SELECT);
	m_msgIdTable.insert(MSG_C_2_S_END_SELECT);
	m_msgIdTable.insert(MSG_C_2_S_VIP_LOG);
	m_msgIdTable.insert(MSG_C_2_S_VIDEO);
	m_msgIdTable.insert(MSG_C_2_S_ONE_VIP_LOG);
	m_msgIdTable.insert(MSG_C_2_S_REQUEST_DRAW);
	m_msgIdTable.insert(MSG_C_2_S_WRITE_PHONE);
	m_msgIdTable.insert(MSG_C_2_S_ACTIVE_LOG);
	m_msgIdTable.insert(MSG_C_2_S_ACTIVE_INFO);
	m_msgIdTable.insert(MSG_C_2_S_SHAREVIDEO);
	m_msgIdTable.insert(MSG_C_2_S_REQSHAREVIDEO);
	m_msgIdTable.insert(MSG_C_2_S_GETSHAREVIDEO);

	//
	m_msgIdTable.insert(MSG_C_2_S_QIANG_DIZHU);
	m_msgIdTable.insert(MSG_C_2_S_QDZOPER_NEW);
	m_msgIdTable.insert(MSG_C_2_S_REQ_SIGN_COLLECT);
	m_msgIdTable.insert(MSG_C_2_S_REQ_SIGN_COLLECT_WIN_STATE);
	m_msgIdTable.insert(MSG_C_2_S_ASKGOLDDESK_STATE);
	//
	m_msgIdTable.insert(MSG_C_2_S_COUPONS_EXCHANGE);
	m_msgIdTable.insert(MSG_C_2_S_COUPONS_EXCHANGE_RECORD);
	m_msgIdTable.insert(MSG_C_2_S_GIFT_EXCHANGE);
	m_msgIdTable.insert(MSG_C_2_S_GET_TASK);
	m_msgIdTable.insert(MSG_C_2_S_RECV_TASK_REWARD);
	m_msgIdTable.insert(MSG_C_2_S_GET_USER_INFO);
	m_msgIdTable.insert(MSG_C_2_S_GET_INVITING_INFO);
	m_msgIdTable.insert(MSG_C_2_S_BIND_INVITER);
	m_msgIdTable.insert(MSG_C_2_S_USER_SHARED);
	m_msgIdTable.insert(MSG_C_2_S_REQ_LUCKY_DRAW_NUM);
	m_msgIdTable.insert(MSG_C_2_S_LOGIN_CENTER);
	m_msgIdTable.insert(MSG_C_2_S_LOGIN_SERVER);

	m_msgIdTable.insert(MSG_S_2_C_ACTIVE_OPEN); //转盘消息;
	m_msgIdTable.insert(MSG_C_2_S_REQUEST_SHARE_CARD); //分享活动;

	m_msgIdTable.insert(MSG_C_2_S_WEALTH_CHANGE); //道具兑换;
}

bool OutsideNet::IsValidMsgId(Lint id)
{
	return (id >= MSG_C_2_S_MSG && id < MSG_S_2_C_MAX);
}