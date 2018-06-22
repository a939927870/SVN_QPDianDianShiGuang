#include "AlertWork.h"
#include "LMsg.h"
#include "HttpRequest.h"
#include "LLog.h"
#include "Config.h"

CAlertWork::CAlertWork()
{
}


CAlertWork::~CAlertWork()
{
}

bool CAlertWork::Init()
{
	return true;
}

bool CAlertWork::Final()
{
	return true;
}

void CAlertWork::Start()
{
	LRunnable::Start();
	m_gcService.Start();
}

void CAlertWork::Run()
{
	while (!GetStop())
	{
		LMsg* msg = (LMsg*)Pop();
		if (msg == NULL)
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

void CAlertWork::Clear()
{

}

void CAlertWork::Join()
{
	LRunnable::Join();
}

void CAlertWork::Stop()
{
	m_gcService.Stop();
	LRunnable::Stop();
}

void CAlertWork::HanderMsg(LMsg* msg)
{
	if ( NULL == msg )
	{
		return;
	}

	switch (msg->m_msgId)
	{
	case MSG_TEAM_CARD_ALERT:
		HanderTeamCardAlert((LMsgTeamCardAlert*)msg);
		break;
	default:
		LLOG_ERROR("CAlertWork::HanderMsg -- Wrong msg. msgid = %d %x", msg->m_msgId, msg->m_msgId);
		break;
	}
}

void CAlertWork::HanderTeamCardAlert(LMsgTeamCardAlert* msg)
{
	Lstring strAlertUrl = gConfig.GetAlertUrl();

	std::stringstream strStream;
	strStream << strAlertUrl << "?createrId=" << msg->nCreaterID << "&teamId=" << msg->nTeamID << "&teamName=" << msg->strTeamName;

	LLOG_ERROR("CAlertWork::HanderTeamCardAlert CreateID=%d,TeamID=%d", msg->nCreaterID, msg->nTeamID);

	Lstring strResult = "";
	if (HttpRequest::Instance().HttpPost(strStream.str(), "", strResult))
	{
		if (strResult.find("errorCode\":0") == string::npos)
		{
			LLOG_ERROR("CAlertWork::HanderTeamCardAlert server error:%s",strResult.c_str());
		}
	}
	else
	{
		LLOG_ERROR("CAlertWork::HanderTeamCardAlert send alert fail");
	}
}