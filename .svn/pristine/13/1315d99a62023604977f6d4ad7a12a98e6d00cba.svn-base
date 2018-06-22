#ifndef _LMSG_S2S_H_
#define _LMSG_S2S_H_

#include "LMsg.h"
#include "LUser.h"
#include "LCharge.h"
#include "LTool.h"
#include "TCPClient.h"
#include "LMsgCT.h"

const Lint g_nGateNum_x = 60;
const Lint g_nLogicNum_x = 60;

//////////////////////////////////////////////////////////////////////////
//logic 跟 center ;

//////////////////////////////////////////////////////////////////////////
struct LMsgL2CeLogin:public LMsg
{
	Lint	m_ID;//类型;
	Lstring m_key;
	Lstring m_ServerName;
	Lint	m_needUserIdInfo;

	LMsgL2CeLogin() :LMsg(MSG_L_2_CE_LOGIN), m_ID(0), m_needUserIdInfo(1)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_key);
		buff.Read(m_ServerName);
		buff.Read(m_needUserIdInfo);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_key);
		buff.Write(m_ServerName);
		buff.Write(m_needUserIdInfo);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeLogin();
	}
};


//////////////////////////////////////////////////////////////////////////

struct LogicInfo
{
	Lint	    	m_id = 0;
	Lint		    m_deskCount = 0;
	Lstring	    	m_ip;
	Lshort	    	m_port = 0;
	LSocketPtr   	m_sp;
	TCPClientPtr	m_client;
	Lint	    	m_flag = 0;	//0,normal; -1,failover
};

struct LMsgLMG2GateLogicInfo :public LMsg
{
	Lint		m_ID = 0;
	Lint		m_count = 0;
	LogicInfo	m_logic[g_nLogicNum_x];

	LMsgLMG2GateLogicInfo() :LMsg(MSG_LMG_2_G_SYNC_LOGIC)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < g_nLogicNum_x; ++i)
		{
			buff.Read(m_logic[i].m_id);
			buff.Read(m_logic[i].m_deskCount);
			buff.Read(m_logic[i].m_ip);
			buff.Read(m_logic[i].m_port);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < g_nLogicNum_x; ++i)
		{
			buff.Write(m_logic[i].m_id);
			buff.Write(m_logic[i].m_deskCount);
			buff.Write(m_logic[i].m_ip);
			buff.Write(m_logic[i].m_port);
		} 
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GateLogicInfo();
	}
};

struct GateInfo
{
	Lint		m_id = 0;
	Lint		m_userCount = 0;
	Lstring		m_ip;
	Lshort		m_port = 0;
	Lstring		m_ip2;
	Lshort		m_port2 = 0;
	LSocketPtr	m_sp;
};

struct LMsgL2CeGateInfo :public LMsg
{
	Lint		m_ID = 0;
	Lint		m_count = 0;
	GateInfo	m_gate[g_nGateNum_x];

	LMsgL2CeGateInfo() :LMsg(MSG_L_2_CE_GATE_INFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_count);
		for (Lint i = 0; i < m_count && i < g_nGateNum_x; ++i)
		{
			buff.Read(m_gate[i].m_id);
			buff.Read(m_gate[i].m_userCount);
			buff.Read(m_gate[i].m_ip);
			buff.Read(m_gate[i].m_port);
			buff.Read(m_gate[i].m_ip2);
			buff.Read(m_gate[i].m_port2);
		}
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < g_nGateNum_x; ++i)
		{
			buff.Write(m_gate[i].m_id);
			buff.Write(m_gate[i].m_userCount);
			buff.Write(m_gate[i].m_ip);
			buff.Write(m_gate[i].m_port);
			buff.Write(m_gate[i].m_ip2);
			buff.Write(m_gate[i].m_port2);
		} 
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeGateInfo();
	}
};


//////////////////////////////////////////////////////////////////////////
struct LMsgCe2LUserLogin:public LMsg
{
	Lint		m_seed;
	LUser		user;

	LMsgCe2LUserLogin() :LMsg(MSG_CE_2_L_USER_LOGIN), m_seed(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_seed);
		user.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_seed);
		user.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUserLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//center发送玩家id信息到;

//玩家基本信息;
struct UserIdInfo
{
	Lint		m_id;
	Lstring		m_unionId;
	Lstring		m_nike;
	Lstring		m_headImageUrl;
	Lint		m_sex;
};

struct LMsgCe2LUserIdInfo:public LMsg
{
	Lint	m_count;
	std::vector<UserIdInfo> m_info;
	
	LMsgCe2LUserIdInfo() :LMsg(MSG_CE_2_L_USER_ID_INFO), m_count(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			UserIdInfo info;
			buff.Read(info.m_id);
			buff.Read(info.m_unionId);
			buff.Read(info.m_nike);
			buff.Read(info.m_sex);
			buff.Read(info.m_headImageUrl);
			m_info.push_back(info);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_count = m_info.size();
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			buff.Write(m_info[i].m_id);
			buff.Write(m_info[i].m_unionId);
			buff.Write(m_info[i].m_nike);
			buff.Write(m_info[i].m_sex);
			buff.Write(m_info[i].m_headImageUrl);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUserIdInfo();
	}

	virtual Lint preAllocSendMemorySize() //重新分配64K,由于消息长度是short型
	{
		return 1024 * 64;
	}
};


//logic 发送保存 玩家遊戲局數;
struct LMsgLMG2CeSaveUserPlayCount:public LMsg
{
	Lstring	m_unionid;//类型;
	Lint	m_playCount;

	LMsgLMG2CeSaveUserPlayCount() :LMsg(MSG_LMG_2_CE_User_Save_PlayCount), m_playCount(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_unionid);
		buff.Read(m_playCount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_unionid);
		buff.Write(m_playCount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CeSaveUserPlayCount();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送保存 玩家信息;
struct LMsgL2CeSaveUser:public LMsg
{
	Lint	m_ID = 0;//类型;
	Lint	m_type = 0;
	LUser	m_user;

	LMsgL2CeSaveUser() :LMsg(MSG_L_2_CE_USER_SAVE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_type);
		m_user.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_type);
		m_user.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeSaveUser();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送保存 桌子信息;
struct LMsgL2CeSaveLogItem:public LMsg
{
	Lint		m_type;//类型,0-插入,1-更新;
	Lstring		m_sql;

	LMsgL2CeSaveLogItem() :LMsg(MSG_L_2_CE_ROOM_SAVE), m_type(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeSaveLogItem();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送保存 玩家房卡信息;
struct LMsgL2CeSaveCardInfo:public LMsg
{
	LCharge	m_charge;

	LMsgL2CeSaveCardInfo() :LMsg(MSG_L_2_CE_CARD_SAVE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		m_charge.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_charge.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeSaveCardInfo();
	}
};

struct LMsgLMG2CeCouponChanged : public LMsg
{
	Lstring		m_unionId;
	Lint		m_count = 0;
	Lint		m_changedReason = 0;

	LMsgLMG2CeCouponChanged() : LMsg(MSG_LMG_2_CE_COUPON_CHANGED) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_unionId);
		buff.Read(m_count);
		buff.Read(m_changedReason);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_unionId);
		buff.Write(m_count);
		buff.Write(m_changedReason);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CeCouponChanged;
	}
};

struct LMsgLMG2CeOfflineUserCardChanged : public LMsg
{
	Lstring		m_unionId;
	Ldouble		m_count = 0.0f;
	Lint		m_cardType = 0;
	Lint		m_operType = 0;
	Lstring		m_admin;
	Lint        m_teamId = 0;

	LMsgLMG2CeOfflineUserCardChanged() : LMsg(MSG_LMG_2_CE_OFFLINE_USER_CARD_CHANGED) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_unionId);
		buff.Read(m_count);
		buff.Read(m_cardType);
		buff.Read(m_operType);
		buff.Read(m_admin);
		buff.Read(m_teamId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_unionId);
		buff.Write(m_count);
		buff.Write(m_cardType);
		buff.Write(m_operType);
		buff.Write(m_admin);
		buff.Write(m_teamId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CeOfflineUserCardChanged;
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 通知玩家登陆服务器;
struct LMsgL2CeUserServerLogin :public LMsg
{
	Lint		m_serverID;//类型,0-插入,1-更新;
	Lstring		m_openID;
	Lint		m_useTeamId;

	LMsgL2CeUserServerLogin() :LMsg(MSG_L_2_CE_USER_LOGIN), m_serverID(0), m_useTeamId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_openID);
		buff.Read(m_useTeamId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_openID);
		buff.Write(m_useTeamId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUserServerLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 通知玩家登出服务器;
struct LMsgL2CeUserServerLogout:public LMsg
{
	Lint		m_serverID;//类型,0-插入,1-更新;
	Lstring		m_openID;

	LMsgL2CeUserServerLogout() :LMsg(MSG_L_2_CE_USER_LOGOUT), m_serverID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_openID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_openID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUserServerLogout();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送玩家充值;
struct LMsgCe2LGMCharge:public LMsg
{
	Lint		m_userid = 0;
	Lint		m_cardType = 0;
	Ldouble		m_cardCount = 0;
	Lint		m_oper = 0;
	Lstring		m_admin;

	LMsgCe2LGMCharge() :LMsg(MSG_CE_2_L_GM_CHARGE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_cardType);
		buff.Read(m_cardCount);
		buff.Read(m_oper);
		buff.Read(m_admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_cardType);
		buff.Write(m_cardCount);
		buff.Write(m_oper);
		buff.Write(m_admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMCharge();
	}
};


//////////////////////////////////////////////////////////////////////////
//center 发送gm设置跑马灯;
struct LMsgCe2LGMHorse :public LMsg
{
	Lstring		m_str;
	
	LMsgCe2LGMHorse() :LMsg(MSG_CE_2_L_GM_HORSE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMHorse();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送gm够买提示;
struct LMsgCe2LGMBuyInfo:public LMsg
{
	Lstring		m_str;

	LMsgCe2LGMBuyInfo() :LMsg(MSG_CE_2_L_GM_BUYINFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMBuyInfo();
	}
};

//////////////////////////////////////////////////////////////////////////
//center 发送gm够买提示;
struct LMsgCe2LGMHide:public LMsg
{
	Lint		m_hide;

	LMsgCe2LGMHide() :LMsg(MSG_CE_2_L_GM_HIDE), m_hide(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_hide);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_hide);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMHide();
	}
};

struct LMsgCe2LGMModifyPlayerType : public LMsg
{
	Lint	m_userId;
	Lstring m_playerType;

	LMsgCe2LGMModifyPlayerType() :LMsg(MSG_CE_2_LMG_GM_MODIFY_PLAYERTYPE), m_userId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_playerType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_playerType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMModifyPlayerType();
	}
};

struct LMsgCe2LGMChargeDiamond : public LMsg
{
	Lint		m_userId = 0;
	Lint		m_error = 0;			// 0-succeed, 1-failed;
	Lint		m_diamondCharged = 0;

	LMsgCe2LGMChargeDiamond() : LMsg(MSG_CE_2_LMG_GM_CHARGE_DIAMOND)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_error);
		buff.Read(m_diamondCharged);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_error);
		buff.Write(m_diamondCharged);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMChargeDiamond();
	}
};

struct LMsgCe2LGMPurchase : public LMsg
{
	Lint		m_userId;
	Lint		m_error;			// 0-succeed, 1-failed;
	Lint		m_itemId;
	Lint		m_diamondPaid;
	Ldouble		m_cardAdded;
	Lint		m_cardType;
	Lint		m_operType;

	LMsgCe2LGMPurchase() : LMsg(MSG_CE_2_LMG_GM_PURCHASE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_error);
		buff.Read(m_itemId);
		buff.Read(m_diamondPaid);
		buff.Read(m_cardAdded);
		buff.Read(m_cardType);
		buff.Read(m_operType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_error);
		buff.Write(m_itemId);
		buff.Write(m_diamondPaid);
		buff.Write(m_cardAdded);
		buff.Write(m_cardType);
		buff.Write(m_operType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LGMPurchase();
	}
};

struct LMsgLMG2CeCardExchange : public LMsg
{
	Lstring		m_unionId;
	Lint		m_oper;
	Lint		m_exchangeType; //0减 1加
	Lint		m_cardType;
	Ldouble		m_cardNum;

	LMsgLMG2CeCardExchange() : LMsg(MSG_LMG_2_CE_CardExchange),m_oper(0),m_exchangeType(0),m_cardType(2),m_cardNum(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_unionId);
		buff.Read(m_oper);
		buff.Read(m_exchangeType);
		buff.Read(m_cardType);
		buff.Read(m_cardNum);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_unionId);
		buff.Write(m_oper);
		buff.Write(m_exchangeType);
		buff.Write(m_cardType);
		buff.Write(m_cardNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CeCardExchange();
	}
};

//保存金币相关操作;
struct LMsgLMG2CeGoldChange : public LMsg
{
	Lstring		m_unionId;
	Lint		m_oper;   //0首次赠送金币,1:每日赠送金币;2,消耗金币(输),3，赢金币;
	Lint		m_GoldNum;
	Lint        m_lastTime;
	Lint        m_RemainingGifts;

	LMsgLMG2CeGoldChange() : LMsg(MSG_LMG_2_CE_GOLDCHANGE),m_oper(0),m_GoldNum(0), m_lastTime(0), m_RemainingGifts(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_unionId);
		buff.Read(m_oper);
		buff.Read(m_GoldNum);
		buff.Read(m_lastTime);
		buff.Read(m_RemainingGifts);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_unionId);
		buff.Write(m_oper);
		buff.Write(m_GoldNum);
		buff.Write(m_lastTime);
		buff.Write(m_RemainingGifts);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CeGoldChange();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 跟 gate 之间的消息交互;
//////////////////////////////////////////////////////////////////////////

struct LMsgG2LLogin :public LMsg
{
	Lint		m_id = 0;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port = 0;

	LMsgG2LLogin() :LMsg(MSG_G_2_L_LOGIN)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LLogin();
	}
};

struct LMsgG2LMGLogin :public LMsg
{
	Lint		m_id = 0;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port = 0;
	Lstring		m_ip2;
	Lshort		m_port2 = 0;

	LMsgG2LMGLogin() :LMsg(MSG_G_2_LMG_LOGIN)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_ip2);
		buff.Read(m_port2);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_ip2);
		buff.Write(m_port2);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LMGLogin();
	}
};

struct LMsgL2LMGLogin :public LMsg
{
	Lint		m_id = 0;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port = 0;
	Lint		m_firstLogin = 0;

	LMsgL2LMGLogin() :LMsg(MSG_L_2_LMG_LOGIN)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_firstLogin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_firstLogin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGLogin();
	}
};


//////////////////////////////////////////////////////////////////////////
//gate 发送玩家消息 到 logic;
struct LMsgG2LUserMsg :public LMsg
{
	Lint			m_userGateId;
	Lint			m_userMsgId;
	Lstring			m_ip;
	LBuffPtr		m_dataBuff;
	LMsg*			m_userMsg;

	LMsgG2LUserMsg() :LMsg(MSG_G_2_L_USER_MSG), m_userGateId(0), m_userMsgId(0), m_userMsg(NULL)
	{
	}

	virtual~LMsgG2LUserMsg()
	{
		if (m_userMsg)
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userGateId);
		buff.Read(m_userMsgId);
		buff.Read(m_ip);

		int msgid = MSG_ERROR_MSG;

		try{
			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
			msgpack::object obj = unpack.get();

			ReadMapData(obj, "m_msgId", msgid);
			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("MSG_G_2_L_USER_MSG::RecvMsgPack error");
		}
		
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userGateId);
		buff.Write(m_userMsgId);
		buff.Write(m_ip);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发现玩家消息到 gate;
struct LMsgL2GUserMsg : public LMsg
{
	Lint			m_id;
	LBuffPtr		m_dataBuff;
	
	LMsgL2GUserMsg() :LMsg(MSG_L_2_G_USER_MSG), m_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//gate 发送玩家掉线消息 到 logic;
struct LMsgG2LUserOutMsg :public LMsg
{
	Lint			m_userGateId;
	
	LMsgG2LUserOutMsg():LMsg(MSG_G_2_L_USER_OUT_MSG), m_userGateId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userGateId);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userGateId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LUserOutMsg();
	}
};


//////////////////////////////////////////////////////////////////////////
//logic 发送玩家消息到 gate;
struct LMsgL2GUserOutMsg : public LMsg
{
	Lint			m_id;

	LMsgL2GUserOutMsg():LMsg(MSG_L_2_G_USER_OUT_MSG), m_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GUserOutMsg();
	}
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//logicmanager 跟 logicdb 之间的消息交互;
//logic 登录 logicdb;
struct LMsgLMG2LdbLogin : public LMsg
{
	Lstring			m_key;
	Lint			m_serverID;
	LMsgLMG2LdbLogin():LMsg(MSG_LMG_2_LDB_LOGIN), m_serverID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LdbLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 登录 logicdb;
struct LMsgL2LdbLogin : public LMsg
{
	Lstring			m_key;
	Lint			m_serverID;
	LMsgL2LdbLogin():LMsg(MSG_L_2_LDB_LOGIN), m_serverID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_key);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_key);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LdbLogin();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送保存 桌子信息;
struct LMsgL2LDBSaveLogItem:public LMsg
{
	Lint		m_type = 0;//类型,0-插入,1-更新;
	Lstring		m_sql;
	Lint		m_serverID = 0;

	LMsgL2LDBSaveLogItem() :LMsg(MSG_L_2_LDB_ROOM_SAVE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveLogItem();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送保存录像
struct LMsgL2LDBSaveVideo :public LMsg
{
	Lint		m_type = 0;//类型,0-插入,1-更新;
	Lstring		m_sql;
	Lint		m_serverID = 0;

	LMsgL2LDBSaveVideo() :LMsg(MSG_L_2_LDB_VIDEO_SAVE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_type);
		buff.Read(m_sql);
		buff.Read(m_serverID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_type);
		buff.Write(m_sql);
		buff.Write(m_serverID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求战绩
struct LMsgL2LBDReqVipLog:public LMsg
{
	Lint	m_userId;
	Lint	m_reqUserId;

	LMsgL2LBDReqVipLog() :LMsg(MSG_L_2_LDB_VIP_LOG),m_userId(0),m_reqUserId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_reqUserId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_reqUserId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LBDReqVipLog();
	}
};

//玩家请求查看某轮录像;
struct LMsgL2LDBReqOneVipLog:public LMsg
{
	Lstring	m_log_Id;
	Lint	m_reqUserId;

	LMsgL2LDBReqOneVipLog() :LMsg(MSG_L_2_LDB_ONE_VIP_LOG),m_reqUserId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_log_Id);
		buff.Read(m_reqUserId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_log_Id);
		buff.Write(m_reqUserId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqOneVipLog();
	}
};
//////////////////////////////////////////////////////////////////////////
//logicdb 返回 玩家战绩;
struct LogInfo
{
	Lstring			m_id;		//id;
	Lint			m_time = 0;		//时间;
	Lint			m_flag = 0;		//房间规则;
	Lint			m_deskId = 0;	//桌子id;
	Lstring			m_secret;	//密码;
	Lint			m_maxCircle = 0;//总圈数;
	Lint			m_curCircle = 0;//当前圈数;
	Lint			m_posUserId[4] = { 0 };//0-3各个位置上的玩家id;
	Lint			m_curZhuangPos = 0;//当前庄家;
	Ldouble			m_score[4] = { 0 };// 各个位置上面的积分（累计积分）
	Ldouble			m_calScore[4] = { 0 };// 统计积分
	Ldouble			m_iniScore[4] = { 0 };// 初始积分
	Lint			m_reset = 0;//是否解算;
	Lstring			m_data;
	Lstring			m_playtype;	//玩法;
};

struct LMsgLBD2LReqOneVipLog:public LMsg
{
	Lint		m_userid;
	Lint		m_size;
	Lint        m_flag;
	Lstring		m_data;
	std::vector<Lint>    m_playType;
	LMsgLBD2LReqOneVipLog() :LMsg(MSG_LDB_2_L_ONE_VIP_LOG),m_size(0),m_userid(0), m_flag(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_size);
		buff.Read(m_flag);
		buff.Read(m_data);

		Lint size;
		buff.Read(size);
		Lint value = 0;
		for(Lint i = 0; i < size; ++i)
		{
			buff.Read(value);
			m_playType.push_back(value);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_size);
		buff.Write(m_flag);
		buff.Write(m_data);

		Lint size = m_playType.size();
		buff.Write(size);
		for(int i = 0; i < size; ++i)
		{	
			buff.Write(m_playType[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLBD2LReqOneVipLog();
	}
};

struct LMsgLBD2LReqVipLog:public LMsg
{
	Lint		m_userid = 0;
	Lint		m_count = 0;
	LogInfo		m_info[20];

	LMsgLBD2LReqVipLog() :LMsg(MSG_LDB_2_L_VIP_LOG)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Read(info.m_id);
			buff.Read(info.m_time);
			buff.Read(info.m_flag);
			buff.Read(info.m_deskId);
			buff.Read(info.m_secret);
			buff.Read(info.m_maxCircle);
			buff.Read(info.m_curCircle);
			buff.Read(info.m_posUserId[0]);
			buff.Read(info.m_posUserId[1]);
			buff.Read(info.m_posUserId[2]);
			buff.Read(info.m_posUserId[3]);

			buff.Read(info.m_curZhuangPos);
			buff.Read(info.m_score[0]);
			buff.Read(info.m_score[1]);
			buff.Read(info.m_score[2]);
			buff.Read(info.m_score[3]);
			buff.Read(info.m_calScore[0]);
			buff.Read(info.m_calScore[1]);
			buff.Read(info.m_calScore[2]);
			buff.Read(info.m_calScore[3]);
			buff.Read(info.m_iniScore[0]);
			buff.Read(info.m_iniScore[1]);
			buff.Read(info.m_iniScore[2]);
			buff.Read(info.m_iniScore[3]);
			buff.Read(info.m_reset);
//			buff.Read(info.m_playtype);
			buff.Read(info.m_data);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Write(info.m_id);
			buff.Write(info.m_time);
			buff.Write(info.m_flag);
			buff.Write(info.m_deskId);
			buff.Write(info.m_secret);
			buff.Write(info.m_maxCircle);
			buff.Write(info.m_curCircle);
			buff.Write(info.m_posUserId[0]);
			buff.Write(info.m_posUserId[1]);
			buff.Write(info.m_posUserId[2]);
			buff.Write(info.m_posUserId[3]);

			buff.Write(info.m_curZhuangPos);
			buff.Write(info.m_score[0]);
			buff.Write(info.m_score[1]);
			buff.Write(info.m_score[2]);
			buff.Write(info.m_score[3]);
			buff.Write(info.m_calScore[0]);
			buff.Write(info.m_calScore[1]);
			buff.Write(info.m_calScore[2]);
			buff.Write(info.m_calScore[3]);
			buff.Write(info.m_iniScore[0]);
			buff.Write(info.m_iniScore[1]);
			buff.Write(info.m_iniScore[2]);
			buff.Write(info.m_iniScore[3]);
			buff.Write(info.m_reset);
//			buff.Write(info.m_playtype);
			buff.Write(info.m_data);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLBD2LReqVipLog();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求录像;
struct LMsgL2LDBReqVideo:public LMsg
{
	Lint	m_userId;
	Lstring	m_videoId;

	LMsgL2LDBReqVideo() :LMsg(MSG_L_2_LDB_VIDEO), m_userId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//ldb 返回 玩家战绩;
struct LMsgLDB2LReqVideo:public LMsg
{
	Lint		m_userid = 0;
	Lint		m_count = 0;
	VideoLog	m_video;
	Lint		m_userCount;

	LMsgLDB2LReqVideo() :LMsg(MSG_LDB_2_L_VIDEO), m_userCount(4)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_count);
		buff.Read(m_userCount);
		m_video.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_count);
		buff.Write(m_userCount);
		m_video.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LReqVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//ldb 返回 id信息;
struct LMsgLDB2LLogInfo:public LMsg
{
	Lint	m_lastId = 0;
	Lint	m_videoId = 0;
	Lint	m_count = 0;
	std::vector<LogInfo> m_info;

	LMsgLDB2LLogInfo() :LMsg(MSG_LDB_2_L_LOG_INFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_lastId);
		buff.Read(m_videoId);
		buff.Read(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo info;
			buff.Read(info.m_id);
			buff.Read(info.m_time);
			buff.Read(info.m_flag);
			buff.Read(info.m_deskId);
			buff.Read(info.m_secret);
			buff.Read(info.m_maxCircle);
			buff.Read(info.m_curCircle);
			buff.Read(info.m_posUserId[0]);
			buff.Read(info.m_posUserId[1]);
			buff.Read(info.m_posUserId[2]);
			buff.Read(info.m_posUserId[3]);

			buff.Read(info.m_curZhuangPos);
			buff.Read(info.m_score[0]);
			buff.Read(info.m_score[1]);
			buff.Read(info.m_score[2]);
			buff.Read(info.m_score[3]);
			buff.Read(info.m_reset);
			buff.Read(info.m_data);

			m_info.push_back(info);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_lastId);
		buff.Write(m_videoId);
		m_count = m_info.size();
		buff.Write(m_count);
		for(Lint i = 0 ; i < m_count; ++i)
		{
			LogInfo& info = m_info[i];
			buff.Write(info.m_id);
			buff.Write(info.m_time);
			buff.Write(info.m_flag);
			buff.Write(info.m_deskId);
			buff.Write(info.m_secret);
			buff.Write(info.m_maxCircle);
			buff.Write(info.m_curCircle);
			buff.Write(info.m_posUserId[0]);
			buff.Write(info.m_posUserId[1]);
			buff.Write(info.m_posUserId[2]);
			buff.Write(info.m_posUserId[3]);

			buff.Write(info.m_curZhuangPos);
			buff.Write(info.m_score[0]);
			buff.Write(info.m_score[1]);
			buff.Write(info.m_score[2]);
			buff.Write(info.m_score[3]);
			buff.Write(info.m_reset);
			buff.Write(info.m_data);
		}


		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LLogInfo();
	}
};

//////////////////////////////////////////////////////////////////////////
//loigc 通过精确的videoid来查找sharevideo中的对应录像;
struct LMsgL2LDBGetSsharedVideo:public LMsg
{
	Lint	m_userId;
	Lstring	m_videoId;

	LMsgL2LDBGetSsharedVideo() :LMsg(MSG_L_2_LDB_GETSHAREVIDEOBYVIDEOID),m_userId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBGetSsharedVideo();
	}
};

//logic 玩家请求分享某录像，先去sharevideo中查看是否已被分享;
struct LMsgL2LDBGGetShareVideoId:public LMsg
{
	Lint	m_userId;
	Lstring	m_videoId;

	LMsgL2LDBGGetShareVideoId() :LMsg(MSG_L_2_LDB_GETSHAREVIDEOID),m_userId(0), m_videoId("")
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBGGetShareVideoId();
	}
};

//logic 玩家通过shareid来获取该分享码下的录像 在sharevideo中查询;
struct LMsgL2LDBGetShareVideo:public LMsg
{
	Lint	m_userId;
	Lint	m_shareId;

	LMsgL2LDBGetShareVideo() :LMsg(MSG_L_2_LDB_GETSHAREVIDEO),m_userId(0),m_shareId(0)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_shareId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_shareId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBGetShareVideo();
	}
};


//logic 请求保存某已转移到sharevideo中的某录像的shareid;
struct LMsgL2LDBSaveShareVideoId:public LMsg
{
	Lint	m_shareId;
	Lstring	m_videoId;

	LMsgL2LDBSaveShareVideoId() :LMsg(MSG_L_2_LDB_SAVESHAREVIDEOID),m_shareId(0),m_videoId("")
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_shareId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_shareId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveShareVideoId();
	}
};


//logicdb返回某录像是否有shareid，没有则将其从video中移至sharevideo中;
struct LMsgLDB2LRetShareVideoId:public LMsg
{
	Lint	m_userId;
	Lint	m_shareId;
	Lstring	m_videoId;

	LMsgLDB2LRetShareVideoId() :LMsg(MSG_LDB_2_L_RETSHAREVIDEOID),m_userId(0),m_shareId(0),m_videoId("")
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_shareId);
		buff.Read(m_videoId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_shareId);
		buff.Write(m_videoId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LRetShareVideoId();
	}
};

struct ShareVideoInfo{
	Lstring	m_videoId;
	Lint	m_time;
	Lint	m_userId[4];
	Ldouble	m_score[4];
	ShareVideoInfo():m_time(0){
		memset(m_userId,0,sizeof(m_userId));
		memset(m_score,0,sizeof(m_score));
	}
};
//logicdb 返回 通过shareid查询到的可能的录像;
struct LMsgLDB2LRetShareVideo:public LMsg
{
	Lint		m_userid;
	Lint		m_count;
	std::vector<ShareVideoInfo> m_info; 
	LMsgLDB2LRetShareVideo() :LMsg(MSG_LDB_2_L_RETSHAREVIDEO),m_count(0),m_userid(0)
	{

	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_count);
		for(int x=0;x<m_count;x++)
		{	
			ShareVideoInfo info;
			buff.Read(info.m_videoId);
			buff.Read(info.m_time);
			for(int i=0;i<4;i++)
			{
				buff.Read(info.m_userId[i]);
				buff.Read(info.m_score[i]);
			}
			m_info.push_back(info);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_count);
		for(int x=0;x<m_count;x++)
		{
			ShareVideoInfo& info = m_info[x];
			buff.Write(info.m_videoId);
			buff.Write(info.m_time);
			for(int i=0;i<4;i++)
			{
				buff.Write(info.m_userId[i]);
				buff.Write(info.m_score[i]);
			}
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LRetShareVideo();
	}
};

//////////////////////////////////////////////////////////////////////////
//logic 发送玩家请求活动;
struct LMsgL2LDBReqActive:public LMsg
{
	Lint	m_userId = 0;
	Lint	m_activeId = 0;

	LMsgL2LDBReqActive() :LMsg(MSG_L_2_LDB_REQ_ACTIVE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_activeId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_activeId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqActive();
	}
};

struct LMsgLDB2LActiveInfo:public LMsg
{
	Lint		m_userid = 0;
	Lint		m_count = 0;
	LActiveInfo	m_DrawInfo[10];
	LMsgLDB2LActiveInfo() :LMsg(MSG_LDB_2_L_REQ_ACTIVE){ }

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_count);
		for (Lint i = 0; i < 10 && i < m_count; ++i)
		{
			buff.Read(m_DrawInfo[i].m_activeID);
			buff.Read(m_DrawInfo[i].m_rewardID);
			buff.Read(m_DrawInfo[i].m_phoneNum);
			buff.Read(m_DrawInfo[i].m_date);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 10; ++i)
		{
			buff.Write(m_DrawInfo[i].m_activeID);
			buff.Write(m_DrawInfo[i].m_rewardID);
			buff.Write(m_DrawInfo[i].m_phoneNum);
			buff.Write(m_DrawInfo[i].m_date);
		} 
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgLDB2LActiveInfo(); }
};
//logic 发送玩家抽奖结果;
struct LMsgL2LDBSyncDraw:public LMsg
{
	Lint	m_userId = 0;
	Lint	m_activeID = 0;
	Lint	m_giftID = 0;
	Lint	m_date = 0;

	LMsgL2LDBSyncDraw() :LMsg(MSG_L_2_LDB_SYC_DRAW)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_activeID);
		buff.Read(m_giftID);
		buff.Read(m_date);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_activeID);
		buff.Write(m_giftID);
		buff.Write(m_date);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSyncDraw();
	}
};
//logic 发送玩家更新电话号码
struct LMsgL2LDBSyncPhone:public LMsg
{
	Lint	m_userId = 0;
	Lint	m_activeID = 0;
	Lint	m_date = 0;
	Lstring	m_phone;

	LMsgL2LDBSyncPhone() :LMsg(MSG_L_2_LDB_SYC_PHONE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_activeID);
		buff.Read(m_date);
		buff.Read(m_phone);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_activeID);
		buff.Write(m_date);
		buff.Write(m_phone);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSyncPhone();
	}
};


struct LMsgL2CeUerReLogin:public LMsg
{
	Lint	m_ID = 0;//类型;
	Lint	m_seed = 0;
	Lint	m_userid = 0;
	Lint	m_gateId = 0;
	Lint	m_geteUserId = 0;
	Lstring	m_ip;
	Lstring	m_uuid;
	Lstring	m_md5;

	LMsgL2CeUerReLogin() :LMsg(MSG_L_2_CE_USER_RELOGIN)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_seed);
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_ip);
		buff.Read(m_uuid);
		buff.Read(m_md5);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_seed);
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_ip);
		buff.Write(m_uuid);
		buff.Write(m_md5);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2CeUerReLogin();
	}
};
struct LMsgCe2LUserReLogin:public LMsg
{
	Lint	m_seed = 0;
	Lint	m_userid = 0;
	Lint	m_gateId = 0;
	Lint	m_geteUserId = 0;
	Lstring	m_ip;
	Lstring	m_md5;

	LMsgCe2LUserReLogin() :LMsg(MSG_CE_2_L_USER_RELOGIN)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_seed);
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_ip);
		buff.Read(m_md5);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_seed);
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_ip);
		buff.Write(m_md5);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCe2LUserReLogin();
	}
};

struct LMsgLMG2GHorseInfo :public LMsg
{
	Lstring	m_str;

	LMsgLMG2GHorseInfo():LMsg(MSG_LMG_2_G_HORSE_INFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_str);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_str);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2GHorseInfo();
	}
};


struct LMsgLMG2LLogin:public LMsg
{
	Lint		m_userid = 0;
	Lint		m_gateId = 0;
	Lint		m_geteUserId = 0;
	Lint        m_gameZone = 0; //PK_GAME_ZONE，金币场还是房卡场;
	Lstring		m_ip;
	Lstring		m_buyInfo;//够买信息;
	Lint		m_hide = 0;//购买隐藏0-隐藏 1- 不隐藏;
	Lint		m_card_num;	//房卡数 （Card2）

	LMsgLMG2LLogin() :LMsg(MSG_LMG_2_L_USER_LOGIN), m_card_num(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_gameZone);
		buff.Read(m_ip);
		buff.Read(m_buyInfo);
		buff.Read(m_hide);
		buff.Read(m_card_num);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_gameZone);
		buff.Write(m_ip);
		buff.Write(m_buyInfo);
		buff.Write(m_hide);
		buff.Write(m_card_num);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LLogin();
	}
};

struct LMsgL2LMGModifyUserState:public LMsg
{
	Lint	m_userid = 0;
	Lint	m_userstate = 0;
	Lint	m_logicID = 0;

	LMsgL2LMGModifyUserState() :LMsg(MSG_L_2_LMG_MODIFY_USER_STATE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_userstate);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_userstate);
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyUserState();
	}
};

struct LMsgL2GModifyUserState:public LMsg
{
	Lint	m_gateid = 0;
	Lint	m_userstate = 0;
	Lint	m_logicID = 0;

	LMsgL2GModifyUserState() :LMsg(MSG_L_2_G_MODIFY_USER_STATE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gateid);
		buff.Read(m_userstate);
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gateid);
		buff.Write(m_userstate);
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GModifyUserState();
	}
};


struct LMsgLMG2LCreateDesk : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lint				m_geteUserId;
	Lint				m_deskID;
	Lstring				m_ip;
	LUser				m_usert;

	Lint				m_flag;				//房间类型，1-2圈，2-四圈，3-8圈;
	Lstring				m_secret;			//房间密码，如果为空，服务器会随机一个密码;
	Lint				m_gold;				//房间抵住，单位元，最低为1元，最多不能超过100;
	Lint				m_gameType;			//游戏类型;
	Lint                m_nMaxFanshu;       //最大翻倍数;
	Lint				m_robotNum;			//0,不加机器人，1，2，3加机器人数量;
	Lint                m_black3first;     //起手出牌是否可以不带黑桃3;1为可以不带，0必须带黑桃3;
	Lint				m_lenth;			//发牌长度;
	CardValue			m_cardValue[CARD_COUNT];

	std::vector<Lint>	m_extValue;			//附加字段	;	

	Lint				m_useTeamCard;		// 0 不使用工会房卡创建房间，1 使用工会房卡创建房间 [2017-7-21 willing]
	Lint				m_teamID;			// 工会ID [2017-7-23 willing]
	Lstring				m_teamName;			// 工会名称 [2017-7-25 willing]
	Lint				m_free;				// 是否是免费玩法，1 免费，0 不免费 [2017-8-1 willing]
	Lint				m_maYouDesk;					//0:不是 1：快速组局 2：自由创建
	Lint                m_proportionate = 0;    //抽成比例
	Lint                m_circleGoldLimit = 0;  //每局限制进行金币数
	Lint                m_stake = 0; //押注倍率

	///////////////////////////长沙玩法////////////////
	Lint				m_playTypeCount = 0;
	std::vector<Lint>	m_playtype;			//玩法
	Lint				m_state = 0;			//玩法规则 0 转转   1 长沙, 100开始为比赛场
	Lint				m_rank = 0;				//第几名
	Lint				m_score = 0;			//玩家分数
	Lint				m_overScore = 0;		//淘汰分数
	Lint				m_process = 0;          //比赛进程

	Lstring				m_location;			//位置
	Lint				m_canInteraction = 0;	//是否可以互动

	Lint				m_antiCheat = 0;		// 0 关闭 1 开启
	Lint				m_aaRoom = 0;			//aa房间 0否，1是
	Lint				m_playerNum = 0;		//游戏人数


	LMsgLMG2LCreateDesk() :LMsg(MSG_LMG_2_L_CREATE_DESK), m_userid(0), m_gateId(0), m_geteUserId(0), m_deskID(0)
		, m_flag(0), m_gold(0),  m_robotNum(0), m_useTeamCard(0), m_teamID(0), m_free(0), m_maYouDesk(0), m_playTypeCount(0),m_state(0)
		, m_rank(0), m_score(0), m_overScore(0), m_process(0)
	{
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_flag);
		buff.Read(m_gold);
		buff.Read(m_gameType);
		buff.Read(m_nMaxFanshu);
		buff.Read(m_robotNum);
		buff.Read(m_black3first);
		buff.Read(m_lenth);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}

		Lint size;
		buff.Read(size);
		
		Lint value = 0;
		for(Lint i = 0; i < size; ++i)
		{
			buff.Read(value);
			m_extValue.push_back(value);
		}

		buff.Read(m_useTeamCard);
		buff.Read(m_teamID);
		buff.Read(m_teamName);
		buff.Read(m_free);
		buff.Read(m_maYouDesk);
		buff.Read(m_proportionate);
		buff.Read(m_circleGoldLimit);
		buff.Read(m_stake);

		buff.Read(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			Lint playType;
			buff.Read(playType);
			m_playtype.push_back(playType);
		}
		buff.Read(m_state);
		buff.Read(m_rank);
		buff.Read(m_score);
		buff.Read(m_overScore);
		buff.Read(m_process);
		buff.Read(m_location);
		buff.Read(m_canInteraction);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_flag);
		buff.Write(m_gold);
		buff.Write(m_gameType);
		buff.Write(m_nMaxFanshu);
		buff.Write(m_robotNum);
		buff.Write(m_black3first);
		buff.Write(m_lenth);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}

		Lint size = m_extValue.size();

		buff.Write(size);
		int value = 0;
		for(int i = 0; i < size; ++i)
		{	value = m_extValue[i];
			buff.Write(value);
		}
		buff.Write(m_useTeamCard);
		buff.Write(m_teamID);
		buff.Write(m_teamName);
		buff.Write(m_free);
		buff.Write(m_maYouDesk);
		buff.Write(m_proportionate);
		buff.Write(m_circleGoldLimit);
		buff.Write(m_stake);

		buff.Write(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			buff.Write(m_playtype[i]);
		}
		buff.Write(m_state);
		buff.Write(m_rank);
		buff.Write(m_score);
		buff.Write(m_overScore);
		buff.Write(m_process);
		buff.Write(m_location);
		buff.Write(m_canInteraction);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LCreateDesk();
	}
};


struct LMsgLMG2LIntoGoldDesk : public LMsg
{
	Lint				m_userid;
	Lint				m_gateId;
	Lint				m_geteUserId;
	Lint				m_deskID;
	Lstring				m_ip;
	LUser				m_usert;
	Lint				m_gameType;			//游戏类型;
	Lint				m_robotNum;			//0,不加机器人，1，2，3加机器人数量;
	Lint				m_lenth;			//发牌长度;
	CardValue			m_cardValue[CARD_COUNT];

	std::vector<Lint>	m_extValue;			//附加字段;			


	LMsgLMG2LIntoGoldDesk() :LMsg(MSG_LMG_2_L_INTO_GOLDDESK)
	{
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_gameType);
		buff.Read(m_robotNum);
		buff.Read(m_lenth);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}

		Lint size;
		buff.Read(size);

		Lint value = 0;
		for(Lint i = 0; i < size; ++i)
		{
			buff.Read(value);
			m_extValue.push_back(value);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_gameType);
		buff.Write(m_robotNum);
		buff.Write(m_lenth);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}

		Lint size = m_extValue.size();

		buff.Write(size);
		int value = 0;
		for(int i = 0; i < size; ++i)
		{	value = m_extValue[i];
		buff.Write(value);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LIntoGoldDesk();
	}
};

struct LMsgLMG2LAddToDesk : public LMsg
{
	Lint			m_userid = 0;
	Lint			m_gateId = 0;
	Lint			m_geteUserId = 0;
	Lint			m_deskID = 0;
	Lstring			m_ip;
	LUser			m_usert;
	Lstring			m_location;

	Lint			m_rank;				//第几名
	Lint			m_score;			//玩家分数
	Lint			m_overScore;		//淘汰分数
	Lint            m_sportId;          //比赛场id
	Lint            m_process;          //比赛进程 0 就是 淘汰赛阶段 1 晋级 第一阶段 2 晋级第二阶段 3 晋级最后阶段

	LMsgLMG2LAddToDesk() :LMsg(MSG_LMG_2_L_ADDTO_DESK)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_location);

		buff.Read(m_rank);
		buff.Read(m_score);
		buff.Read(m_overScore);
		buff.Read(m_sportId);
		buff.Read(m_process);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_location);

		buff.Write(m_rank);
		buff.Write(m_score);
		buff.Write(m_overScore);
		buff.Write(m_sportId);
		buff.Write(m_process);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LAddToDesk();
	}
};


struct LMsgL2LMGModifyCard :public LMsg
{
	Lint			m_userid = 0;
	Lint			isAddCard = 0;
	Lint			cardType = 0;
	Ldouble			cardNum = 0;
	Lint			operType = 0;
	Lstring			admin;
	Lint            m_nGameType = 0;  // 游戏的玩法类型，用于分玩法免费

	LMsgL2LMGModifyCard() :LMsg(MSG_L_2_LMG_MODIFY_USER_CARD)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(isAddCard);
		buff.Read(cardType);
		buff.Read(cardNum);
		buff.Read(operType);
		buff.Read(admin);
		buff.Read(m_nGameType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(isAddCard);
		buff.Write(cardType);
		buff.Write(cardNum);
		buff.Write(operType);
		buff.Write(admin);
		buff.Write(m_nGameType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyCard();
	}
};

struct LMsgL2LMGGoldChange :public LMsg
{
	Lint		m_userid = 10;
	Lint		m_oper = 10;   //0首次赠送金币,1:每日赠送金币;2,输赢金币;3，抽奖消耗金币;4，抽奖奖励金币;5，每局房间费消耗金币;
	Lint		m_GoldNum = 10;
	LMsgL2LMGGoldChange() :LMsg(MSG_L_2_LMG_GOLDCHANGE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_oper);
		buff.Read(m_GoldNum);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_oper);
		buff.Write(m_GoldNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGGoldChange();
	}
};

struct LMsgL2LMGModifyPlayCount :public LMsg
{
	Lint			m_userid = 0;
	Lint			m_curPlayCount = 0;
	LMsgL2LMGModifyPlayCount() :LMsg(MSG_L_2_LMG_MODIFY_USER_PLAYCOUNT)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_curPlayCount);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_curPlayCount);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGModifyPlayCount();
	}
};

struct LMsgL2LMGRecyleDeskID :public LMsg
{
	Lint			m_userid = 0;
	Lint			m_serverID = 0;
	Lint			m_deskID = 0;
	LMsgL2LMGRecyleDeskID() :LMsg(MSG_L_2_LMG_RECYLE_DESKID)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_serverID);
		buff.Read(m_deskID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_serverID);
		buff.Write(m_deskID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGRecyleDeskID();
	}
};

struct LMsgL2LMGRecyleGoldDeskPlace :public LMsg
{
	Lint			m_serverID = 0;
	Lint			m_deskID = 0;
	Lint            m_gameType = 0;
	LMsgL2LMGRecyleGoldDeskPlace() :LMsg(MSG_L_2_LMG_RECYLE_GOLDDESKPLACE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_deskID);
		buff.Read(m_gameType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_deskID);
		buff.Write(m_gameType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGRecyleGoldDeskPlace();
	}
};

struct LMsgL2LMGUseGoldDeskPlace :public LMsg
{
	Lint			m_serverID = 0;
	Lint			m_deskID = 0;
	Lint            m_gameType = 0;
	Lint            m_usedCounts = 0;
	LMsgL2LMGUseGoldDeskPlace() :LMsg(MSG_L_2_LMG_USE_GOLDDESKPLACE)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_serverID);
		buff.Read(m_deskID);
		buff.Read(m_gameType);
		buff.Read(m_usedCounts);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_serverID);
		buff.Write(m_deskID);
		buff.Write(m_gameType);
		buff.Write(m_usedCounts);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUseGoldDeskPlace();
	}
};

//G2L
struct LMsgG2LHEARTBEAT :public LMsg
{
	Lint			m_gateID;
	LMsgG2LHEARTBEAT() :LMsg(MSG_G_2_L_BREATH), m_gateID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gateID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gateID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LHEARTBEAT();
	}
};

struct LMsgL2GHEARTBEAT_BACK :public LMsg
{
	Lint			m_logicID;
	LMsgL2GHEARTBEAT_BACK() :LMsg(MSG_L_2_G_BREATH), m_logicID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2GHEARTBEAT_BACK();
	}
};

//G2LMG
struct LMsgG2LMGHEARTBEAT :public LMsg
{
	Lint			m_gateID;
	LMsgG2LMGHEARTBEAT() :LMsg(MSG_G_2_LMG_BREATH), m_gateID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gateID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gateID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2LMGHEARTBEAT();
	}
};

struct LMsgLMG2GHEARTBEAT_BACK :public LMsg
{
	LMsgLMG2GHEARTBEAT_BACK() :LMsg(MSG_LMG_2_G_BREATH)
	{
	}

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
		return new LMsgLMG2GHEARTBEAT_BACK();
	}
};


//L2LMG
struct LMsgL2LMGHEARTBEAT :public LMsg
{
	Lint			m_logicID;
	LMsgL2LMGHEARTBEAT() :LMsg(MSG_L_2_LMG_BREATH), m_logicID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGHEARTBEAT();
	}
};

struct LMsgLMG2LHEARTBEAT_BACK :public LMsg
{
	LMsgLMG2LHEARTBEAT_BACK() :LMsg(MSG_LMG_2_L_BREATH)
	{
	}

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
		return new LMsgLMG2LHEARTBEAT_BACK();
	}
};

//L2LDB
struct LMsgL2LDBHEARTBEAT :public LMsg
{
	Lint			m_logicID;
	LMsgL2LDBHEARTBEAT() :LMsg(MSG_L_2_LDB_BREATH), m_logicID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBHEARTBEAT();
	}
};

struct LMsgLDB2LHEARTBEAT_BACK :public LMsg
{
	LMsgLDB2LHEARTBEAT_BACK() :LMsg(MSG_LDB_2_L_BREATH)
	{
	}

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
		return new LMsgLDB2LHEARTBEAT_BACK();
	}
};

//LMG2LMG
struct LMsgLMG2LDBHEARTBEAT :public LMsg
{
	Lint			m_logicID;
	LMsgLMG2LDBHEARTBEAT() :LMsg(MSG_LMG_2_LDB_BREATH), m_logicID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_logicID);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDBHEARTBEAT();
	}
};

struct LMsgLDB2LMGHEARTBEAT_BACK :public LMsg
{
	LMsgLDB2LMGHEARTBEAT_BACK() :LMsg(MSG_LDB_2_LMG_BREATH)
	{
	}

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
		return new LMsgLDB2LMGHEARTBEAT_BACK();
	}
};
//LMG2CE
struct LMsgLMG2CEHEARTBEAT :public LMsg
{
	Lint			m_logicID;
	LMsgLMG2CEHEARTBEAT() :LMsg(MSG_LMG_2_CE_BREATH),m_logicID(0)
	{
	}

	virtual bool Read(msgpack::object& obj)
	{
 		ReadMapData(obj, "m_logicID", m_logicID);
		return true;
	}

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack,2);
		WriteKeyValue(pack, "m_msgId", m_msgId);
		WriteKeyValue(pack, "m_logicID", m_logicID);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CEHEARTBEAT();
	}
};

struct LMsgCE2LMGHEARTBEAT_BACK :public LMsg
{
	LMsgCE2LMGHEARTBEAT_BACK() :LMsg(MSG_CE_2_LMG_BREATH)
	{
	}

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
		return new LMsgCE2LMGHEARTBEAT_BACK();
	}
};

//center发给logicmanager礼券信息和礼物信息;
struct LMsgCE2LMGUserGiftInfo :public LMsg
{
	Lint    m_gift_count;
	std::vector<gift_info> m_gift_info; 
	LMsgCE2LMGUserGiftInfo():LMsg(MSG_CE_2_LMG_User_Gift_Info), m_gift_count(0)
	{
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_gift_count);
		for(int x=0;x<m_gift_count;x++)
		{
			gift_info l_gift;
			l_gift.Read(buff);
			m_gift_info.push_back(l_gift);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_gift_count);
		for(size_t x=0;x<m_gift_info.size();x++)
		{
			m_gift_info[x].Write(buff);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGUserGiftInfo();
	}
};
//logicmanager 发给 center 礼券兑换记录;
struct LMsgLMG2CEUserCouponsExchangeRecord :public LMsg
{
	Lstring	m_unionId;
	Lint	m_logicId;
	LMsgLMG2CEUserCouponsExchangeRecord() :LMsg(MSG_LMG_2_CE_User_Coupons_Exchange_Record), m_logicId(0)
	{
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_unionId);
		buff.Read(m_logicId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_unionId);
		buff.Write(m_logicId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CEUserCouponsExchangeRecord();
	}
};
//center 发给 logicmanager 礼券兑换记录;
struct LMsgCE2LMGUserCouponsExchangeRecord :public LMsg
{
	Lint	m_userId;
	Lint	m_record_count;
	std::vector<gift_exchange_record> m_record;
	LMsgCE2LMGUserCouponsExchangeRecord() :LMsg(MSG_CE_2_LMG_User_Coupons_Exchange_Record),m_record_count(0),m_userId(0)
	{
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_record_count);
		for(int x=0;x<m_record_count;x++)
		{
			gift_exchange_record record;
			record.Read(buff);
			m_record.push_back(record);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_record_count);
		for(size_t x=0;x<m_record.size();x++)
		{
			m_record[x].Write(buff);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGUserCouponsExchangeRecord();
	}
};
//logicmanager 发给 center 礼券兑换消息;
struct LMsgLMG2CEUserCouponsExchangeInfo :public LMsg
{
	Lstring	m_unionId;
	Lint    m_logicId = 0;
	Lint	m_giftId = 0;
	Lint    m_time = 0;
	Lstring m_phoneNum;
	LMsgLMG2CEUserCouponsExchangeInfo() :LMsg(MSG_LMG_2_CE_User_Coupons_Exchange_Info)
	{
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_unionId);
		buff.Read(m_logicId);
		buff.Read(m_giftId);
		buff.Read(m_time);
		buff.Read(m_phoneNum);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_unionId);
		buff.Write(m_logicId);
		buff.Write(m_giftId);
		buff.Write(m_time);
		buff.Write(m_phoneNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CEUserCouponsExchangeInfo();
	}
};
//center 发给 logicmanager 礼券兑换结果
struct LMsgCE2LMGUserCouponsExchangeInfo :public LMsg
{
	Lint	m_userId = 0;
	Lint	m_errorId = 0;
	LMsgCE2LMGUserCouponsExchangeInfo() :LMsg(MSG_CE_2_LMG_User_Coupons_Exchange_Info)
	{
	}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_errorId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_errorId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgCE2LMGUserCouponsExchangeInfo();
	}
};

struct LMsgL2LDBExecSql : public LMsg
{
	Lstring		m_sql;

	LMsgL2LDBExecSql() : LMsg(MSG_L_2_LDB_EXEC_SQL) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_sql);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_sql);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBExecSql();
	}
};

struct LMsgLMG2LDBReqTaskProgress : public LMsg
{
	Lint			m_userId = 0;
	Lint			m_taskId = 0;
	Lstring			m_sql;

	LMsgLMG2LDBReqTaskProgress() : LMsg(MSG_LMG_2_LDB_REQ_TASK_PROGRESS) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_taskId);
		buff.Read(m_sql);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_taskId);
		buff.Write(m_sql);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDBReqTaskProgress();
	}
};

struct LMsgLDB2LMGTaskProgress : public LMsg
{
	Lint		m_userId = 0;
	Lint		m_taskId = 0;
	Lint		m_result = 0;
	Lint		m_progressId = 0;
	Lint		m_progress = 0;
	Lint		m_rewardRecved = 0;

	LMsgLDB2LMGTaskProgress() : LMsg(MSG_LDB_2_LMG_REQ_TASK_PROGRESS) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_taskId);
		buff.Read(m_result);
		buff.Read(m_progressId);
		buff.Read(m_progress);
		buff.Read(m_rewardRecved);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_taskId);
		buff.Write(m_result);
		buff.Write(m_progressId);
		buff.Write(m_progress);
		buff.Write(m_rewardRecved);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGTaskProgress();
	}
};

struct LMsgLMG2LDBReqBindingRelationships : public LMsg
{
	Lint			m_userId;

	LMsgLMG2LDBReqBindingRelationships() : LMsg(MSG_LMG_2_LDB_REQ_BINDING_RELATIONSHIPS), m_userId(0) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDBReqBindingRelationships;
	}
};

struct LMsgLDB2LMGBindingRelationships : public LMsg
{
	Lint		m_userId = 0;
	Lint		m_inviterId = 0;
	Lint		m_finished = 0;
	Lint		m_inviteeCount = 0;
	Lint		m_invitees[NUM_MAX_INVITEE] = { 0 };
	Lint		m_taskFinished[NUM_MAX_INVITEE] = { 0 };

	LMsgLDB2LMGBindingRelationships() : LMsg(MSG_LDB_2_LMG_REQ_BINDING_RELATIONSHIPS) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_inviterId);
		buff.Read(m_finished);
		buff.Read(m_inviteeCount);
		for (int i = 0; i < m_inviteeCount; i++)
		{
			buff.Read(m_invitees[i]);
			buff.Read(m_taskFinished[i]);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_inviterId);
		buff.Write(m_finished);
		buff.Write(m_inviteeCount);
		for (int i = 0; i < m_inviteeCount; i++)
		{
			buff.Write(m_invitees[i]);
			buff.Write(m_taskFinished[i]);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGBindingRelationships;
	}
};

struct LMsgLMG2LDBInsertTaskProgress : public LMsg
{
	Lint		m_userId = 0;
	Lint		m_taskId = 0;
	Lstring		m_sql;

	LMsgLMG2LDBInsertTaskProgress() : LMsg(MSG_LMG_2_LDB_INSERT_TASK_PROGRESS) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_taskId);
		buff.Read(m_sql);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_taskId);
		buff.Write(m_sql);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDBInsertTaskProgress;
	}

};

struct LMsgLDB2LMGInsertTaskProgress : public LMsg
{
	Lint		m_userId = 0;
	Lint		m_taskId = 0;
	Lint		m_progressId = 0;

	LMsgLDB2LMGInsertTaskProgress() : LMsg(MSG_LDB_2_LMG_INSERT_TASK_PROGRESS) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_taskId);
		buff.Read(m_progressId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_taskId);
		buff.Write(m_progressId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGInsertTaskProgress;
	}

};

struct LMsgLMG2LDBReqActiveGiftStock : public LMsg
{
	Lint	m_activeId;

	LMsgLMG2LDBReqActiveGiftStock() : LMsg(MSG_LMG_2_LDB_REQ_ACTIVE_GIFT_STOCK), m_activeId(0) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_activeId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_activeId);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDBReqActiveGiftStock;
	}

};

struct LMsgLDB2LMGActiveGiftStock : public LMsg
{
	Lint					m_error = 0;		// 0 : succeed, not 0 : failed
	Lint					m_activeId = 0;
	std::vector<Lint>		m_giftStock;

	LMsgLDB2LMGActiveGiftStock() : LMsg(MSG_LDB_2_LMG_ACTIVE_GIFT_STOCK) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_error);
		buff.Read(m_activeId);
		Lsize size = 0;
		buff.Read(size);
		for (Lsize i = 0; i < size; i++)
		{
			Lint giftStock = 0;
			buff.Read(giftStock);
			m_giftStock.push_back(giftStock);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_error);
		buff.Write(m_activeId);
		Lsize size = m_giftStock.size();
		buff.Write(size);
		for (Lsize i = 0; i < size; i++)
			buff.Write(m_giftStock[i]);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGActiveGiftStock;
	}
};

struct LMsgLMG2LDBReqLuckyDrawNum : public LMsg
{
	Lint		m_userId;

	LMsgLMG2LDBReqLuckyDrawNum() : LMsg(MSG_LMG_2_LDB_REQ_LUCKY_DRAW_NUM), m_userId(0) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDBReqLuckyDrawNum;
	}
};

struct LMsgLDB2LMGLuckyDrawNum : public LMsg
{
	Lint	m_userId = 0;
	Lint	m_error = 0;
	Lint	m_drawNum = 0;

	LMsgLDB2LMGLuckyDrawNum() : LMsg(MSG_LDB_2_LMG_LUCKY_DRAW_NUM) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_error);
		buff.Read(m_drawNum);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_error);
		buff.Write(m_drawNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGLuckyDrawNum;
	}

};

// logingate to center;
struct LMsgLGCEUserMsgWrapper:public LMsg
{
	Lint	m_idOnGate;
	LMsg*	m_userMsg;
	bool	m_deleteUserMsg;

	LMsgLGCEUserMsgWrapper() : LMsg(MSG_LG_CE_USER_MSG_WRAPPER)
	{
		m_idOnGate = 0;
		m_userMsg = NULL;
		m_deleteUserMsg = true;
	}

	~LMsgLGCEUserMsgWrapper()
	{
		if(m_deleteUserMsg && m_userMsg != NULL)
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_idOnGate);
		m_userMsg = NULL;

		MsgHeader header = {0};
		buff.Read((char*)&header, sizeof(header));
		if(header.bodySize > (0xFFFF - sizeof(header)))
		{
			LLOG_ERROR("LMsgLGCEUserMsgWrapper::Read -- The data len more than max value");
			return true;
		}
		try
		{
			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
			msgpack::object obj = unpack.get();

			Lint msgId = 0;
			ReadMapData(obj, "m_msgId", msgId);
			m_userMsg = LMsgFactory::Instance().CreateMsg(msgId);
			if (m_userMsg)
				m_userMsg->Read(obj);
			else
				LLOG_ERROR("LMsgLGCEUserMsgWrapper::Read(): user msg id %d does not exist.", msgId);
		}
		catch (...)
		{
			LLOG_ERROR("LMsgLGCEUserMsgWrapper::Read()::RecvMsgPack error");
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_idOnGate);
		auto dataBuff = m_userMsg->GetSendBuff();
		buff.Write(dataBuff->Data(), dataBuff->Size());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLGCEUserMsgWrapper();
	}
};

struct LMsgLG2CEHeartBeat : public LMsg
{
	LMsgLG2CEHeartBeat() : LMsg(MSG_LG_2_CE_HEARTBEAT)
	{
	}

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
		return new LMsgLG2CEHeartBeat();
	}
};


////收集字写入数据库;
//struct LMsgL2LDBSyncCollect:public LMsg
//{
//	Lint	m_userId;
//	Lint	m_activeID;
//	Lint	m_giftID;
//	Lint	m_date;
//
//	LMsgL2LDBSyncCollect() :LMsg(MSG_L_2_LDB_SYC_DRAW)
//	{
//	}
//
//	virtual bool Read(LBuff& buff)
//	{
//		buff.Read(m_userId);
//		buff.Read(m_activeID);
//		buff.Read(m_giftID);
//		buff.Read(m_date);
//		return true;
//	}
//
//	virtual bool Write(LBuff& buff)
//	{
//		buff.Write(m_userId);
//		buff.Write(m_activeID);
//		buff.Write(m_giftID);
//		buff.Write(m_date);
//		return true;
//	}
//
//	virtual LMsg* Clone()
//	{
//		return new LMsgL2LDBSyncCollect();
//	}
//};
//-----------------------------------------------------
//十一活动
struct LActiveSignCollectSS
{
	Lint	m_activeType;
	Lint	m_drawChanceCount;	//每天能抽几次
	Lint	m_date;
	std::vector<Lint>	m_words;	//1~8
	Lstring	m_nickName;	//
	Lint	m_isUpdate;
	Lint	m_hadGift;
	LActiveSignCollectSS()
	{
		m_activeType = 0;
		m_drawChanceCount = 0;
		m_date = 0;
		m_isUpdate = 0;
		m_hadGift = 0;
	}

	bool Read(LBuff& buff)
	{
		buff.Read(m_activeType);
		buff.Read(m_drawChanceCount);
		buff.Read(m_date);
		
		Lint size = 0;
		Lint value = 0;
		buff.Read(size);
		for(int i = 0; i< size; ++i)
		{
			buff.Read(value);
			m_words.push_back(value);
		}

		buff.Read(m_nickName);
		buff.Read(m_isUpdate);
		buff.Read(m_hadGift);
		return true;
	}

	bool Write(LBuff& buff)
	{
		buff.Write(m_activeType);
		buff.Write(m_drawChanceCount);
		buff.Write(m_date);
		
		Lint size = m_words.size();
		Lint value = 0;
		buff.Write(size);
		for(int i = 0; i< size; ++i)
		{
			value = m_words[i];
			buff.Write(value);
		}

		buff.Write(m_nickName);
		buff.Write(m_isUpdate);
		buff.Write(m_hadGift);
		return true;
	}
};

//
struct LMsgL2LMGAddUserBashu :public LMsg
{
	Lint			m_userid = 0;
	Lint			m_addBashu = 0;	//把数
	Lstring			admin;
	LMsgL2LMGAddUserBashu() :LMsg(MSG_L_2_LMG_MODIFY_USER_BASHU)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_addBashu);
		buff.Read(admin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_addBashu);
		buff.Write(admin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGAddUserBashu();
	}
};

//2016十一活动
//请求 签到，集字，获奖数据（lgm 请求数据）
struct LMsgL2LDBReqSignCollectWin:public LMsg
{
	Lint	m_userId = 0;
	Lint	m_activeType = 0;

	LMsgL2LDBReqSignCollectWin() :LMsg(MSG_L_2_LDB_REQ_SIGN_COLLECT_WIN)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_activeType);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_activeType);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBReqSignCollectWin();
	}
};

//返回 签到，集字，获奖数据 （ldb 返回数据到 lgm）
struct LMsgLDB2LResSignCollectWin:public LMsg
{
	Lint		m_userid = 0;
	Lint		m_type = 0;	//类型区分
	Lint		m_count = 0;
	//Lstring		m_userNick;
	LActiveSignCollectSS	m_DrawInfo[100];
	LMsgLDB2LResSignCollectWin() :LMsg(MSG_LDB_2_L_RES_SIGN_COLLECT_WIN){ }

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_type);
		buff.Read(m_count);
		//buff.Read(m_userNick);
		for (Lint i = 0; i < 100 && i < m_count; ++i)
		{
			m_DrawInfo[i].Read(buff);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_type);
		buff.Write(m_count);
		//buff.Write(m_userNick);
		for (Lint i = 0; i < m_count && i < 100; ++i)
		{
			m_DrawInfo[i].Write(buff);
		} 
		return true;
	}

	virtual LMsg* Clone(){ return new LMsgLDB2LResSignCollectWin(); }
};

// 保存 签到,集字, 获奖名单;（lgm请求保存数据）
struct LMsgL2LDBSaveSignCollectWin:public LMsg
{
	Lint	m_userId = 0;
	Lint	m_activeType = 0;
	Lint	m_drawChanceCount = 0;
	Lint	m_date = 0;
	Lstring	m_name;
	Lint	m_isUpdate = 0; //数据库id
	std::vector<Lint> m_words;
	Lint	m_hadGift = 0;

	LMsgL2LDBSaveSignCollectWin() :LMsg(MSG_L_2_LDB_SAVE_SIGN_COLLECT_WIN)
	{
		m_hadGift = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_activeType);
		buff.Read(m_drawChanceCount);
		buff.Read(m_date);
		buff.Read(m_name);
		buff.Read(m_isUpdate);

		Lint size = 0;
		Lint value = 0;
		buff.Read(size);
		for(int i = 0; i < size; i++)
		{
			buff.Read(value);
			m_words.push_back(value);
		}

		buff.Read(m_hadGift);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_activeType);
		buff.Write(m_drawChanceCount);
		buff.Write(m_date);
		buff.Write(m_name);
		buff.Write(m_isUpdate);
		
		Lint size = m_words.size();
		int value = 0;

		buff.Write(size);
		for(int i = 0; i < size; ++i)
		{	value = m_words[i];
		buff.Write(value);
		}

		buff.Write(m_hadGift);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LDBSaveSignCollectWin();
	}
};

struct LMsgLMG2LDBInsertBindingRelationship : public LMsg
{
	Lint		m_userId;
	Lint		m_inviter;
	Lint		m_maxInviteeNum;

	LMsgLMG2LDBInsertBindingRelationship()
		: LMsg(MSG_LMG_2_LDB_INSERT_BINDING_RELATIONSHIP), m_userId(-1), m_inviter(-1), m_maxInviteeNum(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_inviter);
		buff.Read(m_maxInviteeNum);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_inviter);
		buff.Write(m_maxInviteeNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LDBInsertBindingRelationship;
	}
};

struct LMsgLDB2LMGInsertBindingRelationship : public LMsg
{
	Lint					m_userId;
	Lint					m_inviter;
	ErrorCode::ErrorCode	m_errorCode;

	LMsgLDB2LMGInsertBindingRelationship()
		: LMsg(MSG_LDB_2_LMG_INSERT_BINDING_RELATIONSHIP), m_userId(-1), m_inviter(-1), m_errorCode(ErrorCode::ErrorNone)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_inviter);
		buff.Read(m_errorCode);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_inviter);
		buff.Write(m_errorCode);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLDB2LMGInsertBindingRelationship;
	}
};

struct LMsgL2LMGUserCircleIncreased : public LMsg
{
	Lint			m_userid;
	LMsgL2LMGUserCircleIncreased() :LMsg(MSG_L_2_LMG_USER_CIRCLE_INCREASED), m_userid(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUserCircleIncreased();
	}
};


//MaYouQuan begin
// TeamServer 登陆到 LogicManager [2017-7-10 willing]	
struct LMsgTS2LMG_Login : public LMsg
{
	Lstring strTeamServerIp = "";
	Lshort	nTeamServerPort = 0;
	Lint	m_TeamServerID = 0;
	LMsgTS2LMG_Login() :LMsg(MSG_TS_2_LMG_LOGIN) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(strTeamServerIp);
		buff.Read(nTeamServerPort);
		buff.Read(m_TeamServerID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(strTeamServerIp);
		buff.Write(nTeamServerPort);
		buff.Write(m_TeamServerID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2LMG_Login(); }
};

// TeamServer 到 LogicManager 的心跳 [2017-7-10 willing]
struct LMsgTS2LMG_Heartbeat : public LMsg
{
	LMsgTS2LMG_Heartbeat() :LMsg(MSG_TS_2_LMG_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2LMG_Heartbeat(); }
};


// LogicManager 到 TeamServer 的心跳 [2017-7-10 willing]	
struct LMsgLMG2TS_Heartbeat : public LMsg
{
	LMsgLMG2TS_Heartbeat() :LMsg(MSG_LMG_2_TS_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_Heartbeat(); }
};


// TeamServer 登陆到 LogicDB [2017-7-10 willing]
struct LMsgTS2LDB_Login : public LMsg
{
	LMsgTS2LDB_Login() :LMsg(MSG_TS_2_LDB_LOGIN) {}
	virtual bool Read(LBuff& buff)
	{
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2LDB_Login(); }
};


// TeamServer 到 LogicDB 的心跳 [2017-7-10 willing]
struct LMsgTS2LDB_Heartbeat : public LMsg
{
	LMsgTS2LDB_Heartbeat() :LMsg(MSG_TS_2_LDB_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2LDB_Heartbeat(); }
};

// LogicDB 到 TeamServer 的心跳 [2017-7-10 willing]
struct LMsgLDB2TS_Heartbeat : public LMsg
{
	LMsgLDB2TS_Heartbeat() :LMsg(MSG_LDB_2_TS_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLDB2TS_Heartbeat(); }
};


// LogicServer 登陆到 TeamServer [2017-7-10 willing]
struct LMsgL2TS_Login : public LMsg
{
	Lint nLogicServerID = 0;

	LMsgL2TS_Login() :LMsg(MSG_L_2_TS_LOGIN) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nLogicServerID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nLogicServerID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_Login(); }
};


// LogicServer 到 TeamServer 的心跳 [2017-7-10 willing]
struct LMsgL2TS_Heartbeat : public LMsg
{
	Lint nLogicServerID = 0;

	LMsgL2TS_Heartbeat() :LMsg(MSG_L_2_TS_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nLogicServerID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nLogicServerID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_Heartbeat(); }
};

// TeamServer 到 LogicServer 的心跳 [2017-7-10 willing]
struct LMsgTS2L_Heartbeat : public LMsg
{
	LMsgTS2L_Heartbeat() :LMsg(MSG_TS_2_L_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2L_Heartbeat(); }
};

// LogicManager 通知 LogicServer 链接 TeamServer [2017-7-11 willing]
struct LMsgLMG2L_TeamServer : public LMsg
{
	Lstring strTeamServerIp = "";
	Lshort  nTeamServerPort = 0;

	LMsgLMG2L_TeamServer() :LMsg(MSG_LMG_2_L_TEAMSERVER) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(strTeamServerIp);
		buff.Read(nTeamServerPort);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(strTeamServerIp);
		buff.Write(nTeamServerPort);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2L_TeamServer(); }
};

// LogicManager 通知 GateServer 链接 TeamServer [2017-7-11 willing]
struct LMsgLMG2G_TeamServer : public LMsg
{
	Lstring strTeamServerIp = "";
	Lshort  nTeamServerPort = 0;
	Lint	m_TeamServerID = 0;
	LMsgLMG2G_TeamServer() :LMsg(MSG_LMG_2_G_TEAMSERVER) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(strTeamServerIp);
		buff.Read(nTeamServerPort);
		buff.Read(m_TeamServerID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(strTeamServerIp);
		buff.Write(nTeamServerPort);
		buff.Write(m_TeamServerID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2G_TeamServer(); }
};

// GateServer 到 TeamServer 的心跳 [2017-7-11 willing]
struct LMsgG2TS_Heartbeat : public LMsg
{
	Lint nGateId = 0;
	LMsgG2TS_Heartbeat() :LMsg(MSG_G_2_TS_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nGateId);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nGateId);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgG2TS_Heartbeat(); }
};

// TeamServer 到 GateServer 的心跳 [2017-7-11 willing]
struct LMsgTS2G_Heartbeat : public LMsg
{
	LMsgTS2G_Heartbeat() :LMsg(MSG_TS_2_G_HEARTBEAT) {}
	virtual bool Read(LBuff& buff)
	{
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2G_Heartbeat(); }
};



// GateServer 登陆到 TeamServer [2017-7-11 willing]
struct LMsgG2TS_Login : public LMsg
{
	Lint nGateId = 0;
	LMsgG2TS_Login() :LMsg(MSG_G_2_TS_LOGIN) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nGateId);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nGateId);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgG2TS_Login(); }
};

// LogicManager 通知 TeamServer，Logic Gate 掉线 [2017-7-11 willing]
struct LMsgLMG2TS_LogicGateLogout : public LMsg
{
	Lint nIsLogic = 0;
	Lint nServerID = 0;

	LMsgLMG2TS_LogicGateLogout() :LMsg(MSG_LMG_2_TS_LG_LOGOUT) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nIsLogic);
		buff.Read(nServerID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nIsLogic);
		buff.Write(nServerID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_LogicGateLogout(); }
};

// LogicManager 通知 TeamServer 有玩家登陆 [2017-7-18 willing]
struct LMsgLMG2TS_UserLogin : public LMsg
{
	Llong nUserUnique = 0;		// 根据玩家所在的GateServer计算出的唯一ID [2017-7-18 willing]
	Lint nGateID = 0;				// 玩家所在的GateServerID [2017-7-18 willing]
	Lint nUserID = 0;			// 玩家的UserID [2017-7-18 willing]
	Llong nGateUserID = 0;
	Lint nOnUseTeamId = 0; //正在使用的teamid

	LMsgLMG2TS_UserLogin() :LMsg(MSG_LMG_2_TS_USER_LOGIN) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserUnique);
		buff.Read(nGateID);
		buff.Read(nUserID);
		buff.Read(nGateUserID);
		buff.Read(nOnUseTeamId);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserUnique);
		buff.Write(nGateID);
		buff.Write(nUserID);
		buff.Write(nGateUserID);
		buff.Write(nOnUseTeamId);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_UserLogin(); }
};

// TS返回 有玩家登陆 结果
struct LMsgTS2LMG_UserLogin : public LMsg
{
	Lint nUserID = 0;	// 玩家的UserID [2017-7-18 willing]
	Lint nOnUseTeamId = 0; //正在使用的teamid
	Lstring sOnUseTeamName; //正在使用的名字
	Lint nResCode = 0; //0 成功 1 以上是异常
	QueryTeamInfo teaminfo; //俱乐部自己的信息
	LMsgTS2LMG_UserLogin() :LMsg(MSG_TS_2_LMG_USER_LOGIN), nUserID(0), nOnUseTeamId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserID);
		buff.Read(nOnUseTeamId);
		buff.Read(sOnUseTeamName);
		buff.Read(nResCode);
		teaminfo.Read(buff);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserID);
		buff.Write(nOnUseTeamId);
		buff.Write(sOnUseTeamName);
		buff.Write(nResCode);
		teaminfo.Write(buff);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgTS2LMG_UserLogin();
	}
};

// TS返回 让玩家进入房间 结果
struct LMsgTS2LMG_UserInLogic : public LMsg
{
	Lint nUserID = 0;	// 玩家的UserID [2017-7-18 willing]
	Lint nOnUseTeamId = 0; //正在使用的teamid
	Lstring sOnUseTeamName; //正在使用的名字
	Lint nResCode = 0; //0 成功 1 以上是异常
	LMsgTS2LMG_UserInLogic() :LMsg(MSG_TS_2_LMG_USER_IN_LOGIC), nUserID(0), nOnUseTeamId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserID);
		buff.Read(nOnUseTeamId);
		buff.Read(sOnUseTeamName);
		buff.Read(nResCode);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserID);
		buff.Write(nOnUseTeamId);
		buff.Write(sOnUseTeamName);
		buff.Write(nResCode);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgTS2LMG_UserInLogic();
	}
};


// LogicManager 通知 TeamServer 有玩家登出 [2017-7-18 willing]
struct LMsgLMG2TS_UserLogout : public LMsg
{
	Llong nUserUnique = 0;		// 根据玩家所在的GateServer计算出的唯一ID [2017-7-18 willing]
	Lint nUserID = 0;			// 玩家的UserID [2017-7-18 willing]

	LMsgLMG2TS_UserLogout() :LMsg(MSG_LMG_2_TS_USER_LOGOUT) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserUnique);
		buff.Read(nUserID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserUnique);
		buff.Write(nUserID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_UserLogout(); }
};



//// LogicManager 通知 TeamServer 加入桌子 [2017-7-23 willing]
struct LMsgLMG2TS_AddDesk : public LMsg
{
	Lint			m_userID;					// 玩家ID [2017-7-23 willing]
	Lstring			m_location;					//玩家地理位置
	Lint			m_deskId;					// 工会ID [2017-7-23 willing]
	Ldouble         m_card_num = 0.0;                 //自己的金币
	LMsgLMG2TS_AddDesk() :LMsg(MSG_LMG_2_TS_ADD_ROOM)
	{
		m_userID = 0;				// 玩家ID [2017-7-23 willing]
		m_deskId = 0;					// 工会ID [2017-7-23 willing]
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userID);
		buff.Read(m_location);
		buff.Read(m_deskId);
		buff.Read(m_card_num);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userID);
		buff.Write(m_location);
		buff.Write(m_deskId);
		buff.Write(m_card_num);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2TS_AddDesk();
	}
};

//// LogicManager 通知 TeamServer 创建桌子 [2017-7-23 willing]
struct LMsgLMG2TS_CreateDesk : public LMsg
{
	Lint			m_userID;				// 玩家ID [2017-7-23 willing]
	Lint			m_playtypeCount;
	Lint			m_free;					// 该玩法是否免费,0 不免费，1 免费 [2017-8-1 willing]

	Lint		m_flag;						//房间类型，4圈，8圈，12圈
	Lstring		m_secret;					//房间密码，如果为空，服务器会随机一个密码
	Lint		m_gold;						//房间抵住，单位元，最低为1元，最多不能超过100
	Lint		m_state;					//玩法规则 0 转转   1 长沙	//贵州废弃
	Lint		m_robotNum;					// 0,不加机器人，1，2，3加机器人数量
	CardValue	m_cardValue[CARD_COUNT];	//配牌
	std::vector<Lint>	m_playtype;			//玩法
	Lint		m_up_down_ji;				//上下鸡
	Lint		m_all_ji;					//满堂鸡
	Lint		m_queNum[DEFAULT_DESK_USER_COUNT];	// 调试模式 设置的缺门 [2016-9-1 willing]
	Lint		m_lian_zhuang;				// 连庄是否算分 [2016-9-12 willing]
	Lint        m_zhuangPos;                // 调试模式设置庄的位置 [2016-10-27 willing]
	Lint		m_useTeamCard;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
	Lint		m_teamID;					// 工会ID [2017-7-23 willing]
	Lint		m_stake = 0;			    // 押注倍数
	Lint		m_circleGoldLimit = 0;
	Lint		m_playerNum = 0;		//游戏人数
	Lint		m_lenth = 0;            //调牌长度
	Lint		m_nMaxFanshu = 0;       //最大番薯
	LMsgLMG2TS_CreateDesk() :LMsg(MSG_LMG_2_TS_CREATE_ROOM)
	{
		m_userID = 0;				// 玩家ID [2017-7-23 willing]
		m_playtypeCount = 0;
		m_free = 0;

		m_flag = 0;						//房间类型，4圈，8圈，12圈
		m_secret = "";					//房间密码，如果为空，服务器会随机一个密码
		m_gold = 0;						//房间抵住，单位元，最低为1元，最多不能超过100
		m_state = 0;					//玩法规则 0 转转   1 长沙	//贵州废弃
		m_robotNum = 0;					// 0,不加机器人，1，2，3加机器人数量
		for (int i = 0; i < CARD_COUNT; i++)
		{
			m_cardValue[i].m_color = 0;
			m_cardValue[i].m_number = 0;
		}

		m_playtype.clear();			//玩法
		m_up_down_ji = 0;				//上下鸡
		m_all_ji = 0;					//满堂鸡
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			m_queNum[i] = 0;
		}
		m_lian_zhuang = 0;				// 连庄是否算分 [2016-9-12 willing]
		m_zhuangPos = 0;                // 调试模式设置庄的位置 [2016-10-27 willing]
		m_useTeamCard = 0;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
		m_teamID = 0;					// 工会ID [2017-7-23 willing]
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userID);
		buff.Read(m_playtypeCount);
		buff.Read(m_free);
		buff.Read(m_flag);
		buff.Read(m_secret);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		for (int i = 0; i < CARD_COUNT; i++)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}
		for (int i = 0; i < m_playtypeCount; i++)
		{
			Lint nTeam = 0;
			buff.Read(nTeam);
			m_playtype.push_back(nTeam);
		}
		buff.Read(m_up_down_ji);
		buff.Read(m_all_ji);
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			buff.Read(m_queNum[i]);
		}
		buff.Read(m_lian_zhuang);
		buff.Read(m_zhuangPos);
		buff.Read(m_useTeamCard);
		buff.Read(m_teamID);
		buff.Read(m_circleGoldLimit);
		buff.Read(m_playerNum);
		buff.Read(m_stake);
		buff.Read(m_lenth);
		buff.Read(m_nMaxFanshu);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_playtypeCount = m_playtype.size();

		buff.Write(m_userID);
		buff.Write(m_playtypeCount);
		buff.Write(m_free);
		buff.Write(m_flag);
		buff.Write(m_secret);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (int i = 0; i < CARD_COUNT; i++)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}
		for (int i = 0; i < m_playtypeCount; i++)
		{
			buff.Write(m_playtype[i]);
		}
		buff.Write(m_up_down_ji);
		buff.Write(m_all_ji);
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			buff.Write(m_queNum[i]);
		}
		buff.Write(m_lian_zhuang);
		buff.Write(m_zhuangPos);
		buff.Write(m_useTeamCard);
		buff.Write(m_teamID);
		buff.Write(m_circleGoldLimit);
		buff.Write(m_playerNum);
		buff.Write(m_stake);
		buff.Write(m_lenth);
		buff.Write(m_nMaxFanshu);
		
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2TS_CreateDesk();
	}
};

// TeamServer 通知 LogicManager 检查桌子的创建结果 [2017-7-23 willing]
struct LMsgTS2LMG_CreateDesk : public LMsg
{
	Lint			m_userID;				// 玩家ID [2017-7-23 willing]
	Lint			m_playtypeCount;
	Lint		m_free;						// 是否是免费房间，1 免费， 0 不免费 [2017-8-1 willing]

	Lint		m_flag;						//房间类型，4圈，8圈，12圈
	Lstring		m_secret;					//房间密码，如果为空，服务器会随机一个密码
	Lint		m_gold;						//房间抵住，单位元，最低为1元，最多不能超过100
	Lint		m_state;					//玩法规则 0 转转   1 长沙	//贵州废弃
	Lint		m_robotNum;					// 0,不加机器人，1，2，3加机器人数量
	CardValue	m_cardValue[CARD_COUNT];	//配牌
	std::vector<Lint>	m_playtype;			//玩法
	Lint		m_up_down_ji;				//上下鸡
	Lint		m_all_ji;					//满堂鸡
	Lint		m_queNum[DEFAULT_DESK_USER_COUNT];	// 调试模式 设置的缺门 [2016-9-1 willing]
	Lint		m_lian_zhuang;				// 连庄是否算分 [2016-9-12 willing]
	Lint        m_zhuangPos;                // 调试模式设置庄的位置 [2016-10-27 willing]
	Lint		m_useTeamCard;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
	Lint		m_teamID;					// 工会ID [2017-7-23 willing]
	Lstring		m_teamName;					// 工会名称 [2017-7-25 willing]
	Lint		m_maYouDesk;					//0:不是 1：快速组局 2:自由创建
	Lint		m_deskId;
	Lint		m_stake = 0;
	Lint		m_circleGoldLimit;          //每一圈的限制
	Lint		m_playerNum = 0; //人数

	Lint        m_nMaxFanshu = 0;       //最大翻倍数;
	Lint        m_black3first = 0;     //起手出牌是否可以不带黑桃3;1为可以不带，0必须带黑桃3;
	Lint		m_lenth = 0;			//发牌长度;
	Lint		m_proportionate = 0;    //抽成比例

	std::vector<Lint>	m_extValue;			//附加字段	;	

	LMsgTS2LMG_CreateDesk() :LMsg(MSG_TS_2_LMG_CREATE_ROOM)
	{
		m_userID = 0;				// 玩家ID [2017-7-23 willing]
		m_playtypeCount = 0;
		m_free = 0;

		m_flag = 0;						//房间类型，4圈，8圈，12圈
		m_secret = "";					//房间密码，如果为空，服务器会随机一个密码
		m_gold = 0;						//房间抵住，单位元，最低为1元，最多不能超过100
		m_state = 0;					//玩法规则 0 转转   1 长沙	//贵州废弃
		m_robotNum = 0;					// 0,不加机器人，1，2，3加机器人数量
		for (int i = 0; i < CARD_COUNT; i++)
		{
			m_cardValue[i].m_color = 0;
			m_cardValue[i].m_number = 0;
		}

		m_playtype.clear();			//玩法
		m_up_down_ji = 0;				//上下鸡
		m_all_ji = 0;					//满堂鸡
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			m_queNum[i] = 0;
		}
		m_lian_zhuang = 0;				// 连庄是否算分 [2016-9-12 willing]
		m_zhuangPos = 0;                // 调试模式设置庄的位置 [2016-10-27 willing]
		m_useTeamCard = 0;				// 0 不使用工会房卡创建房间，1使用工会房卡创建房间 [2017-7-21 willing]
		m_teamID = 0;					// 工会ID [2017-7-23 willing]
		m_teamName = "";
		m_maYouDesk = 0;
		m_deskId = 0;
		m_circleGoldLimit = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userID);
		buff.Read(m_playtypeCount);
		buff.Read(m_free);
		buff.Read(m_flag);
		buff.Read(m_secret);
		buff.Read(m_gold);
		buff.Read(m_state);
		buff.Read(m_robotNum);
		for (int i = 0; i < CARD_COUNT; i++)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}
		for (int i = 0; i < m_playtypeCount; i++)
		{
			Lint nTeam = 0;
			buff.Read(nTeam);
			m_playtype.push_back(nTeam);
		}
		buff.Read(m_up_down_ji);
		buff.Read(m_all_ji);
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			buff.Read(m_queNum[i]);
		}
		buff.Read(m_lian_zhuang);
		buff.Read(m_zhuangPos);
		buff.Read(m_useTeamCard);
		buff.Read(m_teamID);
		buff.Read(m_teamName);
		buff.Read(m_maYouDesk);
		buff.Read(m_deskId);
		buff.Read(m_stake);
		buff.Read(m_circleGoldLimit);
		buff.Read(m_playerNum);

		buff.Read(m_nMaxFanshu);
		buff.Read(m_black3first);
		buff.Read(m_lenth);
		buff.Read(m_proportionate);
		
		Lint size;
		buff.Read(size);

		Lint value = 0;
		for (Lint i = 0; i < size; ++i)
		{
			buff.Read(value);
			m_extValue.push_back(value);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_playtypeCount = m_playtype.size();

		buff.Write(m_userID);
		buff.Write(m_playtypeCount);
		buff.Write(m_free);
		buff.Write(m_flag);
		buff.Write(m_secret);
		buff.Write(m_gold);
		buff.Write(m_state);
		buff.Write(m_robotNum);
		for (int i = 0; i < CARD_COUNT; i++)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}
		for (int i = 0; i < m_playtypeCount; i++)
		{
			buff.Write(m_playtype[i]);
		}
		buff.Write(m_up_down_ji);
		buff.Write(m_all_ji);
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			buff.Write(m_queNum[i]);
		}
		buff.Write(m_lian_zhuang);
		buff.Write(m_zhuangPos);
		buff.Write(m_useTeamCard);
		buff.Write(m_teamID);
		buff.Write(m_teamName);
		buff.Write(m_maYouDesk);
		buff.Write(m_deskId);
		buff.Write(m_stake);
		buff.Write(m_circleGoldLimit);
		buff.Write(m_playerNum);

		buff.Write(m_nMaxFanshu);
		buff.Write(m_black3first);
		buff.Write(m_lenth);
		buff.Write(m_proportionate);

		Lint size = m_extValue.size();

		buff.Write(size);
		int value = 0;
		for (int i = 0; i < size; ++i)
		{
			value = m_extValue[i];
			buff.Write(value);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgTS2LMG_CreateDesk();
	}
};

// 通知TeamServer 退还 预扣的房卡 [2017-7-23 willing]
struct LMsgLMG2TS_GiveBackCard : public LMsg
{
	Lint nTeamID = 0;
	Lint nUserID = 0;
	Lint nFlag = 0;
	Lint nFastPlay = 0;
	LMsgLMG2TS_GiveBackCard() :LMsg(MSG_LMG_2_TS_GIVE_BACK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nUserID);
		buff.Read(nFlag);
		buff.Read(nFastPlay);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(nUserID);
		buff.Write(nFlag);
		buff.Write(nFastPlay);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_GiveBackCard(); }
};

// LogicServer 通知 TeamServer 真正扣卡 [2017-7-10 willing]
struct LMsgL2TS_DelCard : public LMsg
{
	Lint nTeamID = 0;
	Lint nFlag = 0;
	Lint nUserID = 0;

	LMsgL2TS_DelCard() :LMsg(MSG_L_2_TS_DEL_CARD) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nFlag);
		buff.Read(nUserID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(nFlag);
		buff.Write(nUserID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_DelCard(); }
};

// LogicServer 通知 TeamServer 记录 牌局日志[2017-7-23 willing]
struct LMsgL2TS_VideoLog : public LMsg
{
	Lint nTeamID;
	Lint nDeskID;
	Lint nFlag;
	Lint m_maxCircle;
	Lint nUserID[DEFAULT_DESK_USER_COUNT];
	Ldouble nScore[DEFAULT_DESK_USER_COUNT];

	LMsgL2TS_VideoLog() :LMsg(MSG_L_2_TS_VIDEO_LOG), nTeamID(0), nDeskID(0), nFlag(0), m_maxCircle(0)
	{
		memset(nUserID, 0, sizeof(nUserID));
		memset(nScore, 0, sizeof(nScore));
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nDeskID);
		buff.Read(nFlag);
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			buff.Read(nUserID[i]);
			buff.Read(nScore[i]);
		}
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(nDeskID);
		buff.Write(nFlag);
		for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)
		{
			buff.Write(nUserID[i]);
			buff.Write(nScore[i]);
		}
		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_VideoLog(); }
};

// LogicServer 通知 TeamServer 有玩家请求加入桌子[2017-7-24 willing]
struct LMsgL2TS_AddDesk : public LMsg
{
	Lint nTeamID = 0;

	Lint			m_userid = 0;
	Lint			m_gateId = 0;
	Lint			m_geteUserId = 0;
	Lint			m_deskID = 0;
	Lstring			m_ip;
	LUser			m_usert;
	Lstring			m_location;

	LMsgL2TS_AddDesk() :LMsg(MSG_L_2_TS_ADD_DESK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_location);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_location);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_AddDesk(); }
};

// TeamServer 通知 LogicServer 是否可以加入桌子[2017-7-23 willing]
struct LMsgTS2L_AddDeskRet : public LMsg
{
	Lint nTeamID = 0;
	Lint nRet = 0;

	Lint			m_userid = 0;
	Lint			m_gateId = 0;
	Lint			m_geteUserId = 0;
	Lint			m_deskID = 0;
	Lstring			m_ip;
	LUser			m_usert;
	Lstring			m_location;

	LMsgTS2L_AddDeskRet() :LMsg(MSG_TS_2_L_ADD_DESK_RET) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nRet);
		buff.Read(m_userid);
		buff.Read(m_gateId);
		buff.Read(m_geteUserId);
		buff.Read(m_deskID);
		buff.Read(m_ip);
		m_usert.Read(buff);
		buff.Read(m_location);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(nRet);
		buff.Write(m_userid);
		buff.Write(m_gateId);
		buff.Write(m_geteUserId);
		buff.Write(m_deskID);
		buff.Write(m_ip);
		m_usert.Write(buff);
		buff.Write(m_location);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2L_AddDeskRet(); }
};

struct STeamVideoLogS2S
{
	Lint nTime = 0;
	Lint nDeskID = 0;
	Lint nUserID[DEFAULT_DESK_USER_COUNT] = { 0 };
	Lstring strUserName[DEFAULT_DESK_USER_COUNT];
	Lint nScore[DEFAULT_DESK_USER_COUNT] = { 0 };
	Lint nOptState = 0;
	Lint	m_flag = -1;
};

// TeamServer 把玩家获取的 工会战绩信息发送给 LogicManager，信息还要做填充 [2017-7-24 willing]
struct LMsgTS2LMG_TeamVideoLog : public LMsg
{
	Lint nPackageIndex = 0;
	Lint nRequestUserID = 0;
	Lint nTeamID = 0;
	Lint nLogCount = 0;
	std::vector<STeamVideoLogS2S> teamVideoLog;

	LMsgTS2LMG_TeamVideoLog() :LMsg(MSG_TS_2_LMG_TEAM_VIDEO_LOG) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nPackageIndex);
		buff.Read(nRequestUserID);
		buff.Read(nTeamID);
		buff.Read(nLogCount);

		for (int i = 0; i < nLogCount; i++)
		{
			STeamVideoLogS2S log;
			buff.Read(log.nTime);
			buff.Read(log.nDeskID);

			buff.Read(log.nUserID[0]);
			buff.Read(log.nUserID[1]);
			buff.Read(log.nUserID[2]);
			buff.Read(log.nUserID[3]);

			buff.Read(log.strUserName[0]);
			buff.Read(log.strUserName[1]);
			buff.Read(log.strUserName[2]);
			buff.Read(log.strUserName[3]);

			buff.Read(log.nScore[0]);
			buff.Read(log.nScore[1]);
			buff.Read(log.nScore[2]);
			buff.Read(log.nScore[3]);

			teamVideoLog.push_back(log);
		}

		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nPackageIndex);
		buff.Write(nRequestUserID);
		buff.Write(nTeamID);

		nLogCount = teamVideoLog.size();

		buff.Write(nLogCount);

		for (int i = 0; i < nLogCount; i++)
		{
			STeamVideoLogS2S& log = teamVideoLog[i];
			buff.Write(log.nTime);
			buff.Write(log.nDeskID);

			buff.Write(log.nUserID[0]);
			buff.Write(log.nUserID[1]);
			buff.Write(log.nUserID[2]);
			buff.Write(log.nUserID[3]);

			buff.Write(log.strUserName[0]);
			buff.Write(log.strUserName[1]);
			buff.Write(log.strUserName[2]);
			buff.Write(log.strUserName[3]);

			buff.Write(log.nScore[0]);
			buff.Write(log.nScore[1]);
			buff.Write(log.nScore[2]);
			buff.Write(log.nScore[3]);
		}
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2LMG_TeamVideoLog(); }
};

// LogicServer 把玩家进入游戏，离开游戏 的状态发送给 TeamServer [2017-7-25 willing]
struct LMsgLS2TS_GameState : public LMsg
{
	Lint nUserID = 0;
	Lint nIsInGame = 0;				// 是否在游戏中 [2017-7-25 willing]
	Lint nPos = DEFAULT_DESK_USER_COUNT;		// 玩家在桌子中的位置 [2017-8-7 willing]
	LMsgLS2TS_GameState() :LMsg(MSG_L_2_TS_GAME_STATE) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserID);
		buff.Read(nIsInGame);
		buff.Read(nPos);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserID);
		buff.Write(nIsInGame);
		buff.Write(nPos);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLS2TS_GameState(); }
};

// TeamServer 获取玩家的昵称和URL [2017-7-26 willing]
struct LMsgTS2LMG_GetUserInfo : public LMsg
{
	Lint nUserID = 0;

	LMsgTS2LMG_GetUserInfo() :LMsg(MSG_TS_2_LMG_GET_USER_INFO) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2LMG_GetUserInfo(); }
};

// LogicManager 返回玩家信息 [2017-7-26 willing]
struct LMsgLMG2TS_UserInfo : public LMsg
{
	Lint nUserID = 0;
	Lstring strUserNick = "";
	Lstring strHeadImageUrl = "";

	LMsgLMG2TS_UserInfo() :LMsg(MSG_LMG_2_TS_USER_INFO) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserID);
		buff.Read(strUserNick);
		buff.Read(strHeadImageUrl);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserID);
		buff.Write(strUserNick);
		buff.Write(strHeadImageUrl);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_UserInfo(); }
};

// LogicServer 创建了一个新的俱乐部桌子 [2017-8-3 willing]
struct LMsgL2TS_NewTeamDesk : public LMsg
{
	Lint nTeamID = 0;
	Lint nDeskID = 0;
	Lint nCreaterUserID = 0;						// 房间创建者的ID [2017-8-3 willing]
	Lint nMajongType = 0;							// 玩法名称 [2017-8-2 willing]
	Lint nMaxCircle = 0;							// 最大局数 [2017-8-3 willing]
	Lint nMaxUserCount = 0;							// 桌子的最大玩家数量 [2017-8-4 willing]
	Lint nUpDownJi = 0;								// 上下鸡
	Lint nAllJi = 0;								// 满堂鸡
	Lint nLianZhuang = 0;							// 连庄是否算分 [2016-9-12 willing]
	Lint nPlayTypeCount = 0;						// 小玩法数量 [2017-8-3 willing]
	std::vector<Lint>	tPlayType;				// 小玩法选项 [2017-8-2 willing]
	Lint m_fastDesk = 0;							//0：不是  1：是快速组局
	Lint nCircleGoldLimit = 0;                       //当前圈的限制
	Lint nStake = 0;                                 //押注倍率
	Lint nMaxFanshu = 0;                             //最大炸上限
	LMsgL2TS_NewTeamDesk() :LMsg(MSG_L_2_TS_NEW_TEAM_DESK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nDeskID);
		buff.Read(nCreaterUserID);
		buff.Read(nMajongType);
		buff.Read(nMaxCircle);
		buff.Read(nMaxUserCount);
		buff.Read(nUpDownJi);
		buff.Read(nAllJi);
		buff.Read(nLianZhuang);
		buff.Read(nPlayTypeCount);

		for (int i = 0; i < nPlayTypeCount; i++)
		{
			int nTeam = 0;
			buff.Read(nTeam);
			tPlayType.push_back(nTeam);
		}
		buff.Read(m_fastDesk);
		buff.Read(nCircleGoldLimit);
		buff.Read(nStake);
		buff.Read(nMaxFanshu);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		nPlayTypeCount = tPlayType.size();

		buff.Write(nTeamID);
		buff.Write(nDeskID);
		buff.Write(nCreaterUserID);
		buff.Write(nMajongType);
		buff.Write(nMaxCircle);
		buff.Write(nMaxUserCount);
		buff.Write(nUpDownJi);
		buff.Write(nAllJi);
		buff.Write(nLianZhuang);
		buff.Write(nPlayTypeCount);

		for (int i = 0; i < nPlayTypeCount; i++)
		{
			buff.Write(tPlayType[i]);
		}
		buff.Write(m_fastDesk);
		buff.Write(nCircleGoldLimit);
		buff.Write(nStake);
		buff.Write(nMaxFanshu);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_NewTeamDesk(); }
};

// LogicServer 通知桌子人员改变 [2017-8-3 willing]
struct LMsgL2TS_TeamDeskUserChg : public LMsg
{
	Lint nTeamID = 0;
	Lint nDeskID = 0;
	Lint nUserID = 0;
	Lint nJoinOrLeave = 0;		// 1 加入桌子，0 离开桌子 [2017-8-3 willing]

	LMsgL2TS_TeamDeskUserChg() :LMsg(MSG_L_2_TS_TEAM_DESK_USER_CHG) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nDeskID);
		buff.Read(nUserID);
		buff.Read(nJoinOrLeave);

		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(nDeskID);
		buff.Write(nUserID);
		buff.Write(nJoinOrLeave);

		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_TeamDeskUserChg(); }
};

// LogicServer 通知删除桌子 [2017-8-3 willing]
struct LMsgL2TS_DelTeamDesk : public LMsg
{
	Lint nTeamID = 0;
	Lint nDeskID = 0;

	LMsgL2TS_DelTeamDesk() :LMsg(MSG_L_2_TS_DEL_TEAM_DESK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nTeamID);
		buff.Read(nDeskID);

		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nTeamID);
		buff.Write(nDeskID);

		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2TS_DelTeamDesk(); }
};

// TeamServer 获取玩家是否在线 [2017-7-26 willing]
struct LMsgTS2LMG_QueryIsOnLine : public LMsg
{
	Lint nUserID = 0;

	LMsgTS2LMG_QueryIsOnLine() :LMsg(MSG_TS_2_LMG_QUERY_IS_ONLINE) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserID);

		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserID);

		return true;
	}
	virtual LMsg* Clone() { return new LMsgTS2LMG_QueryIsOnLine(); }
};

// LogicManager 返回玩家的在线状态 [2017-7-26 willing]
struct LMsgLMG2TS_UserOnLineRet : public LMsg
{
	Lint nUserID = 0;
	Lint nOnLine = 0;		// 1 在线，0 离线 [2017-8-4 willing]

	LMsgLMG2TS_UserOnLineRet() :LMsg(MSG_LMG_2_TS_USER_ONLINE_RET) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nUserID);
		buff.Read(nOnLine);

		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nUserID);
		buff.Write(nOnLine);

		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_UserOnLineRet(); }
};

// Gate 发送到 TeamServer 的消息 [2017-7-18 willing]
struct LMsgG2TUserMsg :public LMsg
{
	Llong			m_userGateId;		// 玩家所在GateServerID [2017-7-18 willing]
	Lint			m_gateID;			// GateServer 的 ID [2017-7-18 willing]
	Lint			m_userMsgId;
	Lstring			m_ip;
	LBuffPtr		m_dataBuff;
	LMsg*			m_userMsg;

	LMsgG2TUserMsg() :LMsg(MSG_G_2_T_USER_MSG), m_userMsg(NULL), m_userGateId(0), m_userMsgId(0), m_gateID(0)
	{
	}

	virtual~LMsgG2TUserMsg()
	{
		if (m_userMsg)
			delete m_userMsg;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userGateId);
		buff.Read(m_gateID);
		buff.Read(m_userMsgId);
		buff.Read(m_ip);

		int msgid = MSG_ERROR_MSG;

		try {
			msgpack::unpacked  unpack;
			msgpack::unpack(&unpack, buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
			msgpack::object obj = unpack.get();

			ReadMapData(obj, "m_msgId", msgid);
			m_userMsg = LMsgFactory::Instance().CreateMsg(m_userMsgId);
			if (m_userMsg)
			{
				m_userMsg->Read(obj);
			}
			else
			{
				LLOG_ERROR("LMsgG2LUserMsg read msgId not exiest %d", m_userMsgId);
			}
		}
		catch (...)
		{
			LLOG_ERROR("MSG_G_2_L_USER_MSG::RecvMsgPack error");
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userGateId);
		buff.Write(m_gateID);
		buff.Write(m_userMsgId);
		buff.Write(m_ip);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgG2TUserMsg();
	}
};

struct LMsgT2GUserMsg : public LMsg
{
	Llong			m_id;
	LBuffPtr		m_dataBuff;

	LMsgT2GUserMsg() :LMsg(MSG_T_2_G_USER_MSG), m_id(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		m_dataBuff = LBuffPtr(new LBuff);
		m_dataBuff->Write(buff.Data() + buff.GetOffset(), buff.Size() - buff.GetOffset());
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_dataBuff->Data() + m_dataBuff->GetOffset(), m_dataBuff->Size() - m_dataBuff->GetOffset());
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgT2GUserMsg();
	}
};
// LogicManager 通知 TeamServer 回收了桌子 [2017-7-26 willing]
struct LMsgLMG2TS_RecycleDeskId : public LMsg
{
	Lint nDeskId = 0;

	LMsgLMG2TS_RecycleDeskId() :LMsg(MSG_LMG_2_TS_RecycleDeskId) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(nDeskId);

		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(nDeskId);

		return true;
	}
	virtual LMsg* Clone() { return new LMsgLMG2TS_RecycleDeskId(); }
};

// TeamServer 通知 LogicManager 检查桌子的加入结果 
struct LMsgTS2LMG_AddDesk : public LMsg
{
	Lint			m_userID;				// 玩家ID 
	Lstring			m_location;					//玩家地理位置
	std::vector<Lint> m_deskIds;     //工会ID s
	std::vector<Lint> m_limitGolds;   //限制进入的金币
	LMsgTS2LMG_AddDesk() :LMsg(MSG_TS_2_LMG_ADD_ROOM), m_userID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userID);
		buff.Read(m_location);

		Lint deskids = 0;
		buff.Read(deskids);
		Lint limits = 0;
		buff.Read(limits);

		m_deskIds.resize(deskids);
		for (Lint i = 0; i < deskids; ++i)
		{
			buff.Read(m_deskIds[i]);
		}

		m_limitGolds.resize(limits);
		for (Lint i = 0; i < limits; ++i)
		{
			buff.Read(m_limitGolds[i]);
		}

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userID);
		buff.Write(m_location);
		Lint deskids = m_deskIds.size();
		buff.Write(deskids);
		Lint limits = m_limitGolds.size();
		buff.Write(limits);

		for (Lint i = 0; i < deskids; ++i)
		{
			buff.Write(m_deskIds[i]);
		}

		for (Lint i = 0; i < limits; ++i)
		{
			buff.Write(m_limitGolds[i]);
		}

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgTS2LMG_AddDesk();
	}
};
struct LMsgTS2LMG_ReqDeskId : public LMsg
{
	LMsgTS2LMG_ReqDeskId() :LMsg(MSG_TS_2_LMG_REQ_DESKID)
	{
	}

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
		return new LMsgTS2LMG_ReqDeskId();
	}
};
struct LMsgLMG2TS_ReqDeskRet : public LMsg
{
	int						m_IdCount;
	std::vector<int>		m_deskVec;
	LMsgLMG2TS_ReqDeskRet() :LMsg(MSG_LMG_2_TS__REQ_DESKID_REQ), m_IdCount(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_IdCount);
		for (int i = 0; i < m_IdCount; i++)
		{
			int deskId = 0;
			buff.Read(deskId);
			m_deskVec.push_back(deskId);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		m_IdCount = m_deskVec.size();
		buff.Write(m_IdCount);
		for (int i = 0; i < m_IdCount; i++)
		{
			buff.Write(m_deskVec[i]);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2TS_ReqDeskRet();
	}
};
struct TeamServerInfo
{
	Lint			m_nTeamServerID = 0;
	Lstring			m_strTeamServerIP;
	Lshort			m_sTeamServerPort = 0;
	//LSocketPtr		m_TeamServerSocket;		//LM专用的,
	TCPClientPtr	m_TeamServerTcpClient;
};
//MaYouQuan end


struct LMsgLMG2LModifyRoomCard :public LMsg
{
	Lint		m_userid;
	Ldouble		m_card_num;	//房卡数 （Card2）
	Lint		m_oper; //操作类型
	LMsgLMG2LModifyRoomCard() :LMsg(MSG_LMG_2_L_MODIFY_ROOM_CARD), m_userid(0), m_card_num(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userid);
		buff.Read(m_card_num);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userid);
		buff.Write(m_card_num);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LModifyRoomCard();
	}
};

// TeamServer 通知 LogicManager 兑换金币
struct LMsgTS2LMG_ReqExchageGold : public LMsg
{
	Lint			m_userID;				// 玩家ID 
	Lint		    m_toUserId = 0;
	Lint		    m_goldNum = 0;
	LMsgTS2LMG_ReqExchageGold() :LMsg(MSG_TS_2_LMG_REQ_EXCHAGEGOLD), m_userID(0), m_toUserId(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userID);
		buff.Read(m_toUserId);
		buff.Read(m_goldNum);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userID);
		buff.Write(m_toUserId);
		buff.Write(m_goldNum);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgTS2LMG_ReqExchageGold();
	}
};


// LogicManager 通知 TeamServer 兑换金币
//MSG_LMG_2_TS_REQ_EXCHAGEGOLDM = 40039,   //LM返回 TS请求兑换金币
struct LMsgLMG2TS_ReqExchageGold : public LMsg
{
	SExchageGoldOptLog m_logMsg;

	LMsgLMG2TS_ReqExchageGold() :LMsg(MSG_LMG_2_TS_REQ_EXCHAGEGOLDM)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_logMsg.lLogId);
		buff.Read(m_logMsg.nFromUserId);
		buff.Read(m_logMsg.nToUserId);
		buff.Read(m_logMsg.nGoldNum);
		buff.Read(m_logMsg.sFromName);
		buff.Read(m_logMsg.sToName);
		buff.Read(m_logMsg.nOpt);
		buff.Read(m_logMsg.nReqTime);
		buff.Read(m_logMsg.nProTime);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_logMsg.lLogId);
		buff.Write(m_logMsg.nFromUserId);
		buff.Write(m_logMsg.nToUserId);
		buff.Write(m_logMsg.nGoldNum);
		buff.Write(m_logMsg.sFromName);
		buff.Write(m_logMsg.sToName);
		buff.Write(m_logMsg.nOpt);
		buff.Write(m_logMsg.nReqTime);
		buff.Write(m_logMsg.nProTime);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2TS_ReqExchageGold();
	}
};

enum
{
	USER_JOINMATCH_JOIN = 1,
	USER_JOINMATCH_OVER = 2,
	USER_JOINMATCH_QUIT = 3,
};

struct LMsgMa2LM_UserMatchInfo : public LMsg
{
	int		m_Type = 0;//类型 1,参加,  2结束, 3放弃
	int		m_nUserDBID = 0;
	int		m_nMatchID = 0;
	int     m_nIsFreeJoin = 0; //是否是免费报名 
	int     m_nServerID = 0;
	LMsgMa2LM_UserMatchInfo() :LMsg(MSG_MA_2_LM_USER_MATCH_INFO) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_Type);
		buff.Read(m_nUserDBID);
		buff.Read(m_nMatchID);
		buff.Read(m_nIsFreeJoin);
		buff.Read(m_nServerID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_Type);
		buff.Write(m_nUserDBID);
		buff.Write(m_nMatchID);
		buff.Write(m_nIsFreeJoin);
		buff.Write(m_nServerID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgMa2LM_UserMatchInfo(); }
};


////////////////////////////长沙麻将 begin////////////////////////////


//比赛结果返回
//MSG_L_2_LMG_SPORT_RESULT = 11031,

//LogServer 比赛结果返回
struct LMsgL2LMGSportResult :public LMsg
{
	Lint m_logicId; //具体逻辑服的id
	Lint m_rescode; //0 正常返回，1 等待超时
	Lint m_deskid;  //桌子号
	Lint m_desktype; //桌子类型

	Lint m_uid1; //第1个玩家id
	Lint m_uid2; //第2个玩家id
	Lint m_uid3; //第3个玩家id
	Lint m_uid4; //第4个玩家id

	Lint m_score1; //第1玩家的积分
	Lint m_score2; //第2玩家积分
	Lint m_score3; //第3玩家的积分
	Lint m_score4; //第4玩家的积分

	LMsgL2LMGSportResult() :LMsg(MSG_L_2_LMG_SPORT_RESULT)
	{
		m_logicId = 0;
		m_rescode = 0;
		m_deskid = 0;
		m_desktype = 0;
		m_uid1 = 0;
		m_uid2 = 0;
		m_uid3 = 0;
		m_uid4 = 0;
		m_score1 = 0;
		m_score2 = 0;
		m_score3 = 0;
		m_score4 = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_logicId);
		buff.Read(m_rescode);
		buff.Read(m_deskid);
		buff.Read(m_desktype);
		buff.Read(m_uid1);
		buff.Read(m_uid2);
		buff.Read(m_uid3);
		buff.Read(m_uid4);
		buff.Read(m_score1);
		buff.Read(m_score2);
		buff.Read(m_score3);
		buff.Read(m_score4);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_logicId);
		buff.Write(m_rescode);
		buff.Write(m_deskid);
		buff.Write(m_desktype);
		buff.Write(m_uid1);
		buff.Write(m_uid2);
		buff.Write(m_uid3);
		buff.Write(m_uid4);
		buff.Write(m_score1);
		buff.Write(m_score2);
		buff.Write(m_score3);
		buff.Write(m_score4);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGSportResult();
	}
};


//请求 解散房间
//MSG_LMG_2_CE_SPORT_RESET_DESK = 6422

struct LMsgLMG2CESportResetDesk : public LMsg
{
	Lint m_uDeskid;

	LMsgLMG2CESportResetDesk() : LMsg(MSG_LMG_2_CE_SPORT_RESET_DESK)
	{
		m_uDeskid = 0;
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_uDeskid);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_uDeskid);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2CESportResetDesk();
	}
};


//MSG_LMG_2_L_MATCH_ADD_DESK = 12020,		//比赛场创建房间
struct LMsgLMG2LMatchAddDesk : public LMsg
{
	Lint m_add_count;
	LMsgLMG2LCreateDesk m_create;
	std::vector<LMsgLMG2LAddToDesk> m_adds;

	LMsgLMG2LMatchAddDesk() :LMsg(MSG_LMG_2_L_MATCH_ADD_DESK), m_add_count(0)
	{
		m_adds.clear();
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_add_count);
		m_create.Read(buff);

		for (int i = 0; i < m_add_count; ++i)
		{
			LMsgLMG2LAddToDesk tmp_add;
			tmp_add.Read(buff);
			m_adds.push_back(tmp_add);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_add_count);
		m_create.Write(buff);

		int sz = m_adds.size();
		for (int i = 0; i < sz; ++i)
		{
			m_adds[i].Write(buff);
		}
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LMatchAddDesk();
	}
};

struct MatchInfo
{
	Lint			m_id;
	Lstring			m_ip;
	Lshort			m_port;
	LSocketPtr		m_sp;
	TCPClientPtr	m_client;
	Lint			m_flag;	//0,normal; -1,failover
	MatchInfo() :m_id(0), m_port(0) {}
};

struct LMsgLMG2LogicMatchInfo :public LMsg
{
	Lint		m_ID;
	MatchInfo	m_match;

	LMsgLMG2LogicMatchInfo() :LMsg(MSG_LMG_2_L_SYNC_MATCH), m_ID(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ID);
		buff.Read(m_match.m_id);
		buff.Read(m_match.m_ip);
		buff.Read(m_match.m_port);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ID);
		buff.Write(m_match.m_id);
		buff.Write(m_match.m_ip);
		buff.Write(m_match.m_port);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgLMG2LogicMatchInfo();
	}
};


struct LMsgL2MLogin :public LMsg
{
	Lint		m_id;
	Lstring		m_key;
	Lstring		m_ip;
	Lshort		m_port;
	Lint		m_firstLogin;

	LMsgL2MLogin() :LMsg(MSG_L_2_M_LOGIN), m_id(0), m_port(0), m_firstLogin(0)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_id);
		buff.Read(m_key);
		buff.Read(m_ip);
		buff.Read(m_port);
		buff.Read(m_firstLogin);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_id);
		buff.Write(m_key);
		buff.Write(m_ip);
		buff.Write(m_port);
		buff.Write(m_firstLogin);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2MLogin();
	}
};


struct LMsgL2LMGUserCompleteGame : public LMsg
{
	Lint			m_userId;
	Lint			m_startTime;
	Lint			m_totalCircleNum;
	Lint			m_playerNum;
	Lint			m_players[DESK_USER_COUNT];

	LMsgL2LMGUserCompleteGame() : LMsg(MSG_L_2_LMG_USER_COMPLETE_GAME) {}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_userId);
		buff.Read(m_startTime);
		buff.Read(m_totalCircleNum);
		buff.Read(m_playerNum);
		for (int i = 0; i < m_playerNum && i < DESK_USER_COUNT; i++)
			buff.Read(m_players[i]);
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_userId);
		buff.Write(m_startTime);
		buff.Write(m_totalCircleNum);
		buff.Write(m_playerNum);
		for (int i = 0; i < m_playerNum && i < DESK_USER_COUNT; i++)
			buff.Write(m_players[i]);
		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGUserCompleteGame();
	}
};


////////////////////////////长沙麻将 end///////////////////////////////

////////////////////////////广东麻将 /////////////////////////////////
struct LMsg_Match2L_CreateMatchDesk : public LMsg
{
	Lint				m_deskID = 0;
	Lint				m_nMatchID = 0;
	Lint				m_BigPlayType = 0;	//大玩法 
	Lint				m_UserCount = 4;
	Lint				m_MaxCricle = 0;
	Lstring				m_MatchName;		//比赛的名字
	Lint				m_nCurStage = 1;	//第几轮
	CardValue			m_cardValue[CARD_COUNT];

	int					m_nGroupID = 0;
	Lint				m_playTypeCount = 0;
	std::vector<Lint>	m_playtype;			//小玩法
	Lint                m_nForceFinishTime = 0; //比赛场比赛强退时间 s，用于分散比赛结束造成的压力
	int                 m_IntoTuoGuanRemainTime = 15000;    //未托管进入托管状态剩余时间
	int                 m_InTuoGuanCanChuPaiTime = 3000;   //托管状态下可出牌时间
	Lint                m_MatchStartTime = 0;   //比赛开始时间
	Lint                m_CreaterID = 0;			//创建人的ID
	Lint                m_MatchType = 0;           //循环赛 专赛 定点赛 广东周年赛
	Lint				m_canInteraction = 0;		   //玩家之间是否可以互动扔鸡蛋送鲜花,默认开启
	int                 m_nInitualDifen = 500;               //初始底分倍率
	int                 m_nDifenPerMinIncStep = 500;         //底分每分钟增长步长
	int                 m_nDingJuInitualDifen = 10;      //定局积分阶段的底分

	Lint				m_userid[4] = { 0 };
	Lint				m_gateId[4] = { 0 };
	bool				m_IsOnline[4] = { 0 };
	Llong				m_geteUserId[4] = { 0 };
	Lint				m_Gold[4] = { 0 };
	LUser				m_usert[4];
	Lstring				m_ip[4];
	Lstring				m_location[4];

	LMsg_Match2L_CreateMatchDesk() :LMsg(MSG_MA_2_L_CREATE_MATCHDESK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_deskID);
		buff.Read(m_nMatchID);
		buff.Read(m_BigPlayType);
		buff.Read(m_UserCount);
		buff.Read(m_MaxCricle);
		buff.Read(m_MatchName);
		buff.Read(m_nCurStage);
		buff.Read(m_nGroupID);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Read(m_cardValue[i].m_color);
			buff.Read(m_cardValue[i].m_number);
		}
		buff.Read(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			Lint playType = 0;
			buff.Read(playType);
			m_playtype.push_back(playType);
		}
		buff.Read(m_nForceFinishTime);
		buff.Read(m_IntoTuoGuanRemainTime);
		buff.Read(m_InTuoGuanCanChuPaiTime);
		buff.Read(m_MatchStartTime);
		buff.Read(m_CreaterID);
		buff.Read(m_MatchType);
		buff.Read(m_canInteraction);
		buff.Read(m_nInitualDifen);
		buff.Read(m_nDifenPerMinIncStep);
		buff.Read(m_nDingJuInitualDifen);
		for (int i = 0; i < 4; ++i)
		{
			buff.Read(m_userid[i]);
			buff.Read(m_gateId[i]);
			buff.Read(m_geteUserId[i]);
			buff.Read(m_IsOnline[i]);
			buff.Read(m_Gold[i]);
			buff.Read(m_ip[i]);
			m_usert[i].Read(buff);
			buff.Read(m_location[i]);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_deskID);
		buff.Write(m_nMatchID);
		buff.Write(m_BigPlayType);
		buff.Write(m_UserCount);
		buff.Write(m_MaxCricle);
		buff.Write(m_MatchName);
		buff.Write(m_nCurStage);
		buff.Write(m_nGroupID);
		for (Lint i = 0; i < CARD_COUNT; ++i)
		{
			buff.Write(m_cardValue[i].m_color);
			buff.Write(m_cardValue[i].m_number);
		}
		m_playTypeCount = m_playtype.size();
		buff.Write(m_playTypeCount);
		for (Lint i = 0; i < m_playTypeCount; i++)
		{
			buff.Write(m_playtype[i]);
		}
		buff.Write(m_nForceFinishTime);
		buff.Write(m_IntoTuoGuanRemainTime);
		buff.Write(m_InTuoGuanCanChuPaiTime);
		buff.Write(m_MatchStartTime);
		buff.Write(m_CreaterID);
		buff.Write(m_MatchType);
		buff.Write(m_canInteraction);
		buff.Write(m_nInitualDifen);
		buff.Write(m_nDifenPerMinIncStep);
		buff.Write(m_nDingJuInitualDifen);
		for (int i = 0; i < 4; ++i)
		{
			buff.Write(m_userid[i]);
			buff.Write(m_gateId[i]);
			buff.Write(m_geteUserId[i]);
			buff.Write(m_IsOnline[i]);
			buff.Write(m_Gold[i]);
			buff.Write(m_ip[i]);
			m_usert[i].Write(buff);
			buff.Write(m_location[i]);
		}
		return true;
	}
	virtual LMsg* Clone() { return new LMsg_Match2L_CreateMatchDesk(); }
};

//发向LM的MatchServer本身的信息
//比赛场
struct MatchServerInfo
{
	Lint			m_nMatchServerID = 0;
	Lstring			m_strMatchServerIP;
	Lshort			m_sMatchServerPort = 0;
	LSocketPtr		m_MatchServerSocket;		//LM专用的,
	TCPClientPtr	m_MatchServerTcpClient;		//Gate,LS用.
};
//LM通知Gate,LS 比赛服信息
struct LMsgLM2Gate2Logic_MatchServerInfo :public LMsg
{
	Lint			m_count = 0;
	MatchServerInfo	m_InfoVec[256];
	LMsgLM2Gate2Logic_MatchServerInfo() :LMsg(MSG_LM_2_G_2LS_SYNC_MATCHSERVER) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_count);
		if (m_count > 256)
		{
			m_count = 256;
		}
		for (Lint i = 0; i < m_count; ++i)
		{
			buff.Read(m_InfoVec[i].m_nMatchServerID);
			buff.Read(m_InfoVec[i].m_strMatchServerIP);
			buff.Read(m_InfoVec[i].m_sMatchServerPort);
		}
		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_count);
		for (Lint i = 0; i < m_count && i < 256; ++i)
		{
			buff.Write(m_InfoVec[i].m_nMatchServerID);
			buff.Write(m_InfoVec[i].m_strMatchServerIP);
			buff.Write(m_InfoVec[i].m_sMatchServerPort);
		}
		return true;
	}
	virtual LMsg* Clone()
	{
		return new LMsgLM2Gate2Logic_MatchServerInfo();
	}
};

struct LMsg_L2Match_RetCreateMatchDesk : public LMsg
{
	int			m_errNo = 0;
	int			m_nMatchID = 0;
	int			m_deskID = 0;
	int			m_nGroupID = 0;
	LMsg_L2Match_RetCreateMatchDesk() :LMsg(MSG_L_2_MA_RETCREATE_MATCHDESK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_errNo);
		buff.Read(m_nMatchID);
		buff.Read(m_deskID);
		buff.Read(m_nGroupID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_errNo);
		buff.Write(m_nMatchID);
		buff.Write(m_deskID);
		buff.Write(m_nGroupID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsg_L2Match_RetCreateMatchDesk(); }
};

enum
{
	MA_CONTRAL_DESK_FORCE_COLSE = 1,
	MA_CONTRAL_DESK_LEFT_1_MINTS = 2,
	MA_CONTRAL_DESK_RESET = 3,
	MA_CONTRAL_DESK_END = 4,
};
struct LMsg_Match2L_ContralMatchDesk : public LMsg
{
	int			m_nMatchID = 0;
	int			m_deskID = 0;
	int			m_nGroupID = 0;
	int			m_ContalType = 0;  //控制类型, 1强制结束, 2还有1分钟结束, 3解散房间
	LMsg_Match2L_ContralMatchDesk() :LMsg(MSG_MA_2_L_CONTRAL_DESK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_ContalType);
		buff.Read(m_nMatchID);
		buff.Read(m_deskID);
		buff.Read(m_nGroupID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_ContalType);
		buff.Write(m_nMatchID);
		buff.Write(m_deskID);
		buff.Write(m_nGroupID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsg_Match2L_ContralMatchDesk(); }
};


enum
{
	RET_CONTRAL_DESK_OK = 0,
	RET_CONTRAL_DESK_NOTFOUND = 1,
	RET_CONTRAL_DESK_ID_ERROR = 2,
};
struct LMsg_L2Match_Ret_ContralMatchDesk : public LMsg
{
	int			m_nMatchID = 0;
	int			m_deskID = 0;
	int			m_nGroupID = 0;
	int			m_errNo = 0;  //0成功, 1未发现,ID不对应.
	LMsg_L2Match_Ret_ContralMatchDesk() :LMsg(MSG_L_2_MA_Ret_CONTRAL_DESK) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_errNo);
		buff.Read(m_nMatchID);
		buff.Read(m_deskID);
		buff.Read(m_nGroupID);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_errNo);
		buff.Write(m_nMatchID);
		buff.Write(m_deskID);
		buff.Write(m_nGroupID);
		return true;
	}
	virtual LMsg* Clone() { return new LMsg_L2Match_Ret_ContralMatchDesk(); }
};


struct LMsgS2C_MatchInfoText :public LMsgSC
{
	Lint			m_Type = 0;//0浮动, 1弹窗, 2OK弹窗
	Lint			m_strID = 0;
	Lstring			m_notic;
	Lint            m_errorCode = 0;//错误码 
	Lint            m_nMatchID = 0;
	MSGPACK_DEFINE_MAP(m_msgId, m_Type, m_strID, m_notic, m_errorCode, m_nMatchID);
	LMsgS2C_MatchInfoText() :LMsgSC(MSG_S_2_C_MATCH_INFOTXT) {}
	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack) { pack.pack(*this); return true; }
	virtual LMsg* Clone() { return new LMsgS2C_MatchInfoText(); }
};


struct LMsgL2Match_DeskCircleOverGoldInfo :public LMsg
{
	int			m_nDeskID = 0;
	int			m_nMatchID = 0;
	int			m_serverID = 0;
	int			m_nGroupID = 0;
	int			m_UserID[4] = { 0 };
	Ldouble		m_Gold[4] = { 0 };
	bool		m_bFinalEnd = true;
	LMsgL2Match_DeskCircleOverGoldInfo() :LMsg(MSG_L_2_MA_DESK_REPRTOVER) {}
	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_nDeskID);
		buff.Read(m_nMatchID);
		buff.Read(m_serverID);
		buff.Read(m_nGroupID);
		buff.Read(m_UserID);
		buff.Read(m_Gold);
		buff.Read(m_bFinalEnd);
		return true;
	}
	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_nDeskID);
		buff.Write(m_nMatchID);
		buff.Write(m_serverID);
		buff.Write(m_nGroupID);
		buff.Write(m_UserID);
		buff.Write(m_Gold);
		buff.Write(m_bFinalEnd);
		return true;
	}
	virtual LMsg* Clone() { return new LMsgL2Match_DeskCircleOverGoldInfo(); }
};


//上报LMG中马信息
struct LMsgL2LMGReportHorseInfo :public LMsg
{
	Lint			m_playerNum = 0;
	Lint			m_userid[DEFAULT_DESK_USER_COUNT] = { 0 };
	Lint			m_horseNum[DEFAULT_DESK_USER_COUNT] = { 0 };
	LMsgL2LMGReportHorseInfo() :LMsg(MSG_L_2_LMG_REPORT_HORSE_INFO)
	{
	}

	virtual bool Read(LBuff& buff)
	{
		buff.Read(m_playerNum);
		for (int i = 0; i < m_playerNum && i < DEFAULT_DESK_USER_COUNT; i++)
			buff.Read(m_userid[i]);

		for (int i = 0; i < m_playerNum && i < DEFAULT_DESK_USER_COUNT; i++)
			buff.Read(m_horseNum[i]);

		return true;
	}

	virtual bool Write(LBuff& buff)
	{
		buff.Write(m_playerNum);
		for (int i = 0; i < m_playerNum && i < DEFAULT_DESK_USER_COUNT; i++)
			buff.Write(m_userid[i]);

		for (int i = 0; i < m_playerNum && i < DEFAULT_DESK_USER_COUNT; i++)
			buff.Write(m_horseNum[i]);

		return true;
	}

	virtual LMsg* Clone()
	{
		return new LMsgL2LMGReportHorseInfo();
	}
};


////////////////////////////广东麻将 end//////////////////////////////


#endif