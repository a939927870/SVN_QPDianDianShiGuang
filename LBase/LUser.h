#ifndef _L_USER_H_
#define _L_USER_H_ 

#include "LBase.h"
#include "LBuff.h"
#include "LMsgCT.h"


class LUser
{
public:
	LUser();
	~LUser();

	void	Read(LBuff& buff);
	void	Write(LBuff& buff);

public:
	Lint		m_id;
	Lstring		m_openid;
	Lstring		m_nike;
	Lint		m_sex;
	Lstring		m_provice;
	Lstring		m_city;
	Lstring		m_country;
	Lstring		m_headImageUrl;
	Lstring		m_unioid;
	Lstring		m_playerType;
	Lint		m_numOfCard1s;
	Ldouble		m_numOfCard2s;
	Lint		m_numOfCard3s;
	Lint		m_regTime;
	Lint		m_lastLoginTime;
	Lint		m_new;
	Lint		m_gm;
	Ldouble		m_totalbuynum;
	Lint		m_totalplaynum;
	Lint		m_diamondNum;
	Lint        m_couponNum;
	Lint        m_Gold;
	Lint        m_NewGolduser;
	Lint        m_RemainingGifts;
	Lint        m_LastGiftTime; 

	Lstring     m_location;  //玩家登录GPS位置，不计入数据库;
	Lstring     m_address;

	Lint		m_onUseTeamId = 0; //正在使用的俱乐部
	Lint		m_useTeamId = 0; //使用的俱乐部
	Lint		m_curHeadIconId = 0;
	Lstring		m_gainHeadIconId;
	Lint		m_nAppId = 0;
	
	QueryTeamInfo m_teaminfo; //俱乐部自己的信息
};

#endif