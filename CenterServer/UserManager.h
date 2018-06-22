#ifndef _USER_MANAGER_H_
#define _USER_MANAGER_H_

#include "LSingleton.h"
#include "DUser.h"

class UserManager:public LSingleton<UserManager>
{
public:
	
	virtual	bool		Init();
	virtual	bool		Final();

	DUser*				GetUserByOpenId(const Lstring& openid);

	void				DelUserByOpenId(const Lstring& openid);


	Lint				CreateUser(DUser* user);

	void				SaveUser(DUser* user);

	void                SaveUserExcepteSomeInfo(DUser* user);
	void                SaveUserGold(DUser* user);

	void				LoadUserInfo();

	void				LoadUserIdInfo(Lint serverID);
	
	void				AddUser(DUser* user);

	void				UpdateUserCoupon(const Lstring & unionId, Lint couponChanged, Lint changedReason);

	bool				HaveTeamId(Lint userId, Lint teamId);

	bool				GetTeamMember(Lint userId, Lint teamId, Lint& numOfCard1s, Ldouble& numOfCard2s, Lint& numOfCard3s, Ldouble& totalbuynum, Lint& totalplaynum);
	void				UpdateTeamMember(Lint userId, Lint teamId, Lint numOfCard1s, Ldouble numOfCard2s, Lint numOfCard3s, Ldouble totalbuynum, Lint totalplaynum);
private:
	void				UpdateUserIntoDB(DUser* user);
	void		     	SaveUserCoupon(DUser * user);
	void				InsertCouponLogToDB(DUser * user, Lint couponChanged, Lint operType);

	static	Lint		GetRandInsertIDFromDB();

private:
	std::map<Lstring, DUser*>	m_userMap;
};

#define gUserManager UserManager::Instance()

#endif