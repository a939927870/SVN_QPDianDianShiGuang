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

void GameHandler::AddHuInfoLog(const VideoLog& log, const LMsgS2CGameOver& over)
{
	stringstream ss;
	for (Lint nWinPos = 0; nWinPos < 4; ++nWinPos)
	{
		if (over.m_cs_win[nWinPos] == WIN_SUB_ZIMO || over.m_cs_win[nWinPos] == WIN_SUB_BOMB || over.m_cs_win[nWinPos] == WIN_SUB_DZIMO || over.m_cs_win[nWinPos] == WIN_SUB_DBOMB)
		{
			ss.str("");
			ss << "AddHuInfoLog ";
			ss << log.m_deskId << ";" << log.m_flag << ";" << log.m_userId[nWinPos] << ";";
			for (auto& hu : over.m_hu[nWinPos])
			{
				ss << hu;
				if (hu != over.m_hu[nWinPos].back())
				{
					ss << ",";
				}
			}
			ss << ";";

			LLOG_ERROR(ss.str().c_str());
		}
	}


}

#include "GuangdongJihuHandler.h"
