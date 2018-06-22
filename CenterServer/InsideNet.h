#ifndef _INSIDE_NET_H_
#define _INSIDE_NET_H_


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


class GmNet :public LNet, public LSingleton<GmNet>
{
public:
	virtual bool Init();
	virtual bool Final();

	virtual void RecvMsgPack(LBuffPtr recv, LSocketPtr s);
};

#define gGmNet GmNet::Instance()

#endif