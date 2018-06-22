#ifndef __HANDER_HTTP_H__
#define __HANDER_HTTP_H__
// 和后台通信的接口都在这里
#include "LSingleton.h"
#include "LSocket.h"

class CHanderHttp : public LSingleton<CHanderHttp>
{
	typedef std::map<Lstring, Lstring>	HttpParamMap;
public:
	CHanderHttp();
	virtual ~CHanderHttp();

	virtual	bool Init();
	virtual	bool Final();

	void			SendRet(const Lstring& errorInfo, LSocketPtr send);
	void			SendHttpRet(Lint nErrorCode, LSocketPtr send);

	void			HanderHttpCreatTeam(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpModifyTeam(HttpParamMap& param, LSocketPtr sp);

	void			HanderHttpQueryUserInfos(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpQueryExchageList(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpOptExchage(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpOtherJoinTeam(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpCardAlert(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpMarkLog(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpPerDayLimit(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpDelTeam(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpAddMembers(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpAddMember(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpJoinTeam(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpQueryUserInfo(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpQueryTeamList(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpQueryTeamInfo(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpQueryMemberList(HttpParamMap& param, LSocketPtr sp);
	
	void			HanderHttpQueryMemberCount(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpQueryLogList(HttpParamMap& param, LSocketPtr sp);
	void			HanderHttpQueryWinnum(HttpParamMap& param, LSocketPtr sp);

	Lstring			GetHttpParamString(HttpParamMap& param, const Lstring& strKey);
	Lint			GetHttpParamLint(HttpParamMap& param, const Lstring& strKey);
	Lint			GetHttpParamLlong(HttpParamMap& param, const Lstring& strKey);
	bool			IsHaverParam(HttpParamMap& param, const Lstring& strKey);
protected:
	bool			IsUserExist(Lint nUserID,Lstring& strNike, Lstring& strHeadImageUrl);
	std::vector<std::string>	mySplit(std::string str, std::string s);
	std::vector<int>			mySplit_int(std::string str, std::string s);

private:
	void			ProcessModifyTeam_all_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp);
	void			ProcessModifyTeam_playTypes_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp);
	void			ProcesskModifyTeam_deskCntLimit_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp);
	void			ProcessModifyTeam_boardInfo_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp);
	void			ProcessModifyTeam_teamName_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp);
	void			ProcessModifyTeam_playTypes_deskCntLimit_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp);
	void			ProcessModifyTeam_deskProportionate_(Lint createrId, Lint teamId, HttpParamMap& param, LSocketPtr sp);
};

#define gHanderHttp CHanderHttp::Instance()

#endif
