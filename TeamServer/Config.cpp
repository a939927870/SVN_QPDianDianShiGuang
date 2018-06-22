#include "Config.h"
#include "LLog.h"

#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/xml_parser.hpp>  
#include <boost/typeof/typeof.hpp> 

using namespace boost::property_tree;

bool Config::Init()
{
	const Lstring strFileName = "teamserver_config.xml";
	LoadFile(strFileName);
	return true;
}

bool Config::LoadFile(const Lstring& strFileName)
{
	ptree pt;
	read_xml(strFileName, pt);
	ptree root = pt.get_child("TeamServer");

	m_nTeamServerID = root.get<Lint>("Id", 1);
	m_nLogLevel = root.get<Lint>("LogLevel", 1);
	m_strDbHost = root.get<std::string>("DbHost", "127.0.0.1");
	m_strDbUser = root.get<std::string>("DbUser", "root");
	m_strDbPass = root.get<std::string>("DbPassword", "123456");
	m_strDbName = root.get<std::string>("DbName", "game");
	m_snDbPort = root.get<Lshort>("DbPort", 3306);

	m_strOutsideIp = root.get<std::string>("OutsideIp", "127.0.0.1");
	m_snOutsidePort = root.get<Lshort>("OutsidePort", 2001);

	m_strLogicManagerIp = root.get<std::string>("LogicManagerIp", "127.0.0.1");
	m_snLogicManagerPort = root.get<Lshort>("LogicManagerPort", 100001);

	m_strHttpListenIp = root.get<std::string>("HttpIp", "127.0.0.1");
	m_snHttpListenPort = root.get<Lshort>("HttpPort", 9002);

	m_strAlertUrl = root.get<std::string>("AlterUrl", "");

	m_nMemJoinTeamMaxCount = root.get<Lint>("MemJoinTeamMaxCount", 0);
	m_nMemPerDayLimit = root.get<Lint>("MemPerDayLimit", 0);
	m_nMaxCreateTeamCount = root.get<Lint>("MaxCreateTeamCount", 0);
	m_nMaxTeamMemCount = root.get<Lint>("MaxTeamMemCount", 0);

	//LLOG_ERROR("TeamServer LoadFile Succeed!");
	return false;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	return m_nLogLevel;
}

Lstring	Config::GetDbHost()
{
	return m_strDbHost;
}

Lstring	Config::GetDbUser()
{
	return m_strDbUser;
}

Lstring	Config::GetDbPass()
{
	return m_strDbPass;
}

Lstring	Config::GetDbName()
{
	return m_strDbName;
}

Lshort Config::GetDbPort()
{
	return m_snDbPort;
}

Lint Config::GetTeamServerID()
{
	return m_nTeamServerID;
}

Lstring Config::GetOutsideIp()
{
	return m_strOutsideIp;
}

Lshort	Config::GetOutsidePort()
{
	return m_snOutsidePort;
}

Lstring Config::GetLogicManagerIp()
{
	return m_strLogicManagerIp;
}

Lshort	Config::GetLogicManagerPort()
{
	return m_snLogicManagerPort;
}

Lstring	Config::GetHttpListenIp()
{
	return m_strHttpListenIp;
}

Lshort Config::GetHttpListenPort()
{
	return m_snHttpListenPort;
}

Lint Config::GetMemJoinTeamMaxCount()
{
	return m_nMemJoinTeamMaxCount;
}

Lint Config::GetMemPerDayLimit()
{
	return m_nMemPerDayLimit;
}

Lstring	Config::GetAlertUrl()
{
	//return m_ini.GetString("AlertUrl", "http://172.16.81.129:8080/gonghui/gonghui/team/sendAlertSms");
	return m_strAlertUrl;
}

Lint Config::GetMaxCreateTeamCount()
{
	return m_nMaxCreateTeamCount;
}

Lint Config::GetMaxTeamMemCount()
{
	return m_nMaxTeamMemCount;
}