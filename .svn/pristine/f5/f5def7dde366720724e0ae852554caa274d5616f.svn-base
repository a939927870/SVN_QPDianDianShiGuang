#include "InsideNet.h"
#include "LMsg.h"
#include "Config.h"
#include "Work.h"
#include "LLog.h"


bool InsideNet::Init()
{
	LNet::Init(gConfig.GetLogicDBIp(), gConfig.GetLogicDBPort(), 8, true, false);
	return true;
}

bool InsideNet::Final()
{
	return true;
}

void InsideNet::RecvMsgPack(LBuffPtr recv, LSocketPtr s)
{
	int msgid = 0 ;
	recv->Read(msgid);
	LMsg* msg = LMsgFactory::Instance().CreateMsg(msgid);
	if(msg != NULL)
	{
		msg->Read(*recv.get());
		msg->m_sp = s;
		gWork.Push(msg);
	}
	else
	{
		LLOG_ERROR("UserNet::RecvMsgPack error msgId=%d",msgid);
	}
}