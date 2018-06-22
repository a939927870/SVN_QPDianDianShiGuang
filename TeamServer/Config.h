#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	Lint	GetLogLevel();
	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lshort	GetDbPort();
	Lint	GetTeamServerID();
	Lstring GetOutsideIp();
	Lshort	GetOutsidePort();
	Lstring GetLogicManagerIp();
	Lshort	GetLogicManagerPort();
	Lstring	GetHttpListenIp();
	Lshort	GetHttpListenPort();
	Lint	GetMemJoinTeamMaxCount();
	Lint	GetMemPerDayLimit();
	Lstring	GetAlertUrl();
	Lint	GetMaxCreateTeamCount();
	Lint	GetMaxTeamMemCount();

private:
	bool LoadFile(const Lstring& strFileName);

private:
	boost::mutex m_mutexConfig;
	LIniConfig	m_ini;

	Lint	m_nLogLevel;
	Lstring	m_strDbHost;
	Lstring	m_strDbUser;
	Lstring	m_strDbPass;
	Lstring	m_strDbName;
	Lshort	m_snDbPort;
	Lint	m_nTeamServerID;
	Lstring m_strOutsideIp;
	Lshort	m_snOutsidePort;
	Lstring m_strLogicManagerIp;
	Lshort	m_snLogicManagerPort;
	Lstring	m_strHttpListenIp;
	Lshort	m_snHttpListenPort;
	Lint	m_nMemJoinTeamMaxCount;
	Lint	m_nMemPerDayLimit;
	Lstring	m_strAlertUrl;
	Lint	m_nMaxCreateTeamCount;
	Lint	m_nMaxTeamMemCount;

};

#define gConfig Config::Instance()

#endif