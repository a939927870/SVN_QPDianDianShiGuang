#ifndef __TEAM_INFO_H__
#define __TEAM_INFO_H__
#include "LBase.h"
#include <map>
#include "GameDefine.h"

// 玩家在游戏中的状态枚举 [2017-7-14 willing]
enum E_TEAM_MEM_GATE_STATE
{
	ETMGS_OFFLINE,					// 离线 [2017-7-14 willing]
	ETMGS_ONLINE,					// 在线 [2017-7-14 willing]
	ETMGS_GAME,						// 游戏中 [2017-7-14 willing]

	ETMGS_Count
};

// 玩家在公会中的状态 [2017-7-14 willing]
enum E_TEAM_MEM_TEAM_STATE
{
	ETMTS_NOT_EXIST,
	ETMTS_JOIN, //已经加入
	ETMTS_NORMAL, //正常状态
	ETMTS_LEAVE, //正在离开

	ETMTS_Count
};
//工会快速组局 玩法配置
struct STeamPlayConfig
{
	Lint				m_id;
	Lint				m_roomType;				//房间类型  8局  16局
	Lint				m_playEnum;				//玩法枚举  
	Lint				m_teamID;				//工会ID
	Lint				m_userID;				//玩家ID
	std::vector<Lint>	m_playtype;				//玩法
	Lint				m_time;					//创建时间
	Lint                m_circleGoldLimit = 0;
	STeamPlayConfig(): m_roomType(0), m_playEnum(0), m_teamID(0),m_time(0),m_userID(0),m_id(0)
	{

	}

	// 存储 [2017-7-17 willing]
	void	SaveTeamPlayConfigInfo(Lint time);

	// 成员信息存库 [2017-7-20 willing]
	void	UpdateTeamPlayConfigInfo(Lint time);

	// 删除成员 [2017-7-20 willing]
	void	RmvTeamPlayConfigInfo(Lint nUserID);
};
typedef std::map<Lint, STeamPlayConfig>	tTeamPlayConfigMap;
// 工会成员信息结构 [2017-7-14 willing]
struct STeamMember
{
	Lint	nTeamID = 0;				// 所在工会ID [2017-7-14 willing]
	Lint	nUserID = 0;				// 玩家UserID [2017-7-14 willing]
	Lstring strUserName;			// 玩家名称 [2017-7-21 willing]
	Lstring strHeadImageUrl;		// 玩家头像的URL [2017-7-14 willing]
	Lint	nTodayUserCardNum = 0;		// 当天已经使用的工会房卡数量 [2017-7-14 willing]
	Lint	nGameState = 0;				// 玩家在游戏中的状态 [2017-7-14 willing]
	Lint	nTeamState = 0;				// 玩家在公会中的状态 [2017-7-14 willing]
	Lint	nLastUserCardTime = 0;		// 最后一次使用工会房卡的时间戳 [2017-7-14 willing]
	Lint	nJoinTime = 0;				// 加入工会的时间 [2017-7-28 willing]
	Lint	m_reqTime = 0;				//申请加入或者离开的时间戳
	//////////////////////////////////////////////////////////////////////////
	Lint	nCareForDesk = 0;			// 1 订阅了桌子信息的变化，0 没有订阅  [2017-8-3 willing]

	Ldouble	nNumOfCard2s = 0;		    // 玩家的卡

	STeamMember()
	{
		Reset();
	}

	void Reset()
	{
		nTeamID = 0;
		nUserID = 0;
		strUserName = "";
		strHeadImageUrl = "";
		nTodayUserCardNum = 0;
		nGameState = ETMGS_OFFLINE;
		nTeamState = ETMTS_NOT_EXIST;
		nLastUserCardTime = 0;
		nJoinTime = 0;
		nCareForDesk = 0;
		nNumOfCard2s = 0;
	}
};

typedef std::map<Lint, STeamMember>	tTeamMemberMap;


// 工会信息结构 [2017-7-14 willing]
struct STeamInfo
{
	Lint	nTeamID;				// 工会ID [2017-7-14 willing]
	Lint	nCreaterID;				// 创建者ID [2017-7-14 willing]
	Lstring strCreaterName;			// 创建者名称 [2017-7-19 willing]
	Lstring strCreaterImageUrl;		// 创建者的URL
	Lstring	strTeamName;			// 工会名称 [2017-7-14 willing]
	Lint	nCards;					// 工会房卡 [2017-7-14 willing]
	Lint	nCardAlert;				// 工会最低房卡警报值 [2017-7-21 willing]
	Llong	nCreateTime;			// 工会创建时间戳 [2017-7-14 willing]
	Lint	nMemCardLimitPerDay;	// 工会成员每天最大可使用工会卡的数量 [2017-7-14 willing]
	Lint	nState;					// 当前工会状态，是否允许使用工会房卡创建房间 等 [2017-7-14 willing]
	tTeamMemberMap	m_tTeamMemberMap;
	Lint	nOnLineCount;			// 当前在线数量 [2017-7-18 willing]
	Lint	nMemCount;				// 只包含状态正常的，不包含申请中的玩家 [2017-7-19 willing]
	Lint	nAdminID;				//工会会主ID
	Lint	nYuKouCard;				// 预扣的房卡 [2017-7-23 willing]
	Lint	nSendAlertTime;			// 上次发送房卡不足的警报的时间 [2017-7-25 willing]
	Lstring		m_boardInfo;		//公告信息
	Lint	nServerID;				// 17002

	//新加内容
	std::vector<Lint> vPlayTypes;   // 玩法内容
	Lint	nManageId;              //上级玩家id
	Lint	nCountyId;              //城市id
	Lint    nDeskCntLimit;          //开桌限制
	Lint    nDeskProportionate;     //桌子的抽成比例

	STeamInfo()
	{
		nCards = 0;
		nCardAlert = 300;
		nTeamID = 0;
		nCreaterID = 0;
		strCreaterName = "";
		strTeamName = "";
		nCreateTime = 0;
		nMemCardLimitPerDay = 0;
		nState = ETMTS_NORMAL;
		nOnLineCount = 0;
		nMemCount = 0;
		nAdminID = 0;
		nYuKouCard = 0;
		nSendAlertTime = 0;
		strCreaterImageUrl = "";
		m_boardInfo = "";
		nServerID = GAME_APPID;
		vPlayTypes.clear();
		nManageId = 0;
		nCountyId = 0;
		nDeskCntLimit = 0;
		nDeskProportionate = 0;
	}

	// 存储 [2017-7-17 willing]
	void	SaveTeamInfo();

	// 成员信息存库 [2017-7-20 willing]
	void	SaveMemInfo(Lint nUserID);

	// 删除成员 [2017-7-20 willing]
	void	RmvMemInfo(Lint nUserID);

	// 获取指定的玩家在 工会中的状态 [2017-7-20 willing]
	Lint	GetUserTeamState(Lint nUserID);
};

typedef std::map<Lint, STeamInfo>	tTeamInfoMap;



#endif
