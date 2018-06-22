#include "LRunnable.h"


LRunnable::LRunnable()
{
	m_msgDoing = &m_msgQueue[0];
	m_msgRecv  = &m_msgQueue[1];
	m_stop = false;
}

LRunnable::~LRunnable()
{
}

void LRunnable::Start()
{
	if(!m_pthread)
	{
		m_pthread = boost::shared_ptr<boost::thread> (new boost::thread(boost::bind(&LRunnable::ThreadFun,this)));
	}
}

void LRunnable::Stop()
{
	m_stop = true;
}

void LRunnable::Join()
{
	if(m_pthread)
	{
		m_pthread->join();
	}
}

void LRunnable::Push(void* msg)
{
	boost::mutex::scoped_lock l(m_msgMutex);
	m_msgRecv->push_back(msg);
}

void LRunnable::Swap()
{
	boost::mutex::scoped_lock l(m_msgMutex);
	std::list<void*>* tmp = m_msgDoing;
	m_msgDoing = m_msgRecv;
	m_msgRecv = tmp;
}

void* LRunnable::Pop()
{
	void* pack = NULL;
	if(m_msgDoing->empty())
	{
		Swap();
	}

	if(m_msgDoing->empty())
	{
		return pack;
	}

	pack = m_msgDoing->front();
	m_msgDoing->pop_front();
	return pack;
}

bool LRunnable::GetStop()
{
	return m_stop;
}

void LRunnable::ThreadFun()
{
	Run();
}

void LRunnable::Sleep(int interval)
{
	boost::this_thread::sleep(boost::posix_time::millisec(interval));  
}