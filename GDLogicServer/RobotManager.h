#ifndef _ROBOTMANAGER_H
#define _ROBOTMANAGER_H
#include "LSingleton.h"
#include "Robot.h"
class RobotManager :public LSingleton<RobotManager>
{
public:
	RobotManager(void);
	~RobotManager(void);

	virtual	bool	Init();
	virtual	bool	Final();
	bool	Init(const char* filename);
	bool	AddRobotToDesk(Lint nDeskIndex, Lint nPlayerCount, Lint appId);
	Lint	getFree();
private:
	std::vector<Robot*>		m_RobotList;
};
#define gRobotManager RobotManager::Instance()


#endif
