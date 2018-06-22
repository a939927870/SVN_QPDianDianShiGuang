#include "GameHandler.h"


GameHandlerFactory::GameHandlerFactory(void)
{
}
GameHandlerFactory::~GameHandlerFactory(void)
{
}

GameHandlerFactory* GameHandlerFactory::getInstance(void)
{
    static GameHandlerFactory sInstance;
    return &sInstance;
}

GameHandler* GameHandlerFactory::create(GameType gameType)
{
    std::map<GameType, std::list<GameHandler*> >::iterator iHandlers = mFreeHandlers.find(gameType);
    if (iHandlers != mFreeHandlers.end())
    {
        if (iHandlers->second.size() > 0)
        {
            GameHandler *handler = iHandlers->second.front();
            iHandlers->second.pop_front();
            return handler;
        }
    }

    std::map<GameType, GameHandlerCreator*>::iterator iCreator = mCreators.find(gameType);
    if (iCreator != mCreators.end())
    {
        return iCreator->second->create();
    }
    return 0;
}
void GameHandlerFactory::destroy(GameType gameType, GameHandler *gameHandler)
{
    if (gameHandler)
    {
        gameHandler->shutdown();

        std::map<GameType, std::list<GameHandler*> >::iterator iHandlers = mFreeHandlers.find(gameType);
        if (iHandlers != mFreeHandlers.end())
        {
            iHandlers->second.push_back(gameHandler);
        }
        else
        {
            std::list<GameHandler*> handlers;
            handlers.push_back(gameHandler);
            mFreeHandlers[gameType] = handlers;
        }
    }
}

void GameHandlerFactory::registerCreator(GameType gameType, GameHandlerCreator *creator)
{
    if (creator)
    {
        mCreators[gameType] = creator;
    }
}
void GameHandlerFactory::unregisterCreator(GameType gameType)
{
    mCreators.erase(gameType);
}
