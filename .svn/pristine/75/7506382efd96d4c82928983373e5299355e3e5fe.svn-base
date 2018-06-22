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
private:
	void		CreateMsgIdTable();
	bool CheckMsgHeader(LSocketPtr s, const MsgHeader& header);
	bool IsValidMsgId(Lint id);
	std::unordered_set<Lint>		m_msgIdTable;
};

#define gOutsideNet OutsideNet::Instance()

#endif