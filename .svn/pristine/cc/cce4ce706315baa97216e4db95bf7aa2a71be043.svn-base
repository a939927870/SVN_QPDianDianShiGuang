#include "GarbageCollectionService.h"
#include "LMsg.h"

GarbageCollectionService::GarbageCollectionService()
	: m_running(false)
{
}

GarbageCollectionService::~GarbageCollectionService()
{
	Stop();
}

void GarbageCollectionService::Start()
{
	if (!m_running)
	{
		int workerNum = 1;
		for (int i = 0; i < workerNum; i++)
			m_threads.create_thread(boost::bind(&GarbageCollectionService::CollectionLoop, this));
		m_running = true;
	}
}

void GarbageCollectionService::Stop()
{
	if (m_running)
	{
		for (size_t i = 0; i < m_threads.size(); i++)
			m_queue.Push(NULL);

		m_threads.join_all();

		m_running = false;
	}
}

void GarbageCollectionService::Collect(LMsg * msg)
{
	if(m_running)
		m_queue.Push(msg);
}

void GarbageCollectionService::CollectionLoop()
{
	while (true)
	{
		LMsg* msg = m_queue.Pop();
		if (msg != NULL)
			delete msg;
		else
			break;
	}
}