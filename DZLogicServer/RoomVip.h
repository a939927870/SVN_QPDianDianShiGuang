#ifndef _ROOM_VIP_H
#define _ROOM_VIP_H

#include "LMsg.h"
#include "Desk.h"
#include "LVipLog.h"

struct VipLogItem :public LVipLogItem
{
public:
	Desk*			m_desk;
public:
	VipLogItem();
	~VipLogItem();
	Lint		GetOwerId();
	
	void		AddLog(User** user, Ldouble* gold, Ldouble cal_gold[4], Lint* winInfo, Lint *bomb, Lstring& videoId);

	bool		ExiestUser(Lint id);

	void		InsertToDb();

	void		UpdateToDb();

	bool		AddUser(User* user);

	Lint		GetUserPos(User* user);

	Ldouble		GetUserScore(User* user);
	void		SetUserScore(User* user, Ldouble nScore);

	bool		isEnd();

	bool		isFinalEnd();	//一把打完

	bool		IsFull(User* user);

	void		SendEnd();

	void		SendInfo();

	bool		IsBegin();

	void		RemoveUser(Lint id);
};

class VipLogMgr :public LSingleton<VipLogMgr>
{
public:	
	virtual	bool	Init();
	virtual	bool	Final();

	void			SetVipId(Lint id);

	Lstring			GetVipId();

	void			SetVideoId(Lint id);

	Lstring			GetVideoId();

	VipLogItem*		GetNewLogItem(Lint maxCircle,Lint usrId);

	VipLogItem*		GetLogItem(Lstring& logId);

	VipLogItem*	FindUserPlayingItem(Lint id);//查询玩家正在进行中的桌子

	void		AddPlayingItem(VipLogItem* item);//添加未完成的桌子

	void		Tick();
private:
	std::map<Lstring,VipLogItem*> m_item;
	Lint	m_id;

	Lint	m_videoId;
};

#define gVipLogMgr VipLogMgr::Instance()


//vip房间;
class RoomVip:public LSingleton<RoomVip>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	void			Tick(LTime& cur);

	Desk*			GetFreeDesk(Lint nDeskID, PK_GAME_STYLE gameType);

	Desk*			GetDeskById(Lint id);

	Lint			CreateVipDesk(LMsgLMG2LCreateDesk* pMsg,User* pUser);

	Lint			AddToVipDesk(User* pUser, Lint nDeskID);

	Lint            IntoVipGoldDesk(LMsgLMG2LIntoGoldDesk* msg, User* pUser);

	bool			LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser);
	
private:
	DeskMap		m_deskMapUseing;
	DeskVec		m_deskMapFree;

	Lint		m_curDeskId;

	std::queue<Lint>  m_deskId;
};

#define gRoomVip RoomVip::Instance()



#endif