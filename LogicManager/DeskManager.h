#ifndef _DESK_MANAGER_H
#define _DESK_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "LMsgS2S.h"

typedef std::map<Lint, Lint> DeskState;

struct WaitGoldDesk
{
	Lint m_deskID;    //等待分配玩家的金币场桌子ID;数组元素对应不同游戏类型;初始化和没有再分配桌子时为-1;
	Lint m_leftPlace; //正等待分配的金币场桌子的剩余空位;
};

class DeskManager:public LSingleton<DeskManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	void			RecycleDeskId(LMsgL2LMGRecyleDeskID* msg);
	void            RecycleGoldDeskPlace(LMsgL2LMGRecyleGoldDeskPlace* msg);
	void            UseGoldDeskPlace(LMsgL2LMGUseGoldDeskPlace* msg);
	Lint			GetFreeDeskId(Lint nServerID);

	Lint			getServerIDbyDeskID(Lint nDeskID);

	void			RecycleDumpServer(Lint nLogicID);

	Lint			GetShareVideoId();

	//金币场相关;
	Lint			GetFreeGoldDeskId(Lint nServerID, Lint gameStyle);

	Lint            GetFreeGoldDeskCounts();

	void			HanderTSReqDeskId(std::vector<int>& deskVec);
	void			HanderFastDeskID(Lint deskid, Lint nServerID);
private:
	std::queue<Lint>	m_FreeDeskList;
    std::queue<Lint>	m_sharevideoId;
	DeskState			m_DeskLogicID;

	std::vector<WaitGoldDesk>  m_vecWaitGoldDesk[game_style_sum];
};
#define gDeskManager DeskManager::Instance()

#endif