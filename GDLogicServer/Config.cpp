#include "Config.h"
#include "LLog.h"
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/xml_parser.hpp>  
#include <boost/typeof/typeof.hpp> 

using namespace boost::property_tree;

bool Config::Init()
{
	LoadFile("gdlogic_config.xml");
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	return m_nlogLevel;
}

Lstring Config::GetLogicManagerIp()
{
	return m_strLogicManagerIp;
}

Lshort Config::GetLogicManagerPort()
{
	return m_nsLogicManagerPort;
}

Lstring Config::GetCenterKey()
{
	return "45fcbdcbfbaa1a2f7a70df0a2a766c1d";
}

Lstring Config::GetLogicDbIp()
{
	return m_strLogicDbIp;
}

Lshort Config::GetLogicDbPort()
{
	return m_nsLogicDbPort;
}

Lstring Config::GetDBKey()
{
	return "45fcbdcbfbaa1a2f7a70df0a2a766c1d";
}

Lstring Config::GetInsideIp()
{
	return m_strInsideIp;
}

Lshort Config::GetInsidePort()
{
	return m_nsInsidePort;
}

bool Config::GetIfAddRobot()
{
	return (m_nAddRobot != 0);
}

bool Config::GetDebugModel()
{
	return (m_nDebugMod != 0);
}

Lint Config::GetServerID()
{
	return m_ServerID;
}

Lstring Config::GetServerName()
{
	return m_strServerName;
}
Lstring Config::GetRemoteLogUrl()
{
	return m_strRemoteLogUrl;
}

Lstring Config::GetRemoteLogCacheFileName()
{
	return m_strRemoteLogCacheFileName;
	//return m_ini.GetString("RemoteLogCacheFileName", "/tmp/rlog_cache.log");
}

Lint Config::GetMaxCachedLogSize()
{
	return m_nMaxCacheLogSize;
	//return m_ini.GetInt("MaxCachedLogSize", 20480);
}

Lint Config::GetMaxCachedLogNum()
{
	return m_nMaxCacheFileLogNum;
	//return m_ini.GetShort("MaxCachedLogNum", 100);
}

Lint Config::GetMaxFileLogSize()
{
	return m_nMaxFileLogSize;
	//return m_ini.GetInt("MaxFileLogSize", 10485760);
}

Lint Config::GetRemoteLogThreadStart()
{
	return m_nRemoteLogThreadStart;
	//return m_ini.GetInt("RemoteLogThreadStart", 0);
}

void Config::LoadFile(Lstring filename)
{
	ptree pt;
	read_xml(filename, pt);
	ptree root = pt.get_child("LogicServer");

	m_nlogLevel = root.get<Lint>("LogLevel", 1);
	m_ServerID = root.get<Lint>("<xmlattr>.id", 1);
	m_strServerName = root.get<std::string>("<xmlattr>.name", "Unknown Server");

	m_strInsideIp = root.get<std::string>("InsideIp", "127.0.0.1");
	m_nsInsidePort = root.get<Lshort>("InsidePort", 6001);

	m_strLogicManagerIp = root.get<std::string>("LogicManagerIp", "127.0.0.1");
	m_nsLogicManagerPort = root.get<Lshort>("LogicManagerPort", 10001);

	m_strLogicDbIp = root.get<std::string>("LogicDBIp", "127.0.0.1");
	m_nsLogicDbPort = root.get<Lshort>("LogicDBPort", 7001);
	
	m_nAddRobot = root.get<Lint>("AddRobot", 0);
	m_nDebugMod = root.get<Lint>("DebugModel", 0);

	m_strRemoteLogUrl = root.get<std::string>("RemoteLogUrl", "");
	m_nRemoteLogThreadStart = root.get<Lint>("RemoteLogThreadStart", 1);

	m_strRemoteLogCacheFileName = root.get<std::string>("RemoteLogCacheFileName", "/tmp/rlog_cache.log");
	m_nMaxCacheLogSize = root.get<Lint>("MaxCacheLogSize", 20480);
	m_nMaxCacheFileLogNum = root.get<Lint>("MaxCachedLogNum", 100);
	m_nMaxFileLogSize = root.get<Lint>("MaxFileLogSize", 10485760);

	LLOG_ERROR("LogicServer LoadFile Succeed!");
}
