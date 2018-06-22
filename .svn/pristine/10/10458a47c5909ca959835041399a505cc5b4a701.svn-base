#ifndef GameHandler_h
#define GameHandler_h
#include "LBase.h"
#include "User.h"
#include "Card.h"
#include <list>

class Desk;

class GameHandler
{
public:
    virtual bool startup(Desk *game) { return false; }
    virtual void shutdown(void) {}
    virtual void gameStart(void) {}

	 virtual void	HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg){}
	 virtual void	HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg){}
	 virtual void	HanderUserEndSelect(User* pUser,LMsgC2SUserEndCardSelect* msg){}		//玩家处理海底牌
	 virtual void	HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg){}
	 virtual void	OnUserReconnect(User* pUser){}
	 virtual void	OnGameOver(Lint result,Lint winpos[],Lint bombpos, CardVector winCards[]){}
	 virtual void	SetDeskPlay(){}
	 virtual void	ProcessRobot(Lint pos, User * pUser){}
	 virtual void	SetPlayType(std::vector<Lint>& playtype){}
	 virtual void   HanderUserChoosePiao(User* pUser,LMsgC2SPiaoSelect* msg) {}
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
