#ifndef _L_MSG_H_
#define _L_MSG_H_

#include "LSocket.h"
#include "LSingleton.h"
#include "LTool.h"
#include "LLog.h"
#include "LVideo.h"
#include "defines.h"
#include "GameDefine.h"

//读取vector;
static void ReadMapVector(msgpack::object & obj, const char * name, std::vector<Lint> & out)
{
	msgpack::object array;
	ReadMapData(obj, name, array);
	if (array.type == msgpack::type::ARRAY)
	{
		for (Lsize i = 0; i < array.via.array.size; ++i)
		{
			Lint v;
			msgpack::object& obj =  *(array.via.array.ptr+i);
			obj.convert(v);
			out.push_back(v);
		}
	}
}


struct LMsg;

class LMsgFactory : public LSingleton<LMsgFactory>
{
public:
	virtual bool Init();

	virtual bool Final();

	void RegestMsg(LMsg* pMsg);

	LMsg* CreateMsg(int msgId);

private:
	std::map<int,LMsg*> m_msgMap;
};


struct LMsg
{
	LMsg(int id);
	virtual~LMsg();

	LBuffPtr		GetSendBuff();
	void			WriteMsgId(LBuff& buff);
	//void			ReadMsgId(LBuff& buff);

	virtual bool	Write(LBuff& buff);
	virtual bool	Write(msgpack::packer<msgpack::sbuffer>& pack);
	virtual bool	Read(LBuff& buff);
	virtual bool	Read(msgpack::object& obj);
	virtual LMsg*	Clone();
	virtual Lint	preAllocSendMemorySize();

	LBuffPtr			m_send;
	LSocketPtr			m_sp;
	Lint				m_msgId;
	bool				m_isMsgPack;
};

//客户端 服务器交互的消息;
struct LMsgSC :public LMsg
{
	LMsgSC(int id) :LMsg(id)
	{
		m_isMsgPack = true;
	}
};
//玩家状态信息定义;
enum LGUserMsgState
{
	LGU_STATE_IDLE,			//空闲，发到Center;
	LGU_STATE_CENTER,		//玩家在大厅，发到Center;
	LGU_STATE_DESK,			//玩家在桌子，发到Logic;
};
//服务器端跟客户端通讯的消息号
enum LMSG_ID
{
	MSG_ERROR_MSG = 0,//错误消息;

	MSG_C_2_S_MSG = 1,//客户端发给服务器;
	MSG_S_2_C_MSG = 2,//服务器返回结果;

	
	MSG_C_2_S_RE_LOGIN =	10, //客户端请求重连;
	MSG_C_2_S_LOGIN =	11, //客户端请求登录;
	MSG_S_2_C_LOGIN =	12, //服务器返回客户登录结果;

	MSG_S_2_C_ITEM_INFO = 13,  //返回玩家物品信息;
	MSG_S_2_C_HORSE_INFO = 14,  //跑马灯信息;

	MSG_C_2_S_HEART = 15, //心跳包;
	MSG_S_2_C_HEART = 16, //心跳包;

	MSG_C_2_S_NOTICE = 17, //请求服务器通知;
	MSG_S_2_C_NOTICE = 18, //服务器返回通知;

	//vip场相关消息;
	MSG_C_2_S_CREATE_ROOM = 20,//玩家请求创建房间;
	MSG_S_2_C_CREATE_ROOM = 21,//玩家请求创建房间结果;

	MSG_C_2_S_ADD_ROOM = 22,//玩家请求加入房间;
	MSG_S_2_C_ADD_ROOM = 23,//玩家请求加入房间结果;

	MSG_C_2_S_LEAVE_ROOM = 24,//玩家请求退出房间;
	MSG_S_2_C_LEAVE_ROOM = 25,//玩家请求退出房间结果;

	MSG_C_2_S_RESET_ROOM = 26,//申请解算房间;
	MSG_S_2_C_RESET_ROOM = 27,// 服务器广播申请解算房间结果;

	MSG_C_2_S_RESET_ROOM_SELECT = 28,//玩家执行申请操作;
	
	

	MSG_S_2_C_INTO_DESK = 30,//服务器通知玩家进入桌子;
	MSG_S_2_C_ADD_USER = 31,//桌子添加玩家;
	MSG_S_2_C_DEL_USER = 32,//桌子删除玩家;
	MSG_S_2_C_DESK_USER_SCORE_CHARGE = 33, //桌子上玩家分数变化，充值
	MSG_S_2_C_BROCAST_USER_NOGOLD = 34,  //通知玩家充值

	MSG_S_2_C_DESK_STATE = 35,//发送桌子状态;

	MSG_C_2_S_READY = 36,//客户端玩家准备;
	MSG_S_2_C_READY = 37,//服务器广播某个玩家准备;

	MSG_S_2_C_USER_ONLINE_INFO = 40,//服务器广播玩家在线状态;

	MSG_S_2_C_VIP_INFO = 41,//同步桌子信息;

	MSG_C_2_S_REQUEST_SHARE_CARD = 43,		//客户端请求抽卡
	MSG_S_2_C_FINISH_SHARE_CARD = 44,       //广播客户端抽卡结果

	MSG_S_2_C_START = 50,//服务器发送游戏开始消息;
	MSG_S_2_C_GET_CARD = 51,//服务器发送玩家摸牌，准备出牌的消息;
	
	MSG_C_2_S_PLAY_CARD = 52,//客户端请求出牌;
	MSG_S_2_C_PLAY_CARD = 53,//服务器广播玩家出牌;

	MSG_S_2_C_USER_THINK = 54,//服务器通知玩家思考;

	MSG_C_2_S_USER_OPER = 55,//玩家操作结果;
	MSG_S_2_C_USER_OPER = 56,//服务器广播玩家操作结果;

	MSG_C_2_S_USER_SPEAK = 57,//玩家请求说话;
	MSG_S_2_C_USER_SPEAK = 58,//服务器广播玩家说话;

	MSG_S_2_C_ONLYONE_CARD = 59,//服务器广播玩家报单;

	MSG_S_2_C_GAME_OVER = 60,//服务器通知本场游戏结束;

	MSG_C_2_S_USER_OUT_ROOM = 61,//玩家请求退出房间;
	MSG_S_2_C_USER_OUT_ROOM = 62,//玩家退出房间结果;
	MSG_S_2_C_USER_OPER_TING = 63,//玩家听牌


	MSG_S_2_C_START_HU = 65,//玩家起手胡;
	MSG_C_2_S_START_HU_SELECT = 66,//玩家选择起手胡;
	MSG_S_2_C_START_HU_SELECT = 67,//玩家选择起手胡结果;
	
	MSG_S_2_C_GANG_RESULT = 68,//玩家杠牌结果;

	MSG_S_2_C_END_CARD_SELECT = 69,//服务器广播海底牌选择;
	MSG_C_2_S_END_SELECT = 70,//玩家选择结果;
	MSG_S_2_C_END_GET = 71,//服务器广播玩家摸海底牌;

	///////////////chang sha ////////////////////
	MSG_S_2_C_ZHA_BIRD = 72,//服务器广播扎鸟结果
	MSG_S_2_C_Wang_Ba = 73,//服务器广播王霸

	///////////////chang sha ////////////////////

	MSG_S_2_C_VIP_END = 80,//游戏结束;

	MSG_C_2_S_VIP_LOG = 90,//玩家请求vip房间对账单;
	MSG_S_2_C_VIP_LOG = 91,//玩家请求vip房间对账单结果;

	MSG_C_2_S_ONE_VIP_LOG = 1110,//玩家请求某轮对账单;
	MSG_S_2_C_ONE_VIP_LOG = 1111,//玩家请求某轮对账单结果;

	MSG_C_2_S_VIDEO = 92,//玩家请求录像;
	MSG_S_2_C_VIDEO = 93,//玩家请求录像;

	MSG_S_2_C_ACTIVE_INFO = 94,				//通知客户端活动状态;
	MSG_C_2_S_REQUEST_DRAW = 95,			//客户端请求抽卡;
	MSG_S_2_C_FINISH_DRAW = 96,				//通知客户端抽卡结果;
	MSG_C_2_S_WRITE_PHONE = 97,				//客户端请求写入电话;
	MSG_S_2_C_WRITE_PHONE = 98,				//通知客户端写入电话结果;
	MSG_C_2_S_ACTIVE_LOG = 99,				//客户端请求活动日志;
	MSG_S_2_C_ACTIVE_LOG = 100,				//返馈客户端参与活动日志;
	MSG_C_2_S_ACTIVE_INFO = 101,			//客户端请求活动信息;
	MSG_S_2_C_ACTIVE_OPEN = 102,			//通知客户端当前是否有活动;

    MSG_C_2_S_SHAREVIDEO = 106,//玩家请求查看某shareid的分享录像;
	MSG_S_2_C_SHAREVIDEO = 107,//服务器返回某shareid对应的分享录像;
	MSG_C_2_S_REQSHAREVIDEO = 108,//玩家请求分享某录像;
	MSG_S_2_C_RETSHAREVIDEOID = 109,//服务器返回分享的id;
	MSG_C_2_S_GETSHAREVIDEO = 110,//玩家通过精确的videoid查看分享的某录像;



	MSG_S_2_C_CARD_LEFT = 103,//游戏结束服务器通知玩家剩余牌信息;
	MSG_S_2_C_SAMEIP    = 105,//游戏开始广播有相同IP玩家消息;

	MSG_S_2_C_GENGZHUANG_SUCC = 112,//服务器广播玩家跟庄成功
	//MSG_S_2_C_USER_CHARGE_DIAMOND = 112,	// 通知客户端充值结果;
	MSG_S_2_C_USER_PURCHASE = 113,			// 通知客户端购买道具结果;
	MSG_S_2_C_PLAYER_TYPE =114,				// 通知客户端玩家类型;

	MSG_S_2_C_ASK_DIZHU = 115,              // 通知客户端抢地主;
	MSG_C_2_S_QIANG_DIZHU = 116,            // 客户端返回抢地主结果;
	MSG_S_2_C_ANS_DIZHU = 117,              // 服务器广播客户端操作;
	MSG_S_2_C_WHO_IS_DIZHU = 118,           // 服务器广播最终地主位置;
	MSG_S_2_C_DI_PAI = 119,                 // 服务器广播底牌消息;
	MSG_S_2_C_QDZQUERY_NEW = 1115,          // 四川抢地主相关消息，具体消息看子消息号;
	MSG_S_2_C_QDZOPER_NEW = 1116,           // 四川抢地主相关消息，具体消息看子消息号;
	MSG_C_2_S_QDZOPER_NEW = 1117,           // 四川抢地主相关消息;
	MSG_S_2_C_SHOWCARDS = 1118,             //展示玩家自己的手牌;

	MSG_C_2_S_COUPONS_EXCHANGE = 120,//玩家请求兑换窗口信息
	MSG_S_2_C_COUPONS_EXCHANGE = 121,//服务器返回兑换窗口信息
	MSG_C_2_S_COUPONS_EXCHANGE_RECORD = 122,//玩家请求兑换记录
	MSG_S_2_C_COUPONS_EXCHANGE_RECORD = 123,//服务器返回兑换记录
	MSG_C_2_S_GIFT_EXCHANGE = 124,//客户端请求兑换商品
	MSG_S_2_C_GIFT_EXCHANGE = 125,//服务器返回兑换商品结果
	MSG_C_2_S_GET_TASK = 126,			// 请求获取任务信息
	MSG_S_2_C_TASK_LIST = 127,			// 服务器返回任务信息
	MSG_C_2_S_RECV_TASK_REWARD = 128,	// 请求领取任务奖励
	MSG_S_2_C_RECV_TASK_REWARD = 129,	// 返回任务奖励领取结果
	MSG_C_2_S_GET_USER_INFO = 130,		// 请求获取指定玩家的信息
	MSG_S_2_C_USER_INFO = 131,			// 服务器返回玩家信息
	MSG_C_2_S_GET_INVITING_INFO = 132,	// 请求获取邀请信息
	MSG_S_2_C_INVITING_INFO = 133,		// 服务器返回邀请信息
	MSG_C_2_S_BIND_INVITER = 134,		// 请求绑定邀请人
	MSG_S_2_C_BIND_INVITER = 135,		// 服务器返回绑定邀请人结果
	MSG_C_2_S_USER_SHARED = 136,		// 玩家进行了分享
	MSG_C_2_S_REQ_LUCKY_DRAW_NUM = 137,	// 请求玩家抽奖次数
	MSG_S_2_C_LUCKY_DRAW_NUM = 138,		// 服务器推送玩家抽奖次数

	//十一活动
	MSG_C_2_S_REQ_SIGN_COLLECT = 139,	//请求,签到, 集字（执行）
	MSG_S_2_C_RES_SIGN = 140,	//响应签到
	MSG_S_2_C_RES_COLLECT = 141,	//响应集字

	MSG_C_2_S_REQ_SIGN_COLLECT_WIN_STATE = 142,	//请求签到状态（查询）
	MSG_S_2_C_RES_SIGN_STATE = 143,	//响应签到状态;
	MSG_S_2_C_RES_COLLECT_STATE = 144,	//响应集字状态;
	MSG_S_2_C_RES_WIN_STATE = 145,	//响应中奖状态;

	MSG_C_2_S_ASKGOLDDESK_STATE = 1480,                    //玩家在金币场相关请求操作;
	//任务
	MSG_S_2_C_REWARD_NOT_RECEIVED = 146,	// 通知玩家有奖励未领取

	MSG_C_2_S_WEALTH_CHANGE = 147,             // 玩家申请道具兑换;
	MSG_S_2_C_WEALTH_CHANGE_RESULT = 148,      // 玩家申请道具兑换结果;


	//////////////////////////////////////////////////////////////////////////
	//client 跟 server 之间消息交互;
	//////////////////////////////////////////////////////////////////////////
	MSG_C_2_S_LOGIN_CENTER = 150,//客户端请求登录中心服务器;
	MSG_S_2_C_LOGIN_CENTER = 150,//客户端请求登录中心服务器;

	MSG_C_2_S_LOGIN_SERVER = 160,//客户端请求登录服务器;
	MSG_S_2_C_LOGIN_SERVER = 160,//客户端请求登录服务器;

	MSG_S_2_C_CANTING = 166,	//服务器告诉客户端可以听牌 并发送打出哪张牌可以听哪些牌

	///////////////////////////长沙麻将协议begin//////////////

	//MSG_S_2_C_Piao_Select = 118,//请求玩家选择飘
	//MSG_C_2_S_Piao_Select = 119,//玩家选择飘结果
	//MSG_S_2_C_Piao_Count = 143,//广播飘结果

	MSG_S_2_C_Piao_Select = 161,//请求玩家选择飘
	MSG_C_2_S_Piao_Select = 162,//玩家选择飘结果
	MSG_S_2_C_Piao_Count = 163,//广播飘结果

	//MSG_S_2_C_GANG_THINK = 117,//玩家杠牌思考
	MSG_S_2_C_GANG_THINK = 164,//玩家杠牌思考

	//MSG_S_2_C_GHOST_CARD = 161,//服务器广播鬼牌
	MSG_S_2_C_GHOST_CARD = 165,//服务器广播鬼牌

	MSG_S_2_C_SPORT_TIPS = 176,//竞技的提示

	MSG_S_2_C_ATTACK_MANAGED = 187,   //玩家获得托管状态和取消托管状态
	MSG_C_2_S_CANCEL_MANAGED = 188,   //请求取消托管

	///////////////////////////长沙麻将end///////////////////

	//////////////////////////广东麻将///////////////////////

	//MSG_C_2_S_INTERACTION = 114,//玩家交互
	//MSG_S_2_C_INTERACTION = 115,//玩家交互
	MSG_C_2_S_INTERACTION = 210,//玩家交互
	MSG_S_2_C_INTERACTION = 211,//玩家交互

	MSG_S_2_C_MATCH_INFOTXT = 212,		//比赛场相关浮动消息
	MSG_C_2_S_TUOGUAN = 213,		//玩家操作托管信息.
	MSG_S_2_C_TUOGUAN_INFO = 214,		//托管状态信息.

	//MSG_C_2_S_SIGNIN_REQ = 162, //签到请求（包括补签)
	//MSG_S_2_C_SIGNIN_RES = 163, //签到回复
	//MSG_C_2_S_SIGNIN_INFO_REQ = 164, //用户当前签到信息请求
	//MSG_S_2_C_SIGNIN_INFO_RES = 165, //用户当前签到信息回复

	MSG_C_2_S_SIGNIN_REQ = 215, //签到请求（包括补签)
	MSG_S_2_C_SIGNIN_RES = 216, //签到回复
	MSG_C_2_S_SIGNIN_INFO_REQ = 217, //用户当前签到信息请求
	MSG_S_2_C_SIGNIN_INFO_RES = 218, //用户当前签到信息回复

	//////////////////////////广东麻将end////////////////////


	//MaYouQuan begin
	//////////////////////////////////////////////////////////////////////////
	MSG_C_And_T_Begin = 300,			// 客户端和工会服务器通信消息ID的最小值 [2017-7-17 willing]
	MSG_S_2_C_NOTICE_EVENT = 305,		// 服务器通知会长对某个玩家的操作 [2017-7-20 willing]
	MSG_C_2_S_REQ_TEAM_LIST = 306,		// 玩家请求自己所在的工会列表 [2017-7-20 willing]
	MSG_S_2_C_TEAM_LIST = 307,			// 向客户端返回工会列表 [2017-7-20 willing]
	MSG_C_2_S_TEAM_INFO = 308,			// 获取工会详情信息 [2017-7-21 willing]
	MSG_S_2_C_Team_Info = 309,			// 返回工会详情信息 [2017-7-21 willing]
	MSG_C_2_S_Member_List = 310,		// 请求工会成员列表 [2017-7-21 willing]
	MSG_S_2_C_Member_List = 311,		// 返回工会成员列表 [2017-7-21 willing]
	MSG_C_2_S_Cancel_Join = 312,		// 取消入会申请 [2017-7-21 willing]
	MSG_S_2_C_Cancel_Join = 313,		// 取消入会申请的结果 [2017-7-21 willing]
	MSG_S_2_C_Notice_Dismiss = 314,		// 通知所有玩家，所在的工会被会长解散了 [2017-7-22 willing]
	MSG_C_2_S_Leave_Team = 315,			// 申请离开工会 [2017-7-22 willing]
	MSG_S_2_C_Leave_Team_Rst = 316,		// 申请离开工会的结果 [2017-7-22 willing]
	MSG_S_2_C_CreateDeskError = 317,	// 通过工会创建桌子失败 [2017-7-23 willing]
	MSG_S_2_C_TeamInfoChange = 318,		// 工会信息改变 [2017-7-23 willing]
	MSG_C_2_S_GET_VIDEO_LOG = 319,		// 查看成员的战绩 [2017-7-24 willing]
	MSG_S_2_C_VIDEO_LOG = 320,			// 服务器返回玩家战绩 [2017-7-24 willing]
	MSG_S_2_C_MemberInfoChange = 321,	// 成员信息改变 [2017-7-25 willing]
	MSG_S_2_C_NEW_TEAM_DESK = 322,		// 通知工会成员新创建了一个桌子 [2017-8-3 willing]
	MSG_S_2_C_TeamDeskUserChg = 323,	// 工会桌子中的人员变化了 [2017-8-3 willing]
	MSG_S_2_C_DelTeamDesk = 324,		// 删除一个工会桌子，被解散或者已经结束 [2017-8-3 willing]
	MSG_C_2_S_GetAllTeamDesk = 325,		// 客户端请求当前所有的工会桌子 [2017-8-3 willing]
	MSG_S_2_C_ALLTeamDesk = 326,		// 所有工会中的桌子 [2017-8-3 willing]
	MSG_C_2_S_SubscribeTeamDesk = 327,	// 订阅桌子变化信息 [2017-8-3 willing]
	MSG_C_2_S_GetTeamOptLog = 328,		// 玩家查询自己对工会的操作日志 [2017-8-8 willing]
	MSG_S_2_C_MemTeamOptLog = 329,		// 服务器返回玩家对工会的操作日志 [2017-8-8 willing]
	MSG_C_2_S_GET_RANGE_VIDEO_LOG = 330,// 获取指定时间内的对局日志 [2017-8-10 willing]
	MSG_C_2_S_CREATE_KUAISU_JU = 331,	// 玩家创建快速组局
	MSG_C_2_S_FAST_PLAY = 332,			//快速组局
	MSG_S_2_C_FAST_PLAY = 333,			//返回快速组局结果
	MSG_C_2_S_OPT_MEM = 334,			//会主批复 申请操作
	MSG_S_2_C_OPT_MEM_RET = 335,		//返回批复处理结果
	MSG_S_2_C_PLAY_CONFIG = 336,		//服务器返回群主创建的快速组局玩法配置
	MSG_C_2_S_OPT_TEAMINFO = 337,		//群主修改工会昵称、公告
	MSG_C_2_S_ADMIN_IVITE = 338,		// 群主通过邀请玩家加入工会
	MSG_S_2_C_ADMIN_IVITE_RET = 339,	// 群主通过邀请玩家加入工会处理结果
	MSG_C_2_S_QueryTeam = 340,			// 客户端查询工会信息 [2017-7-17 willing]
	MSG_S_2_C_TeamInfo = 341,			// 服务器返回 工会信息 [2017-7-18 willing]
	MSG_S_2_C_JOIN_RST = 342,			// 申请加入工会的结果 [2017-7-19 willing]
	MSG_C_2_S_REQ_JOIN = 343,			// 申请加入工会 [2017-7-19 willing]
	MSG_C_2_S_REQ_EXCHAGEGOLD = 344, // 请求兑换金币
	MSG_S_2_C_REQ_EXCHAGEGOLDD = 345, // 请求兑换金币 结果
	MSG_C_2_S_REQ_EXCHAGEGOLD_LOG = 346, // 请求最近兑换金币 记录
	MSG_S_2_C_EXCHAGEGOLD_OPT_LOG = 347, // 金币兑换日志
	MSG_C_2_S_CANNCEL_EXCHAGEGOLD = 348, // 取消兑换
	MSG_S_2_C_CANNCEL_EXCHAGEGOLD = 349, // 取消兑换
	MSG_C_2_S_INVITE_INROOM = 350, // 邀请加入
	MSG_S_2_C_INVITE_INROOM = 351, // 邀请加入其他玩家
	MSG_S_2_C_EXCHAGEGOLD_OPTNOTICE = 352, // 回收金币操作
	MSG_C_And_T_End = 355,		// 客户端和工会服务器通信消息ID的最大值 [2017-7-17 willing]
	
	//MaYouQuan end
	MSG_C_2_S_USE_ITEM = 356, //
	MSG_C_2_S_GMADD_ITEM = 357, //
	//道具系统 end 

	//金币信息
	MSG_S_2_C_SYNC_USER_GOLD_INFO = 400, //同步玩家金币信息
	MSG_S_2_C_SYNC_USER_SELF_GOLD_MATCH_INFO = 401, //类似219消息，通知玩家自己金币场状态
	MSG_C_2_S_CARD_EXCHANGE_GOLD_INFO_LIST_REQ = 402, //客户端房卡兑换金币信息列表
	MSG_S_2_C_CARD_EXCHANGE_GOLD_INFO_LIST_RSP = 403, //返回房卡兑换金币信息列表
	MSG_C_2_S_CARD_EXCHANGE_GOLD_REQ = 404, //客户端请求房卡兑换金币
	MSG_S_2_C_CARD_EXCHANGE_GOLD_RSP = 405, //返回房卡兑换金币结果


	MSG_S_2_C_MAX = 4096, //服务器客户端之间最大消息号;

	//////////////////////////////////////////////////////////////////////////
	//下面这是服务器与服务器之间的消息交互;
	//////////////////////////////////////////////////////////////////////////
	//logic 跟 center 之间的交互;
	MSG_L_2_CE_LOGIN = 6000, //logic登录center;
	
	MSG_L_2_CE_GATE_INFO = 6002, //logic同步gate信息到center;

	MSG_CE_2_L_USER_ID_INFO = 6003, //center同步玩家id信息;

	MSG_CE_2_L_USER_LOGIN = 6004, //center同步玩家请求登录的消息;

	MSG_L_2_CE_USER_SAVE = 6005, //logic同步玩家更新的消息;

	MSG_LMG_2_CE_User_Save_PlayCount = 6408,//logicmanager同步玩家玩的局数;

	MSG_L_2_CE_ROOM_SAVE = 6006, //logic同步更新room的消息;

	MSG_L_2_CE_CARD_SAVE = 6007, //logic同步玩家房卡操作;

	MSG_L_2_CE_USER_RELOGIN = 6008,	//玩家重新登陆;

	MSG_CE_2_L_USER_RELOGIN = 6010,	//玩家重新登陆;

	MSG_L_2_CE_USER_LOGIN = 6011,			//同步Center玩家当前登陆某服务器;

	MSG_L_2_CE_USER_LOGOUT = 6012,			//同步Center玩家当前登出某服务器;

	MSG_CE_2_L_REQ_LOG_INFO = 6013, //center发送玩家请求战绩信息			---已废;

	MSG_L_2_CE_REQ_VIDEO = 6014, //logic发送玩家请求录像					---已废;

	MSG_CE_2_L_REQ_VIDEO = 6015, //center发送玩家请求录像					---已废;

	MSG_CE_2_L_GM_CHARGE = 6020, //center发送gm充值请求;

	MSG_CE_2_L_GM_HORSE = 6021, //center发送gm跑马灯;

	MSG_CE_2_L_GM_BUYINFO = 6022, //center发送gm够买提示;

	MSG_CE_2_L_GM_HIDE = 6023, //center发送gmyincang;
	//6024 - 
	MSG_CE_2_LMG_GM_MODIFY_PLAYERTYPE = 6026,	//center发送gm修改玩家类型;

	MSG_CE_2_LMG_GM_CHARGE_DIAMOND = 6027,	// center发送gm充值钻石;

	MSG_CE_2_LMG_GM_PURCHASE = 6028,			// center发送gm购买道具;

	MSG_LMG_2_CE_CardExchange = 6029,			// logicmananger发送房卡变化;

	MSG_LMG_2_CE_BREATH = 6144,				//逻辑心跳;
	MSG_CE_2_LMG_BREATH = 6145,				//逻辑心跳回复;

	MSG_LMG_2_CE_COUPON_CHANGED = 6401,	// logicmanager通知center玩家的礼券变化了;

	MSG_CE_2_LMG_User_Gift_Info = 6402, //center发给logicmanager礼券信息和礼物信息;

	MSG_LMG_2_CE_User_Coupons_Exchange_Record = 6403, //logicmanager 发给 center 礼券兑换记录;

	MSG_CE_2_LMG_User_Coupons_Exchange_Record = 6404,//center 发给 logicmanager 礼券兑换记录;

	MSG_LMG_2_CE_User_Coupons_Exchange_Info = 6405, //logicmanager 发给 center 礼券兑换消息;

	MSG_CE_2_LMG_User_Coupons_Exchange_Info = 6406,//center 发给 logicmanager 礼券兑换结果;

	MSG_LMG_2_CE_OFFLINE_USER_CARD_CHANGED = 6407, // 通知center不在线玩家房卡数量变化;

	////////////////////////////////////长沙麻将 begin///////////////////////

	MSG_LMG_2_CE_SPORT_RESET_DESK = 6422,       // 请求 比赛场解散房间
	
	///////////////////////////////////长沙麻将 end/////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//gete 跟 logic 之间的交互;
	MSG_G_2_L_LOGIN = 7000, //gate登录logic		由LogicManager转发;
	MSG_G_2_L_BREATH = 7001,			//逻辑心跳;
	MSG_L_2_G_BREATH = 7002,			//逻辑心跳;

	MSG_G_2_L_USER_MSG = 8000, //gate转发玩家消息到logic;
	MSG_L_2_G_USER_MSG = 8001, //logic转发玩家消息到gate;

	MSG_G_2_L_USER_OUT_MSG = 8002, //gate转发玩家退出消息到logic;
	MSG_L_2_G_USER_OUT_MSG = 8003, //logic转发玩家退出消息到gate;

	MSG_L_2_G_MODIFY_USER_STATE = 8006,	//修改玩家状态 gate to logic ;

	MSG_G_2_T_USER_MSG = 8008,		// gate 转发玩家消息到 工会服务器 [2017-7-18 willing]
	MSG_T_2_G_USER_MSG = 8009,		// gate 转发玩家消息到 玩家 [2017-7-18 willing]


//////////////////////////////////////////////////////////////////////////
//LogicManager 与 Logic;
	MSG_L_2_LMG_LOGIN = 11001,				//逻辑管理登陆;
	MSG_L_2_LMG_BREATH = 11002,				//逻辑心跳;
	MSG_LMG_2_L_BREATH = 11003,		//逻辑心跳回复;

	MSG_LMG_2_L_CREATE_DESK = 11011,		//创建房间	只有玩家不在Logic上才创建;
	MSG_LMG_2_L_ADDTO_DESK = 11013,			//加入房间	只有玩家不在Logic上才加入;
	MSG_LMG_2_L_USER_LOGIN = 11016,			//玩家重新登陆;

	MSG_LMG_2_L_INTO_GOLDDESK = 11111,      //金币场玩家进入桌子;

	MSG_L_2_LMG_MODIFY_USER_STATE = 11017,			//修改玩家状态  logicmanager to logic;
	MSG_L_2_LMG_MODIFY_USER_CARD = 11018,			//玩游戏所耗房卡;
	MSG_L_2_LMG_MODIFY_USER_PLAYCOUNT = 11019,		//修改玩家把数;
	MSG_L_2_LMG_RECYLE_DESKID = 11020,				//创建失败，或桌子解散时;

	MSG_L_2_LMG_RECYLE_GOLDDESKPLACE = 11120,				//金币场桌子回收一个空位;   
	MSG_L_2_LMG_USE_GOLDDESKPLACE = 11121,				    //金币场桌子使用一个空位，用于机器人;
	MSG_LMG_2_CE_GOLDCHANGE = 11122,                        //金币增减;
	MSG_L_2_LMG_GOLDCHANGE = 11124,                         //金币变化sever通知LogicManager;
	MSG_S_2_C_GOLD_CHANGE = 11123,                          //通知客户端金币变化;
	MSG_S_2_C_GOLDDESK_STATE = 11126,                       //金币场桌子先关状态变化通知玩家;
	
	//十一活动;
	MSG_L_2_LMG_MODIFY_USER_BASHU = 11021,			//统计把数;
	//任务
	MSG_L_2_LMG_USER_CIRCLE_INCREASED = 11022,		//玩家又玩了一局

	MSG_L_2_LMG_REPORT_HORSE_INFO = 11024,           //上传玩家中马数据
	MSG_LMG_2_L_MODIFY_ROOM_CARD = 11025,           //修改房间里面的金币卡等数目

    ////////////////////////////////////////长沙麻将/////////////////////////
	MSG_L_2_LMG_USER_COMPLETE_GAME = 11032,			//玩家完成一把游戏
	MSG_L_2_LMG_SPORT_RESULT = 11033,      //比赛结果返回

	///////////////////////////////////////长沙麻将 end//////////////////////

	//////////////////////////////////////广东麻将 /////////////////////////
	//竞技场,比赛场, 金币场开始	

	MSG_MA_2_L_CREATE_MATCHDESK = 11900,			//创建比赛场房间 
	MSG_L_2_MA_RETCREATE_MATCHDESK = 11901,			//返回创建桌子结果

	MSG_L_2_MA_DESK_REPRTOVER = 11911,			//Logic报告比赛桌子决算信息
	MSG_MA_2_L_CONTRAL_DESK = 11921,			//match控制桌子, 强退, 结束时间等.
	MSG_L_2_MA_Ret_CONTRAL_DESK = 11922,			//Logic反馈控制结果.
	MSG_LM_2_G_2LS_SYNC_MATCHSERVER = 11930,			//同步比赛场信息

	//////////////////////////////////////广东麻将end//////////////////////

	
	MSG_MA_2_LM_USER_MATCH_INFO = 11940,			//比赛场通知LM玩家 比赛信息

//LogicManager 与 Gate;
	MSG_G_2_LMG_LOGIN = 12001,			//逻辑管理登陆;
	MSG_G_2_LMG_BREATH = 12002,			//逻辑心跳;
	MSG_LMG_2_G_BREATH = 12003,			//逻辑心跳;
	MSG_LMG_2_G_SYNC_LOGIC = 12010,		//同步逻辑信息;
	MSG_LMG_2_G_HORSE_INFO = 12011,		//跑马灯;

////////////////////////////////////////长沙麻将/////////////////////////
	MSG_LMG_2_L_MATCH_ADD_DESK = 12020,		//比赛场创建房间

		MSG_L_2_M_LOGIN = 13005,        //logic 登陆 matchserver

		MSG_LMG_2_G_SYNC_MATCH = 13010,     //Logic 同步网关 MATCH信息
		MSG_LMG_2_L_SYNC_MATCH = 13011,     //Logic 同步LOGIC MATCH信息
///////////////////////////////////////长沙麻将 end//////////////////////

	//////////////////////////////////////////////////////////////////////////
	//logicmanager 和 logicdb 交互消息;

	MSG_LMG_2_LDB_BREATH = 14997,	//心跳;
	MSG_LDB_2_LMG_BREATH = 14998,	//心跳;
	MSG_LMG_2_LDB_LOGIN = 14999,	//LogicManager 登入LogicDB;
	//logic 和 logicdb 直接的消息交互;
	MSG_L_2_LDB_LOGIN = 15000, //logic 登录 logicdb;

	MSG_L_2_LDB_VIP_LOG = 15001,//logic 请求 vip战绩;
	MSG_LDB_2_L_VIP_LOG = 15002,//logicdb 返回 vip战绩;

	MSG_L_2_LDB_ONE_VIP_LOG = 15026,//logic 请求 某轮vip战绩;
	MSG_LDB_2_L_ONE_VIP_LOG = 15027,//logicdb 返回 某轮vip战绩;

	MSG_L_2_LDB_VIDEO  = 15003,//logic 请求 video;
	MSG_LDB_2_L_VIDEO  = 15004,//logicdb 返回 video;

	MSG_L_2_LDB_ROOM_SAVE = 15005, //logic同步更新room的消息;
	MSG_L_2_LDB_VIDEO_SAVE = 15006, //logic同步更新video的消息;

	MSG_L_2_LDB_REQ_ACTIVE = 15007,		//Logic请求活动;
	MSG_LDB_2_L_REQ_ACTIVE = 15008,		//Center返回请求活动;
	MSG_L_2_LDB_SYC_DRAW = 15009,		//Center返回请求活动;
	MSG_L_2_LDB_SYC_PHONE = 15010,		//Center返回请求活动;

	MSG_LDB_2_L_LOG_INFO = 15011, //center同步未完成的桌子信息;
	MSG_L_2_LDB_BREATH = 15012,	//心跳;
	MSG_LDB_2_L_BREATH = 15013,	//心跳;

	MSG_L_2_LDB_GETSHAREVIDEOID = 15020,//logic 玩家请求分享某录像，先去sharevideo中查看是否已被分享;
	MSG_L_2_LDB_GETSHAREVIDEO = 15021,//logic 玩家通过shareid来获取该分享码下的录像 在sharevideo中查询;
	MSG_LDB_2_L_RETSHAREVIDEOID  = 15022,//logicdb返回某录像是否有shareid，没有则将其从video中移至sharevideo中;
	MSG_LDB_2_L_RETSHAREVIDEO  = 15023,//logicdb 返回 通过shareid查询到的可能的录像;
	MSG_L_2_LDB_SAVESHAREVIDEOID = 15024,//logic 请求保存某已转移到sharevideo中的某录像的shareid;
	MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID = 15025, //loigc 通过精确的videoid来查找sharevideo中的对应录像;

	MSG_L_2_LDB_EXEC_SQL = 15030,		// 请求logicDB执行sql语句
	MSG_LMG_2_LDB_REQ_TASK_PROGRESS = 15031,	// logicmanager请求任务进度数据
	MSG_LDB_2_LMG_REQ_TASK_PROGRESS = 15032,	// logicDB返回任务进度
	MSG_LMG_2_LDB_REQ_BINDING_RELATIONSHIPS = 15033,	// logicmanager请求邀请绑定关系信息
	MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS = 15034,	// logicDB返回邀请绑定关系信息
	MSG_LMG_2_LDB_INSERT_TASK_PROGRESS = 15035,			// logicmanager请求插入任务进度数据
	MSG_LDB_2_LMG_INSERT_TASK_PROGRESS = 15036,			// logicDB返回插入任务进度数据结果
	MSG_LMG_2_LDB_REQ_ACTIVE_GIFT_STOCK = 15037,		// logicmanager请求活动的奖品库存等信息
	MSG_LDB_2_LMG_ACTIVE_GIFT_STOCK = 15038,			// logicDB返回活动的奖品库存等信息
	MSG_LMG_2_LDB_REQ_LUCKY_DRAW_NUM = 15039,			// 请求玩家抽奖次数
	MSG_LDB_2_LMG_LUCKY_DRAW_NUM = 15040,				// 返回玩家抽奖次数
	//十一活动
	MSG_L_2_LDB_REQ_SIGN_COLLECT_WIN = 15041,	//请求活动数据
	MSG_LDB_2_L_RES_SIGN_COLLECT_WIN = 15042,	//返回活动数据
	MSG_L_2_LDB_SAVE_SIGN_COLLECT_WIN = 15043,		//保存签到,集字,获奖名单
	//任务
	MSG_LMG_2_LDB_INSERT_BINDING_RELATIONSHIP = 15044,	// 请求插入绑定关系
	MSG_LDB_2_LMG_INSERT_BINDING_RELATIONSHIP = 15045,	// 返回插入绑定关系结果


	/////////////////////////////////////////////////////////////////////////
	// LoginGate与Center之间的通信
	MSG_LG_CE_USER_MSG_WRAPPER = 20000,	// logingate和center之间对用户消息的封装
	MSG_LG_2_CE_HEARTBEAT  = 20001, //gamelogin发给center heartbeat
	//////////////////////////////////////////////////////////////////////////

	MSG_S_2_S_KEEP_ALIVE = 30000,
	MSG_S_2_S_KEEP_ALIVE_ACK = 30001,

	//////////////////////////////////////////////////////////////////////////
	MSG_TS_2_LMG_LOGIN = 40000,		// TeamServer 登陆到 LogicManager [2017-7-10 willing]
	MSG_TS_2_LMG_HEARTBEAT = 40001,	// TeamServer 到 LogicManager 的心跳 [2017-7-10 willing]
	MSG_LMG_2_TS_HEARTBEAT = 40002,	// LogicManager 到 TeamServer 的心跳 [2017-7-10 willing]
	MSG_TS_2_LDB_LOGIN = 40003,		// TeamServer 登陆到 LogicDB [2017-7-10 willing]
	MSG_TS_2_LDB_HEARTBEAT = 40004,	// TeamServer 到 LogicDB 的心跳 [2017-7-10 willing]
	MSG_LDB_2_TS_HEARTBEAT = 40005,	// LogicDB 到 TeamServer 的心跳 [2017-7-10 willing]
	MSG_L_2_TS_LOGIN = 40006,		// LogicServer 登陆到 TeamServer [2017-7-10 willing]
	MSG_L_2_TS_HEARTBEAT = 40007,	// LogicServer 到 TeamServer 的心跳 [2017-7-10 willing]
	MSG_TS_2_L_HEARTBEAT = 40008,	// TeamServer 到 LogicServer 的心跳 [2017-7-10 willing]
	MSG_LMG_2_L_TEAMSERVER = 40009,	// LogicManager 通知 LogicServer 链接 TeamServer [2017-7-11 willing]
	MSG_LMG_2_G_TEAMSERVER = 40010,	// LogicManager 通知 GateServer 链接 TeamServer [2017-7-11 willing]
	MSG_G_2_TS_HEARTBEAT = 40011,	// GateServer 到 TeamServer 的心跳 [2017-7-11 willing]
	MSG_TS_2_G_HEARTBEAT = 40012,	// TeamServer 到 GateServer 的心跳 [2017-7-11 willing]
	MSG_G_2_TS_LOGIN = 40013,		// GateServer 登陆到 TeamServer [2017-7-11 willing]
	MSG_LMG_2_TS_LG_LOGOUT = 40014,	// LogicManager 通知 TeamServer，Logic Gate 掉线 [2017-7-11 willing]
	MSG_LMG_2_TS_USER_LOGIN = 40015,// LogicManager 通知 TeamServer 有玩家登陆 [2017-7-18 willing]
	MSG_LMG_2_TS_USER_LOGOUT = 40016,// LogicManager 通知 TeamServer 有玩家登出 [2017-7-18 willing]
	MSG_LMG_2_TS_CREATE_ROOM = 40017,// LogicManager 通知 TeamServer 创建桌子 [2017-7-23 willing]
	MSG_TS_2_LMG_CREATE_ROOM = 40018,// TeamServer 通知 LogicManager 检查桌子的创建结果 [2017-7-23 willing]
	MSG_LMG_2_TS_GIVE_BACK = 40019,	 // 通知TeamServer 退还 预扣的房卡 [2017-7-23 willing]
	MSG_L_2_TS_DEL_CARD = 400020,	 // LogicServer 通知 TeamServer 真正扣卡 [2017-7-10 willing]
	MSG_L_2_TS_VIDEO_LOG = 400021,   // LogicServer 通知 TeamServer 记录 牌局日志[2017-7-23 willing]
	MSG_L_2_TS_ADD_DESK = 400022,    // LogicServer 通知 TeamServer 有玩家请求加入桌子[2017-7-23 willing]
	MSG_TS_2_L_ADD_DESK_RET = 400023,// TeamServer 通知 LogicServer 是否可以加入桌子[2017-7-23 willing]
	MSG_TS_2_LMG_TEAM_VIDEO_LOG = 400024,	// TeamServer 把玩家获取的 工会战绩信息发送给 LogicManager，信息还要做填充 [2017-7-24 willing]
	MSG_L_2_TS_GAME_STATE = 400025,	 // LogicServer 把玩家进入游戏，离开游戏 的状态发送给 TeamServer [2017-7-25 willing]
	MSG_TS_2_LMG_GET_USER_INFO = 400026,// TeamServer 获取玩家的昵称和URL [2017-7-26 willing]
	MSG_LMG_2_TS_USER_INFO = 400027,	// LogicManager 返回玩家信息 [2017-7-26 willing]
	MSG_L_2_TS_NEW_TEAM_DESK = 400028,	// LogicServer 创建了一个新的俱乐部桌子 [2017-8-3 willing]
	MSG_L_2_TS_TEAM_DESK_USER_CHG = 400029,	// LogicServer 通知桌子人员改变 [2017-8-3 willing]
	MSG_L_2_TS_DEL_TEAM_DESK = 400030,	// LogicServer 通知删除桌子 [2017-8-3 willing]
	MSG_TS_2_LMG_QUERY_IS_ONLINE = 400031,// TeamServer 获取玩家是否在线 [2017-7-26 willing]
	MSG_LMG_2_TS_USER_ONLINE_RET = 400032,// LogicManager 返回玩家的在线状态 [2017-7-26 willing]
	MSG_LMG_2_TS_RecycleDeskId = 400033,// LogicManager 通知 TeamServer 回收了桌子 [2017-7-26 willing]
	MSG_LMG_2_TS_ADD_ROOM = 40034,// LogicManager 通知 TeamServer 加入桌子 [2017-7-23 willing]
	MSG_TS_2_LMG_ADD_ROOM = 40035,// TeamServer 通知 LogicManager 检查桌子的加入结果 [2017-7-23 willing]
	MSG_TS_2_LMG_REQ_DESKID = 40036,		//TS请求桌子id
	MSG_LMG_2_TS__REQ_DESKID_REQ = 40037,   //LM返回桌子id
	MSG_TS_2_LMG_REQ_EXCHAGEGOLD = 40038,   //TS请求兑换金币
	MSG_LMG_2_TS_REQ_EXCHAGEGOLDM = 40039,   //LM返回 TS请求兑换金币
	MSG_TS_2_LMG_USER_LOGIN = 40040,   //TS返回 有玩家登陆 结果
	MSG_TS_2_LMG_USER_IN_LOGIC = 40041,   //TS返回 进入逻辑

	//add by arena   MsgID >= 200 && MsgID <= 300 Just for Arena;
	MSG_MIN_ARENA_MSGID = 200,
	MSG_C_2_S_INTO_ARENA  = 201,
	MSG_S_2_C_R4_INTO_ARENA = 202,
	MSG_C_2_S_QUIT_ARENA = 203,
	MSG_S_2_C_R4_QUIT_ARENA = 204,
	MSG_MAX_ARENA_MSGID = 300,
	//add end 
};

//////////////////////////////////////////////////////////////////////////
//socket断开的消息;
#define MSG_CLIENT_KICK 0x7FFFFFFF

struct LMsgKick:public LMsg
{
	LMsgKick():LMsg(MSG_CLIENT_KICK){}

	virtual LMsg* Clone()
	{
		return new LMsgKick();
	}
};

//////////////////////////////////////////////////////////////////////////
//所有socket连上的消息;
#define MSG_CLIENT_IN 0x7FFFFFFE

struct LMsgIn:public LMsg
{
	LMsgIn():LMsg(MSG_CLIENT_IN){}

	virtual LMsg* Clone()
	{
		return new LMsgIn();
	}
};

//////////////////////////////////////////////////////////////////////////
//http消息号
#define MSG_HTTP 0x7FFFFFFD

struct LMsgHttp:public LMsg
{
	std::string m_head;//消息头
	std::string m_body;//消息体

	LMsgHttp():LMsg(MSG_HTTP){}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_head);
		buff.Write(m_body);
		return true;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_head);
		buff.Read(m_body);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgHttp();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送消息
struct LMsgC2SMsg :public LMsgSC
{
	Lstring			m_openId;
	Lstring			m_nike;
	Lstring			m_sign;
	Lstring			m_plate;
	Lstring			m_accessToken;
	Lstring			m_refreshToken;
	Lstring			m_md5;
	Lint			m_severID;
	Lstring			m_uuid;
	Lint			m_sex;
	Lstring			m_imageUrl;
	Lstring			m_nikename;
	Lint			m_wxConfirmed = 0;
	Lstring         m_location = "";
	Lstring         m_address = "";
	Lstring			m_passwd;//密码
	Lint			m_nTeamId = 0;//俱乐部id

	LMsgC2SMsg() :LMsgSC(MSG_C_2_S_MSG), m_severID(0), m_sex(0)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_openId", m_openId);
		ReadMapData(obj, "m_nike", m_nike);
		ReadMapData(obj, "m_sign", m_sign);
		ReadMapData(obj, "m_plate", m_plate);
		ReadMapData(obj, "m_accessToken", m_accessToken);
		ReadMapData(obj, "m_refreshToken", m_refreshToken);
		ReadMapData(obj, "m_md5", m_md5);
		ReadMapData(obj, "m_severID", m_severID);
		ReadMapData(obj, "m_uuid", m_uuid);
		ReadMapData(obj, "m_sex", m_sex);
		ReadMapData(obj, "m_imageUrl", m_imageUrl);
		ReadMapData(obj, "m_nikename", m_nikename);
		ReadMapData(obj, "m_wxConfirmed", m_wxConfirmed);
		ReadMapData(obj, "m_location", m_location);
		ReadMapData(obj, "m_address", m_address);
		ReadMapData(obj, "m_passwd", m_passwd);
		ReadMapData(obj, "m_nTeamId", m_nTeamId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,18);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_openId", m_openId);
		WriteKeyValue(pack, "m_nike", m_nike);
		WriteKeyValue(pack, "m_sign", m_sign);
		WriteKeyValue(pack, "m_plate", m_plate);
		WriteKeyValue(pack, "m_accessToken", m_accessToken);
		WriteKeyValue(pack, "m_refreshToken", m_refreshToken);
		WriteKeyValue(pack, "m_md5", m_md5);
		WriteKeyValue(pack, "m_severID", m_severID);
		WriteKeyValue(pack, "m_uuid", m_uuid);
		WriteKeyValue(pack, "m_sex", m_sex);
		WriteKeyValue(pack, "m_imageUrl", m_imageUrl);
		WriteKeyValue(pack, "m_nikename", m_nikename);
		WriteKeyValue(pack, "m_wxConfirmed", m_wxConfirmed);
		WriteKeyValue(pack, "m_location", m_location);
		WriteKeyValue(pack, "m_address", m_address);
		WriteKeyValue(pack, "m_passwd", m_passwd);
		WriteKeyValue(pack, "m_nTeamId", m_nTeamId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SMsg();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送消息
struct LMsgS2CMsg :public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-服务器还没启动成功 2-微信登陆失败 3- 微信返回失败 4-创建角色失败 5- 在原APP未退
	Lint			m_seed;
	Lint			m_id;
	Lstring			m_gateIp;
	Lshort			m_gatePort;
	Lint            m_totalPlayNum;
	Lstring			m_errorMsg;	//若登陆失败，返回玩家错误原因;

	Lint			m_sex; //account 登录 性别
	Lstring			m_imageUrl;
	Lstring			m_nikename;


	LMsgS2CMsg() :LMsgSC(MSG_S_2_C_MSG), m_errorCode(0), m_seed(0), m_id(0), m_gatePort(0), m_sex(0)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, "m_errorCode", m_errorCode);
		ReadMapData(obj, "m_seed", m_seed);
		ReadMapData(obj, "m_id", m_id);
		ReadMapData(obj, "m_gateIp", m_gateIp);
		ReadMapData(obj, "m_gatePort", m_gatePort);
		ReadMapData(obj, "m_errorMsg", m_errorMsg);
		ReadMapData(obj, "m_totalPlayNum", m_totalPlayNum);
		ReadMapData(obj, "m_sex", m_sex);
		ReadMapData(obj, "m_imageUrl", m_imageUrl);
		ReadMapData(obj, "m_nikename", m_nikename);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 11);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_errorCode", m_errorCode);
		WriteKeyValue(pack, "m_seed", m_seed);
		WriteKeyValue(pack, "m_id", m_id);
		WriteKeyValue(pack, "m_gateIp", m_gateIp);
		WriteKeyValue(pack, "m_gatePort", m_gatePort);
		WriteKeyValue(pack, "m_errorMsg", m_errorMsg);
		WriteKeyValue(pack, "m_totalPlayNum", m_totalPlayNum);
		WriteKeyValue(pack, "m_sex", m_sex);
		WriteKeyValue(pack, "m_imageUrl", m_imageUrl);
		WriteKeyValue(pack, "m_nikename", m_nikename);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CMsg();
	}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送心跳包
struct LMsgC2SHeart:public LMsgSC
{
	//Lint	m_time;//时间戳

	LMsgC2SHeart() :LMsgSC(MSG_C_2_S_HEART){}

	virtual bool Read(msgpack::object& obj)
	{
		//buff.Read(m_time);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		//buff.Write(m_time);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SHeart();}
};


//////////////////////////////////////////////////////////////////////////
//服务器发送心跳包
struct LMsgS2CHeart :public LMsgSC
{
	Lint	m_time;//时间戳

	LMsgS2CHeart() :LMsgSC(MSG_S_2_C_HEART),m_time(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CHeart(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求通知
struct LMsgC2SNotice :public LMsgSC
{
	
	LMsgC2SNotice() :LMsgSC(MSG_C_2_S_NOTICE){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SNotice(); }
};


//////////////////////////////////////////////////////////////////////////
//服务器返回通知内容
struct Notice
{
	Notice(): m_time(0){}
	Lint	m_time;//时间戳
	Lstring	m_title;//标题
	Lstring m_content;//内容

	MSGPACK_DEFINE(m_time, m_title, m_content); 
};

struct LMsgS2CNotice :public LMsgSC
{
	Lint	m_count;//时间戳
	Notice	m_data[20];

	LMsgS2CNotice() :LMsgSC(MSG_S_2_C_NOTICE), m_count(0){}

	virtual bool Read(msgpack::object& obj)
	{
		m_count = 0;

		msgpack::object array;
		ReadMapData(obj, "array", array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object pv = *(array.via.array.ptr+i);
				pv.convert(m_data[i]);
				m_count++;
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKey(pack, "array");
		std::vector<Notice>vce;
		for (Lint i = 0; i < m_count&&i < 20; ++i)
		{
			vce.push_back(m_data[i]);
		}
		WriteKey(pack,vce);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CNotice(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端重连
struct LMsgC2SReLogin:public LMsgSC
{
	Lint	m_seed;
	Lint	m_id;
	Lstring	m_md5;

	LMsgC2SReLogin() :LMsgSC(MSG_C_2_S_RE_LOGIN), m_seed(0), m_id(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_seed), m_seed);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_md5), m_md5);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_seed), m_seed);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SReLogin();
	}
};
//////////////////////////////////////////////////////////////////////////
//客户端请求登录大厅;

struct LMsgC2SLogin:public LMsgSC
{
	Lint	m_seed;
	Lint	m_id;
	Lstring	m_md5;

	LMsgC2SLogin() :LMsgSC(MSG_C_2_S_LOGIN), m_seed(0), m_id(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_seed), m_seed);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_md5), m_md5);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_seed), m_seed);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SLogin();
	}
};

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家物品信息;
struct LMsgS2CItemInfo:public LMsgSC
{
	Lint	m_card1;
	Ldouble	m_card2;
	Lint	m_card3;
	Lint    m_gold;
	Lint	m_diamondNum;

	LMsgS2CItemInfo() :LMsgSC(MSG_S_2_C_ITEM_INFO), m_card1(0), m_card2(0), m_card3(0), m_gold(0), m_diamondNum(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_card1), m_card1);
		ReadMapData(obj, NAME_TO_STR(m_card2), m_card2);
		ReadMapData(obj, NAME_TO_STR(m_card3), m_card3);
		ReadMapData(obj, NAME_TO_STR(m_gold), m_gold);
		ReadMapData(obj, NAME_TO_STR(m_diamondNum), m_diamondNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card1), m_card1);
		WriteKeyValue(pack, NAME_TO_STR(m_card2), m_card2);
		WriteKeyValue(pack, NAME_TO_STR(m_card3), m_card3);
		WriteKeyValue(pack, NAME_TO_STR(m_gold), m_gold);
		WriteKeyValue(pack, NAME_TO_STR(m_diamondNum), m_diamondNum);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CItemInfo();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回跑马灯信息
struct LMsgS2CHorseInfo :public LMsgSC
{
	Lstring	m_str;

	LMsgS2CHorseInfo() :LMsgSC(MSG_S_2_C_HORSE_INFO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_str), m_str);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_str), m_str);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CHorseInfo(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家进入桌子
struct LMsgS2CIntoDesk:public LMsgSC
{
	Lint			m_deskId = 0;
	Lint            m_gameZone = 0;
	Lint			m_pos = 0;	//我的位置，0-3 分别代表东，南，西，北;
	Lint			m_ready = 0;//我是否准备，0-未准备，1-已经准备;
	Ldouble			m_score = 0.0f;//我的积分;
	Lint            m_gold = 0; //金币场金币值;
	Lint			m_gameStyle = 0;//0-只能自摸，1-可以收炮，2-可以抢杠;
	Lint			m_maxCircle = 0;//最大局数;
	Lint            m_curCircle = 0;//当前局数;
	Lint            m_nMaxFanshu = 0;//游戏最大番数;
	std::vector<Lint>			m_extValue;	//附加变量;
	Lint			m_nProportionate = 0; //抽成率
	Lint			m_nCircleGoldLimit = 0; //每局限制进入的金币数
	Lint	        m_nStake = 0; //翻倍数

	Lint			m_flag = 0;//0-只能自摸，1-可以收炮，2-可以抢杠
	std::vector<Lint>	m_playtype;

	Lint			m_rank = 0; //玩家比赛等级
	Lint			m_overScore = 0; //淘汰积分
	Lint            m_sportId = 0; //比赛id
	Lint            m_process = 0; //比赛进程 0 就是 淘汰赛阶段 1 晋级 第一阶段 2 晋级第二阶段 3 晋级最后阶段

	/////////////////////////广东///////////////////
	Lint		m_canInteraction = 0;	//是否可以交互
	Lint		m_antiCheat = 0; // 0 普通 1 防作弊
	Lint		m_curMatchId = 0;	//比赛ID
	Lint		m_playerNum = 0; // 玩家数目
	Lint		m_aaRoom = 0; // 0 普通 1 AA
	Lint		m_nFastPlay = 0;		//是否是专赛  //0:不是 1：快速组局 2：自由创建

	LMsgS2CIntoDesk():LMsgSC(MSG_S_2_C_INTO_DESK){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_ready), m_ready);
		ReadMapData(obj, NAME_TO_STR(m_score), m_score);
	    ReadMapData(obj, NAME_TO_STR(m_nMaxFanshu), m_nMaxFanshu);
		ReadMapVector(obj, NAME_TO_STR(m_extValue), m_extValue);
		ReadMapData(obj, NAME_TO_STR(m_nProportionate), m_nProportionate);
		ReadMapData(obj, NAME_TO_STR(m_nCircleGoldLimit), m_nCircleGoldLimit);
		ReadMapData(obj, NAME_TO_STR(m_nStake), m_nStake);

		ReadMapData(obj, NAME_TO_STR(m_state), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_playtype), m_playtype);
		ReadMapData(obj, NAME_TO_STR(m_rank), m_rank);
		ReadMapData(obj, NAME_TO_STR(m_overScore), m_overScore);
		ReadMapData(obj, NAME_TO_STR(m_sportId), m_sportId);
		ReadMapData(obj, NAME_TO_STR(m_process), m_process);

		ReadMapData(obj, NAME_TO_STR(m_canInteraction), m_canInteraction);
		ReadMapData(obj, NAME_TO_STR(m_antiCheat), m_antiCheat);
		ReadMapData(obj, NAME_TO_STR(m_curMatchId), m_curMatchId);
		ReadMapData(obj, NAME_TO_STR(m_playerNum), m_playerNum);
		ReadMapData(obj, NAME_TO_STR(m_aaRoom), m_aaRoom);
		ReadMapData(obj, NAME_TO_STR(m_nFastPlay), m_nFastPlay);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 27);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_gameZone), m_gameZone);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_ready), m_ready);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);
		WriteKeyValue(pack, NAME_TO_STR(m_gameStyle), m_gameStyle);
		WriteKeyValue(pack, NAME_TO_STR(m_maxCircle), m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_nMaxFanshu), m_nMaxFanshu);
		WriteKeyValue(pack, NAME_TO_STR(m_extValue), m_extValue);
		WriteKeyValue(pack, NAME_TO_STR(m_gold), m_gold);
		WriteKeyValue(pack, NAME_TO_STR(m_nProportionate), m_nProportionate);
		WriteKeyValue(pack, NAME_TO_STR(m_nCircleGoldLimit), m_nCircleGoldLimit);
		WriteKeyValue(pack, NAME_TO_STR(m_nStake), m_nStake);

		WriteKeyValue(pack, NAME_TO_STR(m_state), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_playtype), m_playtype);
		WriteKeyValue(pack, NAME_TO_STR(m_rank), m_rank);
		WriteKeyValue(pack, NAME_TO_STR(m_overScore), m_overScore);
		WriteKeyValue(pack, NAME_TO_STR(m_sportId), m_sportId);
		WriteKeyValue(pack, NAME_TO_STR(m_process), m_process);

		WriteKeyValue(pack, NAME_TO_STR(m_canInteraction), m_canInteraction);
		WriteKeyValue(pack, NAME_TO_STR(m_antiCheat), m_antiCheat);
		WriteKeyValue(pack, NAME_TO_STR(m_curMatchId), m_curMatchId);
		WriteKeyValue(pack, NAME_TO_STR(m_playerNum), m_playerNum);
		WriteKeyValue(pack, NAME_TO_STR(m_aaRoom), m_aaRoom);
		WriteKeyValue(pack, NAME_TO_STR(m_nFastPlay), m_nFastPlay);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CIntoDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回桌子添加一个玩家;
struct LMsgS2CDeskAddUser:public LMsgSC
{
	Lint			m_userId;
	Lstring			m_nike;//玩家昵称;
	Lstring			m_face;//玩家图形，为空就为默认图形;
	Lint			m_sex;//玩家性别;
	Lint			m_pos;	//位置，0-3 分别代表东，南，西，北;
	Lint			m_ready;//0-未准备，1-已准备;
	Ldouble			m_score;//玩家积分;
	Lint            m_gold;
	Lstring			m_ip;
	Lstring         m_location = "";
	Lstring         m_address = "";

	LMsgS2CDeskAddUser() :LMsgSC(MSG_S_2_C_ADD_USER), m_userId(0), m_sex(0), m_pos(0), m_ready(0), m_score(0), m_gold(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_nike), m_nike);
		ReadMapData(obj, NAME_TO_STR(m_face), m_face);
		ReadMapData(obj, NAME_TO_STR(m_sex), m_sex);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_ready), m_ready);
		ReadMapData(obj, NAME_TO_STR(m_score), m_score);
		ReadMapData(obj, NAME_TO_STR(m_gold), m_gold);
		ReadMapData(obj, NAME_TO_STR(m_ip), m_ip);
		ReadMapData(obj, NAME_TO_STR(m_location), m_location);
		ReadMapData(obj, NAME_TO_STR(m_address), m_address);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 12);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_nike), m_nike);
		WriteKeyValue(pack, NAME_TO_STR(m_face), m_face);
		WriteKeyValue(pack, NAME_TO_STR(m_sex), m_sex);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_ready), m_ready);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);
		WriteKeyValue(pack, NAME_TO_STR(m_gold), m_gold);
		WriteKeyValue(pack, NAME_TO_STR(m_ip), m_ip);
		WriteKeyValue(pack, NAME_TO_STR(m_location), m_location);
		WriteKeyValue(pack, NAME_TO_STR(m_address), m_address);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskAddUser();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回删除玩家
struct LMsgS2CDeskDelUser:public LMsgSC
{
	Lint			m_pos;//删除玩家位置 如果pos == 我的位置，代表我自己被踢出桌子，把桌子其他人清掉

	LMsgS2CDeskDelUser() :LMsgSC(MSG_S_2_C_DEL_USER), m_pos(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CDeskDelUser();}
};

//////////////////////////////////////////////////////////////////////////
//客户端请求推出房间;
struct LMsgC2SLeaveDesk:public LMsgSC
{
	Lint			m_pos;//我的位置
	Lint            m_force = 0;//是否强制

	LMsgC2SLeaveDesk() :LMsgSC(MSG_C_2_S_LEAVE_ROOM), m_pos(-1){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_force), m_force);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_force), m_force);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SLeaveDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回结果
struct LMsgS2CLeaveDesk:public LMsgSC
{
	Lint			m_errorCode;//0-成功，1-失败

	LMsgS2CLeaveDesk() :LMsgSC(MSG_S_2_C_LEAVE_ROOM), m_errorCode(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CLeaveDesk();}
};

//////////////////////////////////////////////////////////////////////////
//客户端申请解算房间
struct LMsgC2SResetDesk:public LMsgSC
{
	Lint	m_pos;//位置
	LMsgC2SResetDesk():LMsgSC(MSG_C_2_S_RESET_ROOM), m_pos(-1){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
struct LMsgS2CResetDesk:public LMsgSC
{
	Lint					m_errorCode;//0-等待操作中，1-未开始直接解算，2-三个人同意，解算成功，3-时间到，解算成功，4-有一个人拒绝，解算失败; 5-有人解散房间
	Lint					m_time;//倒计时;
	Lstring					m_applay;//申请的玩家;
	Lstring					m_refluse;//拒绝的玩家,有一个玩家拒绝，则解算房间失败;
	Lint					m_flag;//0-等待我同意或者拒绝，1-我已经同意或者拒绝  ;                                                                                                                             
	std::vector<Lstring>	m_agree;//同意的玩家;
	std::vector<Lstring>	m_wait;//等待操作的玩家;
	

	LMsgS2CResetDesk() :LMsgSC(MSG_S_2_C_RESET_ROOM), m_errorCode(0), m_time(0), m_flag(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_apply), m_applay);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		
		WriteKey(pack,"m_agree");
		WriteKey(pack, m_agree);

		WriteKey(pack,"m_refuse");
		WriteKey(pack,m_refluse);

		WriteKey(pack, "m_wait");
		WriteKey(pack, m_wait);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
//客户端同意或者拒绝操作
struct LMsgC2SSelectResetDesk:public LMsgSC
{
	Lint	m_pos;//位置
	Lint	m_flag;//1-同意，2-拒绝
	LMsgC2SSelectResetDesk():LMsgSC(MSG_C_2_S_RESET_ROOM_SELECT), m_pos(0), m_flag(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SSelectResetDesk();}
};

//////////////////////////////////////////////////////////////////////////
//服务器发送桌子状态，这个消息主要用于重连用;
struct LMsgS2CDeskState:public LMsgSC
{
	Lint				m_state = 0;//当前桌子状态 1-等待，2-游戏中，会有后面的数据;
	std::vector<Lint>	m_card;//我手上的牌;
	std::vector<Lint>   m_out[4];
	std::vector<Lint>   m_laiziCard;
	std::vector<Lint>   m_laiziChange[4];
	Lint                m_cardNum[4];//玩家剩余牌数;
	std::vector<Lint>   m_dipai;     //断线重连地主抢到的底牌信息;
	Lint                m_diZhuPos = 0;
	Lint                m_gameStyle = 0;
	Lint                m_CurBomb[4];
	Ldouble             m_nUserBeishu[4];
	Lint				m_pokerStyle = 0;	//当前牌型;
	Lint				m_pokerNumber = 0;	//当前牌型最大值;
	std::vector<Lint>   m_laiziNumber;	//
	Lint				m_difen = 0;	//底分;
	std::vector<bool>   m_IsTuoguan; //1表示託管中，0表示未託管;
	std::vector<Lint>   m_Int_1;
	std::vector<Lint>	m_daobudao;	//
	std::vector<Lint>	m_labula;

	///////////////////////////长沙使用////////////
	Lint		m_time = 0;//倒计时
	Lint		m_flag = 0;//0-等待别人，1-该我出牌，2-该我思考
	Lint		m_zhuang = 0;//庄的位置
	Lint		m_dCount = 0;//桌面剩余的牌
	Lint		m_pos = 0;//我的位置
	Lint		m_cardCount[4];//0-3号玩家手上的牌数量
	Lint        m_score[4];
	Lint		m_user_count = 0;
	Lint		m_myPos = 0;
	//我手上的牌
	CardValue	m_cardValue[14];

	//碰的牌
	Lint		m_pCount[4];
	CardValue	m_pCard[4][20];

	//暗杠的牌
	Lint		m_aCount[4];
	CardValue	m_aCard[4][20];

	//明杠的牌
	Lint		m_mCount[4];
	CardValue	m_mCard[4][20];

	//出出去的牌
	Lint		m_oCount[4];
	CardValue	m_oCard[4][25];

	//吃的牌
	Lint		m_eCount[4];
	CardValue	m_eCard[4][20];

	CardValue   m_WangbaCard;

	bool m_ShangWang;
	bool m_XiaWang;

	bool m_isBaoTing;

	//听的数据
	Lint m_tingType; //0-明听，1-暗听
	Lint m_tingPos[4]; //如果是-1标示 没有 听
	CardValue m_tingCard[4]; //听的信息
	//////////////////////////长沙使用////////////

	//////////////////////////广东使用///////////
	Lint		m_flowerCount[DEFAULT_DESK_USER_COUNT];
	CardValue	m_flowerCard[DEFAULT_DESK_USER_COUNT][8];
	Lint		m_successive[DEFAULT_DESK_USER_COUNT];
	Lint		m_redDragonNum[DEFAULT_DESK_USER_COUNT];
	CardValue	m_ghostCard;

	/////////////////////////////////////////////

	LMsgS2CDeskState() :LMsgSC(MSG_S_2_C_DESK_STATE), m_ShangWang(false), m_XiaWang(false), m_isBaoTing(false) {
		memset(m_score, 0, sizeof(m_score));
		memset(m_cardCount, 0, sizeof(m_cardCount));
		memset(m_pCount, 0, sizeof(m_pCount));
		memset(m_aCount, 0, sizeof(m_aCount));
		memset(m_mCount, 0, sizeof(m_mCount));
		memset(m_oCount, 0, sizeof(m_oCount));
		memset(m_eCount, 0, sizeof(m_eCount));
		memset(&m_WangbaCard, 0, sizeof(m_WangbaCard));
		memset(m_flowerCount, 0, sizeof(m_flowerCount));
		memset(m_flowerCard, 0, sizeof(m_flowerCard));
		memset(m_successive, 0, sizeof(m_successive));
		memset(m_redDragonNum, 0, sizeof(m_redDragonNum));
		m_user_count = 0;
		m_tingType = 0;
		memset(m_tingPos, -1, sizeof(m_tingPos));
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_zhuang), m_zhuang);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_dCount);
		//ReadMapData(obj, NAME_TO_STR(m_myPos), m_myPos);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_myCard), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			m_cardCount[m_pos] = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}
		m_ghostCard.Read(obj);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 26 + 12 + 5 * m_user_count + 11);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);

		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		WriteKeyValue(pack, NAME_TO_STR(m_out0), m_out[0]);
		WriteKeyValue(pack, NAME_TO_STR(m_out1), m_out[1]);
		WriteKeyValue(pack, NAME_TO_STR(m_out2), m_out[2]);
		WriteKeyValue(pack, NAME_TO_STR(m_out3), m_out[3]);
		WriteKeyValue(pack, NAME_TO_STR(m_laiziCard), m_laiziCard);
		WriteKeyValue(pack, NAME_TO_STR(m_laiziChange0), m_laiziChange[0]);
		WriteKeyValue(pack, NAME_TO_STR(m_laiziChange1), m_laiziChange[1]);
		WriteKeyValue(pack, NAME_TO_STR(m_laiziChange2), m_laiziChange[2]);
		WriteKeyValue(pack, NAME_TO_STR(m_laiziChange3), m_laiziChange[3]);
		WriteKey(pack, "m_cardNum");
		std::vector<int> veci;
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_cardNum[i]);
		}
		WriteKey(pack,veci);
		WriteKeyValue(pack, NAME_TO_STR(m_dipai), m_dipai);
		WriteKeyValue(pack, NAME_TO_STR(m_diZhuPos), m_diZhuPos);
		WriteKeyValue(pack, NAME_TO_STR(m_gameStyle), m_gameStyle);
		WriteKey(pack, "m_CurBomb");
		veci.clear();
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_CurBomb[i]);
		}
		WriteKey(pack,veci);
		WriteKey(pack, "m_nUserBeishu");
		veci.clear();
		std::vector<Ldouble> vecf;
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecf.push_back(m_nUserBeishu[i]);
		}
		WriteKey(pack, vecf);

		WriteKeyValue(pack, NAME_TO_STR(m_pokerStyle),m_pokerStyle);
		WriteKeyValue(pack, NAME_TO_STR(m_laiziNumber),m_laiziNumber);
		WriteKeyValue(pack, NAME_TO_STR(m_IsTuoguan), m_IsTuoguan);
		WriteKeyValue(pack, NAME_TO_STR(m_difen), m_difen);
		WriteKeyValue(pack, NAME_TO_STR(m_Int_1), m_Int_1);
		WriteKeyValue(pack, NAME_TO_STR(m_daobudao), m_daobudao);
		WriteKeyValue(pack, NAME_TO_STR(m_labula), m_labula);
		WriteKeyValue(pack, NAME_TO_STR(m_pokerNumber), m_pokerNumber);


		///////////////////////长沙使用/////////////////
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		WriteKeyValue(pack, NAME_TO_STR(m_dCount), m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_ShangWang), m_ShangWang);
		WriteKeyValue(pack, NAME_TO_STR(m_XiaWang), m_XiaWang);
		WriteKeyValue(pack, NAME_TO_STR(m_isBaoTing), m_isBaoTing);
		m_WangbaCard.Write(pack);
		std::vector<CardValue> vec_cv;
		WriteKey(pack, "m_CardCount");
		veci.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_cardCount[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_myCard");
		vec_cv.clear();
		for (Lint i = 0; i < m_cardCount[m_myPos]; ++i)
		{
			vec_cv.push_back(m_cardValue[i]);
		}
		WriteKey(pack, vec_cv);

		WriteKey(pack, "m_score");
		veci.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_score[i]);
		}
		WriteKey(pack, veci);

		for (Lint i = 0; i < m_user_count; ++i)
		{
			char name[32];
			sprintf(name, "m_pCard%d", i);
			WriteKey(pack, name);
			vec_cv.clear();
			for (Lint j = 0; j < m_pCount[i]; j += 3)
			{
				vec_cv.push_back(m_pCard[i][j]);
			}
			WriteKey(pack, vec_cv);
		}

		for (Lint i = 0; i < m_user_count; ++i)
		{
			char name[32];
			sprintf(name, "m_aCard%d", i);
			WriteKey(pack, name);
			vec_cv.clear();
			for (Lint j = 0; j < m_aCount[i]; j += 4)
			{
				vec_cv.push_back(m_aCard[i][j]);
			}
			WriteKey(pack, vec_cv);
		}

		for (Lint i = 0; i < m_user_count; ++i)
		{
			char name[32];
			sprintf(name, "m_mCard%d", i);
			WriteKey(pack, name);
			vec_cv.clear();
			for (Lint j = 0; j < m_mCount[i]; j += 4)
			{
				vec_cv.push_back(m_mCard[i][j]);
			}
			WriteKey(pack, vec_cv);
		}

		for (Lint i = 0; i < m_user_count; ++i)
		{
			char name[32];
			sprintf(name, "m_oCard%d", i);
			WriteKey(pack, std::string(name));
			vec_cv.clear();
			for (Lint j = 0; j < m_oCount[i]; ++j)
			{
				vec_cv.push_back(m_oCard[i][j]);
			}
			WriteKey(pack, vec_cv);
		}
		for (Lint i = 0; i < m_user_count; ++i)
		{
			char name[32];
			sprintf(name, "m_eCard%d", i);
			WriteKey(pack, std::string(name));
			vec_cv.clear();
			for (Lint j = 0; j < m_eCount[i]; ++j)
			{
				vec_cv.push_back(m_eCard[i][j]);
			}
			WriteKey(pack, vec_cv);
		}

		///////////////////////长沙使用/////////////////
		//////////////////////广东使用/////////////////
		std::vector<CardValue> vec;
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			char name[32];
			sprintf_s(name, sizeof(name), "m_flowerCard%d", i);
			WriteKey(pack, std::string(name));
			vec.clear();
			for (Lint j = 0; j < m_flowerCount[i]; ++j)
			{
				vec.push_back(m_flowerCard[i][j]);
			}
			WriteKey(pack, vec);
		}

		WriteKey(pack, "m_successive");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_successive[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_redDragonNum");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_redDragonNum[i]);
		}
		WriteKey(pack, veci);
		m_ghostCard.Write(pack);
		///////////////////////////////////////////////

		//////////////////////////////长沙麻将 补充//////////////
		WriteKeyValue(pack, NAME_TO_STR(m_tingType), m_tingType);

		veci.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_tingPos[i]);
		}
		WriteKeyValue(pack, NAME_TO_STR(m_tingPos), veci);

		vec.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			vec.push_back(m_tingCard[i]);
		}
		WriteKeyValue(pack, NAME_TO_STR(m_tingCard), vec);
		//////////////////////////////长沙麻将 补充//////////////

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CDeskState(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求准备;
struct LMsgC2SUserReady:public LMsgSC
{
	Lint m_pos;

	LMsgC2SUserReady() :LMsgSC(MSG_C_2_S_READY), m_pos(-1){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserReady();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回玩家准备
struct LMsgS2CUserReady:public LMsgSC
{
	Lint			m_pos;//准备玩家的位置;

	LMsgS2CUserReady() :LMsgSC(MSG_S_2_C_READY), m_pos(-1){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserReady();}
};


//////////////////////////////////////////////////////////////////////////
//服务器返回玩家准备
struct LMsgS2CUserOnlineInfo:public LMsgSC
{
	Lint			m_flag;//0-掉线了，1-回来了
	Lint			m_pos;//
	Lstring			m_ip;

	LMsgS2CUserOnlineInfo() :LMsgSC(MSG_S_2_C_USER_ONLINE_INFO), m_flag(0), m_pos(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_ip), m_ip);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserOnlineInfo(); }
};
//////////////////////////////////////////////////////////////////////////
//服务器广播游戏开始;
struct LMsgS2CPlayStart:public LMsgSC
{
	std::vector<Lint>	m_card;
	Lint				m_hpos = 0;//黑桃三的位置玩家;(四川痞子斗地主地主位置）
	Lint                m_gameStyle = 0;
	std::vector<Lint>   m_firstPos_card;             

	////////长沙使用////////////
	Lint		m_zhuang = 0;
	Lint		m_dCount = 0;//桌面剩余的牌	
	Lint		m_cardCount[4];//0-3号玩家手上的牌数量
	Lint		m_pos = 0;
	std::vector<Lint> m_score;
	//我手上的牌
	CardValue	m_cardValue[14];
	////////长沙使用////////////

	LMsgS2CPlayStart() :LMsgSC(MSG_S_2_C_START), m_hpos(0), m_gameStyle(0){
		memset(m_cardCount, 0, sizeof(m_cardCount));
	}

	virtual bool Read(msgpack::object& obj)
	{
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_myCard), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			//m_cardCount[0] = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}

		msgpack::object array1;
		ReadMapData(obj, NAME_TO_STR(m_CardCount), array1);
		if (array1.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array1.via.array.size; ++i)
			{
				msgpack::object& pv = *(array1.via.array.ptr + i);
				pv.convert(m_cardCount[i]);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_zhuang), m_zhuang);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_dCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 10);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		WriteKeyValue(pack, NAME_TO_STR(m_hpos), m_hpos);
		WriteKeyValue(pack, NAME_TO_STR(m_gameStyle), m_gameStyle);
		WriteKeyValue(pack, NAME_TO_STR(m_firstPos_card), m_firstPos_card);

		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);

		WriteKey(pack, "m_myCard");
		std::vector<CardValue>vce;
		for (Lint i = 0; i < m_cardCount[m_pos]; ++i)
		{
			vce.push_back(m_cardValue[i]);
		}
		WriteKey(pack, vce);

		WriteKey(pack, "m_CardCount");
		std::vector<Lint>vcei;
		for (Lint i = 0; i < 4; ++i)
		{
			vcei.push_back(m_cardCount[i]);
		}
		WriteKey(pack, vcei);

		WriteKeyValue(pack, NAME_TO_STR(m_dCount), m_dCount);
		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CPlayStart();}
};

//////////////////////////////////////////////////////////////////////////
//服务器广播玩家出牌

struct ThinkData
{
	ThinkData():m_type(0){}
	Lint	m_type;
	std::vector<CardValue> m_card;

	MSGPACK_DEFINE(m_type, m_card); 
};


//通知玩家进行抢地主操作;
struct LMsgS2CAskDiZhu :public LMsgSC
{
	Lint		m_pos;//通知玩家位置;
	Lint		m_difen;//抢地主底分;
	Lint		m_state;	//0； 1叫地主，2抢地主

	LMsgS2CAskDiZhu() :LMsgSC(MSG_S_2_C_ASK_DIZHU), m_pos(0), m_difen(0), m_state(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_difen), m_difen);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgS2CAskDiZhu(); }
};

//将玩家是否抢地主的结果通知所有玩家;
struct LMsgS2CAnsDiZhu :public LMsgSC
{
	Lint		m_pos;//玩家位置;
	Lint        m_yaobu;//玩家是否选择地主;1为抢地主,0为过;
	Lint		m_difen;//抢地主底分;
	Lint        m_restart;//1，所有玩家不抢，需要重新发牌;其他不用解析;
	Lint		m_state;	//当前抢地主

	LMsgS2CAnsDiZhu() :LMsgSC(MSG_S_2_C_ANS_DIZHU), m_pos(0), m_yaobu(0), m_difen(0), m_restart(0), m_state(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_yaobu), m_yaobu);
		WriteKeyValue(pack, NAME_TO_STR(m_difen), m_difen);
		WriteKeyValue(pack, NAME_TO_STR(m_restart), m_restart);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);

		return true;
	}
	virtual LMsg* Clone(){ return new LMsgS2CAnsDiZhu(); }
};

//将最终抢地主的结果通知所有玩家;
struct LMsgS2CWhoIsDiZhu :public LMsgSC
{
	Lint		m_pos;//玩家位置;
	Lint		m_difen;//抢地主底分;
	std::vector<Lint>  m_LeftCard;

	LMsgS2CWhoIsDiZhu() :LMsgSC(MSG_S_2_C_WHO_IS_DIZHU), m_pos(0), m_difen(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_difen), m_difen);
		WriteKeyValue(pack, NAME_TO_STR(m_LeftCard), m_LeftCard);
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgS2CWhoIsDiZhu(); }
};

//通知玩家自己的手牌;
struct LMsgS2CShowCards :public LMsgSC
{
	Lint		m_pos;//玩家位置;
	std::vector<Lint>  m_MyCard;

	LMsgS2CShowCards() :LMsgSC(MSG_S_2_C_SHOWCARDS), m_pos(-1){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_MyCard), m_MyCard);
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgS2CShowCards(); }
};

//客户端返回玩家抢地主操作;
struct LMsgC2SQiangDiZhu :public LMsgSC
{
	Lint		m_pos;//抢地主玩家位置;
	Lint        m_yaobu;//玩家选择结果;1为抢地主,0为过;
	Lint		m_difen;//抢地主底分;

	LMsgC2SQiangDiZhu() :LMsgSC(MSG_C_2_S_QIANG_DIZHU), m_pos(0), m_yaobu(0), m_difen(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_yaobu), m_yaobu);
		ReadMapData(obj, NAME_TO_STR(m_difen), m_difen);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgC2SQiangDiZhu(); }
};

struct LMsgS2CSameIP :public LMsgSC
{
	Lint		m_pos[2] = {0};//
	Lstring		m_nike[2];
	
	LMsgS2CSameIP() :LMsgSC(MSG_S_2_C_SAMEIP){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack, "m_pos");
		std::vector<Lint> veci;
		for (Lint i = 0; i < 2; ++i)
		{
			veci.push_back(m_pos[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_nike");
		std::vector<Lstring> vecs;
		for (Lint i = 0; i < 2; ++i)
		{
			vecs.push_back(m_nike[i]);
		}
		WriteKey(pack, vecs);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CSameIP(); }
};

//四川斗地主玩法，服务器通知客户端进行相关抢地主操作;
struct LMsgS2CQDZQuery_Sichuan :public LMsgSC
{
	Lint		m_pos;//通知玩家位置;
	Lint        m_operType;//1询问闷抓或者看牌；2询问抓或者不抓；3询问到或者不倒；4询问拉或者不拉;5: 随机点数,花色, 6.地主掷骰子;
	Lint		m_dianshu;	//随机点数;

	LMsgS2CQDZQuery_Sichuan() :LMsgSC(MSG_S_2_C_QDZQUERY_NEW), m_pos(0), m_operType(0), m_dianshu(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_operType), m_operType);
		WriteKeyValue(pack, NAME_TO_STR(m_dianshu), m_dianshu);

		return true;
	}
	virtual LMsg* Clone(){ return new LMsgS2CQDZQuery_Sichuan(); }
};

////四川斗地主玩法，客户端返回玩家相关抢地主操作;
struct LMsgC2SQdzOper_Sichuan :public LMsgSC
{
	Lint		m_pos;//抢地主玩家位置;
	Lint        m_operType;////1询问闷抓或者看牌；2询问抓或者不抓；3询问到或者不倒；4询问拉或者不拉 5.随机点 6.花色;
	Lint		m_playerOper;//1代表闷抓，抓，到，拉;0代表看牌，不抓，不倒，或者不拉;

	LMsgC2SQdzOper_Sichuan() :LMsgSC(MSG_C_2_S_QDZOPER_NEW), m_pos(0), m_operType(0), m_playerOper(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_operType), m_operType);
		ReadMapData(obj, NAME_TO_STR(m_playerOper), m_playerOper);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgC2SQdzOper_Sichuan(); }
};

////四川斗地主玩法，服务器广播玩家相关抢地主操作结果;
struct LMsgS2CQdzOper_Sichuan :public LMsgSC
{
	Lint		m_pos;//操作玩家位置;
	Lint        m_operType;////1询问闷抓或者看牌；2询问抓或者不抓；3询问到或者不倒；4询问拉或者不拉;
	Lint		m_playerOper;//玩家动作,1代表闷抓，抓，到，拉;0代表看牌，不抓，不倒，或者不拉;
	Ldouble      m_nUserBeishu[4] = { 0 };//操作之后各个玩家倍数;
	Lint        m_blank;//预留位置,二人斗地主作为是否重新发牌使用;

	LMsgS2CQdzOper_Sichuan() :LMsgSC(MSG_S_2_C_QDZOPER_NEW),m_pos(0), m_operType(0), m_playerOper(0), m_blank(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_operType), m_operType);
		ReadMapData(obj, NAME_TO_STR(m_playerOper), m_playerOper);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{		
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_operType), m_operType);
		WriteKeyValue(pack, NAME_TO_STR(m_playerOper), m_playerOper);

		WriteKey(pack, "m_nUserBeishu");
		std::vector<Ldouble> vecf;
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecf.push_back(m_nUserBeishu[i]);
		}
		WriteKey(pack, vecf);
		WriteKeyValue(pack, NAME_TO_STR(m_blank), m_blank);
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgS2CQdzOper_Sichuan(); }
};

struct LMsgS2COutCard:public LMsgSC
{
	Lint		m_pos;//当前出牌中的玩家;
	Lint		m_time;//出牌倒计时;
	Lint		m_flag;//当前是否第一个出牌，0-是(没有上家)，1-不是;
	Lint        m_last;//当前是否最后一手牌，0-不是，1-是;
	std::vector<std::vector<Lint>> m_card;//提示的牌型;
	std::vector<Lint>              m_cardUnusable; // 提示无法打出的牌;

	///////长沙麻将添加//////////////
	Lint		m_deskCard = 0;//桌面剩余的牌
	Lint		m_gang = 0;//是否杠过，0-没有，1-杠过
	Lint		m_end = 0;//是否海底 0-不是，1-是
	CardValue	m_curCard;//摸上来的牌，这个只有 m_pos == mypos m_flag == 0 的时候才有

	std::vector<ThinkData> m_think;//思考的内容
	///////长沙麻将添加//////////////

	LMsgS2COutCard() :LMsgSC(MSG_S_2_C_GET_CARD), m_pos(0), m_time(0), m_flag(0), m_last(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_dCount), m_deskCard);
		m_curCard.Read(obj);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_think), m_think);
		ReadMapData(obj, NAME_TO_STR(m_gang), m_gang);
		ReadMapData(obj, NAME_TO_STR(m_end), m_end);
		//
		msgpack::object array1;
		ReadMapData(obj, NAME_TO_STR(m_think), array1);
		if (array1.type == MSGPACK_OBJECT_ARRAY)
		{
			for (Lsize i = 0; i < array1.via.array.size; ++i)
			{
				ThinkData v;
				msgpack::object& pv = *(array1.via.array.ptr + i);
				pv.convert(v);
				m_think.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7 + 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);

		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_last), m_last);
		WriteKeyValue(pack, NAME_TO_STR(m_array), m_card);
		WriteKeyValue(pack, NAME_TO_STR(m_cardUnusable), m_cardUnusable);

		///////////////////////////////////
		WriteKeyValue(pack, NAME_TO_STR(m_dCount), m_deskCard);
		m_curCard.Write(pack);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		WriteKeyValue(pack, NAME_TO_STR(m_gang), m_gang);
		WriteKeyValue(pack, NAME_TO_STR(m_end), m_end);
		//////////////////////////////////////

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2COutCard(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求出牌;
struct LMsgC2SUserPlay:public LMsgSC
{
	Lint			  m_flag;//玩家出牌类型 0-出，1-不出牌;
	std::vector<Lint> m_card;//玩家出的牌型;
	Lint			  m_style;	//玩家（首牌类型（带癞子时用）;

	////////////长沙使用/////////////
	ThinkData m_thinkInfo;
	////////////长沙使用/////////////

	LMsgC2SUserPlay() :LMsgSC(MSG_C_2_S_PLAY_CARD), m_flag(0), m_style(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		
		ReadMapVector(obj, NAME_TO_STR(m_card), m_card);		

		ReadMapData(obj, NAME_TO_STR(m_style), m_style);

		ReadMapData(obj, NAME_TO_STR(m_type), m_thinkInfo.m_type);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_think), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_thinkInfo.m_card.push_back(v);
			}
		}
		
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_thinkInfo.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_thinkInfo.m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserPlay();}
};

//////////////////////////////////////////////////////////////////////////
//服务器返回操作结果
struct LMsgS2CUserPlay:public LMsgSC
{
	Lint		m_errorCode; //错误类型描述;
	Lint		m_flag;//玩家出牌类型 0-出，1-不出牌, 2-玩家第一手出牌（开局或者上把出牌无人要）;
	Lint		m_type;//出牌类型;
	Lint		m_typeNumber;	//类型值
	Lint		m_pos;//出牌玩家的位置;
	std::vector<Lint>	m_laiziNumber;	//皮子替换的牌（临时为客户端添加，客户端后面要实现的逻辑）;
	std::vector<Lint>	m_card;

	/////////长沙使用//////////
	ThinkData	m_cs_card;
	std::vector<Lint>	m_hu;
	Lint		m_cardCount = 0;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	CardValue m_huCard;					//自摸胡的那张牌
	/////////长沙使用//////////

	LMsgS2CUserPlay() :LMsgSC(MSG_S_2_C_PLAY_CARD), m_errorCode(0), m_flag(0), m_type(0), m_typeNumber(0), m_pos(0){
		m_cs_card.m_card.clear();
		m_hu.clear();
		m_cardCount = 0;
		m_cardValue.clear();
	}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_cs_type), m_cs_card.m_type);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_think), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_cs_card.m_card.push_back(v);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_hu), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_hu.push_back(v);
			}
		}

		ReadMapData(obj, NAME_TO_STR(m_cardCount), m_cardCount);
		ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_cardValue.push_back(v);
			}
		}

		m_huCard.Read(obj);

		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,15);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		WriteKeyValue(pack, NAME_TO_STR(m_laiziNumber), m_laiziNumber);
		WriteKeyValue(pack, NAME_TO_STR(m_typeNumber), m_typeNumber);

		WriteKeyValue(pack, NAME_TO_STR(m_cs_type), m_cs_card.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_cs_card.m_card);
		WriteKeyValue(pack, NAME_TO_STR(m_hu), m_hu);
		WriteKeyValue(pack, NAME_TO_STR(m_cardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(m_cardValue), m_cardValue);
		m_huCard.Write(pack);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserPlay();}
};

//////////////////////////////////////////////////////////////////////////
//服务器发送玩家思考信息

struct LMsgS2CThink:public LMsgSC
{
	Lint				m_time;//思考倒计时
	Lint				m_flag;//0-等待其他玩家思考中，1-当前我正在思考
	CardValue			m_card;
	std::vector<ThinkData>	m_think;


	LMsgS2CThink() :LMsgSC(MSG_S_2_C_USER_THINK), m_time(0), m_flag(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CThink();}
};

//////////////////////////////////////////////////////////////////////////
//客户端发送思考操作结果
struct LMsgC2SUserOper:public LMsgSC
{
	ThinkData m_think; 
	//Lint m_think;//思考结果0-过，1-胡，2-杠，3-碰,4-补

	LMsgC2SUserOper() :LMsgSC(MSG_C_2_S_USER_OPER){}
	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_think.m_type);

		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_think), array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_think.m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}


	virtual LMsg* Clone(){return new LMsgC2SUserOper();}
};

//////////////////////////////////////////////////////////////////////////
//服务广播操作结果
struct LMsgS2CUserOper:public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-失败;
	Lint		m_pos;//pos=我自己的时候，需要根据类型响应不同的操作;
	ThinkData	m_think; 
	CardValue	m_card; //玩家碰，杠，胡的张牌;
	std::vector<Lint>	m_hu;


	CardValue   m_hucard2;//胡两张时第二张牌
	Lint		m_cardCount;			//手牌数量
	std::vector<CardValue> m_cardValue;	//推倒的手牌
	
	LMsgS2CUserOper() :LMsgSC(MSG_S_2_C_USER_OPER), m_errorCode(0), m_pos(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_think), m_think);
		m_card.Read(obj);

		ReadMapData(obj, NAME_TO_STR(m_hu), m_hu);
		ReadMapData(obj, NAME_TO_STR(m_cardCount), m_cardCount);
		ReadMapData(obj, NAME_TO_STR(m_cardValue), m_cardValue);
		ReadMapData(obj, NAME_TO_STR(m_hucard2_number), m_hucard2.m_number);
		ReadMapData(obj, NAME_TO_STR(m_hucard2_color), m_hucard2.m_color);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 11);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_think), m_think);
		m_card.Write(pack);
		WriteKeyValue(pack, NAME_TO_STR(m_hu), m_hu);

		WriteKeyValue(pack, NAME_TO_STR(m_cardCount), m_cardCount);
		WriteKeyValue(pack, NAME_TO_STR(m_cardValue), m_cardValue)
		WriteKeyValue(pack, NAME_TO_STR(m_hucard2_number), m_hucard2.m_number);
		WriteKeyValue(pack, NAME_TO_STR(m_hucard2_color), m_hucard2.m_color);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserOper();}
};
//////////////////////////////////////////////////////////////////////////
//服务器广播玩家手牌;
struct LMsgS2CCard :public LMsgSC
{
	Lint m_pos;
	std::vector<Lint>   m_cards[4]; //三家剩余牌数;

	LMsgS2CCard() :LMsgSC(MSG_S_2_C_CARD_LEFT), m_pos(-1){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_cards0), m_cards[0]);
		WriteKeyValue(pack, NAME_TO_STR(m_cards1), m_cards[1]);
		WriteKeyValue(pack, NAME_TO_STR(m_cards2), m_cards[2]);
		WriteKeyValue(pack, NAME_TO_STR(m_cards3), m_cards[3]);
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgS2CCard(); }
};

//////////////////////////////////////////////////////////////////////////

struct BirdInfo
{
	Lint	m_pos;
	CardValue m_card;

	MSGPACK_DEFINE(m_pos, m_card);

	BirdInfo() :m_pos(-1) {}
	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		m_card.Write(pack);
		return true;
	}
};

//服务器广播游戏结束;
struct LMsgS2CGameOver:public LMsgSC
{
	Lint		m_win;//
	Ldouble		m_score[4] = { 0 }; //三家积分情况;
	Lint		m_bomb[4] = { 0 }; //三家炸弹数量;
	Lint		m_chuntian[4] = { 0 }; //三家春天情况;
	Lint		m_count[4] = { 0 }; //三家剩余排数;
	Lstring		m_nike[4];
	Ldouble      m_gold[4] = { 0 }; //三家获得的金币(抽成后)

	////////长沙 棋牌///////////////
	Lint		m_result = 0;//0-自摸，1-点炮，2-慌庄
	Lint		m_cs_win[4];//4家胡牌情况，0-没胡，1-自摸，2-收炮，3-点炮 7-抢杠胡 8-杠上炮 9-杠上花 10-杠上开花点杠包赔
	//Lint		m_score[4];//东 南 西 北 各家输赢积分，这里需要更新玩家积分
	Lint		m_agang[4];//暗杠数量
	Lint		m_mgang[4];//明杠数量
	Lint		m_dmgang[4];//被点杠数量
	Lint		m_dgang[4];//点杠数量
	std::vector<Lint>		m_startHu[4];//起手胡牌
	std::vector<Lint>		m_hu[4];//胡牌的数组

									//四家手上的牌
	//Lint		m_count[4];
	CardValue	m_card[4][14];

	std::vector<CardValue> m_hucards[4];	//胡的牌
	Lint		m_end = 0;
	std::vector<BirdInfo>	m_bird_infos;	//鸟牌

	Lint		m_user_count = 0;

	CardValue   m_WangbaCard;
	bool        m_isBaoTing[4];
	////////长沙 棋牌///////////////

	////////广东 棋牌//////////////
	Lint m_nHorseHitAll;		//一马全中
	std::vector<CardValue> m_ghostCard;		//鬼牌
	Lint m_gengZhuangNum;
	Lint m_playerNum;				//玩家数目
	Lint m_successive[DEFAULT_DESK_USER_COUNT];					//连庄
	Lint m_redDragonNum[DEFAULT_DESK_USER_COUNT];	//红中马数量
	std::vector<CardValue> m_gd_hucards;	//胡的牌
	////////广东 棋牌end///////////

	LMsgS2CGameOver() :LMsgSC(MSG_S_2_C_GAME_OVER), m_win(0), m_nHorseHitAll(0), m_playerNum(0){
		memset(m_cs_win, 0, sizeof(m_cs_win));
		memset(m_score, 0, sizeof(m_score));
		memset(m_agang, 0, sizeof(m_agang));
		memset(m_mgang, 0, sizeof(m_mgang));
		memset(m_dmgang, 0, sizeof(m_dmgang));
		memset(m_dgang, 0, sizeof(m_dgang));
		memset(m_count, 0, sizeof(m_count));
		memset(&m_WangbaCard, 0, sizeof(m_WangbaCard));
		m_user_count = 0;
		for (int i = 0; i < 4; ++i)
		{
			m_isBaoTing[i] = false;
		}
		memset(m_successive, 0, sizeof(m_successive));
		memset(m_redDragonNum, 0, sizeof(m_redDragonNum));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8 + 26 + m_user_count + 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_win), m_win);
		
		WriteKey(pack, "m_nike");
		std::vector<Lstring> vecs;
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecs.push_back(m_nike[i]);
		}
		WriteKey(pack,vecs);

		WriteKey(pack, "m_score");
		std::vector<Ldouble> vecf;
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecf.push_back(m_score[i]);
		}
		WriteKey(pack, vecf);

		std::vector<Lint> veci;
		WriteKey(pack, "m_bomb");
		veci.clear();
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_bomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_chuntian");
		veci.clear();
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_chuntian[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_count");
		veci.clear();
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_count[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_gold");
		vecf.clear();
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecf.push_back(m_gold[i]);
		}
		WriteKey(pack, vecf);


		//////////////////长沙棋牌////////////

		WriteKey(pack, "m_cs_win");
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_cs_win[i]);
		}
		WriteKey(pack, veci);

		WriteKeyValue(pack, NAME_TO_STR(m_result), m_result);
		WriteKeyValue(pack, NAME_TO_STR(m_end), m_end);
		WriteKeyValue(pack, "m_isBaoTing0", m_isBaoTing[0]);
		WriteKeyValue(pack, "m_isBaoTing1", m_isBaoTing[1]);
		WriteKeyValue(pack, "m_isBaoTing2", m_isBaoTing[2]);

		WriteKeyValue(pack, "m_isBaoTing3", m_isBaoTing[3]);
		m_WangbaCard.Write(pack);

		for (Lint i = 0; i < m_user_count; ++i)
		{
			char name[32];
			sprintf_s(name, sizeof(name), "m_hu%d", i + 1);
			WriteKey(pack, name);
			WriteKey(pack, m_hu[i]);
		}

		WriteKey(pack, "m_agang");
		veci.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_agang[i]);
		}
		WriteKey(pack, veci);

		WriteKeyValue(pack, NAME_TO_STR(m_startHu0), m_startHu[0]);
		WriteKeyValue(pack, NAME_TO_STR(m_startHu1), m_startHu[1]);
		WriteKeyValue(pack, NAME_TO_STR(m_startHu2), m_startHu[2]);
		WriteKeyValue(pack, NAME_TO_STR(m_startHu3), m_startHu[3]);

		WriteKey(pack, "m_mgang");
		veci.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_mgang[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_dmgang");
		veci.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_dmgang[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_dgang");
		veci.clear();
		for (Lint i = 0; i < m_user_count; ++i)
		{
			veci.push_back(m_dgang[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "array0");
		std::vector<CardValue> vec;
		for (Lint i = 0; i < m_count[0]; ++i)
		{
			vec.push_back(m_card[0][i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "array1");
		vec.clear();
		for (Lint i = 0; i < m_count[1]; ++i)
		{
			vec.push_back(m_card[1][i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "array2");
		vec.clear();
		for (Lint i = 0; i < m_count[2]; ++i)
		{
			vec.push_back(m_card[2][i]);
		}
		WriteKey(pack, vec);

		WriteKey(pack, "array3");
		vec.clear();
		for (Lint i = 0; i < m_count[3]; ++i)
		{
			vec.push_back(m_card[3][i]);
		}
		WriteKey(pack, vec);

		WriteKeyValue(pack, NAME_TO_STR(m_hucards1), m_hucards[0]);
		WriteKeyValue(pack, NAME_TO_STR(m_hucards2), m_hucards[1]);
		WriteKeyValue(pack, NAME_TO_STR(m_hucards3), m_hucards[2]);
		WriteKeyValue(pack, NAME_TO_STR(m_hucards4), m_hucards[3]);

		WriteKeyValue(pack, NAME_TO_STR(m_bird_infos), m_bird_infos);

		//////////////////长沙棋牌////////////

		//////////////////广东 棋牌//////////
		WriteKeyValue(pack, NAME_TO_STR(m_nHorseHitAll), m_nHorseHitAll);
		WriteKeyValue(pack, NAME_TO_STR(m_ghostCard), m_ghostCard);
		WriteKeyValue(pack, NAME_TO_STR(m_playerNum), m_playerNum);
		WriteKeyValue(pack, NAME_TO_STR(m_gengZhuangNum), m_gengZhuangNum);
		
		WriteKey(pack, "m_successive");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_successive[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_redDragonNum");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_redDragonNum[i]);
		}
		WriteKey(pack, veci);
		WriteKeyValue(pack, NAME_TO_STR(m_hucards), m_gd_hucards);
		/////////////////广东 棋牌///////////
		
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CGameOver();}
};


//////////////////////////////////////////////////////////////////////////
//客户端请求退出房间;
struct LMsgC2SUserOutRoom:public LMsgSC
{
	Lint m_pos;//准备玩家的位置;

	LMsgC2SUserOutRoom():LMsgSC(MSG_C_2_S_USER_OUT_ROOM), m_pos(-1){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserOutRoom();}
};

//////////////////////////////////////////////////////////////////////////
//服务返回退出结果;
struct LMsgS2CUserOutRoom:public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-失败;
	Lint		m_pos;
	
	LMsgS2CUserOutRoom():LMsgSC(MSG_S_2_C_USER_OUT_ROOM), m_errorCode(0), m_pos(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserOutRoom();}
};


//////////////////////////////////////////////////////////////////////////
//服务器发送起手胡消息;

struct StartHuInfo
{
	Lint	m_type;
	std::vector<CardValue> m_card;

	MSGPACK_DEFINE(m_type, m_card); 
};

struct LMsgS2CUserStartHu:public LMsgSC
{
	std::vector<StartHuInfo> m_card;

	LMsgS2CUserStartHu():LMsgSC(MSG_S_2_C_START_HU){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);

		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserStartHu();}
};

//////////////////////////////////////////////////////////////////////////
//服务返回退出结果;
struct LMsgC2SUserStartHuSelect:public LMsgSC
{
	Lint		m_type;
	std::vector<CardValue> m_card;

	LMsgC2SUserStartHuSelect():LMsgSC(MSG_C_2_S_START_HU_SELECT), m_type(0){}

	virtual bool Read(msgpack::object& obj)
	{

		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		//ReadMapData(obj, NAME_TO_STR(m_card), m_card);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_card), array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				CardValue v;
				msgpack::object& obj =  *(array.via.array.ptr+i);
				obj.convert(v);
				m_card.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserStartHuSelect();}
};

//////////////////////////////////////////////////////////////////////////
//服务广播玩家操作;
struct LMsgS2CUserStartHuSelect:public LMsgSC
{
	Lint		m_pos;
	Lint		m_type;
	std::vector<CardValue> m_card;

	LMsgS2CUserStartHuSelect():LMsgSC(MSG_S_2_C_START_HU_SELECT), m_pos(0), m_type(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserStartHuSelect();}
};

//////////////////////////////////////////////////////////////////////////
//玩家杠牌;
struct LMsgS2CUserGangCard:public LMsgSC
{
	Lint		m_pos;
	Lint		m_flag;//0-没有自摸，1-自摸;
	std::vector<CardValue> m_card;

	LMsgS2CUserGangCard():LMsgSC(MSG_S_2_C_GANG_RESULT), m_pos(0), m_flag(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserGangCard();}
};


//////////////////////////////////////////////////////////////////////////
//服务器广播玩家选择海底牌;
struct LMsgS2CUserEndCardSelect:public LMsgSC
{
	Lint		m_pos;
	Lint		m_time;

	LMsgS2CUserEndCardSelect():LMsgSC(MSG_S_2_C_END_CARD_SELECT), m_pos(0), m_time(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserEndCardSelect();}
};

//////////////////////////////////////////////////////////////////////////
//客户的选择海底;
struct LMsgC2SUserEndCardSelect:public LMsgSC
{
	Lint		m_flag;//0-不要，1-要;

	LMsgC2SUserEndCardSelect():LMsgSC(MSG_C_2_S_END_SELECT), m_flag(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){return new LMsgC2SUserEndCardSelect();}
};


//////////////////////////////////////////////////////////////////////////
//服务器广播玩家摸海底;
struct LMsgS2CUserEndCardGet:public LMsgSC
{
	Lint		m_pos;	//位置;
	Lint		m_flag;//是否自摸 0-没有，1-自摸;
	CardValue	m_card;

	LMsgS2CUserEndCardGet():LMsgSC(MSG_S_2_C_END_GET), m_pos(0), m_flag(0){}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone(){return new LMsgS2CUserEndCardGet();}
};

//////////////////////////////////////////////////////////////////////////
//客户端请求说话;
struct LMsgC2SUserSpeak:public LMsgSC
{
	Lint		m_type;//说话类型，0-表情，1-语音,2-文字;
	Lint		m_id;	//表情id;
	Lstring		m_musicUrl; //语音地址链接;
	Lstring		m_msg;//聊天文字;

	LMsgC2SUserSpeak() :LMsgSC(MSG_C_2_S_USER_SPEAK), m_type(0), m_id(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_musicUrl), m_musicUrl);
		ReadMapData(obj, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SUserSpeak(); }
};

//////////////////////////////////////////////////////////////////////////
//活动协议
struct LMsgS2CActiveOpen:public LMsgSC
{
	Lint		m_activeID;			//活动ID

	LMsgS2CActiveOpen() :LMsgSC(MSG_C_2_S_ACTIVE_INFO), m_activeID(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActiveOpen(); }
};

struct LMsgC2SActiveInfoRequest:public LMsgSC
{
	Lint		m_activeID;			//活动ID

	LMsgC2SActiveInfoRequest() :LMsgSC(MSG_S_2_C_ACTIVE_OPEN), m_activeID(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActiveInfoRequest(); }
};

struct LMsgS2CActiveInfo:public LMsgSC
{
	Lint		m_activeID;			//活动ID;
	Lint		m_RewardID;			//1，2，3，4;
	Lint		m_LastJoinDate;		//抽奖日期;
	Lint		m_LastGiftState;	//0无状态， 1已抽奖;
	Lint		m_NeedPhoneNum; 	//0不需要   1需要留电话 ;
	Lint        m_LeftNums;         //剩余抽奖次数;
	Lint        m_error;            //失败；1，活动未开始;

	LMsgS2CActiveInfo() :LMsgSC(MSG_S_2_C_ACTIVE_INFO), m_activeID(0), m_RewardID(0), m_LastJoinDate(0), m_LastGiftState(0), m_NeedPhoneNum(0), m_LeftNums(0), m_error(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_activeID), m_activeID);
		ReadMapData(obj, NAME_TO_STR(m_RewardID), m_RewardID);
		ReadMapData(obj, NAME_TO_STR(m_LastJoinDate), m_LastJoinDate);
		ReadMapData(obj, NAME_TO_STR(m_LastGiftState), m_LastGiftState);
		ReadMapData(obj, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		ReadMapData(obj, NAME_TO_STR(m_LeftNums), m_LeftNums);
		ReadMapData(obj, NAME_TO_STR(m_error), m_error);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_activeID), m_activeID);
		WriteKeyValue(pack, NAME_TO_STR(m_RewardID), m_RewardID);
		WriteKeyValue(pack, NAME_TO_STR(m_LastJoinDate), m_LastJoinDate);
		WriteKeyValue(pack, NAME_TO_STR(m_LastGiftState), m_LastGiftState);
		WriteKeyValue(pack, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		WriteKeyValue(pack, NAME_TO_STR(m_LeftNums), m_LeftNums);
		WriteKeyValue(pack, NAME_TO_STR(m_error), m_error);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CActiveInfo(); }
};

struct LMsgC2SWealthChange :public LMsgSC
{
	Lint m_Weath1Type = 0;   //消耗玩家物品类型；
	Lint m_Weath1Num = 0;    //消耗玩家物品数量；
	Lint m_Weath2Type = 0;   //需要换到的物品类型;
	LMsgC2SWealthChange() :LMsgSC(MSG_C_2_S_WEALTH_CHANGE) { }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_msgId), m_msgId);
		ReadMapData(obj, NAME_TO_STR(m_Weath1Type), m_Weath1Type);
		ReadMapData(obj, NAME_TO_STR(m_Weath1Num), m_Weath1Num);
		ReadMapData(obj, NAME_TO_STR(m_Weath2Type), m_Weath2Type);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_Weath1Type), m_Weath1Type);
		WriteKeyValue(pack, NAME_TO_STR(m_Weath1Num), m_Weath1Num);
		WriteKeyValue(pack, NAME_TO_STR(m_Weath2Type), m_Weath2Type);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgC2SWealthChange(); }
};

struct LMsgS2CWealthChangeResult :public LMsgSC
{
	Lint m_Weath1Type = 0;   //消耗玩家物品类型；
	Lint m_Weath1Num = 0;    //消耗玩家物品数量；
	Lint m_Weath2Type = 0;   //需要换到的物品类型;
	Lint m_Weath2Num = 0;   //换到的物品数量;
	LMsgS2CWealthChangeResult() :LMsgSC(MSG_S_2_C_WEALTH_CHANGE_RESULT) { }

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_Weath1Type), m_Weath1Type);
		WriteKeyValue(pack, NAME_TO_STR(m_Weath1Num), m_Weath1Num);
		WriteKeyValue(pack, NAME_TO_STR(m_Weath2Type), m_Weath2Type);
		WriteKeyValue(pack, NAME_TO_STR(m_Weath2Num), m_Weath2Num);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgS2CWealthChangeResult(); }
};

struct LMsgC2SRequestDraw:public LMsgSC
{

	LMsgC2SRequestDraw() :LMsgSC(MSG_C_2_S_REQUEST_DRAW){ }

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SRequestDraw(); }
};


struct LMsgS2CFinishDraw:public LMsgSC
{
	Lint		m_errorCode;			//1,没有抽奖次数;2、活动未开启 3、抽奖失败，重新抽奖 4、金币不足抽奖  5、未打够金币场数;
	Lint		m_RewardID;			//1，2，3，4;
	Lint		m_date;;			//1，2，3，4;
	Lint		m_NeedPhoneNum;		//是否需要电话;
	LMsgS2CFinishDraw() :LMsgSC(MSG_S_2_C_FINISH_DRAW), m_errorCode(0), m_RewardID(0), m_date(0), m_NeedPhoneNum(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_RewardID), m_RewardID);
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_RewardID), m_RewardID);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_NeedPhoneNum), m_NeedPhoneNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CFinishDraw(); }
};

struct LMsgC2SWritePhone : public LMsgSC
{
	Lint		m_activeID;			//活动ID;
	Lint		m_date;			//1，2，3，4;
	Lstring		m_phoneNum;		//日期;
	LMsgC2SWritePhone() :LMsgSC(MSG_C_2_S_WRITE_PHONE), m_activeID(0), m_date(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_activeID), m_activeID);
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_phoneNum), m_phoneNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_activeID), m_activeID);
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_phoneNum), m_phoneNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SWritePhone(); }
};


struct LMsgS2CWritePhone:public LMsgSC
{
	Lint		m_errorCode;			//活动ID;
	Lstring		m_PhoneNum;			//电话;
	LMsgS2CWritePhone() :LMsgSC(MSG_S_2_C_WRITE_PHONE), m_errorCode(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_PhoneNum), m_PhoneNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_PhoneNum), m_PhoneNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CWritePhone(); }
};

struct LMsgC2SActiveLog:public LMsgSC
{

	LMsgC2SActiveLog() :LMsgSC(MSG_C_2_S_ACTIVE_LOG){ }

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SActiveLog(); }
};
struct LActiveInfo
{
	Lint    m_activeID;
	Lint	m_rewardID = 0;
	Lint	m_date = 0;
	Lstring	m_phoneNum;
	MSGPACK_DEFINE(m_rewardID, m_date,m_phoneNum, m_activeID);
	LActiveInfo()
	{
		m_activeID = 0;
		m_date = 0;
		m_rewardID = 0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_rewardID), m_rewardID);
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_phoneNum), m_phoneNum);
		ReadMapData(obj, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_rewardID), m_rewardID);
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_phoneNum), m_phoneNum);
		WriteKeyValue(pack, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}
};
struct LMsgS2CDrawInfo:public LMsgSC
{
	Lint		m_num;			//活动ID;
	std::vector<LActiveInfo>	m_DrawInfo;
	LMsgS2CDrawInfo() :LMsgSC(MSG_S_2_C_ACTIVE_LOG), m_num(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_num), m_num);
		ReadMapData(obj, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_num), m_num);
		WriteKeyValue(pack, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CDrawInfo(); }
};


//////////////////////////////////////////////////////////////////////////
//服务广播操作结果
struct LMsgS2CUserSpeak:public LMsgSC
{
	Lint		m_userId;//说话人id;
	Lint		m_pos;//说话人的位置;
	Lint		m_type;//说话类型;
	Lint		m_id;//类型id;
	Lstring		m_musicUrl;//语音url链接;
	Lstring		m_msg;//聊天文字;

	LMsgS2CUserSpeak() :LMsgSC(MSG_S_2_C_USER_SPEAK), m_userId(0), m_pos(0), m_type(0), m_id(0){ }

	
	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		//ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserSpeak(); }
};


//////////////////////////////////////////////////////////////////////////
//服务广播玩家报单
struct LMsgS2COnlyOneCard:public LMsgSC
{
	Lint		m_pos;//说话人的位置;
	Lint        m_leftCards;
	
	LMsgS2COnlyOneCard() :LMsgSC(MSG_S_2_C_ONLYONE_CARD), m_pos(0), m_leftCards(0){ }


	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_leftCards), m_leftCards);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2COnlyOneCard(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求创建桌子;
struct LMsgC2SCreateDesk :public LMsgSC
{
	Lint        m_GameZone;//游戏使用房卡还是金币;金币场为1，其他目前均为房卡场;
	Lint		m_flag;//房间类型，1-2圈，2-四圈，3-8圈;
	Lstring		m_secret;//房间密码，如果为空，服务器会随机一个密码;
	Lint		m_gold;//房间抵住，单位元，最低为1元，最多不能超过100;
	Lint		m_gameType;//游戏类型;
	Lint        m_nMaxFanshu;//最大翻倍数;
	Lint		m_robotNum;		// 0,不加机器人，1，2，3加机器人数量;
	Lint        m_lenth;     // 调牌时送来长度;
	Lint        m_black3first; //第一轮先手出牌是否可以不带黑桃3;  1为可以不带黑桃3，0是必须带黑桃3;
	CardValue	m_cardValue[CARD_COUNT];
	std::vector<Lint> m_extValue;
	Lint		m_playerNum = 0;	//游戏玩家数目(2,3,4)
	Lint		m_useTeamCard = 0;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
	Lint		m_teamID = 0;					// 工会ID [2017-7-23 willing]
	std::vector<Lint>	m_playtype;	//玩法
	Lint        m_stake = 0;                 //押注倍数
	Lint        m_circleGoldLimit = 0;       //每局限制进入的金币 

	LMsgC2SCreateDesk() :LMsgSC(MSG_C_2_S_CREATE_ROOM){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_GameZone), m_GameZone);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_secret), m_secret);
		ReadMapData(obj, NAME_TO_STR(m_gold), m_gold);
		ReadMapData(obj, NAME_TO_STR(m_state), m_gameType);
		ReadMapData(obj, NAME_TO_STR(m_nMaxFanshu), m_nMaxFanshu);
		ReadMapData(obj, NAME_TO_STR(m_robotNum), m_robotNum);
		ReadMapData(obj, NAME_TO_STR(m_lenth), m_lenth);
		ReadMapData(obj, NAME_TO_STR(m_black3first), m_black3first);
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_cardValue), array);
		if (array.type == msgpack::type::ARRAY)
		{
			for (Lsize i = 0; i < array.via.array.size && i < CARD_COUNT; ++i)
			{
				msgpack::object& pv = *(array.via.array.ptr + i);
				pv.convert(m_cardValue[i]);
			}
		}

		ReadMapVector(obj, NAME_TO_STR(m_extValue), m_extValue);
		ReadMapData(obj, NAME_TO_STR(m_playerNum), m_playerNum);
		ReadMapData(obj, NAME_TO_STR(m_useTeamCard), m_useTeamCard);
		ReadMapData(obj, NAME_TO_STR(m_teamID), m_teamID);
		ReadMapData(obj, NAME_TO_STR(m_stake), m_stake);
		ReadMapData(obj, NAME_TO_STR(m_circleGoldLimit), m_circleGoldLimit);
		ReadMapData(obj, NAME_TO_STR(m_playtype), array);
		if (array.type == MSGPACK_OBJECT_ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				Lint v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_playtype.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		//WriteMap(pack, 5);
		//WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		//WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);
		//WriteKeyValue(pack, NAME_TO_STR(m_secret), m_secret);
		//WriteKeyValue(pack, NAME_TO_STR(m_gold), m_gold);
		//WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		//WriteKeyValue(pack, NAME_TO_STR(m_robotNum), m_robotNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SCreateDesk(); }
};

//////////////////////////////////////////////////////////////////////////
//玩家请求创建桌子结果;
struct LMsgS2CCreateDeskRet :public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-房卡不够，2-其他未知错误; 3,未退出房卡场试图进入金币场;4、金币场暂时无法登陆;5,玩法类型有误 8 商圈服务器异常，11、金币不足；
	Lint		m_deskId;//桌子id;

	LMsgS2CCreateDeskRet() :LMsgSC(MSG_S_2_C_CREATE_ROOM), m_errorCode(0), m_deskId(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CCreateDeskRet(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端加入桌子
struct LMsgC2SAddDesk :public LMsgSC
{
	Lint		m_deskId;//桌子id;
	Lstring		m_location; //玩家地理位置
	Lint		m_useTeamCard;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]

	LMsgC2SAddDesk() :LMsgSC(MSG_C_2_S_ADD_ROOM), m_deskId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		ReadMapData(obj, NAME_TO_STR(m_location), m_location);
		ReadMapData(obj, NAME_TO_STR(m_useTeamCard), m_useTeamCard);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_location), m_location);
		WriteKeyValue(pack, NAME_TO_STR(m_useTeamCard), m_useTeamCard);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SAddDesk(); }
};

//////////////////////////////////////////////////////////////////////////
//玩家请求加入桌子结果
struct LMsgS2CAddDeskRet :public LMsgSC
{
	Lint		m_errorCode;//0-成功，1-人已满,2-房间不存在,3-未知错误
	Lint		m_deskId;//桌子号码
	
	LMsgS2CAddDeskRet() :LMsgSC(MSG_S_2_C_ADD_ROOM), m_errorCode(0), m_deskId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CAddDeskRet(); }
};


//////////////////////////////////////////////////////////////////////////
//vip比赛结束
struct LMsgS2CVipEnd :public LMsgSC
{
	Lint	m_playerNum;					//玩家数
	Lint	m_id[4];//玩家id
	Lstring	m_nike[4];//游戏玩家昵称
	Lstring	m_imageUrl[4];//游戏玩家昵称	
	Ldouble	m_gold[4];//输赢积分总数
	Ldouble	m_score[4];//输赢积分实际积分
	Lint	m_bomb[4];//放炮次数
	Lint	m_win[4];//收炮次数
	Lint	m_best[4];
	Lint	m_lose[4];//三家剩余排数

	/////////////////////长沙////////////
	Lint	m_zimo[4];//自摸次数
	Lint	m_mgang[4];//明杠次数
	Lint	m_agang[4];//暗杠的次数

	Lint	m_dzimo[4];//大胡自摸次数
	Lint	m_dbomb[4];//大胡放炮次数
	Lint	m_dwin[4];//大胡收炮次数
	////////////////////长沙/////////////

	////////////////////广东////////////
	Lint	m_baohu[DEFAULT_DESK_USER_COUNT];//爆胡次数
	Lint	m_fannum[DEFAULT_DESK_USER_COUNT];//番数
	Lint	m_maNum[DEFAULT_DESK_USER_COUNT];//中马总数
	Lint	m_ghostZimo[DEFAULT_DESK_USER_COUNT];//有鬼自摸次数
	Lint	m_noghostZimo[DEFAULT_DESK_USER_COUNT];//无鬼自摸次数
	Lint	m_normalhu[DEFAULT_DESK_USER_COUNT];		//普通胡
	Lint    m_singleflow[DEFAULT_DESK_USER_COUNT];	//花单吊

	///////////////////广东end//////////

	LMsgS2CVipEnd() :LMsgSC(MSG_S_2_C_VIP_END)
	{
		m_playerNum = 0;
		memset(m_baohu, 0, sizeof(m_baohu));
		memset(m_fannum, 0, sizeof(m_fannum));
		memset(m_maNum, 0, sizeof(m_maNum));
		memset(m_ghostZimo, 0, sizeof(m_ghostZimo));
		memset(m_noghostZimo, 0, sizeof(m_noghostZimo));
		memset(m_normalhu, 0, sizeof(m_normalhu));
		memset(m_singleflow, 0, sizeof(m_singleflow));
	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 10 + 6 + 8);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKeyValue(pack, NAME_TO_STR(m_playerNum), m_playerNum);
		WriteKey(pack,"m_nike");
		std::vector<Lstring>vecs;
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecs.push_back(m_nike[i]);
		}
		WriteKey(pack,vecs);

		WriteKey(pack,"m_imageUrl");
		vecs.clear();
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecs.push_back(m_imageUrl[i]);
		}
		WriteKey(pack,vecs);

		WriteKey(pack,"m_id");
		std::vector<Lint>veci;
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_id[i]);
		}
		WriteKey(pack,veci);

		std::vector<Ldouble>vecf;
		WriteKey(pack,"m_gold");
		vecf.clear();
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecf.push_back(m_gold[i]);
		}
		WriteKey(pack, vecf);

		WriteKey(pack,"m_win");
		veci.clear();
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_win[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_lose");
		veci.clear();
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_lose[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_bomb");
		veci.clear();
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_bomb[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack,"m_best");
		veci.clear();
		for(Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_best[i]);
		}
		WriteKey(pack,veci);

		WriteKey(pack, "m_score");
		vecf.clear();
		for (Lint i = 0; i < DESK_USER_COUNT; ++i)
		{
			vecf.push_back(m_score[i]);
		}
		WriteKey(pack, vecf);


		WriteKey(pack, "m_zimo");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_zimo[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_mgang");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_mgang[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_agang");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_agang[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_dzimo");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dzimo[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_dbomb");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dbomb[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_dwin");
		veci.clear();
		for (Lint i = 0; i < 4; ++i)
		{
			veci.push_back(m_dwin[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_baohu");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_baohu[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_fannum");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_fannum[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_maNum");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_maNum[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_ghostZimo");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_ghostZimo[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_noghostZimo");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_noghostZimo[i]);
		}
		WriteKey(pack, veci);

		//
		WriteKey(pack, "m_normalhu");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_normalhu[i]);
		}
		WriteKey(pack, veci);

		WriteKey(pack, "m_singleflow");
		veci.clear();
		for (Lint i = 0; i < DEFAULT_DESK_USER_COUNT; ++i)
		{
			veci.push_back(m_singleflow[i]);
		}
		WriteKey(pack, veci);
		return true;
	}


	virtual LMsg* Clone(){ return new LMsgS2CVipEnd(); }
};

//////////////////////////////////////////////////////////////////////////
//客户端请求对账单
struct LMsgC2SVipLog :public LMsgSC
{
	Lint		m_time;//id
	Lint		m_userId;//请求某个玩家的战绩
	
	LMsgC2SVipLog() :LMsgSC(MSG_C_2_S_VIP_LOG), m_time(0), m_userId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SVipLog(); }
};

//////////////////////////////////////////////////////////////////////////
//vip对账结果

struct VipLog
{
	VipLog():m_time(0){}
	Lint	m_time;
	Lstring	m_videoId;
	Ldouble  m_score[4] = { 0 };
	Ldouble	m_calScore[4] = { 0 };
};

struct VipLogItemInfo
{
	VipLogItemInfo(): m_time(0), m_flag(0), m_reset(0), m_deskId(0), m_size(0){}
	Lstring m_id;
	Lint m_time;
	Lint m_flag;
	Lint m_userid[4] = { 0 };
	Lstring m_nike[4];
	Ldouble m_score[4] = { 0 };
	Ldouble m_calScore[4] = { 0 };
	Ldouble m_iniScore[4] = { 0 };
	Lint m_reset;//是否解散;
	Lint m_deskId;//是否解散;
	Lint m_size;
	std::vector<Lint> m_playtype;
	VipLog m_item[32];
};

struct LMsgS2CVipLog:public LMsgSC
{
	Lint				m_size;//长度;
	VipLogItemInfo		m_info[20];//log信息;
	
	LMsgS2CVipLog() :LMsgSC(MSG_S_2_C_VIP_LOG), m_size(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack,"m_data");
		WriteArray(pack,m_size);
		for(Lint i = 0 ; i < m_size; ++i)
		{
			WriteMap(pack,9);
			WriteKeyValue(pack,"m_id",m_info[i].m_id);
			WriteKeyValue(pack,"m_time",m_info[i].m_time);
			WriteKeyValue(pack,"m_flag",m_info[i].m_flag);
			//WriteKeyValue(pack,"m_reset",m_info[i].m_reset);
			WriteKeyValue(pack,"m_deskId",m_info[i].m_deskId);

			std::vector<Ldouble> vecf;
			//WriteKey(pack, "m_playType");
			//WriteArray(pack, m_info[i].m_playtype.size()/2);
			//for (Lint jj = 0; jj + 1 < m_info[i].m_playtype.size(); jj += 2)
			//{
			//	WriteMap(pack,1);
			//	//veci.push_back(m_info[i].m_playtype[jj]);
			//	veci.push_back(m_info[i].m_playtype[jj + 1]);
			//	WriteKey(pack,getGameTypeStr(m_info[i].m_playtype[jj]));
			//	WriteKey(pack,veci);
			//	veci.clear();
			//}
			std::vector<Lstring> vecs;
			std::vector<Lint>vec;
			for(Lint j = 0 ; j < DESK_USER_COUNT; ++j)
			{
				vecs.push_back(m_info[i].m_nike[j]);
				vecf.push_back(m_info[i].m_score[j]);
				vec.push_back(m_info[i].m_userid[j]);
			}
			WriteKey(pack,"m_userid");
			WriteKey(pack,vec);

			WriteKey(pack,"m_nike");
			WriteKey(pack,vecs);

			WriteKey(pack,"m_score");
			WriteKey(pack, vecf);

			vecf.clear();
			for (Lint j = 0; j < DESK_USER_COUNT; ++j)
			{
				vecf.push_back(m_info[i].m_calScore[j]);
			}

			WriteKey(pack, "m_calScore");
			WriteKey(pack, vecf);

			vecf.clear();
			for (Lint j = 0; j < DESK_USER_COUNT; ++j)
			{
				vecf.push_back(m_info[i].m_iniScore[j]);
			}

			WriteKey(pack, "m_iniScore");
			WriteKey(pack, vecf);

			//WriteKey(pack,"m_match");
			//WriteArray(pack,m_info[i].m_size);
			//for(Lint k = 0 ; k < m_info[i].m_size; ++k)
			//{
			//	WriteMap(pack,3);
			//	WriteKeyValue(pack,"m_time",m_info[i].m_item[k].m_time);
			//	WriteKeyValue(pack,"m_videoId",m_info[i].m_item[k].m_videoId);

			//	std::vector<Lint> vec;
			//	for(Lint n = 0 ; n < DESK_USER_COUNT; ++n)
			//	{
			//		vec.push_back(m_info[i].m_item[k].m_score[n]);
			//	}
			//	WriteKey(pack,"m_score");
			//	WriteKey(pack,vec);
			//}
		}
	
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVipLog(); }
};

//客户端请求某轮对账单;
struct LMsgC2SOneVipLog :public LMsgSC
{
	Lstring		m_log_id;//请求某logid的对账单;
	Lint		m_userId;

	LMsgC2SOneVipLog() :LMsgSC(MSG_C_2_S_ONE_VIP_LOG),m_userId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_id), m_log_id);
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}
	virtual LMsg* Clone(){ return new LMsgC2SOneVipLog(); }
};

//服务器返回玩家某轮对账单
struct LMsgS2COneVipLog:public LMsgSC
{
	Lint	m_size;
	VipLog	m_videos[20];
	Lint	m_user_count;
	Lint    m_flag;
	std::vector<Lint>    m_playType;

	LMsgS2COneVipLog() :LMsgSC(MSG_S_2_C_ONE_VIP_LOG),m_size(0), m_user_count(0), m_flag(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKeyValue(pack,"m_flag", m_flag);
		std::vector<Lint> veci;
		WriteKey(pack, "m_playType");
		WriteArray(pack, m_playType.size()/2);
		for (size_t jj = 0; jj + 1 < m_playType.size(); jj += 2)
		{
			WriteMap(pack,1);
			//veci.push_back(m_info[i].m_playtype[jj]);
			veci.push_back(m_playType[jj + 1]);
			WriteKey(pack,getGameTypeStr(m_playType[jj]));
			WriteKey(pack,veci);
			veci.clear();
		}

		WriteKey(pack,"m_match");
		WriteArray(pack,m_size);
		for(Lint k = 0 ; k < m_size; ++k)
		{
			WriteMap(pack,4);
			WriteKeyValue(pack,"m_time",m_videos[k].m_time);
			WriteKeyValue(pack,"m_videoId",m_videos[k].m_videoId);

			std::vector<Ldouble> vecf;
			for(Lint n = 0 ; n < m_user_count; ++n)
			{
				vecf.push_back(m_videos[k].m_score[n]);
			}
			WriteKey(pack,"m_score");
			WriteKey(pack, vecf);

			vecf.clear();
			for (Lint n = 0; n < m_user_count; ++n)
			{
				vecf.push_back(m_videos[k].m_calScore[n]);
			}
			WriteKey(pack, "m_calScore");
			WriteKey(pack, vecf);
		}
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2COneVipLog(); }
};


//////////////////////////////////////////////////////////////////////////
//客户端请求录像;
struct LMsgC2SVideo :public LMsgSC
{
	Lstring		m_videoId;//id

	LMsgC2SVideo() :LMsgSC(MSG_C_2_S_VIDEO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SVideo(); }
};

//////////////////////////////////////////////////////////////////////////
//vip对账结果;

struct LMsgS2CVideo:public LMsgSC
{
	VideoLog		m_video;//log信息;
	Lint			m_sex[4] = { 0 };
	Lstring			m_nike[4];
	Lstring			m_imageUrl[4];
	Lint			m_deskId = 0;
	Lint			m_curCircle = 0;
	Lint			m_maxCircle = 0;
	Lint			m_flag = 0;
	Ldouble			m_score[4] = { 0 };
	Ldouble			m_calScore[4] = { 0 };
	Lint			m_user_count;
	Lint			m_zhuang;
	
	LMsgS2CVideo() :LMsgSC(MSG_S_2_C_VIDEO), m_user_count(4) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 20);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_deskId), m_video.m_deskId);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_video.m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_maxCircle), m_video.m_maxCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_video.m_flag);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_video.m_time);

		std::vector<Lint> veci;
		WriteKey(pack, "m_playType");
		WriteArray(pack, m_video.m_playType.size()/2);
		for (size_t jj = 0; jj + 1 < m_video.m_playType.size(); jj += 2)
		{
			WriteMap(pack,1);
			//veci.push_back(m_video.m_playType[jj]);   //不需要给客户端;客户端只需要getGameTypeStr获取的字符串即可;
			veci.push_back(m_video.m_playType[jj + 1]);
			WriteKey(pack,getGameTypeStr(m_video.m_playType[jj]));
			WriteKey(pack,veci);
			veci.clear();
		}

		std::vector<Lint>vecid;
		std::vector<Lint>vecsex;
		std::vector<Ldouble>vecscore;
		std::vector<Ldouble>veccalscore;
		std::vector<Lstring>vecnike;
		std::vector<Lstring>vecurl;

		for(Lint i = 0 ; i < DESK_USER_COUNT; ++i)
		{
			vecid.push_back(m_video.m_userId[i]);
			vecnike.push_back(m_nike[i]);
			vecsex.push_back(m_sex[i]);
			vecurl.push_back(m_imageUrl[i]);
			vecscore.push_back(m_video.m_score[i]);
			veccalscore.push_back(m_video.m_calScore[i]);
		}

		WriteKey(pack, "m_calScore");
		WriteKey(pack, veccalscore);

		WriteKey(pack,"m_score");
		WriteKey(pack,vecscore);

		WriteKey(pack,"m_userid");
		WriteKey(pack,vecid);

		WriteKey(pack,"m_nike");
		WriteKey(pack, vecnike);

		WriteKey(pack, "m_sex");
		WriteKey(pack, vecsex);

		WriteKey(pack, "m_imageUrl");
		WriteKey(pack, vecurl);

		WriteKey(pack,"m_time");
		WriteKey(pack,m_video.m_time);

		WriteKey(pack,"m_oper");
		WriteKey(pack,m_video.m_oper);

		for(Lint i = 0 ; i < DESK_USER_COUNT; ++i)
		{
			std::stringstream ss ;
			ss << "m_card" << i;
			WriteKey(pack,ss.str());
			WriteKey(pack,m_video.m_handCard[i]);
		}

		WriteKeyValue(pack, NAME_TO_STR(m_zhuang), m_zhuang);

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVideo(); }
};

//////////////////////////////////////////////////////////////////////////
//同步vip房间信息;
struct LMsgS2CVipInfo :public LMsgSC
{
	Lint			m_curCircle = 0;//当前局数;
	Lint			m_curMaxCircle = 0;//最大局数;

	LMsgS2CVipInfo() :LMsgSC(MSG_S_2_C_VIP_INFO){}

	virtual bool Read(msgpack::object& obj)
	{
		//ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		//ReadMapData(obj, NAME_TO_STR(m_deskId), m_deskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_curCircle), m_curCircle);
		WriteKeyValue(pack, NAME_TO_STR(m_curMaxCircle), m_curMaxCircle);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CVipInfo(); }
};
//
//struct LMsgS2CUserChargeDiamond : public LMsgSC
//{
//	Lint		m_error;			// 0-success, 1-failed
//
//	LMsgS2CUserChargeDiamond() :LMsgSC(MSG_S_2_C_USER_CHARGE_DIAMOND),m_error(0){}
//
//	virtual bool Read(msgpack::object& obj)
//	{
//		ReadMapData(obj, NAME_TO_STR(m_error), m_error);
//		return true;
//	}
//
//	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
//	{
//		WriteMap(pack, 2);
//		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
//		WriteKeyValue(pack, NAME_TO_STR(m_error), m_error);
//		return true;
//	}
//
//	virtual LMsg* Clone(){ return new LMsgS2CUserChargeDiamond(); }
//};

struct LMsgS2CUserPurchase :public LMsgSC
{
	Lint		m_error;			// 0-success, 1-failed
	Lint		m_itemId;			// 道具Id

	LMsgS2CUserPurchase()
		:LMsgSC(MSG_S_2_C_USER_PURCHASE), m_error(0), m_itemId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_error), m_error);
		ReadMapData(obj, NAME_TO_STR(m_itemId), m_itemId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), m_error);
		WriteKeyValue(pack, NAME_TO_STR(m_itemId), m_itemId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CUserPurchase(); }
};

struct LMsgS2CPlayerType :public LMsgSC
{
	Lstring		m_playerType;

	LMsgS2CPlayerType() :LMsgSC(MSG_S_2_C_PLAYER_TYPE){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_playerType), m_playerType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_playerType), m_playerType);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CPlayerType(); }
};

//玩家通过精确的videoid查看分享的某录像;
struct LMsgC2SGetShareVideo :public LMsgSC
{
	Lstring		m_videoId;//id

	LMsgC2SGetShareVideo() :LMsgSC(MSG_C_2_S_GETSHAREVIDEO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_videoId), m_videoId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGetShareVideo(); }
};

//玩家请求查看某shareid的分享录像;
struct LMsgC2SShareVideo :public LMsgSC
{
	Lint		m_shareID;//id

	LMsgC2SShareVideo() :LMsgSC(MSG_C_2_S_SHAREVIDEO),m_shareID(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_shareID), m_shareID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_shareID), m_shareID);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SShareVideo(); }
};

//玩家请求分享某录像;
struct LMsgC2SReqShareVideo :public LMsgSC
{
	Lstring		m_videoid;//id

	LMsgC2SReqShareVideo() :LMsgSC(MSG_C_2_S_REQSHAREVIDEO){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_videoId), m_videoid);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_videoid), m_videoid);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SReqShareVideo(); }
};

//服务器返回分享的id;
struct LMsgS2CRetShareVideoId :public LMsgSC
{
	Lint		m_errorId;//0成功 1失败
	Lint		m_shareId;//id

	LMsgS2CRetShareVideoId() :LMsgSC(MSG_S_2_C_RETSHAREVIDEOID),m_errorId(0),m_shareId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorId), m_errorId);
		ReadMapData(obj, NAME_TO_STR(m_shareId), m_shareId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorId), m_errorId);
		WriteKeyValue(pack, NAME_TO_STR(m_shareId), m_shareId);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CRetShareVideoId(); }
};


struct S2CShareVideo{
	Lstring	m_videoId;
	Lint	m_time;
	Lstring	m_userNike[4];
	Ldouble	m_score[4];
};

//服务器返回某shareid对应的分享录像;
struct LMsgS2CShareVideo:public LMsgSC
{
	Lint		m_count;
	Lint		m_errorId;//0失败 1成功
	std::vector<S2CShareVideo> m_vec_share_video;

	LMsgS2CShareVideo() :LMsgSC(MSG_S_2_C_SHAREVIDEO),m_errorId(0),m_count(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorId), m_errorId);

		WriteKey(pack,"m_data");
		WriteArray(pack,m_count);

		for(Lint i = 0 ; i < m_count; ++i)
		{
			WriteMap(pack,4);
			WriteKeyValue(pack, NAME_TO_STR(m_videoId),m_vec_share_video[i].m_videoId);
			WriteKeyValue(pack, NAME_TO_STR(m_time), m_vec_share_video[i].m_time);
			std::vector<Ldouble> vecscore;
			std::vector<Lstring> vecnike;
			for(Lint n = 0 ; n < 4; ++n)
			{
				vecnike.push_back(m_vec_share_video[i].m_userNike[n]);
				vecscore.push_back(m_vec_share_video[i].m_score[n]);
			}

			WriteKey(pack,"m_score");
			WriteKey(pack,vecscore);

			WriteKey(pack,"m_nike");
			WriteKey(pack, vecnike);
		}

		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CShareVideo(); }
};

struct gift_info{
	Lint	m_giftId; //1:iphone 2:100元话费
	Lint	m_couponsNeedCount;

	void	Read(LBuff& buff){
		buff.Read(m_giftId);
		buff.Read(m_couponsNeedCount);
	}
	void	Write(LBuff& buff){
		buff.Write(m_giftId);
		buff.Write(m_couponsNeedCount);
	}

	MSGPACK_DEFINE(m_giftId,m_couponsNeedCount);
};

//服务器返回 礼券兑换
struct LMsgS2CCouponsExchange:public LMsgSC
{
	Lint		m_CouponsCount;//礼券数量
	std::vector<gift_info> m_giftInfo;

	LMsgS2CCouponsExchange():LMsgSC(MSG_S_2_C_COUPONS_EXCHANGE),m_CouponsCount(0){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_CouponsCount), m_CouponsCount);
		WriteKeyValue(pack, NAME_TO_STR(m_giftInfo), m_giftInfo);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CCouponsExchange(); }
};

//客户端请求兑换记录
struct LMsgC2SCouponsExchangeRecord:public LMsgSC
{
	Lint		m_userId;
	LMsgC2SCouponsExchangeRecord():LMsgSC(MSG_C_2_S_COUPONS_EXCHANGE_RECORD),m_userId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SCouponsExchangeRecord(); }
};

struct gift_exchange_record
{
	Lint m_giftId;
	Lint m_coupons_need;
	Lint m_time;
	Lint m_state;
	Lstring m_phoneNum;
	MSGPACK_DEFINE(m_giftId,m_coupons_need,m_time,m_state,m_phoneNum); 
	void	Read(LBuff& buff){
		buff.Read(m_giftId);
		buff.Read(m_coupons_need);
		buff.Read(m_time);
		buff.Read(m_state);
		buff.Read(m_phoneNum);
	}
	void	Write(LBuff& buff){
		buff.Write(m_giftId);
		buff.Write(m_coupons_need);
		buff.Write(m_time);
		buff.Write(m_state);
		buff.Write(m_phoneNum);
	}
};
//服务器返回兑换记录
struct LMsgS2CCouponsExchangeRecord:public LMsgSC
{
	std::vector<gift_exchange_record> m_record;
	LMsgS2CCouponsExchangeRecord():LMsgSC(MSG_S_2_C_COUPONS_EXCHANGE_RECORD){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_record), m_record);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CCouponsExchangeRecord(); }
};

//客户端请求兑换商品
struct LMsgC2SGiftExchange:public LMsgSC
{
	Lint m_giftId;
	Lstring m_phoneNum;
	LMsgC2SGiftExchange():LMsgSC(MSG_C_2_S_GIFT_EXCHANGE), m_giftId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_giftId), m_giftId);
		ReadMapData(obj, NAME_TO_STR(m_phoneNum), m_phoneNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SGiftExchange(); }
};

//服务器返回兑换商品结果
struct LMsgS2CGiftExchange:public LMsgSC
{
	Lint m_errorId; //0：失败 1：成功
	Lint m_giftId;
	Lint m_CouponsCount;
	LMsgS2CGiftExchange():LMsgSC(MSG_S_2_C_GIFT_EXCHANGE),m_CouponsCount(0),m_errorId(0), m_giftId(0){}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorId), m_errorId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorId), m_errorId);
		WriteKeyValue(pack, NAME_TO_STR(m_giftId), m_giftId);
		WriteKeyValue(pack, NAME_TO_STR(m_CouponsCount), m_CouponsCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGiftExchange(); }
};

struct LMsgC2SGetTask :public LMsgSC
{
	Lint		m_taskId;	// 0xffff:all tasks, otherwise, the task specified by task id

	LMsgC2SGetTask() : LMsgSC(MSG_C_2_S_GET_TASK), m_taskId(-1) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGetTask; }
};

struct TaskBasicInfo
{
	Lint		m_id;				// 任务id
	Lstring		m_description;		// 任务描述
	Lint		m_curProgress;		// 完成量
	Lint		m_total;			// 任务总量
	Lint		m_recvedReward;		// 是否领取奖励,非0：没有领取，0：已经领取
	Lint		m_remainingTime;	// 距离下次任务开始的剩余时间

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_desc), m_description);
		WriteKeyValue(pack, NAME_TO_STR(m_cur), m_curProgress);
		WriteKeyValue(pack, NAME_TO_STR(m_total), m_total);
		WriteKeyValue(pack, NAME_TO_STR(m_recvedReward), m_recvedReward);
		WriteKeyValue(pack, NAME_TO_STR(m_remainingTime), m_remainingTime);
		return true;
	}
};

struct LMsgS2CTaskList : public LMsgSC
{
	std::vector<TaskBasicInfo>		m_tasks;

	LMsgS2CTaskList() : LMsgSC(MSG_S_2_C_TASK_LIST) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKey(pack, NAME_TO_STR(m_tasks));
		WriteArray(pack, m_tasks.size());
		for (Lsize i = 0; i < m_tasks.size(); i++)
			m_tasks[i].Write(pack);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CTaskList; }
};

struct LMsgC2SRecvTaskReward : public LMsgSC
{
	Lint		m_taskId;

	LMsgC2SRecvTaskReward() : LMsgSC(MSG_C_2_S_RECV_TASK_REWARD), m_taskId(-1) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_taskId), m_taskId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SRecvTaskReward; }
};

struct Reward
{
	Lint	m_type = 0;
	Lint	m_count = 0;
};

struct LMsgS2CRecvTaskReward : public LMsgSC
{
	Lint						m_taskId = 0;
	ErrorCode::ErrorCode		m_error = ErrorCode::ErrorNone;
	Reward						m_reward;

	LMsgS2CRecvTaskReward() : LMsgSC(MSG_S_2_C_RECV_TASK_REWARD){}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_reward));
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_reward.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_count), m_reward.m_count);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CRecvTaskReward;
	}
};

struct LMsgC2SGetUserInfo : public LMsgSC
{
	Lint		m_userId;

	LMsgC2SGetUserInfo() : LMsgSC(MSG_C_2_S_GET_USER_INFO), m_userId(-1) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGetUserInfo; }
};

struct UserBasicInfo
{
	Lint		m_id = 0;
	Lstring		m_unionId;
	Lstring		m_nickName;
	Lstring		m_headImgUrl;

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_unionId), m_unionId);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_headImgUrl), m_headImgUrl);
		return true;
	}
};

struct LMsgS2CUserInfo : public LMsgSC
{
	ErrorCode::ErrorCode			m_error = ErrorCode::ErrorNone;
	UserBasicInfo					m_user;

	LMsgS2CUserInfo() : LMsgSC(MSG_S_2_C_USER_INFO) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_user));
		m_user.Write(pack);

		return true;
	}

	virtual LMsg* Clone() {return new LMsgS2CUserInfo;}
};

struct LMsgC2SGetInvitingInfo : public LMsgSC
{
	LMsgC2SGetInvitingInfo() : LMsgSC(MSG_C_2_S_GET_INVITING_INFO) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SGetInvitingInfo; }
};

#define NUM_MAX_INVITEE		30

struct InviteeInfo
{
	UserBasicInfo		m_info;
	Lint				m_taskFinished;		// 被邀请者任务是否完成，0：没有完成，非0：完成

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_info.m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_unionId), m_info.m_unionId);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_info.m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_headImgUrl), m_info.m_headImgUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_taskFinished), m_taskFinished);
		return true;
	}
};

struct LMsgS2CInvitingInfo : public LMsgSC
{
	Reward						m_reward;
	UserBasicInfo				m_inviter;
	std::vector<InviteeInfo>	m_invitees;

	LMsgS2CInvitingInfo() : LMsgSC(MSG_S_2_C_INVITING_INFO) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);

		WriteKey(pack, NAME_TO_STR(m_reward));
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_reward.m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_count), m_reward.m_count);

		WriteKey(pack, NAME_TO_STR(m_inviter));
		m_inviter.Write(pack);

		WriteKey(pack, NAME_TO_STR(m_invitees));
		WriteArray(pack, m_invitees.size());

		for (size_t i = 0; i < m_invitees.size(); i++)
			m_invitees[i].Write(pack);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CInvitingInfo; }
};

struct LMsgC2SBindInviter : public LMsgSC
{
	Lint				m_inviterId;
	LMsgC2SBindInviter() : LMsgSC(MSG_C_2_S_BIND_INVITER), m_inviterId(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_inviterId), m_inviterId);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_inviterId), m_inviterId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SBindInviter; }
};

struct LMsgS2CBindInviter : public LMsgSC
{
	ErrorCode::ErrorCode			m_error = ErrorCode::ErrorNone;
	UserBasicInfo					m_inviter;

	LMsgS2CBindInviter() : LMsgSC(MSG_S_2_C_BIND_INVITER) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_error), (Lint)m_error);
		WriteKey(pack, NAME_TO_STR(m_inviter));
		m_inviter.Write(pack);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CBindInviter; }
};

struct LMsgC2SUserShared : public LMsgSC
{
	LMsgC2SUserShared() : LMsgSC(MSG_C_2_S_USER_SHARED) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SUserShared; }
};

struct LMsgC2SReqLuckyDrawNum : public LMsgSC
{
	LMsgC2SReqLuckyDrawNum() : LMsgSC(MSG_C_2_S_REQ_LUCKY_DRAW_NUM) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SReqLuckyDrawNum; }
};

struct LMsgS2CLuckyDrawNum : public LMsgSC
{
	Lint	m_drawNum;

	LMsgS2CLuckyDrawNum() : LMsgSC(MSG_S_2_C_LUCKY_DRAW_NUM), m_drawNum(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_drawNum), m_drawNum);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CLuckyDrawNum; }
};

//-----------------------------------------

//数据十一活动
struct LActiveSignCollect
{
	Lint	m_activeType;
	Lint	m_drawChanceCount;	//每天能抽几次
	Lint	m_date;
	std::vector<Lint>	m_words;	//1~8
	Lstring	m_nickName;	//
	Lint	m_id;
	MSGPACK_DEFINE(m_activeType, m_drawChanceCount, m_date, m_words, m_nickName, m_id); 
	LActiveSignCollect()
	{
		m_activeType = 0;
		m_drawChanceCount = 0;
		m_date = 0;
		m_id = 0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_activeType), m_activeType);
		ReadMapData(obj, NAME_TO_STR(m_drawChanceCount), m_drawChanceCount);
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_words), m_words);
		ReadMapData(obj, NAME_TO_STR(m_nickName), m_nickName);
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_activeType), m_activeType);
		WriteKeyValue(pack, NAME_TO_STR(m_drawChanceCount), m_drawChanceCount);
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_words), m_words);
		WriteKeyValue(pack, NAME_TO_STR(m_nickName), m_nickName);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		return true;
	}
};

//十一活动(签到）
struct LMsgC2SReqSignCollect:public LMsgSC
{

	Lint m_type;	//活动类型

	LMsgC2SReqSignCollect() :LMsgSC(MSG_C_2_S_REQ_SIGN_COLLECT), m_type(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SReqSignCollect(); }
};


struct LMsgS2CResSign:public LMsgSC
{
	Lint		m_errorCode = 0;			
	Lint		m_date = 0;			
	Lint		m_rewardCardCount = 0;		
	LMsgS2CResSign() :LMsgSC(MSG_S_2_C_RES_SIGN){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_rewardCardCount), m_rewardCardCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_rewardCardCount), m_rewardCardCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CResSign(); }
};

struct LMsgS2CResCollect:public LMsgSC
{
	Lint		m_errorCode = 0;
	Lint		m_date = 0;			
	Lint		m_word = 0;
	Lint		m_rewardCardCount = 0;
	Lint		m_chanceCount = 0;
	LMsgS2CResCollect() :LMsgSC(MSG_S_2_C_RES_COLLECT){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_errorCode), m_errorCode);
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_word), m_word);
		ReadMapData(obj, NAME_TO_STR(m_rewardCardCount), m_rewardCardCount);
		ReadMapData(obj, NAME_TO_STR(m_chanceCount), m_chanceCount);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 6);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_word), m_word);
		WriteKeyValue(pack, NAME_TO_STR(m_rewardCardCount), m_rewardCardCount);
		WriteKeyValue(pack, NAME_TO_STR(m_chanceCount), m_chanceCount);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CResCollect(); }
};


//----------------------状态-------
//-----------------------------------------
//十一活动(签到）;
struct LMsgC2SReqSignCollectWinState:public LMsgSC
{
	Lint m_type; 

	LMsgC2SReqSignCollectWinState() :LMsgSC(MSG_C_2_S_REQ_SIGN_COLLECT_WIN_STATE), m_type(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SReqSignCollectWinState(); }
};


struct LMsgS2CResSignState:public LMsgSC
{
	Lint		m_num = 0;			//活动ID;
	Lint		m_signState = 0;	//签到状态;
	std::vector<LActiveSignCollect>	m_DrawInfo;
	LMsgS2CResSignState() :LMsgSC(MSG_S_2_C_RES_SIGN_STATE){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_num), m_num);
		ReadMapData(obj, NAME_TO_STR(m_signState), m_signState);
		ReadMapData(obj, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_num), m_num);
		WriteKeyValue(pack, NAME_TO_STR(m_signState), m_signState);
		WriteKeyValue(pack, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CResSignState(); }
};

//-----------------------------------------
struct LMsgS2CResCollectState:public LMsgSC
{
	Lint		m_num;			//活动ID;
	std::vector<LActiveSignCollect>	m_DrawInfo;
	LMsgS2CResCollectState() :LMsgSC(MSG_S_2_C_RES_COLLECT_STATE), m_num(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_num), m_num);
		ReadMapData(obj, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_num), m_num);
		WriteKeyValue(pack, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CResCollectState(); }
};

//-----------------------------------------
struct LMsgS2CResWinState:public LMsgSC
{
	Lint		m_num;			//活动ID;
	std::vector<LActiveSignCollect>	m_DrawInfo;
	LMsgS2CResWinState() :LMsgSC(MSG_S_2_C_RES_WIN_STATE), m_num(0){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_num), m_num);
		ReadMapData(obj, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_num), m_num);
		WriteKeyValue(pack, NAME_TO_STR(m_DrawInfo), m_DrawInfo);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CResWinState(); }
};

struct LMsgS2CGoldChange:public LMsgSC
{
	Lint        m_changeNum = 0;      //变化的金币数;
	Lint		m_changeReason = 0;   //变化原因：0首次赠送金币,1:每日赠送金币;2,消耗金币(输),3，赢金币;
	Lint        m_giveTimes = 0;      //每日赠送次数;
	Lint		m_GoldNum = 0;        //变化后总金币数;
	LMsgS2CGoldChange() :LMsgSC(MSG_S_2_C_GOLD_CHANGE){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_changeNum), m_changeNum);
		ReadMapData(obj, NAME_TO_STR(m_changeReason), m_changeReason);
		ReadMapData(obj, NAME_TO_STR(m_giveTimes), m_giveTimes);
		ReadMapData(obj, NAME_TO_STR(m_GoldNum), m_GoldNum);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_changeNum), m_changeNum);
		WriteKeyValue(pack, NAME_TO_STR(m_changeReason), m_changeReason);
		WriteKeyValue(pack, NAME_TO_STR(m_giveTimes), m_giveTimes);
		WriteKeyValue(pack, NAME_TO_STR(m_GoldNum), m_GoldNum);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGoldChange(); }
};

struct LMsgC2SAskGoldDeskState:public LMsgSC
{
	Lint        m_eventType = 0;      //事件码;1:进入金币房;2:退出金币房; 3:进入托管; 4:退出托管;
	Lint		m_value = 0;          //事件值;（留用）
	Lint		m_pos = 0;            //事件触发玩家位置;

	LMsgC2SAskGoldDeskState() :LMsgSC(MSG_C_2_S_ASKGOLDDESK_STATE){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_eventType), m_eventType);
		ReadMapData(obj, NAME_TO_STR(m_value), m_value);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgC2SAskGoldDeskState(); }
};

struct LMsgS2CGoldDeskState:public LMsgSC
{
	Lint        m_eventType = 0;      //事件码;1:进入金币房;2:退出金币房; 3:进入托管; 4:退出托管;
	Lint		m_value = 0;          //事件值;（留用）
	Lint		m_pos = 0;            //事件触发玩家位置;
	LMsgS2CGoldDeskState() :LMsgSC(MSG_S_2_C_GOLDDESK_STATE){ }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_eventType), m_eventType);
		ReadMapData(obj, NAME_TO_STR(m_value), m_value);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_eventType), m_eventType);
		WriteKeyValue(pack, NAME_TO_STR(m_value), m_value);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgS2CGoldDeskState(); }
};

struct LMsgS2CRewardNotReceived : public LMsgSC
{
	LMsgS2CRewardNotReceived() : LMsgSC(MSG_S_2_C_REWARD_NOT_RECEIVED) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CRewardNotReceived; }
};

struct LMsgS2SKeepAlive : public LMsg
{
	LMsgS2SKeepAlive() : LMsg(MSG_S_2_S_KEEP_ALIVE) {}

	virtual bool Read(LBuff& buff)
	{
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2SKeepAlive();
	}
};

struct LMsgS2SKeepAliveAck : public LMsg
{
	LMsgS2SKeepAliveAck() : LMsg(MSG_S_2_S_KEEP_ALIVE_ACK) {}

	virtual bool Read(LBuff& buff)
	{
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2SKeepAliveAck();
	}
};

#define  DAILY_SHARE_CARD    99999
struct LMsgC2SRequest_Share_Card :public LMsgSC
{
	Lint		m_activeID;
	LMsgC2SRequest_Share_Card() :LMsgSC(MSG_C_2_S_REQUEST_SHARE_CARD), m_activeID(0) { }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_activeID), m_activeID);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SRequest_Share_Card(); }
};


struct LMsgS2CRet_Share_Card :public LMsgSC
{
	Lint		m_playerState;    //0表示玩家尚未分享获取卡片，1表示玩家已经分享获取房卡，2表示玩家分享获取房卡成功;
	Lint		m_date;           //服务器时间;
	Lint        m_nCardNum;

	LMsgS2CRet_Share_Card() :LMsgSC(MSG_S_2_C_FINISH_SHARE_CARD), m_playerState(0), m_date(0), m_nCardNum(0) { }

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_playerState), m_playerState);
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_playerState), m_playerState);
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_nCardNum), m_nCardNum);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CRet_Share_Card(); }
};


//add by arena   MsgID >= 200 && MsgID <= 300 Just for Arena;
//client quest for into arena;
struct LMsgC2SIntoArena :public LMsgSC
{
	Lint			  m_nArenaType;

	LMsgC2SIntoArena() :LMsgSC(MSG_C_2_S_INTO_ARENA), m_nArenaType(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_nArenaType), m_nArenaType);
		return true;
	}
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgC2SIntoArena(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回操作结果
struct LMsgS2CR4IntoArena :public LMsgSC
{
	Lint		m_errorCode; //错误类型描述;
	Lint		m_nArenaType;

	LMsgS2CR4IntoArena() :LMsgSC(MSG_S_2_C_R4_INTO_ARENA), m_errorCode(0), m_nArenaType(0){	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_nArenaType), m_nArenaType);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CR4IntoArena(); }
};

struct LMsgC2SQuitArena :public LMsgSC
{
	Lint			  m_nArenaType;

	LMsgC2SQuitArena() :LMsgSC(MSG_C_2_S_QUIT_ARENA), m_nArenaType(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_nArenaType), m_nArenaType);
		return true;
	}
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgC2SQuitArena(); }
};

//////////////////////////////////////////////////////////////////////////
//服务器返回操作结果
struct LMsgS2CR4QuitArena :public LMsgSC
{
	Lint		m_errorCode; //错误类型描述;
	Lint		m_nArenaType;

	LMsgS2CR4QuitArena() :LMsgSC(MSG_S_2_C_R4_QUIT_ARENA), m_errorCode(0), m_nArenaType(0) {	}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_nArenaType), m_nArenaType);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CR4QuitArena(); }
};
//add end 


// 向工会创建者发送房卡不足的警报
#define MSG_TEAM_CARD_ALERT	0x7FFFFFFC
struct LMsgTeamCardAlert :public LMsg
{
	Lint nCreaterID = 0;
	Lint nTeamID = 0;
	Lstring strTeamName = "";

	LMsgTeamCardAlert() :LMsg(MSG_TEAM_CARD_ALERT) {}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(nCreaterID);
		buff.Write(nTeamID);
		buff.Write(strTeamName);
		return true;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(nCreaterID);
		buff.Read(nTeamID);
		buff.Read(strTeamName);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgTeamCardAlert();
	}
};


//服务器返回某个玩家分数，卡金币变化
//MSG_S_2_C_DESK_USER_SCORE_CHARGE = 33, //桌子上玩家分数变化，充值
struct LMsgS2CDeskUserScoreCharge :public LMsgSC
{
	Lint			m_userId;
	Lint			m_pos;	//位置，0-3 分别代表东，南，西，北
	Ldouble			m_score;//玩家积分
	Lint		    m_oper; //操作类型

	LMsgS2CDeskUserScoreCharge() :LMsgSC(MSG_S_2_C_DESK_USER_SCORE_CHARGE), m_userId(0), m_pos(0), m_score(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_userId), m_userId);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		ReadMapData(obj, NAME_TO_STR(m_score), m_score);
		ReadMapData(obj, NAME_TO_STR(m_oper), m_oper);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_score), m_score);
		WriteKeyValue(pack, NAME_TO_STR(m_oper), m_oper);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CDeskUserScoreCharge(); }
};

//广播通知客户端钱不够 请等待
//MSG_S_2_C_BROCAST_USER_NOGOLD = 34, //通知玩家充值
struct LMsgS2CBrocastUserNoGold :public LMsgSC
{
	Lint					m_time;//倒计时 秒数
	Lint					m_flag;//0-等待我充值，1-等待别人充值 2-正在离开，等待充值时间已经到
	std::vector<Lstring>	m_wait;//等待操作的玩家 需要充值的玩家


	LMsgS2CBrocastUserNoGold() :LMsgSC(MSG_S_2_C_BROCAST_USER_NOGOLD), m_time(0), m_flag(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_time), m_time);
		ReadMapData(obj, NAME_TO_STR(m_flag), m_flag);
		ReadMapData(obj, NAME_TO_STR(m_wait), m_wait);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_time), m_time);
		WriteKeyValue(pack, NAME_TO_STR(m_flag), m_flag);

		WriteKey(pack, "m_wait");
		WriteKey(pack, m_wait);

		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CBrocastUserNoGold(); }
};

////////////////////长沙麻将 begin///////////////////////////////////

//玩家飘思考
struct LMsgS2CPiaoSelect :public LMsgSC
{
	LMsgS2CPiaoSelect() :LMsgSC(MSG_S_2_C_Piao_Select) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CPiaoSelect(); }
};

//玩家飘思考
struct LMsgC2SPiaoSelect :public LMsgSC
{
	Lint m_position;
	Lint m_piao_count;
	LMsgC2SPiaoSelect() :LMsgSC(MSG_C_2_S_Piao_Select), m_position(0), m_piao_count(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_piao_count), m_piao_count);
		ReadMapData(obj, NAME_TO_STR(m_position), m_position);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SPiaoSelect(); }
};


//玩家飘思考
struct LMsgS2CPiaoCount :public LMsgSC
{
	std::vector<Lint> m_piao;
	LMsgS2CPiaoCount() :LMsgSC(MSG_S_2_C_Piao_Count) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_piao), m_piao);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CPiaoCount(); }
};



// 托管状态产生和取消托管都会发这个消息
//MSG_S_2_C_ATTACK_MANAGED = 187;

struct LMsgS2CAttachManaged : public LMsgSC
{
	Lint m_managed; //1 为托管状态， 0为不托管状态    
	LMsgS2CAttachManaged() : LMsgSC(MSG_S_2_C_ATTACK_MANAGED), m_managed(0) { }
	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_managed), m_managed);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_managed), m_managed);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgS2CAttachManaged();
	}
};


// 客户端取消托管状态
//MSG_C_2_S_CANCEL_MANAGED = 188;

struct LMsgC2SCancelManaged : public LMsgSC
{
	LMsgC2SCancelManaged() : LMsgSC(MSG_C_2_S_CANCEL_MANAGED) {}
	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgC2SCancelManaged();
	}
};

struct gang_think {
	CardValue m_gang_card;//杠出的牌
	std::vector<ThinkData> m_think;//思考
	MSGPACK_DEFINE(m_gang_card, m_think);
};

//////////////////////////////////////////////////////////////////////////
//玩家杠牌思考
struct LMsgS2CUserGangThinkCard :public LMsgSC
{
	Lint		m_pos;
	CardValue	m_gang;
	std::vector<gang_think> m_card;

	LMsgS2CUserGangThinkCard() :LMsgSC(MSG_S_2_C_GANG_THINK), m_pos(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_gang), m_gang);
		WriteKeyValue(pack, NAME_TO_STR(m_card), m_card);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUserGangThinkCard(); }
};



//竞技的提示
//MSG_S_2_C_SPORT_TIPS = 176,

//例子说明:
//如果m_id 为0表示通知，数据变化；客户端可以以m_type 作为类型替换数值
//预选赛被淘汰：m_type 为0 m_args1 为名次
//预选赛对手匹配：m_type为1 m_args1当前积分 m_args2当前排名 m_args3胜率 m_args4前50名积分
//预选赛最后结束等待：m_type为2 m_args1晋级名次 m_args2剩余桌子数
//晋级成功提示：m_type为3 m_args1比赛排名 m_args2积分 
//晋级失败提示：m_type为4 m_args1比赛排名 m_args2积分 m_args3胜率 m_args4即将进入的阶段 
//晋级赛等待结果已经晋级提示：m_type为5 m_args1本桌排名 m_args2等待剩余桌数  
//晋级赛等待结果未定晋级提示：m_type为6 m_args1本桌排名 m_args2等待剩余桌数 
//晋级比赛等待结束：m_type为7 m_args1剩余桌子数 
//晋级比赛结束：m_type为8 m_args1名次 m_args2奖励id1 m_args3奖励id2 m_args4奖励id3 m_args5奖励id4

struct LMsgS2CSportTips : public LMsgSC
{
	Lint	m_id;    //提示内容id，0表示通知
	Lint	m_type;  //提示类型，0预选被淘汰，1预选赛结束；2比赛结束，等待其他桌子结束，
	Lint    m_sportId; //比赛场id
	Lint    m_args1; //预定义参数 如果：type为1，剩余的桌子数
	Lint    m_args2; //预定义参数	
	Lint    m_args3; //预定义参数
	Lint    m_args4; //预定义参数
	Lint    m_args5; //预定义参数

	LMsgS2CSportTips() : LMsgSC(MSG_S_2_C_SPORT_TIPS) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_id), m_id);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_sportId), m_sportId);
		ReadMapData(obj, NAME_TO_STR(m_args1), m_args1);
		ReadMapData(obj, NAME_TO_STR(m_args2), m_args2);
		ReadMapData(obj, NAME_TO_STR(m_args3), m_args3);
		ReadMapData(obj, NAME_TO_STR(m_args4), m_args4);
		ReadMapData(obj, NAME_TO_STR(m_args5), m_args5);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 9);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_sportId), m_sportId);
		WriteKeyValue(pack, NAME_TO_STR(m_args1), m_args1);
		WriteKeyValue(pack, NAME_TO_STR(m_args2), m_args2);
		WriteKeyValue(pack, NAME_TO_STR(m_args3), m_args3);
		WriteKeyValue(pack, NAME_TO_STR(m_args4), m_args4);
		WriteKeyValue(pack, NAME_TO_STR(m_args5), m_args5);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CSportTips; }
};


//服务器广播玩家扎鸟信息
struct LMsgS2CZhaBirdShow :public LMsgSC
{
	Lint    m_type;  //0:扎鸟 1:飞鸟
	Lint	m_end_pos;
	std::vector<BirdInfo>	m_bird_infos;	//鸟牌

	LMsgS2CZhaBirdShow() :LMsgSC(MSG_S_2_C_ZHA_BIRD), m_end_pos(0), m_type(0) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_end_pos), m_end_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_bird_infos), m_bird_infos);


		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CZhaBirdShow(); }
};


//服务器广播王霸牌信息
struct LMsgS2CWangBa :public LMsgSC
{
	CardValue m_Wang_Ba;	//王霸牌

	bool m_ShangWang;
	bool m_XiaWang;

	LMsgS2CWangBa() :LMsgSC(MSG_S_2_C_Wang_Ba), m_ShangWang(false), m_XiaWang(false) {}

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_ShangWang), m_ShangWang);
		WriteKeyValue(pack, NAME_TO_STR(m_XiaWang), m_XiaWang);
		m_Wang_Ba.Write(pack);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CWangBa(); }
};



////////////////////长沙麻将 end///////////////////////////////////

///////////////////广东麻将////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
struct ChangeCard_Ting
{
	int outCardId;
	//	U64 u64TingCards;
	Luint  High;
	Luint  Low;
	MSGPACK_DEFINE(outCardId, High, Low);
	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(out), outCardId);
		WriteKeyValue(pack, NAME_TO_STR(High), High);
		WriteKeyValue(pack, NAME_TO_STR(Low), Low);
		return true;
	}

	ChangeCard_Ting(int out, Luint  H, Luint  L) { outCardId = out, High = H; Low = L; }
};
typedef std::vector<ChangeCard_Ting> Ting_Result;
//////////////////////////////////////////////////////////////////////////


enum InteractionType
{
	SENDFLOWERS = 0,	//送鲜花
	THROWEGGS = 1,		//扔鸡蛋
};

//客户端发送玩家交互请求
struct LMsgC2SInteraction :public LMsgSC
{
	Lint		m_sendPos;		//发起者位置
	Lint		m_targetPos;	//目标位置
	Lint		m_interactionType;	//道具类型

	LMsgC2SInteraction() :LMsgSC(MSG_C_2_S_INTERACTION), m_sendPos(0), m_targetPos(0), m_interactionType(SENDFLOWERS) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_sendPos), m_sendPos);
		ReadMapData(obj, NAME_TO_STR(m_targetPos), m_targetPos);
		ReadMapData(obj, NAME_TO_STR(m_interactionType), m_interactionType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_sendPos), m_sendPos);
		WriteKeyValue(pack, NAME_TO_STR(m_targetPos), m_targetPos);
		WriteKeyValue(pack, NAME_TO_STR(m_interactionType), m_interactionType);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SInteraction(); }
};

//服务器转发玩家交互请求
struct LMsgS2CInteraction :public LMsgSC
{
	Lint		m_sendPos;		//发起者位置
	Lint		m_targetPos;	//目标位置
	Lint		m_interactionType;	//道具类型

	LMsgS2CInteraction() :LMsgSC(MSG_S_2_C_INTERACTION), m_sendPos(0), m_targetPos(0), m_interactionType(SENDFLOWERS) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_sendPos), m_sendPos);
		ReadMapData(obj, NAME_TO_STR(m_targetPos), m_targetPos);
		ReadMapData(obj, NAME_TO_STR(m_interactionType), m_interactionType);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 4);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_sendPos), m_sendPos);
		WriteKeyValue(pack, NAME_TO_STR(m_targetPos), m_targetPos);
		WriteKeyValue(pack, NAME_TO_STR(m_interactionType), m_interactionType);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CInteraction(); }
};


enum REQ_TUOGUAN_TYPE
{
	JOIN_TUOGUAN,
	QUIT_TUOGUAN,
};

struct LMsgC2S_TuoGuan : public LMsgSC
{
	Lint			m_ReqType = JOIN_TUOGUAN;// 请求托管
	LMsgC2S_TuoGuan() :LMsgSC(MSG_C_2_S_TUOGUAN) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_ReqType);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgC2S_TuoGuan(); }
};

enum TUOGUAN_STATUS
{
	NOT_IN_TUOGUAN,
	IN_TUOGUAN,
};

struct LMsgS2C_TuoGuanInfo : public LMsgSC
{
	Lint			m_nPos = -1;
	int				m_nType = NOT_IN_TUOGUAN;//当前托管状态 
	LMsgS2C_TuoGuanInfo() :LMsgSC(MSG_S_2_C_TUOGUAN_INFO) {}
	MSGPACK_DEFINE_MAP(m_msgId, m_nType, m_nPos);
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this);	return true; }
	virtual bool Read(msgpack::object& obj) { obj.convert(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2C_TuoGuanInfo(); }
};


struct LMsgC2SSignInInfoReq :public LMsgSC
{

	LMsgC2SSignInInfoReq() :LMsgSC(MSG_C_2_S_SIGNIN_INFO_REQ) { }

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SSignInInfoReq(); }
};

struct LSignInReqInfo
{
	Lint	m_date;
	Lint	m_type; /* 0 signin 1:fill signin */
	MSGPACK_DEFINE(m_date, m_type);
	LSignInReqInfo()
	{
		m_date = 0;
		m_type = 0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		return true;
	}
};

struct LSignInInfo
{
	Lint	m_date;
	Lint	m_state;
	MSGPACK_DEFINE(m_date, m_state);
	LSignInInfo()
	{
		m_date = 0;
		m_state = 0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_date), m_date);
		ReadMapData(obj, NAME_TO_STR(m_state), m_state);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_date), m_date);
		WriteKeyValue(pack, NAME_TO_STR(m_state), m_state);
		return true;
	}
};

struct LMsgS2CSignInInfoRes :public LMsgSC
{
	std::vector<LSignInInfo>	m_SignInInfo;
	LMsgS2CSignInInfoRes() :LMsgSC(MSG_S_2_C_SIGNIN_INFO_RES) { }

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_SignInInfo), m_SignInInfo);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CSignInInfoRes(); }
};

struct LMsgC2SSignInReq :public LMsgSC
{
	std::vector<LSignInReqInfo>	m_vSignInReqInfos;
	LMsgC2SSignInReq() :LMsgSC(MSG_C_2_S_SIGNIN_REQ) { }

	virtual bool Read(msgpack::object& obj)
	{
		msgpack::object array;
		ReadMapData(obj, NAME_TO_STR(m_vdates), array);
		if (array.type == msgpack::v1::type::ARRAY)
		{
			Lint m_count = array.via.array.size;
			for (Lsize i = 0; i < array.via.array.size; ++i)
			{
				LSignInReqInfo v;
				msgpack::object& obj = *(array.via.array.ptr + i);
				obj.convert(v);
				m_vSignInReqInfos.push_back(v);
			}
		}
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_vSignInReqInfos), m_vSignInReqInfos);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgC2SSignInReq(); }
};

struct LMsgS2CSignInRes :public LMsgSC
{
	Lint		m_errorCode;
	std::vector<Lint>		m_vdates;
	std::vector<Lint>		m_vrewardIDs;
	Lint		m_cost;
	LMsgS2CSignInRes() :LMsgSC(MSG_S_2_C_SIGNIN_RES), m_errorCode(0), m_cost(0) { }

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_errorCode), m_errorCode);
		WriteKeyValue(pack, NAME_TO_STR(m_vdates), m_vdates);
		WriteKeyValue(pack, NAME_TO_STR(m_vrewardIDs), m_vrewardIDs);
		WriteKeyValue(pack, NAME_TO_STR(m_cost), m_cost);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CSignInRes(); }
};

//服务器广播鬼牌
struct LMsgS2CGhostCard :public LMsgSC
{
	CardValue	m_ghostCard;//鬼牌
	LMsgS2CGhostCard() :LMsgSC(MSG_S_2_C_GHOST_CARD) {}

	virtual bool Read(msgpack::object& obj)
	{
		m_ghostCard.Read(obj);;
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		m_ghostCard.Write(pack);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CGhostCard(); }
};


struct LMsgS2CGengZhuangSucc :public LMsgSC
{

	LMsgS2CGengZhuangSucc() :LMsgSC(MSG_S_2_C_GENGZHUANG_SUCC) { }

	virtual bool Read(msgpack::object& obj)
	{
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 1);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CGengZhuangSucc(); }
};


//////////////////广东麻将 end////////////////////////////////////

//服务广播听的牌
//MSG_S_2_C_USER_OPER_TING = 63,//玩家听牌
struct LMsgS2CUserOperTing :public LMsgSC
{
	Lint		m_type; //0-明听，1-暗听
	Lint		m_pos;  //pos=我自己的时候，需要根据类型响应不同的操作
	CardValue	m_card; //所听的牌

	LMsgS2CUserOperTing() :LMsgSC(MSG_S_2_C_USER_OPER_TING), m_type(0), m_pos(-1) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_type), m_type);
		ReadMapData(obj, NAME_TO_STR(m_pos), m_pos);
		m_card.Read(obj);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 5);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		m_card.Write(pack);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CUserOperTing(); }
};

//告诉客户端听后能打出那些牌
//MSG_S_2_C_CANTING = 166,	//服务器告诉客户端可以听牌 并发送打出哪张牌可以听哪些牌
struct LMsgS2CCanTing : public LMsgSC
{
	std::vector<Lint> m_tingCards;//数值是，(m_color * 10 + m_number);
	LMsgS2CCanTing() : LMsgSC(MSG_S_2_C_CANTING) {}

	virtual bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_tingCards), m_tingCards);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 2);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_tingCards), m_tingCards);
		return true;
	}

	virtual LMsg* Clone() { return new LMsgS2CCanTing; }
};


#endif