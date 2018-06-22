#include "UserManager.h"
#include "DbServer.h"
#include "LLog.h"
#include "LMsgS2S.h"
#include "Work.h"

bool UserManager::Init()
{
	LoadUserInfo();
	return true;
}

void UserManager::LoadUserIdInfo(Lint serverID)
{
	LTime cur;

	int iUserCount = 0;
 	std::map<Lstring, DUser*>::iterator it = m_userMap.begin();
	while(it != m_userMap.end())
	{
		int iCount = 0;
		LMsgCe2LUserIdInfo send;
		for(; it != m_userMap.end() && iCount < 50; ++it)
		{
			UserIdInfo info;
			auto& usert = it->second->m_usert;
			info.m_id = usert.m_id;
			info.m_nike = usert.m_nike;
			info.m_headImageUrl = usert.m_headImageUrl;
			info.m_unionId = usert.m_unioid;
			info.m_sex = usert.m_sex;
			send.m_info.push_back(info);

			iCount++;
			iUserCount++;
		}

		send.m_count = iCount;
		
		gWork.SendMsgToLogic(send, serverID, 2000);
	}

	LTime now;
	LLOG_ERROR("complet load user info. time = %ld, UserCount = %ld", now.Secs() - cur.Secs(), iUserCount);
}

void UserManager::LoadUserInfo()
{
	//加载所有的玩家
	//DbTool tool;
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::LoadUserInfo mysql null");
		return;
	}

	Lint limitfrom = 0;
	Lint count = 500000;

	while(true)
	{
		std::stringstream ss;
		ss << "select Id,OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,PlayerType,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,DiamondNum,CouponNum, Gold,NewGolduser, RemainingGifts, LastGiftTime FROM user ORDER BY Id DESC LIMIT ";
		ss << limitfrom << "," << count;

		if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
		{
			LLOG_ERROR("UserManager::LoadUserInfo sql error %s", mysql_error(m));
			break;
		}

		MYSQL_RES* res = mysql_store_result(m);
		MYSQL_ROW row = mysql_fetch_row(res);
		if(!row)
		{
			break;
		}

		while(row)
		{
			++limitfrom;
			DUser* user = new DUser();
			user->m_usert.m_id = atoi(*row++);
			user->m_usert.m_openid = *row++;
			user->m_usert.m_nike = *row++;
			user->m_usert.m_sex = atoi(*row++);
			user->m_usert.m_provice = *row++;
			user->m_usert.m_city = *row++;
			user->m_usert.m_country = *row++;
			user->m_usert.m_headImageUrl = *row++;
			user->m_usert.m_unioid = *row++;
			user->m_usert.m_playerType = *row++;
			user->m_usert.m_numOfCard1s = atoi(*row++);
			user->m_usert.m_numOfCard2s = atoi(*row++);
			user->m_usert.m_numOfCard3s = atoi(*row++);
			user->m_usert.m_lastLoginTime = atoi(*row++);
			user->m_usert.m_regTime = atoi(*row++);
			user->m_usert.m_new = atoi(*row++);
			user->m_usert.m_gm = atoi(*row++);
			user->m_usert.m_totalbuynum = atoi(*row++);
			user->m_usert.m_totalplaynum = atoi(*row++);
			user->m_usert.m_diamondNum = atoi(*row++);
			user->m_usert.m_couponNum = atoi(*row++);
			user->m_usert.m_Gold = atoi(*row++);
			user->m_usert.m_NewGolduser = atoi(*row++);
			user->m_usert.m_RemainingGifts = atoi(*row++);
			user->m_usert.m_LastGiftTime = atoi(*row++);
			AddUser(user);
			row = mysql_fetch_row(res);
		}
		mysql_free_result(res);
		LLOG_ERROR("UserManager::get user count %d", limitfrom);
	}

	LLOG_ERROR("UserManager::LoadUserIdInfo user count %d", limitfrom);
}

bool UserManager::Final()
{
	return true;
}

void UserManager::SaveUser(DUser* user)
{
	//DbTool tool;
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::SaveUser mysql null");
		return;
	}

	UpdateUserIntoDB(user);
}

DUser* UserManager::GetUserByOpenId(const Lstring& openid)
{
	if (m_userMap.count(openid))
	{
		LLOG_DEBUG("UserManager::GetUserByOpenId %s", openid.c_str());
		return m_userMap[openid];
	}

	LLOG_DEBUG("UserManager::GetUserByOpenId null %s", openid.c_str());
	return NULL;
}

void UserManager::DelUserByOpenId(const Lstring& openid)
{
	if (m_userMap.count(openid))
	{
		m_userMap.erase(m_userMap.find(openid));
	}
}

Lint UserManager::CreateUser(DUser* user)
{
	//DbTool tool;
	MYSQL* m = gWork.GetDbSession().GetMysql();

	if (m == NULL)
	{
		LLOG_ERROR("UserManager::CreateUser mysql null");
		return 0;
	}

	//指定随机的id	
	Lint nInsertID = GetRandInsertIDFromDB();
	if (nInsertID == 0)
	{
		LLOG_ERROR("UserManager::RandInsertID==0");
		return 0;
	}

	//插入数据库
	std::stringstream ss;
	ss << "INSERT INTO user (Id,OpenId,Nike,Sex,Provice,City,Country,HeadImageUrl,UnionId,PlayerType,NumsCard1,NumsCard2,NumsCard3,LastLoginTime,RegTime,New,Gm,TotalCardNum,TotalPlayNum,DiamondNum,CouponNum) VALUES (";
	ss << "'" << nInsertID << "',";
	ss << "'" << user->m_usert.m_openid << "',";
	ss << "'" << user->m_usert.m_nike << "',";
	ss << "'" << user->m_usert.m_sex << "',";
	ss << "'" << user->m_usert.m_provice << "',";
	ss << "'" << user->m_usert.m_city << "',";
	ss << "'" << user->m_usert.m_country << "',";
	ss << "'" << user->m_usert.m_headImageUrl << "',";
	ss << "'" << user->m_usert.m_unioid << "',";
	ss << "'" << user->m_usert.m_playerType << "',";
	ss << "'" << user->m_usert.m_numOfCard1s << "',";
	ss << "'" << std::to_string(user->m_usert.m_numOfCard2s) << "',";
	ss << "'" << user->m_usert.m_numOfCard3s << "',";
	ss << "'" << user->m_usert.m_lastLoginTime<< "',";
	ss << "'" << user->m_usert.m_regTime << "',";
	ss << "'" << user->m_usert.m_new << "',";
	ss << "'" << user->m_usert.m_gm << "',";
	ss << "'" << std::to_string(user->m_usert.m_totalbuynum) << "',";
	ss << "'" << user->m_usert.m_totalplaynum << "',";
	ss << "'" << user->m_usert.m_diamondNum << "',";
	ss << "'" << user->m_usert.m_couponNum << "'";
	ss << ")";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::CreateUser sql error %s", ss.str().c_str());
		return 0;
	}

	user->m_usert.m_id = (Lint)mysql_insert_id(m);
	return user->m_usert.m_id;
}

void UserManager::AddUser(DUser* user)
{
	if (!m_userMap.count(user->m_usert.m_unioid))
	{
		m_userMap[user->m_usert.m_unioid] = user;
	}
}

void UserManager::UpdateUserIntoDB(DUser* user)
{
	if(user != NULL)
	{
		std::stringstream ss;
		ss << "UPDATE user SET ";
		ss << "Nike='"<<  user->m_usert.m_nike <<"',";
		ss << "Sex='"<<  user->m_usert.m_sex <<"',";
		ss << "Provice='"<<  user->m_usert.m_provice <<"',";
		ss << "City='"<<  user->m_usert.m_city <<"',";
		ss << "Country='"<<  user->m_usert.m_country<<"',";
		ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
		ss << "PlayerType='" << user->m_usert.m_playerType<<"',";
		ss << "NumsCard1='"<<  user->m_usert.m_numOfCard1s<<"',";
		ss << "NumsCard2='"<< std::to_string(user->m_usert.m_numOfCard2s)<<"',";
		ss << "NumsCard3='"<<  user->m_usert.m_numOfCard3s<<"',";
		ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
		ss << "Gm='" << user->m_usert.m_gm << "',";
		ss << "New='"<<  user->m_usert.m_new << "',";
		ss << "TotalCardNum='" << std::to_string(user->m_usert.m_totalbuynum) << "',";
		ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<<"',";
		ss << "DiamondNum='" << user->m_usert.m_diamondNum<<"',";
		ss << "CouponNum='"<< user->m_usert.m_couponNum<<"',";
		ss << "Gold='"<< user->m_usert.m_Gold<<"',";
		ss << "NewGolduser='"<< user->m_usert.m_NewGolduser<<"',";
		ss << "RemainingGifts='"<< user->m_usert.m_RemainingGifts<<"',";
		ss << "LastGiftTime='"<< user->m_usert.m_LastGiftTime<<"'";
		ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

		LLOG_DEBUG("UserManager::UpdateUserIntoDB sql =%s", ss.str().c_str());
		gDbServer.Push(move(ss.str()), user->m_usert.m_id);
		UpdateTeamMember(user->m_usert.m_id, user->m_usert.m_onUseTeamId, user->m_usert.m_numOfCard1s, user->m_usert.m_numOfCard2s, user->m_usert.m_numOfCard3s, user->m_usert.m_totalbuynum, user->m_usert.m_totalplaynum);
	}
}

void UserManager::UpdateUserCoupon(const Lstring& unionId, Lint couponChanged, Lint changedReason)
{
	DUser* user = gUserManager.GetUserByOpenId(unionId);
	if (user != NULL)
	{
		user->m_usert.m_couponNum += couponChanged;
		SaveUserCoupon(user);
		InsertCouponLogToDB(user, couponChanged, changedReason);
	}
	else
		LLOG_ERROR("Work::HanderLogicManagerCouponChanged:user %s does not exist.", unionId.c_str());
}

void UserManager::SaveUserCoupon(DUser* user)
{
	std::stringstream ss;
	ss << "UPDATE user SET ";
	ss << "couponNum=" << user->m_usert.m_couponNum;
	ss << " WHERE Id=";
	ss << user->m_usert.m_id;

	LLOG_DEBUG("UserManager::SaveUserCoupon sql =%s", ss.str().c_str());

	gDbServer.Push(move(ss.str()), user->m_usert.m_id);
}

void UserManager::InsertCouponLogToDB(DUser* user, Lint couponChanged, Lint changedReason)
{
	std::stringstream ss;
	ss << "INSERT INTO coupondetails (Time, CouponNum, Reason, UserId) VALUES(";
	ss << gWork.GetCurTime().Secs() << ",";
	ss << couponChanged << ",";
	ss << changedReason << ",";
	ss << user->m_usert.m_id << ")";

	LLOG_DEBUG("UserManager::InsertCouponLogToDB sql =%s", ss.str().c_str());

	gDbServer.Push(move(ss.str()), user->m_usert.m_id);
}

void UserManager::SaveUserExcepteSomeInfo(DUser* user)
{
	if(user != NULL)
	{
		std::stringstream ss;
		ss << "UPDATE user SET ";
		ss << "Nike='"<<  user->m_usert.m_nike <<"',";
		ss << "Sex='"<<  user->m_usert.m_sex <<"',";
		ss << "Provice='"<<  user->m_usert.m_provice <<"',";
		ss << "City='"<<  user->m_usert.m_city <<"',";
		ss << "Country='"<<  user->m_usert.m_country<<"',";
		ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
		ss << "PlayerType='" << user->m_usert.m_playerType<<"',";
		ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
		ss << "New='"<<  user->m_usert.m_new << "',";
		ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<<"',";
		ss << "CouponNum='"<< user->m_usert.m_couponNum<<"'";
		ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

		LLOG_DEBUG("UserManager::UpdateUserIntoDB sql =%s", ss.str().c_str());

		gDbServer.Push(move(ss.str()), user->m_usert.m_id);
	}
}

void UserManager::SaveUserGold(DUser* user)
{
	if(user != NULL)
	{
		std::stringstream ss;
		ss << "UPDATE user SET ";
		ss << "Nike='"<<  user->m_usert.m_nike <<"',";
		ss << "Sex='"<<  user->m_usert.m_sex <<"',";
		ss << "Provice='"<<  user->m_usert.m_provice <<"',";
		ss << "City='"<<  user->m_usert.m_city <<"',";
		ss << "Country='"<<  user->m_usert.m_country<<"',";
		ss << "HeadImageUrl='"<<  user->m_usert.m_headImageUrl<<"',";
		ss << "PlayerType='" << user->m_usert.m_playerType<<"',";
		ss << "LastLoginTime='"<<  user->m_usert.m_lastLoginTime<<"',";
		ss << "New='"<<  user->m_usert.m_new << "',";
		ss << "TotalPlayNum='"<<  user->m_usert.m_totalplaynum<<"',";
		ss << "CouponNum='"<< user->m_usert.m_couponNum<<"',";
		ss << "Gold='"<<  user->m_usert.m_Gold<<"',";
		ss << "NewGolduser='"<< user->m_usert.m_NewGolduser<<"',";
		ss << "RemainingGifts='"<< user->m_usert.m_RemainingGifts<<"',";
		ss << "LastGiftTime='"<< user->m_usert.m_LastGiftTime<<"'";
		ss << " WHERE UnionId='" << user->m_usert.m_unioid << "'";

		LLOG_DEBUG("UserManager::UpdateUserIntoDB sql =%s", ss.str().c_str());

		gDbServer.Push(move(ss.str()), user->m_usert.m_id);
	}
}

//randid,begin
Lint UserManager::GetRandInsertIDFromDB()
{
	MYSQL* m = gWork.GetDbSession().GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("UserManager::GetRandInsertIDFromDB mysql null");
		return 0;
	}
	////////////////////////////////////////////////////////
	Lint nid = 0;
	Lint nInsertID = 0;
	static Lstring  SQLSelect = "select Id, UserId from user_id_table order by Id  limit 1";
	if (mysql_real_query(m, SQLSelect.c_str(), SQLSelect.size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB1  error: %s  Sql: %s", mysql_error(m), SQLSelect.c_str());
		return 0;
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("GetRandInsertIDFromDB2  error: %s ", mysql_error(m));
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("GetRandInsertIDFromDB3  user_id_table Empty!");
		return 0;
	}
	nid = atoi(*row++);
	nInsertID = atoi(*row);
	mysql_free_result(res);
	std::stringstream ss;
	ss << "delete from user_id_table where Id = " << nid;
	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("GetRandInsertIDFromDB4 sql error: %s  Sql: %s", mysql_error(m), ss.str().c_str());
		return 0;
	}
	return nInsertID;
}
//randid,end


void UserManager::UpdateTeamMember(Lint userId, Lint teamId, Lint numOfCard1s, Ldouble numOfCard2s, Lint numOfCard3s, Ldouble totalbuynum, Lint totalplaynum)
{
	MYSQL* m = gWork.GetDbSession().GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("UserManager::UpdateTeamMember mysql null");
		return;
	}

	//查询数据库
	std::stringstream ss;
	ss << "UPDATE team_member SET ";
	ss << "NumsCard1='" << numOfCard1s << "',";
	ss << "NumsCard2='" << std::to_string(numOfCard2s) << "',";
	ss << "NumsCard3='" << numOfCard3s << "',";
	ss << "TotalCardNum='" << std::to_string(totalbuynum) << "',";
	ss << "TotalPlayNum='" << totalplaynum << "'";
	ss << " WHERE UserID='" << userId << "' and TeamID='" << teamId << "'";

	if (mysql_real_query(m, ss.str().c_str(), ss.str().size()))
	{
		LLOG_ERROR("UserManager::UpdateTeamMember  error: %s  Sql: %s", mysql_error(m), ss.str().c_str());
		return;
	}
}

bool UserManager::HaveTeamId(Lint userId, Lint teamId)
{
	MYSQL* m = gWork.GetDbSession().GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("UserManager::HaveTeamId mysql null");
		return false;
	}

	Lstring strCreateImagerURL;
	strCreateImagerURL = "";
	std::stringstream strStream;
	strStream << "select TeamID from team_member where TeamID=" << teamId << " and UserID=" << userId << " and TeamState=2" << ";";
	if (mysql_real_query(m, strStream.str().c_str(), strStream.str().size()))
	{
		LLOG_ERROR("UserManager::HaveTeamId  error: %s  Sql: %s", mysql_error(m), strStream.str().c_str());
		return false;
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("UserManager::HaveTeamId  error: %s ", mysql_error(m));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("UserManager::HaveTeamId  user Empty!");
		return false;
	}

	Lint TeamID = atoi(*row++);
	mysql_free_result(res);

	return TeamID == teamId;
}

bool UserManager::GetTeamMember(Lint userId, Lint teamId, Lint& numOfCard1s, Ldouble& numOfCard2s, Lint& numOfCard3s, Ldouble& totalbuynum, Lint& totalplaynum)
{
	MYSQL* m = gWork.GetDbSession().GetMysql();
	if (m == NULL)
	{
		LLOG_ERROR("UserManager::HaveTeamId mysql null");
		return false;
	}

	Lstring strCreateImagerURL;
	strCreateImagerURL = "";
	std::stringstream strStream;
	strStream << "select TeamID,NumsCard1,NumsCard2,NumsCard3,TotalCardNum,TotalPlayNum from team_member where TeamID=" << teamId << " and UserID=" << userId << " and TeamState=2" << ";";
	if (mysql_real_query(m, strStream.str().c_str(), strStream.str().size()))
	{
		LLOG_ERROR("UserManager::HaveTeamId  error: %s  Sql: %s", mysql_error(m), strStream.str().c_str());
		return false;
	}

	MYSQL_RES* res = mysql_store_result(m);
	if (!res)
	{
		LLOG_ERROR("UserManager::HaveTeamId  error: %s ", mysql_error(m));
		return false;
	}

	MYSQL_ROW row = mysql_fetch_row(res);
	if (!row)
	{
		mysql_free_result(res);
		LLOG_ERROR("UserManager::HaveTeamId  user Empty!");
		return false;
	}

	Lint TeamID = atoi(*row++);
	numOfCard1s = atoi(*row++);
	numOfCard2s = atof(*row++);
	numOfCard3s = atoi(*row++);
	totalbuynum = atof(*row++);
	totalplaynum = atoi(*row++);
	mysql_free_result(res);

	return TeamID == teamId;
}
