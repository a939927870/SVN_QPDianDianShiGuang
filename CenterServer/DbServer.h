#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#include "LBase.h"
#include "LSingleton.h"
#include "BlockingQueue.h"

class DbServer : public LSingleton<DbServer>
{
public:
	virtual bool		Init();
	virtual bool		Final();

public:
	void				Start();
	void				Stop();
	void				Push(Lstring&& sql, int threadHashValue);

private:
	typedef BlockingQueue<Lstring>				SQLQueue;

private:
	void				ExecuteLoop(boost::shared_ptr<SQLQueue> queue);

private:
	boost::thread_group							m_threads;

	std::vector<boost::shared_ptr<SQLQueue>>	m_queues;
};

#define gDbServer DbServer::Instance()

#endif