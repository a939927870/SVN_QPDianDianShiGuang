#include "LActive.h"


LActive::LActive(void)
{
	m_RewardID = 0;	
	m_LasJoinDate = 0;
	m_activeID = 0;
}


LActive::~LActive(void)
{
}

void LActive::Read(LBuff& buff)
{
	buff.Read(m_activeID);
	buff.Read(m_RewardID);
	buff.Read(m_phoneNum);
	buff.Read(m_LasJoinDate);
}

void LActive::Write(LBuff& buff)
{
	buff.Write(m_activeID);
	buff.Write(m_RewardID);
	buff.Write(m_phoneNum);
	buff.Write(m_LasJoinDate);
}
