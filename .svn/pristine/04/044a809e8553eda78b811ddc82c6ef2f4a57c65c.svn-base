#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();

	bool			CheckUpdates();


	Lint	GetLogLevel();

	Lstring GetLogicManagerIp();
	Lshort GetLogicManagerPort();
	Lstring GetCenterKey();


	Lstring GetDBIp();
	Lshort GetDBPort();
	Lstring GetDBKey();


	Lstring GetInsideIp();
	Lshort GetInsidePort();
	bool	GetIfAddRobot();
	bool	GetDebugModel();
	Lint	GetServerID();
	Lstring	GetServerName();

	//remote log
	Lstring GetRemoteLogUrl();
	Lstring GetRemoteLogCacheFileName();
	Lint GetMaxCachedLogSize();
	Lshort GetMaxCachedLogNum();
	Lint GetMaxFileLogSize();
	Lint GetRemoteLogThreadStart();
private:
	LIniConfig	m_ini;


private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;
	Lint		m_addRobot;						//是否自动添加机器人
	Lint		m_DebugMod;
	Lint		m_ServerID;						//服务器ID
	Lint		m_logLevel;
	std::time_t		m_lastModifiedTime;
};

#define gConfig Config::Instance()

#endif