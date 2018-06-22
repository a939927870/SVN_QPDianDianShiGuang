#include "Config.h"
#include "LLog.h"


bool Config::Init()
{
	m_ini.LoadFile("gate_config.ini");
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetId()
{
	return m_ini.GetInt("Id", 1);
}

Lint Config::GetLogLevel()
{
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring	Config::GetOutsideIp()
{
	return m_ini.GetString("OutsideIp", "127.0.0.1");
}

Lshort Config::GetOutsidePort()
{
	return m_ini.GetShort("OutsidePort", 8001);
}

Lstring	Config::GetOutsideIp2()
{
	return m_ini.GetString("OutsideIp2", "127.0.0.1");
}

Lshort Config::GetOutsidePort2()
{
	return m_ini.GetShort("OutsidePort2", 8001);
}

Lstring	Config::GetLogicManagerIp()
{
	return m_ini.GetString("LogicManagerIp", "127.0.0.1");
}

Lshort Config::GetLogicManagerPort()
{
	return m_ini.GetShort("LogicManagerPort", 10001);
}

Lstring Config::GetLogicKey()
{
	return m_ini.GetString("LogicKey", "123231");
}