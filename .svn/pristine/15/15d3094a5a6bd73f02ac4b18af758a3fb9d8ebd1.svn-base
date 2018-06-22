#ifndef __DBSession_
#define __DBSession_

#include <boost/thread/mutex.hpp>
#include "LBase.h"

class DBSession
{
public:
	DBSession();
	~DBSession();

	bool Init(const Lstring& host, const Lstring& user, const Lstring& pass, const Lstring& dbname, const Lstring&  charset, Lshort port);

	MYSQL* GetMysql();

	bool	Excute(const std::string& sql);

private:
	Lstring m_Host;
	Lstring m_User;
	Lstring m_Pass;
	Lstring m_DBName;
	Lstring m_CharSet;
	Lshort  m_Port;
	MYSQL*  m_Mysql;
	static boost::mutex	m_initLock;
};

#endif		// __DBSession_