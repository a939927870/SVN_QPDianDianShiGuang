#include "Config.h"
#include "LLog.h"
#include "BaseCommonFunc.h"
#include "Work.h"
#include "boost/regex.hpp"

const char  PATH_ConfigFile[64] = "logic_manager_config.ini";

bool Config::Init()
{
	m_ini.LoadFile("logic_manager_config.ini");
	m_free = m_ini.GetInt("Free", 0);
	m_DebugMod = m_ini.GetInt("DebugModel", 0);
	m_ServerID = m_ini.GetInt("ServerID", -1);
	m_OpenShareGet1Card = m_ini.GetInt("OpenShardGet1Card", 0);
	m_proportionate = m_ini.GetInt("Proportionate", 20);
	return true;
}

bool Config::Final()
{
	return true;
}

Lint Config::GetLogLevel()
{
	//默认debug log;
	return m_ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
}

Lstring Config::GetCenterIp()
{
	return m_ini.GetString("CenterIp", "127.0.0.1");
}

Lshort Config::GetCenterPort()
{
	return m_ini.GetShort("CenterPort", 5001);
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

void Config::SetFree(Lint free)
{
	m_free = free;
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

Lint Config::GetLogic4GoldGame()
{
	return m_Logic4GoldGame;
}

bool Config::CheckUpdates()
{
	bool updated = false;
	std::time_t lastModifiedTime = GetLastModifiedTime(PATH_ConfigFile);

	
	Lint today = gWork.GetCurTime().GetDate();
	Lint nowTime = gWork.GetCurTime().GetTime(); 

	static Lint bfree = 0;
	static Lint StartDay = 0;
	static Lint EndDay = 22000101;
	static Lint StartTime = 0;
	static Lint EndTime = 240000;
	if (lastModifiedTime > 0 && m_lastModifiedTime != lastModifiedTime)
	{
		LLOG_ERROR("config file %s was updated.", PATH_ConfigFile);
		LIniConfig ini;
		ini.LoadFile(PATH_ConfigFile);
		m_logLevel = ini.GetInt("LogLevel", LLOG_LEVEL_DEBUG);
		m_OpenShareGet1Card = m_ini.GetInt("OpenShardGet1Card", 0);

		bfree = ini.GetInt("Free", 0);
		StartDay = ini.GetInt("freeStartDay", 0);
		EndDay = ini.GetInt("freeEndDay", 22000101);
		StartTime = ini.GetInt("freeStartTime", 0);
		EndTime = ini.GetInt("freeEndTime", 240000);
		m_failoverLogics = ini.GetString("FailoverLogics", "");
		m_lastModifiedTime = lastModifiedTime;
		m_proportionate = ini.GetInt("Proportionate", 20);

		// 玩法免费配置
		LoadGameTypeFree(ini);

		//logic分配设定;
		m_Logic4GoldGame = ini.GetInt("logic4GoldGame", -1);
		updated = true;
	}
	if (1 == bfree && today >= StartDay && today <= EndDay && nowTime >= StartTime && nowTime <= EndTime)  //分时段每日免费模式；
	{
		m_free = 1;
	}
	else if (2 == bfree &&    //跨多日连续免费模式;
		(today > StartDay ||  (today == StartDay && nowTime > StartTime) ) &&  //确保在StartDay日StartTime之后；
		(today < EndDay || (today == EndDay && nowTime < EndTime) )            //及EndDay日EndTime之前；
		)
	{
		m_free = 1;
	}
	else
	{
		m_free = 0;
	}
	return updated;
}

void Config::LoadGameTypeFree(LIniConfig& ini)
{
	std::list<std::string> fields;
	const std::string line = move(ini.GetString("FreeGameTypeList", ""));

	//热免費；
	const char* comma_pattern =
		"(?!\\s*$)"
		"\\s*"
		"(?:"
		"'([^']*(?:[\\S\\s][^']*)*?)'"
		"|\"([^\"]*(?:[\\S\\s][^\"]*)*?)\""
		"|([^,'\"]*\\w[^,'\"]*?)"
		")"
		"\\s*"
		"(?:,|$)";
	try {
		boost::regex pattern(comma_pattern);
		auto beg = boost::sregex_iterator(line.begin(), line.end(), pattern);
		auto end = boost::sregex_iterator();
		for (auto i = beg; i != end; ++i) {
			auto it = ++i->begin();
			auto iend = i->end();
			for (; it != iend; ++it)
				if (it->length() > 0) {
					fields.push_back(*it);
					break;
				}
		}
	}
	catch (boost::regex_error& e) {
		LLOG_ERROR("%s\n", e.what());
		return;
	}
	std::list<Lint> ids;	//short,no need map
	if (!fields.empty())
		for (auto i = fields.begin(), iend = fields.end(); i != iend; ++i)
			try {
			ids.push_back(atoi(i->c_str()));
		}
	catch (...) {}

	m_gameTypeFree.clear();
	for (auto i = ids.begin(), iend = ids.end(); i != iend; ++i)
	{
		if (*i >= 0 && *i < 100)   //有效GameID;
			m_gameTypeFree.insert(std::make_pair(*i, 1));
	}
}
Lint Config::GetGameTypeFree(Lint GameType)
{
	auto itFind = m_gameTypeFree.find(GameType);
	if (itFind == m_gameTypeFree.end())
		return 0;
	return itFind->second;
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

Lint Config::GetShareGet1Card() 
{
	Lint date = gWork.GetCurTime().GetDate();
	if (date >= 20170124 && date <= 20170202)
	{
		return 1;
	}
	return m_OpenShareGet1Card; 
}