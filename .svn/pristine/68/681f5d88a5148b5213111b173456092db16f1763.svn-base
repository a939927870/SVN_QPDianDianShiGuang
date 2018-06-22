#include "Robot.h"
#include "Card.h"
#include "Desk.h"
#include "LMsg.h"
Robot::Robot(LUser& data, Lint gateId, Lint gateUserId):User(data, gateId, gateUserId), m_robotShowId(0)
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
		if (pos != INVAILD_POS)
		{
			switch (m_desk->getDeskState())
			{
			case DESK_FREE:
				{

				}
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
						for (int i = 0; i < m_desk->m_desk_user_count; i ++)
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
								m_desk->HanderSelectResutDesk(this, &msg);
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

Lint Robot::GetUserDataId(bool bRandId)
{
	if (!bRandId) return m_userData.m_id;
	if (m_robotShowId == 0)
	{
		m_robotShowId = (L_Rand(1, 100000) + L_Rand(1, 1000) + L_Rand(1, 1000000));
	}
	return m_robotShowId;
}

bool Robot::IsRobot()
{
	return true;
}
