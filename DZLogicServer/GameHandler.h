#ifndef GameHandler_h
#define GameHandler_h
#include "LBase.h"
#include "User.h"
#include "GameDefine.h"
#include <list>

class Desk;

class GameHandler
{
public:
	virtual bool startup(Desk *game) { return false; }
	virtual void shutdown(void) {}
	virtual void gameStart(void) {}

	virtual void			HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg){}
	virtual void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg){}
	virtual bool			OnUserReconnect(User* pUser){return false;}
	virtual void			OnGameOver(Lint result,Lint bombpos){}
	virtual void            SetDeskDeal(){}
	virtual void            CheckSameIP(){}
	virtual void			ProcessRobot(Lint pos, User * pUser){}
	virtual void            HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg){}
	virtual void            SetDeskPlay(){};

	virtual void            QiangDiZhu(Lint nPos){};
	virtual void            HanderUserQiangDiZhu(User* pUser,LMsgC2SQiangDiZhu* msg){};
	virtual void            HanderUserQiangDiZhuOper(User* pUser,LMsgC2SQdzOper_Sichuan* msg){};
	virtual void            dealCard_new(){};
	virtual void            SetDeskEnd(Lchar pos){};
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

	GameHandler* create(PK_GAME_STYLE gameType);
	void destroy(PK_GAME_STYLE gameType, GameHandler *gameHandler);

	void registerCreator(PK_GAME_STYLE gameType, GameHandlerCreator *creator);
	void unregisterCreator(PK_GAME_STYLE gameType);

private:
	std::map<PK_GAME_STYLE, GameHandlerCreator*> mCreators;
	std::map<PK_GAME_STYLE, std::list<GameHandler*> > mFreeHandlers;
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
