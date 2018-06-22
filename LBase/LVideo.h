//查牌功能


#ifndef _L_VIDEO_H_
#define _L_VIDEO_H_

#include "LTool.h"
#include "LBuff.h"
#include "GameDefine.h"

struct CardValue
{
	Lint m_color;
	Lint m_number;

	MSGPACK_DEFINE(m_color, m_number); 
	CardValue()
	{
		m_color = 0;
		m_number = 0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_color), m_color);
		ReadMapData(obj, NAME_TO_STR(m_number), m_number);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteKeyValue(pack, NAME_TO_STR(m_color), m_color);
		WriteKeyValue(pack, NAME_TO_STR(m_number), m_number);
		return true;
	}

	Lint GetNCIndex()
	{
		return (m_color * 10 + m_number);
	}
};

bool operator== (const CardValue& card1, const CardValue& card2);

struct LiangPai
{
	Lint     m_ncCard;    //亮牌， NC值
	Lint     m_nType;     //0 表示手牌   1 表示碰牌  2 表示杠牌   3表示天听
	Lint     m_nOutIndex; //出牌的位置

	MSGPACK_DEFINE(m_ncCard, m_nType);

	LiangPai()
	{
		m_ncCard = 0;
		m_nType = 0;
		m_nOutIndex = 0;
	}

	bool Read(msgpack::object& obj)
	{
		ReadMapData(obj, NAME_TO_STR(m_ncCard), m_ncCard);
		ReadMapData(obj, NAME_TO_STR(m_nType), m_nType);
		ReadMapData(obj, NAME_TO_STR(m_nOutIndex), m_nOutIndex);
		return true;
	}

	bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		WriteMap(pack, 3);
		WriteKeyValue(pack, NAME_TO_STR(m_ncCard), m_ncCard);
		WriteKeyValue(pack, NAME_TO_STR(m_nType), m_nType);
		WriteKeyValue(pack, NAME_TO_STR(m_nOutIndex), m_nOutIndex);
		return true;
	}

	//亮牌和亮牌类型最好同时设置，避免遗忘类型
	void setLiangPai(Lint ncCard, Lint outIndex, Lint nType = 0)
	{
		m_ncCard = ncCard;
		m_nType = nType;
		m_nOutIndex = outIndex;
	}

	void resetOutIndex()
	{
		m_nOutIndex = 0;
	}

	void setType(Lint nType)
	{
		m_nType = nType;
	}

	void clear()
	{
		m_ncCard = 0;
		m_nType = 0;
		m_nOutIndex = 0;
	}

	Lint GetNCIndex()
	{
		return m_ncCard;
	}

	Lint GetOutIndex()
	{
		return m_nOutIndex;
	}
};

enum VIDEO_OPER
{
	VIDEO_OPER_NULL =  0,//不出;
	VIDEO_OPER_SINGLE = 1,//单张;
	VIDEO_OPER_DOUNLE = 2,//对子;
	VIDEO_OPER_THREE = 3,//三个;
	VIDEO_OPER_THREE_ONE = 4,//三带1张;
	VIDEO_OPER_THREE_TWO = 5, //三带2张;
	VIDEO_OPER_THREE_LIST = 6, // 飞机;
	VIDEO_OPER_THREE_LIST_ONE = 7,//飞机带单;
	VIDEO_OPER_THREE_LIST_TWO = 8,//飞机带双;
	VIDEO_OPER_SINGLE_LIST = 11,//单顺;
	VIDEO_OPER_DOUBLE_LIST = 12,//连对;
	VIDEO_OPER_BOMB = 13,//炸弹;

	//录像中实际记入牌型使用以下值;
// 	card_style_error = 0,               //不出;
// 	card_style_single = 1,				//单张;
// 	card_style_double = 2,				//一对;
// 	card_style_three  = 3,				//三个;
// 	card_style_three_single = 4,		//三带单;
// 	card_style_three_double = 5,		//三带对;
// 	card_style_three_list = 6,			//飞机;
// 	card_style_three_list_single = 7,	//飞机带单;
// 	card_style_three_list_double = 8,	//飞机带对;
// 	card_style_bomb_and_single = 9,		//四代2单;
// 	card_style_bomb_and_double = 10,		//四代2对;
// 	card_style_single_list = 11,			//单顺;
// 	card_style_double_list = 12,			//双顺;
// 	card_style_bomb3 = 13,				//带癞子的炸弹;
// 	card_style_bomb2 = 14,				//纯粹硬炸弹;
// 	card_style_bomb1 = 15,				//纯粹软炸弹;
// 	card_style_rocket = 16,				//火箭;
// 	card_style_four2 = 17,              //四带二;

	VIDEO_OPER_QIANGDIZHU = 21, //抢地主;
	VIDEO_OPER_BUQIANG = 22, //不抢地主;
	VIDEO_OPER_DIPAI = 23,//地主拿底牌;
	VIDEO_OPER_REDEALCARD = 24, //无人抢地主，重新发牌;

	VIDEO_OPER_CHUNTIAN = 50, //玩家春天;
	VIDEO_OPER_FANCHUN = 51, //玩家反春;

	VIDEO_OPER_BEISHU = 98,//倍数;
	VIDEO_OPER_SHOWCARDS = 99,//通知某个玩家手牌;
	//VIDEO_OPER_MENZHUANG = 100;//看牌;
	//VIDEO_OPER_MENZHUANG = 101;//闷抓;
	//VIDEO_OPER_MENZHUANG = 200;//不抓(二人不叫地主);
	//VIDEO_OPER_MENZHUANG = 201;//明抓（二人斗叫）;
	VIDEO_OPER_JIAODIZHU = 201,//叫地主				(3人抢地主玩法 不叫地主）;
	VIDEO_OPER_BUJIAODIZHU = 200,//不叫地主			（3人抢地主玩法 叫地主）;
	//VIDEO_OPER_MENZHUANG = 300;//不倒(二斗为不抢);
	//VIDEO_OPER_MENZHUANG = 301;//倒了(二斗抢地主);
	//VIDEO_OPER_MENZHUANG = 400;//不拉;
	//VIDEO_OPER_MENZHUANG = 401;//我拉;

	VIDEO_OPER_GUANJIA = 402,	//管家(花色）;
};

enum VIDEO_OPER_CS
{
	VIDEO_OPER_GET_CARD = 1,	//摸牌
	VIDEO_OPER_OUT_CARD,		//出牌
	VIDEO_OPER_AN_GANG,			//暗杠
	VIDEO_OPER_MING_GANG,		//我出牌明杠
	VIDEO_OPER_PENG_CARD,		//碰牌
	VIDEO_OPER_GANG,			//别人出牌，我杠
	VIDEO_OPER_SHOUPAO,			//收炮
	VIDEO_OPER_ZIMO,			//自摸
	VIDEO_OPER_HUANGZHUANG,		//流局
	VIDEO_OPER_EAT,				//吃
	VIDEO_OPER_SELF_BU,			//明补自已的
	VIDEO_OPER_OTHER_BU,		//明补他人的
	VIDEO_OPER_AN_BU,			//暗补

	VIDEO_OPER_START_HU_QUEYISE,		//缺一色
	VIDEO_OPER_START_HU_BANBANHU,		//板板胡
	VIDEO_OPER_START_HU_DASIXI,			//大四喜
	VIDEO_OPER_START_HU_LIULIUSHUN,		//六六顺

	VIDEO_OPER_GET_BU_CARDS,			//获取补牌 2张

	VIDEO_OPEN_THINK = 21,//玩家思想
	VIDEO_OPEN_DOING = 22,//玩家决定
	VIDEO_OPEN_HAIDI_REMIND = 53,	//海底提示;
	VIDEO_OPEN_HAIDI_SELECT = 54,	//海底选择;
	VIDEO_OPEN_HAIDI_PASS = 55,	//海底过;
	VIDEO_OPEN_HAIDI_SHOW = 56,	//海底过;

	VIDEO_OPEN_BIRD_SHOW = 61,	//扎鸟;
	VIDEO_PLAYER_RECONNECT = 62,	//扎鸟;
	VIDEO_OPEN_USER_PIAO = 63,	//飘;

	VIDEO_OPEN_WANGBA = 73,
	VIDEO_OPER_XIABU = 64,        //补虾牌
};

enum VIDEO_OPER_GD 
{
	//前面部分和CS一样
	VIDEO_GHOST_CARD = 62,	//生成鬼牌
	VIDEO_GHOST_PENG = 63,	//带鬼碰
	VIDEO_GHOST_GANG1 = 64,	//带鬼补杠
	VIDEO_GHOST_GANG2 = 65,    //带鬼2次补杠

	VIDEO_OPER_GENGZHUANG_SUCC = 100, //跟庄成功
	VIDEO_OPER_FEED_CARD = 101, //补牌
	VIDEO_OPER_HIT_ALL_HORSE = 102, //广东鸡胡海底捞中马
	VIDEO_OPER_JIEYANG_CATCHHOUSE = 103, //揭阳麻将抓马
	VIDEO_OPER_QISHOU_FEED_CARD = 104, //起手补花牌
};

struct VideoOper
{
	Lint		m_pos;
	Lint		m_oper;
	std::vector<Lint> m_cards;	//玩家操作的牌;
	std::vector<Lint> m_addCards;	//玩家操作的牌;

	MSGPACK_DEFINE(m_pos,m_oper,m_cards, m_addCards); 
	VideoOper()
	{
		m_pos = 0;
		m_oper = 0;
		m_cards.clear();
		m_addCards.clear();
	}

	Lstring		ToString()
	{
		std::stringstream ss;
		ss << m_pos << ",";
		ss << m_oper;
		for (size_t i = 0; i< m_cards.size(); i++)
		{
			ss << "," << m_cards[i];
		}
		if (m_addCards.size())
		{
			ss << "|" << m_addCards[0];
			for (size_t i = 1; i< m_addCards.size(); i++)
			{
				ss << "," << m_addCards[i];
			}
		}
		ss << ";";
		return ss.str();
	}

	void	FromString(const Lstring& str)
	{
		//std::vector<Lstring> des;
		//L_ParseString(str, des, ",");
		//if (des.size() >= 2)
		//{
		//	m_pos = atoi(des[0].c_str());
		//	m_oper = atoi(des[1].c_str());
		//	std::vector<Lstring> splids;
		//	for (int i = 2; i < des.size(); i++)
		//	{
		//		Lint v = atoi(des[i].c_str());
		//		m_cards.push_back(v);
		//	}
		//}

		std::vector<Lstring> des1;
		L_ParseString(str, des1, "|");
		if (des1.size() >= 1)
		{
			std::vector<Lstring> des;
			L_ParseString(des1[0], des, ",");

			m_pos = atoi(des[0].c_str());
			m_oper = atoi(des[1].c_str());
			//std::vector<Lstring> splids;
			for (size_t i = 2; i < des.size(); i++)
			{
				Lint v = atoi(des[i].c_str());
				m_cards.push_back(v);
			}
		}
		if (des1.size() >= 2)
		{
			std::vector<Lstring> des;
			L_ParseString(des1[1], des, ",");
			//std::vector<Lstring> splids;
			for (size_t i = 0; i < des.size(); i++)
			{
				Lint v = atoi(des[i].c_str());
				m_addCards.push_back(v);
			}
		}
	}
};


struct VideoOper2
{
	Lint		m_pos;
	Lint		m_oper;
	std::vector<CardValue> m_cards;		//第一个为目标牌（吃，碰，等），后面的是已有的牌

	MSGPACK_DEFINE(m_pos, m_oper, m_cards);
	VideoOper2()
	{
		m_pos = 0;
		m_oper = 0;
		m_cards.clear();
	}

	Lstring		ToString()
	{
		std::stringstream ss;
		ss << m_pos << ",";
		ss << m_oper << ",";
		for (int i = 0; i< m_cards.size(); i++)
		{
			ss << m_cards[i].m_color << "|" << m_cards[i].m_number;
			if (i < m_cards.size() - 1)
			{
				ss << ",";
			}
		}
		ss << ";";
		return ss.str();
	}

	void	FromString(Lstring str)
	{
		std::vector<Lstring> des;
		L_ParseString(str, des, ",");
		if (des.size() >= 3)
		{
			m_pos = atoi(des[0].c_str());
			m_oper = atoi(des[1].c_str());
			std::vector<Lstring> splids;
			for (int i = 2; i < des.size(); i++)
			{
				L_ParseString(des[i], splids, "|");
				if (splids.size() >= 2)
				{
					CardValue card;
					card.m_color = atoi(splids[0].c_str());
					card.m_number = atoi(splids[1].c_str());
					m_cards.push_back(card);
				}
			}
		}
	}
};

enum EMGameLogicType;
class VideoLog
{
public:
	VideoLog();
	virtual~VideoLog();
	void		AddOper(Lint oper,Lint pos, const std::vector<Lint>& cards);
	void		AddOper(Lint oper, Lint pos, const std::vector<CardValue>& cards);
	void		AddOper(Lint oper, Lint pos, const std::vector<Lint>& cards, const std::vector<Lint>& addCards);

	void		DealCard(Lint* userId, std::vector<Lint>* vec, Lint time,Lint zhuang, Ldouble* score,Lint deskId,Lint cur,Lint max,Lint flag, std::vector<Lint>& playType);
	void		DealCard(Lint* userId, std::vector<CardValue>* vec, Lint time, Lint zhuang, Ldouble* score, Lint deskId, Lint cur, Lint max, Lint flag, std::vector<Lint>* playType);

	Lstring		ToString();

	void		FromString(const Lstring& str);
	void		FromString(Lstring str, Lint user_count);

	void		Write(LBuff& buff);

	void		Read(LBuff& buff);

	void		Clear();

	Lstring		GetInsertSql();

	Lstring     PlayTypeToStrint();

	void        PlayTypeFromString(const Lstring& str);

	void        SetEveryResult(int para1, ...);
	void        SetEveryResult2(int para1, ...);

protected:
	Lint        GetUserCounts();

private:
	Lstring		ToString1();
	Lstring		ToString2();
	Lstring     PlayTypeToStrint1();
	Lstring     PlayTypeToStrint2();
public:
	Lint							m_time;
	Lstring							m_Id;
	Lint							m_zhuang;
	Lint							m_deskId;
	Lint							m_curCircle;
	Lint							m_maxCircle;
	Ldouble							m_score[4];
	Ldouble							m_calScore[4];
	Lint							m_userId[4];
	Lint							m_flag;//牌局玩法;
	std::vector<Lint>				m_handCard[4];
	std::vector<CardValue>			m_handCard2[4];
	std::vector<VideoOper>			m_oper;
	std::vector<VideoOper2>			m_oper2;
	std::vector<Lint>				m_playType;
	Lstring							m_str;
	Lstring							m_str_playtype;
	Lint							m_nAppId;

	EMGameLogicType m_type; //日志类型
};



#endif