#include "Config.h"
#include "LLog.h"
#include "BaseCommonFunc.h"

#define PATH_ConfigFile		"cslogic_config.ini"

bool Config::Init()
{
	m_ini.LoadFile(PATH_ConfigFile);
	m_addRobot = m_ini.GetInt("AddRobot", 0);
	m_DebugMod = m_ini.GetInt("DebugModel", 0);
	m_ServerID = m_ini.GetInt("ServerID", -1);
	m_logLevel = m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
	m_lastModifiedTime = GetLastModifiedTime(PATH_ConfigFile);
	return true;
}

bool Config::Final()
{
	return true;
}

bool Config::CheckUpdates()
{
	bool updated = false;
	std::time_t lastModifiedTime = GetLastModifiedTime(PATH_ConfigFile);
	if (lastModifiedTime > 0 && m_lastModifiedTime != lastModifiedTime)
	{
		LLOG_ERROR("config file %s was updated.", PATH_ConfigFile);
		LIniConfig ini;
		ini.LoadFile(PATH_ConfigFile);
		m_logLevel = ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
		m_lastModifiedTime = lastModifiedTime;
		updated = true;
	}
	return updated;
}

Lint Config::GetLogLevel()
{
	return m_logLevel;
}

Lstring Config::GetLogicManagerIp()
{
	return m_ini.GetString("LogicManagerIp", "127.0.0.1");
}

Lshort Config::GetLogicManagerPort()
{
	return m_ini.GetShort("LogicManagerPort", 9001);
}

Lstring Config::GetCenterKey()
{
	return m_ini.GetString("CenterKey", "45fcbdcbfbaa1a2f7a70df0a2a766c1d");
}

Lstring Config::GetDBIp()
{
	return m_ini.GetString("DBIp", "127.0.0.1");
}

Lshort Config::GetDBPort()
{
	return m_ini.GetShort("DBPort", 7001);
}

Lstring Config::GetDBKey()
{
	return m_ini.GetString("DBKey", "45fcbdcbfbaa1a2f7a70df0a2a766c1d");
}

Lstring Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp", "127.0.0.1");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort", 6001);
}

bool Config::GetIfAddRobot()
{
	return (m_addRobot != 0);
}

bool Config::GetDebugModel()
{
	return (m_DebugMod != 0);
}

Lint Config::GetServerID()
{
	return m_ServerID;
}

Lstring Config::GetServerName()
{
	return m_ini.GetString("ServerName", "Unknow Server");
}

Lstring Config::GetRemoteLogUrl()
{
	return m_ini.GetString("RemoteLogUrl", "");
}

Lstring Config::GetRemoteLogCacheFileName()
{
	return m_ini.GetString("RemoteLogCacheFileName", "/tmp/rlog_cache.log");
}

Lint Config::GetMaxCachedLogSize()
{
	// default value: 20 * 1024
	return m_ini.GetInt("MaxCachedLogSize", 20480);
}

Lshort Config::GetMaxCachedLogNum()
{
	// default value: 100
	return m_ini.GetShort("MaxCachedLogNum", 100);
}

Lint Config::GetMaxFileLogSize()
{
	// default value: 10 * 1024 * 1024
	return m_ini.GetInt("MaxFileLogSize", 10485760);
}

Lint Config::GetRemoteLogThreadStart()
{
	return m_ini.GetInt("RemoteLogThreadStart", 0);
}