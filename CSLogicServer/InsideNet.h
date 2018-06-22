#ifndef _INSIDE_NET_H_
#define _INSIDE_NET_H_


#include "LNet.h"
#include "LSingleton.h"


class InsideNet :public LNet, public LSingleton<InsideNet>
{
public:
	virtual bool Init();
	virtual bool Final();

	virtual void RecvMsgPack(LBuffPtr recv,LSocketPtr s);
};

#define gInsideNet InsideNet::Instance()

#endif