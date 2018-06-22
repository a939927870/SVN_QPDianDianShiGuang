#ifndef _CONFIG_H_
#define _CONFIG_H_
#include "LSingleton.h"

//class Config :public LSingleton<Config>
//{
//public:
//	virtual	bool	Init();
//
//	virtual	bool	Final();
//
//
//	Lint	GetLogLevel();
//
//	Lstring GetLogicManagerIp();
//	Lshort GetLogicManagerPort();
//	Lstring GetCenterKey();
//
//
//	Lstring GetDBIp();
//	Lshort GetDBPort();
//	Lstring GetDBKey();
//
//
//	Lstring GetInsideIp();
//	Lshort GetInsidePort();
//	bool	GetIfAddRobot();
//	bool	GetDebugModel();
//	Lint	GetServerID();
//	
//	Lstring GetRemoteLogUrl();
//	Lstring GetRemoteLogCacheFileName();
//	Lint GetMaxCachedLogSize();
//	Lshort GetMaxCachedLogNum();
//	Lint GetMaxFileLogSize();
//	Lint GetRemoteLogThreadStart();
//	
//	Lstring	GetServerName();
//private:
//	LIniConfig	m_ini;
//
//
//private:
//	Lshort		m_inPort;
//	Lshort		m_outPort;
//
//	Lstring		m_inIp;
//	Lstring		m_outIp;
//	Lint		m_addRobot;						//是否自动添加机器人
//	Lint		m_DebugMod;

//};

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	Lint	GetId();
	Lint	GetLogLevel();

	Lstring	GetLogicManagerIp();
	Lshort	GetLogicManagerPort();
	Lstring GetInsideIp();
	Lshort GetInsidePort();
	Lstring	GetOutsideIp();
	Lshort	GetOutsidePort();
	Lstring GetCenterKey();
	
	Lstring GetLogicDbIp();
	Lshort GetLogicDbPort();
	Lstring GetDBKey();
	
	bool	GetIfAddRobot();
	bool	GetDebugModel();
	Lint	GetServerID();
		
	Lstring GetRemoteLogUrl();
	Lstring GetRemoteLogCacheFileName();
	Lint GetMaxCachedLogSize();
	Lint GetMaxCachedLogNum();
	Lint GetMaxFileLogSize();
	Lint GetRemoteLogThreadStart();
	Lstring	GetServerName();

private:
	void LoadFile(Lstring filename);

private:
	Lint		m_ServerID;
	Lint		m_nlogLevel;
	Lshort		m_nsInsidePort;
	Lstring		m_strInsideIp;
	Lstring		m_strLogicManagerIp;
	Lshort		m_nsLogicManagerPort;
	Lstring		m_strLogicDbIp;
	Lshort		m_nsLogicDbPort;
	Lint		m_nAddRobot;
	Lint		m_nDebugMod;
	Lstring		m_strRemoteLogUrl;
	Lstring		m_strRemoteLogCacheFileName;
	Lint		m_nMaxCacheLogSize;
	Lint		m_nMaxCacheFileLogNum;
	Lint		m_nMaxFileLogSize;
	Lint		m_nRemoteLogThreadStart;
	Lstring		m_strServerName;
};

#define gConfig Config::Instance()

#endif