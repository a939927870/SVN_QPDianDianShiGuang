#include "User.h"
#include "Work.h"

void User::HanderUserArenaMSG(LMsg* msg)
{
	switch (msg->m_msgId)
	{
	case MSG_C_2_S_INTO_ARENA:
		HanderUserIntoArena((LMsgC2SIntoArena*) msg);
		break;
	case MSG_C_2_S_QUIT_ARENA:
		HanderUserQuitArena((LMsgC2SQuitArena*) msg);
		break;
	default:
		LLOG_ERROR("LogicManager Message Error: %d", msg->m_msgId);
		break;
	}
}

void User::HanderUserIntoArena(LMsgC2SIntoArena* msg)
{

}

void User::HanderUserQuitArena(LMsgC2SQuitArena* msg)
{

}