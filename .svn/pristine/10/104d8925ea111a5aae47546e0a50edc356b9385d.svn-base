#ifndef __WeiXinAuthService_
#define __WeiXinAuthService_

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "CircularArrayQueue.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LSocket.h"
#include "LSingleton.h"

struct AuthenticationContext
{
	Lint			idOnGate;
	LMsgC2SMsg*		msg;
	LSocketPtr		sp;
};

class WeiXinAuthService : public LSingleton<WeiXinAuthService>
{
    typedef CircularArrayQueue<LMsg *> MsgQueue;
    typedef boost::shared_ptr<MsgQueue> MsgQueuePtr;
public:
	WeiXinAuthService();
	~WeiXinAuthService();

	virtual bool Init();
	virtual bool Final();

public:
	void		Start();
	void		Stop();
	void Push(LMsg *msg);
	void HandleMsg(LMsg *msg);
	//void Send(LMsg &send);
	//void		SubmitAuthentication(AuthenticationContext* msg);

private:
	void            HandleUserAuth(LMsgLGCEUserMsgWrapper *msg);
	void		AuthenticationLoop(MsgQueuePtr queue);
	bool		DoAuthentication(LMsgC2SMsg* msg);
	bool		DoWeiXinAuthentication(LMsgC2SMsg * msg);

private:
	bool m_running;
	boost::thread_group		m_threads;
	std::vector<MsgQueuePtr>   m_queues;
	int m_workerNum;
};


#endif	// __WeiXinAuthService_
