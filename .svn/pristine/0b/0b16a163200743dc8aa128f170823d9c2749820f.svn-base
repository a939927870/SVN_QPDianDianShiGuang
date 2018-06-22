#ifndef _L_RUNNABLE_H_
#define _L_RUNNABLE_H_

#include "LBase.h"
#include "LSingleton.h"

/*
*	brief 线程类，封装了基本的线程操作，
*		  消息传递的机制		
*/
class LRunnable
{
public:
	LRunnable();
	virtual~LRunnable();

	/*
	*	brief 线程的基本操作
	*/
	virtual void Start();
	virtual	void Stop();
	virtual	void Join();
	virtual void Sleep(int interval);

	/*
	*	brief 线程消息的投递
	*/
	virtual void Push(void* msg);
	virtual void Swap();
	virtual void* Pop();

	virtual void Clear() = 0;

	/*
	*	brief 这个是线程运行的函数，根据具体逻辑去实现
	*/
	virtual void Run() = 0;

	virtual	bool GetStop();
	
private:
	void ThreadFun();

protected:
	boost::shared_ptr<boost::thread> m_pthread;
	std::list<void*>     m_msgQueue[2];
	std::list<void*>*    m_msgRecv;
	std::list<void*>*    m_msgDoing;
	boost::mutex         m_msgMutex;
	bool				 m_stop;
};

#endif