#include "OutsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "LTool.h"
#include "LSocket.h"
#include "BaseCommonFunc.h"

bool OutsideNet::Init()
{
	LNet::Init(gConfig.GetOutsideIp(), gConfig.GetOutsidePort(), 4, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	try
	{
		Lint msgid = 0;
		recv->Read(msgid);
		LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
		if (msg)
		{
			msg->Read(*recv.get());
			msg->m_sp = s;
			gWork.Push(msg);
		}
		else
		{
			LLOG_ERROR("InsideNet::RecvMsgPack error msgId=%d", msgid);
		}
	}
	catch (...)
	{
		LLOG_ERROR("InsideNet::RecvMsgPack: catch error");
	}
}

//////////////////////////////////////////////////////////////////////////
bool HttpNet::Init()
{
	LNet::Init(gConfig.GetHttpListenIp(), gConfig.GetHttpListenPort(), 1, true, true);
	return true;
}

bool HttpNet::Final()
{
	return true;
}

void HttpNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0;
	recv->Read(msgid);
	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
	if (msg != NULL)
	{
		msg->Read(*recv.get());
		msg->m_sp = s;
		gWork.Push(msg);
	}
	else
	{
		LLOG_ERROR("HttpNet::RecvMsgPack error msgId=%d", msgid);
	}
}