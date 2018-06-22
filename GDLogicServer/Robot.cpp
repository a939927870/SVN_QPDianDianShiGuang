#include "Robot.h"
#include "Card.h"
#include "Desk.h"
#include "LMsg.h"
Robot::Robot(LUser& data, Lint gateId,Llong gateUserId):User(data, gateId, gateUserId), mIndex(0)
{
	
}

Robot::~Robot(void)
{
}

void Robot::Tick()
{
	User::Tick();
	
	if (m_desk != NULL)
	{
		Lint pos = m_desk->GetUserPos(this);
		if (pos != DEFAULT_INVAILD_POS)
		{
			switch (m_desk->getDeskState())
			{
			case DESK_FREE:
				{

				}
				break;
			case DESK_WAIT:
				{
					if (m_desk->m_readyState[pos] != 1)
					{
						LMsgC2SUserReady msg;
						msg.m_pos = pos;
						m_desk->HanderUserReady(this, &msg);
					}
				}
				break;
			case DESK_PLAY:
				{
					//结束房间
					{
						bool bEndRoom = false;
						for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i ++)
						{
							if (m_desk->m_reset[i] == RESET_ROOM_TYPE_AGREE)
							{
								bEndRoom = true;
								break;
							}
						}
						if (bEndRoom)
						{
							if (m_desk->m_reset[pos] == RESET_ROOM_TYPE_WAIT)
							{
								LMsgC2SSelectResetDesk msg;
								msg.m_pos = pos;
								msg.m_flag = 1;
								m_desk->HanderSelectResetDesk(this, &msg);
							}	
							return;
						}	
					}			
					//处理机器人逻辑
					if (m_desk->mGameHandler)
					{
						m_desk->mGameHandler->ProcessRobot(pos, this);
					}
				}
				break;
			}			
		}
	}
}

void Robot::SetRobotIndex(Lint index)
{
	mIndex = index;
}

Lint Robot::GetRobotIndex()
{
	return mIndex;
}

bool Robot::IsRobot()
{
	return true;
}

