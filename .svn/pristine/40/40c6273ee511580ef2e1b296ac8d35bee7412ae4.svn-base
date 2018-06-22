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

GameHandler* GameHandlerFactory::create(PK_GAME_STYLE gameType)
{
	std::map<PK_GAME_STYLE, std::list<GameHandler*> >::iterator iHandlers = mFreeHandlers.find(gameType);
	if (iHandlers != mFreeHandlers.end())
	{
		if (iHandlers->second.size() > 0)
		{
			GameHandler *handler = iHandlers->second.front();
			iHandlers->second.pop_front();
			return handler;
		}
	}

	std::map<PK_GAME_STYLE, GameHandlerCreator*>::iterator iCreator = mCreators.find(gameType);
	if (iCreator != mCreators.end())
	{
		return iCreator->second->create();
	}
	return 0;
}
void GameHandlerFactory::destroy(PK_GAME_STYLE gameType, GameHandler *gameHandler)
{
	if (gameHandler)
	{
		gameHandler->shutdown();

		std::map<PK_GAME_STYLE, std::list<GameHandler*> >::iterator iHandlers = mFreeHandlers.find(gameType);
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

void GameHandlerFactory::registerCreator(PK_GAME_STYLE gameType, GameHandlerCreator *creator)
{
	if (creator)
	{
		mCreators[gameType] = creator;
	}
}
void GameHandlerFactory::unregisterCreator(PK_GAME_STYLE gameType)
{
	mCreators.erase(gameType);
}


#include "DDZHandler_2Player.h"
#include "DDZHandler_ArenaPVE.h"
#include "DDZHandler_classical.h"
#include "DDZHandler_SanLai.h"
#include "DDZHandler_ShanxiWakeng.h"
#include "DDZHandler_SiChuan.h"
#include "DDZHandler_SiChuan3_PiZi.h"
#include "DDZHandler_SiChuanPiZi.h"


