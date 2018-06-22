#ifndef _DESK_H_
#define _DESK_H_

#include "User.h"
#include "LTime.h"
#include "LVideo.h"
#include "gr_card.h"
#include "GameDefine.h"
#include "GameHandler.h"

class Room;
struct VipLogItem;


class Desk
{
public:
	Desk();
	~Desk();

	Lint			GetDeskId();
	void			SetVip(VipLogItem* vip);
	VipLogItem*		GetVip();

	void			Tick(LTime& curr);

	void			HanderUserReady(User* pUser,LMsgC2SUserReady* msg);
	void            HanderUserQiangDiZhu(User* pUser,LMsgC2SQiangDiZhu* msg);
	void            HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg);
	void			HanderUserPlayCard(User* pUser, LMsgC2SUserPlay* msg);

	void			HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg);

	void			HanderUserOutDesk(User* pUser);

	//申请解算房间;
	void			HanderResutDesk(User* pUser,LMsgC2SResetDesk* msg);

	void            HanderAskGoldDeskState(User* pUser,LMsgC2SAskGoldDeskState* msg);
	void            BoadCastGoldDeskEvent(Lint eventType, Lint pos);

	void            recycleDeskPlace(Lint deskID, Lint gameType);

	//金币场退出桌子;
	void            HanderQuitGoldDesk(User* pUser,LMsgC2SResetDesk* msg);
	void            unSetTimeOfWaitUser(Lint waitReason, Lint userPos);
	void            setTimeOfWaitUser(Lint waitReason, Lint userPos, Lint WaitTime);

	//玩家选择解算操作;
	void			HanderSelectResutDesk(User* pUser,LMsgC2SSelectResetDesk* msg);

	Lint			getDeskState();
	void			ResetEnd();
	bool			OnUserReconnect(User* pUser);

	void			OnUserInRoom(User* user);
	void            OnUserInGoldRoom(User* user);
	void			OnUserOutRoom(User* user);

	Lint			GetFreePos();
	Lint			GetUserPos(User* user);
	User*			GetPosUser(Lint pos);

	Lint			GetNextPos(Lint prev);
	Lint			GetPrevPos(Lint next);

	Lint			GetUserCount();
	
	void			SetDeskWait();
	void			OnDeskWait();

	void			CheckGameStart();
	void			DeakCard();//发牌;
	
	void			SetDeskFree();
	void			OnDeskFree();

	void			CheckReset();
	void			ClearUser();
	void			BoadCast(LMsg& msg);
	void			BoadCastWithOutUser(LMsg& msg,User* user);
	void			FinalEnd();	//打完一把;

	void			SetAllReady();
	void			ResetClear();

	bool            InitDesk(int iDeskId, PK_GAME_STYLE gameType);
	bool			HasRobot();	//是否有机器人;

	void            _clearDesk();

	void			SetTeamID(Lint nTeamID);
	Lint			GetTeamID() { return m_nTeamID; }

	void			SetTeamName(Lstring strTeamName);
	Lstring			GetTeamName() { return m_strTeamName; }

	void			SetProportionate(Lint nProportionate) { m_proportionate = nProportionate; }
	Lint			GetProportionate() { return m_proportionate; }
	void			SetStake(Lint nStake) { m_stake = nStake; }
	Lint			GetStake() { return m_stake; }

	void			SetCircleGoldLimit(Lint nCircleGoldLimit) { m_circleGoldLimit = nCircleGoldLimit; }
	Lint			GetCircleGoldLimit() { return m_circleGoldLimit; }

	bool			CheckRoom(LTime& curr);
	void			DelCard(Ldouble gold[4]);

protected:
	bool            _createRegister(PK_GAME_STYLE gameType);
	void            SwitchScoreToUser_(Ldouble gold[4]);
	bool            isNewPlay();
public:
	Lint			m_id;
	User*			m_user[DESK_USER_COUNT];
	Lint            m_nUserCounts;
	Lint            m_nCardCounts;

	PK_GAME_ZONE    m_gameZone;
	PK_GAME_STYLE   m_gameStyle;      // 游戏类型;
	GameHandler*    m_GameHandler;    // 具体游戏处理器;
	Lint            m_nMaxFanshu;      //游戏最大翻倍数（炸彈限制）; （-1為不限制，0，沒有炸彈（不翻倍），+n 炸弹上线（翻数上限）
	Lchar           m_readyState[DESK_USER_COUNT];//玩家准备状态;

	Lint			m_deskState;//桌子状态;
	Lint			m_state;    //房间类型;
	
	VipLogItem*		m_vip;
		
	Lint            m_resetPos;//申请解散房间玩家位置;
	Lint			m_reset[DESK_USER_COUNT];
	Lstring			m_resetUser;//申请的玩家;
	Lint			m_resetTime;//申请解算的时间;

	Lint            m_tiaopaishu;       //测试玩家调牌张数，0代表不调牌;
	CardValue       m_specialCard[56];  //存储玩家特定发牌牌局;

	VideoLog		m_video;//录像;

	std::vector<Lint>	m_extValue; //附加标志;

	//四川痞子;
	//BYTE			m_scoreType;	//翻倍2， 加点1, 0 无效;
	Lint			m_nTeamID;						// 所属工会 [2017-7-23 willing]
	Lstring			m_strTeamName;					// 工会名称 [2017-7-25 willing]

	Lint			m_proportionate = 0;            //抽成率
	Lint			m_circleGoldLimit = 0;          //每局进入的金币数
	Lint			m_stake = 0;                    //押注倍率

public:
	bool			CanGameStart();
	void            BrocastUserNoGold();
	void			BrocastUserNoGold(Lint exPos);
	Lint			getGameType();
private:
	Lint			m_waitChargeTime = 0;					//等待充值的时间

};

typedef std::map<DWORD,Desk*> DeskMap;
typedef std::queue<Desk*> DeskVec;
#endif