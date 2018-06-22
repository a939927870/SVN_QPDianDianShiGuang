#ifndef _ROOM_VIP_H
#define _ROOM_VIP_H

#include "LMsg.h"
#include "Desk.h"
#include "LVipLog.h"

//牌局类型
enum EPaiJuType
{
	PJT_HAO_YOU_ZHUO = 1, //好友桌
	PJT_JIN_BI_CHANG = 2, //金币场
	PJT_BI_SAI_CHANG = 3, //比赛场
};

//把数单位
enum EBaShuUnit
{
	BSU_JU = 1,		//局
	BSU_QUAN = 2,	//圈
	BSU_GUO = 3,	//锅
};

struct VipLogItem :public LVipLogItem
{
public:
	Desk*			m_desk;
public:
	VipLogItem();
	~VipLogItem();
	Lint		GetOwerId();
	
	void		AddTuiDaoLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo,Lint zhuangPos,Lint* agang,Lint* mgang, Lint *bomb,Lstring& videoId, Lint* maNum, Lint *ghostZimo, Lint* noghostZimo);
	void		AddJiPingLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo,Lint zhuangPos,Lint* agang,Lint* mgang, Lint *bomb,Lint *fanshu, Lint* baohu, Lstring& videoId);
	void		AddZhuanZhuanLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo,Lint zhuangPos,Lint* agang,Lint* mgang, Lint *bomb,Lstring& videoId, Lint* maNum);
	void		AddJokerChickenLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo,Lint zhuangPos,Lint* agang,Lint* mgang, Lint *bomb,Lstring& videoId, Lint* maNum, Lint *normal, Lint* singleflower);
	void		AddChaoZhouLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo,Lint zhuangPos,Lint* agang,Lint* mgang, Lint *bomb,Lstring& videoId, Lint* maNum);
	void		AddHongkongLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lstring& videoId, Lint* maNum);
	void		AddHongZhongLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lint *fanshu, Lint *zimo, Lint maNum[], Lstring& videId);
	void		AddJieyangLog(User** user, Ldouble* gold, Ldouble* cal_gold, Lint* winInfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lint *bomb, Lstring& videoId, Lint* maNum);
	bool		ExiestUser(Lint id);

	void		InsertToDb();

	void		UpdateToDb();

	bool		AddUser(User* user);

	Lint		GetUserPos(User* user);

	Ldouble		GetUserScore(User* user);
	void		SetUserScore(User* user, Ldouble nScore);

	bool		isEnd();
	void		onCircleEnd(User** user);
	void		ReportHorseInfo2LogicManager(User** user, Lint* maNum);

	bool		IsFull(User* user);
	//比赛场
	void		Report2MatchServer(const LMsgS2CVipEnd & endmsg);
	void		Report2MatchServer(const LMsgS2CGameOver & endmsg);

	void		SendEnd();

	void		SendInfo();

	bool		IsBegin();

	void		RemoveUser(Lint id);

	int         GetCurrDifen()const { return m_nCurDifen; }
	void        SetCurrDifen();
	Lint		GetAppId()const {return m_nAppId;}
	void		SetAppId(Lint appId) { m_nAppId = appId; }

	/*远程日志相关begin*/

	//牌局类型 1:好友桌 2:金币场 3:比赛场
	EPaiJuType GetPaiJuType();
	//把数单位 1:局 2:圈 3:锅
	EBaShuUnit GetBaShuUnit();
	//应扣房卡数
	Lint GetCardCountToDel();
	//实扣房卡数
	Lint GetCardCountRealDel();
	//设置实际扣卡数
	void AddCardCountRealDel(Ldouble nCardDel);
	/*远程日志相关end*/

private:
	Lint	m_nCardDel = 0; //实际扣卡数
	Lint	m_nAppId = 0;

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


//vip房间
class RoomVip:public LSingleton<RoomVip>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	void			Tick(LTime& cur);

	Desk*			GetFreeDesk(Lint nDeskID, Lint nAppId);

	Desk*			GetDeskById(Lint id);

	bool			CreateVipDesk(LMsgLMG2LCreateDesk* pMsg,User* pUser);
	//比赛场
	VipLogItem*		CreateMatchDesk(LMsg_Match2L_CreateMatchDesk* pMsg);
	
	bool			AddToVipDesk(User* pUser, Lint nDeskID);

	bool			LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser);
	
private:
	DeskMap		m_deskMapUseing;
	DeskVec		m_deskMapFree;

	Lint		m_curDeskId;

	std::queue<Lint>  m_deskId;
};

#define gRoomVip RoomVip::Instance()



#endif