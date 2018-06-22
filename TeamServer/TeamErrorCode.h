#pragma once

// 错误码定义 [2017-7-22 willing]
enum E_TEAM_ERROR_CODE
{
	ETEC_SUCCESS = 0,				// 成功 [2017-7-22 willing]
	ETEC_TEAM_NOT_EXIST = 1,		// 工会不存在 [2017-7-22 willing]
	ETEC_TEAM_MEM_NOT_EXIST = 2,	// 工会成员不存在 [2017-7-22 willing]
	ETEC_CANOT_CANCEL_JOIN = 3,		// 玩家在工会中的状态不是申请加入状态，不能撤销加入 [2017-7-22 willing]
	ETEC_CANOT_LEAVE_TEAM = 4,		// 玩家在工会中的状态不是已经加入状态，不能申请离开 [2017-7-22 willing]
	ETEC_JUSHU_ERROR = 5,			// 创建房间时传递的局数不正确 [2017-7-23 willing]
	ETEC_CARD_NOT_ENOUGH = 6,		// 工会房卡不足 [2017-7-23 willing]
	ETEC_TEAM_NOT_SAME = 7,			// 不在同一个工会，不能查询战绩 [2017-7-24 willing]
	ETEC_QUERY_VIDEO_LOG_ERR = 8,	// 查询战绩失败[2017-7-24 willing]
	ETEC_PERDAY_CARD_LIMIT = 9,		// 每日使用工会房卡超过上限 [2017-7-24 willing]
	ETEC_JOIN_MAX_TEAM = 10,			// 加入工会数超过了上限 [2017-7-29 willing]
	ETEC_TEAM_MAX_COUNT = 11,		// 申请加入的工会成员已经达到了上限 [2017-7-29 willing]
	ETEC_ALREADY_JOIN = 12,			// 已经申请过加入了工会 [2017-7-29 willing]
	ETEC_TEAM_NORMAL = 13,			// 已经是工会正式成员了，不能申请加入 [2017-7-29 willing]
	ETEC_TEAM_LEAVE = 14,			// 已经申请了离开，不能申请加入 [2017-7-29 willing]
	ETEC_LEAVE_CREATE_DESK = 15,	// 不是工会正式成员，不能使用工会房卡创建房间 [2017-7-29 willing]
	ETEC_IN_DESK_LEAVE = 16,		// 在工会桌子中，不能离开工会 [2017-8-5 willing]
	ETEC_IN_DESK = 17,				//在桌子里边，不能再创建桌子了
	ETEC_TEAM_PLAY_CONFIG = 18,		//快速组局玩法没有配置
	ETEC_TEAM_ADMIN_ERR = 19,		//不是工会的管理者
	ETEC_TEAM_ADMIN_OPT_ERR = 20,	//操作错误
	ETEC_MEM_STATE_ERROR = 21,		// 会员状态不正常
	ETEC_MEM_IN_MATCH = 22,			// 玩家已加入比赛场
	ETEC_DESK_OVER_LIMIT = 23,      // 桌子数超过限制
	ETEC_NO_DESK = 24,              // 没有桌子
	ETEC_GOLD_NOT_LIMIT = 101,      // 钱不够加入桌子
};

// 后台HTTP请求错误码定义 [2017-7-25 willing]
enum E_HTTP_ERQUEST_ERROR_CODE
{
	EHEEC_SUCCESS,				// 成功 [2017-7-25 willing]
	EHEEC_PARAM_ERROR,			// 参数错误 [2017-7-25 willing]
	EHEEC_TEAM_NAME,			// 工会名称重复 [2017-7-25 willing]
	EHEEC_TEAM_NOT_EXIST,		// 工会不存在 [2017-7-25 willing]
	EHEEC_TEAM_CREATEID_ERR,	// 工会的创建者ID不正确 [2017-7-25 willing]
	EHEEC_MEM_OPT_ERR,			// 成员管理的操作码不能识别 [2017-7-25 willing]
	EHEEC_MEM_OPT_NOTALL_SUC,	// 成员管理的操作没有全部成功 [2017-7-27 willing]
	EHEEC_CERATE_TEAM_MAX_COUNT,// 创建的工会已经达到最大限制了 [2017-7-29 willing]
	EHEEC_TEAM_MEM_COUNT,		// 目标工会已经满员 [2017-7-30 willing]
	EHEEC_IS_IN_TEAM,			// 该玩家已经是工会成员 [2017-8-5 willing]
	EHEEC_NOT_FOUND_USER,		// 目标玩家不存在 [2017-8-5 willing]
	EHEEC_JOIN_TEAM_MAX,		// 加入俱乐部个数达到上限
	EHEEC_NOT_HAS_MANAGER,		// 创建工会时没有managerID
	EHEEC_NOT_HAS_COUNTY,       // 创建公会时没有countyId
	EHEEC_NOT_HAS_DESK_CNT_LIMIT, //创建工会时没有deskCntLimit
	EHEEC_NOT_HAS_COUNTY_CIRCLE_GOLD_LIMIT, //创建工会时没有circleGoldLimit
};

// 工会成员对工会的操作类型 [2017-8-9 willing]
enum E_TEAM_MEM_OPT_TYPE
{
	ETMOT_REQ_JOIN_TEAM,		// 申请加入工会 [2017-8-9 willing]
	ETMOT_REQ_LEAVE_TEAM,		// 申请离开工会 [2017-8-9 willing]
	ETMOT_ALLOW_JOIN_TEAM,		// 会长同意了加入申请 [2017-8-9 willing]
	ETMOT_REJECT_JOIN_TEAM,		// 会长拒绝了成员的加入申请 [2017-8-9 willing]
	ETMOT_REJECT_LEAVE_TEAM,	// 会长拒绝了您的离开申请 [2017-8-9 willing]
	ETMOT_ALLOW_LEAVE_TEAM,		// 会长同意了您的离开申请 [2017-8-9 willing]
	ETMOT_RMV_FROM_TEAM,		// 会长将您请离了工会 [2017-8-9 willing]
	ETMOT_TEAM_ADD_MEM,			// 会长主动添加玩家 [2017-8-10 willing]
	ETMOT_CANCEL_JOIN_TEAM,		// 玩家撤销了 加入工会申请 [2017-8-10 willing]
};

//后台修改具体数值
enum E_TEAM_INFO_OPT_TYPE
{
	ETMOT_all,		// 全部修改
	ETMOT_playTypes,		// 修改具体数值
	ETMOT_deskCntLimit,		// 修改具体数值
	ETMOT_boardInfo,	// 修改具体数值
	ETMOT_teamName,		// 修改具体数值
	ETMOT_playTypes_deskCntLimit,		// 修改具体数值
	ETMOT_deskProportionate,// 修改具体数值
};