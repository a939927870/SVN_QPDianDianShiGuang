#include "Robot.h"
//#include "Card.h"
#include "Desk.h"
#include "LMsg.h"
#include "RoomVip.h"
Robot::Robot(LUser& data, Lint gateId,Lint gateUserId):User(data, gateId, gateUserId), mIndex(0)
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

		//结束房间;
		{
			bool bEndRoom = false;
			for (int i = 0; i < m_desk->m_nUserCounts; i ++)
			{
				if (m_desk->m_reset[i] == 1)
				{
					bEndRoom = true;
					break;
				}
			}
			if (bEndRoom)
			{
				if (m_desk->m_reset[pos] == 0)
				{
					LMsgC2SSelectResetDesk msg;
					msg.m_pos = pos;
					msg.m_flag = 1;
					m_desk->HanderSelectResutDesk(this, &msg);
				}	
				return;
			}	
		}	

		if (pos != INVAILD_POS)
		{
			switch (m_desk->m_deskState)
			{
			case DESK_FREE:
				{

				}
			case DESK_WAIT:
				{
					//优化，机器人已经准备不再准备;
					if (1 != m_desk->m_readyState[pos])
					{
						LMsgC2SUserReady msg;
						msg.m_pos = pos;
						m_desk->HanderUserReady(this, &msg);
					}
				}
				break;
			case DESK_PLAY:
				{
					m_desk->m_GameHandler->ProcessRobot(pos, this);
				}
				break;
			case QIANG_DIZHU:
				m_desk->m_GameHandler->ProcessRobot(pos, this);
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


