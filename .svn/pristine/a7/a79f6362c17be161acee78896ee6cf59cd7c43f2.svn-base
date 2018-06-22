#ifndef _LVIP_LOG_H
#define _LVIP_LOG_H

#include "LTool.h"
#include "LBuff.h"
#include "GameDefine.h"

struct VipDeskLog
{
	Lstring	 m_videoId;//录像id
	Ldouble	 m_gold[DESK_USER_COUNT];
	Ldouble	 m_cal_gold[DESK_USER_COUNT];
	Lint	 m_win[DESK_USER_COUNT];
	Lint	 m_zhuangPos;//坐庄的位置
	Lint	 m_time;//本局时间
	Lint	 m_angang[DESK_USER_COUNT];//暗杠
	Lint	 m_mgang[DESK_USER_COUNT];//明杠
	Lint	 m_bomb[DESK_USER_COUNT];	//点炮者的放炮信息

	////////////////广东///////////////////

	Lint	 m_fanshu[DEFAULT_DESK_USER_COUNT];//番数
	Lint	 m_baohu[DEFAULT_DESK_USER_COUNT];	//爆胡次数
	Lint	m_maNum[DEFAULT_DESK_USER_COUNT];	//马牌数量
	Lint	m_ghostZimo[DEFAULT_DESK_USER_COUNT];	//有鬼自摸
	Lint	m_noghostZimo[DEFAULT_DESK_USER_COUNT];	//无鬼自摸
	Lint	m_normalhu[DEFAULT_DESK_USER_COUNT];		//普通胡
	Lint    m_singleflow[DEFAULT_DESK_USER_COUNT];	//花单吊

	/////////////////广东end//////////////
	VipDeskLog()
	{
		memset(m_gold, 0, sizeof(m_gold));
		memset(m_cal_gold, 0, sizeof(m_cal_gold));
		memset(m_win, 0, sizeof(m_win));
		memset(m_angang, 0, sizeof(m_angang));
		memset(m_mgang, 0, sizeof(m_mgang));
		memset(m_bomb, 0, sizeof(m_bomb));
		memset(m_fanshu, 0, sizeof(m_fanshu));
		memset(m_baohu, 0, sizeof(m_baohu));
		memset(m_maNum, 0, sizeof(m_maNum));
		memset(m_ghostZimo, 0, sizeof(m_ghostZimo));
		memset(m_noghostZimo, 0, sizeof(m_noghostZimo));
		memset(m_normalhu, 0, sizeof(m_normalhu));
		memset(m_singleflow, 0, sizeof(m_singleflow));
		m_zhuangPos = INVAILD_POS;
		m_time = 0;
		m_videoId = "";
	}
};

struct LVipLogItem
{
	Lstring			m_id;		//id
	Lint			m_time;		//时间
	Lint			m_flag;		//房间玩法
	Lint			m_subFlag;	//房间子玩法
	Lint			m_deskId;	//桌子id
	Lstring			m_secret;	//密码
	Lint			m_maxCircle;//总圈数
	Lint			m_curCircle;//当前圈数
	Lint			m_posUserId[DESK_USER_COUNT];//0-3各个位置上的玩家id
	Lint			m_curZhuangPos;//当前庄家
	Ldouble			m_score[4];// 各个位置上面的积分
	Ldouble			m_calScore[4];// 各个位置上面的金币
	Ldouble			m_iniScore[4];// 各个位置上面的初始积分
	Lint			m_reset;//是否解算
	std::vector<Lint> m_playtype;

	Lint			m_curMatchId = 0;	//比赛ID
	Lstring			m_MatchName;		//比赛的名字
	Lint			m_nCurStage = 1;	//第几轮
	Lint            m_nMaxMember = 0;   //最大比赛人数
	int				m_nGroupID = 0;		//小组ID
	Lint            m_nForceFinishTime = 0;  //强制结束比赛的时间
	int             m_IntoTuoGuanRemainTime = 15000;    //未托管进入托管状态剩余时间
	int             m_InTuoGuanCanChuPaiTime = 3000;   //托管状态下可出牌时间
	Lint            m_MatchStartTime = 0;   //比赛开始时间
	int		        m_CreaterID = 0;			//创建人的ID
	Lint            m_MatchType = 0;           //循环赛 专赛 定点赛 广东周年赛
	int             m_nInitualDifen = 500;               //初始底分倍率
	int             m_nDifenPerMinIncStep = 500;         //底分每分钟增长步长
	int             m_nDingJuInitualDifen = 1;      //定局积分阶段的底分
	int             m_nCurDifen = 1;                     //默认为1

	std::vector<VipDeskLog*> m_log;//每一把的记录
	bool			m_free;

	Lint			m_iPlayerCapacity;				//桌子玩家容量不能超过DEFAULT_DESK_USER_COUNT
	Lint			m_playerNum;

	LVipLogItem();
	virtual  ~LVipLogItem();
	virtual  Lstring		ToString();
	virtual  Lstring        PlayTypeToStrint();

	virtual  void			FromString(const Lstring& str);
	virtual  Lstring        PlayTypeToString4RLog();

	int				GetMatchID()const { return m_curMatchId; }
	void			SetMatchID(int nid) { m_curMatchId = nid; }
};
#endif