#ifndef __BlockingQueue_
#define __BlockingQueue_

#include <list>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/lock_guard.hpp> 
#include <boost/thread/condition_variable.hpp>
#include "LBase.h"

using namespace std;

template<typename T>
class BlockingQueue
{
public:
	BlockingQueue(int capacity = -1)
	{
		m_capacity = capacity > 0 ? capacity : INT_MAX;
	}
	~BlockingQueue(void){}

public:
	void Push(const T& element)
	{
		boost::lock_guard<boost::mutex> guard(m_lock);
		m_notFull.wait(m_lock, [this]() {return this->m_queue.size() < this->m_capacity;});
		m_queue.push_front(element);
		m_notEmpty.notify_all();
	}

	T Pop(void)
	{
		boost::lock_guard<boost::mutex> guard(m_lock);
		m_notEmpty.wait(m_lock, [this]() {return !this->m_queue.empty();});
		T element = move(m_queue.back());
		m_queue.pop_back();
		m_notFull.notify_all();
		return move(element);
	}

	void Clear(void)
	{
		boost::lock_guard<boost::mutex> guard(m_lock);
		while (!m_queue.empty)
		{
			m_queue.pop_front();
			m_notFull.notify_all();
		}
	}

	Lsize GetElementNum(void) const
	{
		boost::lock_guard<boost::mutex> guard(m_lock);
		return m_queue.size();
	}

private:
	Lsize								m_capacity;
	list<T>								m_queue;
	boost::mutex						m_lock;
	boost::condition_variable_any		m_notEmpty;
	boost::condition_variable_any		m_notFull;
};

#endif	// __BlockingQueue_
