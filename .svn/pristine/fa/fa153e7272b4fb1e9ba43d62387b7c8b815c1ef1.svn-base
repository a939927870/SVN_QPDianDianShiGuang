#include "Config.h"
#include "LLog.h"


bool Config::Init()
{
	m_ini.LoadFile("center_config.ini");
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring	Config::GetDbHost()
{
	return m_ini.GetString("DbHost", "192.168.1.139");
}

Lstring	Config::GetDbUser()
{
	return m_ini.GetString("DbUser", "root");
}

Lstring	Config::GetDbPass()
{
	return m_ini.GetString("DbPass", "");
}

Lstring	Config::GetDbName()
{
	return m_ini.GetString("DbName", "test");
}

Lshort	Config::GetDbPort()
{
	return m_ini.GetShort("DbPort", 3306);
}


Lstring	Config::GetInsideIp()
{
	return m_ini.GetString("InsideIp","127.0.0.1");
}

Lshort Config::GetInsidePort()
{
	return m_ini.GetShort("InsidePort",2001);
}

Lstring	Config::GetGMIp()
{
	return m_ini.GetString("GMIp", "127.0.0.1");
}

Lshort Config::GetGMPort()
{
	return m_ini.GetShort("GMPort", 9001);
}

Lstring Config::GetLocalLogin()
{
	return m_ini.GetString("LocalLogin", "false");
}

Lstring	Config::GetCenterKey()
{
	return m_ini.GetString("CenterKey", "45fcbdcbfbaa1a2f7a70df0a2a766c1d");
}
Lint Config::GetWxAuthSwitch()
{
	return m_ini.GetInt("WxAuth", 0);;
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