#ifndef _D_USER_H_
#define _D_USER_H_


#include "LUser.h"

class DUser
{
public:
	DUser();
	~DUser();

	bool	GetCopy();
	void	SetCopy(bool copy);
	
public:
	LUser m_usert;
	bool  m_hasCopy;		//在logic服务器是否有备份
	Lint  m_lastReqTime;	//最近一次请求登录的时间
	Lint  m_logicID;		//玩家当前所在服务器Index
};


#endif