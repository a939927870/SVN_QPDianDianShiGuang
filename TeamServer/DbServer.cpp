#include "LLog.h"
#include "DbServer.h"
#include "Config.h"
#include "DBSession.h"

bool DbServer::Init()
{
	return true;
}

bool DbServer::Final()
{
	return true;
}

void DbServer::Start()
{
	m_queues.clear();
	int workerNum = 4;
	for(int i = 0; i < workerNum; i++)
	{
		boost::shared_ptr<SQLQueue> queue(new SQLQueue);
		m_queues.push_back(queue);
	}

	for(int i = 0; i < workerNum; i++)
		m_threads.create_thread(boost::bind(&DbServer::ExecuteLoop, this, m_queues[i]));
}

void DbServer::Stop()
{
	for(int i = 0; i < m_queues.size(); i++)
	{
		auto queue = m_queues[i];
		if(queue)
			queue->Push(Lstring());
	}
	m_threads.join_all();
	m_queues.clear();
}

void DbServer::Push(Lstring&& sql, int threadHashValue)
{
	if(m_threads.size() == 0)
		return;

	int threadIndex = threadHashValue % m_threads.size();
	if(threadIndex >=0 && threadIndex < m_queues.size())
	{
		auto queue = m_queues[threadIndex];
		if(queue)
			queue->Push(move(sql));
	}
}

void DbServer::ExecuteLoop(boost::shared_ptr<SQLQueue> queue)
{
	DBSession session;
	if (!session.Init(gConfig.GetDbHost(), gConfig.GetDbUser(), gConfig.GetDbPass(), gConfig.GetDbName(), "utf8mb4", gConfig.GetDbPort()))
	{
		LLOG_ERROR("DbServer::ExecuteLoop: DBSession::Init error");
		return;
	}
	stringstream ss;
	ss << boost::this_thread::get_id();
	LLOG_ERROR("Thread %s:connecting to database succeed.", ss.str().c_str());

	while(true)
	{
		const Lstring sql = move(queue->Pop());
		if(!sql.empty())
			session.Excute(sql);
		else
			break;
	}
}