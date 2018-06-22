#ifndef _OUTSIDE_NET_H_
#define _OUTSIDE_NET_H_


#include "LNet.h"
#include "LSingleton.h"

class OutsideNet :public LNet, public LSingleton<OutsideNet>
{
public:
	virtual bool Init();
	virtual bool Final();

	virtual void RecvMsgPack(LBuffPtr recv,LSocketPtr s);

};

#define gOutsideNet OutsideNet::Instance()

//////////////////////////////////////////////////////////////////////////
class HttpNet :public LNet, public LSingleton<HttpNet>
{
public:
	virtual bool Init();
	virtual bool Final();

	virtual void RecvMsgPack(LBuffPtr recv, LSocketPtr s);
};

#define gHttpNet HttpNet::Instance()

#endif