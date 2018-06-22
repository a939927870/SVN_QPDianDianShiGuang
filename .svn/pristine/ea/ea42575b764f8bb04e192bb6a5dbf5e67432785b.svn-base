#include "Config.h"
#include "LLog.h"

bool Config::Init()
{
	m_ini.LoadFile("dzlogic_config.ini");
	m_free = m_ini.GetInt("Free", 0);
	m_addRobot = m_ini.GetInt("AddRobot", 0);
	m_DebugMod = m_ini.GetInt("DebugModel", 0);
	m_ServerID = m_ini.GetInt("ServerID", -1);
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	//Ä¬ÈÏdebug log
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
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

bool Config::GetFree()
{
	return m_free == 1;
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

Lint Config::GetRandomCardMode()
{
	return m_ini.GetShort("RandomCardMode", 0);
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
