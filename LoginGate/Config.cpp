#include "Config.h"
#include "LLog.h"


bool Config::Init()
{
	m_ini.LoadFile("logingate_config.ini");
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

Lstring Config::GetOutsideIp()
{
	return m_ini.GetString("OutsideIp", "127.0.0.1");
}

Lshort	Config::GetOutsidePort()
{
	return m_ini.GetShort("OutsidePort", 5001);
}

Lstring Config::GetCenterIp()
{
	return m_ini.GetString("CenterIp", "127.0.0.1");
}

Lshort	Config::GetCenterPort()
{
	return m_ini.GetShort("CenterPort", 5001);
}

