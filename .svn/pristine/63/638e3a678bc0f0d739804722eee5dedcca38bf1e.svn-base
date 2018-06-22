#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	Config() : m_OpenShareGet1Card(0) {}
	~Config() {}

	virtual	bool	Init();

	virtual	bool	Final();

	bool CheckUpdates();


	Lint	GetLogLevel();

	Lstring GetCenterIp();
	Lshort GetCenterPort();
	Lstring GetCenterKey();


	Lstring GetDBIp();
	Lshort GetDBPort();
	Lstring GetDBKey();


	Lstring GetInsideIp();
	Lshort GetInsidePort();
	bool	GetFree();
    void	SetFree(Lint free);
	bool	GetDebugModel();
	Lint	GetServerID();
	Lstring	GetServerName();
	Lint    GetLogic4GoldGame();

	Lstring GetRemoteLogUrl();
	Lstring GetRemoteLogCacheFileName();
	Lint GetMaxCachedLogSize();
	Lshort GetMaxCachedLogNum();
	Lint GetMaxFileLogSize();
	Lint GetRemoteLogThreadStart();

	void    SetShareGet1Card(Lint bopen) { m_OpenShareGet1Card = bopen; };
	Lint	GetShareGet1Card();

	Lint    GetGameTypeFree(Lint GameType);
	Lint    GetProportionate() { return m_proportionate; }

protected:
	void    LoadGameTypeFree(LIniConfig& ini);

private:
	LIniConfig	m_ini;


private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;
	Lint		m_free;
	Lint		m_DebugMod;
	Lint		m_ServerID;						//服务器ID;
	std::time_t		m_lastModifiedTime = 0;
	Lint			m_logLevel;
	Lstring			m_failoverLogics;
	Lint            m_Logic4GoldGame;

	Lint        m_OpenShareGet1Card;

	std::map<Lint, Lint>  m_gameTypeFree;       //分服免費；
	Lint       m_proportionate = 20; //抽成比例
};

#define gConfig Config::Instance()

#endif