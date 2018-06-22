#ifndef _L_MSG_CT_H_
#define _L_MSG_CT_H_

#include "LMsg.h"
#include "GameDefine.h"
#include <vector>

typedef std::map<Lint, Lint>		tIntIntMap;
typedef std::list<Lint>				tIntList;
typedef std::map<Lint, tIntList>	tIntListMap;
typedef std::map<Lstring, Lint>		tStringIntMap;


struct STeamPlayConfigMsg
{
	Lint				m_roomType = 0;				//房间类型  8局  16局
	Lint				m_playEnum = -1;				//玩法枚举  
	Lint				m_teamID = 0;				//工会ID
	std::vector<Lint>	m_playtype;					//玩法
	Llong				m_nCreateTime = 0;			// 玩法创建的时间戳 
	Lint                m_circleGoldLimit = 0;
	Lint                m_playtype_count;
	MSGPACK_DEFINE_MAP(m_roomType, m_playEnum, m_teamID, m_playtype, m_nCreateTime, m_circleGoldLimit);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_roomType);
		buff.Read(m_playEnum);
		buff.Read(m_teamID);
		buff.Read(m_playtype_count);
		m_playtype.resize(m_playtype_count);
		for (Lint i = 0; i < m_playtype_count; ++i)
		{
			buff.Read(m_playtype[i]);
		}
		buff.Read(m_nCreateTime);
		buff.Read(m_circleGoldLimit);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_roomType);
		buff.Write(m_playEnum);
		buff.Write(m_teamID);
		m_playtype_count = m_playtype.size();
		buff.Write(m_playtype_count);
		for (Lint i = 0; i < m_playtype_count; ++i)
		{
			buff.Write(m_playtype[i]);
		}
		buff.Write(m_nCreateTime);
		buff.Write(m_circleGoldLimit);
		return true;
	}
};


// 客户端查询工会信息 [2017-7-17 willing]
struct LMsgC2SQuerTeam : public LMsgSC
{
	int   nTeamID = 0;		// 要查询的工会ID [2017-7-17 willing]

	LMsgC2SQuerTeam() :LMsgSC(MSG_C_2_S_QueryTeam) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SQuerTeam(); }
};

// 发送消息用到的工会信息结构体 [2017-7-21 willing]
struct STeamDescInfoMsg
{
	Lint		nTeamID = 0;
	Lstring		strTeamName;
	Lint		nCreaterID = 0;
	Lstring		strCreaterName;
	Lint		nMemCount = 0;
	Lint		nOnLienCount = 0;
	Lstring		m_CreaterHeadImageURL;
	Lint		nReqJoinTime = 0;		// 工会创建时间
	Lint		m_isMaster = 0;			//是否是群主 0：不是 1：是
	Lstring		m_boardInfo;
	Lint		m_nCards = 0;

	//新加内容
	Lint	nManageId = 0;              //上级玩家id
	Lint	nCountyId = 0;              //城市id
	Lint    nDeskCntLimit = 0;          //开桌数限制
	std::vector<Lint> vPlayTypes;   // 玩法内容
	Lint    nDeskProportionate = 0; //桌子的抽成比例

	STeamPlayConfigMsg	m_playConfig;
	MSGPACK_DEFINE_MAP(nTeamID, strTeamName, nCreaterID, strCreaterName, nMemCount, nOnLienCount, nReqJoinTime, m_CreaterHeadImageURL,
		m_isMaster, m_playConfig, m_boardInfo, m_nCards, nManageId, nCountyId, nDeskCntLimit, vPlayTypes, nDeskProportionate);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(strTeamName);
		buff.Read(nCreaterID);
		buff.Read(strCreaterName);
		buff.Read(nMemCount);
		buff.Read(nOnLienCount);
		buff.Read(m_CreaterHeadImageURL);
		buff.Read(nReqJoinTime);
		buff.Read(m_isMaster);
		buff.Read(m_boardInfo);
		buff.Read(m_nCards);
		buff.Read(nManageId);
		buff.Read(nCountyId);
		buff.Read(nDeskCntLimit);
		Lint vPlayTypes_count;
		buff.Read(vPlayTypes_count);
		vPlayTypes.resize(vPlayTypes_count);
		for (Lint i = 0; i < vPlayTypes_count; ++i)
		{
			buff.Read(vPlayTypes[i]);
		}
		buff.Read(nDeskProportionate);
		m_playConfig.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(strTeamName);
		buff.Write(nCreaterID);
		buff.Write(strCreaterName);
		buff.Write(nMemCount);
		buff.Write(nOnLienCount);
		buff.Write(m_CreaterHeadImageURL);
		buff.Write(nReqJoinTime);
		buff.Write(m_isMaster);
		buff.Write(m_boardInfo);
		buff.Write(m_nCards);
		buff.Write(nManageId);
		buff.Write(nCountyId);
		buff.Write(nDeskCntLimit);
		Lint vPlayTypes_count = vPlayTypes.size();
		buff.Write(vPlayTypes_count);
		for (Lint i = 0; i < vPlayTypes_count; ++i)
		{
			buff.Write(vPlayTypes[i]);
		}
		buff.Write(nDeskProportionate);
		m_playConfig.Write(buff);
		return true;
	}
};

// 服务器返回 工会信息 [2017-7-18 willing]
struct LMsgS2CQueryTeamInfo : public LMsgSC
{
	int		nTeamID = 0;				// 要查询的工会ID,如果查询不到 该值为0 [2017-7-17 willing]
	Lint	nCreaterID = 0;				// 创建者ID [2017-7-14 willing]
	Lstring	strCreaterName = "";		// 工会名称 [2017-7-14 willing]
	Lstring	strTeamName = "";			// 工会名称 [2017-7-14 willing]
	Lint	nMemCount = 0;
	Lint	nOnLineCount = 0;
	Lstring	m_CreaterHeadImageURL = "";
	Lint	nState = 0;					// 查询玩家在该工会中的状态 [2017-7-29 willing]
	Lint	m_isInDesk = 0;		//是否加入了房间

	STeamDescInfoMsg teamInfo;//公会本身的信息

	LMsgS2CQueryTeamInfo() :LMsgSC(MSG_S_2_C_TeamInfo) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nCreaterID, strCreaterName, strTeamName, nMemCount, nOnLineCount,nState, m_CreaterHeadImageURL, m_isInDesk, teamInfo);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CQueryTeamInfo(); }
};


// 服务器返回 工会信息 [2017-7-18 willing]
struct QueryTeamInfo
{
	Lint	nTeamID = 0;				// 要查询的工会ID,如果查询不到 该值为0 [2017-7-17 willing]
	Lint	nCreaterID = 0;				// 创建者ID [2017-7-14 willing]
	Lstring	strCreaterName = "";		// 工会名称 [2017-7-14 willing]
	Lstring	strTeamName = "";			// 工会名称 [2017-7-14 willing]
	Lint	nMemCount = 0;
	Lint	nOnLineCount = 0;
	Lstring	m_CreaterHeadImageURL = "";
	Lint	nState = 0;					// 查询玩家在该工会中的状态 [2017-7-29 willing]
	Lint	m_isInDesk = 0;		//是否加入了房间

	STeamDescInfoMsg teamInfo;//公会本身的信息

	MSGPACK_DEFINE_MAP(nTeamID, nCreaterID, strCreaterName, strTeamName, nMemCount, nOnLineCount, nState, m_CreaterHeadImageURL, m_isInDesk, teamInfo);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nCreaterID);
		buff.Read(strCreaterName);
		buff.Read(strTeamName);
		buff.Read(nMemCount);
		buff.Read(nOnLineCount);
		buff.Read(m_CreaterHeadImageURL);
		buff.Read(nState);
		buff.Read(m_isInDesk);

		teamInfo.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(nCreaterID);
		buff.Write(strCreaterName);
		buff.Write(strTeamName);
		buff.Write(nMemCount);
		buff.Write(nOnLineCount);
		buff.Write(m_CreaterHeadImageURL);
		buff.Write(nState);
		buff.Write(m_isInDesk);

		teamInfo.Write(buff);
		return true;
	}
};


//////////////////////////////////////////////////////////////////////////
//返回玩家登陆结果消息
struct LMsgS2CLogin :public LMsgSC
{
	Lint		m_errorCode;//登录错误码，0-登录成功,1-帐号未注册，2-帐号已登录，3-未知错误 17-该账号有其他商户在房间 请使用之前商户号
	Lint		m_id;
	Lstring		m_unionId;
	Lstring		m_nike;
	Lint		m_exp;
	Lint		m_sex;
	Lstring		m_face;//
	Lstring		m_provice;
	Lstring		m_city;
	Lstring		m_ip;
	Lstring		m_playerType;
	Lint		m_new;//0-不是，1-是
	Lint		m_card1;//新手奖励房卡1
	Ldouble		m_card2;//新手奖励房卡2
	Lint		m_card3;//新手奖励房卡3
	Lint        m_gold; //玩家金币数;
	Lint		m_state;//0-进入大厅，1-进入桌子
	Lstring		m_buyInfo;//够买信息
	Lint		m_hide;//购买隐藏0-隐藏 1- 不隐藏
	Lint		m_gm;//0-不是，1-是GM
	Lint		m_curHeadIconId; //玩家头像框ID
	Lstring     m_phone;
	std::vector<Lint>	m_gainHeadIconId; //玩家已获取的头像框ID
	Lint		m_nCurTime = 0; //服务器当前时间戳

	Lint        m_nTeamId = 0; //当前使用的俱乐部id
	Lstring     m_sTeamName; //当前使用的俱乐部名称 有错误的时候才显示

	Lint	    nState = 0;	// 查询玩家在该工会中的状态 [2017-7-29 willing]
	Lint	    m_isInDesk = 0;	//是否加入了房间
	Lint		m_serverTime;

	STeamDescInfoMsg teamInfo; //俱乐部自己的信息

	LMsgS2CLogin() :LMsgSC(MSG_S_2_C_LOGIN), m_errorCode(0), m_id(0), m_exp(0), m_sex(0)
		, m_new(0), m_card1(0), m_card2(0.0f), m_card3(0), m_state(0), m_hide(0), m_gm(0), m_curHeadIconId(0), m_serverTime(0)
	{
	}

	MSGPACK_DEFINE_MAP(m_msgId, m_errorCode, m_id, m_unionId, m_nike, m_exp, m_sex, m_face, m_provice,
		m_city, m_ip, m_playerType, m_new, m_card1, m_card2, m_card3, m_gold, m_state, m_buyInfo, m_hide, m_gm,
		m_curHeadIconId, m_phone, m_gainHeadIconId, m_nCurTime, m_nTeamId, m_sTeamName, 
		nState, m_isInDesk, m_serverTime, teamInfo);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CLogin(); }
};


// 申请加入工会 [2017-7-19 willing]
struct LMsgC2SReqJoin : public LMsgSC
{
	int   nTeamID = 0;		// 要申请加入的工会ID [2017-7-17 willing]

	LMsgC2SReqJoin() :LMsgSC(MSG_C_2_S_REQ_JOIN) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SReqJoin(); }
};

// 申请加入工会的结果 [2017-7-19 willing]
struct LMsgS2CJoinRst : public LMsgSC
{
	Lint  nTeamID = 0;
	int   nJoinRst = 0;		// 0 加入成功，>0 失败原因 [2017-7-17 willing]

	LMsgS2CJoinRst() :LMsgSC(MSG_S_2_C_JOIN_RST) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID,nJoinRst);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CJoinRst(); }
};

// 服务器通知会长对某个玩家的操作 [2017-7-20 willing]
struct LMsgS2CNoticeEvent : public LMsgSC
{
	Lint nTeamID = 0;
	Lstring strCreaterName = "";
	Lstring strTeamName = "";
	Lint nOpt = 0;				// 0 加入申请提交成功，1 会长批准加入，2 拒绝加入， 3 拒绝离开 4 会长同意离开，5 被会长移除 6 会长主动把玩家加入工会 15 会长更改公会信息
	STeamDescInfoMsg m_teamInfo;
	LMsgS2CNoticeEvent() :LMsgSC(MSG_S_2_C_NOTICE_EVENT) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID,strCreaterName, strTeamName, nOpt, m_teamInfo);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CNoticeEvent(); }
};

// 玩家请求自己所在的工会列表 [2017-7-20 willing]
struct LMsgC2SReqTeamList : public LMsgSC
{
	int   nTeamState = 0;		// 1 已经申请的，2 已经加入的功能 [2017-7-20 willing]

	LMsgC2SReqTeamList() :LMsgSC(MSG_C_2_S_REQ_TEAM_LIST) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamState);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SReqTeamList(); }
};


struct LMsgS2CTeamPlayConfig : public LMsgSC
{
	STeamPlayConfigMsg	m_playConfig;
	MSGPACK_DEFINE_MAP(m_msgId, m_playConfig);
	LMsgS2CTeamPlayConfig() :LMsgSC(MSG_S_2_C_PLAY_CONFIG) {}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CTeamPlayConfig(); }
};




// 向客户端返回工会列表 [2017-7-20 willing]
struct LMsgS2CTeamList : public LMsgSC
{
	Lint nReqState = 0;		// 玩家请求的状态值 [2017-7-21 willing]
	Lint	m_isInDesk = 0;		//是否加入了房间
	std::vector<STeamDescInfoMsg> teamInfoList;

	LMsgS2CTeamList() :LMsgSC(MSG_S_2_C_TEAM_LIST) {}
	MSGPACK_DEFINE_MAP(m_msgId, nReqState, teamInfoList, m_isInDesk);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CTeamList(); }
};

// 获取工会详情信息 [2017-7-21 willing]
struct LMsgC2SGetTeamInfo : public LMsgSC
{
	int   nTeamID = 0;

	LMsgC2SGetTeamInfo() :LMsgSC(MSG_C_2_S_TEAM_INFO) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SGetTeamInfo(); }
};

// 返回工会详情信息 [2017-7-21 willing]
struct LMsgS2CTeamInfo : public LMsgSC
{
	Lint	m_error = 0;				//0:正常
	Lint	nTeamID = 0;				// 工会ID [2017-7-14 willing]
	Lstring strCreaterName;			// 创建者名称 [2017-7-19 willing]
	Lstring	strTeamName;			// 工会名称 [2017-7-14 willing]
	Lint	nCards = 0;					// 工会房卡 [2017-7-14 willing]
	Llong	nCreateTime = 0;			// 工会创建时间戳 [2017-7-14 willing]
	Lint	nMemCardLimitPerDay = 0;	// 工会成员每天最大可使用工会卡的数量 [2017-7-14 willing]
	Lint	nState = 0;					// 当前工会状态，是否允许使用工会房卡创建房间 等 [2017-7-14 willing]
	Lint	nOnLineCount = 0;			// 当前在线数量 [2017-7-18 willing]
	Lint	nMemCount = 0;				// 只包含状态正常的，不包含申请中的玩家 [2017-7-19 willing]

	Lint	nTodayUserCard = 0;			// 玩家当天的已经使用的工会房卡 [2017-7-26 willing]
	Lstring	m_boardInfo;				//工会公告
	LMsgS2CTeamInfo() :LMsgSC(MSG_S_2_C_Team_Info) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, strCreaterName, strTeamName, nCards, nCreateTime,
		nMemCardLimitPerDay, nState, nOnLineCount, nMemCount , nTodayUserCard, m_boardInfo, m_error);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CTeamInfo(); }
};

// 请求工会成员列表 [2017-7-21 willing]
struct LMsgC2SGetTeamMemberList : public LMsgSC
{
	int   nTeamID = 0;

	LMsgC2SGetTeamMemberList() :LMsgSC(MSG_C_2_S_Member_List) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SGetTeamMemberList(); }
};

// 发送消息用到的工会成员信息结构 [2017-7-21 willing]
struct STeamMemberMsg
{
	Lint	nUserID = 0;				// 玩家UserID [2017-7-14 willing]
	Lstring strUserName;			// 玩家名称 [2017-7-21 willing]
	Lstring strHeadImageUrl;		// 玩家头像的URL [2017-7-14 willing]
	Lint	nGameState = 0;				// 玩家在游戏中的状态 [2017-7-14 willing]
	Lint	nTeamState = 0;				//玩家在工会中的状态 1:申请加入  2：正常成员  3：申请离开
	Lint	m_reqTime = 0;				//申请加入或者离开的时间戳
	Lint	m_joinTime = 0;
	MSGPACK_DEFINE_MAP(nUserID, strUserName, strHeadImageUrl, nGameState, nTeamState, m_reqTime, m_joinTime);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
};

// 返回工会成员列表 [2017-7-21 willing]
struct LMsgS2CMemberList : public LMsgSC
{
	Lint nPackageIndex = 0;			// 0 第一个包，>0 后续的包，-1 最后一个包 [2017-8-11 willing]
	std::vector<STeamMemberMsg> memberList;

	LMsgS2CMemberList() :LMsgSC(MSG_S_2_C_Member_List) {}
	MSGPACK_DEFINE_MAP(m_msgId, nPackageIndex, memberList);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CMemberList(); }
};

// 取消入会申请 [2017-7-21 willing]
struct LMsgC2SCancelJoin : public LMsgSC
{
	int   nTeamID = 0;

	LMsgC2SCancelJoin() :LMsgSC(MSG_C_2_S_Cancel_Join) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SCancelJoin(); }
};

// 取消入会申请的结果 [2017-7-21 willing]
struct LMsgS2CCancelJoin : public LMsgSC
{
	Lint nTeamID = 0;			// 工会ID [2017-7-22 willing]
	Lint nResult = 0;			// 0 成功 [2017-7-21 willing]
	Lstring strTeamName = "";	// 工会名称 [2017-7-21 willing]

	LMsgS2CCancelJoin() :LMsgSC(MSG_S_2_C_Cancel_Join) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID,nResult,strTeamName);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CCancelJoin(); }
};

// 通知所有玩家，所在的工会被会长解散了 [2017-7-22 willing]
struct LMsgS2CNoticeTeamDismiss : public LMsgSC
{
	Lstring strCreaterName = "";	// 创建者名称 [2017-7-22 willing]
	Lstring strTeamName = "";		// 工会名称 [2017-7-21 willing]
	Lint    nTeamId = 0;			//工会ID
	LMsgS2CNoticeTeamDismiss() :LMsgSC(MSG_S_2_C_Notice_Dismiss) {}
	MSGPACK_DEFINE_MAP(m_msgId, strCreaterName, strTeamName, nTeamId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CNoticeTeamDismiss(); }
};
// 申请离开工会 [2017-7-22 willing]
struct LMsgC2SLeaveTeam : public LMsgSC
{
	int   nTeamID = 0;

	LMsgC2SLeaveTeam() :LMsgSC(MSG_C_2_S_Leave_Team) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SLeaveTeam(); }
};

// 申请离开工会的结果 [2017-7-22 willing]		
struct LMsgS2CLeaveTeamRst : public LMsgSC
{
	Lint nTeamID = 0;			// 工会ID [2017-7-22 willing]
	Lint nResult = 0;			// 0 成功 [2017-7-21 willing]
	Lstring strTeamName = "";	// 工会名称 [2017-7-21 willing]

	LMsgS2CLeaveTeamRst() :LMsgSC(MSG_S_2_C_Leave_Team_Rst) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nResult, strTeamName);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CLeaveTeamRst(); }
};

// 通过工会创建桌子失败 [2017-7-23 willing]
struct LMsgC2SCreateTeamDeskError : public LMsgSC
{
	int   nErrorCode = 0;

	LMsgC2SCreateTeamDeskError() :LMsgSC(MSG_S_2_C_CreateDeskError) {}
	MSGPACK_DEFINE_MAP(m_msgId, nErrorCode);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SCreateTeamDeskError(); }
};

// 工会信息改变 [2017-7-23 willing]
struct LMsgS2CTeamInfoChange : public LMsgSC
{
	Lint nTeamID = 0;			// 工会ID [2017-7-23 willing]
	Lint nCards = 0;			// 工会房卡数量 [2017-7-23 willing]

	LMsgS2CTeamInfoChange() :LMsgSC(MSG_S_2_C_TeamInfoChange) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID,nCards);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CTeamInfoChange(); }
};

// 查看成员的战绩 [2017-7-24 willing]
struct LMsgC2SGetVideoLog : public LMsgSC
{
	int		nTeamID = 0;
	int		nUserID = 0;
	int		nYear = 0;
	int		nMonth = 0;
	int		nDay = 0;

	LMsgC2SGetVideoLog() :LMsgSC(MSG_C_2_S_GET_VIDEO_LOG) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nUserID, nYear, nMonth, nDay);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SGetVideoLog(); }
};

// 客户端查询工会战绩用到的结构体 [2017-7-24 willing]
struct STeamVideoLog
{
	Lint nTime = 0;
	Lint nDeskID = 0;
	std::vector<Lint> nUserID;
	std::vector<Lstring> strUserName;
	std::vector<Lint> nScore;
	Lint nOptState = 0;
	Lint m_flag = 0;
	Lint m_maxCircle = 0;
	MSGPACK_DEFINE_MAP(nTime, nDeskID, nUserID, strUserName, nScore, nOptState, m_flag, m_maxCircle);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
};

// 服务器返回玩家战绩 [2017-7-24 willing]
struct LMsgS2CVideoLog : public LMsgSC
{
	Lint nPackageIndex = 0;			//-2:查询战绩失败 -1：最后一个包 0：第一个
	std::vector<STeamVideoLog>	teamVideoLog;

	LMsgS2CVideoLog() :LMsgSC(MSG_S_2_C_VIDEO_LOG) {}
	MSGPACK_DEFINE_MAP(m_msgId, nPackageIndex, teamVideoLog);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CVideoLog(); }
};

// 成员信息改变 [2017-7-25 willing]
struct LMsgS2CMemberInfoChange : public LMsgSC
{
	Lint nTeamID = 0;
	Lint nTodayUseCardCount = 0;

	LMsgS2CMemberInfoChange() :LMsgSC(MSG_S_2_C_MemberInfoChange) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nTodayUseCardCount);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CMemberInfoChange(); }
};

// 通知工会成员新创建了一个桌子 [2017-8-3 willing]
struct LMsgS2CNewTeamDesk : public LMsgSC
{
	Lint nTeamID = 0;
	Lint nDeskID = 0;
	std::vector<Lint> nUserID;
	std::vector<Lstring> strHeadImageUrl;			// 玩家的头像列表 [2017-8-5 willing]
	Lint nMajongType = 0;							// 玩法名称 [2017-8-2 willing]
	Lint nMaxCircle = 0;							// 最大局数 [2017-8-3 willing]
	Lint nMaxUserCount = 0;							// 桌子的最大玩家数量 [2017-8-4 willing]
	Lint nUpDownJi = 0;								// 上下鸡
	Lint nAllJi = 0;								// 满堂鸡
	Lint nLianZhuang = 0;							// 连庄是否算分 [2016-9-12 willing]
	Lint nPlayTypeCount = 0;						// 小玩法数量 [2017-8-3 willing]
	std::vector<Lint>	tPlayType;					// 小玩法选项 [2017-8-2 willing]
	Llong nCreateTime;								// 桌子创建的时间戳 [2017-8-5 willing]

	LMsgS2CNewTeamDesk() :LMsgSC(MSG_S_2_C_NEW_TEAM_DESK),nCreateTime(0) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nDeskID,nUserID, strHeadImageUrl,nMajongType, nMaxCircle, nMaxUserCount,
						nUpDownJi, nAllJi, nLianZhuang, nPlayTypeCount, tPlayType, nCreateTime);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CNewTeamDesk(); }
};

// 工会桌子中的人员变化了 [2017-8-3 willing]
struct LMsgS2CTeamDeskUserChg : public LMsgSC
{
	Lint nTeamID = 0;
	Lint nDeskID = 0;
	Lint nUserID = 0;
	Lint nIsJoin = 0;		// 1 加入桌子，0 离开桌子 [2017-8-3 willing]
	Lstring strHeadImageUrl;

	LMsgS2CTeamDeskUserChg() :LMsgSC(MSG_S_2_C_TeamDeskUserChg) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nDeskID, nUserID, nIsJoin, strHeadImageUrl);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CTeamDeskUserChg(); }
};

// 删除一个工会桌子，被解散或者已经结束 [2017-8-3 willing]
struct LMsgS2CDelTeamDesk: public LMsgSC
{
	Lint nTeamID = 0;
	Lint nDeskID = 0;

	LMsgS2CDelTeamDesk() :LMsgSC(MSG_S_2_C_DelTeamDesk) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nDeskID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CDelTeamDesk(); }
};

// 客户端请求当前所有的工会桌子 [2017-8-3 willing]
struct LMsgC2SGetAllTeamDesk : public LMsgSC
{
	int		nTeamID = 0;
	int		nUserID = 0;

	LMsgC2SGetAllTeamDesk() :LMsgSC(MSG_C_2_S_GetAllTeamDesk) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nUserID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SGetAllTeamDesk(); }
};

// 所有工会中的桌子 [2017-8-3 willing]
struct STeamDeskInfoMsg
{
	Lint nTeamID = 0;
	Lint nDeskID = 0; //房间号
	std::vector<Lint> nUserID;
	std::vector<Lstring> strHeadImageUrl;			// 玩家头像 [2017-8-5 willing]
	std::vector<Lstring> m_nike;					// 玩家昵称
	Lint nMajongType = 0;							// 玩法名称 [2017-8-2 willing]
	Lint nMaxCircle = 0;							// 最大局数 [2017-8-3 willing]
	Lint nMaxUserCount = 0;
	//Lint nPlayTypeCount = 0;						// 小玩法数量 [2017-8-3 willing]
	std::vector<Lint>	tPlayType;					// 小玩法选项 [2017-8-2 willing]
	Llong nCreateTime = 0;							// 桌子创建的时间戳 [2017-8-5 willing]
	Lint nStake = 0;							    //押注倍数
	Lint nCircleGoldLimit = 0;                      //每一桌子的限制进入的金币数
	Lint nMaxFanshu = 0;                            //客户的炸数
	MSGPACK_DEFINE_MAP(nTeamID, nDeskID, nUserID, strHeadImageUrl,nMajongType, nMaxCircle, 
						nMaxUserCount, tPlayType,nCreateTime, nStake, m_nike, nCircleGoldLimit, nMaxFanshu);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
};

struct LMsgS2CAllTeamDesk : public LMsgSC
{
	Lint nTeamID = 0;
	std::vector<STeamDeskInfoMsg> tAllTeamDesk;

	LMsgS2CAllTeamDesk() :LMsgSC(MSG_S_2_C_ALLTeamDesk) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, tAllTeamDesk);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CAllTeamDesk(); }
};

// 订阅桌子变化信息 [2017-8-3 willing]
struct LMsgC2SSubscribeTeamDesk : public LMsgSC
{
	int		nTeamID = 0;
	int		nCareFor = 0;		// 0 取消订阅，1 订阅 [2017-8-3 willing]

	LMsgC2SSubscribeTeamDesk() :LMsgSC(MSG_C_2_S_SubscribeTeamDesk) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nCareFor);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SSubscribeTeamDesk(); }
};

// 玩家查询自己对工会的操作日志 [2017-8-8 willing]
struct LMsgC2SGetTeamOptLog : public LMsgSC
{
	LMsgC2SGetTeamOptLog() :LMsgSC(MSG_C_2_S_GetTeamOptLog) {}
	MSGPACK_DEFINE_MAP(m_msgId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SGetTeamOptLog(); }
};

// 服务器返回玩家对工会的操作日志 [2017-8-8 willing]
struct STeamMemOptLog
{
	int nTeamID = 0;
	Lstring strTeamName = "";
	int nOpt = 0;
	Llong nTimestamp = 0;		// 时间戳 [2017-8-8 willing]

	MSGPACK_DEFINE_MAP(nTeamID, strTeamName, nOpt, nTimestamp);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
};

struct LMsgS2CMemTeamOptLog : public LMsgSC
{
	std::vector<STeamMemOptLog> tAllTeamMemOptLog;

	LMsgS2CMemTeamOptLog() :LMsgSC(MSG_S_2_C_MemTeamOptLog) {}
	MSGPACK_DEFINE_MAP(m_msgId, tAllTeamMemOptLog);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CMemTeamOptLog(); }
};

// 获取指定时间内的对局日志 [2017-8-10 willing]
struct LMsgC2SGetRangeVideoLog : public LMsgSC
{
	int		nTeamID = 0;
	int		nYear = 0;
	int		nMonth = 0;
	int		nDay = 0;
	int		nEndYear = 0;
	int		nEndMonth = 0;
	int		nEndDay = 0;
	LMsgC2SGetRangeVideoLog() :LMsgSC(MSG_C_2_S_GET_RANGE_VIDEO_LOG) {}
	MSGPACK_DEFINE_MAP(m_msgId, nTeamID, nYear, nMonth, nDay, nEndYear, nEndMonth, nEndDay);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SGetRangeVideoLog(); }
};

struct LMsgC2SCreatePlayConfig : public LMsgSC
{
	Lint				m_roomType;				//房间类型  8局  16局
	Lint				m_playEnum;				//玩法枚举  
	Lint				m_teamID;				//工会ID
	std::vector<Lint>	m_playtype;				//玩法
	Lint				m_opt;					//操作类型 0：
	Lint                m_circleGoldLimit = 0;
	LMsgC2SCreatePlayConfig() :LMsgSC(MSG_C_2_S_CREATE_KUAISU_JU),m_roomType(0),m_playEnum(0),m_teamID(0),m_opt(0) {}
	MSGPACK_DEFINE_MAP(m_msgId,m_roomType, m_playEnum,m_playtype,m_teamID,m_opt, m_circleGoldLimit);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SCreatePlayConfig(); }
};



struct LMsgC2SFastPlay : public LMsgSC
{
	Lint				m_teamID;				//工会ID
	LMsgC2SFastPlay() :LMsgSC(MSG_C_2_S_FAST_PLAY), m_teamID(0){}
	MSGPACK_DEFINE_MAP(m_msgId, m_teamID);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SFastPlay(); }
};
struct LMsgS2CFastPlayRet : public LMsgSC
{
	Lint				m_error;
	LMsgS2CFastPlayRet() :LMsgSC(MSG_S_2_C_FAST_PLAY), m_error(0) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_error);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CFastPlayRet(); }
};

struct LMsgC2SOptMember : public LMsgSC
{
	int		m_teamId = 0;	//工会ID
	int		m_opt = 0;		// 1:批准加入 2:拒绝加入 3:拒绝离开  4:同意离开 5:删除玩家 
	int		m_userId = 0;	//操作的玩家id

	LMsgC2SOptMember() :LMsgSC(MSG_C_2_S_OPT_MEM) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_teamId, m_opt, m_userId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SOptMember(); }
};

struct LMsgS2COptMemRet : public LMsgSC
{
	int		m_error = 0;		//0:成功 3:工会不存在 4:工会的创建者ID不正确  5:成员管理的操作码不能识别
	int		m_teamId = 0;
	int		m_opt = 0;			// 操作类型    1:批准加入 2:拒绝加入 3:拒绝离开  4:同意离开 5:删除玩家 
	int		m_userId = 0;
	LMsgS2COptMemRet() :LMsgSC(MSG_S_2_C_OPT_MEM_RET) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_error, m_teamId, m_opt, m_userId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2COptMemRet(); }
};

struct LMsgC2SOptTeamInfo : public LMsgSC
{
	int		m_teamId = 0;	//工会ID
	Lstring	m_teamName;
	Lstring m_boardInfo;

	LMsgC2SOptTeamInfo() :LMsgSC(MSG_C_2_S_OPT_TEAMINFO),m_teamId(0) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_teamId, m_teamName, m_boardInfo);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SOptTeamInfo(); }
};
struct LMsgS2CAdminInviteOtherRet : public LMsgSC
{
	Lint m_error;
	Lint m_teamID;
	Lstring m_strTeamName;

	LMsgS2CAdminInviteOtherRet() :LMsgSC(MSG_S_2_C_ADMIN_IVITE_RET), m_error(0),m_teamID(0) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_error);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CAdminInviteOtherRet(); }
};

struct LMsgC2SAdminInviteOther : public LMsgSC
{
	int		m_teamId;	//工会ID
	Lint	m_userid;
	Lint	m_adminId;
	LMsgC2SAdminInviteOther() :LMsgSC(MSG_C_2_S_ADMIN_IVITE), m_teamId(0),m_userid(0),m_adminId(0) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_teamId, m_userid, m_adminId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SAdminInviteOther(); }
};

////////////////////////////////////////////////////////////////////////////////////////
// 请求兑换金币
//MSG_C_2_S_REQ_EXCHAGEGOLD = 344, // 请求兑换金币
struct LMsgC2SReqExchageGold : public LMsgSC
{
	Lint		nToUserId = 0;
	Lint		nGoldNum = 0;
	LMsgC2SReqExchageGold() :LMsgSC(MSG_C_2_S_REQ_EXCHAGEGOLD) {}
	MSGPACK_DEFINE_MAP(m_msgId, nToUserId, nGoldNum);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SReqExchageGold(); }
};

// 请求兑换金币 结果
//MSG_S_2_C_REQ_EXCHAGEGOLDD = 345, // 请求兑换金币 结果
struct LMsgS2CReqExchageGold : public LMsgSC
{
	Lint nResCode = 0; //0 成功， 1 兑换的钱不够， 2 找不到人， 3 其他错误

	LMsgS2CReqExchageGold() :LMsgSC(MSG_S_2_C_REQ_EXCHAGEGOLDD) {}
	MSGPACK_DEFINE_MAP(m_msgId, nResCode);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CReqExchageGold(); }
};

// 请求最近兑换金币 记录
//MSG_C_2_S_REQ_EXCHAGEGOLD_LOG = 346, // 请求最近兑换金币 记录
struct LMsgC2SReqExchageGoldLog : public LMsgSC
{
	LMsgC2SReqExchageGoldLog() :LMsgSC(MSG_C_2_S_REQ_EXCHAGEGOLD_LOG) {}
	MSGPACK_DEFINE_MAP(m_msgId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SReqExchageGoldLog(); }
};

// 金币的操作日志
struct SExchageGoldOptLog
{
	Llong lLogId = 0; //日志的id
	Lint nFromUserId = 0; //谁请求的
	Lint nToUserId = 0; //让谁回收
	Lint nGoldNum = 0; //回收多少
	Lstring sFromName = ""; //谁申请的
	Lstring sToName = ""; //谁处理
	Lint nOpt = 0; //0尚未处理 1已经处理 2 from取消处理 3 to取消处理
	Lint nReqTime = 0;		//请求的时间 
	Lint nProTime = 0;		//处理的时间 

	MSGPACK_DEFINE_MAP(lLogId, nFromUserId, nToUserId, nGoldNum, sFromName, sToName, nOpt, nReqTime, nProTime);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
};

//金币兑换日志 只是10条记录
//MSG_S_2_C_EXCHAGEGOLD_OPT_LOG = 347, // 金币兑换日志
struct LMsgS2CExchageGoldOptLog : public LMsgSC
{
	std::vector<SExchageGoldOptLog> tAllExchageGoldOptLog;

	LMsgS2CExchageGoldOptLog() :LMsgSC(MSG_S_2_C_EXCHAGEGOLD_OPT_LOG) {}
	MSGPACK_DEFINE_MAP(m_msgId, tAllExchageGoldOptLog);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CExchageGoldOptLog(); }
};


// 取消兑换
//MSG_C_2_S_CANNCEL_EXCHAGEGOLD = 348, // 取消兑换
struct LMsgC2SCanncelExchageGold : public LMsgSC
{
	Llong		lLogId = 0;
	LMsgC2SCanncelExchageGold() :LMsgSC(MSG_C_2_S_CANNCEL_EXCHAGEGOLD) {}
	MSGPACK_DEFINE_MAP(m_msgId, lLogId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SCanncelExchageGold(); }
};

// 取消兑换 结果
//MSG_S_2_C_CANNCEL_EXCHAGEGOLD = 349, // 取消兑换 结果
struct LMsgS2CCanncelExchageGold : public LMsgSC
{
	Lint nResCode = 0; //0 成功， 1 错误id， 2 不能取消，例如已经被兑换等， 3 其他错误

	LMsgS2CCanncelExchageGold() :LMsgSC(MSG_S_2_C_CANNCEL_EXCHAGEGOLD) {}
	MSGPACK_DEFINE_MAP(m_msgId, nResCode);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CCanncelExchageGold(); }
};


//MSG_S_2_C_EXCHAGEGOLD_OPTNOTICE = 352, // 回收金币操作 通知
struct LMsgS2CExchageGoldOptNotice : public LMsgSC
{
	Lint nResCode = 0; //0 已经被回收， 1 放弃回收
	Lint nFromUserId = 0; //谁请求的
	Lint nToUserId = 0; //让谁回收
	Lint nGoldNum = 0; //回收多少
	Lstring sFromName = ""; //谁申请的
	Lstring sToName = ""; //谁处理

	LMsgS2CExchageGoldOptNotice() :LMsgSC(MSG_S_2_C_EXCHAGEGOLD_OPTNOTICE) {}
	MSGPACK_DEFINE_MAP(m_msgId, nResCode);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CExchageGoldOptNotice(); }
};

////////////////////////////////////////////////////////////////////////////////////////

// 邀请进入房间
//MSG_C_2_S_INVITE_INROOM = 350, // 邀请进入房间
struct LMsgC2SInviteInRoom : public LMsgSC
{
	Lint memberId = 0; //要邀请人的id
	LMsgC2SInviteInRoom() :LMsgSC(MSG_C_2_S_INVITE_INROOM) {}
	MSGPACK_DEFINE_MAP(m_msgId, memberId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2SInviteInRoom(); }
};

// 收到  邀请进入房间
//MSG_S_2_C_INVITE_INROOM = 351, // 收到 邀请进入房间
struct LMsgS2CInviteInRoom : public LMsgSC
{
	Lint fromId = 0; //谁邀请的
	Lstring sFromName = ""; //谁邀请的名字
	Lint deskId = 0; //房间id

	LMsgS2CInviteInRoom() :LMsgSC(MSG_S_2_C_INVITE_INROOM) {}
	MSGPACK_DEFINE_MAP(m_msgId, fromId, sFromName, deskId);

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2CInviteInRoom(); }
};

//////////////////////////////////////////////////////////////////////////

#endif // _L_MSG_CT_H_