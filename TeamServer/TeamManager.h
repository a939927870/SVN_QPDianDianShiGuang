#ifndef __TEAM_MANAGER_H__
#define __TEAM_MANAGER_H__
#include "LBase.h"
#include "TeamInfo.h"
#include "LSingleton.h"
#include "LMsg.h"
#include "LMsgCT.h"
#include <map>
#include <vector>

typedef std::vector<SExchageGoldOptLog *>	tC_SExchageGoldOptLogVec;
typedef std::map<Lint, tC_SExchageGoldOptLogVec>	tC_SExchageGoldOptLogVecMap;
typedef std::map<Llong, SExchageGoldOptLog *>	tC_SExchageGoldOptLogMap;
class CTeamManager : public LSingleton<CTeamManager>
{
	
public:
	CTeamManager();
	virtual ~CTeamManager();

	virtual bool Init();
	virtual bool Final();

	Lint	CreateTeam(Lint nCreaterID, Lint nTeamId, const Lstring& strTeamName, const std::vector<Lint>& vPlayTypes, const Lstring& playTypes, Lint manageId,Lint countyId, Lint deskCntLimit, Lint deskProportionate);
	void	ModifyTeamData(LSocketPtr sp, Lint type, Lint createrId, Lint teamId, std::vector<Lint>& playTypes, Lstring& boardInfo, Lstring& strTeamName, Lint deskCntLimit, Lint deskProportionate);

	Lint	AddTeamCard(Lint nTeamID, Lint nCards, Lint nServerID);

	const STeamInfo& FindTeam(Lint nTeamID);
	const STeamMember& FindMember(Lint nTeamID, Lint nUserID);

	void	GetCreateTeamID(Lint nCreaterID, std::list<Lint>& tTeamIdVec);

	// 返回指定的玩家创建了多少个工会 [2017-7-14 willing]
	Lint	GetTeamCount(Lint nCreateID);

	// 返回指定的玩家已经加入了几个工会 [2017-7-21 willing]
	Lint	GetJoinCount(Lint nUserID);

	// 判断指定的玩家是否加入了指定的工会 [2017-7-14 willing]
	bool	IsMember(Lint nTeamID, Lint nUserID);
	bool	IsRealMember(Lint nTeamID, Lint nUserID);
	// 获取指定的玩家在工会中的状态 [2017-7-19 willing]
	Lint	GetMemStateInTeam(Lint nTeamID, Lint nUserID);

	// 用于修改在线，离线状态 [2017-7-20 willing]
	void	ModifyMemGameState(Lint nUserID, Lint nGameState);

	// 会长对成员的操作 [2017-7-20 willing]
	Lint	TeamManagerOptMem(Lint nCreaterID, Lint nTeamID, Lstring strUserIDs, Lint nOpt, std::vector<std::string>& errorUserList);

	Lint	GetTeamMemCount(Lint nTeamID);

	Lint	AddMemToTeam(const STeamMember& teamMember, bool bNeedSave = false);
	Lint	AddTeamPlayConfig(STeamPlayConfig& teamConfig, bool bNeedSave = false);
	Lint	AddTeamExchageOpt(SExchageGoldOptLog* teamConfig, bool bNeedSave = false);
	SExchageGoldOptLog * GetTeamExchageOpt(Llong Id);
	tC_SExchageGoldOptLogVec& GetTeamExchageOptByToUserId(Lint userId);

	Lint	HanderUserFastPlay(Lint teamId, Lint userId, Llong nUserUnique);
	// 玩家请求 工会列表 [2017-7-20 willing]
	void	SendTeamList(Llong nUserUnique, Lint nState);

	// 返回成员列表 [2017-7-21 willing]
	void	SendMemberList(Lint nTeamID, Llong nUserUnique);

	// 取消加入工会 [2017-7-21 willing]
	void	CancelJoin(Lint nTeamID, Llong nUserUnique);

	// 修改工会的最低房卡警报值 [2017-7-22 willing]
	Lint	SetMinCardAlert(Lint nCreaterID, Lint nTeamID, Lint nAlert);

	// 设置工会成员每日房卡最大消耗数量 [2017-7-22 willing]
	Lint	SetPerDayLimit(Lint nCreaterID, Lint nTeamID, Lint nLimit);

	// 申请离开工会 [2017-7-22 willing]
	void	RequestLeaveTeam(Lint nTeamID, Llong nUserUnique);

	// 删除工会 [2017-7-22 willing]
	Lint	DelTeam(Lint nCreaterID, Lint nTeamID);
	//	检查加入房间的玩家是否合格
	Lint	CheckCanAddRoom(Lint userId, Lint deskId);
	// 检测是否可以创建房间 [2017-7-23 willing]
	Lint	CheckCanCreateRoom(Lint nTeamID, Lint nUserID, Lint nFlag);

	//检查是否可以登陆，如果有房间不上这个俱乐部是不让登陆的
	Lint    CheckCanLogin(Lint nUserID, Lint nOnUseTeamId, Lint& nTeamId);

	// 预扣房卡 [2017-7-23 willing]
	void	YuKouCard(Lint nTeamID, Lint nUserID, Lint nFlag,bool isFast = false);

	// 广播房卡改变 [2017-7-23 willing]
	void	BroadcastTeamInfoChange(Lint nTeamID);

	// 通知 退还预扣的卡 [2017-7-23 willing]
	void	GivebackCard(Lint nTeamID, Lint nUserID, Lint nFlag,Lint nFast);

	// 实现真正的扣卡 [2017-7-23 willing]
	void	RealDelCard(Lint nTeamID,Lint nUserID, Lint nFlag);

	// 发送战绩信息 [2017-7-24 willing]
	void	SendVideoLog(bool admin,Lint nTeamID, Lint nUserID, time_t nBeginTimestamp, time_t nEndTimestamp,Llong nUserUnique);

	// 获取 工会成员，当天已经使用了多少工会房卡 [2017-7-26 willing]
	Lint	GetMemTodayUserCard(Lint nTeamID, Lint nUserID);

	// 设置玩家的头像和昵称 [2017-7-26 willing]
	void	SetUserInfo(Lint nUserID, Lstring strNick, Lstring strHeadImageUrl);

	// 在工会中广播指定的消息 [2017-8-2 willing]
	void	BroadcastTeamDeskMsg(Lint nTeamID, LMsg& msg);

	// 订阅/取消订阅桌子信息 [2017-8-3 willing]
	void	SetCareForDesk(Lint nTeamID, Lint nUserID, Lint nCareFor);

	// 玩家查询对工会的日志 [2017-8-8 willing]
	void	SendTeamOptLog(Llong nUserUnique);

	// 记录玩家对工会的操作日志 [2017-8-9 willing]
	void	SaveTameMemOptLog(Lint nTeamID, Lint nUserID, Lint nOptType);

	void	TeamManagerOptMem(LMsgC2SOptMember* msg, Llong nUserUnique);
	void	HanderOptTeamInfo(LMsgC2SOptTeamInfo* msg, Lint nUserID, Llong nUserUnique);
	Lint	GetAdminIdbyTeamID(Lint nTeamID);
	void	HanderRestDeskwithPerLimitUse(Lint teamID, Lint userID, Lint flag,bool is_fast = false);
	void	HanderTeamAdminInviteOther(LMsgC2SAdminInviteOther* msg, Llong nUserUnique);
	bool	IsUserExist(Lint nUserID, Lstring& strNike, Lstring& strHeadImageUrl);
	bool	SendAllExchageGoldOptLog(Lint userID);
	bool	SendInviteInRoom(Lint userId, Lint memberId);
	Lstring	GetCreateHeadImageURL(Lint nCreaterID, Lstring& nike);

	Lint	GetRandInsertIDFromDB();
	void    GetAllTeamMember(Lint nTeamID, std::vector<STeamMember> & members);

protected:
	void	LoadAllTeamInfo();
	void	LoadAllTeamMember();
	void	LoadAllTeamPlayConfig();
	void	LoadAllTeamExchargeOpt();
	
	// 删除一个 玩家已经加入的工会ID [2017-7-22 willing]
	void	RmvJoinTeamID(Lint nUserID, Lint nTeamID);

	std::vector<std::string> split(std::string str, std::string s);
	std::vector<int>			mySplit_int(std::string str, std::string s);
	//// 根据桌子最大人数，返回需要扣除的房卡数 [2017-7-23 willing]
	//Lint	GetCardCount(int nMaxDeskUserCount);

public:
	tIntListMap			m_tUserJoinTeamMap;			// 记录玩家加入的工会ID 
	//tTeamPlayConfigMap	m_tTeamPlayConfigMap;		//工会快速组局 房间配置
private:
	tTeamInfoMap		m_tTeamInfoMap;				// 所有工会信息 [2017-7-14 willing]
	tIntListMap			m_tUserCreatTeamMap;		// 记录玩家加创建的工会ID [2017-7-14 willing]
	//tIntListMap			m_tUserJoinTeamMap;			// 记录玩家加入的工会ID [2017-7-14 willing]
	tStringIntMap		m_tTeamNameIDMap;			// 保存工会名称，工会ID的对应关系，用来判重使用 [2017-7-25 willing]
	
	tC_SExchageGoldOptLogVecMap m_tFromSExchageGoldOptLog;
	tC_SExchageGoldOptLogVecMap m_tToSExchageGoldOptLog;
	tC_SExchageGoldOptLogMap m_tIdSExchageGoldOptLog;
};

#define gTeamManager CTeamManager::Instance()

#endif