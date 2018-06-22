#ifndef __GarbageCollectionService_
#define __GarbageCollectionService_

#include "BlockingQueue.h"

struct LMsg;

class GarbageCollectionService
{
public:
	GarbageCollectionService();
	~GarbageCollectionService();

public:
	void	Start();
	void	Stop();
	void	Collect(LMsg* msg);

private:
	void	CollectionLoop();

private:
	bool						m_running;
	boost::thread_group			m_threads;
	BlockingQueue<LMsg*>		m_queue;
};

#endif	// __GarbageCollectionService_