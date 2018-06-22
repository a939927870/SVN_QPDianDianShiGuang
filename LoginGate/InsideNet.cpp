#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "LTool.h"
#include "LSocket.h"
#include "BaseCommonFunc.h"

bool OutsideNet::Init()
{
	SetMsgHeaderChecker(boost::bind(&OutsideNet::CheckMsgHeader, this, boost::placeholders::_1, boost::placeholders::_2));
	LNet::Init(gConfig.GetOutsideIp(), gConfig.GetOutsidePort(), 4, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0;
	LMsg* msg = NULL;

	try{
		msgpack::unpacked  unpack;
		msgpack::unpack(&unpack, recv->Data() + recv->GetOffset(), recv->Size() - recv->GetOffset());
		msgpack::object obj = unpack.get();		
		ReadMapData(obj, "m_msgId", msgid);

		msg = LMsgFactory::Instance().CreateMsg(msgid);
		if(msg != NULL)
		{
			msg->Read(obj);
			msg->m_sp = s;
			gWork.Push(msg);
			//LLOG_DEBUG("InsideNet::RecvMsgPack user recv msgid=%d",msgid);
		}
		else
		{
			recv->Read(msgid);
			msg = LMsgFactory::Instance().CreateMsg(msgid);
			if (msg)
			{
				msg->Read(*recv.get());
				msg->m_sp = s;
				gWork.Push(msg);
				//LLOG_DEBUG("InsideNet::RecvMsgPack recv msgid=%d", msgid);
			}
			else
			{
				LLOG_ERROR("OutsideNet::RecvMsgPack error msgId=%d", msgid);
			}
		}
	}
	catch (...)
	{
		LLOG_ERROR("InsideNet::RecvMsgPack error");
	}
	if(msg == NULL)
	{
		DisableIP(s->GetRemoteIp());
		s->Kick();
	}
}

bool OutsideNet::CheckMsgHeader(LSocketPtr s, const MsgHeader& header)
{
	bool succeed = false;
	if (header.bodySize >= 64 && header.bodySize <= 2048)
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

bool OutsideNet::IsValidMsgId(Lint id)
{
	return (id >= MSG_C_2_S_MSG && id < MSG_S_2_C_MAX);
}
