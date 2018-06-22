#ifndef GameHandler_h
#define GameHandler_h
#include "LBase.h"
#include "User.h"
#include "Card.h"
#include <list>
#include "NewHandCards.h"

class Desk;

struct GengZhuangInfo
{
	bool   m_isCircleEnd;			//跟庄小循环是否结束
	Card   m_zhuangCard;			//开局是庄家出牌
	Lint   m_cnt;					//跟庄结束时，有几人和庄家牌一致
	Lint   m_nSuccTimes;			//跟庄成功次数
	Lint   m_nBaseScore;			//跟庄基础分数

	GengZhuangInfo() :m_isCircleEnd(false), m_cnt(0), m_nSuccTimes(0), m_nBaseScore(1) {}

	void clear()
	{
		m_isCircleEnd = false;
		m_zhuangCard.m_color = 0;
		m_zhuangCard.m_number = 0;
		m_cnt = 0;
		m_nSuccTimes = 0;
		m_nBaseScore = 1;
	}
};

struct ThinkTool
{
	ThinkVec	m_thinkData;

	void Reset()
	{
		m_thinkData.clear();
	}

	ThinkTool()
	{
		Reset();
	}

	bool HasHu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_BOMB)
				return true;
		}
		return false;
	}

	bool HasPeng()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_PENG)
				return true;
		}
		return false;
	}

	bool HasGhostPeng()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_GHOSTPENG)
				return true;
		}
		return false;
	}

	bool HasChi()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_CHI)
				return true;
		}
		return false;
	}

	bool HasMBu()
	{
		for(Lsize i = 0 ; i < m_thinkData.size();++i)
		{
			if(m_thinkData[i].m_type == THINK_OPERATOR_MBU)
				return true;
		}
		return false;
	}

	bool HasMGang()
	{
		for (Lsize i = 0; i < m_thinkData.size(); ++i)
		{
			if (m_thinkData[i].m_type == THINK_OPERATOR_MGANG)
				return true;
		}
		return false;
	}


	bool NeedThink()
	{
		return !m_thinkData.empty();
	}
};

class GameHandler
{
public:
    virtual bool startup(Desk *game) { return false; }
    virtual void shutdown(void) {}
    virtual void gameStart(void) {}
	virtual  void			Tick(Desk&, time_t) {}
	 virtual void			HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg){}
	 virtual void			HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg){}
	 virtual void			HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg){}		//玩家处理海底牌
	 virtual void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg){}
	 virtual void			HanderUserOperSevenFlower(User* pUser, LMsgC2SUserOper* msg) {}
	 virtual void			OnUserReconnect(User* pUser){}
	 virtual void			OnGameOver(Lint result,Lint winpos[],Lint bombpos, Lint winner = 0) = 0;
	 virtual void			SetDeskPlay(){}
	 virtual void			ProcessRobot(Lint pos, User * pUser){}
	 virtual void SetDeskPlayNum() = 0;

	 virtual Lint GetDeskPlayNum() const { return m_deskplayer_num; }
	 
	 //virtual bool ProcessFoolish(Lint pos, User* pUser) { return true; }	 
	 //virtual void UpdateDeskUserCount(const std::vector<Lint>& playtype){} //更新玩家数量 
	 virtual void			AddHuInfoLog(const VideoLog& log, const LMsgS2CGameOver& over); 
	 //virtual void			OnPlay(User*, LUCHAR) {}
	 //virtual void			OnAccept(User*, suite_t&, std::vector<suite_t>&,bool) {}
	 //////////////////////////////////////////////////////////////////////////
	 bool IsValidPos(const Lint& pos) const
	 {
		 return (pos >= 0 && pos < m_deskplayer_num);		 
	 }
public:

	Desk			*m_desk;
	const Card*		m_curOutCard;//当前出出来的牌
	const Card*		m_curGetCard;//当前获取的牌

	const Card*		m_louHuCard[DEFAULT_DESK_USER_COUNT];		//是否有漏胡记录
	ThinkTool		m_thinkInfo[DEFAULT_DESK_USER_COUNT];//当前打牌思考状态
	ThinkUnit		m_thinkRet[DEFAULT_DESK_USER_COUNT];//玩家返回思考结果

	StartHuVector	m_startHu[DEFAULT_DESK_USER_COUNT];//起手胡牌

	CardVector		m_handCard[DEFAULT_DESK_USER_COUNT];//玩家手上的牌

	CardVector		m_outCard[DEFAULT_DESK_USER_COUNT];	//玩家出的牌

	CardVector		m_pengCard[DEFAULT_DESK_USER_COUNT];//玩家碰的牌，

	CardVector		m_gangCard[DEFAULT_DESK_USER_COUNT];//玩家明杠的牌

	CardVector		m_abombCard[DEFAULT_DESK_USER_COUNT];//玩家按杠的牌

	CardVector		m_eatCard[DEFAULT_DESK_USER_COUNT];//玩家吃的牌

	Lint			m_angang[DEFAULT_DESK_USER_COUNT];//暗杠数量
	Lint			m_minggang[DEFAULT_DESK_USER_COUNT];//明暗杠数量
	Lint			m_diangang[DEFAULT_DESK_USER_COUNT];//点杠数量
	Lint			m_adiangang[DEFAULT_DESK_USER_COUNT];//被点杠数量

	Lint			m_gangPos[DEFAULT_DESK_USER_COUNT];	//某位玩家有过杠

	Lint			m_playerHuInfo[DEFAULT_DESK_USER_COUNT];		//玩家胡牌信息， 因为可以多胡 放炮的人只保存了位置
	Lint			m_playerBombInfo[DEFAULT_DESK_USER_COUNT];	//玩家放炮信息，[玩家位置对应的炮的类型]
	bool			m_first_turn[DEFAULT_DESK_USER_COUNT];
	std::vector<Lint> m_gangRelation[DEFAULT_DESK_USER_COUNT];	//点杠与被点杠者值为点杠者				
	Lint			m_beforePos;//之前操作的位置
	Lint			m_beforeType;//之前操作的类型

	Lint			m_gold;

	Lint			m_zhuangpos;//庄家位置
	Lint			m_curPos;						//当前操作玩家

	Lint			m_endStartPos;						//海底牌结束位置
	Lint			m_endStartCurPos;					//海底牌当前位置
	bool			m_needGetCard;
	bool			m_isThinkingGang;					//当前是否在思考杠
	
	CardVector		m_deskCard;		//桌子上剩余的牌
	CardVector		m_curOutCards;	//当前出出来的牌		(为杠写的)
	VideoLog		m_video;						//录像
	OperateState	mGameInfo;
	Lint			m_deskplayer_num;
};

class GameHandlerCreator
{
public:
    virtual GameHandler* create(void) = 0;
};

class GameHandlerFactory
{
public:
    GameHandlerFactory(void);
    ~GameHandlerFactory(void);

    static GameHandlerFactory* getInstance(void);

    GameHandler* create(GameType gameType);
    void destroy(GameType gameType, GameHandler *gameHandler);

    void registerCreator(GameType gameType, GameHandlerCreator *creator);
    void unregisterCreator(GameType gameType);

private:
    std::map<GameType, GameHandlerCreator*> mCreators;
    std::map<GameType, std::list<GameHandler*> > mFreeHandlers;
};


#define DECLARE_GAME_HANDLER_CREATOR(gameType, className)                           \
class GameHandlerCreator_##className : public GameHandlerCreator                    \
{                                                                                   \
public:                                                                             \
    GameHandlerCreator_##className(void)                                            \
    {                                                                               \
        GameHandlerFactory::getInstance()->registerCreator(gameType,                \
            (GameHandlerCreator*)this);                                             \
    }                                                                               \
    ~GameHandlerCreator_##className(void)                                           \
    {                                                                               \
        GameHandlerFactory::getInstance()->unregisterCreator(                       \
            gameType);                                                              \
    }                                                                               \
    GameHandler* create(void)                                                       \
    {                                                                               \
        return new className();                                                     \
    }                                                                               \
};                                                                                  \
static GameHandlerCreator_##className sGameHandlerCreator_##className;
#endif


#ifdef _DEBUG
#define WAITTIME_TO_PLAY             7000			//毫秒		两局之间,不点开始游戏自动开始时间
#else
#define WAITTIME_TO_PLAY             7000          //9000  default ms两局之间,不点开始游戏自动开始时间
#endif // _DEBUG

#define NEW_TICK_TUOGUAN_DECLARE\
   void Tick(Desk& __, time_t t)\
    {\
		if (!m_desk)\
		{\
			LLOG_ERROR("m_desk is NULL!");\
			return;\
		}\
		if (!m_desk->m_vip)\
		{\
			LLOG_ERROR("m_desk->m_vip is NULL");\
			return;\
		}\
		if (m_desk->m_vip->m_curMatchId == 0)\
		{\
			return;\
		}\
		if (m_desk->getDeskState() == DESK_FREE)\
		{\
			return;\
		}\
		m_desk->m_DeskRemainPlayTime--;\
		if((t - m_desk->m_DeskStartPlayTime) > m_desk->m_vip->m_nForceFinishTime * 1000)\
		{\
			m_desk->m_DeskStartPlayTime = gWork.GetCurTime().MSecs();\
			OnGameOver(WIN_NONE, m_playerHuInfo, DEFAULT_INVAILD_POS, NULL);\
			return;\
		}\
		if (m_desk->getDeskState() != DESK_PLAY)\
		{\
			if (t - m_desk->m_LastWaiteThinkRetTime > WAITTIME_TO_PLAY && m_desk->m_vip->m_curCircle < m_desk->m_vip->m_maxCircle)\
			{\
				m_desk->SetAllReady();\
				m_desk->CheckGameStart();\
			}\
			return;\
		}\
		if (t - m_desk->m_LastWaiteThinkRetTime > m_desk->m_vip->m_InTuoGuanCanChuPaiTime)\
		{\
			for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)\
			{\
				Lint pos = (m_curPlayingPos + i) % DEFAULT_DESK_USER_COUNT;\
				User* puser = m_desk->GetPosUser(pos);\
				if (puser == NULL) continue;\
				if (puser->GetTuoGuan())\
				{\
					if (ProcessTuoGuan(pos, puser))\
					{\
						return;\
					}\
				}\
			}\
		}\
		if (t - m_desk->m_LastWaiteThinkRetTime > m_desk->m_vip->m_IntoTuoGuanRemainTime)\
		{\
			for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)\
			{\
				Lint pos = (m_curPlayingPos + i) % DEFAULT_DESK_USER_COUNT;\
				User* puser = m_desk->GetPosUser(pos);\
				if (puser == NULL) continue;\
				if (ProcessTuoGuan(pos, puser))\
				{\
					return;\
				}\
			}\
		}\
	}\
	bool ProcessTuoGuan(Lint pos, User * pUser,bool bAI=false)\
	{\
		if (pos < 0 || pos > 3)\
			return false;\
		switch (m_desk->getDeskPlayState())\
		{\
		case DESK_PLAY_GET_CARD:\
		case DESK_PLAY_END_CARD:\
		{\
			if (m_curPlayingPos != pos)\
			{\
				return false;\
			}\
			if(!pUser->GetTuoGuan())\
			{\
				pUser->SetTuoGuan(true); \
				LMsgS2C_TuoGuanInfo msg;\
				msg.m_nPos = pos;\
				msg.m_nType = pUser->GetTuoGuan();\
				m_desk->BoadCast(msg);\
				++m_nTrusteeshipCount[pos];\
			}\
			if (bAI)\
			{\
				if (m_thinkInfo[pos].HasHu())\
				{\
					for (int i = 0; i < m_thinkInfo[m_curPlayingPos].m_thinkData.size(); i++)\
					{\
						if (m_thinkInfo[m_curPlayingPos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)\
						{\
							LMsgC2SUserPlay msg;\
							msg.m_thinkInfo.m_type = THINK_OPERATOR_BOMB;\
							EntiryCardVector& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;\
							for (int j = 0; j < mCard.size(); j++)\
							{\
								CardValue card;\
								card.m_color = mCard[j].m_color;\
								card.m_number = mCard[j].m_number;\
								msg.m_thinkInfo.m_card.push_back(card);\
							}\
							m_desk->HanderUserPlayCard(pUser, &msg);\
							return true;\
						}\
					}\
					return false;\
				}\
				else if (m_thinkInfo[pos].NeedThink())\
				{\
					LMsgC2SUserPlay msg;\
					msg.m_thinkInfo.m_type = m_thinkInfo[pos].m_thinkData[0].m_type;\
					EntiryCardVector& mCard = m_thinkInfo[pos].m_thinkData[0].m_card;\
					for (int j = 0; j < mCard.size(); j++)\
					{\
						CardValue card;\
						card.m_color = mCard[j].m_color;\
						card.m_number = mCard[j].m_number;\
						msg.m_thinkInfo.m_card.push_back(card);\
					}\
					m_desk->HanderUserPlayCard(pUser, &msg);\
					return true;\
				}\
				LMsgC2SUserPlay msg;\
				msg.m_thinkInfo.m_type = THINK_OPERATOR_OUT;\
				CardValue card;\
				m_handCard[pos].GetSmartOutCard(card);\
				msg.m_thinkInfo.m_card.push_back(card);\
				m_desk->HanderUserPlayCard(pUser, &msg);\
				return true;\
			}\
			else\
			{\
				if (m_thinkInfo[pos].HasHu())\
				{\
					for (int i = 0; i < m_thinkInfo[m_curPlayingPos].m_thinkData.size(); i++)\
					{\
						if (m_thinkInfo[m_curPlayingPos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)\
						{\
							LMsgC2SUserPlay msg;\
							msg.m_thinkInfo.m_type = THINK_OPERATOR_BOMB;\
							EntiryCardVector& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;\
							for (int j = 0; j < mCard.size(); j++)\
							{\
								CardValue card;\
								card.m_color = mCard[j].m_color;\
								card.m_number = mCard[j].m_number;\
								msg.m_thinkInfo.m_card.push_back(card);\
							}\
							m_desk->HanderUserPlayCard(pUser, &msg);\
							return true;\
						}\
					}\
					return false;\
				}\
				ProcessNonAITuoGuan(pos, pUser);\
			}\
			return true;\
		}\
		break;\
		case DESK_PLAY_THINK_CARD:\
		{\
			if(!m_thinkInfo[pos].NeedThink()) return false;\
			if(!pUser->GetTuoGuan())\
			{\
				pUser->SetTuoGuan(true); \
				LMsgS2C_TuoGuanInfo msg;\
				msg.m_nPos = pos;\
				msg.m_nType = pUser->GetTuoGuan();\
				m_desk->BoadCast(msg);\
				++m_nTrusteeshipCount[pos];\
			}\
			if (bAI)\
			{\
				if (m_thinkInfo[pos].HasHu())\
				{\
					for (int i = 0; i < m_thinkInfo[pos].m_thinkData.size(); i++)\
					{\
						if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)\
						{\
							LMsgC2SUserOper msg;\
							msg.m_think.m_type = THINK_OPERATOR_BOMB;\
							EntiryCardVector& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;\
							for (int j = 0; j < mCard.size(); j++)\
							{\
								CardValue card;\
								card.m_color = mCard[j].m_color;\
								card.m_number = mCard[j].m_number;\
								msg.m_think.m_card.push_back(card);\
							}\
							HanderUserOperCard(pUser, &msg);\
							return true;\
						}\
					}\
					return false;\
				}\
				else if (m_thinkInfo[pos].NeedThink())\
				{\
					LMsgC2SUserOper msg;\
					msg.m_think.m_type = m_thinkInfo[pos].m_thinkData[0].m_type;\
					EntiryCardVector& mCard = m_thinkInfo[pos].m_thinkData[0].m_card;\
					for (int j = 0; j < mCard.size(); j++)\
					{\
						CardValue card;\
						card.m_color = mCard[j].m_color;\
						card.m_number = mCard[j].m_number;\
						msg.m_think.m_card.push_back(card);\
					}\
					HanderUserOperCard(pUser, &msg);\
					return true;\
				}\
			}\
			else if (m_thinkInfo[pos].NeedThink())\
			{\
				LMsgC2SUserOper msg;\
				msg.m_think.m_type = THINK_OPERATOR_NULL;\
				HanderUserOperCard(pUser, &msg);\
				return true;\
			}\
		}\
		break;\
		default:\
		{\
			LLOG_DEBUG("robote NULL ERROR ");\
		}\
		}\
		return false;\
	}\
	void  ProcessNonAITuoGuan(Lint pos, User * pUser)\
	{\
		LMsgC2SUserPlay msg;\
		msg.m_thinkInfo.m_type = THINK_OPERATOR_OUT;\
		CardValue card;\
		int last1Op = m_OperRecoder.GetLast1Operator();\
		if (last1Op == THINK_OPERATOR_PENG || last1Op == THINK_OPERATOR_CHI)\
		{\
			m_handCard[pos].GetTail(card);\
		}\
		else\
		{\
			card.m_color = m_curGetCard.m_color;\
			card.m_number = m_curGetCard.m_number;\
		}\
		msg.m_thinkInfo.m_card.push_back(card);\
		m_desk->HanderUserPlayCard(pUser, &msg);\
	}\
	public:\
	void    UpdateLastWaiteThinkRetTime() { m_desk->m_LastWaiteThinkRetTime = gWork.GetCurTime().MSecs(); }\
	private:\
	int		m_nTrusteeshipCount[DEFAULT_DESK_USER_COUNT];\
	bool	m_bUserInTrusteeship[DEFAULT_DESK_USER_COUNT];




#define OLD_TICK_TUOGUAN_DECLARE \
   void Tick(Desk& __, time_t t)\
    {\
		if (!m_desk)\
		{\
			LLOG_ERROR("m_desk is NULL");\
			return;\
		}\
		if (!m_desk->m_vip)\
		{\
			LLOG_ERROR("m_desk->m_vip is NULL");\
			return;\
		}\
		if (m_desk->m_vip->m_curMatchId == 0)\
		{\
			return;\
		}\
		if (m_desk->getDeskState() == DESK_FREE)\
		{\
			return;\
		}\
        m_desk->m_DeskRemainPlayTime--;\
		if((t - m_desk->m_DeskStartPlayTime) > m_desk->m_vip->m_nForceFinishTime * 1000)\
		{\
			m_desk->m_DeskStartPlayTime = gWork.GetCurTime().MSecs();\
			OnGameOver(WIN_NONE, m_playerHuInfo, DEFAULT_INVAILD_POS, NULL);\
			return;\
		}\
		if (m_desk->getDeskState() != DESK_PLAY)\
		{\
			if (t - m_desk->m_LastWaiteThinkRetTime > WAITTIME_TO_PLAY && m_desk->m_vip->m_curCircle < m_desk->m_vip->m_maxCircle)\
			{\
                m_desk->SetAllReady();\
				m_desk->CheckGameStart();\
			}\
			return;\
		}\
		if (t - m_desk->m_LastWaiteThinkRetTime > m_desk->m_vip->m_InTuoGuanCanChuPaiTime)\
		{\
			for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)\
			{\
				Lint pos = (m_curPos + i) % DEFAULT_DESK_USER_COUNT;\
				User* puser = m_desk->GetPosUser(pos);\
				if (puser == NULL) continue;\
				if (puser->GetTuoGuan())\
				{\
					if (ProcessTuoGuan(pos, puser))\
					{\
						return;\
					}\
				}\
			}\
		}\
		if (t - m_desk->m_LastWaiteThinkRetTime > m_desk->m_vip->m_IntoTuoGuanRemainTime)\
		{\
			for (int i = 0; i < DEFAULT_DESK_USER_COUNT; i++)\
			{\
				Lint pos = (m_curPos + i) % DEFAULT_DESK_USER_COUNT;\
				User* puser = m_desk->GetPosUser(pos);\
				if (puser == NULL) continue;\
				if (ProcessTuoGuan(pos, puser))\
				{\
					return;\
				}\
			}\
		}\
	}\
	bool ProcessTuoGuan(Lint pos, User * pUser,bool bAI=false)\
	{\
		if (pos < 0 || pos > 3)\
			return false;\
		switch (m_desk->getDeskPlayState())\
		{\
		case DESK_PLAY_GET_CARD:\
		case DESK_PLAY_END_CARD:\
		{\
			if (m_curPos != pos)\
			{\
				return false;\
			}\
			if(!pUser->GetTuoGuan())\
			{\
				pUser->SetTuoGuan(true); \
				LMsgS2C_TuoGuanInfo msg;\
				msg.m_nPos = pos;\
				msg.m_nType = pUser->GetTuoGuan();\
				m_desk->BoadCast(msg);\
				++m_nTrusteeshipCount[pos];\
			}\
			if (bAI)\
			{\
				if (m_thinkInfo[pos].HasHu())\
				{\
					for (int i = 0; i < m_thinkInfo[m_curPos].m_thinkData.size(); i++)\
					{\
						if (m_thinkInfo[m_curPos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)\
						{\
							LMsgC2SUserPlay msg;\
							msg.m_thinkInfo.m_type = THINK_OPERATOR_BOMB;\
							std::vector<const Card*>& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;\
							for (int j = 0; j < mCard.size(); j++)\
							{\
								CardValue card;\
								card.m_color = mCard[j]->m_color;\
								card.m_number = mCard[j]->m_number;\
								msg.m_thinkInfo.m_card.push_back(card);\
							}\
							m_desk->HanderUserPlayCard(pUser, &msg);\
							return true;\
						}\
					}\
					return false;\
				}\
				else if (m_thinkInfo[pos].NeedThink())\
				{\
					LMsgC2SUserPlay msg;\
					msg.m_thinkInfo.m_type = m_thinkInfo[pos].m_thinkData[0].m_type;\
					std::vector<const Card*>& mCard = m_thinkInfo[pos].m_thinkData[0].m_card;\
					for (int j = 0; j < mCard.size(); j++)\
					{\
						CardValue card;\
						card.m_color = mCard[j]->m_color;\
						card.m_number = mCard[j]->m_number;\
						msg.m_thinkInfo.m_card.push_back(card);\
					}\
					m_desk->HanderUserPlayCard(pUser, &msg);\
					return true;\
				}\
				LMsgC2SUserPlay msg;\
				msg.m_thinkInfo.m_type = THINK_OPERATOR_OUT;\
				CardValue card;\
                GetSmartOutCard(card);\
				msg.m_thinkInfo.m_card.push_back(card);\
				m_desk->HanderUserPlayCard(pUser, &msg);\
				return true;\
			}\
			else\
			{\
				if (m_thinkInfo[pos].HasHu())\
				{\
					for (int i = 0; i < m_thinkInfo[m_curPos].m_thinkData.size(); i++)\
					{\
						if (m_thinkInfo[m_curPos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)\
						{\
							LMsgC2SUserPlay msg;\
							msg.m_thinkInfo.m_type = THINK_OPERATOR_BOMB;\
							std::vector<const Card*>& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;\
							for (int j = 0; j < mCard.size(); j++)\
							{\
								CardValue card;\
								card.m_color = mCard[j]->m_color;\
								card.m_number = mCard[j]->m_number;\
								msg.m_thinkInfo.m_card.push_back(card);\
							}\
							m_desk->HanderUserPlayCard(pUser, &msg);\
							return true;\
						}\
					}\
					return false;\
				}\
				ProcessNonAITuoGuan(pos, pUser);\
			}\
			return true;\
		}\
		break;\
		case DESK_PLAY_THINK_CARD:\
		{\
			if(!m_thinkInfo[pos].NeedThink()) return false;\
			if(!pUser->GetTuoGuan())\
			{\
				pUser->SetTuoGuan(true); \
				LMsgS2C_TuoGuanInfo msg;\
				msg.m_nPos = pos;\
				msg.m_nType = pUser->GetTuoGuan();\
				m_desk->BoadCast(msg);\
				++m_nTrusteeshipCount[pos];\
			}\
			if (bAI)\
			{\
				if (m_thinkInfo[pos].HasHu())\
				{\
					for (int i = 0; i < m_thinkInfo[pos].m_thinkData.size(); i++)\
					{\
						if (m_thinkInfo[pos].m_thinkData[i].m_type == THINK_OPERATOR_BOMB)\
						{\
							LMsgC2SUserOper msg;\
							msg.m_think.m_type = THINK_OPERATOR_BOMB;\
							std::vector<const Card*>& mCard = m_thinkInfo[pos].m_thinkData[i].m_card;\
							for (int j = 0; j < mCard.size(); j++)\
							{\
								CardValue card;\
								card.m_color = mCard[j]->m_color;\
								card.m_number = mCard[j]->m_number;\
								msg.m_think.m_card.push_back(card);\
							}\
							HanderUserOperCard(pUser, &msg);\
							return true;\
						}\
					}\
					return false;\
				}\
				else if (m_thinkInfo[pos].NeedThink())\
				{\
					LMsgC2SUserOper msg;\
					msg.m_think.m_type = m_thinkInfo[pos].m_thinkData[0].m_type;\
					std::vector<const Card*>& mCard = m_thinkInfo[pos].m_thinkData[0].m_card;\
					for (int j = 0; j < mCard.size(); j++)\
					{\
						CardValue card;\
						card.m_color = mCard[j]->m_color;\
						card.m_number = mCard[j]->m_number;\
						msg.m_think.m_card.push_back(card);\
					}\
					HanderUserOperCard(pUser, &msg);\
					return true;\
				}\
			}\
			else if (m_thinkInfo[pos].NeedThink())\
			{\
				LMsgC2SUserOper msg;\
				msg.m_think.m_type = THINK_OPERATOR_NULL;\
				HanderUserOperCard(pUser, &msg);\
				return true;\
			}\
		}\
		break;\
		default:\
		{\
			LLOG_DEBUG("robote NULL ERROR ");\
		}\
		}\
		return false;\
	}\
	void  ProcessNonAITuoGuan(Lint pos, User * pUser)\
	{\
		LMsgC2SUserPlay msg;\
		msg.m_thinkInfo.m_type = THINK_OPERATOR_OUT;\
		CardValue card;\
		if (m_curGetCard && m_needGetCard)\
		{\
			card.m_color = m_curGetCard->m_color;\
			card.m_number = m_curGetCard->m_number;\
		}\
		else\
		{\
			int size = m_handCard[pos].size();\
			if (size > 0)\
			{\
				card.m_color = m_handCard[pos][size - 1]->m_color;\
				card.m_number = m_handCard[pos][size - 1]->m_number;\
			}\
		}\
		msg.m_thinkInfo.m_card.push_back(card);\
		m_desk->HanderUserPlayCard(pUser, &msg);\
	}\
	void GetSmartOutCard(CardValue& out)\
	{\
		NewHandCard tmpHandCard;\
		for (auto pCard : m_handCard[m_curPos])\
		{\
			tmpHandCard.AddCard(pCard);\
		}\
		tmpHandCard.GetSmartOutCard(out);\
	}\
	public:\
	void    UpdateLastWaiteThinkRetTime() { m_desk->m_LastWaiteThinkRetTime = gWork.GetCurTime().MSecs(); }\
	protected:\
	int		m_nTrusteeshipCount[DEFAULT_DESK_USER_COUNT];\
	bool	m_bUserInTrusteeship[DEFAULT_DESK_USER_COUNT];