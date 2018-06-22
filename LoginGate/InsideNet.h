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
	bool CheckMsgHeader(LSocketPtr s, const MsgHeader& header);
	bool IsValidMsgId(Lint id);
};

#define gOutsideNet OutsideNet::Instance()

#endif