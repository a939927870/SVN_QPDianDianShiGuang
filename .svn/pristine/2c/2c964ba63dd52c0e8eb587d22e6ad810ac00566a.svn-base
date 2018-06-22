#ifndef __ALERT_WORK_H__
#define __ALERT_WORK_H__
#include "LRunnable.h"
#include "LSingleton.h"
#include "GarbageCollectionService.h"
#include "LMsg.h"

class CAlertWork : public LRunnable, public LSingleton<CAlertWork>
{
public:
	CAlertWork();
	virtual ~CAlertWork();

	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();
protected:
	void			HanderMsg(LMsg* msg);

	void			HanderTeamCardAlert(LMsgTeamCardAlert* msg);
private:
	GarbageCollectionService m_gcService;
};

#define gAlertWork CAlertWork::Instance()

#endif
