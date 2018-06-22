#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"
#include "LTool.h"
#include "BaseCommonFunc.h"


bool OutsideNet::Init()
{
	LNet::Init(gConfig.GetInsideIp(), gConfig.GetInsidePort(), 4, true, false);
	return true;
}

bool OutsideNet::Final()
{
	return true;
}

void OutsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	LMsg* msg = ParseInsideNetPacket(recv, s);
	if (msg)
		gWork.Push(msg);
}


//////////////////////////////////////////////////////////////////////////

bool GmNet::Init()
{
	LNet::Init(gConfig.GetGMIp(), gConfig.GetGMPort(), 1, true, true);
	return true;
}

bool GmNet::Final()
{
	return true;
}

void GmNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0;
	recv->Read(msgid);
	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
	if (msg != NULL)
	{
		msg->Read(*recv.get());
		msg->m_sp = s;
		gWork.Push(msg);
		LLOG_DEBUG("GmNet::RecvMsgPack recv msgid=%d", msgid);
	}
	else
	{
		LLOG_ERROR("GmNet::RecvMsgPack error msgId=%d", msgid);
	}
}
